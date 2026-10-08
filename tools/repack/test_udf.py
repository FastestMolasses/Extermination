"""Verify bridge images with an independent UDF producer and reader."""
from __future__ import annotations

import io
from pathlib import Path
import struct
import tempfile
import unittest

from tools.repack import iso

try:
    import pycdlib
except ImportError:
    pycdlib = None

ROOT = Path(__file__).resolve().parents[2]


@unittest.skipIf(pycdlib is None, "pycdlib is needed for independent UDF verification")
class UdfBridgeTests(unittest.TestCase):
    def setUp(self):
        (ROOT / "build/repack").mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="udf-test-", dir=ROOT / "build/repack")
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.original = self.base / "bridge.iso"
        self.content = {"DATA/DATA.DAT": b"D" * 4096, "DATA/INDEX.IDX": b"I" * 2048,
                        "SYSTEM.CNF": b"Synthetic opaque content\n"}
        image = pycdlib.PyCdlib()
        image.new(udf="2.60")
        image.add_directory(iso_path="/DATA", udf_path="/DATA")
        for path, content in self.content.items():
            image.add_fp(io.BytesIO(content), len(content), iso_path="/" + path + ";1",
                         udf_path="/" + path)
        image.write(str(self.original))
        image.close()
        self.tree = self.base / "tree"
        manifest = iso.unpack(self.original, self.tree)
        self.assertIsNotNone(manifest["udf"])
        self.assertEqual(len(manifest["udf"]["anchors"]), 2)
        self.assertEqual(len(manifest["udf"]["files"]), len(self.content))

    def read_namespaces(self, path: Path):
        image = pycdlib.PyCdlib()
        image.open(str(path))
        try:
            result = {}
            self.assertTrue(image.has_udf())
            for name in self.content:
                udf_file, iso_file = io.BytesIO(), io.BytesIO()
                image.get_file_from_iso_fp(udf_file, udf_path="/" + name)
                image.get_file_from_iso_fp(iso_file, iso_path="/" + name + ";1")
                self.assertEqual(udf_file.getvalue(), iso_file.getvalue(), name)
                result[name] = udf_file.getvalue()
            return result
        finally:
            image.close()

    def test_bridge_roundtrip_is_exact_in_both_namespaces(self):
        output = self.base / "no-op.iso"
        iso.pack(self.tree, output)
        self.assertEqual(output.read_bytes(), self.original.read_bytes())
        self.assertEqual(self.read_namespaces(output), self.content)

    def test_resized_archive_is_readable_identically_through_iso_and_udf(self):
        for label, content in [("grow", b"G" * (11 * 2048)), ("shrink", b"S" * 2048)]:
            with self.subTest(label=label):
                (self.tree / "files/DATA/DATA.DAT").write_bytes(content)
                output = self.base / f"{label}.iso"
                iso.pack(self.tree, output)
                expected = dict(self.content, **{"DATA/DATA.DAT": content})
                self.assertEqual(self.read_namespaces(output), expected)

    def test_tag_id_alone_does_not_create_a_backup_anchor(self):
        image = bytearray(self.original.read_bytes())
        candidate = (len(image) // 2048 - 256) * 2048
        struct.pack_into("<H", image, candidate, 2)
        altered = self.base / "not-an-anchor.iso"
        altered.write_bytes(image)
        manifest = iso.inventory(altered)
        self.assertEqual(len(manifest["udf"]["anchors"]), 2)


if __name__ == "__main__":
    unittest.main()
