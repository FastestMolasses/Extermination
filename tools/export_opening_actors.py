#!/usr/bin/env python3
"""Export AREA11's opening actors from the owner's extracted disc and GS state.

Resource 0x98 is embedded in chunk15/f12_id44 at 0xd0800. Its directory
selects Dennis clip 1 and Roger clip 2. Resource 0x47 is a raw model inside
chunk15/f18_id94; equipment 0x6b uses the global chunk27 model directory.
These mappings were checked against the original opening actor list and EE
resource pointers, including a whole-model byte comparison.

Animation uses the stateful 001C8710/001C87C0 cursor operations, including
scale-key bit15's velocity reset, then 001CA0A0's UNNORMALIZED quaternion
blend and 001C6DA0's hierarchy composition. Samples are baked at 0.5 source
frame intervals. The native consumer indexes these samples without blending
matrices or subtracting world translation. Host floating-point arithmetic
is validated against original RAM with an explicitly reported tolerance;
this exporter does not claim that host and EE/VU rounding are bit identical.

All outputs are original-derived and must remain in ignored asset/build dirs.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import math
import struct
from pathlib import Path

import export_native as native
import extract_models as models

BANK_OFFSET = 0xD0800
ACTORS = (
    ("player", "chunk28/f00_id3b.bin", 0, 0xD1C1C0, 0x8102B0, 1),
    ("roger", "chunk15/f18_id94.bin", 0x35000, 0x1877740, 0x7A96E0, 2),
    ("equipment_6b", "chunk27/f01_id37.bin", 0x156080, 0xD00240, 0x7AE920, 0),
)
# Opening RAM: these class1 drawable children use callback0018A6B0. Their
# class8 controller has no mesh, which does not suppress the child draws.
PLAYER_ATTACHMENTS = (
    (47, 4, 0x7AB730), (48, 4, 0x7ABA20), (49, 4, 0x7AC000),
    (50, 4, 0x7AC2F0), (56, 4, 0x7AC5E0), (64, 4, 0x7ABD10),
    (106, 14, 0x7AB440),
)


def f32(x):
    return struct.unpack("<f", struct.pack("<f", x))[0]


def add(a, b):
    return f32(a + b)


def mul(a, b):
    return f32(a * b)


def channel_keys(data, table, bone, widths):
    offset = table + struct.unpack_from("<I", data, table + bone * 4)[0]
    result = []
    while True:
        raw = data[offset:offset + 12]
        if len(raw) != 12:
            raise ValueError("truncated animation key stream")
        flags, time = struct.unpack_from("<HH", raw, 8)
        if time == 0xFFFF:
            # The terminal key is never reached by this non-looping clip.
            result.append((time, result[-1][1], False))
            return result
        values, bit = [], 0
        for width in widths:
            values.append(models._id74_truncfloat(
                models._id74_bits(raw[:10], bit, width), width))
            bit += width
        result.append((time, tuple(values), bool(flags & 0x8000)))
        offset += 12


class Channel:
    """Original per-node cursor fields, with float stores at each operation."""

    def __init__(self, keys, quaternion=False):
        if keys[0][0] != 0 or len(keys) < 2:
            raise ValueError("opening channels must start at zero")
        self.keys = keys
        self.quaternion = quaternion
        self.index = 1  # node +66/+68/+6a: next left key index
        self.remaining = float(keys[1][0])
        self.left = keys[0][1]
        self.right = keys[1][1]
        self.blend = 0.0
        self.reciprocal = f32(1.0 / self.remaining)
        self.value = self.left
        self.velocity = tuple(mul(add(y, -x), self.reciprocal)
                              for x, y in zip(self.left, self.right))

    def advance(self, dt=0.5):
        self.remaining = add(self.remaining, -dt)
        hold = None
        if self.remaining <= 0:
            start, self.left, hold = self.keys[self.index]
            end, self.right, _ = self.keys[self.index + 1]
            duration = float(end - start)
            self.reciprocal = f32(1.0 / duration)
            overshoot = -self.remaining
            self.remaining = add(self.remaining, duration)
            if self.quaternion:
                self.blend = mul(overshoot, self.reciprocal)
            else:
                self.velocity = tuple(mul(add(y, -x), self.reciprocal)
                                      for x, y in zip(self.left, self.right))
                self.value = tuple(add(x, mul(overshoot, v))
                                   for x, v in zip(self.left, self.velocity))
            self.index += 1
        elif self.quaternion:
            self.blend = add(self.blend, mul(dt, self.reciprocal))
        else:
            self.value = tuple(add(x, mul(dt, v))
                               for x, v in zip(self.value, self.velocity))
        return hold

    def quaternion_value(self):
        t = min(self.blend, 1.0)
        inv = add(1.0, -t)
        a, b = self.left, self.right
        dot = add(add(mul(a[0], b[0]), mul(a[1], b[1])), mul(a[2], b[2]))
        dot = add(dot, mul(a[3], b[3]))
        sign = -1.0 if dot < 0 else 1.0
        return tuple(add(mul(sign * x, inv), mul(y, t)) for x, y in zip(a, b))


def quaternion_matrix(q, translation, scale):
    """001CA1C0 storage convention, followed by the evaluator's column scale."""
    x, y, z, w = q
    xx, yy, zz = mul(x, x), mul(y, y), mul(z, z)
    xy, xz, yz = mul(x, y), mul(x, z), mul(y, z)
    wx, wy, wz = mul(w, x), mul(w, y), mul(w, z)
    matrix = (
        (add(1, -mul(2, add(yy, zz))), mul(2, add(xy, -wz)), mul(2, add(xz, wy)), 0),
        (mul(2, add(xy, wz)), add(1, -mul(2, add(xx, zz))), mul(2, add(yz, -wx)), 0),
        (mul(2, add(xz, -wy)), mul(2, add(yz, wx)), add(1, -mul(2, add(xx, yy))), 0),
        (*translation, 1),
    )
    return tuple(tuple(mul(v, scale[c]) if c < 3 and r < 3 else v
                       for r, v in enumerate(col)) for c, col in enumerate(matrix))


def matrix_multiply(a, b):
    out = []
    for c in range(4):
        col = []
        for r in range(4):
            value = mul(a[0][r], b[c][0])
            for k in range(1, 4):
                value = add(value, mul(a[k][r], b[c][k]))
            col.append(value)
        out.append(tuple(col))
    return tuple(out)


class OpeningClip:
    def __init__(self, data, header):
        self.bones, self.length = struct.unpack_from("<HH", data, header)
        self.parents = list(struct.unpack_from(f"<{self.bones}i", data, header + 0x20))
        offsets = struct.unpack_from("<III", data, header + 8)
        self.rotation, self.translation, self.scale = [], [], []
        for i in range(self.bones):
            if self.parents[i] >= i:
                raise ValueError("opening hierarchy is not parent-first")
            self.rotation.append(Channel(channel_keys(data, header + offsets[0], i,
                                                       models.ID74_ROT_WIDTHS), True))
            self.translation.append(Channel(channel_keys(data, header + offsets[1], i,
                                                          models.ID74_VEC_WIDTHS)))
            self.scale.append(Channel(channel_keys(data, header + offsets[2], i,
                                                    models.ID74_VEC_WIDTHS)))

    def advance(self):
        hold = False
        for r, t, s in zip(self.rotation, self.translation, self.scale):
            r.advance()
            t.advance()
            flag = s.advance()
            if flag is not None:
                hold = flag  # original uses last scale transition, not OR
        if hold:
            for r, t, s in zip(self.rotation, self.translation, self.scale):
                r.reciprocal = 0.0
                t.velocity = s.velocity = (0.0, 0.0, 0.0)

    def palette(self):
        world = []
        for i, parent in enumerate(self.parents):
            # Node zero has only the static adjustment matrix in 001C6DA0.
            # The opening has identity static adjustments and actor placement;
            # node1 (and Roger node2) contains the world animation translation.
            local = native.mat_identity() if i == 0 else quaternion_matrix(
                self.rotation[i].quaternion_value(), self.translation[i].value,
                self.scale[i].value)
            world.append(local if parent < 0 else matrix_multiply(world[parent], local))
        return world

    def bake(self):
        frames = [self.palette()]
        for _ in range((self.length - 1) * 2):
            self.advance()
            frames.append(self.palette())
        return frames


def exact_mesh_sections(raw):
    """Decode original packets with exact attribute keys, avoiding rounded welds."""
    payloads, nodes = native._walk_blob_blocks(raw)
    pos, normals, indices, bones, uv, texids = [], [], [], [], [], []
    textures, texture_ids, vertices = [], {}, {}
    for payload in payloads:
        run = []
        for off in range(0, len(payload) - 63, 64):
            w = struct.unpack_from("<f", payload, off + 60)[0]
            if abs(abs(w) - 1.0) > 0.25:
                break
            word = struct.unpack_from("<I", payload, off + 60)[0]
            bone = (word & 0x3FF) >> 3
            if bone < 2 and nodes != 1:
                continue
            if bone >= nodes:
                raise ValueError("mesh vertex addresses a missing bone")
            texture_key = struct.unpack_from("<Q", payload, off)[0] & native.TEX0_KEY_MASK
            if texture_key not in texture_ids:
                fields = native.tex0_fields(texture_key)
                if fields["psm"] != 0x14:
                    raise ValueError("opening texture is not the verified PSMT4 format")
                fields["key"] = texture_key
                texture_ids[texture_key] = len(textures)
                textures.append(fields)
            tid = texture_ids[texture_key]
            # Byte keys preserve distinct original float attributes, even where
            # the older general exporter rounds them to the same decimal value.
            key = (payload[off + 48:off + 60], payload[off + 32:off + 44],
                   payload[off + 16:off + 24], bone, tid)
            if key not in vertices:
                vertices[key] = len(pos)
                pos.append(struct.unpack("<3f", key[0]))
                normals.append(struct.unpack("<3f", key[1]))
                uv.append(struct.unpack("<2f", key[2]))
                bones.append(bone)
                texids.append(tid)
            vertex = vertices[key]
            run.append(vertex)
            if len(run) >= 3 and not word & 0x8000:
                a, b, c = run[-3:]
                if len({a, b, c}) == 3:
                    indices.extend((a, b, c) if len(run) % 2 == 0 else (b, a, c))
    return [(pos, normals, indices, bones, uv, texids)], textures


def original_palette(ram, actor, bones):
    result = []
    for i in range(bones):
        node = struct.unpack_from("<I", ram, actor + 0x110 + 4 * i)[0]
        result.append(struct.unpack_from("<16f", ram, node + 0x90))
    return result


def attach_player_equipment(library, sections, textures, ram):
    """Merge only the seven opening-proven child draws, with identity offsets."""
    report = []
    texture_ids = {entry["key"]: i for i, entry in enumerate(textures)}
    for model_id, bone, actor in PLAYER_ATTACHMENTS:
        offset = struct.unpack_from("<I", library, 4 + model_id * 4)[0]
        size = struct.unpack_from("<I", library, offset + 12)[0]
        raw = library[offset:offset + size]
        if ram is not None:
            address = struct.unpack_from("<I", ram, actor + 0x44)[0]
            if raw != ram[address:address + size]:
                raise ValueError(f"player attachment {model_id}: model bytes differ")
            if ram[actor + 1] != 1 or ram[actor + 0xC] != 1:
                raise ValueError(f"player attachment {model_id}: original draw inactive")
            parent_node = struct.unpack_from("<I", ram, 0x8102B0 + 0x110 + 4 * bone)[0]
            own_node = struct.unpack_from("<I", ram, actor + 0x110)[0]
            if ram[parent_node + 0x90:parent_node + 0xD0] != ram[own_node + 0x90:own_node + 0xD0]:
                raise ValueError(f"player attachment {model_id}: parent matrix differs")
        extra, extra_textures = exact_mesh_sections(raw)
        texture_map = []
        for entry in extra_textures:
            if entry["key"] not in texture_ids:
                texture_ids[entry["key"]] = len(textures)
                textures.append(entry)
            texture_map.append(texture_ids[entry["key"]])
        for positions, normals, indices, bones, uv, texids in extra:
            sections.append((positions, normals, indices, [bone] * len(bones),
                             uv, [texture_map[t] for t in texids]))
        report.append({"library_index": model_id, "bone": bone,
                       "actor_address": actor, "source_offset": offset,
                       "size": size, "sha256": hashlib.sha256(raw).hexdigest(),
                       "matrix_identity_offset_verified": ram is not None})
    return report


def compare_cursor(ram, actor, animation):
    """Check decoded node state as well as the final matrices at source135."""
    errors = {}
    for _ in range(270):
        animation.advance()
    for i, (rotation, translation, scale) in enumerate(zip(
            animation.rotation, animation.translation, animation.scale)):
        node = struct.unpack_from("<I", ram, actor + 0x110 + 4 * i)[0]
        # Every opening node has identity static adjustment (node+70..8c).
        static = struct.unpack_from("<6f3h", ram, node + 0x70)
        if static != (0, 0, 0, 0, 0, 0, 4096, 4096, 4096):
            raise ValueError("opening node has an unhandled static adjustment")
        for name, offset, values in (
            ("translation", 0, translation.value),
            ("translation_velocity", 12, translation.velocity),
            ("scale", 24, scale.value), ("scale_velocity", 36, scale.velocity),
            ("quaternion_left", 48, rotation.left),
            ("quaternion_right", 64, rotation.right),
            ("rotation_blend", 80, (rotation.blend,)),
            ("rotation_reciprocal", 84, (rotation.reciprocal,)),
            ("countdowns", 88, (translation.remaining, scale.remaining, rotation.remaining)),
        ):
            actual = struct.unpack_from(f"<{len(values)}f", ram, node + offset)
            errors[name] = max(errors.get(name, 0),
                               max(abs(x-y) for x, y in zip(actual, values)))
        actual_indices = struct.unpack_from("<3H", ram, node + 0x66)
        if actual_indices != (rotation.index, translation.index, scale.index):
            raise ValueError(f"bone {i}: key cursors differ from original RAM")
    if max(errors.values()) > 2e-7:
        raise ValueError(f"original animation cursor mismatch: {errors}")
    return errors


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--extract", type=Path, default=Path("extract"))
    parser.add_argument("--gs", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--reference-ee", type=Path)
    parser.add_argument("--report", type=Path, required=True)
    args = parser.parse_args()
    data = (args.extract / "chunk15/f12_id44.bin").read_bytes()
    count, *offsets = struct.unpack_from("<4I", data, BANK_OFFSET)
    if (count, offsets) != (3, [0x10, 0x5100, 0x10740]):
        raise ValueError("original AREA11 bank98 directory differs")
    ram = args.reference_ee.read_bytes() if args.reference_ee else None
    report = {"bank_source": "chunk15/f12_id44.bin", "bank_offset": BANK_OFFSET,
              "sample_step": 0.5, "world_translation_preserved": True,
              "quaternion_normalization": False, "scale_hold_flags": True,
              "gs_sha256": hashlib.sha256(args.gs.read_bytes()).hexdigest(),
              "actors": {}}
    poses = {}
    for name, source, offset, address, actor, clip_id in ACTORS:
        source_data = (args.extract / source).read_bytes()
        size = struct.unpack_from("<I", source_data, offset + 12)[0]
        raw = source_data[offset:offset + size]
        if ram is not None and raw != ram[address:address + size]:
            raise ValueError(f"{name}: whole raw model differs from original EE memory")
        sections, textures = exact_mesh_sections(raw)
        attachments = []
        if name == "player":
            attachments = attach_player_equipment(
                (args.extract / "chunk27/f01_id37.bin").read_bytes(),
                sections, textures, ram)
        if name == "equipment_6b":
            parents = [-1]
            frames = [[frame[1]] for frame in poses["roger"]]
        else:
            animation = OpeningClip(data, BANK_OFFSET + offsets[clip_id])
            parents, frames = animation.parents, animation.bake()
            poses[name] = frames
        entries, texels = native.build_texture_blob(None, textures, p2s=args.gs)
        destination = args.out / f"{name}.emdl"
        native.write_emdl(destination, sections, [], parents, frames, 60.0,
                          entries, texels,
                          clips=[{"id": clip_id, "first": 0,
                                  "count": len(frames), "fps": 60.0}])
        info = {"source": source, "offset": offset, "size": size,
                "model_sha256": hashlib.sha256(raw).hexdigest(),
                "reference_address": address, "actor_address": actor,
                "clip_id": clip_id, "frames": len(frames), "bones": len(parents),
                "vertices": sum(len(section[0]) for section in sections),
                "indices": sum(len(section[2]) for section in sections),
                "textures": len(textures), "output_sha256": hashlib.sha256(destination.read_bytes()).hexdigest()}
        if attachments:
            info["attachments"] = attachments
        if ram is not None:
            if name != "equipment_6b":
                clock = struct.unpack_from("<f", ram, actor + 0x3C)[0]
                if clock != 511:
                    raise ValueError("reference EE snapshot is not at opening source frame135")
                info["cursor_max_errors"] = compare_cursor(ram, actor,
                    OpeningClip(data, BANK_OFFSET + offsets[clip_id]))
            reference = original_palette(ram, actor, len(parents))
            # Captured clip clock 511 of length646 means source135 (half_tick270).
            sample = frames[270]
            errors = [abs(x-y) for matrix, ref in zip(sample, reference)
                      for x, y in zip(sum(matrix, ()), ref)]
            info["reference_half_tick"] = 270
            info["max_matrix_error"] = max(errors)
            info["rms_matrix_error"] = math.sqrt(sum(x*x for x in errors) / len(errors))
            if max(errors) > 0.0001:
                raise ValueError(f"{name}: original opening pose error {max(errors)}")
        report["actors"][name] = info
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
