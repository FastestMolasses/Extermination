"""Quick synthetic checks for source-build provenance and boot packaging."""
from __future__ import annotations

from pathlib import Path
import json
import os
import struct
import subprocess
import tempfile
import unittest

from tools.repack import source_build, source_compile


def elf(payload: bytes, offset: int, tail: bytes = b"") -> bytes:
    raw = bytearray(offset)
    raw[:7] = b"\x7fELF\x01\x01\x01"
    struct.pack_into("<I", raw, 28, 52)
    struct.pack_into("<HH", raw, 42, 32, 1)
    struct.pack_into("<8I", raw, 52, 1, offset, 0x100000, 0x100000,
                     len(payload), len(payload) + 64, 7, 16)
    return bytes(raw) + payload + tail


class SourceBuildTests(unittest.TestCase):
    def setUp(self):
        root = source_build.ROOT / "build" / "repack"
        root.mkdir(parents=True, exist_ok=True)
        self.temp = tempfile.TemporaryDirectory(prefix="source-build-test-", dir=root)
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.original, self.linked, self.output = (self.root / name for name in ("original.elf", "linked.elf", "packaged.elf"))

    def test_packaging_uses_fresh_load_and_preserves_only_original_envelope(self):
        payload = bytes(range(128))
        self.original.write_bytes(elf(payload, 128, b"original nonloaded metadata"))
        self.linked.write_bytes(elf(payload, 256, b"new compiler metadata"))
        result = source_build.package_boot(self.original, self.linked, self.output)
        self.assertEqual(self.output.read_bytes(), self.original.read_bytes())
        self.assertEqual(result["linked_load_bytes"], len(payload))
        self.assertEqual(result["preserved_envelope_bytes"], 128 + len(b"original nonloaded metadata"))
        self.assertTrue(result["byte_identical"])

    def test_mismatching_link_fails_without_original_fallback(self):
        self.original.write_bytes(elf(b"original payload", 128))
        self.linked.write_bytes(elf(b"modified payload", 128))
        with self.assertRaisesRegex(ValueError, "no fallback copy"):
            source_build.package_boot(self.original, self.linked, self.output)
        self.assertFalse(self.output.exists())

    def test_short_link_does_not_pass_prefix_only_verification(self):
        self.original.write_bytes(elf(b"same prefix plus original tail", 128))
        self.linked.write_bytes(elf(b"same prefix", 128))
        with self.assertRaisesRegex(ValueError, "address/size"):
            source_build.package_boot(self.original, self.linked, self.output)
        self.assertFalse(self.output.exists())

    def test_measured_zero_linker_padding_is_separate_from_original_envelope(self):
        payload = b"fresh matching load"
        self.original.write_bytes(elf(payload, 128, b"preserved metadata"))
        self.linked.write_bytes(elf(payload + bytes(128), 256))
        result = source_build.package_boot(self.original, self.linked, self.output)
        self.assertEqual(self.output.read_bytes(), self.original.read_bytes())
        self.assertEqual(result["discarded_linker_zero_padding_bytes"], 128)

    def test_nonzero_or_excess_linked_tail_cannot_be_discarded(self):
        payload = b"matching prefix"
        self.original.write_bytes(elf(payload, 128))
        for tail in (bytes(127) + b"X", bytes(256)):
            with self.subTest(length=len(tail)):
                self.linked.write_bytes(elf(payload + tail, 256))
                with self.assertRaisesRegex(ValueError, "unsupported data"):
                    source_build.package_boot(self.original, self.linked, self.output)
                self.assertFalse(self.output.exists())

    def test_input_alias_is_rejected(self):
        self.original.write_bytes(elf(b"payload", 128))
        self.linked.write_bytes(elf(b"payload", 128))
        alias = self.root / "alias.elf"
        alias.hardlink_to(self.original)
        with self.assertRaisesRegex(ValueError, "aliases an input"):
            source_build.package_boot(self.original, self.linked, alias)

    def test_shared_lock_is_removed_after_build_failure(self):
        lock = self.root / "build.lock"
        with self.assertRaisesRegex(RuntimeError, "compiler failure"):
            with source_build.build_lock(lock, progress=lambda message: None):
                self.assertTrue(lock.is_dir())
                raise RuntimeError("compiler failure")
        self.assertFalse(lock.exists())

    def test_missing_compiler_object_cannot_be_masked_by_filler(self):
        with self.assertRaisesRegex(ValueError, "cannot fall back silently"):
            source_build._require_objects([self.root / "missing.o"], "fixture")

    def test_parallel_compile_reports_earlier_failure_after_later_success(self):
        self.assertEqual(source_compile.checked_parallel([("early", 2), ("late", 0)],
                                                          lambda job: job, 2), ["early"])

    def test_renamed_alias_requires_old_script_and_fresh_boundary_agreement(self):
        name = "func_overlay_AREA04_00823580"
        script = name + " = 0x00823580;\n"
        self.assertEqual(source_build._corroborated_aliases("AREA04", {name}, script, {0x823580}, {"renamed"}),
                         {name: 0x823580})
        for old, boundaries in (("", {0x823580}), (script, {0x823540}),
                                (name + " = 0x00823540;", {0x823580})):
            with self.subTest(old=old, boundaries=boundaries):
                self.assertEqual(source_build._corroborated_aliases("AREA04", {name}, old, boundaries, set()), {})

    def test_existing_or_foreign_overlay_symbol_is_not_redefined(self):
        name = "func_overlay_AREA04_00823580"
        script = name + " = 0x00823580;"
        self.assertEqual(source_build._corroborated_aliases("AREA04", {name}, script, {0x823580}, {name}), {})
        self.assertEqual(source_build._corroborated_aliases("AREA03", {name}, script, {0x823580}, set()), {})

    def test_compile_checkpoint_changes_with_compiler_product(self):
        obj = self.root / "build/obj/fresh.o"
        obj.parent.mkdir(parents=True)
        obj.write_bytes(b"first fresh object")
        before = source_build._compile_checkpoint(self.root)
        obj.write_bytes(b"changed object")
        self.assertNotEqual(source_build._compile_checkpoint(self.root), before)

    def test_resume_rejects_failed_compile_or_incomplete_provenance(self):
        for name in ("boot-compile", "overlay-compile", "AREA04-link"):
            with self.subTest(stage=name):
                receipt = dict(status="failed", stages=[dict(name=name, exit_code=1)])
                with self.assertRaisesRegex(ValueError, "resume"):
                    source_build._validate_resume(self.root, self.root, self.root, receipt)


@unittest.skipUnless(os.environ.get("EM_TEST_FULL") == "1" and os.environ.get("EM_TEST_SOURCE_BUILD") == "1",
                     "fresh compiler/container build requires EM_TEST_FULL=1 EM_TEST_SOURCE_BUILD=1")
class FullSourceDiscTests(unittest.TestCase):
    def test_one_command_fresh_source_build_equals_original_image(self):
        root = source_build.ROOT
        tree = Path(os.environ.get("EM_REPACK_TREE", str(root / "build/repack/loose"))).resolve()
        manifest = tree / "iso/manifest.json"
        python = root / ".venv/bin/python"
        if not manifest.is_file() or not python.is_file():
            self.skipTest("requires the user's unpack-disc tree and installed compiler environment")
        reference = json.loads(manifest.read_text())
        with tempfile.TemporaryDirectory(prefix="fresh-source-proof-", dir=root / "build/repack") as name:
            temporary = Path(name)
            output, log = temporary / "output", temporary / "command.log"
            with log.open("wb") as stream:
                result = subprocess.run([str(python), "-m", "tools.repack", "build-disc", "--tree", str(tree),
                                         "--out", str(output), "--require-original"], cwd=root,
                                        stdout=stream, stderr=subprocess.STDOUT)
            if result.returncode:
                failure = root / "build/repack/source-test-failure.log"
                failure.write_bytes(log.read_bytes())
                self.fail(f"fresh source build failed; local command log retained at {failure}")
            receipt = json.loads((output / "build-disc.json").read_text())
            original_image = Path(reference["source_image"])
            self.assertTrue(original_image.is_file(), "the user-owned original image is needed for the final hash proof")
            original_sha = source_build.sha256_file(original_image)
            self.assertEqual(receipt["source"]["status"], "complete")
            self.assertEqual(len(receipt["source"]["overlays"]), 19)
            self.assertEqual(original_sha, reference["image_sha256"])
            self.assertEqual(receipt["iso"]["image_sha256"], original_sha)
            self.assertEqual(source_build.sha256_file(output / "Extermination.iso"), original_sha)


@unittest.skipUnless(os.environ.get("EM_TEST_FULL") == "1" and os.environ.get("EM_SOURCE_BUILD_OUTPUT"),
                     "source receipt verification requires EM_TEST_FULL=1 EM_SOURCE_BUILD_OUTPUT=<build-disc output>")
class CompletedSourceDiscTests(unittest.TestCase):
    def test_completed_fresh_command_matches_actual_original_iso(self):
        root = source_build.ROOT
        output = Path(os.environ["EM_SOURCE_BUILD_OUTPUT"]).resolve()
        tree = Path(os.environ.get("EM_REPACK_TREE", str(root / "build/repack/loose"))).resolve()
        reference = json.loads((tree / "iso/manifest.json").read_text())
        original = Path(reference["source_image"])
        self.assertTrue(original.is_file(), "requires the user-owned original ISO")
        receipt = json.loads((output / "build-disc.json").read_text())
        source = receipt["source"]
        self.assertEqual(source["status"], "complete")
        self.assertGreater(source["fresh_boot_objects"], 0)
        self.assertTrue(all(stage["exit_code"] == 0 for stage in source["stages"]))
        self.assertIn("boot-compile", {stage["name"] for stage in source["stages"]})
        self.assertIn("overlay-compile", {stage["name"] for stage in source["stages"]})
        self.assertEqual(len(source["overlays"]), 19)
        self.assertTrue(all(row["byte_identical"] for row in source["overlays"]))
        self.assertEqual(source_build.sha256_file(output / "Extermination.iso"), source_build.sha256_file(original))
        self.assertEqual(receipt["iso"]["image_sha256"], reference["image_sha256"])


if __name__ == "__main__":
    unittest.main()
