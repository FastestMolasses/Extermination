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

    def test_byte_growth_and_shrink_preserve_exact_payloads_and_dma_bounds(self):
        for number, delta in enumerate((1, 37, -13)):
            with self.subTest(delta=delta):
                _, entry = self.find_file("chunk00")
                path = self.tree / entry["path"]
                original = path.read_bytes()
                path.write_bytes(original + b"x" * delta if delta > 0 else original[:delta])
                expected = payloads(self.tree, self.manifest)
                data, index = self.repack(f"byte-{number}")
                checked_tree = self.base / f"byte-checked-{number}"
                checked = archive.unpack_archive(data, index, checked_tree)
                self.assertEqual(payloads(checked_tree, checked), expected)
                self.assertEqual(word(index.read_bytes(), 8), 8 * SECTOR + delta)
                self.assertEqual(word(index.read_bytes(), 0x20 + 3 * 8 + 4), 2 * SECTOR + delta)
                self.assertEqual(word(index.read_bytes(), 0x104) % SECTOR, 0)
                path.write_bytes(original)

    def test_byte_upload_growth_adds_explicit_padding_without_changing_other_assets(self):
        _, entry = self.find_file("chunk00", "sound")
        path = self.tree / entry["path"]
        path.write_bytes(path.read_bytes() + b"xyz")
        expected = payloads(self.tree, self.manifest)
        data, index = self.repack()
        raw = index.read_bytes()
        self.assertEqual(word(raw, 0x24), SECTOR + 3)
        self.assertEqual(word(raw, 0x28), 2 * SECTOR)
        checked_tree = self.base / "byte-upload"
        checked = archive.unpack_archive(data, index, checked_tree)
        actual = payloads(checked_tree, checked)
        for name, contents in expected.items():
            self.assertEqual(actual[name], contents)
        self.assertEqual(actual["chunk00/padding00.bin"], bytes(SECTOR - 3))
        again_data, again_index = self.base / "again.dat", self.base / "again.idx"
        archive.pack_archive(checked_tree, again_data, again_index)
        self.assertEqual(again_data.read_bytes(), data.read_bytes())
        self.assertEqual(again_index.read_bytes(), index.read_bytes())

    def test_append_entry_updates_count_and_offsets_and_roundtrips(self):
        source = self.base / "authored.bin"
        source.write_bytes(b"authored new asset" * 3)
        edit = archive.add_entry(self.tree, "chunk00.n0", 0x7E, source)
        expected = payloads(self.tree, self.manifest)
        expected[edit["path"]] = source.read_bytes()
        data, index = self.repack()
        self.assertEqual(word(index.read_bytes(), 0x100 + 0x1C), 3)
        self.assertEqual(word(index.read_bytes(), 0x100 + 0x28), 0x7E000000 | 2 * SECTOR)
        self.assert_reunpacks(data, index, expected)
        again_data, again_index = self.base / "added-again.dat", self.base / "added-again.idx"
        archive.pack_archive(self.base / "checked", again_data, again_index)
        self.assertEqual(again_data.read_bytes(), data.read_bytes())
        self.assertEqual(again_index.read_bytes(), index.read_bytes())

    def test_append_cannot_overwrite_unknown_table_slack_or_duplicate_ids(self):
        source = self.base / "authored.bin"
        source.write_bytes(b"new asset")
        with self.assertRaisesRegex(ValueError, "duplicates"):
            archive.add_entry(self.tree, "chunk00.n0", 0x42, source)
        self.assertFalse((self.tree / "edits.json").exists())
        raw = bytearray(self.index.read_bytes())
        raw[0x128] = 1
        special = self.base / "occupied.idx"
        special.write_bytes(raw)
        tree = self.base / "occupied"
        archive.unpack_archive(self.data, special, tree)
        with self.assertRaisesRegex(ValueError, "nonzero unknown"):
            archive.add_entry(tree, "chunk00.n0", 0x7E, source)

    def test_dangling_edit_metadata_symlink_does_not_create_its_target(self):
        source = self.base / "new-asset.bin"
        source.write_bytes(b"authored new asset")
        unrelated = self.base / "missing-metadata.json"
        (self.tree / "edits.json").symlink_to(unrelated)
        with self.assertRaises(ValueError):
            archive.add_entry(self.tree, "chunk00.n0", 0x7E, source)
        self.assertFalse(unrelated.exists())
        self.assertFalse((self.tree / "chunk00.n0/f02_id7e.bin").exists())

    def test_edit_metadata_hardlink_replacement_preserves_the_other_file(self):
        import json
        source = self.base / "new-asset.bin"
        source.write_bytes(b"authored new asset")
        unrelated = self.base / "unrelated-metadata.json"
        original = json.dumps({"schema": "extermination-archive-edits-v1", "additions": []}).encode()
        unrelated.write_bytes(original)
        os.link(unrelated, self.tree / "edits.json")
        archive.add_entry(self.tree, "chunk00.n0", 0x7E, source)
        self.assertEqual(unrelated.read_bytes(), original)
        self.assertFalse(unrelated.samefile(self.tree / "edits.json"))

    def test_dangling_new_payload_symlink_cannot_redirect_creation(self):
        source = self.base / "new-asset.bin"
        source.write_bytes(b"authored new asset")
        unrelated = self.tree / "unrelated-new-file.bin"
        (self.tree / "chunk00.n0/f02_id7e.bin").symlink_to(unrelated)
        with self.assertRaises(ValueError):
            archive.add_entry(self.tree, "chunk00.n0", 0x7E, source)
        self.assertFalse(unrelated.exists())

    def test_zero_sized_upload_stays_empty_at_a_relocated_boundary(self):
        raw = bytearray(SECTOR)
        struct.pack_into("<IIIHHIIII", raw, 0, 0, 0, 3 * SECTOR, 0, 3, 0,
                         2 * SECTOR, 0, 1)
        struct.pack_into("<6I", raw, 0x20, 0, SECTOR, SECTOR, 0, SECTOR, SECTOR)
        struct.pack_into("<I", raw, 0x38, 0x43000000)
        index = self.base / "zero-section.idx"
        data = self.base / "zero-section.dat"
        index.write_bytes(raw)
        data.write_bytes(bytes([19]) * (3 * SECTOR))
        self.tree = self.base / "zero-section"
        self.manifest = archive.unpack_archive(data, index, self.tree)
        _, entry = self.find_file("chunk00", "transient")
        path = self.tree / entry["path"]
        path.write_bytes(path.read_bytes() + b"x")
        packed_data, packed_index = self.repack("zero-packed")
        self.assertEqual(word(packed_index.read_bytes(), 0x28), 2 * SECTOR)
        self.assertEqual(word(packed_index.read_bytes(), 0x2C), 0)
        archive.unpack_archive(packed_data, packed_index, self.base / "zero-checked")

    def test_added_entries_cannot_overflow_nested_descriptor(self):
        source = self.base / "authored.bin"
        source.write_bytes(b"x")
        for ident in range(18):
            archive.add_entry(self.tree, "chunk00.n0", ident, source)
        with self.assertRaisesRegex(ValueError, "fixed INDEX"):
            archive.add_entry(self.tree, "chunk00.n0", 18, source)

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
