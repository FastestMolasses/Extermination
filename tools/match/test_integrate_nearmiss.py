#!/usr/bin/env python3
"""Host-only integrator regression tests; no game data or compiler required."""
import contextlib
import io
import json
import os
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
import types
import unittest
from unittest.mock import patch


SCRIPT = Path(__file__).with_name("integrate_nearmiss.py")
FUNC = "func_00100000"
CANDIDATE = ("// COMPILER: mwcc233\n// CFLAGS: -O4,p -sdatathreshold 0\n"
             "int func_00100000(void) { return 1; }\n")
ORIGINAL = b"// NEARMISS original\r\nint func_00100000(void) { return 0; }\r\n"


def synthetic_elf(text_size=4):
    """Construct an ELF32 with synthetic .text and .shstrtab sections."""
    data = bytearray(52)
    data[:7] = b"\x7fELF\x01\x01\x01"
    text_offset = len(data)
    data.extend(b"\0" * text_size)
    strings = b"\0.text\0.shstrtab\0"
    string_offset = len(data)
    data.extend(strings)
    data.extend(b"\0" * (-len(data) % 4))
    section_offset = len(data)
    data.extend(b"\0" * 40)
    data.extend(struct.pack("<10I", 1, 1, 6, 0, text_offset, text_size, 0, 0, 4, 0))
    data.extend(struct.pack("<10I", 7, 3, 0, 0, string_offset, len(strings), 0, 0, 1, 0))
    struct.pack_into("<I", data, 0x20, section_offset)
    struct.pack_into("<3H", data, 0x2E, 40, 3, 2)
    return bytes(data)


class IntegratorTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="integrator-test-")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        for name in ("src", "build/obj", "build/expected", "docs",
                     "build/asm/matchings/main/code"):
            (self.root / name).mkdir(parents=True)
        self.src = self.root / "src" / (FUNC + ".c")
        self.src.write_bytes(ORIGINAL)
        self.obj = self.root / "build/obj" / (FUNC + ".o")
        self.expected = self.root / "build/expected" / (FUNC + ".o")
        self.obj.write_bytes(synthetic_elf())
        self.expected.write_bytes(synthetic_elf())
        # Synthetic metadata only; no original assembly or assets.
        (self.root / "build/asm/matchings/main/code" / (FUNC + ".s")).write_text(
            "nonmatching func_00100000, 0x4\n")
        self.calls = []
        self.output = io.StringIO()

    def execute(self, *, mode="success", percent=25.0, payload=None,
                objdiff_status=0, candidate_size=4, expected_size=4):
        row = {"func": FUNC, "c_source": CANDIDATE, "wall": "synthetic test"}
        wave = self.root / "wave.json"
        wave.write_text(json.dumps(payload if payload is not None else {"results": [row]}))

        # Keep the production script's top-level control flow. Redirect its
        # hardcoded root and report files before execution in this fixture.
        source = SCRIPT.read_text()
        source, count = re.subn(r'^ROOT = .+$', "ROOT = " + repr(str(self.root)),
                                source, count=1, flags=re.MULTILINE)
        self.assertEqual(count, 1)
        for name in ("wave_pass.txt", "wave_nearmiss.txt"):
            source = source.replace('"/tmp/' + name + '"', repr(str(self.root / name)))
        build = types.ModuleType("build")
        build.normalize_asm = lambda name: None

        def fake_run(command, **kwargs):
            self.calls.append(command)
            if command[0] == "container":
                self.assertTrue(kwargs.get("check"), "compiler errors must propagate")
                self.assertTrue(command[-1].startswith("set -e; "))
                if len(self.calls) == 1:
                    self.assertFalse(self.obj.exists(), "stale candidate must be removed")
                    self.assertFalse(self.expected.exists(), "stale target must be removed")
                if mode == "build_failure":
                    self.obj.write_bytes(b"partial failed output")
                    raise subprocess.CalledProcessError(1, command)
                if mode != "missing_output":
                    for directory, name in re.findall(r'-o build/(obj|expected)/(\w+)\.o', command[-1]):
                        size = candidate_size if directory == "obj" else expected_size
                        (self.root / "build" / directory / (name + ".o")).write_bytes(synthetic_elf(size))
                return subprocess.CompletedProcess(command, 0, "", "")
            if command[0] == "tools/bin/objdiff-cli":
                if mode == "objdiff_interrupt" or (mode == "objdiff_interrupt_second" and command[6] != FUNC):
                    raise KeyboardInterrupt()
                result = {"left": {"symbols": [{"name": command[6], "match_percent": percent}]}}
                return subprocess.CompletedProcess(command, objdiff_status, json.dumps(result), "")
            if command[1:2] == ["tools/decomp/inject_relocs.py"]:
                if mode == "inject_interrupt":
                    raise KeyboardInterrupt()
                if mode == "inject_failure":
                    return subprocess.CompletedProcess(command, 1, "", "failed")
                return subprocess.CompletedProcess(command, 0, "", "")
            self.fail("unexpected subprocess: " + repr(command))

        old_cwd, old_path = os.getcwd(), sys.path[:]
        try:
            with patch.object(sys, "argv", [str(SCRIPT), str(wave)]), \
                 patch.dict(sys.modules, {"build": build}), \
                 patch("subprocess.run", side_effect=fake_run), \
                 contextlib.redirect_stdout(self.output):
                exec(compile(source, str(SCRIPT), "exec"),
                     {"__name__": "__main__", "__file__": str(SCRIPT)})
        finally:
            os.chdir(old_cwd)
            sys.path[:] = old_path

    def assert_restored(self):
        self.assertEqual(self.src.read_bytes(), ORIGINAL)
        self.assertFalse(self.obj.exists())
        self.assertFalse(self.expected.exists())
        self.assertNotIn("KEEP", self.output.getvalue())

    def test_failed_compile_cannot_promote_stale_object(self):
        with self.assertRaises(subprocess.CalledProcessError):
            self.execute(mode="build_failure", percent=100.0)
        self.assert_restored()
        self.assertFalse(any(c[0] == "tools/bin/objdiff-cli" for c in self.calls))

    def test_success_without_fresh_output_is_rejected(self):
        with self.assertRaisesRegex(RuntimeError, "missing fresh"):
            self.execute(mode="missing_output", percent=100.0)
        self.assert_restored()

    def test_existing_nearmiss_restores_exact_bytes(self):
        self.execute(percent=25.0)
        self.assert_restored()
        self.assertIn("REVERT", self.output.getvalue())
        self.assertNotIn("FAILED", self.output.getvalue())

    def test_duplicate_candidates_are_rejected_before_writes(self):
        row = {"func": FUNC, "c_source": CANDIDATE}
        with self.assertRaisesRegex(SystemExit, "duplicate"):
            self.execute(payload={"results": [row, row]})
        self.assertEqual(self.src.read_bytes(), ORIGINAL)
        self.assertEqual(self.obj.read_bytes(), synthetic_elf())
        self.assertEqual(self.calls, [])

    def test_plain_list_input_can_record_nearmiss(self):
        self.execute(payload=[{"func": FUNC, "c_source": CANDIDATE}], percent=75.0)
        self.assertTrue(self.src.read_text().startswith("// NEARMISS"))
        self.assertIn("objdiff 75.00%", self.src.read_text())
        self.assertFalse(self.obj.exists())

    def test_failed_objdiff_cannot_report_match(self):
        self.execute(percent=100.0, objdiff_status=1)
        self.assert_restored()

    def test_interrupted_objdiff_restores_original(self):
        with self.assertRaises(KeyboardInterrupt):
            self.execute(mode="objdiff_interrupt", percent=100.0)
        self.assert_restored()
        self.assertFalse((self.root / "docs/NEARMISS.md").exists())

    def test_interrupted_injector_restores_original(self):
        with self.assertRaises(KeyboardInterrupt):
            self.execute(mode="inject_interrupt", percent=100.0)
        self.assert_restored()

    def test_later_interrupt_also_restores_prior_kept_candidate(self):
        second = "func_00100004"
        second_src = self.root / "src" / (second + ".c")
        second_original = ORIGINAL.replace(FUNC.encode(), second.encode())
        second_src.write_bytes(second_original)
        payload = [{"func": FUNC, "c_source": CANDIDATE},
                   {"func": second, "c_source": CANDIDATE.replace(FUNC, second)}]
        with self.assertRaises(KeyboardInterrupt):
            self.execute(mode="objdiff_interrupt_second", percent=100.0, payload=payload)
        self.assertEqual(self.src.read_bytes(), ORIGINAL)
        self.assertEqual(second_src.read_bytes(), second_original)
        for name in (FUNC, second):
            for directory in ("obj", "expected"):
                self.assertFalse((self.root / "build" / directory / (name + ".o")).exists())
        self.assertIn("validation failed; restored 2", self.output.getvalue())
        self.assertFalse((self.root / "wave_pass.txt").exists())

    def test_failed_injector_cannot_report_match(self):
        self.execute(mode="inject_failure", percent=100.0)
        self.assert_restored()

    def test_oversized_text_cannot_be_kept_at_100_percent(self):
        self.execute(percent=100.0, candidate_size=8)
        self.assert_restored()

    def test_zero_text_cannot_be_kept_at_100_percent(self):
        self.execute(percent=100.0, candidate_size=0)
        self.assert_restored()

    def test_matching_function_accepts_expected_section_padding(self):
        # The function metadata is 4 bytes, while GNU as can align the expected
        # .text section to 16 bytes. Section padding is not extra function code.
        self.execute(percent=100.0, candidate_size=4, expected_size=16)
        self.assertEqual(self.src.read_text(), CANDIDATE)
        self.assertTrue(self.obj.exists())
        self.assertIn("100.0 KEEP", self.output.getvalue())

    def test_fresh_equal_size_match_is_kept(self):
        self.execute(percent=100.0)
        self.assertEqual(self.src.read_text(), CANDIDATE)
        self.assertTrue(self.obj.exists())
        self.assertIn("100.0 KEEP", self.output.getvalue())


if __name__ == "__main__":
    unittest.main()
