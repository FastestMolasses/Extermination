#!/usr/bin/env python3
"""collision_probe.py - decode, compare and report the COLLISION data carried
by the `id 0x44` level files (and validate it against live EE-RAM dumps).

Verdict context (docs/FINDINGS.md "COLLISION WORLD"): the id 0x44 file is not
drawn; it carries the level's *collision world*. Three runtime-confirmed
representations live in (or are pointed into) the file:

  1. **Convex n-gon prims** inside *cells*: a cell list is
     `u32 count` + `count` u32 entries (top bits 0xC0000000 are runtime
     stream/active flags; low 30 bits = byte offset from the list base;
     0 = empty cell). A cell is a 6-float world AABB (min xyz, max xyz),
     `s16 prim_count` at +0x18, prims from +0x1C. Prim header u16 at +0:
     type = hdr & 0xF000, "wide" flag = hdr & 0x800, vertex count = byte +2.

       type 0x1000 (n-gon, walked by func_001A4030):
         +0x04 plane normal (3f) + plane d (f)
         +0x14 cnt * vec3f vertices            [narrow: + cnt*vec3f edge
         normals; record size 0x14 + cnt*0x18] [wide: edge vectors + edge
         normals follow; record size 0x24 + cnt*0x30]
       type 0x2000 (size 0x1C, func_001A50A0) and
       type 0x4000 (size 0x18 / wide 0x2C, func_001A5C30): smaller fixed
         prims (sphere/edge-like; not yet fully decoded)
       type 0x8000 (size 0x14 / wide 0x24): skipped by the polygon walker;
         belongs to the s16-grid path

  2. **s16-quantized grid / heightfield sections** (the high-entropy region
     a level file opens with), queried via per-axis binary search over
     sorted s16 node tables + a float boundary array (func_0019F1A0 →
     func_0019ED80, result kind 4).

  3. **Movable-object hulls**: objects registered in the table at
     `D_00275B8C` (count `D_00275B94`) carry a polygon list at +0x58:
     `u32 count` header (s16 at +0xA == -2 → skip first record), then
     variable records: 3 layer bytes + matrix-index byte, u16 size
     (= 0x18 + 24*N), s16 N, plane normal at +8, plane d at +0x14,
     N vec3f vertices at +0x18, N vec3f edge normals after. The matrix
     index selects *(obj + 0x110 + idx*4) + 0x90 (a 4x4) so door-like
     sub-objects move. Walked by func_001A6440 / func_001A6AD0.

Subcommands
-----------
  decode  <id44.bin>                  locate + decode cell lists and prims
  compare <chunkdir> [--render F]     decode collision, compare against the
          [--at X,Z [Y]]              chunk's render mesh (export_level
                                      record walk): bbox overlap, vertex
                                      proximity, floor probe at X,Z
  ram     <ee_ram.bin> [--spr F]      walk the live runtime tables in an
                                      EE-RAM dump (parse_pcsx2_state.py
                                      output): object table 0x275B8C,
                                      world-section directory 0x28A598,
                                      scratchpad query state 0x700031xx

Disc-data safety: reads the user's locally-extracted files / save-state
dumps; never writes disc bytes anywhere. Runs natively on arm64 macOS.
"""
from __future__ import annotations

import argparse
import math
import struct
import sys
from collections import Counter
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

# Runtime layout constants (boot ELF SCUS-97112, see FINDINGS "COLLISION WORLD")
OBJ_TABLE_PTR = 0x275B8C      # -> u32* object pointers
OBJ_TABLE_CNT = 0x275B94      # s16 count
SECT_DIR = 0x28A598           # 16 u32 world-section pointers (bss)
SPR_QUERY = {                 # scratchpad query block offsets
    "seg_start": 0x3190, "seg_end": 0x31A0, "hit_point": 0x31B0,
    "hit_delta": 0x31C0, "result_ptr": 0x31D0, "hit_flag": 0x31D4,
    "hit_kind": 0x31D8, "hit_id": 0x324E, "cells_ptr": 0x3250,
}
EE_LO, EE_HI = 0x100000, 0x2000000


# ---------------------------------------------------------------------------
# Cell / prim decoding (disc id 0x44 files and RAM images alike)

PRIM_TYPES = (0x1000, 0x2000, 0x4000, 0x8000)


def prim_size(hdr: int, cnt: int) -> int | None:
    t, wide = hdr & 0xF000, hdr & 0x800
    if t == 0x1000:
        return (0x24 + cnt * 0x30) if wide else (0x14 + cnt * 0x18)
    if t == 0x2000:
        return 0x1C
    if t == 0x4000:
        return 0x2C if wide else 0x18
    if t == 0x8000:
        return 0x24 if wide else 0x14
    return None


def walk_cell(d: bytes, c: int):
    """Decode one cell at byte offset c. Returns (aabb, prims) or None.
    prims = list of (offset, hdr, type, polys) where polys is
    (plane(nx,ny,nz,dd), [verts]) for type 0x1000, else None."""
    if c + 0x1C > len(d):
        return None
    aabb = struct.unpack_from("<6f", d, c)
    if not (aabb[0] <= aabb[3] and aabb[1] <= aabb[4] and aabb[2] <= aabb[5]):
        return None
    if not all(abs(v) < 1e5 for v in aabb):
        return None
    (n,) = struct.unpack_from("<h", d, c + 0x18)
    if not 0 <= n <= 4096:
        return None
    o, prims = c + 0x1C, []
    for _ in range(n):
        if o + 4 > len(d):
            return None
        (hdr,) = struct.unpack_from("<H", d, o)
        cnt = d[o + 2]
        sz = prim_size(hdr, cnt)
        if sz is None or o + sz > len(d):
            return None
        poly = None
        if (hdr & 0xF000) == 0x1000 and 3 <= cnt <= 64:
            plane = struct.unpack_from("<4f", d, o + 4)
            nl = plane[0] ** 2 + plane[1] ** 2 + plane[2] ** 2
            if 0.98 < nl < 1.02:
                verts = [struct.unpack_from("<3f", d, o + 0x14 + 12 * k)
                         for k in range(cnt)]
                poly = (plane, verts)
        prims.append((o, hdr, hdr & 0xF000, poly))
        o += sz
    return aabb, prims


def find_cell_lists(d: bytes):
    """Scan for cell-list structures: u32 N + N offset words whose nonzero
    low-30-bit offsets are increasing and decode as valid cells."""
    out, o, L = [], 0, len(d)
    while o < L - 8:
        (N,) = struct.unpack_from("<I", d, o)
        if 2 <= N <= 4096 and o + 4 + 4 * N <= L:
            ents = struct.unpack_from(f"<{N}I", d, o + 4)
            offs = [e & 0x3FFFFFFF for e in ents]
            nz = [x for x in offs if x]
            if (len(nz) >= 2 and nz == sorted(nz) and nz[0] >= 4 + 4 * N
                    and o + nz[-1] + 0x1C <= L):
                cells = []
                for x in nz:
                    cell = walk_cell(d, o + x)
                    if cell is None:
                        cells = None
                        break
                    cells.append((x, cell))
                if cells:
                    out.append((o, N, cells))
                    o += 4 + 4 * N
                    continue
        o += 4
    return out


def scan_loose_ngons(d: bytes):
    """Find type-0x1000 prims by signature alone (used when a level's cell
    list header escapes find_cell_lists): planar unit-normal n-gons."""
    out, o = [], 0
    while o < len(d) - 0x44:
        (hdr,) = struct.unpack_from("<H", d, o)
        if (hdr & 0xF000) == 0x1000:
            cnt = d[o + 2]
            if 3 <= cnt <= 16:
                sz = prim_size(hdr, cnt)
                if o + sz <= len(d):
                    pl = struct.unpack_from("<4f", d, o + 4)
                    nl = pl[0] ** 2 + pl[1] ** 2 + pl[2] ** 2
                    if 0.999 < nl < 1.001 and abs(pl[3]) < 1e6:
                        vs = [struct.unpack_from("<3f", d, o + 0x14 + 12 * k)
                              for k in range(cnt)]
                        if (all(abs(c) < 1e5 for v in vs for c in v) and
                            all(abs(v[0] * pl[0] + v[1] * pl[1] +
                                    v[2] * pl[2] - pl[3]) < 0.05 for v in vs)):
                            out.append((o, hdr, pl, vs))
                            o += sz
                            continue
        o += 4
    return out


def harvest_polys(d: bytes):
    """All decodable collision n-gons in a file: via cell lists first,
    falling back to the loose scan. Returns (polys, lists, hist).
    polys = [(plane, verts)], hist = prim-type Counter."""
    lists = find_cell_lists(d)
    polys, hist = [], Counter()
    for _, _, cells in lists:
        for _, (_aabb, prims) in cells:
            for _, _, t, poly in prims:
                hist[t] += 1
                if poly:
                    polys.append(poly)
    if not polys:
        for _, _, pl, vs in scan_loose_ngons(d):
            hist[0x1000] += 1
            polys.append((pl, vs))
    return polys, lists, hist


# ---------------------------------------------------------------------------
# Reporting helpers

def bbox(pts):
    xs = [p[0] for p in pts]
    ys = [p[1] for p in pts]
    zs = [p[2] for p in pts]
    return (min(xs), min(ys), min(zs), max(xs), max(ys), max(zs))


def fmt_bbox(b):
    return "X[%.1f,%.1f] Y[%.1f,%.1f] Z[%.1f,%.1f]" % (
        b[0], b[3], b[1], b[4], b[2], b[5])


def bbox_iou(a, b):
    ix = max(0.0, min(a[3], b[3]) - max(a[0], b[0]))
    iy = max(0.0, min(a[4], b[4]) - max(a[1], b[1]))
    iz = max(0.0, min(a[5], b[5]) - max(a[2], b[2]))
    inter = ix * iy * iz
    va = (a[3] - a[0]) * (a[4] - a[1]) * (a[5] - a[2])
    vb = (b[3] - b[0]) * (b[4] - b[1]) * (b[5] - b[2])
    return inter / (va + vb - inter) if (va + vb - inter) > 0 else 0.0


def point_in_poly_xz(verts, px, pz):
    inside = False
    for i in range(len(verts)):
        x1, _, z1 = verts[i]
        x2, _, z2 = verts[(i + 1) % len(verts)]
        if (z1 > pz) != (z2 > pz):
            if x1 + (pz - z1) * (x2 - x1) / (z2 - z1) > px:
                inside = not inside
    return inside


# ---------------------------------------------------------------------------
# Subcommands

def cmd_decode(args):
    d = Path(args.file).read_bytes()
    polys, lists, hist = harvest_polys(d)
    print(f"{args.file}: {len(d)} bytes")
    for base, n, cells in lists:
        cv = [v for _, (_a, pr) in cells for _, _, t, p in pr if p
              for v in p[1]]
        cb = fmt_bbox(bbox(cv)) if cv else "(no n-gon verts)"
        print(f"  cell list @0x{base:X}: {n} cells "
              f"({sum(1 for _, e in cells)} non-empty)  n-gon bbox {cb}")
    print(f"  prim types: " + ", ".join(
        f"0x{t:04X} x{c}" for t, c in sorted(hist.items())))
    if polys:
        verts = [v for _, vs in polys for v in vs]
        print(f"  n-gons: {len(polys)} ({len(verts)} verts), "
              f"bbox {fmt_bbox(bbox(verts))}")
        up = sum(1 for pl, _ in polys if pl[1] > 0.9)
        print(f"  up-facing (floor) n-gons: {up}")
    else:
        print("  no decodable n-gons (s16-grid-only file or not collision)")


def cmd_compare(args):
    chunk = Path(args.chunkdir)
    id44 = sorted(chunk.glob("*_id44.bin"))
    if not id44:
        sys.exit(f"no *_id44.bin under {chunk}")
    coll = id44[0].read_bytes()
    polys, lists, hist = harvest_polys(coll)
    cverts = [v for _, vs in polys for v in vs]
    print(f"collision file: {id44[0].name}  "
          f"({len(lists)} cell lists, {sum(hist.values())} prims, "
          f"{len(polys)} n-gons, {len(cverts)} verts)")
    if not cverts:
        sys.exit("no decodable collision n-gons; nothing to compare")
    cb = bbox(cverts)
    print(f"  collision n-gon bbox: {fmt_bbox(cb)}")

    # render mesh (export_level record walk)
    import export_level as el
    rpath = Path(args.render) if args.render else None
    if rpath is None:
        for cand in sorted(chunk.glob("*_id4*.bin")):
            if cand == id44[0]:
                continue
            recs = [r for r in el.walk_records(cand.read_bytes()) if r]
            if len(recs) > 500:
                rpath, rrecs = cand, recs
                break
        else:
            sys.exit("no render-mesh candidate found; pass --render")
    else:
        rrecs = [r for r in el.walk_records(rpath.read_bytes()) if r]
    rverts = [r[0] for r in rrecs]
    rb = bbox(rverts)
    print(f"render mesh:    {rpath.name}  ({len(rverts)} records, "
          f"verbatim per-record TEX0: yes)")
    print(f"  render bbox:          {fmt_bbox(rb)}")
    print(f"  bbox IoU: {bbox_iou(cb, rb):.2f}   collision-inside-render: "
          f"{all(cb[i] >= rb[i] - 2 for i in range(3)) and all(cb[i] <= rb[i] + 2 for i in range(3, 6))}")

    # vertex proximity: collision verts vs nearest render vert (XZ-hashed)
    cell = 4.0
    grid: dict[tuple, list] = {}
    for v in rverts:
        grid.setdefault((int(v[0] // cell), int(v[2] // cell)), []).append(v)
    near = far = 0
    dists = []
    for v in cverts:
        kx, kz = int(v[0] // cell), int(v[2] // cell)
        best = 1e9
        for dx in (-1, 0, 1):
            for dz in (-1, 0, 1):
                for r in grid.get((kx + dx, kz + dz), ()):
                    dd = ((v[0] - r[0]) ** 2 + (v[1] - r[1]) ** 2 +
                          (v[2] - r[2]) ** 2)
                    if dd < best:
                        best = dd
        if best < 1e9:
            dists.append(math.sqrt(best))
    thr = 2.0
    near = sum(1 for x in dists if x <= thr)
    if dists:
        dists.sort()
        print(f"  collision verts within {thr}u of a render vert: "
              f"{near}/{len(cverts)} ({100 * near / len(cverts):.0f}%), "
              f"median dist {dists[len(dists) // 2]:.2f}u")

    if args.at:
        parts = [float(x) for x in args.at.split(",")]
        px, pz = parts[0], parts[1]
        hits = []
        for pl, vs in polys:
            if pl[1] > 0.9 and point_in_poly_xz(vs, px, pz):
                hits.append((pl[3] - pl[0] * px - pl[2] * pz) / pl[1])
        print(f"  floor probe at ({px:.1f},{pz:.1f}): "
              + (f"n-gon floor y = {sorted(hits)}" if hits else
                 "no up-facing n-gon (surface owned by the s16-grid path)"))


def cmd_ram(args):
    ee = Path(args.ee).read_bytes()

    def u32(o):
        return struct.unpack_from("<I", ee, o)[0]

    ptr, cnt = u32(OBJ_TABLE_PTR), struct.unpack_from("<h", ee, OBJ_TABLE_CNT)[0]
    print(f"object table: ptr=0x{ptr:X} count={cnt}")
    if EE_LO <= ptr < EE_HI:
        for k in range(min(cnt, 64)):
            op = u32(ptr + 4 * k)
            if not EE_LO <= op < EE_HI:
                continue
            pl = u32(op + 0x58)
            line = f"  obj[{k}] @0x{op:X} flags=0x{ee[op]:02X}"
            if EE_LO <= pl < EE_HI:
                pc = u32(pl)
                line += f" hull@0x{pl:X} polys={pc}"
                o, good = pl + 4, 0
                for _ in range(min(pc, 64)):
                    size, n = struct.unpack_from("<Hh", ee, o + 4)
                    if size != 0x18 + 24 * n:
                        break
                    pn = struct.unpack_from("<3f", ee, o + 8)
                    if abs(math.sqrt(sum(c * c for c in pn)) - 1) > 0.02:
                        break
                    good += 1
                    o += size
                line += f" (layout-valid: {good}/{pc})"
            print(line)
    print(f"world-section directory @0x{SECT_DIR:X}:")
    for i in range(16):
        p = u32(SECT_DIR + 4 * i)
        if p:
            print(f"  [{i}] 0x{p:X}  head: {ee[p:p + 16].hex()}")
    if args.spr:
        sp = Path(args.spr).read_bytes()
        for name, off in SPR_QUERY.items():
            if name in ("hit_id",):
                val = struct.unpack_from("<h", sp, off)[0]
            elif name in ("result_ptr", "hit_flag", "hit_kind", "cells_ptr"):
                val = hex(struct.unpack_from("<I", sp, off)[0])
            else:
                val = tuple(round(x, 3) for x in
                            struct.unpack_from("<4f", sp, off))
            print(f"  SPR 0x7000{off:04X} {name:10s} = {val}")


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    d = sub.add_parser("decode", help="decode collision cells in an id44 file")
    d.add_argument("file")
    d.set_defaults(fn=cmd_decode)
    c = sub.add_parser("compare", help="collision vs render mesh for a chunk")
    c.add_argument("chunkdir")
    c.add_argument("--render", help="render-mesh file (default: autodetect)")
    c.add_argument("--at", help="X,Z floor probe point (e.g. 107.4,-184)")
    c.set_defaults(fn=cmd_compare)
    r = sub.add_parser("ram", help="walk runtime collision tables in EE RAM")
    r.add_argument("ee", help="ee.bin from parse_pcsx2_state.py")
    r.add_argument("--spr", help="scratchpad.bin for live query state")
    r.set_defaults(fn=cmd_ram)
    args = ap.parse_args(argv)
    args.fn(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
