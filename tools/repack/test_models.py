"""Same-topology native model/glTF tests; disc checks require EM_TEST_FULL."""
from __future__ import annotations

import hashlib
import base64
import json
import os
from pathlib import Path
import struct
import tempfile
import unittest

from tools.repack import models
from tools.repack.archive import ROOT


def synthetic(path, *, colour=False):
    raw = bytearray(b"\xa7" * (64 + models.STRIDE + 37))
    struct.pack_into("<4I", raw, 0, 1, 130, 4, 64 + models.STRIDE)
    raw[0x48:0x50] = models.SIGNATURE
    for i in range(32):
        offset = 0x50 + 64 * i
        raw[offset:offset + 64] = bytes(64)
        struct.pack_into("<Q", raw, offset, 0x2000000000000000 if i < 2 else 0)
        struct.pack_into("<4f", raw, offset + 16, i / 31, (i % 3) / 2, 1, 0)
        attr = (.25, .5, .75, 1) if colour else (0, 1, 0, 0)
        struct.pack_into("<4f", raw, offset + 32, *attr)
        struct.pack_into("<3f", raw, offset + 48, (i % 4) - 1, (i // 4) - 3, (i % 3) - 1)
        joint = 2 if i % 2 == 0 else 3
        struct.pack_into("<I", raw, offset + 60,
                         0x3f800000 | joint * 8 | (0x8000 if i < 2 else 0) | (0x4000 if i % 2 else 0))
    raw[0x50 + 31 * 64:0x50 + 32 * 64] = raw[0x50 + 30 * 64:0x50 + 31 * 64]
    path.write_bytes(raw)


def edit_attribute(tree, name, row, values):
    doc = json.loads((tree / "model.gltf").read_text())
    attrs = doc["meshes"][0]["primitives"][0]["attributes"]
    accessor = doc["accessors"][attrs[name]]
    view = doc["bufferViews"][accessor["bufferView"]]
    code = {5126: "f", 5125: "I", 5123: "H"}[accessor["componentType"]]
    components = {"SCALAR": 1, "VEC2": 2, "VEC3": 3, "VEC4": 4}[accessor["type"]]
    format = "<" + code * components
    buffer = bytearray((tree / "model.bin").read_bytes())
    struct.pack_into(format, buffer, view["byteOffset"] + row * struct.calcsize(format), *values)
    (tree / "model.bin").write_bytes(buffer)


class ModelTests(unittest.TestCase):
    def setUp(self):
        base = ROOT / "build/repack/model-edit"
        base.mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="test-", dir=base)
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.source, self.tree, self.output = (self.base / value for value in ("source.bin", "tree", "output.bin"))

    def export(self, *, colour=False):
        synthetic(self.source, colour=colour)
        return models.unpack_model(self.source, self.tree, kind="static" if colour else "auto")

    def test_unchanged_gltf_is_byte_exact_for_normal_and_colour_packets(self):
        for colour in (False, True):
            source = self.base / f"source-{colour}.bin"
            tree = self.base / f"tree-{colour}"
            synthetic(source, colour=colour)
            manifest = models.unpack_model(source, tree, kind="static" if colour else "auto")
            self.assertEqual(manifest["vertex_count"], 31)
            self.assertTrue(models.pack_model(tree, self.output)["unchanged"])
            self.assertEqual(self.output.read_bytes(), source.read_bytes())

    def test_position_uv_and_normal_edit_only_native_fields_and_alias_tail(self):
        manifest = self.export()
        edit_attribute(self.tree, "POSITION", 30, (0, 4, -1))
        edit_attribute(self.tree, "TEXCOORD_0", 30, (.75, .25))
        edit_attribute(self.tree, "NORMAL", 30, (1, 0, 0))
        summary = models.pack_model(self.tree, self.output)
        self.assertEqual(summary["edited_vertices"], 1)
        original, packed = self.source.read_bytes(), self.output.read_bytes()
        offsets = [manifest["layout"]["vertices"][30]["offset"], *manifest["layout"]["vertices"][30]["aliases"]]
        editable = {offset + delta for offset in offsets for start, size in ((16, 8), (32, 12), (48, 12))
                    for delta in range(start, start + size)}
        self.assertTrue(all(i in editable for i, (a, b) in enumerate(zip(original, packed)) if a != b))
        for offset in offsets:
            self.assertEqual(struct.unpack_from("<3f", packed, offset + 48), (0, 4, -1))
            self.assertEqual(struct.unpack_from("<2f", packed, offset + 16), (.75, .25))
            self.assertEqual(struct.unpack_from("<3f", packed, offset + 32), (1, 0, 0))
        # Independent established forward decoder reads the edited attributes.
        from tools.export_native import load_mesh_sections
        sections, _, _ = load_mesh_sections(self.output)
        positions, normals, _, _, uvs, _ = sections[0]
        self.assertIn(((0, 4, -1), (1, 0, 0), (.75, .25)), list(zip(positions, normals, uvs)))

    def test_authored_colour_edit_keeps_alpha_and_all_packet_bytes(self):
        self.export(colour=True)
        edit_attribute(self.tree, "COLOR_0", 0, (.5, .25, .125))
        models.pack_model(self.tree, self.output)
        a, b = self.source.read_bytes(), self.output.read_bytes()
        self.assertEqual(a[:0x70], b[:0x70])
        self.assertEqual(a[0x7c:], b[0x7c:])
        self.assertEqual(struct.unpack_from("<4f", b, 0x70), (.5, .25, .125, 1))

    def test_rigid_joint_edit_preserves_restart_parity_and_sign_bits(self):
        self.export()
        edit_attribute(self.tree, "POSITION", 0, (0, -3, -1))
        edit_attribute(self.tree, "JOINTS_0", 0, (3, 0, 0, 0))
        models.pack_model(self.tree, self.output)
        a = struct.unpack_from("<I", self.source.read_bytes(), 0x8c)[0]
        b = struct.unpack_from("<I", self.output.read_bytes(), 0x8c)[0]
        self.assertEqual(a & ~1023, b & ~1023)
        self.assertEqual(b & 1023, 24)

    def test_rejects_topology_vertex_identity_and_weight_changes(self):
        self.export()
        original = (self.tree / "model.bin").read_bytes()
        for name, values in (("_NATIVE_ID", (123, 0)), ("WEIGHTS_0", (.5, .5, 0, 0))):
            with self.subTest(name=name):
                edit_attribute(self.tree, name, 0, values)
                with self.assertRaisesRegex(ValueError, "identities or rigid weights"):
                    models.pack_model(self.tree, self.output)
                (self.tree / "model.bin").write_bytes(original)
        doc = json.loads((self.tree / "model.gltf").read_text())
        doc["meshes"][0]["primitives"][0]["mode"] = 5
        (self.tree / "model.gltf").write_text(json.dumps(doc))
        with self.assertRaisesRegex(ValueError, "structure changed"):
            models.pack_model(self.tree, self.output)

    def test_rejects_expanded_bounds_invalid_normals_and_nonfinite_values(self):
        self.export()
        original = (self.tree / "model.bin").read_bytes()
        for name, values, error in (("POSITION", (1000, 0, 0), "bounds"),
                                     ("NORMAL", (0, 2, 0), "unit length"),
                                     ("TEXCOORD_0", (float("nan"), 0), "finite"),
                                     ("JOINTS_0", (127, 0, 0, 0), "palette")):
            with self.subTest(name=name):
                edit_attribute(self.tree, name, 0, values)
                with self.assertRaisesRegex(ValueError, error):
                    models.pack_model(self.tree, self.output)
                (self.tree / "model.bin").write_bytes(original)

    def test_template_metadata_and_source_aliases_are_protected(self):
        self.export()
        for target in (self.source, self.tree / "model.bin"):
            with self.assertRaisesRegex(ValueError, "aliases"):
                models.pack_model(self.tree, target)
        alias = self.base / "hardlink.bin"
        alias.hardlink_to(self.source)
        with self.assertRaisesRegex(ValueError, "aliases"):
            models.pack_model(self.tree, alias)
        template = self.tree / "original.bin"
        template.write_bytes(template.read_bytes()[:-1])
        with self.assertRaisesRegex(ValueError, "template changed"):
            models.pack_model(self.tree, self.output)

    def test_truncated_or_unknown_packets_rejected_without_output(self):
        synthetic(self.source)
        self.source.write_bytes(self.source.read_bytes()[:512])
        with self.assertRaisesRegex(ValueError, "truncated"):
            models.unpack_model(self.source, self.tree)
        self.assertFalse(self.tree.exists())

    def test_gltf_uses_valid_custom_attribute_type_and_common_skin_root(self):
        self.export()
        doc = json.loads((self.tree / "model.gltf").read_text())
        attrs = doc["meshes"][0]["primitives"][0]["attributes"]
        identity = doc["accessors"][attrs["_NATIVE_ID"]]
        self.assertEqual((identity["componentType"], identity["type"]), (5123, "VEC2"))
        skin = doc["skins"][0]
        self.assertEqual(doc["nodes"][skin["skeleton"]]["children"], skin["joints"])

    def test_repacked_strided_buffers_and_accessor_order_preserve_native_semantics(self):
        self.export()
        edit_attribute(self.tree, "TEXCOORD_0", 4, (.375, .625))
        doc = json.loads((self.tree / "model.gltf").read_text())
        original = (self.tree / "model.bin").read_bytes()
        data = bytearray(b"PAD!")
        for accessor in doc["accessors"]:
            view = doc["bufferViews"][accessor["bufferView"]]
            components = {"SCALAR": 1, "VEC2": 2, "VEC3": 3, "VEC4": 4}[accessor["type"]]
            width = {5126: 4, 5125: 4, 5123: 2}[accessor["componentType"]]
            size = components * width
            start = len(data)
            padding = b"PAD!" if view.get("target") == 34962 else b""
            for row in range(accessor["count"]):
                data.extend(original[view["byteOffset"] + row * size:view["byteOffset"] + (row + 1) * size])
                data.extend(padding)
            view.update(byteOffset=start, byteLength=len(data) - start)
            if padding:
                view["byteStride"] = size + len(padding)
        # Reorder accessors and bufferViews independently, refreshing references.
        count = len(doc["accessors"])
        for primitive in doc["meshes"][0]["primitives"]:
            primitive["attributes"] = {name: count - 1 - index for name, index in primitive["attributes"].items()}
            primitive["indices"] = count - 1 - primitive["indices"]
        doc["accessors"].reverse()
        views = len(doc["bufferViews"])
        for accessor in doc["accessors"]:
            accessor["bufferView"] = views - 1 - accessor["bufferView"]
        doc["bufferViews"].reverse()
        doc["buffers"] = [dict(uri="repacked.bin", byteLength=len(data))]
        doc["asset"]["generator"] = "independent serializer"
        doc["nodes"][0]["translation"] = [0, 0, 0]
        (self.tree / "repacked.bin").write_bytes(data)
        (self.tree / "model.bin").unlink()
        (self.tree / "model.gltf").write_text(json.dumps(doc))
        self.assertEqual(models.pack_model(self.tree, self.output)["edited_vertices"], 1)
        self.assertEqual(struct.unpack_from("<2f", self.output.read_bytes(), 0x50 + 4 * 64 + 16), (.375, .625))

    def test_embedded_buffers_and_identity_bind_matrices_roundtrip(self):
        self.export()
        doc = json.loads((self.tree / "model.gltf").read_text())
        data = bytearray((self.tree / "model.bin").read_bytes())
        at = len(data)
        identity = (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
        data.extend(struct.pack("<16f", *identity) * 4)
        doc["bufferViews"].append(dict(buffer=0, byteOffset=at, byteLength=4 * 64))
        doc["accessors"].append(dict(bufferView=len(doc["bufferViews"]) - 1, componentType=5126, count=4, type="MAT4"))
        doc["skins"][0]["inverseBindMatrices"] = len(doc["accessors"]) - 1
        doc["buffers"] = [dict(uri="data:application/octet-stream;base64," + base64.b64encode(data).decode(), byteLength=len(data))]
        (self.tree / "model.gltf").write_text(json.dumps(doc))
        (self.tree / "model.bin").unlink()
        self.assertTrue(models.pack_model(self.tree, self.output)["unchanged"])

    def test_import_rejects_foreign_buffers_bad_views_and_changed_transforms(self):
        self.export()
        original = (self.tree / "model.gltf").read_text()
        for case in ("escape", "view", "transform", "order"):
            doc = json.loads(original)
            if case == "escape":
                doc["buffers"][0]["uri"] = "../source.bin"
            elif case == "view":
                doc["bufferViews"][0]["byteLength"] = 1
            elif case == "transform":
                doc["nodes"][0]["translation"] = [1, 0, 0]
            else:
                doc["meshes"][0]["primitives"][0]["attributes"]["POSITION"] = 1
            (self.tree / "model.gltf").write_text(json.dumps(doc))
            with self.subTest(case=case), self.assertRaises(ValueError):
                models.pack_model(self.tree, self.output)

    def test_rgba_colour_accessor_preserves_fixed_native_alpha(self):
        self.export(colour=True)
        doc = json.loads((self.tree / "model.gltf").read_text())
        index = doc["meshes"][0]["primitives"][0]["attributes"]["COLOR_0"]
        accessor = doc["accessors"][index]
        view = doc["bufferViews"][accessor["bufferView"]]
        data = bytearray((self.tree / "model.bin").read_bytes())
        start = len(data)
        data.extend(struct.pack("<4f", .25, .5, .75, 1) * accessor["count"])
        view.update(byteOffset=start, byteLength=16 * accessor["count"])
        accessor["type"] = "VEC4"
        doc["buffers"][0]["byteLength"] = len(data)
        (self.tree / "model.bin").write_bytes(data)
        (self.tree / "model.gltf").write_text(json.dumps(doc))
        self.assertTrue(models.pack_model(self.tree, self.output)["unchanged"])
        struct.pack_into("<f", data, start + 12, .5)
        (self.tree / "model.bin").write_bytes(data)
        with self.assertRaisesRegex(ValueError, "alpha"):
            models.pack_model(self.tree, self.output)

    def test_static_positions_cannot_expand_a_packet_inside_larger_scene_bounds(self):
        synthetic(self.source, colour=True)
        first = self.source.read_bytes()[:-37]
        raw = bytearray(first + first[64:])
        for row in range(32):
            at = 0x50 + models.STRIDE + row * 64 + 48
            x = struct.unpack_from("<f", raw, at)[0]
            struct.pack_into("<f", raw, at, x + 100)
        self.source.write_bytes(raw)
        models.unpack_model(self.source, self.tree, kind="static")
        edit_attribute(self.tree, "POSITION", 0, (50, -3, -1))
        with self.assertRaisesRegex(ValueError, "packet bounds"):
            models.pack_model(self.tree, self.output)


@unittest.skipUnless(os.environ.get("EM_TEST_FULL") == "1", "real model requires EM_TEST_FULL=1")
class RealModelTests(unittest.TestCase):
    def test_static_colour_packet_roundtrip_and_edit_through_existing_decoder(self):
        from tools.repack.test_audio import native_archive_leaves
        from tools.extract_models import read_vertices
        image = Path(os.environ.get("EM_TEST_ISO", ROOT / "Extermination-rebuilt.iso"))
        if not image.is_file():
            self.skipTest("provide the user's disc via EM_TEST_ISO")
        relative = "chunk11.n2/f01_id44.bin"
        raw = dict(native_archive_leaves(image, {relative}))[relative]
        base = ROOT / "build/repack/model-edit"
        base.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="real-static-", dir=base) as temporary:
            root = Path(temporary)
            source, tree, output = root / "static.bin", root / "tree", root / "edited.bin"
            source.write_bytes(raw)
            manifest = models.unpack_model(source, tree, kind="static")
            self.assertTrue(models.pack_model(tree, output)["unchanged"])
            self.assertEqual(output.read_bytes(), raw)
            edit_attribute(tree, "COLOR_0", 0, (.5, .25, .125))
            report = models.pack_model(tree, output)
            offset = manifest["layout"]["vertices"][0]["offset"]
            rebuilt = output.read_bytes()
            vertex = read_vertices(rebuilt, offset, offset + 64)[0][0]
            self.assertEqual(vertex.attr, (.5, .25, .125))
            self.assertEqual(rebuilt[:offset + 32], raw[:offset + 32])
            self.assertEqual(rebuilt[offset + 44:], raw[offset + 44:])
            (base / "static-test-receipt.json").write_text(json.dumps(dict(
                source_iso=str(image), source_leaf=relative, source_sha256=hashlib.sha256(raw).hexdigest(),
                noop_byte_identical=True, existing_decoder_reads_edited_colour=True,
                untouched_native_bytes_preserved=True, **report), indent=2) + "\n")

    def test_player_model_roundtrip_and_edit_through_existing_decoder(self):
        from tools.repack.test_audio import native_archive_leaves
        image = Path(os.environ.get("EM_TEST_ISO", ROOT / "Extermination-rebuilt.iso"))
        if not image.is_file():
            self.skipTest("provide the user's disc via EM_TEST_ISO")
        relative = "chunk28/f00_id3b.bin"
        raw = dict(native_archive_leaves(image, {relative}))[relative]
        base = ROOT / "build/repack/model-edit"
        base.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="real-test-", dir=base) as temporary:
            root = Path(temporary)
            source, tree, output = root / "player.bin", root / "tree", root / "edited.bin"
            source.write_bytes(raw)
            manifest = models.unpack_model(source, tree)
            self.assertEqual(manifest["packet_count"], 149)
            self.assertEqual(manifest["layout"]["nodes"], 21)
            self.assertTrue(models.pack_model(tree, output)["unchanged"])
            self.assertEqual(output.read_bytes(), raw)
            # Shrink one populated rigid joint around its own original centre.
            doc = json.loads((tree / "model.gltf").read_text())
            attrs = doc["meshes"][0]["primitives"][0]["attributes"]
            data = bytearray((tree / "model.bin").read_bytes())
            accessor = doc["accessors"][attrs["POSITION"]]
            view = doc["bufferViews"][accessor["bufferView"]]
            joint = 7
            minimum, maximum = manifest["layout"]["bounds"][str(joint)]
            centre = [(a + b) / 2 for a, b in zip(minimum, maximum)]
            expected = []
            for i, vertex in enumerate(manifest["layout"]["vertices"]):
                if vertex["joint"] == joint:
                    offset = view["byteOffset"] + i * 12
                    xyz = struct.unpack_from("<3f", data, offset)
                    struct.pack_into("<3f", data, offset, *[centre[k] + .8 * (xyz[k] - centre[k]) for k in range(3)])
                    expected.append(struct.unpack_from("<3f", data, offset))
            (tree / "model.bin").write_bytes(data)
            report = models.pack_model(tree, output)
            self.assertFalse(report["unchanged"])
            from tools.export_native import load_mesh_sections
            sections, _, _ = load_mesh_sections(output)
            observed = set(sections[0][0])
            self.assertTrue(set(expected).issubset(observed))
            modified = output.read_bytes()
            allowed = {vertex_offset + d for vertex in manifest["layout"]["vertices"] if vertex["joint"] == joint
                       for vertex_offset in [vertex["offset"], *vertex["aliases"]] for d in range(48, 60)}
            self.assertTrue(all(i in allowed for i, (a, b) in enumerate(zip(raw, modified)) if a != b))
            receipt = dict(source_iso=str(image), source_leaf=relative, source_sha256=hashlib.sha256(raw).hexdigest(),
                           noop_byte_identical=True, existing_decoder_reads_edited_positions=True,
                           untouched_native_bytes_preserved=True, edited_joint=joint, **report)
            (base / "model-test-receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")


if __name__ == "__main__":
    unittest.main()
