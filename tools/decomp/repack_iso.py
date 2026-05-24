#!/usr/bin/env python3
"""Replace SCUS_971.12 inside a copy of the Extermination ISO with our rebuilt
ELF, producing a PCSX2-loadable image.

The rebuilt ELF (`elf/SCUS_971.12.elf`) carries extra symbol/debug sections
that the original (stripped) ELF doesn't have, so it's too large to drop into
the ISO's existing slot as-is. This script first strips the rebuilt ELF down
to a boot-ready form (only the PROGBITS that the PS2 BIOS actually loads),
then patches it in-place inside the ISO. The PS2 BIOS only ever reads the
ELF header + program headers + loadable content; everything else is cruft.

Usage:
    .venv/bin/python tools/decomp/repack_iso.py [--iso PATH] [--elf PATH]

Defaults to `Extermination-rebuilt.iso` at the repo root and
`elf/SCUS_971.12.elf`. The ISO is modified in place.
"""
from __future__ import annotations

import argparse
import io
import subprocess
import sys
from pathlib import Path

try:
    import pycdlib
except ImportError:
    sys.exit("error: pycdlib not installed. Run: .venv/bin/pip install pycdlib")

ROOT = Path(__file__).resolve().parents[2]
ELF_NAME = "SCUS_971.12"
ELF_ISO_NAME = f"{ELF_NAME};1"        # ISO9660 names have a `;version` suffix
ELF_ISO_PATH = f"/{ELF_ISO_NAME}"
CONTAINER = "exterm-toolchain"
SECTOR = 2048                         # ISO9660 sector size


def strip_elf(src: Path, dst: Path) -> int:
    """Strip every non-loadable section from `src` and write to `dst`.

    Keep only what the PS2 BIOS actually consumes: the loadable PROGBITS
    (the `main` section) and `.reginfo`. Everything else (`.mwcats`, debug,
    symbol tables, GCC-attribute sections) gets dropped. We use
    `--only-section` rather than enumerating remove targets so any future
    extra CodeWarrior section also goes away automatically.
    """
    src_in_container = src.relative_to(ROOT)
    dst_in_container = dst.relative_to(ROOT)
    subprocess.run(
        ["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
         CONTAINER, "sh", "-c",
         f"mipsel-linux-gnu-objcopy --only-section=main "
         f"--only-section=.reginfo --strip-all "
         f"{src_in_container} {dst_in_container}"],
        check=True,
    )
    return dst.stat().st_size


def locate_iso_file(iso_path: Path, name: str) -> tuple[int, int]:
    """Return (file_byte_offset, declared_length) for `name` inside the ISO.

    We use pycdlib only to read the directory record; the actual byte-write
    we do ourselves below. (pycdlib's modify_file_in_place tried to rewrite
    structural metadata and corrupted the ISO on us.)
    """
    iso = pycdlib.PyCdlib()
    iso.open(str(iso_path))
    try:
        rec = iso.get_record(iso_path=f"/{name}")
        return rec.extent_location() * SECTOR, rec.get_data_length()
    finally:
        iso.close()


def swap_iso_file(iso_path: Path, name: str, new_content: bytes) -> None:
    """Replace the bytes of `name` inside the ISO with `new_content`.

    We write directly into the ISO at the file's sector offset. The PS2 BIOS
    reads `data_length` bytes starting at that sector and parses them as an
    ELF — so as long as our content is ≤ the declared length and self-
    contained, the trailing zero padding is harmless (it's not in any
    PT_LOAD program header so it's never copied to RAM).
    """
    file_offset, declared_len = locate_iso_file(iso_path, name)
    if len(new_content) > declared_len:
        raise SystemExit(
            f"error: new ELF is {len(new_content):,} bytes but the ISO slot "
            f"for {name} is only {declared_len:,} bytes. Strip more sections "
            f"to fit, or rebuild the ISO from scratch with extra room.")
    padded = new_content + b"\x00" * (declared_len - len(new_content))
    with open(iso_path, "r+b") as f:
        f.seek(file_offset)
        f.write(padded)


def swap_overlays(iso_path: Path, overlay_dir: Path) -> int:
    """Replace OVERLAY/AREA*.BIN files in the ISO with rebuilt versions.

    Scans `overlay_dir` (build/overlays/) for AREAXX/AREAXX.BIN files and
    swaps each one into the corresponding ISO slot.  Returns the number of
    overlays swapped.

    Each rebuilt .BIN must be exactly the same size as the original (it
    contains the same MWo3 header + text + data; BSS is never stored).
    """
    import pycdlib as _pycdlib
    iso = _pycdlib.PyCdlib()
    iso.open(str(iso_path))
    iso.close()

    count = 0
    for rebuilt in sorted(overlay_dir.glob("AREA*/AREA*.BIN")):
        area = rebuilt.parent.name  # "AREA00", etc.
        iso_name = f"{area}.BIN;1"
        iso_path_str = f"/OVERLAY/{iso_name}"

        try:
            file_off, slot_len = locate_iso_file(iso_path, iso_path_str)
        except Exception as e:
            print(f"[overlay] WARNING: {area}.BIN not found in ISO ({e}), skipping")
            continue

        content = rebuilt.read_bytes()
        if len(content) != slot_len:
            print(f"[overlay] WARNING: {area}.BIN rebuilt={len(content)} "
                  f"!= slot={slot_len} — skipping (size must match)")
            continue

        padded = content + b"\x00" * (slot_len - len(content))
        with open(iso_path, "r+b") as f:
            f.seek(file_off)
            f.write(padded)

        count += 1
        print(f"[overlay] swapped {area}.BIN ({len(content):,} bytes)")

    return count


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--iso", type=Path,
                   default=ROOT / "Extermination-rebuilt.iso",
                   help="ISO to modify in place")
    p.add_argument("--elf", type=Path,
                   default=ROOT / "elf" / f"{ELF_NAME}.elf",
                   help="Rebuilt ELF to insert")
    p.add_argument("--keep-stripped", action="store_true",
                   help="keep the intermediate boot-ready ELF (elf/SCUS_971.12.boot.elf)")
    p.add_argument("--overlays", action="store_true",
                   help="also swap rebuilt OVERLAY/AREA*.BIN files into the ISO")
    p.add_argument("--overlay-dir", type=Path,
                   default=ROOT / "build" / "overlays",
                   help="directory containing AREAXX/AREAXX.BIN rebuilt overlays")
    args = p.parse_args(argv)

    if not args.iso.exists():
        sys.exit(f"error: ISO not found: {args.iso}")
    if not args.elf.exists():
        sys.exit(f"error: rebuilt ELF not found: {args.elf}")

    # 1. Strip the rebuilt ELF to a boot-ready form (no debug/symbol/pdr
    #    cruft) so it fits in the original's ISO slot.
    stripped = args.elf.with_suffix(".boot.elf")
    print(f"[strip] {args.elf.name} ({args.elf.stat().st_size:,} bytes) → "
          f"{stripped.name}")
    stripped_size = strip_elf(args.elf, stripped)
    print(f"[strip] result: {stripped_size:,} bytes")

    # 2. Look up the ISO slot location + size.
    file_off, slot_len = locate_iso_file(args.iso, ELF_ISO_NAME)
    print(f"[iso]   {args.iso.name} ({args.iso.stat().st_size:,} bytes); "
          f"{ELF_NAME} at offset {file_off:#x}, slot {slot_len:,} bytes")

    # 3. Swap in the stripped ELF, padding to the original slot size.
    swap_iso_file(args.iso, ELF_ISO_NAME, stripped.read_bytes())
    print(f"[iso]   replaced {ELF_NAME} with our build")

    if not args.keep_stripped:
        stripped.unlink()

    # 4. Optionally swap rebuilt overlays.
    if args.overlays:
        if not args.overlay_dir.exists():
            print(f"[overlay] no overlay build dir found at {args.overlay_dir}",
                  file=sys.stderr)
        else:
            n = swap_overlays(args.iso, args.overlay_dir)
            print(f"[overlay] {n} overlay(s) swapped into ISO")

    print()
    print(f"✓ {args.iso.name} now boots our rebuilt ELF.")
    print(f"  Load it in PCSX2 via File → Open Disc (or drag the ISO in).")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
