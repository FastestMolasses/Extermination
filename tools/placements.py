#!/usr/bin/env python3
"""placements.py — parse the engine's authoritative object PLACEMENT TABLES
directly from the disc's OVERLAY/AREAxx.BIN modules (no live capture needed).

DISC SOURCE (decoded 2026-06-10, session 11). Overlay modules load FLAT at
EE vaddr 0x823500 (the overlay arena). Each area's placement tables are
plain static data inside its AREAxx.BIN. The engine finds them through a
two-level registry:

    desc = *(u32*)(0x0024D7C0 + 4 * D_00810700)   # area number (high byte)
    table = ((u32*)desc)[D_00810701]              # area sub-state (low byte)

`desc` (the per-area descriptor) lives either in main .data (e.g. AREA06 ->
0x275948) or inside the overlay itself (AREA02 -> 0x828C40); it is an array
of placement-table vaddrs, one per story sub-state (area code 0x0200/0x0201/
0x0202 -> desc[0..2]).

The spawner is func_001B6990: it walks the selected table in 0x28-byte
records until a record whose halfword +0x00 == 0xFF (sentinel), skipping
records with (halfword+0 & 0xFF) == 0x0B (scripted/deferred spawns queried
later by func_0019C6F0), and allocates an actor per record via
func_001AFA90, copying the fields below.

RECORD LAYOUT (0x28 bytes) — field -> actor offset per func_001B6990:

    +0x00  u16  spawn_class   (& 0xFF1F = class: 4 generic placed actor,
                               5 double door, 6 wall station, 8 door
                               assembly, 2 special-link, ...; bits 5..7 of
                               the low byte are flags, e.g. 0x84/0x85/0x86;
                               0xFF = table END sentinel; low byte 0x0B =
                               scripted/deferred, skipped at area load)
    +0x02  u8   model         (object/model record selector -> actor+0x03;
                               e.g. office: 0x36 control panel, 0x37 wall
                               station, 0x2C battery device, 0x38 lockers,
                               0x03 double door)
    +0x03  u8   flags2        (-> actor+0x2E; door variants 0x81/0x02/...)
    +0x04  u16  param         (-> actor+0x0D / matched by func_0019C6F0;
                               for kind-0xB pickups this is the ITEM TYPE:
                               office 0x0B supply crate (blob 0xA2740),
                               0x0C ammo box (blob 0xA3040), 0x0D ammo box
                               (blob 0xA3940))
    +0x06  u16  uid           (-> actor+0x0E; high byte = per-area unique
                               id, indexes the pickup/object state words in
                               scratchpad 0x70003250 via func_0019C6F0)
    +0x08  u16  kind          (-> actor+0x54: 4 = placed object/door,
                               0xB = item pickup, 0xD/0xE = enemy spawn
                               kinds, 0x46 = fixture, 3/8/0xA/0x52 = others)
    +0x0A  u16  link          (-> actor+0x56; 0xFFFF = none; doors carry
                               room/door link ids + flags here)
    +0x0C  f32  pos[3]        (-> actor+0xB0..B8, world units)
    +0x18  f32  rot[3]        (-> actor+0xC0..C8, Euler radians; ry = yaw;
                               rx/rz almost always 0)
    +0x24  u32  behavior      (-> actor+0x10: actor behavior FUNCTION
                               pointer in the main ELF or the overlay; e.g.
                               0x001C4820 = generic placed-prop/pickup
                               state machine, 0x001BC350 = door)

NOTE the FINDINGS s9 live read of this table was shifted +8 bytes (it took
+0x08 `kind` for the leading "type word"); same data, corrected alignment.

Disc-data safety: this tool READS user-extracted overlay files and prints
or writes JSON to user-chosen scratch paths only. Never commit its output.

Usage (macOS arm64, repo root):
  .venv/bin/python tools/placements.py extract/OVERLAY/AREA02.BIN          # known tables
  .venv/bin/python tools/placements.py extract/OVERLAY/AREA02.BIN --scan   # find tables
  .venv/bin/python tools/placements.py extract/OVERLAY/AREA02.BIN \
      --vaddr 0x828170 --json /tmp/office_placements.json
"""
from __future__ import annotations

import argparse
import json
import math
import struct
from dataclasses import dataclass, asdict
from pathlib import Path

ARENA_VADDR = 0x823500
REC_SIZE = 0x28
SENTINEL = 0xFF

# Placement-table vaddrs per overlay, indexed by area sub-state
# (D_00810701).  Recovered from the 0x24D7C0 registry + per-area
# descriptors; verified byte-identical between disc and live EE RAM
# (save states 01/03).
KNOWN_TABLES = {
    "AREA02.BIN": [0x827830, 0x828170, 0x8283D0],   # office = state 1
    "AREA06.BIN": [0x827AC0, 0x8283D0],             # snow level
    "AREA11.BIN": [0x82A3C0],
}


@dataclass
class Placement:
    index: int
    vaddr: int
    spawn_class: int
    model: int
    flags2: int
    param: int
    uid: int
    kind: int
    link: int
    pos: tuple
    rot: tuple
    behavior: int

    @property
    def uid_byte(self) -> int:
        """Per-area unique id (scratchpad state index)."""
        return (self.uid >> 8) & 0xFF

    def matrix34(self):
        """3x4 affine world placement [R|t]. rx/rz are almost always 0;
        when nonzero we compose Ry*Rx*Rz (order unverified against the
        engine — assert-warn upstream if it matters)."""
        rx, ry, rz = self.rot
        def m_rx(a):
            c, s = math.cos(a), math.sin(a)
            return [[1, 0, 0], [0, c, -s], [0, s, c]]
        def m_ry(a):
            c, s = math.cos(a), math.sin(a)
            return [[c, 0, s], [0, 1, 0], [-s, 0, c]]
        def m_rz(a):
            c, s = math.cos(a), math.sin(a)
            return [[c, -s, 0], [s, c, 0], [0, 0, 1]]
        def mul(a, b):
            return [[sum(a[i][k] * b[k][j] for k in range(3))
                     for j in range(3)] for i in range(3)]
        r = m_ry(ry)
        if rx:
            r = mul(r, m_rx(rx))
        if rz:
            r = mul(r, m_rz(rz))
        return [[r[0][0], r[0][1], r[0][2], self.pos[0]],
                [r[1][0], r[1][1], r[1][2], self.pos[1]],
                [r[2][0], r[2][1], r[2][2], self.pos[2]]]


def parse_record(data: bytes, off: int, index: int) -> Placement:
    sc, model, flags2, param, uid, kind, link = struct.unpack_from(
        "<HBBHHHH", data, off)
    pos = struct.unpack_from("<3f", data, off + 0x0C)
    rot = struct.unpack_from("<3f", data, off + 0x18)
    behavior, = struct.unpack_from("<I", data, off + 0x24)
    return Placement(index, ARENA_VADDR + off, sc, model, flags2, param,
                     uid, kind, link, pos, rot, behavior)


def parse_table(data: bytes, vaddr: int, max_entries: int = 512) -> list[Placement]:
    """Parse one sentinel-terminated placement table at overlay vaddr."""
    off = vaddr - ARENA_VADDR
    if not (0 <= off < len(data)):
        raise ValueError(f"vaddr {vaddr:#x} outside overlay "
                         f"(arena {ARENA_VADDR:#x} + {len(data):#x})")
    out: list[Placement] = []
    for i in range(max_entries):
        o = off + i * REC_SIZE
        if o + REC_SIZE > len(data):
            raise ValueError(f"table at {vaddr:#x} ran off end of file "
                             f"without 0xFF sentinel")
        sc, = struct.unpack_from("<H", data, o)
        if sc == SENTINEL:
            return out
        out.append(parse_record(data, o, i))
    raise ValueError(f"no sentinel within {max_entries} records at {vaddr:#x}")


def _plausible(data: bytes, off: int) -> bool:
    sc, = struct.unpack_from("<H", data, off)
    if sc == 0 or sc > 0x200 or sc == SENTINEL:
        return False
    fs = struct.unpack_from("<6f", data, off + 0x0C)
    if not all(f == f and abs(f) < 1e5 for f in fs[:3]):
        return False
    if not all(f == f and abs(f) < 7.0 for f in fs[3:]):
        return False
    ptr, = struct.unpack_from("<I", data, off + 0x24)
    return ptr == 0 or 0x100000 <= ptr < 0x2000000


def find_tables(data: bytes, min_entries: int = 3) -> list[int]:
    """Heuristic scan: vaddrs of sentinel-terminated record runs."""
    found, skip_until = [], 0
    for off in range(0, len(data) - REC_SIZE, 4):
        if off < skip_until or not _plausible(data, off):
            continue
        n, o = 0, off
        while o + REC_SIZE <= len(data) and _plausible(data, o):
            n += 1
            o += REC_SIZE
        sc = struct.unpack_from("<H", data, o)[0] if o + 2 <= len(data) else -1
        if n >= min_entries and sc == SENTINEL:
            found.append(ARENA_VADDR + off)
            skip_until = o + REC_SIZE
    return found


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("overlay", type=Path, help="extracted OVERLAY/AREAxx.BIN")
    ap.add_argument("--vaddr", type=lambda s: int(s, 0), action="append",
                    help="placement-table vaddr(s); default: known tables "
                         "for this overlay, else --scan")
    ap.add_argument("--scan", action="store_true",
                    help="heuristically scan the overlay for tables")
    ap.add_argument("--json", type=Path,
                    help="write entries as JSON to this (scratch) path")
    args = ap.parse_args(argv)

    data = args.overlay.read_bytes()
    vaddrs = args.vaddr or ([] if args.scan else
                            KNOWN_TABLES.get(args.overlay.name, []))
    if args.scan or not vaddrs:
        vaddrs = find_tables(data)
        print(f"scan: {len(vaddrs)} sentinel-terminated tables: "
              + ", ".join(f"{v:#x}" for v in vaddrs))

    blob = {}
    for v in vaddrs:
        entries = parse_table(data, v)
        print(f"\n=== table @{v:#x} ({len(entries)} entries) ===")
        for e in entries:
            print(f"[{e.index:3d}] cls={e.spawn_class:04x} model={e.model:02x}"
                  f" fl={e.flags2:02x} param={e.param:04x} uid={e.uid:04x}"
                  f" kind={e.kind:04x} link={e.link:04x}"
                  f" pos=({e.pos[0]:8.2f},{e.pos[1]:7.2f},{e.pos[2]:8.2f})"
                  f" rot=({e.rot[0]:6.3f},{e.rot[1]:6.3f},{e.rot[2]:6.3f})"
                  f" fn={e.behavior:08x}")
        blob[f"{v:#x}"] = [asdict(e) for e in entries]
    if args.json:
        args.json.write_text(json.dumps(blob, indent=2))
        print(f"\nwrote {args.json}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
