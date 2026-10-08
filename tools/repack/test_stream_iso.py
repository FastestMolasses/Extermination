"""Disc extent changes require paired stream/cue integration, not loose edits."""
from pathlib import Path
import tempfile
import unittest

from tools.repack import iso
from tools.repack.test_iso import synthetic_iso


class StreamIsoTests(unittest.TestCase):
    def setUp(self):
        root = iso.OUTPUT_ROOT
        root.mkdir(parents=True, exist_ok=True)
        temp = tempfile.TemporaryDirectory(prefix="stream-iso-test-", dir=root)
        self.addCleanup(temp.cleanup)
        self.root = Path(temp.name)
        self.original = synthetic_iso(self.root / "original.iso", streams=True)
        self.tree = self.root / "tree"
        iso.unpack(self.original, self.tree)
        self.replacement = self.root / "music.dat"
        self.replacement.write_bytes(b"N" * (7 * iso.SECTOR))

    def test_loose_or_unpaired_stream_growth_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "size changes unsupported"):
            iso.pack(self.tree, self.root / "bad.iso",
                     {"STREAM/MUSIC.DAT": self.replacement})
        with self.assertRaisesRegex(ValueError, "verified cue metadata"):
            iso.pack(self.tree, self.root / "bad.iso", resized_streams={"STREAM/MUSIC.DAT"})
        with self.assertRaisesRegex(ValueError, "verified cue metadata"):
            iso.pack(self.tree, self.root / "bad.iso", {"SCUS_971.12": self.replacement},
                     resized_streams={"SCUS_971.12"})

    def test_validated_growth_and_shrink_reextract_and_repack_exactly(self):
        for size in (7 * iso.SECTOR, iso.SECTOR):
            with self.subTest(size=size):
                self.replacement.write_bytes(b"N" * size)
                output = self.root / f"edited-{size}.iso"
                report = iso.pack(self.tree, output, {"STREAM/MUSIC.DAT": self.replacement},
                                  resized_streams={"STREAM/MUSIC.DAT"})
                self.assertEqual(len(report["resized_files"]), 1)
                tree = self.root / f"edited-{size}"
                manifest = iso.unpack(output, tree)
                for entry in manifest["files"]:
                    path = entry["path"]
                    expected = self.replacement if path == "STREAM/MUSIC.DAT" else self.tree / "files" / path
                    self.assertEqual((tree / "files" / path).read_bytes(), expected.read_bytes())
                rebuilt = self.root / f"repacked-{size}.iso"
                iso.pack(tree, rebuilt)
                self.assertEqual(output.read_bytes(), rebuilt.read_bytes())


if __name__ == "__main__":
    unittest.main()
