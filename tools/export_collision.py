#!/usr/bin/env python3
"""export_collision.py - bake an id 0x44 level COLLISION WORLD into EMCL,
the native port's collision container.

Sources decoded (docs/FINDINGS.md "COLLISION WORLD"):

  1. The "s16-grid heightfield" section -- DECODED (this tool's session):
     it is NOT a quantized heightfield. It is a polygon soup with a
     rank-table acceleration index (the s16 tables that looked like
     quantized heights are sorted node-index lists + per-node rank bounds).
     Section header (located by signature scan; the runtime base is
     world-section directory entry [0] at bss 0x28A598, staged to
     scratchpad by func_00199C50):

       +0x00 u32 off -> vertex pool (vec3f)            (SPR 0x700031FC)
       +0x04 u32 vertex count
       +0x08 u32 off -> edge-normal pool (vec3f)       (SPR 0x70003200)
       +0x0C u32 edge-normal count
       +0x10 u32 off -> vertex-index pool (s16)        (SPR 0x70003204)
       +0x14 u32 index count
       +0x18 u32 off -> sorted rank tables             (SPR 0x70003210)
                        12 contiguous s16[N] tables (6 sort dirs + 6
                        range helpers, func_0019F1A0/func_0019D330)
       +0x1C u32 form (0xC = helper tables present)
       +0x20 u32 off -> node array                     (SPR 0x70003208)
       +0x24 s16 node count N                          (SPR 0x7000320C)

     Node = 64 bytes (func_0019F1A0 / func_0019D330 / func_0019ED80):
       +0x00 s16[6]  boundary vertex index per sort direction
       +0x0C s16[6]  rank bounds per direction (accel only)
       +0x18 u8      polygon vertex count
       +0x1A u8      surface attr (0x50..0x59 = conditional surfaces,
                     gated against the query id at SPR 0x7000324E)
       +0x1C u32     off into the vertex-index pool (count s16 entries)
       +0x20 u32     off into the edge-normal pool (count vec3f)
       +0x24 f32[3]  plane normal     +0x30 f32 plane d

  2. The convex n-gon CELL world (cell list, walked by func_001A0B10 /
     func_001A4030) via tools/collision_probe.py. Narrow prim (type
     0x1000): plane @+4, verts @+0x14, edge normals @+0x14+12*cnt. Wide
     prim (hdr bit 0x800): same layout, then a second mirrored copy of
     the whole record (plane/verts/edges again) -- the first copy is
     exported. Types 0x2000/0x4000/0x8000 are not exported (small prims /
     grid-path duplicates, still undecoded).

EMCL v1 (little endian, our own original container):

  0x00 char[4] "EMCL"        0x04 u32 version = 1
  0x08 u32 vert_count        0x0C u32 poly_count
  0x10 u32 index_count       0x14 u32 flags (bit0 = grid section decoded,
                             bit1 = grid pad byte is the node class,
                             bit2 = the grid rank section follows)
  0x18 f32[6] world bbox (min xyz, max xyz)
  0x30 f32 verts[vert_count*3]
       poly records, 24 B each:
         f32 plane[4]; u32 first (base into index/edge pools);
         u8 vcount; u8 set (2 = cell world, 4 = grid world -- the engine's
         collision-set bits); u8 attr; u8 pad (grid: node +0x1B, the class
         byte, when flags bit1; cells: 0)
       u16 indices[index_count] (+2 B pad if odd)
       f32 edge_normals[index_count*3]

  Grid node class (flags bit1). Every consumer of a grid hit reads the class
  as `*(u16 *)(node + 0x1A) & 0xFF00` (00175CF0, 00176BE0, 001791D0,
  0015DEC0): it is authored per node (+0x1B), not derived from the normal.

  Grid rank section (flags bit2; the port's src/game/em_coll_probe_original.c
  reads it), appended after the edge normals, every field in the level's own
  RAM layout so --verify-ram can compare it byte for byte:
    char[4] "EMRK"; u32 version = 1
    u32 N (grid nodes); u32 first grid poly index in this EMCL (grid node i
        is poly first + i); u32 V (grid vertex pool count)
    f32 grid_verts[V*3]   the grid's own vertex pool (*0x700031FC), which
                          0019F1A0 indexes by node +0x00 (the pool above is
                          de-duplicated, so it cannot stand in for it)
    s16 node_words[N][12] node +0x00..+0x17: the six boundary vertex indices
                          and the six rank bounds (0019DF10 / 0019E640 test
                          +0x0C/+0x0E/+0x14/+0x16)
    s16 tables[12][N]     the 12 contiguous tables at header +0x18: tables
                          0..5 are *(0x70003210 + 4k) (sorted node indices,
                          0019F1A0), 6..11 are *(0x70003228 + 4k)
                          (0019DF10 / 0019E640 span helpers)
    (+2 B pad to a 4-byte boundary)

  --verify-ram EEMEMORY checks the decoded grid against a captured EE RAM
  image (the route beats' eeMemory.bin): the world-section directory
  D_0028A598 entry 0 must name the same header, and node bytes
  +0x00..+0x33 (words, count, attr, class, pool offsets, plane), the vertex,
  vertex-index and edge-normal pools and all 12 tables must equal RAM; with a
  scratchpad.bin beside it, the staged pointers 0x700031FC..0x7000323C and
  the count 0x7000320C must name them too.

MULTI-FILE LEVELS (generalization, 2026-06-10 snow-level session): the
extraction splits each level chunk into f00..fNN files, but the engine
loads them CONTIGUOUSLY into EE RAM, and a collision section may SPAN
file boundaries. chunk15 (the AREA11 snow level, save state 01): the grid
section header sits at f07_id52.bin+0x1800 and its vertex/index/node
pools run on through f08/f09/f10/f11 into f12_id44.bin (live-verified:
directory[0] @0x28A598 = EE 0x13BB740 = f07 base + 0x1800; node array =
f12+0x8E2C). So this tool accepts MULTIPLE input files and decodes the
byte-concatenation — pass the chunk's files in index order. A single-file
call (the office) is unchanged.

SCENE MANIFEST: the bake always runs in TRUE world coordinates (the old
--offset spawn-anchoring is gone; the manifest spawn replaced it). After
writing the EMCL this tool records its own filename in the scene's
`scene.txt` (`collision <file.emcl>` — the port's per-scene boot config,
read by em_game.c; spawn/bgm keys come from export_level.py). `--at`
probes in world coordinates.

Disc-data safety: reads the user's locally extracted files, writes only to
git-ignored output paths. Runs natively on arm64 macOS.

Usage:
  python3 tools/export_collision.py extract/chunk06.n1/f02_id44.bin \
      -o ../extermination-port/assets/scene/office.emcl --at 107.4,-184
  python3 tools/export_collision.py extract/chunk15/f07_id52.bin \
      extract/chunk15/f08_id4d.bin extract/chunk15/f09_id53.bin \
      extract/chunk15/f10_id5b.bin extract/chunk15/f11_id4a.bin \
      extract/chunk15/f12_id44.bin \
      -o ../extermination-port/assets/scene_snow/snow.emcl \
      --at 218.592,201.789
"""
from __future__ import annotations

import argparse
import math
import struct
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

import collision_probe as cp  # cell/prim decode (do not duplicate)

SET_CELLS = 2   # func_001A0B10 world (hub mask bit 1)
SET_GRID = 4    # func_0019D330 world (hub mask bit 2)


def update_manifest(scene_dir: Path, key: str, value: str) -> None:
    """Write/update one key of the SCENE MANIFEST (scene.txt) in the scene
    directory (same format/helper as export_level.py — the port's per-scene
    boot config, parsed by em_game.c scene_manifest_load). Other keys and
    comment lines are preserved."""
    mf = scene_dir / "scene.txt"
    lines = mf.read_text().splitlines() if mf.exists() else []
    lines = [ln for ln in lines if not ln.startswith(key + " ")]
    lines.append(f"{key} {value}")
    mf.write_text("\n".join(lines) + "\n")
    print(f"manifest: {mf}: {key} {value}")


# ---------------------------------------------------------------------------
# Grid ("heightfield") section decode

def find_grid_header(d: bytes):
    """Signature-scan for the world-section [0] header (form 0xC)."""
    L = len(d)
    for o in range(0, L - 0x28, 4):
        v = struct.unpack_from("<9I", d, o)
        if v[7] != 0xC:
            continue
        (n,) = struct.unpack_from("<h", d, o + 0x24)
        offs = (v[0], v[2], v[4], v[6], v[8])
        if not 0 < n < 0x8000:
            continue
        if any(x == 0 or x & 3 or o + x >= L for x in offs):
            continue
        if list(offs) != sorted(offs):
            continue
        # cross-check: pool counts must match the pool spans
        if v[1] != (v[2] - v[0]) // 12 or v[5] != (v[6] - v[4]) // 2:
            continue
        if o + v[8] + 64 * n > L:
            continue
        return o
    return None


def decode_grid(d: bytes, base: int):
    """Decode the grid section at `base`. Returns (verts, polys) where
    polys = [(plane4, [vert_idx], [edge_normal_vec3], attr)] with vert_idx
    into the returned verts list."""
    h = struct.unpack_from("<9I", d, base)
    vert_off, vcnt, en_off, _encnt, il_off, _ilcnt, _tab, _form, node_off = h
    (n_nodes,) = struct.unpack_from("<h", d, base + 0x24)
    verts = [struct.unpack_from("<3f", d, base + vert_off + 12 * i)
             for i in range(vcnt)]
    polys = []
    for i in range(n_nodes):
        o = base + node_off + 64 * i
        cnt, _b19, attr = d[o + 0x18], d[o + 0x19], d[o + 0x1A]
        iloff, enoff = struct.unpack_from("<2I", d, o + 0x1C)
        plane = struct.unpack_from("<4f", d, o + 0x24)
        idxs = list(struct.unpack_from(f"<{cnt}h", d, base + il_off + iloff))
        ens = [struct.unpack_from("<3f", d, base + en_off + enoff + 12 * k)
               for k in range(cnt)]
        polys.append((plane, idxs, ens, attr))
    return verts, polys


def decode_grid_ranks(d: bytes, base: int):
    """The grid data the EMCL poly records do not carry: each node's class
    byte (+0x1B) and its twelve s16 words +0x00..+0x17, the raw grid vertex
    pool, and the 12 contiguous s16[N] tables at header +0x18 (form 0xC)."""
    h = struct.unpack_from("<9I", d, base)
    vert_off, vcnt, _en_off, _encnt, _il_off, _ilcnt, tab_off, form, node_off = h
    (n_nodes,) = struct.unpack_from("<h", d, base + 0x24)
    if form != 0xC:
        raise SystemExit(f"grid form {form:#x}: no span helper tables")
    nodes = [base + node_off + 64 * i for i in range(n_nodes)]
    return {
        "classes": bytes(d[o + 0x1B] for o in nodes),
        "words": b"".join(d[o:o + 0x18] for o in nodes),
        "verts": bytes(d[base + vert_off:base + vert_off + 12 * vcnt]),
        "vert_count": vcnt,
        "tables": bytes(d[base + tab_off:base + tab_off + 12 * 2 * n_nodes]),
        "count": n_nodes,
    }


def rank_section(ranks, first_poly: int) -> bytes:
    """The EMCL grid rank section (module docstring)."""
    out = bytearray(b"EMRK")
    out += struct.pack("<4I", 1, ranks["count"], first_poly, ranks["vert_count"])
    out += ranks["verts"] + ranks["words"] + ranks["tables"]
    if len(out) & 3:
        out += b"\0" * (4 - (len(out) & 3))
    return bytes(out)


def verify_ram(d: bytes, gbase: int, ram_path: Path) -> None:
    """Compare the grid decoded from the disc files with a captured EE RAM
    image (module docstring, --verify-ram)."""
    ram = ram_path.read_bytes()
    u32 = lambda buf, a: struct.unpack_from("<I", buf, a)[0]
    rbase = u32(ram, 0x28A598)
    h = struct.unpack_from("<9I", d, gbase)
    if struct.unpack_from("<9I", ram, rbase) != h or ram[rbase + 0x24:rbase + 0x26] != d[gbase + 0x24:gbase + 0x26]:
        raise SystemExit(f"{ram_path}: D_0028A598[0] = {rbase:#x} is not this grid header")
    vert_off, vcnt, en_off, encnt, il_off, ilcnt, tab_off, _form, node_off = h
    (n,) = struct.unpack_from("<h", d, gbase + 0x24)
    spans = ((vert_off, 12 * vcnt, "vertex pool"), (en_off, 12 * encnt, "edge-normal pool"),
             (il_off, 2 * ilcnt, "vertex-index pool"), (tab_off, 24 * n, "12 rank tables"))
    for off, size, what in spans:
        if ram[rbase + off:rbase + off + size] != d[gbase + off:gbase + off + size]:
            raise SystemExit(f"{ram_path}: grid {what} differs from RAM")
    for i in range(n):
        o = node_off + 64 * i
        # +0x00..+0x33: the words, count, attr, class, the pool offsets and
        # the plane (+0x34.. is not read by any routine the EMCL serves).
        if ram[rbase + o:rbase + o + 0x34] != d[gbase + o:gbase + o + 0x34]:
            raise SystemExit(f"{ram_path}: grid node {i} differs from RAM")
    note = ""
    spad_path = ram_path.parent / "scratchpad.bin"
    if spad_path.exists():
        sp = spad_path.read_bytes()
        s = lambda a: u32(sp, a - 0x70000000)
        want = {0x700031FC: vert_off, 0x70003200: en_off, 0x70003204: il_off, 0x70003208: node_off}
        for k in range(12):
            want[0x70003210 + 4 * k] = tab_off + 2 * n * k
        for addr, off in want.items():
            if s(addr) != rbase + off:
                raise SystemExit(f"{ram_path.parent}: scratchpad {addr:#x} = {s(addr):#x}, not {rbase + off:#x}")
        if s(0x7000320C) != n:
            raise SystemExit(f"{ram_path.parent}: scratchpad 0x7000320C = {s(0x7000320C)}, not {n}")
        note = "; scratchpad 0x700031FC..0x7000323C and 0x7000320C name them"
    print(f"verified against {ram_path}: header {rbase:#x}, {n} nodes (+0x00..+0x33), "
          f"{vcnt} grid verts, {encnt} edge normals, {ilcnt} indices, 12 x {n} table entries equal{note}")


# ---------------------------------------------------------------------------
# Cell n-gon decode (narrow + wide), reusing collision_probe's cell walk

def decode_cell_ngons(d: bytes):
    """All type-0x1000 prims from every cell list as
    [(plane4, [vert_vec3], [edge_normal_vec3])]. Wide prims (hdr & 0x800)
    use the first of their two mirrored record copies."""
    out = []
    for _base, _n, cells in cp.find_cell_lists(d):
        for _off, (_aabb, prims) in cells:
            for po, hdr, t, _poly in prims:
                if t != 0x1000:
                    continue
                cnt = d[po + 2]
                if not 3 <= cnt <= 64:
                    continue
                plane = struct.unpack_from("<4f", d, po + 4)
                vs = [struct.unpack_from("<3f", d, po + 0x14 + 12 * k)
                      for k in range(cnt)]
                ens = [struct.unpack_from("<3f", d,
                                          po + 0x14 + 12 * cnt + 12 * k)
                       for k in range(cnt)]
                out.append((plane, vs, ens))
    return out


# ---------------------------------------------------------------------------
# Validation + EMCL write

def validate(verts, polys, label):
    """A poly is sound if its normal is unit and >= 3 of its verts sit on
    the stored plane. Outdoor terrain (chunk15) legitimately contains
    WARPED QUADS — 3 verts on the plane, the 4th off by up to ~20 u; the
    engine stores exactly one plane per node regardless, so these are
    counted (reported) but not failures."""
    bad_n = bad_p = warped = 0
    for plane, idxs, _ens, _attr in polys:
        nl = math.sqrt(sum(c * c for c in plane[:3]))
        if abs(nl - 1.0) > 0.01:
            bad_n += 1
        devs = [abs(sum(verts[i][k] * plane[k] for k in range(3)) - plane[3])
                for i in idxs]
        on_plane = sum(1 for v in devs if v <= 0.1)
        if on_plane < 3:
            bad_p += 1
        elif max(devs) > 0.1:
            warped += 1
    print(f"  {label}: {len(polys)} polys -- "
          f"non-unit normals: {bad_n}, off-plane verts: {bad_p}"
          + (f", warped quads: {warped}" if warped else ""))
    return bad_n == 0 and bad_p == 0


def floor_probe(verts, polys, px, pz):
    hits = []
    for plane, idxs, _ens, _attr in polys:
        if plane[1] <= 0.9:
            continue
        vs = [verts[i] for i in idxs]
        if cp.point_in_poly_xz(vs, px, pz):
            hits.append((plane[3] - plane[0] * px - plane[2] * pz) / plane[1])
    return sorted(hits)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("id44", nargs="+",
                    help="locally extracted level file(s); multiple files "
                    "are byte-concatenated in argument order (the engine's "
                    "contiguous chunk load — see module docstring)")
    ap.add_argument("-o", "--out", required=True, help="output .emcl path")
    ap.add_argument("--at", default=None,
                    help="X,Z floor-validation probe in world coordinates "
                    "(expects a hit)")
    ap.add_argument("--node-class", action="store_true",
                    help="also export the grid node class byte (+0x1B, flags "
                    "bit1) and the grid rank section (flags bit2). Off by "
                    "default: the live legacy collision reads the pad as 0 "
                    "until the coordinator binds the original floor service")
    ap.add_argument("--verify-ram", type=Path, action="append", default=[],
                    help="a captured EE RAM image (eeMemory.bin) to check the "
                    "grid against (repeatable; module docstring)")
    args = ap.parse_args(argv)

    d = b"".join(Path(p).read_bytes() for p in args.id44)
    pool: list = []          # shared vertex pool
    pool_lut: dict = {}      # exact-float dedupe
    polys: list = []         # (plane, [pool idx], [edge n], set, attr)

    def intern(v):
        i = pool_lut.get(v)
        if i is None:
            i = pool_lut[v] = len(pool)
            pool.append(v)
        return i

    flags = 0
    ranks = None
    gbase = find_grid_header(d)
    if gbase is not None:
        gverts, gpolys = decode_grid(d, gbase)
        print(f"grid section @0x{gbase:X}: {len(gverts)} verts, "
              f"{len(gpolys)} polygon nodes")
        remap = [intern(v) for v in gverts]
        # The pool de-duplicates by float equality (-0.0 == 0.0): every ring
        # vertex the EMCL names must keep the grid pool's exact bits.
        for i, v in enumerate(gverts):
            if struct.pack("<3f", *pool[remap[i]]) != struct.pack("<3f", *v):
                sys.exit(f"grid vertex {i}: the de-duplicated pool changes its bits")
        ok = validate(gverts, gpolys, "grid")
        for ram_path in args.verify_ram:
            verify_ram(d, gbase, ram_path)
        if ok:
            flags |= 1
            ranks = None
            classes = bytes(len(gpolys))
            if args.node_class:
                ranks = decode_grid_ranks(d, gbase)
                flags |= 2 | 4
                classes = ranks["classes"]
            for (plane, idxs, ens, attr), cls in zip(gpolys, classes):
                polys.append((plane, [remap[i] for i in idxs], ens,
                              SET_GRID, attr, cls))
        else:
            print("  grid validation FAILED -- exporting cells only "
                  "(flat-floor fallback in the port)")
    else:
        print("no grid section header found -- exporting cells only "
              "(flat-floor fallback in the port)")
        if args.verify_ram:
            sys.exit("--verify-ram: no grid section to verify")

    cngons = decode_cell_ngons(d)
    cpolys = [(pl, [intern(v) for v in vs], ens, SET_CELLS, 0, 0)
              for pl, vs, ens in cngons]
    cv = {i for _pl, idxs, _e, _s, _a, _c in cpolys for i in idxs}
    print(f"cell world: {len(cpolys)} n-gons ({len(cv)} verts)")
    validate(pool, [(pl, ix, e, a) for pl, ix, e, _s, a, _c in cpolys], "cells")
    polys.extend(cpolys)

    if not polys:
        sys.exit("nothing decoded -- refusing to write an empty EMCL")

    if args.at:
        px, pz = (float(x) for x in args.at.split(","))
        hits = floor_probe(pool, [(pl, ix, e, a)
                                  for pl, ix, e, _s, a, _c in polys], px, pz)
        print(f"floor probe at ({px:.1f},{pz:.1f}): "
              + (f"y = {[round(h, 3) for h in hits]}" if hits else "NO HIT"))
        if not hits:
            print("  WARNING: no walkable floor under the probe point")

    # ---- write EMCL
    xs = [v[0] for v in pool]
    ys = [v[1] for v in pool]
    zs = [v[2] for v in pool]
    bbox = (min(xs), min(ys), min(zs), max(xs), max(ys), max(zs))
    indices: list = []
    blob = bytearray()
    blob += struct.pack("<4sIIIII", b"EMCL", 1, len(pool), len(polys), 0,
                        flags)
    blob += struct.pack("<6f", *bbox)
    for v in pool:
        blob += struct.pack("<3f", *v)
    edge_blob = bytearray()
    for plane, idxs, ens, pset, attr, cls in polys:
        blob += struct.pack("<4fIBBBB", *plane, len(indices), len(idxs),
                            pset, attr, cls)
        indices.extend(idxs)
        for en in ens:
            edge_blob += struct.pack("<3f", *en)
    blob += struct.pack(f"<{len(indices)}H", *indices)
    if len(indices) & 1:
        blob += b"\0\0"
    blob += edge_blob
    struct.pack_into("<I", blob, 0x10, len(indices))
    if ranks is not None:
        blob += rank_section(ranks, 0)   # the grid polys come first

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_bytes(blob)
    print(f"wrote {out} ({len(blob)} bytes): {len(pool)} verts, "
          f"{len(polys)} polys, {len(indices)} indices, flags=0x{flags:X}, "
          f"bbox {cp.fmt_bbox(bbox)}")
    update_manifest(out.parent, "collision", out.name)
    return 0


if __name__ == "__main__":
    sys.exit(main())
