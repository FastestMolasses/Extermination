"""Independent ISO9660 fixtures and exact layout/editing proofs."""
from __future__ import annotations

from pathlib import Path
import os
import struct
import subprocess
import sys
import tempfile
import unittest

from tools.repack import iso

ROOT = Path(__file__).resolve().parents[2]
SECTOR = 2048


def both32(value: int) -> bytes:
    return struct.pack("<I", value) + struct.pack(">I", value)


def both16(value: int) -> bytes:
    return struct.pack("<H", value) + struct.pack(">H", value)


def record(name: bytes, extent: int, size: int, directory=False) -> bytes:
    length = 33 + len(name) + (len(name) % 2 == 0)
    buf = bytearray(length)
    buf[0] = length
    buf[2:10] = both32(extent)
    buf[10:18] = both32(size)
    buf[18:25] = bytes([126, 1, 1, 0, 0, 0, 0])
    buf[25] = 2 if directory else 0
    buf[28:32] = both16(1)
    buf[32] = len(name)
    buf[33:33 + len(name)] = name
    return bytes(buf)


def synthetic_iso(path: Path, *, streams=False) -> Path:
    """Construct raw ISO records without calling the implementation under test."""
    blob = bytearray(b"\xA7" * (34 * SECTOR))
    pvd = bytearray(SECTOR)
    pvd[:7] = b"\x01CD001\x01"
    pvd[8:40] = b"SYNTHETIC".ljust(32)
    pvd[40:72] = b"REPACK_TEST".ljust(32)
    pvd[80:88] = both32(34)
    pvd[120:124] = both16(1)
    pvd[124:128] = both16(1)
    pvd[128:132] = both16(SECTOR)
    directory_name = b"STREAM" if streams else b"DATA"
    payload_names = (b"MUSIC.DAT;1", b"VOICE.DAT;1") if streams else (b"DATA.DAT;1", b"INDEX.IDX;1")
    pvd[132:140] = both32(40 if streams else 38)
    struct.pack_into("<I", pvd, 140, 18)
    struct.pack_into(">I", pvd, 148, 19)
    pvd[156:190] = record(b"\0", 20, SECTOR, True)
    pvd[881] = 1
    blob[16 * SECTOR:17 * SECTOR] = pvd
    terminator = bytearray(SECTOR)
    terminator[:7] = b"\xffCD001\x01"
    blob[17 * SECTOR:18 * SECTOR] = terminator
    for sector, endian in [(18, "<"), (19, ">")]:
        table = bytearray(SECTOR)
        cursor = 0
        for name, extent, parent in [(b"\0", 20, 1), (directory_name, 21, 1), (b"OVERLAY", 22, 1)]:
            entry = bytes([len(name), 0]) + struct.pack(endian + "IH", extent, parent) + name
            if len(name) % 2:
                entry += b"\0"
            table[cursor:cursor + len(entry)] = entry
            cursor += len(entry)
        blob[sector * SECTOR:(sector + 1) * SECTOR] = table
    directories = {
        20: [record(b"\0", 20, SECTOR, True), record(b"\1", 20, SECTOR, True),
             record(directory_name, 21, SECTOR, True), record(b"OVERLAY", 22, SECTOR, True),
             record(b"SCUS_971.12;1", 23, 600), record(b"SYSTEM.CNF;1", 24, 50)],
        21: [record(b"\0", 21, SECTOR, True), record(b"\1", 20, SECTOR, True),
             record(payload_names[0], 25, 2 * SECTOR), record(payload_names[1], 28, SECTOR)],
        22: [record(b"\0", 22, SECTOR, True), record(b"\1", 20, SECTOR, True),
             record(b"AREA00.BIN;1", 30, 512)],
    }
    for sector, records in directories.items():
        content = b"".join(records)
        blob[sector * SECTOR:(sector + 1) * SECTOR] = content.ljust(SECTOR, b"\0")
    for extent, size, value in [(23, 600, 11), (24, 50, 22), (25, 2 * SECTOR, 33),
                                (28, SECTOR, 44), (30, 512, 55)]:
        blob[extent * SECTOR:extent * SECTOR + size] = bytes([value]) * size
    path.write_bytes(blob)
    return path


class IsoTests(unittest.TestCase):
    def setUp(self):
        (ROOT / "build/repack").mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="iso-test-", dir=ROOT / "build/repack")
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.input = synthetic_iso(self.base / "input.iso")
        self.tree = self.base / "tree"
        self.manifest = iso.unpack(self.input, self.tree)

    def test_full_image_roundtrip_preserves_system_area_gaps_and_padding(self):
        output = self.base / "output.iso"
        iso.pack(self.tree, output)
        self.assertEqual(output.read_bytes(), self.input.read_bytes())
        self.assertEqual(len(self.manifest["files"]), 5)

    def test_single_byte_changes_exactly_one_image_byte(self):
        entry = next(f for f in self.manifest["files"] if f["path"] == "DATA/DATA.DAT")
        path = self.tree / "files" / entry["path"]
        payload = bytearray(path.read_bytes())
        payload[123] ^= 1
        path.write_bytes(payload)
        output = self.base / "output.iso"
        iso.pack(self.tree, output)
        expected = bytearray(self.input.read_bytes())
        expected[entry["offset"] + 123] ^= 1
        self.assertEqual(output.read_bytes(), expected)

    def test_grow_and_shrink_patch_both_endian_extent_and_size(self):
        original_files = {f["path"]: (self.tree / "files" / f["path"]).read_bytes()
                          for f in self.manifest["files"]}
        source = self.tree / "files/DATA/DATA.DAT"
        for name, content in [("grown", bytes([91]) * (5 * SECTOR)),
                              ("shrunk", bytes([92]) * SECTOR)]:
            with self.subTest(name=name):
                source.write_bytes(content)
                output = self.base / f"{name}.iso"
                iso.pack(self.tree, output)
                tree = self.base / name
                manifest = iso.unpack(output, tree)
                expected = dict(original_files, **{"DATA/DATA.DAT": content})
                actual = {f["path"]: (tree / "files" / f["path"]).read_bytes()
                          for f in manifest["files"]}
                self.assertEqual(actual, expected)
                entry = next(f for f in manifest["files"] if f["path"] == "DATA/DATA.DAT")
                image = output.read_bytes()
                for offset in entry["records"]:
                    self.assertEqual(image[offset + 2:offset + 10], both32(entry["offset"] // SECTOR))
                    self.assertEqual(image[offset + 10:offset + 18], both32(len(content)))
                volume_size = len(image) // SECTOR
                self.assertEqual(image[16 * SECTOR + 80:16 * SECTOR + 88], both32(volume_size))

    def test_explicit_override_replaces_only_requested_payload(self):
        replacement = self.base / "replacement.dat"
        replacement.write_bytes(b"Q" * (2 * SECTOR))
        output = self.base / "override.iso"
        iso.pack(self.tree, output, overrides={"DATA/DATA.DAT": replacement})
        entry = next(f for f in self.manifest["files"] if f["path"] == "DATA/DATA.DAT")
        expected = bytearray(self.input.read_bytes())
        expected[entry["offset"]:entry["offset"] + entry["size"]] = replacement.read_bytes()
        self.assertEqual(output.read_bytes(), expected)

    def test_malformed_both_endian_field_is_rejected(self):
        blob = bytearray(self.input.read_bytes())
        blob[16 * SECTOR + 84] ^= 1
        invalid = self.base / "invalid.iso"
        invalid.write_bytes(blob)
        with self.assertRaises(ValueError):
            iso.inventory(invalid)

    def test_output_aliases_to_image_or_loose_input_are_rejected(self):
        for target in [self.input, self.tree / "files/DATA/DATA.DAT"]:
            original = target.read_bytes()
            for kind, linker in [("symlink", os.symlink), ("hardlink", os.link)]:
                with self.subTest(target=target.name, kind=kind):
                    alias = self.base / f"{target.name}-{kind}"
                    linker(target, alias)
                    with self.assertRaises(ValueError):
                        iso.pack(self.tree, alias)
                    self.assertEqual(target.read_bytes(), original)

    def test_mismatched_path_tables_are_rejected(self):
        image = bytearray(self.input.read_bytes())
        # The second path-table entry names DATA; alter only its big-endian extent.
        struct.pack_into(">I", image, 19 * SECTOR + 12, 22)
        invalid = self.base / "mismatched-table.iso"
        invalid.write_bytes(image)
        with self.assertRaises(ValueError):
            iso.inventory(invalid)

    def test_cli_inventory_cannot_replace_its_input_image(self):
        original = self.input.read_bytes()
        command = subprocess.run([sys.executable, "-m", "tools.repack", "inventory",
                                  "--iso", str(self.input), "--out", str(self.input)],
                                 cwd=ROOT, capture_output=True, text=True)
        self.assertNotEqual(command.returncode, 0)
        self.assertEqual(self.input.read_bytes(), original)


if __name__ == "__main__":
    unittest.main()
