#!/usr/bin/env python3
"""export_shadow_receivers.py - the drop shadow's receiver data for AREA11.

Original evidence (port docs/SHADOW_ORIGINAL.md): 001D5C80 walks the level
cell grid that 001D52E0 publishes from object 0 of the static-object bank
*D_0028A5A0 (ctx+0x140 = object 0 + 0x20, ctx+0x144/+0x148 = its words 0/1,
ctx+0x150..+0x164 = its floats +0x08..+0x1C), clip-tests every object id of
the cells with the object's AABB (object +0x14..+0x1C min, +0x24..+0x2C
max) and draws it by REFing its VIF data at object +0x40 (001D4F30: word 0
= block count, 0x82 qwords per block): STCYCL 4,4 + UNPACK V4-32 of 128
qwords (32 vertices x 4 qwords) + MSCAL/MSCNT. 001DA310 draws the box
models 0x14 / 0x15 of the chunk27 library *D_0028A56C the same way.
Objects are found with 001C6120: table + (word[1 + id] >> 2 << 2).

Sources (the user's own extracted disc; nothing is embedded here):
  bank     = resource 0x44 (D_0028A490[0x44] = D_0028A5A0, relocated by
             001FF830 state 7), which lies 0x123000 bytes into
             extract/chunk15/f12_id44.bin; chunk15's files load contiguously
             (f12 .. f17 follow each other in EE RAM) and the bank's objects
             run into f13..f17, so the bank is read from that concatenation.
  library  = resource 0x37 (D_0028A56C) = extract/chunk27/f01_id37.bin.
--verify-ram checks both placements and every exported byte against a
captured EE RAM image (the route beats' eeMemory.bin): D_0028A5A0 - (RAM
address of f12's first bytes) == 0x123000, the grid block and every object
record equal, and the library's models 0x14/0x15 equal at D_0028A56C.

Output (disc-derived: write it only to git-ignored locations, the port's
assets/): a little-endian "EMSR" v1 file, read by the port's
em_shadow_receivers_load (src/game/em_shadow_original.c):
  0x00 'EMSR', u32 version 1
  0x08 u32 ctx+0x144 (grid rows), u32 ctx+0x148 (stride)
  0x10 f32 ctx+0x150..+0x164 (6)
  0x28 u32 grid word count W, u32 object slot count N (bank word 0),
       u32 box model count (2), u32 0
  0x38 i32 grid[W]      (object 0 + 0x20, W = rows x stride x 4)
  then for each object id 1..N-1, then each box model (ids 0x14, 0x15):
       u32 id, u32 batches B, f32 AABB min[3], f32 AABB max[3],
       B x 128 qwords (the 32 vertices x 4 qwords each block unpacks)
Runs natively on arm64 macOS (pure Python).

Usage (repo root):
  .venv/bin/python tools/export_shadow_receivers.py \\
      --out ../extermination-port/assets/scene_snow/shadow_receivers.emsr \\
      --verify-ram build/s87/route/06_hill_slide/eeMemory.bin
"""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHUNK15_BANK_FILES = ("f12_id44.bin", "f13_id50.bin", "f14_id5a.bin", "f15_id47.bin",
                      "f16_id88.bin", "f17_id93.bin")
BANK_OFFSET = 0x123000           # resource 0x44 inside f12_id44
LIBRARY_FILE = "chunk27/f01_id37.bin"
D_0028A5A0, D_0028A56C = 0x28A5A0, 0x28A56C
BOX_MODELS = (0x14, 0x15)
BLOCK_QWORDS = 0x82              # 001D4F30: n * 0x82 qwords per REF run
BLOCK_HEAD = (0, 0, 0x01000404, 0x6C808000)   # NOP NOP STCYCL(4,4) UNPACK V4-32 128 @0 +TOPS
BLOCK_TAILS = (0x14000000, 0x17000000)        # MSCAL 0 (first block), MSCNT


def u32(b, a):
    return struct.unpack_from("<I", b, a)[0]


def entry(table: bytes, base: int, ident: int) -> int:
    """001C6120(table, id): table + (word[1 + id] >> 2 << 2)."""
    return base + (struct.unpack_from("<i", table, base + 4 + 4 * ident)[0] >> 2 << 2)


def object_record(data: bytes, off: int, what: str):
    """(batches, AABB min, AABB max, [128-qword blocks]) of one object
    record at `off`, checking every block's VIF codes."""
    count, qwc = u32(data, off), u32(data, off + 4)
    if count < 1 or qwc != count * BLOCK_QWORDS:
        raise SystemExit(f"{what}: block count {count} / qwc {qwc:#x} is not an object record")
    lo = struct.unpack_from("<3f", data, off + 0x14)
    hi = struct.unpack_from("<3f", data, off + 0x24)
    blocks = []
    for b in range(count):
        blk = off + 0x40 + b * BLOCK_QWORDS * 16
        head = struct.unpack_from("<4I", data, blk)
        tail = struct.unpack_from("<4I", data, blk + 0x81 * 16)
        if head != BLOCK_HEAD or tail[0] != BLOCK_TAILS[b > 0] or tail[1:] != (0, 0, 0):
            raise SystemExit(f"{what}: block {b} VIF codes {[hex(x) for x in head]} / "
                             f"{[hex(x) for x in tail]} are not STCYCL+UNPACK 128+MSCAL/MSCNT")
        blocks.append(data[blk + 16:blk + 16 + 128 * 16])
    return count, lo, hi, blocks


def build(extract: Path):
    bank_data = b"".join((extract / "chunk15" / f).read_bytes() for f in CHUNK15_BANK_FILES)
    library = (extract / LIBRARY_FILE).read_bytes()
    base = BANK_OFFSET
    slots = u32(bank_data, base)
    grid_block = entry(bank_data, base, 0)
    rows, stride = u32(bank_data, grid_block), u32(bank_data, grid_block + 4)
    floats = struct.unpack_from("<6f", bank_data, grid_block + 8)
    words = rows * stride * 4
    grid = struct.unpack_from(f"<{words}i", bank_data, grid_block + 0x20)
    objects = []
    for ident in range(1, slots):
        off = entry(bank_data, base, ident)
        objects.append((ident, off) + object_record(bank_data, off, f"bank object {ident:#x}"))
    boxes = []
    for m in BOX_MODELS:
        off = entry(library, 0, m)
        boxes.append((m, off) + object_record(library, off, f"library model {m:#x}"))
    used = {i for i in grid if i > 0}
    if not used <= {o[0] for o in objects}:
        raise SystemExit("grid ids without an object record")
    return dict(bank=bank_data, library=library, slots=slots, grid_block=grid_block, rows=rows,
                stride=stride, floats=floats, grid=grid, objects=objects, boxes=boxes, used=used)


def serialize(x) -> bytes:
    out = bytearray(b"EMSR")
    out += struct.pack("<I", 1)
    out += struct.pack("<2I", x["rows"], x["stride"])
    out += struct.pack("<6f", *x["floats"])
    out += struct.pack("<4I", len(x["grid"]), x["slots"], len(x["boxes"]), 0)
    out += struct.pack(f"<{len(x['grid'])}i", *x["grid"])
    for ident, _, count, lo, hi, blocks in x["objects"] + x["boxes"]:
        out += struct.pack("<2I3f3f", ident, count, *lo, *hi)
        for blk in blocks:
            out += blk
    return bytes(out)


def verify_ram(x, ram_path: Path) -> None:
    ram = ram_path.read_bytes()
    bank_ram, lib_ram = u32(ram, D_0028A5A0), u32(ram, D_0028A56C)
    f12 = x["bank"][:256]
    at = ram.find(f12)
    if at < 0 or bank_ram - at != BANK_OFFSET:
        raise SystemExit(f"D_0028A5A0 = {bank_ram:#x} is not f12_id44 ({at:#x}) + {BANK_OFFSET:#x}")
    span = len(x["bank"]) - BANK_OFFSET
    if ram[bank_ram:bank_ram + span] != x["bank"][BANK_OFFSET:]:
        raise SystemExit("the RAM bank differs from chunk15 f12..f17 from 0x123000")
    grid_ram = bank_ram + x["grid_block"] - BANK_OFFSET
    if ram[grid_ram + 0x20:grid_ram + 0x20 + 4 * len(x["grid"])] != struct.pack(f"<{len(x['grid'])}i", *x["grid"]):
        raise SystemExit("grid block differs")
    for ident, off, count, _, _, blocks in x["objects"]:
        a = bank_ram + off - BANK_OFFSET
        for b, blk in enumerate(blocks):
            s = a + 0x40 + b * BLOCK_QWORDS * 16 + 16
            if ram[s:s + len(blk)] != blk:
                raise SystemExit(f"object {ident:#x} block {b} differs from RAM")
    for m, off, count, _, _, blocks in x["boxes"]:
        s = lib_ram + off + 0x40 + 16
        if ram[s:s + len(blocks[0])] != blocks[0]:
            raise SystemExit(f"library model {m:#x} differs from RAM at D_0028A56C = {lib_ram:#x}")
    print(f"verified against {ram_path}: D_0028A5A0 = {bank_ram:#x} = f12_id44 + {BANK_OFFSET:#x}, "
          f"{span} bank bytes, grid and {len(x['objects'])} objects equal; D_0028A56C = {lib_ram:#x}, "
          f"models {', '.join(hex(m) for m in BOX_MODELS)} equal")


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--extract", type=Path, default=ROOT / "extract")
    ap.add_argument("--out", type=Path, required=True)
    ap.add_argument("--verify-ram", type=Path, action="append", default=[],
                    help="a captured EE RAM image to check against (repeatable)")
    args = ap.parse_args(argv)
    x = build(args.extract)
    for path in args.verify_ram:
        verify_ram(x, path if path.is_absolute() else ROOT / path)
    data = serialize(x)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_bytes(data)
    batches = sum(o[2] for o in x["objects"])
    print(f"wrote {args.out}: grid {x['rows']} x {x['stride']} cells x 4 ids ({len(x['used'])} ids used), "
          f"{len(x['objects'])} objects / {batches} batches, box models "
          f"{', '.join(hex(m) for m in BOX_MODELS)}; {len(data)} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main())
