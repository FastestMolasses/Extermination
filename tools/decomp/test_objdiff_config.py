#!/usr/bin/env python3
"""Regression: old objects/configuration cannot keep a demoted unit counted."""
import contextlib
import io
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import build
from decomp import build as package_build
import verify
import verify_all


class CurrentUnitsTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        for directory in ("src", build.ASM_DIR, "build/obj", "build/expected"):
            (self.root / directory).mkdir(parents=True, exist_ok=True)
        for name in ("kept", "demoted", "removed", "new"):
            (self.root / build.ASM_DIR / f"{name}.s").write_text("fixture\n")
            for directory in ("obj", "expected"):
                (self.root / "build" / directory / f"{name}.o").write_bytes(b"stale")
        (self.root / "src/kept.c").write_text("void kept(void) {}\n")
        (self.root / "src/new.c").write_text("void new(void) {}\n")
        (self.root / "src/demoted.c").write_text("\n// NEARMISS demoted 99.0%\nvoid demoted(void) {}\n")
        self.cached = json.dumps({"units": [{"name": n} for n in ("kept", "demoted", "removed")]})
        (self.root / "objdiff.json").write_text(self.cached)
        for module in (build, package_build):
            for name, value in (("ROOT", self.root), ("SRC", self.root / "src"),
                                ("BUILD", self.root / "build"),
                                ("OBJDIFF_JSON", self.root / "objdiff.json")):
                p = patch.object(module, name, value)
                p.start()
                self.addCleanup(p.stop)

    def test_generated_config_removes_demoted_and_missing_sources(self):
        with contextlib.redirect_stdout(io.StringIO()):
            build.write_objdiff()
        cfg = json.loads((self.root / "objdiff.json").read_text())
        self.assertEqual([u["name"] for u in cfg["units"]], ["kept", "new"])
        self.assertEqual(cfg["units"][0]["base_path"], "build/obj/kept.o")

    def test_per_function_report_ignores_cached_config(self):
        names = []
        def match(unit):
            names.append(unit["name"])
            return 100.0
        with patch.object(verify, "match_percent", match), contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(verify.main([]), 0)
        self.assertEqual(names, ["kept", "new"])
        self.assertEqual((self.root / "objdiff.json").read_text(), self.cached)

    def test_gate_uses_fresh_isolated_config_without_editing_cached_one(self):
        cli = self.root / "fixture-cli"
        cli.touch()
        def run(command, **_kwargs):
            project = Path(command[command.index("-p")+1])
            cfg = json.loads((project / "objdiff.json").read_text())
            self.assertEqual([u["name"] for u in cfg["units"]], ["kept", "new"])
            self.assertEqual(cfg["units"][0]["base_path"], str(self.root / "build/obj/kept.o"))
            Path(command[command.index("-o")+1]).write_text(json.dumps({
                "measures": {"matched_code_percent": 100.0,
                             "matched_functions": 2, "total_functions": 2}}))
            return subprocess.CompletedProcess(command, 0, "", "")
        stage = verify_all.Stage("match", False)
        with patch.object(verify_all, "ROOT", self.root), patch.object(verify_all, "OBJDIFF_CLI", cli), \
                patch.object(verify_all.subprocess, "run", run):
            verify_all.run_match(stage, 95.0)
        self.assertTrue(stage.ok)
        self.assertIn("functions 2/2", stage.detail)
        self.assertEqual((self.root / "objdiff.json").read_text(), self.cached)


if __name__ == "__main__":
    unittest.main()
