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
    markup = struct.pack("<4I", 2, 3, 3, 0xFEEDABCD)
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
        text["groups"][0]["lines"][0]["segments"][0] = "Syn"
        self.assertEqual(json.loads((recovered / "table.json").read_text()), text)

    def test_ambiguous_style_resize_unicode_nul_and_added_lines_are_rejected(self):
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

    def test_longer_and_shorter_plain_text_relocates_all_text_fields(self):
        path = self.tree / "table.json"
        original = json.loads(path.read_text())
        for replacement in ("x", "translated longer\ntext with extra words", ""):
            edited = json.loads(json.dumps(original))
            edited["groups"][0]["lines"][1]["text"] = replacement
            path.write_text(json.dumps(edited))
            output = self.base / "resized.bin"
            tables.pack_table(self.tree, output)
            data = output.read_bytes()
            text_at = struct.unpack_from("<I", data)[0] + struct.unpack_from("<I", data, 8)[0]
            first = struct.unpack_from("<4I", data, text_at + 16)
            second = struct.unpack_from("<4I", data, text_at + 32)
            self.assertEqual(first, (0, 0, 9, 10))
            self.assertEqual(second, (10, 10, len(replacement), len(replacement) + 1))
            self.assertEqual(struct.unpack_from("<I", data, text_at + 8)[0], 11 + len(replacement))
            self.assertTrue(data.endswith(b"tail padding"))
            self.assertEqual(data[:64], self.raw[:64])

    def test_style_segments_relocate_anchors_and_keep_other_record_words(self):
        path = self.tree / "table.json"
        edited = json.loads(path.read_text())
        edited["groups"][0]["lines"][0]["segments"] = ["translated prefix", " suffix"]
        path.write_text(json.dumps(edited))
        output = self.base / "styled.bin"
        tables.pack_table(self.tree, output)
        data = output.read_bytes()
        self.assertEqual(struct.unpack_from("<4I", data, 48), (2, 3, 17, 0xFEEDABCD))
        layout = tables._layout(data, "outer")
        a, b = layout["groups"][0]["lines"]
        self.assertEqual(data[a["offset"]:a["offset"] + a["size"]], b"translated prefix suffix")
        self.assertEqual(b["offset"], a["offset"] + a["size"] + 1)

    def test_engine_scratch_limits_and_conflicting_style_edits(self):
        path = self.tree / "table.json"
        original = json.loads(path.read_text())
        edited = json.loads(json.dumps(original))
        edited["groups"][0]["lines"][1]["text"] = "x" * 128
        path.write_text(json.dumps(edited))
        with self.assertRaisesRegex(ValueError, "func_001FC7B0"):
            tables.pack_table(self.tree, self.base / "bad.bin")
        edited["groups"][0]["lines"][1]["text"] = "x" * 127 + "\n" + "y" * 127
        path.write_text(json.dumps(edited))
        tables.pack_table(self.tree, self.base / "good.bin")
        edited = json.loads(json.dumps(original))
        edited["groups"][0]["lines"][0]["segments"] = ["x" * 128, "suffix"]
        path.write_text(json.dumps(edited))
        with self.assertRaisesRegex(ValueError, "func_001FE070"):
            tables.pack_table(self.tree, self.base / "bad.bin")
        edited["groups"][0]["lines"][0]["segments"] = ["prefix", "suffix"]
        edited["groups"][0]["lines"][0]["text"] = "different"
        path.write_text(json.dumps(edited))
        with self.assertRaisesRegex(ValueError, "conflicting"):
            tables.pack_table(self.tree, self.base / "bad.bin")
        edited = json.loads(json.dumps(original))
        edited["groups"][0]["lines"][0]["segments"] = ["x" * 64, "y" * 64]
        path.write_text(json.dumps(edited))
        with self.assertRaisesRegex(ValueError, "func_001FC7B0"):
            tables.pack_table(self.tree, self.base / "bad.bin")
        for segments in (["prefix", "trailing\x81"], ["lead\x81", "pair"]):
            edited["groups"][0]["lines"][0]["segments"] = segments
            path.write_text(json.dumps(edited))
            with self.assertRaisesRegex(ValueError, "0x81 escape"):
                tables.pack_table(self.tree, self.base / "bad.bin")

    def test_bank_relocates_every_group_and_preserves_padding(self):
        body = outer_fixture()
        pad = b"P" * (-len(body) % 16)
        size = len(body) + len(pad)
        header = bytearray(48)
        struct.pack_into("<4I", header, 0, 48, 2, size * 2, 16)
        for i in range(2):
            struct.pack_into("<4I", header, 16 + i * 16, i * size, i * size // 16, len(body), size)
        raw = bytes(header) + (body + pad) * 2 + b"opaque tail"
        source, tree = self.base / "bank.bin", self.base / "bank"
        source.write_bytes(raw)
        tables.unpack_table(source, tree, kind="bank")
        output = self.base / "bank-out.bin"
        tables.pack_table(tree, output)
        self.assertEqual(output.read_bytes(), raw)
        edited = json.loads((tree / "table.json").read_text())
        edited["groups"][0]["lines"][1]["text"] = "translation with more bytes"
        edited["groups"][1]["lines"][1]["text"] = "x"
        (tree / "table.json").write_text(json.dumps(edited))
        tables.pack_table(tree, output)
        result = output.read_bytes()
        groups = tables._layout(result, "bank")["groups"]
        first, second = struct.unpack_from("<4I", result, 16), struct.unpack_from("<4I", result, 32)
        self.assertEqual(second[0], first[3])
        self.assertEqual(second[1], second[0] // 16)
        self.assertEqual(struct.unpack_from("<I", result, 8)[0], first[3] + second[3])
        self.assertEqual(groups[1]["offset"], 48 + second[0])
        self.assertEqual(result[48 + first[2]:48 + first[2] + len(pad)], pad)
        self.assertTrue(result.endswith(b"opaque tail"))

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
    def test_real_translations_decode_through_existing_forward_extractor(self):
        from tools.export_ui import parse_outer
        root = ROOT / "build/repack"
        image = Path(os.environ.get("EM_TEST_ISO", ROOT / "Extermination-rebuilt.iso"))
        cases = [("chunk00/f02_id02.bin", "bank"), ("chunk03/f14_id16.bin", "outer")]
        leaves = dict(native_archive_leaves(image, {path for path, _ in cases}))
        for path, kind in cases:
            with tempfile.TemporaryDirectory(prefix="translation-proof-", dir=root) as directory:
                base = Path(directory)
                source, tree, output = base / "native.bin", base / "tree", base / "edited.bin"
                source.write_bytes(leaves[path])
                tables.unpack_table(source, tree, kind=kind)
                doc = json.loads((tree / "table.json").read_text())
                doc["groups"][0]["lines"][0]["text"] = "A longer translated message.\nWith another line."
                doc["groups"][0]["lines"][1]["text"] = "Short."
                if kind == "bank":
                    doc["groups"][3]["lines"][0]["segments"] = ["Translated prefix", " styled phrase ", "ending."]
                (tree / "table.json").write_text(json.dumps(doc))
                tables.pack_table(tree, output)
                rebuilt = output.read_bytes()
                layout = tables._layout(rebuilt, kind)
                for group in layout["groups"]:
                    decoded, _ = parse_outer(rebuilt, group["offset"], path)
                    expected = []
                    for row in doc["groups"][group["group"]]["lines"]:
                        text = ("".join(row["segments"]) if kind == "bank" and group["group"] == 3
                                and row["line"] == 0 else row["text"])
                        expected.append(text.encode("latin-1"))
                    self.assertEqual(decoded, expected)
                if kind == "bank":
                    group = layout["groups"][3]
                    self.assertEqual([pos for _, pos in tables._records(rebuilt, group, group["lines"][0])], [17, 32])

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
