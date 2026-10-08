"""Synthetic archive proofs; all fixtures are authored here, never disc data."""
from __future__ import annotations

from pathlib import Path
import os
import struct
import tempfile
import unittest

from tools.repack import archive

ROOT = Path(__file__).resolve().parents[2]
SECTOR = 0x800


def word(blob: bytes, offset: int) -> int:
    return struct.unpack_from("<I", blob, offset)[0]


def synthetic_archive(directory: Path) -> tuple[Path, Path]:
    """Make multiple optional sections, resident aliases, nesting and spare bytes."""
    directory.mkdir(parents=True, exist_ok=True)
    index = bytearray(2 * SECTOR)

    def descriptor(at, ident, offset, size, resident, counts, entries, sections, nested=0):
        struct.pack_into("<IIIHHIIII", index, at, ident, offset, size,
                         counts[0], counts[1], counts[2], resident, nested, len(entries))
        table = at + 0x20
        for start, length in sections:
            struct.pack_into("<II", index, table, start, length)
            table += 8
        for ident, offset in entries:
            struct.pack_into("<I", index, table, ident << 24 | offset)
            table += 4

    descriptor(0, 0, 0, 8 * SECTOR, 4 * SECTOR, (1, 2, 2),
               [(0x43, 0), (0x44, 2 * SECTOR), (0x73, 3 * SECTOR)],
               [(0, SECTOR), (SECTOR, 2 * SECTOR), (3 * SECTOR, SECTOR),
                (0, 2 * SECTOR), (2 * SECTOR, 2 * SECTOR)], nested=1)
    descriptor(0x100, 0, 8 * SECTOR, 2 * SECTOR, 0, (0, 0, 0),
               [(0x42, 0), (0x46, SECTOR)], [])
    descriptor(SECTOR, 1, 10 * SECTOR, 2 * SECTOR, 0, (0, 0, 0),
               [(0x05, 0), (0x06, SECTOR)], [])
    # Metadata outside every live descriptor/table must survive byte-for-byte.
    index[0x90:0xA0] = b"SYNTHETIC_SPARE!"
    index[-16:] = bytes(range(16))
    payload = b"".join(bytes([i + 17]) * SECTOR for i in range(12))
    data_path, index_path = directory / "DATA.DAT", directory / "INDEX.IDX"
    data_path.write_bytes(payload)
    index_path.write_bytes(index)
    return data_path, index_path


def payloads(tree: Path, manifest: dict) -> dict[str, bytes]:
    return {entry["path"]: (tree / entry["path"]).read_bytes()
            for region in manifest["regions"] for entry in region["files"]}


class ArchiveTests(unittest.TestCase):
    def setUp(self):
        (ROOT / "build/repack").mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="archive-test-", dir=ROOT / "build/repack")
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.data, self.index = synthetic_archive(self.base / "input")
        self.tree = self.base / "tree"
        self.manifest = archive.unpack_archive(self.data, self.index, self.tree)

    def repack(self, name="packed"):
        output = self.base / name
        output.mkdir(exist_ok=True)
        dat, idx = output / "DATA.DAT", output / "INDEX.IDX"
        archive.pack_archive(self.tree, dat, idx)
        return dat, idx

    def find_file(self, region_name, kind="resident", ordinal=0):
        region = next(r for r in self.manifest["regions"] if r["label"] == region_name)
        entry = [f for f in region["files"] if f["kind"] == kind][ordinal]
        return region, entry

    def assert_reunpacks(self, data, index, expected):
        destination = self.base / "checked"
        manifest = archive.unpack_archive(data, index, destination)
        self.assertEqual(payloads(destination, manifest), expected)

    def test_byte_identical_roundtrip_preserves_unused_index_bytes(self):
        data, index = self.repack()
        self.assertEqual(data.read_bytes(), self.data.read_bytes())
        self.assertEqual(index.read_bytes(), self.index.read_bytes())

    def test_correct_resident_labels_and_multiple_section_counts(self):
        region, entry = self.find_file("chunk00")
        self.assertTrue(entry["path"].endswith("f00_id43.bin"))
        self.assertEqual(entry["offset"], 4 * SECTOR)
        self.assertEqual((self.tree / entry["path"]).read_bytes(),
                         self.data.read_bytes()[4 * SECTOR:6 * SECTOR])
        self.assertEqual(len([f for f in region["files"] if f["kind"] == "transient"]), 2)

    def test_same_size_mutation_changes_only_target_byte_for_every_loose_file(self):
        original = self.data.read_bytes()
        for region in self.manifest["regions"]:
            for entry in region["files"]:
                with self.subTest(path=entry["path"]):
                    path = self.tree / entry["path"]
                    payload = path.read_bytes()
                    if not payload:
                        continue
                    at = len(payload) // 2
                    edited = bytearray(payload)
                    edited[at] ^= 0x80
                    path.write_bytes(edited)
                    try:
                        data, index = self.repack()
                        expected = bytearray(original)
                        expected[region["offset"] + entry["offset"] + at] ^= 0x80
                        self.assertEqual(data.read_bytes(), expected)
                        self.assertEqual(index.read_bytes(), self.index.read_bytes())
                    finally:
                        path.write_bytes(payload)

    def test_grow_resident_updates_aliases_relocations_and_later_regions(self):
        _, entry = self.find_file("chunk00")
        path = self.tree / entry["path"]
        path.write_bytes(path.read_bytes() + bytes([99]) * SECTOR)
        expected = payloads(self.tree, self.manifest)
        data, index = self.repack()
        raw = index.read_bytes()
        self.assertEqual(word(raw, 8), 9 * SECTOR)
        self.assertEqual(word(raw, 0x14), 4 * SECTOR)
        self.assertEqual(word(raw, 0x20 + 3 * 8 + 4), 3 * SECTOR)
        self.assertEqual(word(raw, 0x20 + 4 * 8), 3 * SECTOR)
        self.assertEqual(word(raw, 0x20 + 4 * 8 + 4), 2 * SECTOR)
        self.assertEqual(word(raw, 0x48 + 4) & 0xFFFFFF, 3 * SECTOR)
        self.assertEqual(word(raw, 0x48 + 8) & 0xFFFFFF, 4 * SECTOR)
        self.assertEqual(word(raw, 0x104), 9 * SECTOR)
        self.assertEqual(word(raw, SECTOR + 4), 11 * SECTOR)
        self.assert_reunpacks(data, index, expected)

    def test_shrink_resident_preserves_other_payloads(self):
        _, entry = self.find_file("chunk00")
        path = self.tree / entry["path"]
        path.write_bytes(path.read_bytes()[:-SECTOR])
        expected = payloads(self.tree, self.manifest)
        data, index = self.repack()
        self.assertEqual(word(index.read_bytes(), 8), 7 * SECTOR)
        self.assert_reunpacks(data, index, expected)

    def test_grow_upload_updates_section_offsets_and_resident_base(self):
        _, entry = self.find_file("chunk00", "sound")
        path = self.tree / entry["path"]
        path.write_bytes(path.read_bytes() + bytes([99]) * SECTOR)
        expected = payloads(self.tree, self.manifest)
        data, index = self.repack()
        raw = index.read_bytes()
        self.assertEqual(word(raw, 0x24), 2 * SECTOR)
        self.assertEqual(word(raw, 0x28), 2 * SECTOR)
        self.assertEqual(word(raw, 0x30), 4 * SECTOR)
        self.assertEqual(word(raw, 0x14), 5 * SECTOR)
        self.assertEqual(word(raw, 0x48 + 4) & 0xFFFFFF, 2 * SECTOR)
        self.assert_reunpacks(data, index, expected)

    def test_shrink_transient_updates_upload_size_and_later_offsets(self):
        _, entry = self.find_file("chunk00", "transient")
        path = self.tree / entry["path"]
        path.write_bytes(path.read_bytes()[:-SECTOR])
        expected = payloads(self.tree, self.manifest)
        data, index = self.repack()
        raw = index.read_bytes()
        self.assertEqual(word(raw, 0x2C), SECTOR)
        self.assertEqual(word(raw, 0x30), 2 * SECTOR)
        self.assertEqual(word(raw, 0x14), 3 * SECTOR)
        self.assert_reunpacks(data, index, expected)

    def test_output_alias_to_loose_input_is_rejected(self):
        _, entry = self.find_file("chunk00")
        source = self.tree / entry["path"]
        original = source.read_bytes()
        for name, linker in [("symlink", os.symlink), ("hardlink", os.link)]:
            with self.subTest(name=name):
                alias = self.base / name
                linker(source, alias)
                with self.assertRaises(ValueError):
                    archive.pack_archive(self.tree, alias, self.base / "new.idx")
                self.assertEqual(source.read_bytes(), original)

    def test_original_source_cannot_be_overwritten_or_required_for_repacking(self):
        expected_data, expected_index = self.data.read_bytes(), self.index.read_bytes()
        with self.assertRaises(ValueError):
            archive.pack_archive(self.tree, self.data, self.base / "new.idx")
        with self.assertRaises(ValueError):
            archive.pack_archive(self.tree, self.base / "new.dat", self.index)
        alias = self.base / "original-hardlink.dat"
        os.link(self.data, alias)
        with self.assertRaises(ValueError):
            archive.pack_archive(self.tree, alias, self.base / "new.idx")
        alias.unlink()
        self.data.unlink()
        self.index.unlink()
        data, index = self.repack()
        self.assertEqual(data.read_bytes(), expected_data)
        self.assertEqual(index.read_bytes(), expected_index)

    def test_resize_crossing_interior_dma_boundary_is_rejected(self):
        raw = bytearray(self.index.read_bytes())
        struct.pack_into("<I", raw, 0x20 + 3 * 8 + 4, SECTOR)
        struct.pack_into("<II", raw, 0x20 + 4 * 8, SECTOR, 3 * SECTOR)
        special = self.base / "interior.idx"
        special.write_bytes(raw)
        self.tree = self.base / "interior"
        self.manifest = archive.unpack_archive(self.data, special, self.tree)
        _, entry = self.find_file("chunk00")
        path = self.tree / entry["path"]
        path.write_bytes(path.read_bytes() + bytes(SECTOR))
        with self.assertRaises(ValueError):
            self.repack()

    def test_relocation_overflow_is_rejected_before_large_output(self):
        _, entry = self.find_file("chunk00")
        with (self.tree / entry["path"]).open("r+b") as file:
            file.truncate(0x1000000 + 2 * SECTOR)
        with self.assertRaises(ValueError):
            self.repack()

    def test_empty_descriptor_anchors_track_grown_and_shrunk_boundaries(self):
        raw = bytearray(self.index.read_bytes()) + bytearray(2 * SECTOR)
        struct.pack_into("<III", raw, 2 * SECTOR, 2, 8 * SECTOR, 0)
        struct.pack_into("<III", raw, 3 * SECTOR, 3, 12 * SECTOR, 0)
        special = self.base / "anchors.idx"
        special.write_bytes(raw)
        self.tree = self.base / "anchors"
        self.manifest = archive.unpack_archive(self.data, special, self.tree)
        _, entry = self.find_file("chunk00")
        path = self.tree / entry["path"]
        original = path.read_bytes()
        for delta, payload in [(SECTOR, original + bytes(SECTOR)),
                               (-SECTOR, original[:-SECTOR])]:
            with self.subTest(delta=delta):
                path.write_bytes(payload)
                _, index = self.repack()
                self.assertEqual(word(index.read_bytes(), 2 * SECTOR + 4), 8 * SECTOR + delta)
                self.assertEqual(word(index.read_bytes(), 3 * SECTOR + 4), 12 * SECTOR + delta)

    def test_unaligned_size_change_is_rejected(self):
        _, entry = self.find_file("chunk00")
        path = self.tree / entry["path"]
        path.write_bytes(path.read_bytes() + b"x")
        with self.assertRaises(ValueError):
            self.repack()

    def test_malformed_resident_and_count_fields_are_rejected(self):
        for offset, value in [(0x14, 20 * SECTOR), (0x18, 100), (0x1C, 500)]:
            with self.subTest(offset=offset):
                index = bytearray(self.index.read_bytes())
                struct.pack_into("<I", index, offset, value)
                bad = self.base / "bad.idx"
                bad.write_bytes(index)
                with self.assertRaises(ValueError):
                    archive.unpack_archive(self.data, bad, self.base / f"bad-{offset}")


if __name__ == "__main__":
    unittest.main()
