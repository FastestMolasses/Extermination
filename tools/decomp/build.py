#!/usr/bin/env python3
"""Build driver for the Extermination (SCUS-97112) matching decompilation.

Environment-aware:
  * macOS-arm64 : `setup` runs splat over the boot ELF and (re)writes
    objdiff.json. Pure Python — no toolchain needed.
  * Linux/amd64 : the compile/link path (Metrowerks mwccps2 under wibo) lands
    here once the toolchain container exists. Not yet wired up — see
    docs/PROGRESS.md, Track A phase 4.

Run with the project venv's Python so splat is importable:
    .venv/bin/python tools/decomp/build.py setup
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONFIG = ROOT / "config"
SRC = ROOT / "src"
BUILD = ROOT / "build"

SPLAT_YAML = CONFIG / "SCUS_971.12.yaml"
OBJDIFF_JSON = ROOT / "objdiff.json"


def run_splat() -> None:
    """Disassemble/split the boot ELF into build/ via splat."""
    if not (CONFIG / "SCUS_971.12").exists():
        sys.exit("error: config/SCUS_971.12 (boot ELF) is missing — extract it "
                 "from your own disc first. See docs/PROGRESS.md.")
    splat = Path(sys.executable).with_name("splat")
    cmd = [str(splat) if splat.exists() else "splat", "split", str(SPLAT_YAML)]
    print("[splat]", " ".join(cmd))
    subprocess.run(cmd, cwd=ROOT, check=True)


def write_objdiff() -> None:
    """Regenerate objdiff.json from the translation units currently in src/.

    objdiff (run natively on the M1) diffs each unit's target object — the
    original code, assembled from splat's disassembly into build/expected/ — vs
    the base object compiled from our C into build/obj/.
    """
    units = []
    for c in sorted(SRC.rglob("*.c")):
        rel = c.relative_to(SRC).with_suffix("")
        units.append({
            "name": rel.as_posix(),
            "target_path": f"build/expected/{rel.as_posix()}.o",
            "base_path": f"build/obj/{rel.as_posix()}.o",
            "metadata": {"source_path": f"src/{rel.as_posix()}.c"},
        })
    cfg = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "min_version": "2.0.0",
        "custom_make": "python3",
        "custom_args": ["tools/decomp/build.py", "--single-file"],
        "build_target": False,
        "watch_patterns": ["*.c", "*.h", "*.s"],
        "units": units,
    }
    OBJDIFF_JSON.write_text(json.dumps(cfg, indent=2) + "\n")
    print(f"[objdiff] wrote {OBJDIFF_JSON.relative_to(ROOT)} ({len(units)} unit(s))")


def cmd_setup(_args: argparse.Namespace) -> None:
    BUILD.mkdir(exist_ok=True)
    run_splat()
    write_objdiff()


def cmd_objdiff(_args: argparse.Namespace) -> None:
    write_objdiff()


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination decomp build driver")
    sub = p.add_subparsers(dest="cmd", required=True)
    sub.add_parser("setup", help="run splat over the boot ELF + write objdiff.json")
    sub.add_parser("objdiff", help="(re)write objdiff.json from src/")

    args = p.parse_args(argv)
    {"setup": cmd_setup, "objdiff": cmd_objdiff}[args.cmd](args)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
