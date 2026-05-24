#!/usr/bin/env python3
"""
tools/overlay/pack_mwo3.py — Prepend the MWo3 header onto a raw text+data binary
to produce a final AREA*.BIN file, and verify byte-identity against the original.

Usage:
    python3 tools/overlay/pack_mwo3.py AREA18 [--raw PATH] [--out PATH] [--no-verify]

The raw binary is taken from build/overlays/AREAXX/AREAXX_raw.bin (produced by
mwldmips with "> rawfile" in its MEMORY block, or extracted from the ELF by
link_overlay.py).  The MWo3 header (64 bytes) is copied verbatim from the original
disc file at extract/OVERLAY/AREAXX.BIN.

Run from the repo root.
"""

from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ARENA_BASE = 0x00823500
MWO3_HDR_SIZE = 0x40


def parse_mwo3_header(data: bytes) -> dict:
    if data[:4] != b'MWo3':
        raise ValueError(f"Bad magic: {data[:4]!r}")
    u32 = lambda o: struct.unpack_from('<I', data, o)[0]
    return {
        'overlay_id':  u32(0x04),
        'load_address': u32(0x08),
        'text_size':   u32(0x0C),
        'data_size':   u32(0x10),
        'bss_size':    u32(0x14),
        'overlay_name': data[0x20:0x40].rstrip(b'\x00').decode('ascii', errors='replace'),
    }


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description="Pack MWo3 header + raw binary → AREA*.BIN")
    ap.add_argument("area", help="Overlay name, e.g. AREA18")
    ap.add_argument("--raw", type=Path, help="Path to raw text+data binary "
                    "(default: build/overlays/AREAXX/AREAXX_raw.bin)")
    ap.add_argument("--out", type=Path, help="Output path "
                    "(default: build/overlays/AREAXX/AREAXX.BIN)")
    ap.add_argument("--no-verify", action="store_true",
                    help="Skip byte-identity check against original")
    args = ap.parse_args(argv)

    name = args.area.upper()
    orig_path = ROOT / "extract" / "OVERLAY" / f"{name}.BIN"
    raw_path  = args.raw or (ROOT / "build" / "overlays" / name / f"{name}_raw.bin")
    out_path  = args.out or (ROOT / "build" / "overlays" / name / f"{name}.BIN")

    if not orig_path.exists():
        sys.exit(f"error: original not found: {orig_path}")
    if not raw_path.exists():
        sys.exit(f"error: raw binary not found: {raw_path}\n"
                 f"Run link_overlay.py {name} first.")

    orig_data = orig_path.read_bytes()
    raw_data  = raw_path.read_bytes()
    hdr = parse_mwo3_header(orig_data)

    print(f"[pack] {name}: header from {orig_path.relative_to(ROOT)}")
    print(f"       overlay_id={hdr['overlay_id']}, "
          f"text=0x{hdr['text_size']:x}, data=0x{hdr['data_size']:x}, "
          f"bss=0x{hdr['bss_size']:x}")

    expected_payload = hdr['text_size'] + hdr['data_size']
    if len(raw_data) != expected_payload:
        print(f"[pack] WARNING: raw binary is {len(raw_data)} bytes, "
              f"expected {expected_payload} (text+data). "
              f"Will use first {expected_payload} bytes.")
        raw_data = raw_data[:expected_payload]

    header_bytes = orig_data[:MWO3_HDR_SIZE]
    rebuilt = header_bytes + raw_data
    out_path.write_bytes(rebuilt)
    print(f"[pack] wrote {out_path.relative_to(ROOT)} ({len(rebuilt)} bytes)")

    if not args.no_verify:
        compare_size = expected_payload
        orig_payload    = orig_data[MWO3_HDR_SIZE: MWO3_HDR_SIZE + compare_size]
        rebuilt_payload = rebuilt[MWO3_HDR_SIZE: MWO3_HDR_SIZE + compare_size]

        if orig_payload == rebuilt_payload:
            print(f"[verify] PASS — {compare_size} bytes byte-identical "
                  f"(header copy + text+data)")
            if rebuilt == orig_data:
                print(f"[verify] PASS — full {len(orig_data)}-byte file identical")
        else:
            diffs = sum(1 for a, b in zip(orig_payload, rebuilt_payload) if a != b)
            pct = (compare_size - diffs) / compare_size * 100
            print(f"[verify] FAIL — {diffs}/{compare_size} bytes differ "
                  f"({pct:.2f}% match)")
            for i, (a, b) in enumerate(zip(orig_payload, rebuilt_payload)):
                if a != b:
                    vram = ARENA_BASE + i
                    print(f"[verify]   first diff at vram 0x{vram:08x}: "
                          f"rebuilt=0x{a:02x} orig=0x{b:02x}")
                    break
            return 2
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
