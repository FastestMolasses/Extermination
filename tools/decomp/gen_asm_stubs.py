#!/usr/bin/env python3
"""gen_asm_stubs.py — create a committed INCLUDE_ASM stub for every function
that is byte-matched (assembled from local splat .s) but has no src/ file yet.

Goal: make EVERY function in the game a committed, claimable per-function unit
so decompilation work parallelizes across files — WITHOUT committing the
disassembly itself (CLAUDE.md hard rule). The stub contains only the function
name + a build directive; the byte-identical machine code keeps coming from the
locally-assembled, git-ignored splat .s (fill_unmatched.py). build.py skips
stubs at compile time (see is_asm_stub), so the build output is unchanged —
this is purely organizational.

A stub looks like:

    // INCLUDE_ASM func_001AFC10  (vram 0x001AFC10, 140 bytes)
    // UNDECOMPILED. Byte-identical machine code is assembled from the local
    // splat disassembly (git-ignored; regenerate with build.py setup). To
    // decompile: replace this file with C that compiles byte-identical, verified
    // with objdiff against build/expected/<name>.o. Until then this stub keeps
    // the function a committed, claimable unit.

Usage:
    .venv/bin/python tools/decomp/gen_asm_stubs.py [--dry-run] [--game-only]

Only functions with a real `glabel` in their splat .s get a stub (data regions
misidentified as code are skipped). Existing src/*.c are never overwritten.
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ASM_DIR = ROOT / "build" / "asm" / "matchings" / "main" / "code"
SRC = ROOT / "src"

GLABEL = re.compile(r"^glabel\s+(\S+)", re.M)
SIZE = re.compile(r"nonmatching \S+, (0x[0-9A-Fa-f]+)")


def func_vram(name: str) -> int | None:
    m = re.search(r"_([0-9A-Fa-f]{6,8})$", name)
    return int(m.group(1), 16) if m else None


def make_stub(name: str, vram: int | None, size: int) -> str:
    vs = f"0x{vram:08X}" if vram is not None else "unknown"
    return (
        f"// INCLUDE_ASM {name}  (vram {vs}, {size} bytes)\n"
        f"// UNDECOMPILED placeholder. The byte-identical machine code for this\n"
        f"// function is assembled from the local splat disassembly (git-ignored;\n"
        f"// regenerate with `build.py setup` from your own disc) and linked by\n"
        f"// fill_unmatched.py — so the rebuilt ELF stays byte-identical with or\n"
        f"// without this file. build.py does NOT compile INCLUDE_ASM stubs.\n"
        f"//\n"
        f"// To decompile: replace this file with C that compiles byte-identical,\n"
        f"// verified with objdiff against build/expected/{name}.o. See\n"
        f"// docs/PROGRESS.md for the matching idioms and the function index in\n"
        f"// docs/FUNCTIONS.csv.\n"
    )


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--dry-run", action="store_true")
    p.add_argument("--game-only", action="store_true",
                   help="only stub vram >= 0x130000 (skip the Sony SDK region)")
    args = p.parse_args(argv)

    if not ASM_DIR.exists():
        sys.exit(f"error: {ASM_DIR} missing — run build.py setup first")

    existing = {p.stem for p in SRC.glob("*.c")}
    created = skipped_have_src = skipped_nonfunc = skipped_sdk = 0
    for sf in sorted(ASM_DIR.glob("*.s")):
        name = sf.stem
        if name in existing:
            skipped_have_src += 1
            continue
        text = sf.read_text(errors="replace")
        if not GLABEL.search(text):
            skipped_nonfunc += 1          # data misidentified as code, etc.
            continue
        vram = func_vram(name)
        if args.game_only and vram is not None and vram < 0x130000:
            skipped_sdk += 1
            continue
        m = SIZE.search(text)
        size = int(m.group(1), 16) if m else 0
        if not args.dry_run:
            (SRC / f"{name}.c").write_text(make_stub(name, vram, size))
        created += 1

    print(f"stubs {'would be ' if args.dry_run else ''}created: {created}")
    print(f"  skipped (already have src): {skipped_have_src}")
    print(f"  skipped (no glabel / not a function): {skipped_nonfunc}")
    if args.game_only:
        print(f"  skipped (SDK region < 0x130000): {skipped_sdk}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
