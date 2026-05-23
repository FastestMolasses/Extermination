#!/usr/bin/env python3
"""Compare the rebuilt ELF against the original and report size-drift functions.

This tool:
  1. Reads config/SCUS_971.12 (original boot ELF) and extracts the PT_LOAD bytes.
  2. Reads elf/SCUS_971.12.elf (rebuilt) and extracts the PT_LOAD bytes.
  3. Reads every .s file in build/asm/matchings/main/code/ to get the declared
     original vram and size for each function.
  4. For each function whose compiled build/obj/<name>.o exists (matched functions),
     reads the .text section size of the .o file and compares it to the declared
     original size.
  5. Reports:
     - Overall byte identity %
     - Per-function size drift (compiled .o .text size vs original size)
     - Whether the byte range in the rebuilt ELF matches the original

Usage (run from repo root, no container needed for the comparison step):
    python3 tools/decomp/diff_link.py [--obj-dir build/obj] [--top N]

Options:
    --obj-dir    Directory containing compiled .o files (default: build/obj)
    --filler-dir Directory containing filler .o files (default: build/filler)
    --top N      Show only the top N largest drift functions (default: all)
    --check-bytes Also check byte content for each matched function's range
    --summary    Print summary statistics only (no per-function detail)
"""
from __future__ import annotations

import argparse
import re
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ORIG_ELF = ROOT / "config" / "SCUS_971.12"
REBUILT_ELF = ROOT / "elf" / "SCUS_971.12.elf"
ASM_DIR = ROOT / "build" / "asm" / "matchings" / "main" / "code"
OBJ_DIR = ROOT / "build" / "obj"
FILLER_DIR = ROOT / "build" / "filler"

# Original ELF layout (from CLAUDE.md / target-identity)
ORIG_LOAD_VRAM = 0x00100000


def find_load_segment(elf_path: Path) -> tuple[int, int, int]:
    """Return (file_offset, vaddr, filesz) of the first PT_LOAD segment."""
    data = elf_path.read_bytes()
    e_phoff = struct.unpack_from("<I", data, 0x1c)[0]
    e_phentsize = struct.unpack_from("<H", data, 0x2a)[0]
    e_phnum = struct.unpack_from("<H", data, 0x2c)[0]
    PT_LOAD = 1
    for i in range(e_phnum):
        off = e_phoff + i * e_phentsize
        p_type = struct.unpack_from("<I", data, off)[0]
        if p_type == PT_LOAD:
            p_offset = struct.unpack_from("<I", data, off + 4)[0]
            p_vaddr = struct.unpack_from("<I", data, off + 8)[0]
            p_filesz = struct.unpack_from("<I", data, off + 16)[0]
            return p_offset, p_vaddr, p_filesz
    raise ValueError(f"No PT_LOAD segment found in {elf_path}")


def get_obj_text_size(obj_path: Path) -> int | None:
    """Return the .text section size of an ELF object file, or None on error."""
    try:
        data = obj_path.read_bytes()
        e_shoff = struct.unpack_from("<I", data, 32)[0]
        e_shentsize = struct.unpack_from("<H", data, 46)[0]
        e_shnum = struct.unpack_from("<H", data, 48)[0]
        e_shstrndx = struct.unpack_from("<H", data, 50)[0]

        shstr_off = e_shoff + e_shstrndx * e_shentsize
        shstr_sh_off = struct.unpack_from("<I", data, shstr_off + 16)[0]
        shstr_sh_size = struct.unpack_from("<I", data, shstr_off + 20)[0]
        shstr = data[shstr_sh_off: shstr_sh_off + shstr_sh_size]

        for i in range(e_shnum):
            sh_off = e_shoff + i * e_shentsize
            name_off = struct.unpack_from("<I", data, sh_off)[0]
            sh_type = struct.unpack_from("<I", data, sh_off + 4)[0]
            sh_size = struct.unpack_from("<I", data, sh_off + 20)[0]
            if sh_type == 0:
                continue
            end = shstr.find(b"\x00", name_off)
            if end < 0:
                continue
            name = shstr[name_off:end].decode("ascii", errors="replace")
            if name == ".text":
                return sh_size
        return None
    except Exception:
        return None


def parse_asm_functions() -> list[tuple[int, str, int]]:
    """Parse all .s files and return list of (vram, name, original_size) sorted by vram."""
    _nonmatching_re = re.compile(r"^nonmatching\s+(\S+),\s+(0x[0-9A-Fa-f]+|\d+)")
    results = []
    for f in ASM_DIR.glob("*.s"):
        name = f.stem
        # Extract vram from first comment /* FILEOFF VRAMADDR ...
        vram = 0
        orig_size = 0
        try:
            text = f.read_text(errors="replace")
            lines = text.splitlines()
            # nonmatching line gives size
            for line in lines[:5]:
                m = _nonmatching_re.match(line.strip())
                if m:
                    sz_str = m.group(2)
                    orig_size = int(sz_str, 16) if sz_str.startswith("0x") else int(sz_str)
                    break
            # vram from first /* FILEOFF VRAMADDR */ comment
            for line in lines[:20]:
                if "/*" in line:
                    try:
                        comment = line.split("/*")[1].split("*/")[0].strip()
                        parts = comment.split()
                        if len(parts) >= 2:
                            vram = int(parts[1], 16)
                            break
                    except (ValueError, IndexError):
                        pass
            if vram == 0 and name.startswith("func_"):
                try:
                    vram = int(name[5:], 16)
                except ValueError:
                    pass
        except Exception:
            pass
        if orig_size > 0:
            results.append((vram, name, orig_size))
    results.sort()
    return results


def main() -> int:
    ap = argparse.ArgumentParser(description="Diff rebuilt ELF against original and report size drift")
    ap.add_argument("--obj-dir", default=str(OBJ_DIR),
                    help=f"compiled .o directory (default: {OBJ_DIR.relative_to(ROOT)})")
    ap.add_argument("--filler-dir", default=str(FILLER_DIR),
                    help=f"filler .o directory (default: {FILLER_DIR.relative_to(ROOT)})")
    ap.add_argument("--top", type=int, default=0,
                    help="show top N drift functions (0=all)")
    ap.add_argument("--check-bytes", action="store_true",
                    help="check byte content for each function's range in the rebuilt ELF")
    ap.add_argument("--summary", action="store_true",
                    help="print summary statistics only")
    ap.add_argument("--drift-only", action="store_true",
                    help="only show functions with non-zero size drift")
    args = ap.parse_args()

    obj_dir = Path(args.obj_dir)
    filler_dir = Path(args.filler_dir)

    # --- Load ELF content ---
    if not ORIG_ELF.exists():
        print(f"[diff_link] ERROR: original ELF not found at {ORIG_ELF}", file=sys.stderr)
        return 1

    if not REBUILT_ELF.exists():
        print(f"[diff_link] ERROR: rebuilt ELF not found at {REBUILT_ELF}", file=sys.stderr)
        print("[diff_link] Run: python3 tools/decomp/link.py --no-fill", file=sys.stderr)
        return 1

    orig_off, orig_vaddr, orig_filesz = find_load_segment(ORIG_ELF)
    rebuilt_off, rebuilt_vaddr, rebuilt_filesz = find_load_segment(REBUILT_ELF)

    orig_data = ORIG_ELF.read_bytes()
    rebuilt_data = REBUILT_ELF.read_bytes()

    orig_load = orig_data[orig_off: orig_off + orig_filesz]
    rebuilt_load = rebuilt_data[rebuilt_off: rebuilt_off + rebuilt_filesz]

    print(f"[diff_link] original LOAD: vaddr=0x{orig_vaddr:08x}, filesz=0x{orig_filesz:x} ({orig_filesz} bytes)")
    print(f"[diff_link] rebuilt  LOAD: vaddr=0x{rebuilt_vaddr:08x}, filesz=0x{rebuilt_filesz:x} ({rebuilt_filesz} bytes)")

    # Overall byte identity
    compare_sz = min(len(orig_load), len(rebuilt_load))
    identical = sum(1 for a, b in zip(orig_load, rebuilt_load) if a == b)
    pct = 100.0 * identical / compare_sz if compare_sz else 0.0
    print(f"[diff_link] Byte identity: {identical}/{compare_sz} bytes ({pct:.2f}%)")

    # First diff
    for i in range(compare_sz):
        if orig_load[i] != rebuilt_load[i]:
            print(f"[diff_link] First diff at vram=0x{orig_vaddr + i:08x} (offset 0x{i:x}): "
                  f"orig=0x{orig_load[i]:02x} rebuilt=0x{rebuilt_load[i]:02x}")
            break

    # --- Parse all functions ---
    funcs = parse_asm_functions()
    print(f"[diff_link] Parsed {len(funcs)} functions from .s files")

    # --- Check size drift for compiled objects ---
    drift_funcs = []
    total_drift = 0
    matched_count = 0
    drift_count = 0

    for vram, name, orig_size in funcs:
        obj_path = obj_dir / f"{name}.o"
        if not obj_path.exists():
            continue
        matched_count += 1
        text_size = get_obj_text_size(obj_path)
        if text_size is None:
            continue
        drift = text_size - orig_size
        if drift != 0:
            drift_count += 1
            total_drift += drift
        drift_funcs.append((drift, vram, name, orig_size, text_size))

    # Sort by absolute drift (largest first)
    drift_funcs.sort(key=lambda x: (-abs(x[0]), x[1]))

    print(f"\n[diff_link] Matched functions analyzed: {matched_count}")
    print(f"[diff_link] Functions with size drift:   {drift_count}")
    print(f"[diff_link] Total accumulated drift:     {total_drift:+d} bytes")

    if args.summary:
        return 0

    # --- Per-function report ---
    show = drift_funcs
    if args.drift_only:
        show = [x for x in show if x[0] != 0]
    if args.top > 0:
        show = show[:args.top]

    if show:
        print(f"\n{'Drift':>8}  {'Vram':>10}  {'Name':<40}  {'OrigSz':>8}  {'ObjSz':>8}")
        print("-" * 80)
        for drift, vram, name, orig_size, text_size in show:
            marker = " <--" if drift != 0 else ""
            print(f"{drift:>+8}  0x{vram:08x}  {name:<40}  {orig_size:>8}  {text_size:>8}{marker}")

    # --- Byte-check for each matched function ---
    if args.check_bytes:
        print("\n[diff_link] Byte check per matched function:")
        # Sort by vram for byte check
        sorted_by_vram = sorted(drift_funcs, key=lambda x: x[1])
        mismatches = 0
        for drift, vram, name, orig_size, text_size in sorted_by_vram:
            if vram < orig_vaddr:
                continue
            off = vram - orig_vaddr
            if off + orig_size > len(orig_load) or off + orig_size > len(rebuilt_load):
                continue
            orig_slice = orig_load[off: off + orig_size]
            rebuilt_slice = rebuilt_load[off: off + orig_size]
            if orig_slice != rebuilt_slice:
                mismatches += 1
                if mismatches <= 20:
                    print(f"  MISMATCH  0x{vram:08x}  {name}")
        print(f"  {mismatches} mismatching function byte ranges")

    return 0


if __name__ == "__main__":
    sys.exit(main())
