#!/usr/bin/env python3
"""gs_conformance_suite.py - the test batches of the GS conformance harness
(tools/gs_conformance.py; docs/GS_CONFORMANCE.md).

Each batch is one DMA kick and one snapshot.  Every test owns its buffers,
resets the whole GS state and clears first (gs_conformance.Test).  The
primitive descriptions stored in each test's `meta` are what the analysis
(tools/gs_conformance_analyse.py) models; the register writes stored in
batch.json are what the GS received.

Coverage tests draw every primitive with a one-bit colour id (R bits 0..7,
G 8..15, B 16..23) through the additive blend ALPHA A=Cs B=0 C=FIX(0x80)
D=Cd, so every pixel records exactly which primitives covered it (and how
often, for the count tests, which add 1 per primitive).

All inputs are deterministic (fixed seeds).  Nothing here is disc-derived.
"""
from __future__ import annotations

import json
import struct
from pathlib import Path

import numpy as np

from gs_conformance import (ALPHA, CLAMP, DIMX, FOGCOL, FRAME, PRIM, RGBAQ, SCISSOR, ST, TEST, TEX0,
                            TEX1, TEXA, UV, XYOFFSET, XYZ2, XYZF2, ZBUF, Batch, Test, vram_of)

ADD = ALPHA(0, 2, 2, 1, 0x80)          # Cs * 0x80 >> 7 + Cd = Cs + Cd
Q16 = 1 / 16


def q(v: float) -> float:
    return round(v * 16) / 16


def id_rgba(k: int) -> tuple[int, int, int, int]:
    bit = 1 << k
    return bit & 0xFF, bit >> 8 & 0xFF, bit >> 16 & 0xFF, 0x80


def vtx(t: Test, x, y, z=0, rgba=None, qv=1.0, st=None, uv=None, f=None, base=None):
    """A+D writes for one vertex; x, y in buffer pixels (the test's XYOFFSET
    is added, or `base` when given)."""
    bx, by = base if base is not None else t.ofs
    w = []
    if rgba is not None:
        w.append(("RGBAQ", RGBAQ(*rgba, q=qv)))
    if st is not None:
        w.append(("ST", ST(*st)))
    if uv is not None:
        w.append(("UV", UV(*uv)))
    if f is None:
        w.append(("XYZ2", XYZ2(x + bx, y + by, z)))
    else:
        w.append(("XYZF2", XYZF2(x + bx, y + by, z, f)))
    return w


def prim(t: Test, kind: str, verts: list, **pflags):
    """verts: list of dicts for vtx().  Records the primitive in meta."""
    w = [("PRIM", PRIM(kind, **pflags))]
    for v in verts:
        w += vtx(t, **v)
    t.ad(*w)
    t.meta.setdefault("prims", []).append(
        {"kind": kind, "flags": pflags,
         "v": [{k: (list(val) if isinstance(val, tuple) else val) for k, val in v.items()} for v in verts]})


def cov_setup(t: Test):
    t.ad(("ALPHA_1", ADD), ("TEST_1", TEST()), ("COLCLAMP", 1))
    t.meta["mode"] = "coverage_ids"


def count_setup(t: Test):
    t.ad(("ALPHA_1", ADD), ("TEST_1", TEST()), ("COLCLAMP", 1))
    t.meta["mode"] = "coverage_count"


def rnd_xy(rng, lo, hi):
    return q(rng.uniform(lo, hi)), q(rng.uniform(lo, hi))


# ---------------------------------------------------------------------------
# layout: known uploads -> the measured swizzle maps
def batch_layout() -> Batch:
    b = Batch("layout", "known uploads: PSMCT32 / PSMCT16 / PSMZ32 page maps and page order")
    t = b.add(Test("ct32_page", "64x32 PSMCT32 upload, pixel (x,y) = y*64+x", 64, 32, noclear=True))
    t.image(t.fbp * 32, 64, "CT32", 64, 32, np.arange(2048, dtype="<u4").tobytes(), label="ct32 index")
    t = b.add(Test("ct32_2x2", "128x64 PSMCT32 upload (4 pages), pixel = 0xC0000000|y*128+x", 128, 64, noclear=True))
    t.image(t.fbp * 32, 128, "CT32", 128, 64, (np.arange(8192, dtype="<u4") | 0xC0000000).tobytes(),
            label="ct32 page order")
    t = b.add(Test("ct16_page", "64x64 PSMCT16 upload, pixel = y*64+x", 64, 64, psm="CT16", noclear=True))
    t.image(t.fbp * 32, 64, "CT16", 64, 64, np.arange(4096, dtype="<u2").tobytes(), label="ct16 index")
    t = b.add(Test("z32_page", "64x32 PSMZ32 upload, pixel = 0x5A000000|y*64+x", 64, 32, psm="Z32", noclear=True))
    t.image(t.fbp * 32, 64, "Z32", 64, 32, (np.arange(2048, dtype="<u4") | 0x5A000000).tobytes(), label="z32 index")
    t = b.add(Test("z16_page", "64x64 PSMZ16 upload, pixel = y*64+x", 64, 64, psm="Z16", noclear=True))
    t.image(t.fbp * 32, 64, "Z16", 64, 64, np.arange(4096, dtype="<u2").tobytes(), label="z16 index")
    # a drawn check through the maps: sprites at known rectangles in CT32 / CT16 / Z24
    t = b.add(Test("draw_check", "sprites at known rectangles; Z24 of each = its id", 128, 64, zpsm="Z24",
                   clear=0x80000000))
    for k, (x0, y0, x1, y1) in enumerate([(0, 0, 8, 8), (70, 3, 90, 40), (5, 33, 64, 64), (100, 50, 128, 64)]):
        prim(t, "sprite", [dict(x=x0, y=y0, z=k + 1, rgba=(k + 1, 2 * k + 1, 3 * k + 1, 0x40 + k)),
                           dict(x=x1, y=y1, z=k + 1, rgba=(k + 1, 2 * k + 1, 3 * k + 1, 0x40 + k))])
    t = b.add(Test("draw_check16", "sprites at known rectangles in a PSMCT16 frame", 64, 64, psm="CT16"))
    for k, (x0, y0, x1, y1) in enumerate([(0, 0, 8, 8), (20, 3, 40, 40), (5, 44, 64, 64)]):
        prim(t, "sprite", [dict(x=x0, y=y0, rgba=(8 * (k + 1), 16 * (k + 1), 24 * (k + 1), 0x80)),
                           dict(x=x1, y=y1, rgba=(8 * (k + 1), 16 * (k + 1), 24 * (k + 1), 0x80))])
    return b


def derive_maps(folder: Path) -> dict:
    """Measure the per-page maps from the layout batch's uploads."""
    import sys
    doc = json.loads((folder / "batch.json").read_text())
    vram = vram_of((folder / "snap/gs.bin").read_bytes())
    tests = {t["name"]: t for t in doc["tests"]}
    report = {}

    def page_map(name, count, dtype, mask):
        page = tests[name]["fbp"]
        raw = np.frombuffer(vram, dtype=dtype)[page * 8192 // np.dtype(dtype).itemsize:][:count] & mask
        ok = sorted(raw.tolist()) == list(range(count))
        inv = np.empty(count, dtype=np.int64)
        inv[raw.astype(np.int64)] = np.arange(count)
        return ok, inv

    ok32, inv32 = page_map("ct32_page", 2048, "<u4", 0xFFFFFFFF)
    ok16, inv16 = page_map("ct16_page", 4096, "<u2", 0xFFFF)
    okz, invz = page_map("z32_page", 2048, "<u4", 0x00FFFFFF)
    okz16, invz16 = page_map("z16_page", 4096, "<u2", 0xFFFF)
    maps = {"ct32": inv32.reshape(32, 64), "ct16": inv16.reshape(64, 64), "z32": invz.reshape(32, 64),
            "z16": invz16.reshape(64, 64)}
    report["permutations"] = {"ct32": ok32, "ct16": ok16, "z32": okz, "z16": okz16}
    sys.path.insert(0, str(Path(__file__).parent))
    from c7cap_partb import gs_word_map
    report["ct32_equals_documented_table"] = bool((gs_word_map(64, 32, 64) == maps["ct32"]).all())
    report["z32_equals_documented_table"] = bool((gs_word_map(64, 32, 64, z=True) == maps["z32"]).all())
    np.savez(folder / "maps.npz", **maps)
    # page order: the 128x64 upload through the maps + row-major pages
    from gs_conformance import decode_buffer, load_maps
    m = load_maps()
    t = tests["ct32_2x2"]
    got = decode_buffer(vram, t["fbp"], 128, 64, "CT32", m)
    report["ct32_page_order_row_major"] = bool((got == (np.arange(8192, dtype=np.uint32) | 0xC0000000)
                                                 .reshape(64, 128)).all())
    # drawn sprites through the maps
    t = tests["draw_check"]
    col = decode_buffer(vram, t["fbp"], 128, 64, "CT32", m)
    z = decode_buffer(vram, t["zbp"], 128, 64, "Z24", m)
    expect = np.full((64, 128), 0x80000000, dtype=np.uint32)
    ez = np.full((64, 128), 0x5A000000, dtype=np.uint32)
    for k, (x0, y0, x1, y1) in enumerate([(0, 0, 8, 8), (70, 3, 90, 40), (5, 33, 64, 64), (100, 50, 128, 64)]):
        expect[y0:y1, x0:x1] = (k + 1) | (2 * k + 1) << 8 | (3 * k + 1) << 16 | (0x40 + k) << 24
        ez[y0:y1, x0:x1] = 0x5A000000 | (k + 1)
    report["draw_check_ct32_exact"] = bool((col == expect).all())
    report["draw_check_z24_exact"] = bool((z == ez).all())
    report["z24_top_byte_kept"] = bool(((z >> 24) == 0x5A).all())
    t = tests["draw_check16"]
    c16 = decode_buffer(vram, t["fbp"], 64, 64, "CT16", m)
    e16 = np.zeros((64, 64), dtype=np.uint16)
    for k, (x0, y0, x1, y1) in enumerate([(0, 0, 8, 8), (20, 3, 40, 40), (5, 44, 64, 64)]):
        r, g, bb = 8 * (k + 1), 16 * (k + 1), 24 * (k + 1)
        e16[y0:y1, x0:x1] = (r >> 3) | (g >> 3) << 5 | (bb >> 3) << 10 | 1 << 15
    report["draw_check_ct16_exact"] = bool((c16 == e16).all())
    report["draw_check_ct16_mismatch"] = int((c16 != e16).sum())
    (folder / "layout_report.json").write_text(json.dumps(report, indent=1) + "\n")
    print("layout:", json.dumps(report))
    return report


# ---------------------------------------------------------------------------
# raster: fill conventions
def batch_raster() -> Batch:
    b = Batch("raster", "triangle / sprite / line / point coverage conventions")
    rng = np.random.RandomState(1601)

    t = b.add(Test("tri_int", "integer-vertex triangles: squares split both ways, a fan, shared edges"))
    cov_setup(t)
    tris = [[(4, 4), (20, 4), (4, 20)], [(20, 4), (20, 20), (4, 20)],        # square, "\" diagonal
            [(24, 4), (40, 4), (40, 20)], [(24, 4), (40, 20), (24, 20)],     # square, "/" diagonal
            [(4, 24), (12, 24), (8, 32)], [(12, 24), (20, 24), (16, 32)],    # touching at a vertex
            [(8, 32), (16, 32), (12, 24)]]                                   # fills the gap
    c, r = (52, 14), 9
    ring = [(52 + 9, 14), (52 + 6, 14 + 6), (52, 14 + 9), (52 - 6, 14 + 6), (52 - 9, 14), (52 - 6, 14 - 6),
            (52, 14 - 9), (52 + 6, 14 - 6)]
    tris += [[c, ring[i], ring[(i + 1) % 8]] for i in range(8)]              # fan of 8
    tris += [[(4, 40), (30, 41), (10, 60)], [(30, 41), (10, 60), (40, 62)],  # shared sloped edge
             [(44, 30), (60, 30), (60, 46)], [(44, 30), (60, 46), (44, 46)],
             [(46, 50), (47, 50), (46, 51)], [(50, 50), (52, 51), (51, 53)], [(56, 50), (56, 50), (60, 56)]]
    for k, tri in enumerate(tris[:24]):
        prim(t, "tri", [dict(x=x, y=y, rgba=id_rgba(k)) for x, y in tri], abe=1)

    for n, (lo, hi, span) in enumerate([(1, 63, 60), (1, 63, 60), (2, 62, 14), (2, 62, 14), (2, 62, 4),
                                         (2, 62, 4), (2, 62, 2), (2, 62, 2)]):
        t = b.add(Test(f"tri_frac_{n}", f"24 random triangles, 1/16 vertices, extent <= {span} px"))
        cov_setup(t)
        for k in range(24):
            cx, cy = rng.uniform(lo + span / 2, hi - span / 2, 2) if span < 60 else (32, 32)
            pts = [(q(np.clip(cx + rng.uniform(-span / 2, span / 2), 0.5, 63.5)),
                    q(np.clip(cy + rng.uniform(-span / 2, span / 2), 0.5, 63.5))) for _ in range(3)]
            prim(t, "tri", [dict(x=x, y=y, rgba=id_rgba(k)) for x, y in pts], abe=1)

    t = b.add(Test("tri_halves", "triangles with vertices on half and quarter pixels (edges through centres)"))
    cov_setup(t)
    for k in range(24):
        cx, cy = 6 + (k % 6) * 10, 6 + (k // 6) * 14
        f = [0.0, 0.5, 0.25, 0.75][k % 4]
        pts = [(cx - 4 + f, cy - 4 + f), (cx + 4 + f, cy - 4 + (k % 3) * 0.5), (cx - 2 + f, cy + 5 + f)]
        if k % 2:
            pts = pts[::-1]
        prim(t, "tri", [dict(x=x, y=y, rgba=id_rgba(k)) for x, y in pts], abe=1)

    t = b.add(Test("tri_degenerate", "zero-area and near-zero triangles"))
    cov_setup(t)
    degen = [[(4, 4), (20, 4), (12, 4)], [(4, 8), (4, 20), (4, 14)], [(8, 8), (20, 20), (14, 14)],
             [(24, 4), (24, 4), (30, 10)], [(34, 4), (34, 4), (34, 4)], [(40.5, 4.5), (50.5, 4.5), (45.5, 4.5)],
             [(4, 30), (30, 30.0625), (4, 30.0625)], [(4, 34.5), (30, 34.5625), (4, 34.5625)],
             [(40, 20), (40.0625, 40), (40, 40)], [(44.5, 20), (44.5625, 40), (44.5, 40)],
             [(10.25, 44.25), (10.375, 44.25), (10.25, 44.375)], [(20.5, 44.5), (20.5625, 44.5), (20.5, 44.5625)],
             [(30.4375, 44.4375), (30.5625, 44.4375), (30.5, 44.5625)], [(50, 50), (60, 60), (55, 55.0625)]]
    for k, tri in enumerate(degen):
        prim(t, "tri", [dict(x=x, y=y, rgba=id_rgba(k)) for x, y in tri], abe=1)

    for n in range(2):
        t = b.add(Test(f"tri_sliver_{n}", "long thin triangles at random angles (width 1/16 .. 1 px)"))
        cov_setup(t)
        for k in range(24):
            a = rng.uniform(0, np.pi)
            L = rng.uniform(20, 56)
            wdt = [Q16, 0.125, 0.25, 0.5, 0.75, 1.0][k % 6]
            cx, cy = rng.uniform(4 + L / 2 * abs(np.cos(a)), 60 - L / 2 * abs(np.cos(a))), \
                rng.uniform(4 + L / 2 * abs(np.sin(a)), 60 - L / 2 * abs(np.sin(a)))
            dx, dy = np.cos(a) * L / 2, np.sin(a) * L / 2
            nx, ny = -np.sin(a) * wdt, np.cos(a) * wdt
            pts = [(q(cx - dx), q(cy - dy)), (q(cx + dx), q(cy + dy)), (q(cx - dx + nx), q(cy - dy + ny))]
            pts = [(min(max(x, 0), 63.9375), min(max(y, 0), 63.9375)) for x, y in pts]
            prim(t, "tri", [dict(x=x, y=y, rgba=id_rgba(k)) for x, y in pts], abe=1)

    t = b.add(Test("tri_orient", "4 triangles, each in all 6 vertex orders (ids 6k..6k+5)"))
    cov_setup(t)
    import itertools
    base = [[(3.3125, 2.75), (28.5, 9.0625), (10.0625, 29.4375)], [(35.5, 3.125), (61.0625, 5.5), (40.25, 28.75)],
            [(4.0, 36.0), (29.0, 36.0), (16.5, 61.0)], [(34.25, 60.5), (60.9375, 35.8125), (47.5, 47.5)]]
    k = 0
    for tri in base:
        for order in itertools.permutations(range(3)):
            prim(t, "tri", [dict(x=tri[i][0], y=tri[i][1], rgba=id_rgba(k)) for i in order], abe=1)
            k += 1

    for n in range(2):
        t = b.add(Test(f"tri_mesh_{n}", "jittered 7x7-vertex grid, 72 triangles: coverage count per pixel"))
        count_setup(t)
        g = np.zeros((7, 7, 2))
        for i in range(7):
            for j in range(7):
                g[i, j] = (2 + j * 10 + (rng.uniform(-3.5, 3.5) if 0 < j < 6 else 0),
                           2 + i * 10 + (rng.uniform(-3.5, 3.5) if 0 < i < 6 else 0))
        g = np.vectorize(q)(g)
        for i in range(6):
            for j in range(6):
                a, bq, c, d = g[i, j], g[i, j + 1], g[i + 1, j], g[i + 1, j + 1]
                for tri in ([a, bq, c], [bq, d, c]) if (i + j + n) % 2 else ([a, bq, d], [a, d, c]):
                    prim(t, "tri", [dict(x=float(x), y=float(y), rgba=(1, 0, 0, 0x80)) for x, y in tri], abe=1)
        t.meta["grid"] = g.tolist()

    for n in range(3):
        t = b.add(Test(f"sprite_frac_{n}", "24 random sprites, 1/16 corners (odd ids: corners given reversed)"))
        cov_setup(t)
        for k in range(24):
            x0, y0 = rnd_xy(rng, 1, 56)
            wdt, hgt = q(rng.uniform(0, [20, 6, 2][n])), q(rng.uniform(0, [20, 6, 2][n]))
            x1, y1 = min(x0 + wdt, 63.9375), min(y0 + hgt, 63.9375)
            vs = [(x0, y0), (x1, y1)] if k % 2 == 0 else [(x1, y1), (x0, y0)]
            prim(t, "sprite", [dict(x=x, y=y, rgba=id_rgba(k)) for x, y in vs], abe=1)

    t = b.add(Test("sprite_special", "integer / half / zero-size / mixed-order sprites"))
    cov_setup(t)
    sp = [((2, 2), (6, 6)), ((8.5, 2.5), (12.5, 6.5)), ((14.25, 2.25), (18.75, 6.75)), ((20, 2), (20, 8)),
          ((22, 2), (28, 2)), ((30, 2), (30, 2)), ((32.5, 2.5), (33, 3)), ((34.4375, 2.4375), (34.5625, 2.5625)),
          ((40, 8), (36, 2)), ((42, 8), (46, 2)), ((52, 2), (48, 8)), ((2, 12), (62, 13)),
          ((2, 15.5), (62, 16.5)), ((2, 18.9375), (62, 19.0625)), ((2.9375, 22), (3.0625, 60)),
          ((6.5, 22), (7.5, 60)), ((10, 22), (60, 60))]
    for k, (a, c) in enumerate(sp):
        prim(t, "sprite", [dict(x=a[0], y=a[1], rgba=id_rgba(k)), dict(x=c[0], y=c[1], rgba=id_rgba(k))], abe=1)

    for n in range(3):
        t = b.add(Test(f"line_frac_{n}", "24 random lines, 1/16 endpoints"))
        cov_setup(t)
        for k in range(24):
            p0 = rnd_xy(rng, 1, 63)
            L = [60, 12, 3][n]
            p1 = (q(np.clip(p0[0] + rng.uniform(-L, L), 0.5, 63.5)), q(np.clip(p0[1] + rng.uniform(-L, L), 0.5, 63.5)))
            prim(t, "line", [dict(x=p0[0], y=p0[1], rgba=id_rgba(k)), dict(x=p1[0], y=p1[1], rgba=id_rgba(k))], abe=1)

    t = b.add(Test("line_axis", "horizontal / vertical / 45-degree / zero-length lines, integer and half"))
    cov_setup(t)
    ln = [((2, 2), (30, 2)), ((30, 4), (2, 4)), ((2, 6.5), (30, 6.5)), ((2.5, 8), (30.5, 8)),
          ((34, 2), (34, 30)), ((36, 30), (36, 2)), ((38.5, 2), (38.5, 30)), ((40, 2.5), (40, 30.5)),
          ((44, 2), (60, 18)), ((60, 20), (44, 36)), ((44.5, 22.5), (58.5, 36.5)), ((2, 12), (28, 25)),
          ((2, 30), (28, 17)), ((10, 40), (10, 40)), ((12.5, 40.5), (12.5, 40.5)), ((2, 50), (62, 51)),
          ((2, 58), (62, 63)), ((50, 40), (51, 62)), ((20, 44), (40, 44.0625)), ((20, 46.9375), (40, 47))]
    for k, (a, c) in enumerate(ln):
        prim(t, "line", [dict(x=a[0], y=a[1], rgba=id_rgba(k)), dict(x=c[0], y=c[1], rgba=id_rgba(k))], abe=1)

    t = b.add(Test("point", "24 points at 1/16 positions"))
    cov_setup(t)
    for k in range(24):
        x, y = (4 + (k % 6) * 10 + (k % 16) / 16, 4 + (k // 6) * 14 + ((k * 7) % 16) / 16)
        prim(t, "point", [dict(x=x, y=y, rgba=id_rgba(k))], abe=1)

    t = b.add(Test("strip_fan", "tristrip (ids by last vertex colour) and trifan: coverage count"))
    count_setup(t)
    strip = [(4, 4), (8, 30), (20, 3.5), (22.5, 29), (36, 6.25), (40, 31), (58.5, 2), (60, 28)]
    prim(t, "tristrip", [dict(x=x, y=y, rgba=(1, 0, 0, 0x80)) for x, y in strip], abe=1)
    fan = [(32, 48), (4, 36), (20, 34), (40, 35.5), (60, 38), (61, 62), (30, 63), (3, 61), (4, 36)]
    prim(t, "trifan", [dict(x=x, y=y, rgba=(1, 0, 0, 0x80)) for x, y in fan], abe=1)
    return b


# ---------------------------------------------------------------------------
# shade: Gouraud colour, flat vertex selection, Z interpolation
def batch_shade() -> Batch:
    b = Batch("shade", "Gouraud colour and alpha, flat colour selection, Z interpolation and Z formats")
    rng = np.random.RandomState(1602)
    t = b.add(Test("gouraud_big", "one Gouraud triangle, RGBA extremes"))
    prim(t, "tri", [dict(x=1, y=1, rgba=(255, 0, 0, 0)), dict(x=63, y=10, rgba=(0, 255, 0, 128)),
                    dict(x=12, y=63, rgba=(0, 0, 255, 255))], iip=1)
    t = b.add(Test("gouraud_small", "four Gouraud triangles with colour deltas of 1..4 over ~30 px"))
    for k, (x0, y0) in enumerate([(1, 1), (33, 1), (1, 33), (33, 33)]):
        c = 10 + 60 * k
        prim(t, "tri", [dict(x=x0, y=y0, rgba=(c, c + 1, c + 2, 0x10 * k)),
                        dict(x=x0 + 30, y=y0 + 3, rgba=(c + k + 1, c, c + 4, 0x10 * k + 3)),
                        dict(x=x0 + 5, y=y0 + 30, rgba=(c + 2, c + 3, c, 0x10 * k + 1))], iip=1)
    for n in range(8):
        t = b.add(Test(f"gouraud_rand_{n}", "one random Gouraud triangle, 1/16 vertices, random RGBA"))
        pts = [rnd_xy(rng, 0.5, 63.5) for _ in range(3)]
        prim(t, "tri", [dict(x=x, y=y, rgba=tuple(int(c) for c in rng.randint(0, 256, 4))) for x, y in pts], iip=1)
    t = b.add(Test("gouraud_strip", "Gouraud tristrip + trifan (shared vertices)"))
    strip = [(2, 2), (6, 30), (22, 3.5), (21.5, 29.25), (40, 2.75), (42, 31), (62, 3), (61, 30)]
    prim(t, "tristrip", [dict(x=x, y=y, rgba=tuple(int(c) for c in rng.randint(0, 256, 4))) for x, y in strip], iip=1)
    fan = [(32, 48), (4, 36), (20, 34), (40, 35.5), (60, 38), (61, 62), (30, 63), (3, 61)]
    prim(t, "trifan", [dict(x=x, y=y, rgba=tuple(int(c) for c in rng.randint(0, 256, 4))) for x, y in fan], iip=1)
    t = b.add(Test("flat_select", "IIP 0: which vertex colour a tri, strip, fan, line, sprite and point take"))
    cols = [(200, 10, 10, 0x11), (10, 200, 10, 0x22), (10, 10, 200, 0x33), (200, 200, 10, 0x44),
            (10, 200, 200, 0x55)]
    prim(t, "tri", [dict(x=2, y=2, rgba=cols[0]), dict(x=20, y=2, rgba=cols[1]), dict(x=2, y=20, rgba=cols[2])])
    prim(t, "tristrip", [dict(x=24, y=2, rgba=cols[0]), dict(x=24, y=20, rgba=cols[1]),
                         dict(x=42, y=2, rgba=cols[2]), dict(x=42, y=20, rgba=cols[3])])
    prim(t, "trifan", [dict(x=53, y=12, rgba=cols[0]), dict(x=45, y=2, rgba=cols[1]),
                       dict(x=62, y=2, rgba=cols[2]), dict(x=62, y=22, rgba=cols[3])])
    prim(t, "line", [dict(x=2, y=26, rgba=cols[0]), dict(x=60, y=26, rgba=cols[1])])
    prim(t, "linestrip", [dict(x=2, y=30, rgba=cols[0]), dict(x=30, y=30, rgba=cols[1]),
                          dict(x=30, y=40, rgba=cols[2])])
    prim(t, "sprite", [dict(x=36, y=30, rgba=cols[0]), dict(x=48, y=42, rgba=cols[1])])
    prim(t, "sprite", [dict(x=50, y=30, rgba=cols[2]), dict(x=62, y=42, rgba=cols[3])], iip=1)
    prim(t, "point", [dict(x=5, y=50, rgba=cols[4])])
    t = b.add(Test("gouraud_line", "Gouraud lines and a line strip"))
    for k in range(8):
        a = (2 + k * 7, 2)
        c = (60 - k * 5, 30 + k * 4)
        prim(t, "line", [dict(x=a[0], y=a[1], rgba=(255, 0, 20 * k, 0)), dict(x=c[0], y=c[1], rgba=(0, 255, 255 - 20 * k, 255))],
             iip=1)
    prim(t, "linestrip", [dict(x=2, y=62, rgba=(0, 0, 0, 0)), dict(x=30, y=40, rgba=(255, 128, 0, 255)),
                          dict(x=62, y=62, rgba=(0, 255, 64, 32))], iip=1)
    # Z
    for zpsm, zs in (("Z24", (0x000000, 0xFFFFFF, 0x7FFFFF)), ("Z32", (0x00000000, 0xFFFFFFFF, 0x80000000))):
        t = b.add(Test(f"z_interp_{zpsm.lower()}", f"Z interpolation over one triangle ({zpsm})", zpsm=zpsm))
        prim(t, "tri", [dict(x=1, y=1, z=zs[0], rgba=(255, 255, 255, 0x80)),
                        dict(x=63, y=8, z=zs[1], rgba=(255, 255, 255, 0x80)),
                        dict(x=9, y=63, z=zs[2], rgba=(255, 255, 255, 0x80))], iip=1)
    t = b.add(Test("z_small", "Z over a small range (1000..1003) and sprite Z (v1 vs v2)", zpsm="Z24"))
    prim(t, "tri", [dict(x=1, y=1, z=1000, rgba=(9, 9, 9, 0x80)), dict(x=63, y=4, z=1003, rgba=(9, 9, 9, 0x80)),
                    dict(x=4, y=40, z=1001, rgba=(9, 9, 9, 0x80))])
    prim(t, "sprite", [dict(x=2, y=44, z=0x111111, rgba=(1, 2, 3, 0x80)), dict(x=30, y=62, z=0x222222, rgba=(4, 5, 6, 0x80))])
    prim(t, "tri", [dict(x=34, y=44, z=0x10, rgba=(9, 9, 9, 0x80)), dict(x=62, y=44, z=0x20, rgba=(9, 9, 9, 0x80)),
                    dict(x=48, y=62, z=0x30, rgba=(9, 9, 9, 0x80))])
    t = b.add(Test("z_range", "Z values beyond the format: sprites Z 0x01234567 / 0xFFFFFFFF into Z24 and Z16",
                   zpsm="Z24"))
    for k, z in enumerate([0x00FFFFFF, 0x01000000, 0x01234567, 0xFFFFFFFF, 0x00ABCDEF]):
        prim(t, "sprite", [dict(x=2 + 12 * k, y=2, z=z, rgba=(20 * k, 0, 0, 0x80)),
                           dict(x=12 + 12 * k, y=30, z=z, rgba=(20 * k, 0, 0, 0x80))])
    t = b.add(Test("z_range16", "Z beyond 16 bits into Z16", zpsm="Z16"))
    for k, z in enumerate([0x0000FFFF, 0x00010000, 0x01234567, 0xFFFFFFFF, 0x0000ABCD, 0x00001234]):
        prim(t, "sprite", [dict(x=2 + 10 * k, y=2, z=z, rgba=(20 * k, 0, 0, 0x80)),
                           dict(x=10 + 10 * k, y=30, z=z, rgba=(20 * k, 0, 0, 0x80))])
    prim(t, "tri", [dict(x=1, y=33, z=0x0000, rgba=(5, 5, 5, 0x80)), dict(x=63, y=36, z=0xFFFF, rgba=(5, 5, 5, 0x80)),
                    dict(x=5, y=63, z=0x8000, rgba=(5, 5, 5, 0x80))])
    return b


# ---------------------------------------------------------------------------
# textures
def rand_ct32(rng, w, h, amax=256):
    a = rng.randint(0, 256, (h, w, 4)).astype(np.uint32)
    a[..., 3] = rng.randint(0, amax, (h, w))
    return (a[..., 0] | a[..., 1] << 8 | a[..., 2] << 16 | a[..., 3] << 24).astype("<u4")


def t4_bytes(idx):
    flat = idx.reshape(-1).astype(np.uint8)
    return (flat[0::2] | flat[1::2] << 4).astype(np.uint8).tobytes()


class Textures:
    """Uploads (as items of an 'upload' test) and TEX0 values for a batch."""

    def __init__(self, b: Batch, seed: int):
        rng = np.random.RandomState(seed)
        up = b.add(Test("upload", "texture and CLUT uploads for this batch (draws nothing)", 64, 32))
        self.data = {}
        self.A = rand_ct32(rng, 16, 16)                          # CT32 16x16
        self.B = rand_ct32(rng, 64, 64)                          # CT32 64x64
        self.C_idx = rng.randint(0, 256, (16, 16)).astype(np.uint8)
        self.C_clut = rand_ct32(rng, 16, 16)                     # 256-entry CLUT image (16x16)
        self.D_idx = rng.randint(0, 16, (16, 16)).astype(np.uint8)
        self.D_clut = rand_ct32(rng, 8, 2)                       # 16-entry CLUT image (8x2)
        self.E_idx = np.arange(256, dtype=np.uint8).reshape(16, 16)
        yy, xx = np.mgrid[0:16, 0:16]
        self.E_clut = (xx | yy << 8 | 0x55 << 16 | 0x80 << 24).astype("<u4")
        self.F_idx = (np.arange(256) % 16).astype(np.uint8).reshape(16, 16)
        yy, xx = np.mgrid[0:2, 0:8]
        self.F_clut = (xx | yy << 8 | 0x66 << 16 | 0x80 << 24).astype("<u4")
        self.blk = {k: b.tex_alloc(p) for k, p in [("A", 1), ("B", 2), ("C", 1), ("Cc", 1), ("D", 1), ("Dc", 1),
                                                   ("E", 1), ("Ec", 1), ("F", 1), ("Fc", 1)]}
        for key, psm, w, h, arr in [("A", "CT32", 16, 16, self.A), ("B", "CT32", 64, 64, self.B),
                                    ("Cc", "CT32", 16, 16, self.C_clut), ("Dc", "CT32", 8, 2, self.D_clut),
                                    ("Ec", "CT32", 16, 16, self.E_clut), ("Fc", "CT32", 8, 2, self.F_clut)]:
            up.image(self.blk[key], 64, psm, w, h, arr.tobytes(), label=key)
        for key, arr in [("C", self.C_idx), ("E", self.E_idx)]:
            up.image(self.blk[key], 128, "T8", 16, 16, arr.tobytes(), label=key)
        for key, arr in [("D", self.D_idx), ("F", self.F_idx)]:
            up.image(self.blk[key], 128, "T4", 16, 16, t4_bytes(arr), label=key)
        up.meta["textures"] = {"blocks": self.blk}
        self.arrays = {"A": self.A, "B": self.B, "C_idx": self.C_idx, "C_clut": self.C_clut, "D_idx": self.D_idx,
                       "D_clut": self.D_clut, "E_idx": self.E_idx, "E_clut": self.E_clut, "F_idx": self.F_idx,
                       "F_clut": self.F_clut}

    def tex0(self, key, tcc=1, tfx=1, cld=1, csa=0):
        if key == "A":
            return TEX0(self.blk["A"], 64, "CT32", 4, 4, tcc, tfx)
        if key == "B":
            return TEX0(self.blk["B"], 64, "CT32", 6, 6, tcc, tfx)
        if key in ("C", "E"):
            return TEX0(self.blk[key], 128, "T8", 4, 4, tcc, tfx, cbp=self.blk[key + "c"], cpsm="CT32", cld=cld)
        if key in ("D", "F"):
            return TEX0(self.blk[key], 128, "T4", 4, 4, tcc, tfx, cbp=self.blk[key + "c"], cpsm="CT32", cld=cld,
                        csa=csa)
        raise KeyError(key)


def tex_state(t: Test, tex0, mmag=0, mmin=0, clamp=0):
    t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=mmag, mmin=mmin)), ("CLAMP_1", clamp))
    t.meta["tex"] = {"tex0": hex(tex0), "mmag": mmag, "mmin": mmin, "clamp": hex(clamp)}


def tex_sprite(t: Test, x0, y0, x1, y1, u0, v0, u1, v1, mode="uv", tw=16, th=16, rgba=(0x80, 0x80, 0x80, 0x80),
               **flags):
    if mode == "uv":
        verts = [dict(x=x0, y=y0, rgba=rgba, uv=(u0, v0)), dict(x=x1, y=y1, rgba=rgba, uv=(u1, v1))]
        prim(t, "sprite", verts, tme=1, fst=1, **flags)
    else:
        verts = [dict(x=x0, y=y0, rgba=rgba, st=(u0 / tw, v0 / th)), dict(x=x1, y=y1, rgba=rgba, st=(u1 / tw, v1 / th))]
        prim(t, "sprite", verts, tme=1, **flags)


def batch_texture() -> Batch:
    b = Batch("texture", "UV/ST texturing: nearest, bilinear, wrap modes, TFX, CLUT T8/T4, perspective")
    tx = Textures(b, 1603)
    b.textures = tx
    for mode in ("uv", "st"):
        t = b.add(Test(f"copy_b_{mode}", f"64x64 CT32 texture 1:1 onto 64x64 ({mode}, nearest, DECAL)"))
        tex_state(t, tx.tex0("B"))
        tex_sprite(t, 0, 0, 64, 64, 0, 0, 64, 64, mode, 64, 64)
    for filt in (0, 1):
        for mode in ("uv", "st"):
            t = b.add(Test(f"mag_a_{'bil' if filt else 'near'}_{mode}", f"16x16 -> 64x64 magnification ({mode})"))
            tex_state(t, tx.tex0("A"), mmag=filt, mmin=filt)
            tex_sprite(t, 0, 0, 64, 64, 0, 0, 16, 16, mode)
        t = b.add(Test(f"mag_a_{'bil' if filt else 'near'}_frac", "fractional UV / positions, several sprites"))
        tex_state(t, tx.tex0("A"), mmag=filt, mmin=filt)
        tex_sprite(t, 0, 0, 32, 32, 0.25, 0.25, 8.25, 8.25)
        tex_sprite(t, 32, 0, 64, 32, 0.5, 0.5, 8.5, 8.5)
        tex_sprite(t, 0.5, 32.5, 32.5, 64, 0.75, 0.0625, 11.75, 12.0625)
        tex_sprite(t, 33.25, 32.75, 63.5, 63.25, 3.3125, 5.1875, 16, 16)
        t = b.add(Test(f"min_b_{'bil' if filt else 'near'}", "64x64 -> 32x32 and -> 21x21 minification"))
        tex_state(t, tx.tex0("B"), mmag=filt, mmin=filt)
        tex_sprite(t, 0, 0, 32, 32, 0, 0, 64, 64, tw=64, th=64)
        tex_sprite(t, 32, 0, 53, 21, 0, 0, 64, 64, tw=64, th=64)
        tex_sprite(t, 0, 32, 32, 64, 0.5, 0.5, 64.5, 64.5, tw=64, th=64)
    for wname, wm, extra in [("repeat", 0, {}), ("clamp", 1, {}), ("region_clamp", 2, dict(minu=3, maxu=11, minv=2, maxv=9)),
                             ("region_repeat", 3, dict(minu=7, maxu=4, minv=3, maxv=8))]:
        for filt in (0, 1):
            t = b.add(Test(f"wrap_{wname}_{'bil' if filt else 'near'}", f"S,T from -1 to 2 (ST, Q=1): {wname}"))
            tex_state(t, tx.tex0("A"), mmag=filt, mmin=filt, clamp=CLAMP(wm, wm, **extra))
            prim(t, "sprite", [dict(x=0, y=0, rgba=(0x80,) * 4, st=(-1.0, -1.0)),
                               dict(x=64, y=64, rgba=(0x80,) * 4, st=(2.0, 2.0))], tme=1)
    cols = [(0x80, 0x80, 0x80, 0x80), (0x40, 0xC0, 0xFF, 0x20), (0xFF, 0x10, 0x60, 0xFF), (0x00, 0x33, 0x99, 0x00)]
    for tfx in range(4):
        for tcc in (0, 1):
            t = b.add(Test(f"tfx{tfx}_tcc{tcc}", "four vertex colours (bands) x texture A, nearest"))
            tex_state(t, tx.tex0("A", tcc=tcc, tfx=tfx))
            for k, c in enumerate(cols):
                tex_sprite(t, 0, 16 * k, 64, 16 * k + 16, 0, 0, 16, 4, rgba=c)
    for key in ("C", "D"):
        for filt in (0, 1):
            t = b.add(Test(f"clut_{key}_{'bil' if filt else 'near'}", f"{'T8' if key == 'C' else 'T4'} + CT32 CLUT, 16x16 -> 64x64"))
            tex_state(t, tx.tex0(key, tfx=1), mmag=filt, mmin=filt)
            tex_sprite(t, 0, 0, 64, 64, 0, 0, 16, 16)
    for key in ("E", "F"):
        t = b.add(Test(f"clut_map_{key}", "identity indices 1:1: which CLUT image position each index reads"))
        tex_state(t, tx.tex0(key, tfx=1))
        tex_sprite(t, 0, 0, 16, 16, 0, 0, 16, 16)
    t = b.add(Test("clut_csa", "T4 with CSA 1 (second 16-entry group of a 256-entry CLUT image)"))
    tex_state(t, TEX0(tx.blk["F"], 128, "T4", 4, 4, 1, 1, cbp=tx.blk["Ec"], cpsm="CT32", cld=1, csa=1))
    tex_sprite(t, 0, 0, 16, 16, 0, 0, 16, 16)
    # perspective: a quad as two triangles with per-vertex Q
    for n, (qs, filt) in enumerate([((1.0, 1.0, 1.0, 1.0), 1), ((1.0, 0.25, 1.0, 0.25), 1), ((0.5, 1.0, 2.0, 0.75), 1),
                                    ((1.0, 0.25, 1.0, 0.25), 0)]):
        t = b.add(Test(f"persp_{n}", f"ST/Q quad, Q per corner {qs}, {'bilinear' if filt else 'nearest'}"))
        tex_state(t, tx.tex0("A"), mmag=filt, mmin=filt)
        corners = [(2, 2, 0.0, 0.0), (62, 5, 1.0, 0.0), (1, 61, 0.0, 1.0), (60, 63, 1.0, 1.0)]
        vs = [dict(x=x, y=y, rgba=(0x80,) * 4, qv=qq, st=(s * qq, tt * qq)) for (x, y, s, tt), qq in zip(corners, qs)]
        prim(t, "tri", [vs[0], vs[1], vs[2]], tme=1)
        prim(t, "tri", [vs[1], vs[3], vs[2]], tme=1)
    t = b.add(Test("tri_uv", "textured triangles, UV (FST 1), bilinear, and Gouraud MODULATE"))
    tex_state(t, tx.tex0("A", tfx=0), mmag=1, mmin=1)
    prim(t, "tri", [dict(x=1, y=1, rgba=(0x80, 0x80, 0x80, 0x80), uv=(0.5, 0.5)),
                    dict(x=63, y=3, rgba=(0xFF, 0x40, 0x80, 0x80), uv=(15.5, 1.25)),
                    dict(x=5, y=62, rgba=(0x20, 0x80, 0xFF, 0x80), uv=(2.0625, 15.75))], iip=1, tme=1, fst=1)
    # the game's level class (LEVEL_MATERIALS.md): tristrip IIP TME FGE, TEST 0x5000D, TEX1 0x60, CLAMP 0,
    # T8 + CT32 CLUT, MODULATE, TCC 1, vertex alpha 0x80, fog, ZBUF Z24 GEQUAL
    t = b.add(Test("level_class", "the level class state over a fogged, perspective tristrip", zpsm="Z24",
                   zclear=0x000100))
    t.ad(("TEST_1", 0x5000D), ("FOGCOL", FOGCOL(0x30, 0x40, 0x58)))
    tex_state(t, tx.tex0("C", tfx=0), mmag=1, mmin=1, clamp=0)
    pts = [(2, 3, 0x2000, 1.0, 0.0, 0.0, 255), (4, 60, 0x2100, 0.5, 0.0, 2.0, 200), (30, 1, 0x1800, 0.8, 1.5, 0.0, 128),
           (33, 62, 0x1F00, 0.4, 1.5, 2.0, 60), (62, 4, 0x1000, 1.0, 3.0, 0.0, 30), (61, 63, 0x1E00, 0.3, 3.0, 2.0, 0)]
    prim(t, "tristrip", [dict(x=x, y=y, z=z, qv=qq, rgba=(0x60 + 20 * i, 0x80, 0x70, 0x80), st=(s * qq, tt * qq), f=f)
                         for i, (x, y, z, qq, s, tt, f) in enumerate(pts)], iip=1, tme=1, fge=1)
    return b


# ---------------------------------------------------------------------------
# pixel pipeline: fog, alpha test, blending, PABE/FBA, dither, colclamp, Z test, scissor, offsets
def batch_pixel() -> Batch:
    b = Batch("pixel", "fog, alpha test, blending (81 equations + game presets), PABE, FBA, DATE, Z test, scissor")
    tx = Textures(b, 1604)
    b.textures = tx
    rng = np.random.RandomState(1605)
    dest = rand_ct32(rng, 64, 32)
    b.dest = dest

    t = b.add(Test("fog_cols", "64 one-pixel columns with F = 4*col (+3 on odd rows' band), 4 colour bands"))
    t.ad(("FOGCOL", FOGCOL(0x30, 0x60, 0x90)))
    bands = [(0, 0, 0, 0x80), (255, 255, 255, 0x80), (0x80, 0x40, 0x20, 0x33), (0x13, 0xEE, 0x7B, 0xFF)]
    for k, c in enumerate(bands):
        for col in range(64):
            f = min(255, 4 * col + (3 if k % 2 else 0))
            prim(t, "sprite", [dict(x=col, y=16 * k, rgba=c, f=f), dict(x=col + 1, y=16 * k + 16, rgba=c, f=f)], fge=1)
    t = b.add(Test("fog_tri", "fog weight interpolated over a Gouraud triangle (F 0 / 255 / 128)"))
    t.ad(("FOGCOL", FOGCOL(0xF0, 0x10, 0x80)))
    prim(t, "tri", [dict(x=1, y=1, rgba=(0x20, 0x90, 0xFF, 0x80), f=0), dict(x=63, y=6, rgba=(0xFF, 0xFF, 0x00, 0x80), f=255),
                    dict(x=7, y=63, rgba=(0x00, 0x40, 0x10, 0x80), f=128)], iip=1, fge=1)
    t = b.add(Test("fog_tex", "fog over a textured sprite (texture A MODULATE, F per band)"))
    t.ad(("FOGCOL", FOGCOL(0x55, 0xAA, 0x11)))
    tex_state(t, tx.tex0("A", tfx=0))
    for k in range(4):
        prim(t, "sprite", [dict(x=0, y=16 * k, rgba=(0x80, 0x60, 0xA0, 0x80), uv=(0, 0), f=64 * k + 21),
                           dict(x=64, y=16 * k + 16, rgba=(0x80, 0x60, 0xA0, 0x80), uv=(16, 4), f=64 * k + 21)],
             tme=1, fst=1, fge=1)
    # alpha test: 64 columns alpha = 4*col (+1 on the second band), AREF 0x40 (rows 0..31) / 0x81 (rows 32..63)
    for atst in ATST_NAMES:
        t = b.add(Test(f"atest_{atst}", f"ATST {atst}, AFAIL KEEP, AREF 0x40 / 0x81 bands", zpsm="Z24"))
        for band, aref in enumerate((0x40, 0x81)):
            t.ad(("TEST_1", TEST(ate=1, atst=atst, aref=aref, afail="KEEP")))
            for col in range(64):
                a = min(255, 4 * col + band)
                prim(t, "sprite", [dict(x=col, y=32 * band, z=0x1000 + col, rgba=(0xC0, 0x30, 0x90, a)),
                                   dict(x=col + 1, y=32 * band + 32, z=0x1000 + col, rgba=(0xC0, 0x30, 0x90, a))])
    for afail in ("KEEP", "FB_ONLY", "ZB_ONLY", "RGB_ONLY"):
        t = b.add(Test(f"afail_{afail}", f"ATST LESS AREF 0x40, AFAIL {afail}: colour and Z of failing pixels",
                       zpsm="Z24", clear=0x11223344, zclear=0x000777))
        t.ad(("TEST_1", TEST(ate=1, atst="LESS", aref=0x40, afail=afail)))
        for col in range(64):
            a = 2 * col
            prim(t, "sprite", [dict(x=col, y=0, z=0x00ABCD, rgba=(0xE0, 0x70, 0x0F, a)),
                               dict(x=col + 1, y=64, z=0x00ABCD, rgba=(0xE0, 0x70, 0x0F, a))])
    # blending: the source is texture B read 1:1 (DECAL, TCC 1); src_copy records it with ABE 0,
    # dest_only records the uploaded destination; then all 81 equations with FIX 0x5A
    def blend_test(name, desc, alpha, colclamp=1, pabe=0, fba=0, prim_abe=1, test=None, fbmsk=None):
        t = b.add(Test(name, desc, 64, 32))
        t.image(t.fbp * 32, 64, "CT32", 64, 32, dest.tobytes(), label="dest")
        tex_state(t, tx.tex0("B", tfx=1))
        t.ad(("ALPHA_1", alpha), ("COLCLAMP", colclamp), ("PABE", pabe), ("FBA_1", fba))
        if test is not None:
            t.ad(("TEST_1", test))
        if fbmsk is not None:
            t.ad(("FRAME_1", FRAME(t.fbp, 64, "CT32", fbmsk)))
            t.meta["fbmsk"] = hex(fbmsk)
        tex_sprite(t, 0, 0, 64, 32, 0, 0, 64, 32, abe=prim_abe)
        t.meta["blend"] = {"alpha": hex(alpha), "colclamp": colclamp, "pabe": pabe, "fba": fba, "abe": prim_abe}
        return t
    t = b.add(Test("dest_only", "the uploaded destination image, nothing drawn", 64, 32))
    t.image(t.fbp * 32, 64, "CT32", 64, 32, dest.tobytes(), label="dest")
    blend_test("src_copy", "texture B 1:1, ABE 0 (the blend source per pixel)", ALPHA(0, 2, 2, 2, 0x80), prim_abe=0)
    for a in range(3):
        for bb in range(3):
            for c in range(3):
                for d in range(3):
                    blend_test(f"blend_{a}{bb}{c}{d}", "ALPHA A B C D (A/B/D: 0 Cs 1 Cd 2 zero; C: 0 As 1 Ad 2 FIX), "
                               "FIX 0x5A, COLCLAMP 1", ALPHA(a, bb, c, d, 0x5A))
    for alpha, name in [(0x44, "game_44"), (ALPHA(0, 2, 2, 1, 0x80), "game_68"), (ALPHA(0, 2, 2, 2, 0x80), "game_a8")]:
        blend_test(f"blend_{name}", "game preset, COLCLAMP 1", alpha)
        blend_test(f"blend_{name}_cc0", "game preset, COLCLAMP 0 (wrap)", alpha, colclamp=0)
    blend_test("blend_0101_cc0", "(Cs - Cd) * Ad >> 7 + Cs with COLCLAMP 0 (negative and overflow wrap)", ALPHA(0, 1, 1, 0, 0x5A), colclamp=0)
    blend_test("pabe_44", "ALPHA 0x44 with PABE 1", 0x44, pabe=1)
    blend_test("fba_44", "ALPHA 0x44 with FBA 1", 0x44, fba=1)
    blend_test("fba_noblend", "ABE 0 with FBA 1", 0x44, fba=1, prim_abe=0)
    blend_test("date_0", "DATE 1 DATM 0, ABE 0", 0x44, prim_abe=0, test=TEST(date=1, datm=0))
    blend_test("date_1", "DATE 1 DATM 1, ABE 0", 0x44, prim_abe=0, test=TEST(date=1, datm=1))
    blend_test("date_game", "the shadow receivers' TEST 0x5C00D (Z ALWAYS here: no Z of its own) and ALPHA 0x44",
               0x44, test=0x5C00D & ~(3 << 17) | (1 << 17))
    blend_test("fbmsk", "ABE 0 with FBMSK 0xFF00F00F", 0x44, prim_abe=0, fbmsk=0xFF00F00F)
    # Z tests: 64 column sprites with Z = 0x800000 + col - 32 against a 0x800000 clear
    for ztst in ("NEVER", "ALWAYS", "GEQUAL", "GREATER"):
        t = b.add(Test(f"ztest_{ztst}", f"ZTST {ztst}: column Z = 0x800000 + col - 32 vs 0x800000", zpsm="Z24",
                       zclear=0x800000))
        t.ad(("TEST_1", TEST(ztst=ztst)))
        for col in range(64):
            z = 0x800000 + col - 32
            prim(t, "sprite", [dict(x=col, y=0, z=z, rgba=(0x40, 0x80, 0xC0, 0x80)),
                               dict(x=col + 1, y=64, z=z, rgba=(0x40, 0x80, 0xC0, 0x80))])
    t = b.add(Test("zmsk", "ZTST GEQUAL with ZMSK 1: colour passes, Z unchanged", zpsm="Z24", zclear=0x800000))
    t.ad(("ZBUF_1", ZBUF(t.zbp, "Z24", zmsk=1)), ("TEST_1", TEST(ztst="GEQUAL")))
    for col in range(64):
        z = 0x800000 + col - 32
        prim(t, "sprite", [dict(x=col, y=0, z=z, rgba=(0x40, 0x80, 0xC0, 0x80)),
                           dict(x=col + 1, y=64, z=z, rgba=(0x40, 0x80, 0xC0, 0x80))])
    t = b.add(Test("ztri_geq", "two crossing Gouraud-Z triangles with GEQUAL: per-pixel Z order", zpsm="Z24"))
    t.ad(("TEST_1", TEST(ztst="GEQUAL")))
    prim(t, "tri", [dict(x=1, y=1, z=0x100000, rgba=(255, 0, 0, 0x80)), dict(x=63, y=10, z=0x900000, rgba=(255, 0, 0, 0x80)),
                    dict(x=10, y=63, z=0x500000, rgba=(255, 0, 0, 0x80))])
    prim(t, "tri", [dict(x=63, y=1, z=0x800000, rgba=(0, 255, 0, 0x80)), dict(x=2, y=20, z=0x200000, rgba=(0, 255, 0, 0x80)),
                    dict(x=55, y=63, z=0x600000, rgba=(0, 255, 0, 0x80))])
    t = b.add(Test("scissor", "SCISSOR (10..40, 5..50) over a covering triangle, sprite and line"))
    t.ad(("SCISSOR_1", SCISSOR(10, 40, 5, 50)))
    cov_setup(t)
    prim(t, "tri", [dict(x=0, y=0, rgba=id_rgba(0)), dict(x=64, y=0, rgba=id_rgba(0)), dict(x=0, y=64, rgba=id_rgba(0))], abe=1)
    prim(t, "sprite", [dict(x=20, y=20, rgba=id_rgba(1)), dict(x=64, y=64, rgba=id_rgba(1))], abe=1)
    prim(t, "line", [dict(x=0, y=45, rgba=id_rgba(2)), dict(x=63, y=2, rgba=id_rgba(2))], abe=1)
    return b


ATST_NAMES = ["NEVER", "ALWAYS", "LESS", "LEQUAL", "EQUAL", "GEQUAL", "GREATER", "NOTEQUAL"]


# ---------------------------------------------------------------------------
# dither (DTHE / DIMX), CT16 conversion, and the frame's XYOFFSET half line
DIMX_STD = [[-4, 2, -3, 3], [0, -2, 1, -1], [-3, 3, -4, 2], [1, -1, 0, -2]]
DIMX_ID = [[-4, -3, -2, -1], [0, 1, 2, 3], [3, 2, 1, 0], [-1, -2, -3, -4]]


def batch_frame() -> Batch:
    b = Batch("frame", "dithering (DTHE/DIMX) and CT16 conversion; the frame's XYOFFSET half-line offsets")

    def grad(t):
        prim(t, "sprite", [dict(x=0, y=0, rgba=(0, 0, 0, 0x80)), dict(x=64, y=4, rgba=(0, 0, 0, 0x80))])
        for k in range(8):                       # flat bands: colours 8k+1 .. 8k+7 steps
            c = 3 + 33 * k
            prim(t, "sprite", [dict(x=0, y=4 + 4 * k, rgba=(c, c + 1, c + 2, 0x80)),
                               dict(x=64, y=8 + 4 * k, rgba=(c, c + 1, c + 2, 0x80))])
        prim(t, "tri", [dict(x=0, y=36, rgba=(0, 0, 0, 0x80)), dict(x=64, y=36, rgba=(255, 128, 64, 0x80)),
                        dict(x=0, y=64, rgba=(40, 255, 7, 0x80))], iip=1)
        prim(t, "tri", [dict(x=64, y=36, rgba=(255, 128, 64, 0x80)), dict(x=64, y=64, rgba=(255, 255, 255, 0x80)),
                        dict(x=0, y=64, rgba=(40, 255, 7, 0x80))], iip=1)
    for psm in ("CT16", "CT32"):
        for dname, dm in (("off", None), ("std", DIMX_STD), ("id", DIMX_ID)):
            for cc in (1, 0):
                if psm == "CT32" and cc == 0:
                    continue
                t = b.add(Test(f"dither_{psm.lower()}_{dname}_cc{cc}", f"{psm} frame, DTHE {'0' if dm is None else '1'} "
                               f"DIMX {dname}, COLCLAMP {cc}: flat bands + Gouraud", 64, 64, psm=psm))
                t.ad(("DTHE", 0 if dm is None else 1), ("COLCLAMP", cc))
                if dm is not None:
                    t.ad(("DIMX", DIMX(dm)))
                    t.meta["dimx"] = dm
                grad(t)
    t = b.add(Test("dither_ct16_alpha", "CT16 alpha bit: A 0x00 / 0x7F / 0x80 / 0xFF sprites", 64, 64, psm="CT16"))
    for k, a in enumerate((0x00, 0x7F, 0x80, 0xFF)):
        prim(t, "sprite", [dict(x=16 * k, y=0, rgba=(100, 150, 200, a)), dict(x=16 * k + 16, y=64, rgba=(100, 150, 200, a))])
    # XYOFFSET: the same primitive list (window coordinates + (1792, 1936)) under the frame's two offsets
    rng = np.random.RandomState(1606)
    tris = [[rnd_xy(rng, 1, 63) for _ in range(3)] for _ in range(6)]
    for name, ofs in (("xyofs_1936_0", (1792.0, 1936.0)), ("xyofs_1936_5", (1792.0, 1936.5)),
                      ("xyofs_x_1792_5", (1792.5, 1936.0))):
        t = b.add(Test(name, f"XYOFFSET {ofs}; primitives at window coords + (1792, 1936)", ofs=ofs))
        cov_setup(t)
        for k, tri in enumerate(tris):
            prim(t, "tri", [dict(x=x, y=y, rgba=id_rgba(k), base=(1792.0, 1936.0)) for x, y in tri], abe=1)
        prim(t, "sprite", [dict(x=10.25, y=40.5, rgba=id_rgba(6), base=(1792.0, 1936.0)),
                           dict(x=30.75, y=50.25, rgba=id_rgba(6), base=(1792.0, 1936.0))], abe=1)
        prim(t, "line", [dict(x=2, y=60.5, rgba=id_rgba(7), base=(1792.0, 1936.0)),
                         dict(x=60, y=55, rgba=id_rgba(7), base=(1792.0, 1936.0))], abe=1)
        prim(t, "tri", [dict(x=40, y=40, rgba=(0, 0, 0x10, 0x80), base=(1792.0, 1936.0)),
                        dict(x=63, y=45, rgba=(0, 0, 0x80, 0x80), base=(1792.0, 1936.0)),
                        dict(x=45, y=63, rgba=(0, 0, 0xF0, 0x80), base=(1792.0, 1936.0))], iip=1, abe=1)
        t.meta["base"] = [1792.0, 1936.0]
    # the game's full field: 512x224 CT32 with the two offsets, a few large Gouraud triangles
    for name, ofy in (("field_1936_0", 1936.0), ("field_1936_5", 1936.5)):
        t = b.add(Test(name, f"512x224 field, XYOFFSET (1792, {ofy}), Gouraud triangles in centred coords", 512, 224,
                       ofs=(1792.0, ofy), clear=0x80000000))
        rng2 = np.random.RandomState(1607)
        for k in range(6):
            pts = [(q(rng2.uniform(0, 512)), q(rng2.uniform(0, 224))) for _ in range(3)]
            prim(t, "tri", [dict(x=x, y=y, rgba=tuple(int(c) for c in rng2.randint(0, 256, 3)) + (0x80,),
                                 base=(1792.0, 1936.0)) for x, y in pts], iip=1)
        t.meta["base"] = [1792.0, 1936.0]
    return b




# ---------------------------------------------------------------------------
# probe2: follow-up questions raised by the first analysis
def batch_probe2() -> Batch:
    b = Batch("probe2", "follow-ups: which primitives dither, Gouraud order dependence and 1-D gradients, "
                        "line diamond-boundary starts")
    # 1. dithering per primitive kind (CT16, DTHE 1, DIMX id) and the CT32 references
    for psm in ("CT16", "CT32"):
        t = b.add(Test(f"dither_kinds_{psm.lower()}", "DTHE 1 DIMX id: flat tri, Gouraud tri (constant and ramp), "
                       "IIP 1 sprite, DECAL-textured sprite, Gouraud line", 64, 64, psm=psm))
        t.ad(("DTHE", 1), ("DIMX", DIMX(DIMX_ID)))
        c = (0x5B, 0x9D, 0x2E, 0x80)
        prim(t, "tri", [dict(x=0, y=0, rgba=c), dict(x=30, y=0, rgba=c), dict(x=0, y=30, rgba=c)])
        prim(t, "tri", [dict(x=32, y=0, rgba=c), dict(x=62, y=0, rgba=c), dict(x=32, y=30, rgba=c)], iip=1)
        prim(t, "tri", [dict(x=0, y=32, rgba=(0, 0, 0, 0x80)), dict(x=30, y=32, rgba=(255, 128, 60, 0x80)),
                        dict(x=0, y=62, rgba=(30, 200, 255, 0x80))], iip=1)
        prim(t, "sprite", [dict(x=34, y=34, rgba=c), dict(x=48, y=48, rgba=c)], iip=1)
        prim(t, "line", [dict(x=50, y=34, rgba=(0, 0, 0, 0x80)), dict(x=63, y=62, rgba=(255, 255, 255, 0x80))], iip=1)
        t.meta["dimx"] = DIMX_ID
    # 2. Gouraud: the same random triangles in all six vertex orders; 1-D ramps
    import itertools
    rng = np.random.RandomState(1608)
    for k in range(2):
        pts = [rnd_xy(rng, 0.5, 63.5) for _ in range(3)]
        cols = [tuple(int(c) for c in rng.randint(0, 256, 4)) for _ in range(3)]
        for o, order in enumerate(itertools.permutations(range(3))):
            t = b.add(Test(f"gouraud_order_{k}_{o}", f"random Gouraud triangle {k}, vertex order {order}", zpsm="Z24"))
            prim(t, "tri", [dict(x=pts[i][0], y=pts[i][1], z=[0x10000, 0xFF0000, 0x801234][i], rgba=cols[i])
                            for i in order], iip=1)
    for k, (dx0, dy0, a) in enumerate([(0, 0, 60), (0.25, 0, 60), (0, 0, 7), (0.5, 0.5, 255), (0.0625, 0, 1)]):
        t = b.add(Test(f"ramp_x_{k}", f"colour depends on x only: 0 at x={dx0}, {a} at x={dx0}+60"))
        prim(t, "tri", [dict(x=dx0, y=dy0, rgba=(0, 0, 0, 0)), dict(x=dx0 + 60, y=dy0, rgba=(a, a, a, a)),
                        dict(x=dx0, y=dy0 + 60, rgba=(0, 0, 0, 0))], iip=1)
        t = b.add(Test(f"ramp_y_{k}", f"colour depends on y only: 0 at y={dy0}, {a} at y={dy0}+60"))
        prim(t, "tri", [dict(x=dx0, y=dy0, rgba=(0, 0, 0, 0)), dict(x=dx0 + 60, y=dy0, rgba=(0, 0, 0, 0)),
                        dict(x=dx0, y=dy0 + 60, rgba=(a, a, a, a))], iip=1)
    # 3. lines starting and ending exactly on diamond boundaries, all eight classes, both majors and directions
    t = b.add(Test("line_diamond", "lines whose start / end lie on a pixel diamond's boundary (|dx|+|dy| = 1/2)"))
    cov_setup(t)
    offs = [(-0.25, -0.25), (0.25, -0.25), (-0.25, 0.25), (0.25, 0.25), (-0.5, 0), (0.5, 0), (0, -0.5), (0, 0.5)]
    k = 0
    for j, (ox, oy) in enumerate(offs):
        for major, (ddx, ddy) in enumerate([(9, 2), (-9, 3)]):
            if k >= 24:
                break
            sx, sy = 4 + 7 * (k % 8) + ox, 6 + 20 * (k // 8) + oy
            prim(t, "line", [dict(x=sx, y=sy, rgba=id_rgba(k)), dict(x=sx + ddx if ddx > 0 else sx + ddx + 12,
                                                                     y=sy + ddy, rgba=id_rgba(k))], abe=1)
            k += 1
    t = b.add(Test("line_diamond_end", "lines ending on diamond boundaries (y-major and x-major, both directions)"))
    cov_setup(t)
    k = 0
    for j, (ox, oy) in enumerate(offs):
        for (ddx, ddy) in [(2, 9), (-3, -9), (9, -2)]:
            if k >= 24:
                break
            ex, ey = 12 + 7 * (k % 8) + ox, 12 + 20 * (k // 8) + oy
            prim(t, "line", [dict(x=ex - ddx, y=ey - ddy, rgba=id_rgba(k)), dict(x=ex, y=ey, rgba=id_rgba(k))], abe=1)
            k += 1
    return b


def batches() -> list[Batch]:
    return [batch_layout(), batch_raster(), batch_shade(), batch_texture(), batch_pixel(), batch_frame(),
            batch_probe2()]
