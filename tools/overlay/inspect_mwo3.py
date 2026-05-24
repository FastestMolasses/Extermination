#!/usr/bin/env python3
"""
tools/overlay/inspect_mwo3.py — MWo3 overlay file inspector.

Reads a Metrowerks MWo3 overlay file (OVERLAY/AREA*.BIN) and prints its header
fields, verifies the file structure, and optionally extracts the text/data sections.

Usage:
    python3 tools/overlay/inspect_mwo3.py OVERLAY/AREA07.BIN [--extract]

The MWo3 format (confirmed for all 19 Extermination overlays):
    [0x00..0x03]  magic: "MWo3" (0x4D 0x57 0x6F 0x33)
    [0x04..0x07]  overlay_id: u32 LE, 1-based sequential ID
    [0x08..0x0B]  load_address: u32 LE, always 0x00823500 (arena base)
    [0x0C..0x0F]  text_size: u32 LE, bytes of code section stored in file
    [0x10..0x13]  data_size: u32 LE, bytes of initialized data section in file
    [0x14..0x17]  bss_size: u32 LE, bytes of zero-init BSS (NOT in file)
    [0x18..0x1B]  static_init_address: u32 LE, vram of C++ static ctor list start
    [0x1C..0x1F]  static_init_end_address: u32 LE, vram of C++ static ctor list end
    [0x20..0x3F]  overlay_name[32]: NUL-terminated ASCII name, e.g. "Area07.bin"
    [0x40..]      text section (text_size bytes), loads to load_address
    [0x40+text..]  data section (data_size bytes), loads to load_address+text_size

File invariant:  filesize == 0x40 + text_size + data_size  (verified for all 19 files)

The text section always begins with 0x40 bytes of zeros (nop sled).
The first actual function is at vram load_address + 0x40 (= 0x00823540).
"""

import argparse
import struct
import sys
from pathlib import Path

HEADER_SIZE = 0x40
ARENA_BASE = 0x00823500
EXPECTED_MAGIC = b'MWo3'


def parse_header(data: bytes) -> dict:
    if len(data) < HEADER_SIZE:
        raise ValueError(f'File too short: {len(data)} < {HEADER_SIZE}')
    magic = data[0:4]
    if magic != EXPECTED_MAGIC:
        raise ValueError(f'Bad magic: {magic!r}, expected {EXPECTED_MAGIC!r}')
    u32 = lambda o: struct.unpack_from('<I', data, o)[0]
    return {
        'magic': magic.decode('ascii'),
        'overlay_id': u32(0x04),
        'load_address': u32(0x08),
        'text_size': u32(0x0C),
        'data_size': u32(0x10),
        'bss_size': u32(0x14),
        'static_init_address': u32(0x18),
        'static_init_end_address': u32(0x1C),
        'overlay_name': data[0x20:0x40].rstrip(b'\x00').decode('ascii', errors='replace'),
    }


def verify(data: bytes, hdr: dict, path: Path) -> list[str]:
    warnings = []
    expected_size = HEADER_SIZE + hdr['text_size'] + hdr['data_size']
    if len(data) != expected_size:
        warnings.append(
            f'File size mismatch: got {len(data)} (0x{len(data):x}), '
            f'expected {expected_size} (0x{expected_size:x}) = '
            f'0x40 + text_size(0x{hdr["text_size"]:x}) + data_size(0x{hdr["data_size"]:x})'
        )
    if hdr['load_address'] != ARENA_BASE:
        warnings.append(
            f'Unexpected load_address: 0x{hdr["load_address"]:08x} '
            f'(expected 0x{ARENA_BASE:08x})'
        )
    # Check the first 0x40 bytes of text are all zero (nop sled)
    if hdr['text_size'] >= 0x40:
        nop_sled = data[HEADER_SIZE:HEADER_SIZE + 0x40]
        if nop_sled != b'\x00' * 0x40:
            warnings.append('Expected 0x40 bytes of zeros at start of text section (nop sled)')
    return warnings


def print_header(hdr: dict, filesize: int):
    load = hdr['load_address']
    text_end = load + hdr['text_size']
    data_end = text_end + hdr['data_size']
    bss_end = data_end + hdr['bss_size']
    total_mem = hdr['text_size'] + hdr['data_size'] + hdr['bss_size'] + HEADER_SIZE
    si_range = ''
    if hdr['static_init_address'] != hdr['static_init_end_address']:
        si_range = (
            f"  0x{hdr['static_init_address']:08x}..0x{hdr['static_init_end_address']:08x} "
            f"({hdr['static_init_end_address'] - hdr['static_init_address']} bytes)"
        )
    else:
        si_range = f"  0x{hdr['static_init_address']:08x} (empty — no static ctors)"

    print(f"magic             : {hdr['magic']}")
    print(f"overlay_id        : {hdr['overlay_id']} (PT_LOAD slot {hdr['overlay_id']})")
    print(f"overlay_name      : {hdr['overlay_name']!r}")
    print(f"load_address      : 0x{load:08x}")
    print(f"text_size         : 0x{hdr['text_size']:x} ({hdr['text_size']:,} bytes)")
    print(f"  .text vram      : 0x{load:08x}..0x{text_end:08x}")
    print(f"  first func vram : 0x{load + 0x40:08x}  (+0x40 past nop sled)")
    print(f"data_size         : 0x{hdr['data_size']:x} ({hdr['data_size']:,} bytes)")
    print(f"  .data vram      : 0x{text_end:08x}..0x{data_end:08x}")
    print(f"bss_size          : 0x{hdr['bss_size']:x} ({hdr['bss_size']:,} bytes)")
    print(f"  .bss vram       : 0x{data_end:08x}..0x{bss_end:08x}")
    print(f"static_init       :{si_range}")
    print(f"file size         : 0x{filesize:x} ({filesize:,} bytes)")
    print(f"total memory      : 0x{total_mem:x} ({total_mem:,} bytes)  [text+data+bss+header]")


def main():
    ap = argparse.ArgumentParser(description='MWo3 overlay inspector')
    ap.add_argument('file', type=Path, help='AREA*.BIN file to inspect')
    ap.add_argument('--extract', action='store_true',
                    help='Write text/data sections as separate files beside the input')
    ap.add_argument('--all', action='store_true',
                    help='Print summary for all AREA*.BIN files in the same directory')
    args = ap.parse_args()

    if args.all:
        parent = args.file.parent
        files = sorted(parent.glob('AREA*.BIN'))
        print(f"{'File':<12} {'ID':>3} {'TextSz':>8} {'DataSz':>8} {'BSSSz':>10} {'FirstFunc':>10}  Name")
        print('-' * 75)
        for p in files:
            data = p.read_bytes()
            try:
                hdr = parse_header(data)
            except ValueError as e:
                print(f'{p.name:<12} ERROR: {e}')
                continue
            ff = hdr['load_address'] + 0x40
            print(
                f"{p.name:<12} {hdr['overlay_id']:>3} "
                f"0x{hdr['text_size']:06x} 0x{hdr['data_size']:06x} "
                f"0x{hdr['bss_size']:08x} 0x{ff:08x}  {hdr['overlay_name']}"
            )
        return

    data = args.file.read_bytes()
    try:
        hdr = parse_header(data)
    except ValueError as e:
        print(f'ERROR: {e}', file=sys.stderr)
        sys.exit(1)

    print_header(hdr, len(data))

    warnings = verify(data, hdr, args.file)
    if warnings:
        print()
        for w in warnings:
            print(f'WARNING: {w}')
    else:
        print()
        print('Structure verified OK.')

    if args.extract:
        stem = args.file.stem
        text_path = args.file.with_name(stem + '.text.bin')
        data_path = args.file.with_name(stem + '.data.bin')
        text_path.write_bytes(data[HEADER_SIZE:HEADER_SIZE + hdr['text_size']])
        data_path.write_bytes(data[HEADER_SIZE + hdr['text_size']:])
        print(f'\nExtracted:')
        print(f'  text: {text_path}  ({hdr["text_size"]:,} bytes)')
        print(f'  data: {data_path}  ({hdr["data_size"]:,} bytes)')


if __name__ == '__main__':
    main()
