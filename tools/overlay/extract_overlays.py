#!/usr/bin/env python3
"""
tools/overlay/extract_overlays.py — Extract OVERLAY/AREA*.BIN files from the ISO.

Reads the ISO at Extermination-rebuilt.iso (or --iso PATH) and writes all
AREA*.BIN files to extract/OVERLAY/.

Usage:
    .venv/bin/python tools/overlay/extract_overlays.py [--iso PATH] [--area AREA18]

Run from the repo root.  Requires pycdlib (.venv/bin/pip install pycdlib).
"""

from __future__ import annotations

import argparse
import io
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

try:
    import pycdlib
except ImportError:
    sys.exit("error: pycdlib not installed. Run: .venv/bin/pip install pycdlib")


def extract_overlays(iso_path: Path, out_dir: Path,
                     only_area: str | None = None) -> list[Path]:
    """Extract OVERLAY/AREA*.BIN files from iso_path into out_dir."""
    out_dir.mkdir(parents=True, exist_ok=True)

    iso = pycdlib.PyCdlib()
    iso.open(str(iso_path))
    extracted = []
    try:
        for child in iso.list_children(iso_path='/OVERLAY'):
            name_b = child.file_identifier()
            name = name_b.decode('ascii') if isinstance(name_b, bytes) else name_b
            if name in ('.', '..'):
                continue
            base_name = name.split(';')[0]   # strip ;1 version suffix
            if only_area and base_name.upper() != only_area.upper():
                continue
            out_path = out_dir / base_name
            buf = io.BytesIO()
            iso.get_file_from_iso_fp(buf, iso_path=f'/OVERLAY/{name}')
            buf.seek(0)
            data = buf.read()
            out_path.write_bytes(data)
            extracted.append(out_path)
            print(f"  extracted {base_name} ({len(data):,} bytes) → {out_path.relative_to(ROOT)}")
    finally:
        iso.close()
    return extracted


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description="Extract OVERLAY/AREA*.BIN from ISO")
    ap.add_argument("--iso", type=Path,
                    default=ROOT / "Extermination-rebuilt.iso",
                    help="ISO path (default: Extermination-rebuilt.iso)")
    ap.add_argument("--area", help="Extract only this area (e.g. AREA18)")
    ap.add_argument("--out", type=Path,
                    default=ROOT / "extract" / "OVERLAY",
                    help="Output directory (default: extract/OVERLAY/)")
    args = ap.parse_args(argv)

    if not args.iso.exists():
        sys.exit(f"error: ISO not found: {args.iso}")

    print(f"Extracting overlays from {args.iso.name} ...")
    extracted = extract_overlays(args.iso, args.out, args.area)
    print(f"Done — {len(extracted)} file(s) written to {args.out.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
