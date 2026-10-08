"""Exact native text-table inverse and bounded-edit controls."""
from __future__ import annotations

import json
import os
from pathlib import Path
import struct
import tempfile
import unittest

from tools.repack import tables
from tools.repack.test_audio import native_archive_leaves

ROOT = Path(__file__).resolve().parents[2]


def outer_fixture():
    strings = [b"synthetic", b"native\xE9\ntext"]
    directory = 16 + 16 * len(strings)
    markup = b"opaque markup!!!"
    text = bytearray(directory)
    struct.pack_into("<4I", text, 0, directory, len(strings), sum(len(s) + 1 for s in strings), 1)
    position = 0
    for i, s in enumerate(strings):
        struct.pack_into("<4I", text, 16 + 16 * i, position, position, len(s), len(s) + 1)
        position += len(s) + 1
    outer = bytearray(directory)
    struct.pack_into("<4I", outer, 0, directory, len(strings), len(markup), 16)
    struct.pack_into("<4I", outer, 16, 0, 17, 29, len(markup))
    return bytes(outer) + markup + bytes(text) + b"".join(s + b"\0" for s in strings)


class TableTests(unittest.TestCase):
    def setUp(self):
        (ROOT / "build/repack").mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="table-test-", dir=ROOT / "build/repack")
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.source = self.base / "source.bin"
        self.raw = outer_fixture() + b"tail padding"
        self.source.write_bytes(self.raw)
        self.tree = self.base / "tree"
        self.manifest = tables.unpack_table(self.source, self.tree, kind="outer")

    def test_noop_preserves_all_unknown_fields_and_bytes(self):
        output = self.base / "output.bin"
        result = tables.pack_table(self.tree, output)
        self.assertTrue(result["byte_identical"])
        self.assertEqual(output.read_bytes(), self.raw)

    def test_single_character_changes_only_expected_native_byte(self):
        path = self.tree / "table.json"
        text = json.loads(path.read_text())
        text["groups"][0]["lines"][0]["text"] = "Synthetic"
        path.write_text(json.dumps(text))
        output = self.base / "edited.bin"
        tables.pack_table(self.tree, output)
        expected = bytearray(self.raw)
        expected[self.manifest["layout"]["groups"][0]["lines"][0]["offset"]] = ord("S")
        self.assertEqual(output.read_bytes(), expected)
        recovered = self.base / "recovered"
        tables.unpack_table(output, recovered)
        self.assertEqual(json.loads((recovered / "table.json").read_text()), text)

    def test_resized_unicode_nul_and_added_lines_are_rejected(self):
        path = self.tree / "table.json"
        original = json.loads(path.read_text())
        for replacement in ["short", "synthetic!", "\u2603ynthetic", "\0ynthetic"]:
            edited = json.loads(json.dumps(original))
            edited["groups"][0]["lines"][0]["text"] = replacement
            path.write_text(json.dumps(edited))
            with self.assertRaises(ValueError):
                tables.pack_table(self.tree, self.base / "output.bin")
        original["groups"][0]["lines"].append({"line": 2, "text": "new"})
        path.write_text(json.dumps(original))
        with self.assertRaises(ValueError):
            tables.pack_table(self.tree, self.base / "output.bin")

    def test_input_alias_and_malformed_count_are_rejected(self):
        alias = self.base / "alias.bin"
        os.link(self.source, alias)
        with self.assertRaises(ValueError):
            tables.pack_table(self.tree, alias)
        malformed = bytearray(self.raw)
        struct.pack_into("<I", malformed, 4, 0xFFFFFFFF)
        self.source.write_bytes(malformed)
        with self.assertRaises(ValueError):
            tables.unpack_table(self.source, self.base / "bad", kind="outer")


@unittest.skipUnless(os.environ.get("EM_TEST_FULL") == "1", "set EM_TEST_FULL=1 for real text tables")
class FullTableTests(unittest.TestCase):
    def test_real_bank_and_bare_outer_roundtrip_and_edit(self):
        root = ROOT / "build/repack"
        root.mkdir(parents=True, exist_ok=True)
        records = []
        image = Path(os.environ.get("EM_TEST_ISO", ROOT / "Extermination-rebuilt.iso"))
        self.assertTrue(image.is_file(), "provide the user's disc through EM_TEST_ISO")
        cases = [("chunk00/f02_id02.bin", "bank", 271), ("chunk03/f14_id16.bin", "outer", 54)]
        leaves = dict(native_archive_leaves(image, {name for name, _, _ in cases}))
        for name, kind, count in cases:
            with tempfile.TemporaryDirectory(prefix="table-proof-", dir=root) as directory:
                base = Path(directory)
                source = base / "source.bin"
                source.write_bytes(leaves[name])
                manifest = tables.unpack_table(source, base / "tree", kind=kind)
                self.assertEqual(sum(g["line_count"] for g in manifest["layout"]["groups"]), count)
                output = base / "output.bin"
                result = tables.pack_table(base / "tree", output)
                self.assertEqual(output.read_bytes(), source.read_bytes())
                table_path = base / "tree/table.json"
                table = json.loads(table_path.read_text())
                row = next(line for group in table["groups"] for line in group["lines"] if line["text"])
                row["text"] = ("Z" if row["text"][0] != "Z" else "Y") + row["text"][1:]
                table_path.write_text(json.dumps(table))
                tables.pack_table(base / "tree", output)
                changed = output.read_bytes()
                self.assertEqual(sum(a != b for a, b in zip(changed, source.read_bytes())), 1)
                tables.unpack_table(output, base / "edited", kind=kind)
                self.assertEqual(json.loads((base / "edited/table.json").read_text()), table)
                records.append({"source_image": str(image.resolve()), "source": name, "lines": count,
                                "original_sha256": result["sha256"],
                                "byte_identical": True, "edited_changed_bytes": 1})
        (root / "tables-test-receipt.json").write_text(json.dumps(records, indent=2) + "\n")


if __name__ == "__main__":
    unittest.main()
