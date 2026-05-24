#!/usr/bin/env python3
"""tools/overlay/compile_overlay_src.py

Compile every .c under src/overlays/AREAXX/ into
build/overlays/AREAXX/obj/<stem>.o via mwccmips (run under qemu-i386 wibo32).

Per-file CFLAGS may be supplied as a "// CFLAGS: ..." comment on the first
non-blank line of the .c file (same convention as tools/decomp/build.py).
Default is "-O4,p -sdatathreshold 4".

Usage (inside exterm-toolchain container):
    python3 tools/overlay/compile_overlay_src.py AREA18
    python3 tools/overlay/compile_overlay_src.py --all
"""
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_CFLAGS = "-O4,p -sdatathreshold 4"


def file_cflags(src: Path) -> str:
    try:
        with src.open() as f:
            for line in f:
                s = line.strip()
                if not s:
                    continue
                if s.startswith("// CFLAGS:"):
                    return s[len("// CFLAGS:"):].strip()
                break
    except OSError:
        pass
    return DEFAULT_CFLAGS


def compile_one(c_src: Path, out_obj: Path) -> int:
    out_obj.parent.mkdir(parents=True, exist_ok=True)
    flags = file_cflags(c_src)
    cmd = ["qemu-i386", "tools/bin/wibo32", "tools/mwccps2/mwccmips.exe",
           "-c"] + flags.split() + ["-o", str(out_obj.relative_to(ROOT)),
                                    str(c_src.relative_to(ROOT))]
    r = subprocess.run(cmd, cwd=ROOT)
    return r.returncode


def process_area(area: str) -> int:
    src_dir = ROOT / "src" / "overlays" / area
    obj_dir = ROOT / "build" / "overlays" / area / "obj"
    if not src_dir.exists():
        print(f"[compile_overlay_src] no src for {area}, skipping")
        return 0
    sources = sorted(src_dir.glob("*.c"))
    if not sources:
        print(f"[compile_overlay_src] {area}: no .c files in {src_dir.relative_to(ROOT)}")
        return 0
    errors = 0
    for c in sources:
        out_o = obj_dir / f"{c.stem}.o"
        rc = compile_one(c, out_o)
        status = "OK" if rc == 0 else f"FAIL rc={rc}"
        print(f"[compile_overlay_src] {area}/{c.name} -> {out_o.name}  {status}")
        if rc != 0:
            errors += 1
    return errors


def main():
    ap = argparse.ArgumentParser()
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument("area", nargs="?", help="Overlay name, e.g. AREA18")
    g.add_argument("--all", action="store_true",
                   help="Compile all src/overlays/AREA*/")
    args = ap.parse_args()

    if args.all:
        areas = sorted(p.name for p in (ROOT / "src" / "overlays").iterdir()
                       if p.is_dir())
    else:
        areas = [args.area.upper()]

    total_errors = 0
    for a in areas:
        total_errors += process_area(a)
    return 1 if total_errors else 0


if __name__ == "__main__":
    sys.exit(main())
