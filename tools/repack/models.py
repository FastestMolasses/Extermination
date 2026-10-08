"""Same-topology glTF views of bounded native VIF model packets.

The native template is authoritative. No packet, material, animation, palette,
or allocation is rebuilt. Only existing float attributes and rigid joint bits
can change; duplicate tail records are updated together. Runtime palette poses
are external, so the glTF skin uses identity nodes in native bone-local units.
"""
from __future__ import annotations

import hashlib
import base64
import copy
import json
import math
import os
from pathlib import Path
import struct
import tempfile
from urllib.parse import unquote, urlsplit

from .archive import safe_output

SCHEMA = "extermination-model-v1"
SIGNATURE = bytes.fromhex("040400010080806c")
STRIDE = 0x820
TEX0_MASK = (1 << 61) - 1


def _hash(raw):
    return hashlib.sha256(raw).hexdigest()


def _layout(raw: bytes, kind="auto") -> dict:
    if kind not in ("auto", "skinned", "static"):
        raise ValueError("model kind must be auto, skinned or static")
    is_blob = len(raw) >= 0x50 and raw[0x48:0x50] == SIGNATURE
    if kind == "auto":
        if not is_blob:
            raise ValueError("auto accepts framed actor blobs; use static explicitly for level colour packets")
        kind = "skinned"
    nodes, starts = 0, []
    if kind == "skinned":
        if not is_blob:
            raise ValueError("skinned input needs a framed actor blob with a declared node count")
        count, qwc, nodes, size = struct.unpack_from("<4I", raw)
        if (not 0 < count < 4096 or not 1 <= nodes <= 128
                or qwc != count * 130 or size != 64 + count * STRIDE or size > len(raw)):
            raise ValueError("unsupported or truncated actor packet count/size/node table")
        starts = [0x50 + i * STRIDE for i in range(count)]
        if any(raw[start - 8:start] != SIGNATURE for start in starts):
            raise ValueError("actor packet framing changed or unsupported UNPACK")
    else:
        # Explicit static mode means the caller has identified a colour-kernel
        # stream. The exact VIF pair fixes payload width, count and cycle mode.
        cursor = 0
        while (at := raw.find(SIGNATURE, cursor)) >= 0:
            cursor = at + len(SIGNATURE)
            if at % 16 != 8:
                continue
            if at + 8 + 0x800 > len(raw):
                raise ValueError("truncated static UNPACK packet")
            starts.append(at + 8)
        if not starts or any(b < a + 0x800 for a, b in zip(starts, starts[1:])):
            raise ValueError("no disjoint bounded static packets")
    vertices, packets, materials = [], [], []
    material_ids, indices = {}, {}
    bounds = {}
    for start in starts:
        offsets = [start + 64 * i for i in range(32)]
        tail = []
        while len(offsets) > 1 and raw[offsets[-1]:offsets[-1] + 64] == raw[offsets[-2]:offsets[-2] + 64]:
            tail.append(offsets.pop())
        packet = []
        for offset in offsets:
            uv = struct.unpack_from("<2f", raw, offset + 16)
            attr = struct.unpack_from("<4f", raw, offset + 32)
            position = struct.unpack_from("<3f", raw, offset + 48)
            wbits = struct.unpack_from("<I", raw, offset + 60)[0]
            w = struct.unpack_from("<f", raw, offset + 60)[0]
            if (not all(math.isfinite(value) for value in (*uv, *attr, *position, w))
                    or abs(abs(w) - 1) > .05 or raw[offset + 8:offset + 16] != bytes(8)):
                raise ValueError("unsupported vertex record or nonfinite geometry")
            joint = (wbits & 1023) >> 3 if nodes else 0
            if nodes and (wbits & 7 or joint >= nodes):
                raise ValueError("joint address exceeds the existing palette")
            if kind == "skinned":
                if attr[3] != 0 or abs(sum(value * value for value in attr[:3]) - 1) > .001:
                    raise ValueError("actor normal is not an existing unit normal")
            elif attr[3] != 1 or any(not 0 <= value <= 1 for value in attr[:3]):
                raise ValueError("static packet is not an authored colour record")
            key = str(joint) if nodes else f"packet{len(packets)}"
            bound = bounds.setdefault(key, [list(position), list(position)])
            for axis in range(3):
                bound[0][axis] = min(bound[0][axis], position[axis])
                bound[1][axis] = max(bound[1][axis], position[axis])
            vertex = len(vertices)
            packet.append(vertex)
            vertices.append(dict(offset=offset, aliases=sorted(tail) if offset == offsets[-1] else [], joint=joint))
            tex0 = struct.unpack_from("<Q", raw, offset)[0] & TEX0_MASK
            if tex0 not in material_ids:
                material_ids[tex0] = len(materials)
                materials.append(tex0)
                indices[material_ids[tex0]] = []
            # The GS kick vertex chooses the texture and ADC/parity. Keep all
            # original triangles, including degenerate stitching triangles.
            if len(packet) >= 3 and not wbits & 0x8000:
                a, b, c = packet[-3:]
                if wbits & 0x4000:
                    a, b = b, a
                indices[material_ids[tex0]].extend((a, b, c))
        packets.append(dict(offset=start, vertices=packet))
    if not vertices or not any(indices.values()):
        raise ValueError("no drawable triangles in supported packets")
    return dict(kind=kind, nodes=nodes, vertices=vertices, packets=packets,
                materials=materials, indices={str(k): v for k, v in indices.items() if v}, bounds=bounds)


def _document(raw: bytes, layout: dict) -> tuple[dict, bytes]:
    data = bytearray()
    doc = dict(asset={"version": "2.0", "generator": "Extermination same-topology model editor"},
               buffers=[], bufferViews=[], accessors=[], meshes=[], materials=[], nodes=[],
               scenes=[{"nodes": [0]}], scene=0,
               extras={"nativeUnits": True, "pose": "identity palette; native animation remains external"})

    def accessor(values, components, component_type=5126, target=34962):
        code, width = {5126: ("f", 4), 5125: ("I", 4), 5123: ("H", 2)}[component_type]
        while len(data) % 4:
            data.append(0)
        offset = len(data)
        flat = [item for row in values for item in row]
        data.extend(struct.pack("<" + code * len(flat), *flat))
        view = len(doc["bufferViews"])
        doc["bufferViews"].append(dict(buffer=0, byteOffset=offset, byteLength=len(flat) * width, target=target))
        result = dict(bufferView=view, componentType=component_type, count=len(values),
                      type={1: "SCALAR", 2: "VEC2", 3: "VEC3", 4: "VEC4"}[components])
        index = len(doc["accessors"])
        doc["accessors"].append(result)
        return index

    records = layout["vertices"]
    positions = [struct.unpack_from("<3f", raw, row["offset"] + 48) for row in records]
    uv = [struct.unpack_from("<2f", raw, row["offset"] + 16) for row in records]
    attribute = [struct.unpack_from("<3f", raw, row["offset"] + 32) for row in records]
    attrs = dict(POSITION=accessor(positions, 3), TEXCOORD_0=accessor(uv, 2))
    doc["accessors"][attrs["POSITION"]].update(min=[min(v[k] for v in positions) for k in range(3)],
                                               max=[max(v[k] for v in positions) for k in range(3)])
    attrs["NORMAL" if layout["nodes"] else "COLOR_0"] = accessor(attribute, 3)
    # glTF 2.0 forbids UNSIGNED_INT vertex attributes. Two ushort halves
    # retain exact native byte offsets without float rounding or a uint32 attr.
    attrs["_NATIVE_ID"] = accessor([(v["offset"] & 65535, v["offset"] >> 16) for v in records], 2, 5123)
    if layout["nodes"]:
        attrs["JOINTS_0"] = accessor([(v["joint"], 0, 0, 0) for v in records], 4, 5123)
        attrs["WEIGHTS_0"] = accessor([(1, 0, 0, 0) for _ in records], 4)
    primitives = []
    for material, triangles in layout["indices"].items():
        primitives.append(dict(attributes=attrs.copy(), indices=accessor([(i,) for i in triangles], 1, 5125, 34963),
                               material=int(material), mode=4))
    doc["meshes"] = [dict(name="native_packet_mesh", primitives=primitives)]
    doc["materials"] = [dict(name=f"TEX0_{value:016x}",
                             pbrMetallicRoughness={"baseColorFactor": [1, 1, 1, 1], "metallicFactor": 0, "roughnessFactor": 1},
                             extras={"nativeTEX0": f"0x{value:016x}"}) for value in layout["materials"]]
    doc["nodes"] = [dict(name="native_mesh", mesh=0)]
    if layout["nodes"]:
        doc["nodes"][0]["skin"] = 0
        joints = list(range(2, layout["nodes"] + 2))
        doc["nodes"].append(dict(name="native_palette_root", children=joints))
        doc["nodes"].extend(dict(name=f"native_palette_{index:03}") for index in range(layout["nodes"]))
        doc["skins"] = [dict(joints=joints, skeleton=1, name="existing_rigid_palette")]
        doc["scenes"][0]["nodes"].append(1)
    doc["buffers"] = [dict(uri="model.bin", byteLength=len(data))]
    return doc, bytes(data)


def _input(tree, name):
    path = tree / name
    if not path.is_file() or not path.resolve().is_relative_to(tree.resolve()):
        raise ValueError("model input is absent or escapes its tree")
    return path


def _output(path, inputs):
    result = safe_output(path)
    for source in inputs:
        if source.exists() and (result == source.resolve() or result.exists() and result.samefile(source)):
            raise ValueError("model output aliases an input")
    return result


def unpack_model(source: Path, out_dir: Path, *, kind="auto") -> dict:
    source, out = Path(source).resolve(), safe_output(out_dir)
    if out.exists() and (not out.is_dir() or any(out.iterdir())):
        raise ValueError("model unpack destination must be new or empty")
    if source.is_relative_to(out):
        raise ValueError("model unpack destination contains its source")
    raw = source.read_bytes()
    layout = _layout(raw, kind)
    doc, buffer = _document(raw, layout)
    manifest = dict(schema=SCHEMA, source_path=str(source), source_sha256=_hash(raw), source_size=len(raw),
                    layout=layout, vertex_count=len(layout["vertices"]), packet_count=len(layout["packets"]))
    out.mkdir(parents=True, exist_ok=True)
    (out / "original.bin").write_bytes(raw)
    (out / "model.bin").write_bytes(buffer)
    (out / "model.gltf").write_text(json.dumps(doc, indent=2) + "\n")
    (out / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest


def _import(doc, expected, tree, original_buffer):
    """Validate native semantics independently of glTF buffer/accessor packing."""
    buffers, inputs = [], []
    if doc.get("asset", {}).get("version") != "2.0":
        raise ValueError("expected glTF 2.0")
    for descriptor in doc.get("buffers", []):
        uri = descriptor.get("uri", "")
        if uri.startswith(("data:application/octet-stream;base64,", "data:application/gltf-buffer;base64,")):
            try:
                raw = base64.b64decode(uri.split(",", 1)[1], validate=True)
            except ValueError as error:
                raise ValueError("invalid embedded glTF buffer") from error
        else:
            parsed = urlsplit(uri)
            if parsed.scheme or parsed.netloc or parsed.query or parsed.fragment or not parsed.path:
                raise ValueError("glTF buffers must be local files or embedded base64")
            path = _input(tree, unquote(parsed.path))
            inputs.append(path)
            raw = path.read_bytes()
        if len(raw) != descriptor.get("byteLength"):
            raise ValueError("glTF buffer size changed or is invalid")
        buffers.append(raw)

    def read(document, payloads, index, types, components, count):
        try:
            if type(index) is not int or index < 0:
                raise ValueError("invalid glTF accessor index")
            accessor = document["accessors"][index]
            component = accessor["componentType"]
            if (accessor.get("sparse") is not None or component not in types
                    or accessor["type"] != components or accessor["count"] != count):
                raise ValueError("glTF vertex count, accessor type or sparse storage is unsupported")
            view_index = accessor["bufferView"]
            if type(view_index) is not int or view_index < 0:
                raise ValueError("invalid glTF buffer view index")
            view = document["bufferViews"][view_index]
            if type(view["buffer"]) is not int or view["buffer"] < 0:
                raise ValueError("invalid glTF buffer index")
            payload = payloads[view["buffer"]]
            n = {"SCALAR": 1, "VEC2": 2, "VEC3": 3, "VEC4": 4, "MAT4": 16}[components]
            code, width = {5126: ("f", 4), 5125: ("I", 4), 5123: ("H", 2), 5121: ("B", 1)}[component]
            size = n * width
            stride = view.get("byteStride", size)
            start, offset, length = view.get("byteOffset", 0), accessor.get("byteOffset", 0), view["byteLength"]
            if (any(type(value) is not int or value < 0 for value in (start, offset, length, stride))
                    or stride < size or stride % width or start % width or offset % width
                    or start + length > len(payload) or offset + (count - 1) * stride + size > length):
                raise ValueError("glTF accessor exceeds its buffer view")
            normalize = accessor.get("normalized", False)
            if type(normalize) is not bool or normalize and component not in (5121, 5123):
                raise ValueError("unsupported glTF normalization")
            rows = [struct.unpack_from("<" + code * n, payload, start + offset + i * stride) for i in range(count)]
            if normalize:
                divisor = 255 if component == 5121 else 65535
                rows = [tuple(value / divisor for value in row) for row in rows]
            return rows
        except (KeyError, IndexError, TypeError, struct.error) as error:
            raise ValueError("malformed glTF accessor or buffer reference") from error

    canonical, baseline = copy.deepcopy(doc), copy.deepcopy(expected)
    values = None
    try:
        primitives = canonical["meshes"][0]["primitives"]
        old_primitives = baseline["meshes"][0]["primitives"]
        if len(canonical["meshes"]) != 1 or len(primitives) != len(old_primitives):
            raise ValueError("glTF mesh or primitive structure changed")
        for primitive, old in zip(primitives, old_primitives):
            if set(primitive["attributes"]) != set(old["attributes"]):
                raise ValueError("glTF attributes or native vertex identities changed")
            current = {}
            for name, accessor in primitive["attributes"].items():
                old_accessor = baseline["accessors"][old["attributes"][name]]
                if name in ("JOINTS_0", "_NATIVE_ID") and doc["accessors"][accessor].get("normalized", False):
                    raise ValueError("native identities and joints cannot be normalized")
                types = ((5121, 5123) if name == "JOINTS_0" else (5123, 5126) if name == "_NATIVE_ID"
                         else (5121, 5123, 5126) if name in ("WEIGHTS_0", "COLOR_0", "TEXCOORD_0") else (5126,))
                imported_accessor = doc["accessors"][accessor]
                if (name in ("WEIGHTS_0", "COLOR_0", "TEXCOORD_0")
                        and imported_accessor["componentType"] != 5126 and not imported_accessor.get("normalized", False)):
                    raise ValueError("integer glTF colours, weights and UVs must be normalized")
                shape = "VEC4" if name == "COLOR_0" and imported_accessor["type"] == "VEC4" else old_accessor["type"]
                rows = read(doc, buffers, accessor, types, shape, old_accessor["count"])
                if name == "COLOR_0" and shape == "VEC4":
                    if any(row[3] != 1 for row in rows):
                        raise ValueError("native colour alpha is fixed at one")
                    rows = [row[:3] for row in rows]
                if name in ("_NATIVE_ID", "WEIGHTS_0"):
                    original = read(expected, [original_buffer], old["attributes"][name], types,
                                    old_accessor["type"], old_accessor["count"])
                    if rows != original:
                        raise ValueError("glTF native vertex identities or rigid weights changed")
                else:
                    current[name] = rows
            if values is not None and current != values:
                raise ValueError("shared native vertices have conflicting glTF edits")
            values = current
            old_index = baseline["accessors"][old["indices"]]
            if doc["accessors"][primitive["indices"]].get("normalized", False):
                raise ValueError("triangle indices cannot be normalized")
            indices = read(doc, buffers, primitive["indices"], (5121, 5123, 5125), "SCALAR", old_index["count"])
            original = read(expected, [original_buffer], old["indices"], (5125,), "SCALAR", old_index["count"])
            if indices != original:
                raise ValueError("glTF triangle indices or order changed")
            primitive["attributes"], primitive["indices"] = old["attributes"], old["indices"]
        for document in (canonical, baseline):
            for node in document.get("nodes", []):
                for field, identity in (("translation", [0, 0, 0]), ("rotation", [0, 0, 0, 1]),
                                        ("scale", [1, 1, 1]), ("matrix", [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1])):
                    if node.get(field) == identity:
                        node.pop(field)
            for skin in document.get("skins", []):
                if "inverseBindMatrices" in skin:
                    matrices = read(doc, buffers, skin.pop("inverseBindMatrices"), (5126,), "MAT4", len(skin["joints"]))
                    identity = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
                    if any(matrix != identity for matrix in matrices):
                        raise ValueError("glTF skin inverse bind transforms changed")
                skin.setdefault("skeleton", 1)
            for field in ("asset", "accessors", "bufferViews", "buffers"):
                document.pop(field, None)
    except (KeyError, IndexError, TypeError) as error:
        raise ValueError("malformed glTF mesh structure") from error

    def semantic(value):
        if isinstance(value, dict):
            return {key: semantic(item) for key, item in value.items() if key not in ("name", "extras")}
        if isinstance(value, list):
            return [semantic(item) for item in value]
        return value

    if semantic(canonical) != semantic(baseline):
        raise ValueError("glTF node transforms, skin, material or document structure changed")
    return values, inputs


def pack_model(tree: Path, out_file: Path) -> dict:
    tree = Path(tree).resolve()
    inputs = [_input(tree, name) for name in ("manifest.json", "original.bin", "model.gltf")]
    manifest = json.loads(inputs[0].read_text())
    if manifest.get("schema") != SCHEMA:
        raise ValueError("unsupported model manifest")
    raw = inputs[1].read_bytes()
    if len(raw) != manifest["source_size"] or _hash(raw) != manifest["source_sha256"]:
        raise ValueError("model native template changed")
    layout = _layout(raw, manifest["layout"]["kind"])
    if layout != manifest["layout"]:
        raise ValueError("model native layout manifest changed")
    expected, original_buffer = _document(raw, layout)
    doc = json.loads(inputs[2].read_text())
    values, buffer_inputs = _import(doc, expected, tree, original_buffer)
    inputs.extend(buffer_inputs)
    output = bytearray(raw)
    edited_vertices = 0
    static_bounds = {vertex: f"packet{number}" for number, packet in enumerate(layout["packets"])
                     for vertex in packet["vertices"]} if not layout["nodes"] else {}
    for i, record in enumerate(layout["vertices"]):
        position, uv = values["POSITION"][i], values["TEXCOORD_0"][i]
        attr = values["NORMAL" if layout["nodes"] else "COLOR_0"][i]
        if not all(math.isfinite(value) for value in (*position, *uv, *attr)):
            raise ValueError("model attributes must be finite")
        joint = record["joint"]
        if layout["nodes"]:
            joints = values["JOINTS_0"][i]
            if any(joints[1:]) or joints[0] >= layout["nodes"] or str(joints[0]) not in layout["bounds"]:
                raise ValueError("joint must use an existing populated rigid palette slot")
            joint = joints[0]
            if abs(sum(value * value for value in attr) - 1) > .001:
                raise ValueError("edited normal must remain unit length")
        elif any(not 0 <= value <= 1 for value in attr):
            raise ValueError("native colours must remain in [0,1]")
        minimum, maximum = layout["bounds"][str(joint) if layout["nodes"] else static_bounds[i]]
        if any(not minimum[k] <= position[k] <= maximum[k] for k in range(3)):
            raise ValueError("position exceeds the existing joint/packet bounds; culling-bound rebuild is unsupported")
        before = raw[record["offset"]:record["offset"] + 64]
        for offset in [record["offset"], *record["aliases"]]:
            for delta, values_at in ((16, uv), (32, attr), (48, position)):
                format = "<" + "f" * len(values_at)
                if tuple(values_at) != struct.unpack_from(format, raw, offset + delta):
                    struct.pack_into(format, output, offset + delta, *values_at)
            if layout["nodes"]:
                wbits = struct.unpack_from("<I", raw, offset + 60)[0]
                struct.pack_into("<I", output, offset + 60, (wbits & ~1023) | (joint << 3))
        edited_vertices += before != output[record["offset"]:record["offset"] + 64]
    # Reparse the edited packet stream to catch any unanticipated structural
    # consequence before writing. It need not have the original local bounds.
    rebuilt = _layout(bytes(output), layout["kind"])
    record_shape = lambda item: [(v["offset"], v["aliases"]) for v in item["vertices"]]
    if record_shape(rebuilt) != record_shape(layout):
        raise ValueError("edit changes duplicated tail structure")
    original_source = Path(manifest["source_path"])
    target = _output(out_file, [*inputs, original_source])
    if target.is_relative_to(tree):
        raise ValueError("model output must be separate from its editable tree")
    target.parent.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(dir=target.parent, prefix=".model-", delete=False) as stream:
            temporary = Path(stream.name)
            stream.write(output)
        os.replace(temporary, target)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)
    return dict(unchanged=output == raw, sha256=_hash(output), size=len(output),
                edited_vertices=edited_vertices, changed_native_bytes=sum(a != b for a, b in zip(raw, output)),
                vertex_count=len(layout["vertices"]), packet_count=len(layout["packets"]))
