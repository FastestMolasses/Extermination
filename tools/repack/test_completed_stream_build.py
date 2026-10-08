"""Read-only oracle for an already completed fresh build with stream edits.

Set EM_STREAM_BUILD_OUTPUT to its build-disc output directory and EM_TEST_ISO
to the original BYO disc. This never builds, extracts a tree, or runs PCSX2.
Only a small hash receipt is written under build/repack/.
"""
from __future__ import annotations

import json
import os
from pathlib import Path
import tempfile
import unittest

from tools import audio_export
from tools.repack import archive, iso
from tools.repack.test_full import udf_hashes


def _read_file(image: Path, entry: dict) -> bytes:
    with image.open("rb") as source:
        source.seek(entry["offset"])
        raw = source.read(entry["size"])
    if len(raw) != entry["size"]:
        raise AssertionError("truncated ISO file")
    return raw


@unittest.skipUnless(os.environ.get("EM_STREAM_BUILD_OUTPUT"),
                     "set EM_STREAM_BUILD_OUTPUT for completed stream build verification")
class CompletedStreamBuildTests(unittest.TestCase):
    def test_both_namespaces_cue_metadata_and_unchanged_files(self):
        self.assertTrue(os.environ.get("EM_TEST_ISO"), "EM_TEST_ISO must identify the original BYO disc")
        original_image = Path(os.environ["EM_TEST_ISO"]).resolve()
        build = Path(os.environ["EM_STREAM_BUILD_OUTPUT"]).resolve()
        receipt_path = build / "build-disc.json"
        receipt = json.loads(receipt_path.read_text())
        image = build / "Extermination.iso"
        self.assertEqual(Path(receipt["iso"]["image_path"]).resolve(), image)
        self.assertEqual(receipt["source"]["status"], "complete")
        patch = receipt["cue_patch"]
        self.assertIsInstance(patch, dict, "build has no stream cue patch")
        self.assertTrue(patch["bundles"], "build has no stream bundle")

        print("Completed stream proof: hashing original and finished ISO namespaces", flush=True)
        baseline, finished = iso.inventory(original_image), iso.inventory(image)
        old = {row["path"]: row for row in baseline["files"]}
        new = {row["path"]: row for row in finished["files"]}
        self.assertEqual(len(old), 43)
        self.assertEqual(set(new), set(old))
        self.assertEqual(finished["image_sha256"], receipt["iso"]["image_sha256"])
        hashes = {name: row["sha256"] for name, row in new.items()}
        print("Completed stream proof: independently reading all 43 UDF files", flush=True)
        self.assertEqual(udf_hashes(image, list(new)), hashes)

        expected = {"DATA/DATA.DAT": receipt["archive"]["data_sha256"],
                    "DATA/INDEX.IDX": receipt["archive"]["index_sha256"],
                    "SCUS_971.12": patch["sha256"]}
        bundles = {}
        for row in patch["bundles"]:
            self.assertIn(row["kind"], ("music", "voice"))
            self.assertNotIn(row["kind"], bundles)
            name = "STREAM/" + row["kind"].upper() + ".DAT"
            expected[name] = row["stream_sha256"]
            self.assertEqual(new[name]["size"], row["stream_size"])
            native = Path(patch["overrides"][name])
            self.assertEqual(archive.sha256_file(native), row["stream_sha256"])
            metadata = json.loads((native.parent / "cue-edits.json").read_text())
            self.assertEqual(metadata["kind"], row["kind"])
            self.assertEqual(metadata["stream_sha256"], row["stream_sha256"])
            self.assertEqual(metadata["original_stream_sha256"], old[name]["sha256"])
            self.assertEqual(metadata["original_elf_sha256"], old["SCUS_971.12"]["sha256"])
            bundles[row["kind"]] = metadata
        self.assertEqual(set(patch["overrides"]), {"STREAM/" + kind.upper() + ".DAT" for kind in bundles})
        for name, digest in hashes.items():
            self.assertEqual(digest, expected.get(name, old[name]["sha256"]), name)

        boot = _read_file(image, new["SCUS_971.12"])
        original_boot = _read_file(original_image, old["SCUS_971.12"])
        patched_path = Path(patch["output"])
        self.assertEqual(archive.sha256_file(patched_path), patch["sha256"])
        self.assertEqual(boot, patched_path.read_bytes())
        self.assertEqual(receipt["source"]["boot"]["sha256"], old["SCUS_971.12"]["sha256"])
        self.assertEqual(patch["original_elf_sha256"], old["SCUS_971.12"]["sha256"])
        self.assertEqual(len(boot), len(original_boot))

        # Existing forward extractor is independent of streams._rows and its
        # patch helper. Its ELF mapping and decoded rows must match the bundle.
        allowed, actual_rows = set(), []
        for kind, metadata in bundles.items():
            music = kind == "music"
            actual = [list(row) for row in audio_export.read_cue_table(patched_path, music)]
            self.assertEqual(actual, metadata["rows"], kind)
            vram, count = audio_export.MUSIC_CUE_TABLE if music else audio_export.VOICE_CUE_TABLE
            # SCUS-97112's canonical PT_LOAD envelope is already byte-checked
            # by source_build. Check its fixed profile fields independently.
            table = vram - 0x100000 + 0x300
            self.assertEqual(actual[0], [0, 0, 0, 0])
            cursor = 0
            for cue, row in enumerate(actual[1:], 1):
                self.assertEqual(row[:2], [cursor // 2048, cursor])
                self.assertGreater(row[2], 0)
                self.assertEqual(row[2] % 2048, 0)
                self.assertEqual(boot[table + cue * 16 + 12:table + (cue + 1) * 16],
                                 original_boot[table + cue * 16 + 12:table + (cue + 1) * 16])
                cursor += row[2]
            self.assertEqual(cursor, metadata["stream_size"])
            for cue in range(1, count):
                offset = table + cue * 16
                allowed.update(range(offset, offset + 12))
                if boot[offset:offset + 12] != original_boot[offset:offset + 12]:
                    actual_rows.append({"kind": kind, "cue": cue, "elf_file_offset": offset})
        self.assertEqual(actual_rows, patch["changed_rows"])
        self.assertTrue(actual_rows, "completed build did not change any cue rows")
        self.assertTrue(all(offset in allowed for offset, (a, b) in enumerate(zip(original_boot, boot)) if a != b),
                        "fresh boot differs outside the permitted start/length fields")

        result = {"schema": "extermination-completed-stream-test-v1", "build": str(build),
                  "build_receipt_sha256": archive.sha256_file(receipt_path),
                  "source_iso": str(original_image), "source_iso_sha256": baseline["image_sha256"],
                  "finished_iso_sha256": finished["image_sha256"], "iso_and_udf_file_count": len(new),
                  "udf_namespace_matches_iso": True, "cue_rows_changed": actual_rows,
                  "boot_changes_confined_to_cue_rows": True,
                  "files": [{"path": name, "original_sha256": old[name]["sha256"],
                             "iso_sha256": digest, "udf_sha256": digest,
                             "changed": old[name]["sha256"] != digest} for name, digest in hashes.items()]}
        destination = archive.safe_output(archive.ROOT / "build/repack/completed-stream-test-receipt.json")
        # Atomic replace avoids writing through a pre-existing hardlink.
        fd, temporary = tempfile.mkstemp(prefix=".stream-test-", dir=destination.parent)
        try:
            with os.fdopen(fd, "w") as output:
                json.dump(result, output, indent=2)
                output.write("\n")
            os.replace(temporary, destination)
        finally:
            if os.path.exists(temporary):
                os.unlink(temporary)
        print(f"Completed stream proof passed: {destination}", flush=True)


if __name__ == "__main__":
    unittest.main()
