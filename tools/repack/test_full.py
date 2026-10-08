"""Slow BYO-disc integration proofs, enabled only with EM_TEST_FULL=1."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest

from tools.repack import archive, iso

ROOT = Path(__file__).resolve().parents[2]
BUFFER = 8 << 20


def digest(path: Path, offset=0, size=None) -> str:
    h = hashlib.sha256()
    with path.open("rb") as source:
        source.seek(offset)
        remaining = path.stat().st_size - offset if size is None else size
        while remaining:
            block = source.read(min(remaining, BUFFER))
            if not block:
                raise AssertionError("input truncated while hashing")
            h.update(block)
            remaining -= len(block)
    return h.hexdigest()


def loose_hashes(tree: Path, manifest: dict) -> dict[str, str]:
    paths = [entry["path"] for region in manifest["regions"] for entry in region["files"]]
    paths += [entry["path"] for entry in manifest["data_layout"] if entry["kind"] == "gap"]
    return {path: digest(tree / path) for path in paths}


def one_difference(test: unittest.TestCase, before: Path, after: Path, expected: int) -> None:
    test.assertEqual(before.stat().st_size, after.stat().st_size)
    found = []
    offset = 0
    with before.open("rb") as original, after.open("rb") as edited:
        while left := original.read(BUFFER):
            right = edited.read(len(left))
            if left != right:
                found.extend(offset + i for i, (a, b) in enumerate(zip(left, right)) if a != b)
                test.assertLessEqual(len(found), 1, "mutation leaked outside the selected byte")
            offset += len(left)
    test.assertEqual(found, [expected])


def udf_hashes(path: Path, names: list[str]) -> dict[str, str]:
    """Read UDF extents with pycdlib, independently of the repacker's parser."""
    import pycdlib

    class HashWriter:
        def __init__(self):
            self.hash = hashlib.sha256()

        def write(self, data):
            self.hash.update(data)
            return len(data)

    image = pycdlib.PyCdlib()
    image.open(str(path))
    try:
        if not image.has_udf():
            return {}
        udf_paths = {}
        for directory, _, files in image.walk(udf_path="/"):
            for name in files:
                actual = directory.rstrip("/") + "/" + name
                normalized = actual.lstrip("/").upper()
                if normalized in udf_paths:
                    raise AssertionError("ambiguous case-folded UDF filename")
                udf_paths[normalized] = actual
        result = {}
        for name in names:
            output = HashWriter()
            image.get_file_from_iso_fp(output, udf_path=udf_paths[name.upper()])
            result[name] = output.hash.hexdigest()
        return result
    finally:
        image.close()


@unittest.skipUnless(os.environ.get("EM_TEST_FULL") == "1", "set EM_TEST_FULL=1 for BYO-disc integration")
class FullDiscTests(unittest.TestCase):
    def test_disc_archive_edits_and_exact_reconstruction(self):
        source = Path(os.environ.get("EM_TEST_ISO", ROOT / "Extermination-rebuilt.iso"))
        self.assertTrue(source.is_file(), "supply the legal disc as Extermination-rebuilt.iso or EM_TEST_ISO")
        output_root = ROOT / "build/repack"
        output_root.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="full-test-", dir=output_root) as temporary:
            base = Path(temporary)
            loose_tree = base / "loose"
            disc_tree, asset_tree = loose_tree / "iso", loose_tree / "archive"
            print("Full proof: unpacking and hashing every ISO file", flush=True)
            subprocess.run([sys.executable, "-m", "tools.repack", "unpack-disc",
                            "--iso", str(source), "--out", str(loose_tree)],
                           cwd=ROOT, check=True, capture_output=True, text=True)
            disc = json.loads((disc_tree / "manifest.json").read_text())
            original_data = disc_tree / "files/DATA/DATA.DAT"
            original_index = disc_tree / "files/DATA/INDEX.IDX"
            manifest = json.loads((asset_tree / "manifest.json").read_text())
            original_loose = loose_hashes(asset_tree, manifest)
            print("Full proof: rebuilding the complete ISO with the documented pack-disc command", flush=True)
            packed_dir = base / "packed"
            command = subprocess.run([sys.executable, "-m", "tools.repack", "pack-disc",
                                      "--tree", str(loose_tree), "--out", str(packed_dir)],
                                     cwd=ROOT, check=True, capture_output=True, text=True)
            result = json.loads(command.stdout)["iso"]
            packed_data, packed_index = packed_dir / "DATA.DAT", packed_dir / "INDEX.IDX"
            archive_hashes = {}
            for name, original, packed in [("DATA.DAT", original_data, packed_data),
                                           ("INDEX.IDX", original_index, packed_index)]:
                expected, actual = digest(original), digest(packed)
                self.assertEqual(actual, expected, name)
                archive_hashes[name] = {"original_sha256": expected, "packed_sha256": actual}
            checked_tree = base / "checked"
            checked = archive.unpack_archive(packed_data, packed_index, checked_tree)
            roundtrip_loose = loose_hashes(checked_tree, checked)
            self.assertEqual(original_loose, roundtrip_loose)
            shutil.rmtree(checked_tree)
            overrides = {"DATA/DATA.DAT": packed_data, "DATA/INDEX.IDX": packed_index}
            output_iso = packed_dir / "Extermination.iso"
            self.assertTrue(result["unchanged"])
            self.assertEqual(digest(output_iso), disc["image_sha256"])
            file_hashes = []
            for entry in disc["files"]:
                actual = digest(output_iso, entry["offset"], entry["size"])
                expected = digest(source, entry["offset"], entry["size"])
                self.assertEqual(actual, expected, entry["path"])
                file_hashes.append({"path": entry["path"], "size": entry["size"],
                                    "original_sha256": expected, "packed_sha256": actual})
            output_iso.unlink()
            receipt = {
                "schema": "extermination-repack-test-v1",
                "source_image": str(source.resolve()),
                "iso": {"original_sha256": disc["image_sha256"],
                        "packed_sha256": result["image_sha256"], "size": disc["image_size"]},
                "archive": archive_hashes,
                "files": file_hashes,
                "loose_files": [{"path": path, "original_sha256": sha,
                                 "packed_sha256": roundtrip_loose[path]}
                                for path, sha in sorted(original_loose.items())],
                "resizes": [],
            }
            expected_files = {f["path"]: f["original_sha256"] for f in file_hashes}
            original_udf = udf_hashes(source, list(expected_files))
            self.assertEqual(original_udf, expected_files)
            for file in file_hashes:
                file["original_udf_sha256"] = original_udf[file["path"]]
                file["packed_udf_sha256"] = original_udf[file["path"]]
            receipt["original_udf_namespace_matches_iso"] = True
            candidates = [(region, entry) for region in manifest["regions"]
                          for entry in region["files"] if entry["kind"] == "resident"
                          and region["nested"] is not None and entry["size"] >= 0x1000]
            self.assertTrue(candidates, "disc has no nested resident file suitable for edit proof")
            region, entry = candidates[0]
            path = asset_tree / entry["path"]
            payload = path.read_bytes()
            print("Full proof: checking one-byte negative control in DATA and the full ISO", flush=True)
            local_offset = len(payload) // 2
            edited = bytearray(payload)
            edited[local_offset] ^= 0x80
            path.write_bytes(edited)
            archive.pack_archive(asset_tree, packed_data, packed_index)
            data_offset = region["offset"] + entry["offset"] + local_offset
            one_difference(self, original_data, packed_data, data_offset)
            self.assertEqual(digest(packed_index), archive_hashes["INDEX.IDX"]["original_sha256"])
            iso.pack(disc_tree, output_iso, overrides=overrides)
            disc_data = next(f for f in disc["files"] if f["path"] == "DATA/DATA.DAT")
            image_offset = disc_data["offset"] + data_offset
            one_difference(self, source, output_iso, image_offset)
            output_iso.unlink()
            receipt["one_byte_control"] = {"path": entry["path"], "data_offset": data_offset,
                                           "iso_offset": image_offset, "changed_byte_count": 1}
            for name, content in [("grown", payload + b"\xA5" * 0x800),
                                  ("shrunk", payload[:-0x800])]:
                print(f"Full proof: {name} resident file, archive and ISO re-layout", flush=True)
                path.write_bytes(content)
                archive.pack_archive(asset_tree, packed_data, packed_index)
                changed = archive.unpack_archive(packed_data, packed_index, checked_tree)
                expected = dict(original_loose)
                expected[entry["path"]] = hashlib.sha256(content).hexdigest()
                self.assertEqual(loose_hashes(checked_tree, changed), expected)
                shutil.rmtree(checked_tree)
                result = iso.pack(disc_tree, output_iso, overrides=overrides)
                actual_files = {f["path"]: f["sha256"] for f in result["files"]}
                expected_files = {f["path"]: f["original_sha256"] for f in file_hashes}
                expected_files["DATA/DATA.DAT"] = digest(packed_data)
                expected_files["DATA/INDEX.IDX"] = digest(packed_index)
                self.assertEqual(actual_files, expected_files)
                self.assertEqual(udf_hashes(output_iso, list(expected_files)), expected_files)
                receipt["resizes"].append({"case": name, "path": entry["path"],
                                           "delta": len(content) - len(payload),
                                           "edited_sha256": expected[entry["path"]],
                                           "all_other_loose_files_unchanged": True,
                                           "all_other_iso_files_unchanged": True,
                                           "udf_namespace_matches_iso": True,
                                           "packed_iso_sha256": result["image_sha256"]})
                output_iso.unlink()
            path.write_bytes(payload)
            receipt["quick_suite"] = "Run python -m unittest discover -s tools/repack -p 'test_*.py'"
            receipt_path = output_root / "test-receipt.json"
            receipt_path.write_text(json.dumps(receipt, indent=2) + "\n")
            print(f"Full proof passed; hashes and controls: {receipt_path}", flush=True)


if __name__ == "__main__":
    unittest.main()
