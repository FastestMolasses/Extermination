#!/usr/bin/env python3
"""gs_conformance_analyse.py - derive the GS rules from the recorded
conformance captures (tools/gs_conformance.py; docs/GS_CONFORMANCE.md).

No emulator.  Every rule is a candidate model evaluated against the
recorded pixels of PCSX2's software renderer; the report states, per rule,
how many pixels the best candidate reproduces and every candidate's score.
A rule counts as measured only where its candidate matches every recorded
pixel it covers.

Clean room: the candidate models come from public GS documentation (the
register semantics, the documented pixel-pipeline formulas) and generic
rasterisation conventions; nothing here was taken from emulator source.

Usage:  .venv/bin/python tools/gs_conformance_analyse.py [section ...]
Writes build/b16/gscap/analysis.json and prints a summary.
"""
from __future__ import annotations

import json
import os
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
CAP = Path(os.environ.get("GSCAP_OUT", ROOT / "build/b16/gscap"))


def load(batch: str):
    doc = json.loads((CAP / batch / "batch.json").read_text())
    tests = {t["name"]: t for t in doc["tests"]}
    return tests


def arr(batch: str, name: str, key: str = "color"):
    return np.load(CAP / batch / f"{name}.npz")[key]


def inputs(batch: str):
    return dict(np.load(CAP / batch / "inputs.npz"))


def ch(v, i):
    return (v >> (8 * i)) & 0xFF


# ---------------------------------------------------------------------------
# primitive geometry in window coordinates, 1/16 pixel integers
def win16(t: dict, v: dict) -> tuple[int, int]:
    base = v.get("base", t["ofs"])
    ox, oy = t["ofs"]
    return round((v["x"] + base[0] - ox) * 16), round((v["y"] + base[1] - oy) * 16)


def expand_prims(t: dict):
    """Yield (kind, [vertex dicts], index) for every drawn primitive,
    expanding strips and fans into triangles / lines."""
    for i, p in enumerate(t["meta"].get("prims", [])):
        k, vs = p["kind"], p["v"]
        if k == "tri":
            yield "tri", vs, i
        elif k == "tristrip":
            for j in range(len(vs) - 2):
                yield "tri", vs[j:j + 3], i
        elif k == "trifan":
            for j in range(1, len(vs) - 1):
                yield "tri", [vs[0], vs[j], vs[j + 1]], i
        elif k == "linestrip":
            for j in range(len(vs) - 1):
                yield "line", vs[j:j + 2], i
        else:
            yield k, vs, i


TIE_RULES = {
    "top-left": lambda nx, ny: (nx > 0) | ((nx == 0) & (ny > 0)),
    "top-right": lambda nx, ny: (nx < 0) | ((nx == 0) & (ny > 0)),
    "bottom-left": lambda nx, ny: (nx > 0) | ((nx == 0) & (ny < 0)),
    "bottom-right": lambda nx, ny: (nx < 0) | ((nx == 0) & (ny < 0)),
    "all": lambda nx, ny: True,
    "none": lambda nx, ny: False,
}


def tri_mask(p16, w, h, sx=0, sy=0, tie="top-left"):
    """Edge-function coverage with exact integers.  Pixel (x, y) is sampled
    at (16x + sx, 16y + sy) in 1/16 units; on an edge (E = 0) a pixel is
    included when the edge's inward normal satisfies the tie rule."""
    (x0, y0), (x1, y1), (x2, y2) = p16
    area = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0)
    if area == 0:
        return np.zeros((h, w), bool)
    s = 1 if area > 0 else -1
    Y, X = np.mgrid[0:h, 0:w]
    px, py = X * 16 + sx, Y * 16 + sy
    m = np.ones((h, w), bool)
    for (ax, ay), (bx, by) in (((x0, y0), (x1, y1)), ((x1, y1), (x2, y2)), ((x2, y2), (x0, y0))):
        e = s * ((bx - ax) * (py - ay) - (by - ay) * (px - ax))
        nx, ny = -s * (by - ay), s * (bx - ax)            # gradient of e = inward normal
        m &= (e > 0) | ((e == 0) & TIE_RULES[tie](nx, ny))
    return m


def sprite_mask(p16, w, h, sx=0, sy=0, rule="half-open"):
    (x0, y0), (x1, y1) = p16
    xa, xb, ya, yb = min(x0, x1), max(x0, x1), min(y0, y1), max(y0, y1)
    Y, X = np.mgrid[0:h, 0:w]
    px, py = X * 16 + sx, Y * 16 + sy
    if rule == "half-open":
        return (px >= xa) & (px < xb) & (py >= ya) & (py < yb)
    if rule == "closed":
        return (px >= xa) & (px <= xb) & (py >= ya) & (py <= yb)
    if rule == "open-closed":
        return (px > xa) & (px <= xb) & (py > ya) & (py <= yb)
    if rule == "as-given":            # half-open from v1 towards v2 regardless of order
        cx = (px >= x0) & (px < x1) if x0 <= x1 else (px > x1) & (px <= x0)
        cy = (py >= y0) & (py < y1) if y0 <= y1 else (py > y1) & (py <= y0)
        return cx & cy
    raise KeyError(rule)


def measured_ids(col):
    return (col & 0xFFFFFF).astype(np.int64)


def coverage_tests(batches=("raster", "frame", "pixel", "probe2")):
    out = []
    for b in batches:
        for name, t in load(b).items():
            if t["meta"].get("mode") in ("coverage_ids", "coverage_count"):
                out.append((b, name, t))
    return out


def score_coverage(kind: str, model, variants):
    """For every variant: mismatching pixels over all primitives of `kind`
    in the id-coverage tests (per-primitive masks vs recorded id bits), and
    for the count tests (sum of masks vs recorded count)."""
    res = {}
    for var in variants:
        bad = total = prims = 0
        worst = []
        for b, name, t in coverage_tests():
            col = arr(b, name)
            h, w = col.shape
            ids = measured_ids(col)
            sc = t["meta"].get("scissor")
            if t["meta"]["mode"] == "coverage_ids":
                for k, (pk, vs, i) in enumerate(expand_prims(t)):
                    if pk != kind or any(v["rgba"] != vs[0]["rgba"] for v in vs):
                        continue
                    idv = vs[-1]["rgba"][0] | vs[-1]["rgba"][1] << 8 | vs[-1]["rgba"][2] << 16
                    if bin(idv).count("1") != 1:
                        continue
                    got = (ids & idv) != 0
                    pred = model([win16(t, v) for v in vs], w, h, *var)
                    if name == "scissor":
                        pred &= scissor_region(w, h)
                    n = int((got != pred).sum())
                    bad += n
                    total += int(got.sum())
                    prims += 1
                    if n:
                        worst.append((f"{b}/{name}#{i}", n))
            else:
                pred = np.zeros((h, w), np.int64)
                any_kind = False
                for pk, vs, i in expand_prims(t):
                    if pk != kind:
                        continue
                    any_kind = True
                    pred += model([win16(t, v) for v in vs], w, h, *var)
                    prims += 1
                if any_kind:
                    got = ids & 0xFF
                    n = int((got != pred).sum())
                    bad += n
                    total += int(got.sum())
                    if n:
                        worst.append((f"{b}/{name}", n))
        res[" ".join(map(str, var))] = {"mismatched_pixels": bad, "covered_pixels": total, "primitives": prims,
                                        "worst": sorted(worst, key=lambda r: -r[1])[:6]}
    return res


def scissor_region(w, h):
    m = np.zeros((h, w), bool)
    m[5:51, 10:41] = True
    return m


def analyse_triangles():
    variants = [(sx, sy, tie) for sx in (0, 8) for sy in (0, 8) for tie in TIE_RULES]
    res = score_coverage("tri", tri_mask, variants)
    best = min(res, key=lambda k: res[k]["mismatched_pixels"])
    return {"best": best, "best_result": res[best],
            "all": {k: v["mismatched_pixels"] for k, v in res.items()}}


def analyse_sprites():
    variants = [(sx, sy, r) for sx in (0, 8) for sy in (0, 8) for r in ("half-open", "closed", "open-closed", "as-given")]
    res = score_coverage("sprite", sprite_mask, variants)
    best = min(res, key=lambda k: res[k]["mismatched_pixels"])
    return {"best": best, "best_result": res[best], "all": {k: v["mismatched_pixels"] for k, v in res.items()}}


SECTIONS = {"tri": analyse_triangles, "sprite": analyse_sprites}  # more below


# ---------------------------------------------------------------------------
# lines and points
def line_mask(p16, w, h, rnd="half-up", span="start-incl", major_tie="x"):
    """Major-axis stepping.  Major samples at integer pixel positions inside
    the half-open span from the start vertex towards the end vertex
    ('start-incl': [start, end) in travel direction; 'min-incl': [min, max)
    regardless of direction).  The minor coordinate is the exact line value
    at that sample, rounded to the nearest pixel ('half-up': floor(v + 1/2),
    'half-down': ceil(v - 1/2))."""
    from fractions import Fraction
    (x0, y0), (x1, y1) = p16
    dx, dy = x1 - x0, y1 - y0
    m = np.zeros((h, w), bool)
    if dx == 0 and dy == 0:
        return m
    xmajor = abs(dx) > abs(dy) or (abs(dx) == abs(dy) and major_tie == "x")
    if not xmajor:                      # swap roles
        (x0, y0), (x1, y1) = (y0, x0), (y1, x1)
        dx, dy = dy, dx
    lim = (w if xmajor else h)
    for p in range(lim):
        s = p * 16
        if span == "start-incl":
            inside = (x0 <= s < x1) if dx > 0 else (x1 < s <= x0)
        else:
            inside = min(x0, x1) <= s < max(x0, x1)
        if not inside:
            continue
        v = Fraction(y0) + Fraction(s - x0) * Fraction(dy, dx)
        if rnd == "half-up":
            r = (v + 8) // 16
        else:
            r = -((-(v - 8)) // 16)
        r = int(r)
        if xmajor and 0 <= r < h:
            m[r, p] = True
        elif not xmajor and 0 <= r < w:
            m[p, r] = True
    return m


def line_mask_diamond(p16, w, h, strict=True, major_tie="x", boundary=()):
    """Diamond-exit endpoints over major-axis stepping.  Every major-axis
    pixel centre line p crossed strictly inside the segment lights the pixel
    (p, round-half-up(minor(p))), unless the END point lies inside that
    pixel's diamond |dx| + |dy| < 1/2 (the line does not leave it).  The
    pixel whose centre line lies just before the start (floor / ceil of the
    start, by direction) is lit only when the START point lies inside its
    diamond; a start exactly on a centre line lights its pixel."""
    from fractions import Fraction as Fr
    (x0, y0), (x1, y1) = p16
    dx, dy = x1 - x0, y1 - y0
    m = np.zeros((h, w), bool)
    if dx == 0 and dy == 0:
        return m
    xmajor = abs(dx) > abs(dy) or (abs(dx) == abs(dy) and major_tie == "x")
    if not xmajor:
        (x0, y0), (x1, y1) = (y0, x0), (y1, x1)
        dx, dy = dy, dx
    sgn = 1 if dx > 0 else -1
    minor = lambda s: Fr(y0) + Fr(s - x0) * Fr(dy, dx)
    rnd = lambda v: int((v + 8) // 16)

    def inside(px16, py16, qx, qy):          # point (px16, py16) in the diamond of pixel (qx, qy)
        ox, oy = px16 - 16 * qx, py16 - 16 * qy  # (major, minor) offsets
        if boundary == "minor-negative":
            d = abs(ox) + abs(oy)
            return d < 8 or (d == 8 and oy < 0)
        if not xmajor:                       # back to window axes
            ox, oy = oy, ox
        d = abs(ox) + abs(oy)
        if d == 8 and boundary:
            return ((ox > 0) - (ox < 0), (oy > 0) - (oy < 0)) in boundary
        return d < 8 if strict else d <= 8

    def light(p, r):
        if xmajor and 0 <= p < w and 0 <= r < h:
            m[r, p] = True
        elif not xmajor and 0 <= p < h and 0 <= r < w:
            m[p, r] = True
    lo, hi = min(x0, x1), max(x0, x1)
    for p in range(-1, max(w, h) + 1):
        s = 16 * p
        if s == x0:
            light(p, rnd(Fr(y0)))                        # start on a centre line
        elif lo < s < hi:
            r = rnd(minor(s))
            if not inside(x1, y1, p, r):
                light(p, r)
        elif s == x1:
            pass                                          # end on a centre line: never exits
        else:
            before = (s < x0 and s > x0 - 16) if sgn > 0 else (s > x0 and s < x0 + 16)
            if before:
                r = rnd(Fr(y0))
                if inside(x0, y0, p, r):
                    light(p, r)
    return m


def point_mask(p16, w, h, rnd="half-up"):
    (x0, y0), = p16
    m = np.zeros((h, w), bool)
    f = (lambda v: (v + 8) // 16) if rnd == "half-up" else (lambda v: v // 16) if rnd == "floor" else \
        (lambda v: -((-v) // 16))
    x, y = f(x0), f(y0)
    if 0 <= x < w and 0 <= y < h:
        m[y, x] = True
    return m


def analyse_lines():
    variants = [(r, s, mt) for r in ("half-up", "half-down") for s in ("start-incl", "min-incl") for mt in ("x", "y")]
    res = score_coverage("line", line_mask, variants)
    res.update({f"diamond-exit strict={st} tie={mt}": v for (st, mt), v in
                zip([(st, mt) for st in (True, False) for mt in ("x", "y")],
                    score_coverage("line", line_mask_diamond,
                                   [(st, mt) for st in (True, False) for mt in ("x", "y")]).values())})
    # a start / end exactly on a diamond boundary (|dx| + |dy| = 1/2): fixed class sets vs the
    # measured rule (inside iff the offset along the MINOR axis is negative)
    for label, bd in (("upper-left edge only", ((-1, -1),)), ("upper edges", ((-1, -1), (1, -1))),
                      ("left edges", ((-1, -1), (-1, 1)))):
        res[f"diamond-exit, boundary: {label}"] = score_coverage("line", line_mask_diamond,
                                                                 [(True, "x", bd)])[f"True x {bd}"]
    res["diamond-exit, boundary counts as inside iff the minor-axis offset < 0"] = score_coverage(
        "line", line_mask_diamond, [(True, "x", "minor-negative")])["True x minor-negative"]
    best = min(res, key=lambda k: res[k]["mismatched_pixels"])
    return {"best": best, "best_result": res[best],
            "all": {k: v["mismatched_pixels"] for k, v in res.items()}}


def analyse_points():
    res = score_coverage("point", point_mask, [("half-up",), ("floor",), ("ceil",)])
    best = min(res, key=lambda k: res[k]["mismatched_pixels"])
    return {"best": best, "best_result": res[best], "all": {k: v["mismatched_pixels"] for k, v in res.items()}}


SECTIONS.update({"line": analyse_lines, "point": analyse_points})


# ---------------------------------------------------------------------------
# helpers for exact attribute models
def bary(p16, X, Y):
    """Exact barycentric numerators (w0, w1, w2) and the area A (1/16 units),
    at pixel positions (X, Y) sampled at integer coordinates."""
    (x0, y0), (x1, y1), (x2, y2) = p16
    A = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0)
    px, py = X.astype(np.int64) * 16, Y.astype(np.int64) * 16
    w1 = (px - x0) * (y2 - y0) - (py - y0) * (x2 - x0)
    w2 = (x1 - x0) * (py - y0) - (y1 - y0) * (px - x0)
    return A - w1 - w2, w1, w2, A


def floor_div(n, d):
    return np.floor_divide(n, d) if d > 0 else np.floor_divide(-n, -d)


def split(col):
    col = col.astype(np.int64)
    return [(col >> (8 * i)) & 0xFF for i in range(4)]


def analyse_gouraud():
    """Gouraud colour: recorded channel values vs floor of the exact
    barycentric value at the pixel's integer sample point."""
    T = load("shade")
    stats = {"channel_values": 0, "equal_floor_exact": 0, "diff_plus1": 0, "diff_minus1": 0, "other": 0,
             "max_distance_to_integer_of_mismatch": 0.0}
    per = {}
    for name in ["gouraud_big", "gouraud_small", "gouraud_strip"] + [f"gouraud_rand_{i}" for i in range(8)]:
        t = T[name]
        col = arr("shade", name).astype(np.int64)
        claimed = np.zeros(col.shape, bool)
        n0 = stats["channel_values"]
        e0 = stats["equal_floor_exact"]
        for pk, vs, i in expand_prims(t):
            p = [win16(t, v) for v in vs]
            m = tri_mask(p, 64, 64) & ~claimed
            claimed |= m
            Y, X = np.nonzero(m)
            w0, w1, w2, A = bary(p, X, Y)
            for c in range(4):
                cv = [v["rgba"][c] for v in vs]
                num = w0 * cv[0] + w1 * cv[1] + w2 * cv[2]
                fl = floor_div(num, A)
                got = (col[Y, X] >> (8 * c)) & 0xFF
                d = got - fl
                stats["channel_values"] += len(d)
                stats["equal_floor_exact"] += int((d == 0).sum())
                stats["diff_plus1"] += int((d == 1).sum())
                stats["diff_minus1"] += int((d == -1).sum())
                stats["other"] += int((np.abs(d) > 1).sum())
                if (d != 0).any():
                    ex = num[d != 0] / A
                    dist = np.abs(ex - np.round(ex)).max()
                    stats["max_distance_to_integer_of_mismatch"] = max(stats["max_distance_to_integer_of_mismatch"],
                                                                       round(float(dist), 4))
        per[name] = {"values": stats["channel_values"] - n0, "equal": stats["equal_floor_exact"] - e0}
    return {"floor_of_exact_barycentric": stats, "per_test": per,
            "verdict": "not exact: the recorded colour equals floor(exact) except near integers; the "
                       "renderer's interpolation arithmetic is not yet identified"}


def analyse_flat():
    T = load("shade")
    t = T["flat_select"]
    col = arr("shade", "flat_select")
    out = []
    ok = True
    for pk, vs, i in expand_prims(t):
        p = [win16(t, v) for v in vs]
        m = {"tri": lambda: tri_mask(p, 64, 64), "sprite": lambda: sprite_mask(p, 64, 64),
             "line": lambda: line_mask_diamond(p, 64, 64, boundary="minor-negative"),
             "point": lambda: point_mask(p, 64, 64)}[pk]()
        vals = np.unique(col[m])
        words = [v["rgba"][0] | v["rgba"][1] << 8 | v["rgba"][2] << 16 | v["rgba"][3] << 24 for v in vs]
        which = [words.index(int(v)) if int(v) in words else None for v in vals]
        good = which == [len(vs) - 1]
        ok &= good
        out.append({"prim": i, "kind": pk, "iip": t["meta"]["prims"][i]["flags"].get("iip", 0),
                    "vertex_taken": which, "last_vertex": good})
    return {"all_take_last_vertex": ok, "cases": out}


def analyse_z():
    T = load("shade")
    res = {}
    for name, fmt in (("z_interp_z24", 24), ("z_interp_z32", 32)):
        t = T[name]
        z = arr("shade", name, "z").astype(np.int64)
        (pk, vs, i), = list(expand_prims(t))
        p = [win16(t, v) for v in vs]
        m = tri_mask(p, 64, 64)
        Y, X = np.nonzero(m)
        w0, w1, w2, A = bary(p, X, Y)
        zv = [v["z"] for v in vs]
        num = w0 * zv[0] + w1 * zv[1] + w2 * zv[2]
        fl = floor_div(num, A)
        got = z[Y, X] & ((1 << fmt) - 1)
        d = got - fl
        res[name] = {"pixels": int(len(d)), "equal_floor_exact": int((d == 0).sum()),
                     "max_abs_diff": int(np.abs(d).max()), "mean_diff": round(float(d.mean()), 2),
                     "top_byte_values": sorted(set((z[Y, X] >> 24).tolist()))[:6] if fmt == 24 else None}
    t = T["z_small"]
    z = arr("shade", "z_small", "z").astype(np.int64)
    prims = list(expand_prims(t))
    spr = prims[1][1]
    p = [win16(t, v) for v in spr]
    m = sprite_mask(p, 64, 64)
    res["sprite_z_values"] = [hex(v) for v in np.unique(z[m] & 0xFFFFFF)]
    res["sprite_vertex_z"] = [hex(v["z"]) for v in spr]
    t = T["z_range"]
    z = arr("shade", "z_range", "z").astype(np.int64)
    rows = []
    for pk, vs, i in expand_prims(t):
        p = [win16(t, v) for v in vs]
        m = sprite_mask(p, 64, 64)
        rows.append({"z_in": hex(vs[1]["z"]), "stored_low24": [hex(v) for v in np.unique(z[m] & 0xFFFFFF)],
                     "top_byte": [hex(v) for v in np.unique(z[m] >> 24)],
                     "colour_written": bool((arr("shade", "z_range")[m] != 0).all())})
    res["z24_out_of_range"] = rows
    t = T["z_range16"]
    z = arr("shade", "z_range16", "z").astype(np.int64)
    rows = []
    for pk, vs, i in expand_prims(t):
        if pk != "sprite":
            continue
        p = [win16(t, v) for v in vs]
        m = sprite_mask(p, 64, 64)
        rows.append({"z_in": hex(vs[1]["z"]), "stored": [hex(v) for v in np.unique(z[m])]})
    res["z16_out_of_range"] = rows
    return res


SECTIONS.update({"gouraud": analyse_gouraud, "flat": analyse_flat, "z": analyse_z})


# ---------------------------------------------------------------------------
# texturing
def tex_channels(tex):
    t = tex.astype(np.int64)
    return np.stack([(t >> (8 * i)) & 0xFF for i in range(4)], -1)          # (h, w, 4)


def wrap_idx(i, n, mode="repeat", lo=0, hi=None):
    if mode == "repeat":
        return i & (n - 1)
    if mode == "clamp":
        return np.clip(i, 0, n - 1)
    if mode == "region_clamp":
        return np.clip(i, lo, hi)
    if mode == "region_repeat":
        return (i & lo) | hi
    raise KeyError(mode)


def bilinear(tex, u, v, bits=4, form="sum", wrap=("repeat",), rnd=0):
    """u, v: texel-space coordinates (float arrays).  Samples at u - 1/2,
    weights quantised to `bits` fractional bits (floor), result floored."""
    th, tw = tex.shape
    T = tex_channels(tex)
    one = 1 << bits
    U = np.floor((u - 0.5) * one).astype(np.int64)
    V = np.floor((v - 0.5) * one).astype(np.int64)
    fu, fv = U & (one - 1), V & (one - 1)
    i0, j0 = U >> bits, V >> bits
    i1, j1 = i0 + 1, j0 + 1
    wi = lambda i: wrap_idx(i, tw, *wrap)
    wj = lambda j: wrap_idx(j, th, *wrap)
    t00, t10 = T[wj(j0), wi(i0)], T[wj(j0), wi(i1)]
    t01, t11 = T[wj(j1), wi(i0)], T[wj(j1), wi(i1)]
    fu, fv = fu[..., None], fv[..., None]
    if form == "sum":
        acc = t00 * (one - fu) * (one - fv) + t10 * fu * (one - fv) + t01 * (one - fu) * fv + t11 * fu * fv
        out = (acc + rnd) >> (2 * bits)
    elif form == "lerp-uv":
        a = t00 + (((t10 - t00) * fu) >> bits)
        b = t01 + (((t11 - t01) * fu) >> bits)
        out = a + (((b - a) * fv) >> bits)
    elif form == "lerp-vu":
        a = t00 + (((t01 - t00) * fv) >> bits)
        b = t10 + (((t11 - t10) * fv) >> bits)
        out = a + (((b - a) * fu) >> bits)
    elif form == "lerp-uv-div":           # truncation toward zero in each lerp
        a = t00 + np.trunc((t10 - t00) * fu / one).astype(np.int64)
        b = t01 + np.trunc((t11 - t01) * fu / one).astype(np.int64)
        out = a + np.trunc((b - a) * fv / one).astype(np.int64)
    return out


def nearest(tex, u, v, wrap=("repeat",)):
    th, tw = tex.shape
    T = tex_channels(tex)
    return T[wrap_idx(np.floor(v).astype(np.int64), th, *wrap), wrap_idx(np.floor(u).astype(np.int64), tw, *wrap)]


def sprite_uv(t, prim_meta, X, Y, sx=0.0, mode="uv", tw=16, th=16):
    """Exact affine U/V over a sprite at pixel sample (X + sx, Y + sx)."""
    v0, v1 = prim_meta["v"]
    (x0, y0), (x1, y1) = win16(t, v0), win16(t, v1)
    x0, y0, x1, y1 = x0 / 16, y0 / 16, x1 / 16, y1 / 16
    if mode == "uv":
        (u0, w0), (u1, w1) = v0["uv"], v1["uv"]
    else:
        (u0, w0), (u1, w1) = (v0["st"][0] * tw, v0["st"][1] * th), (v1["st"][0] * tw, v1["st"][1] * th)
    u = u0 + (X + sx - x0) * (u1 - u0) / (x1 - x0)
    v = w0 + (Y + sx - y0) * (w1 - w0) / (y1 - y0)
    return u, v


def pack(ch):
    ch = ch.astype(np.int64)
    return (ch[..., 0] | ch[..., 1] << 8 | ch[..., 2] << 16 | ch[..., 3] << 24).astype(np.uint32)


def score_sprite_tex(batch, name, texkey, sampler, sx, **kw):
    t = load(batch)[name]
    col = arr(batch, name)
    tex = inputs(batch)[texkey]
    th, tw = tex.shape
    tot = ok = 0
    for i, pm in enumerate(t["meta"]["prims"]):
        p = [win16(t, v) for v in pm["v"]]
        m = sprite_mask(p, *col.shape[::-1])
        Y, X = np.nonzero(m)
        mode = "uv" if "uv" in pm["v"][0] else "st"
        u, v = sprite_uv(t, pm, X, Y, sx, mode, tw, th)
        pred = pack(sampler(tex, u, v, **kw))
        tot += len(X)
        ok += int((pred == col[Y, X]).sum())
    return ok, tot


def analyse_texture():
    res = {}
    I = inputs("texture")
    res["copy_1to1"] = {n: int((arr("texture", n) == I["tex_B"]).sum()) for n in ("copy_b_uv", "copy_b_st")}
    near = {}
    for n, key in (("mag_a_near_uv", "tex_A"), ("mag_a_near_st", "tex_A"), ("mag_a_near_frac", "tex_A"),
                   ("min_b_near", "tex_B")):
        near[n] = {f"sx={sx}": score_sprite_tex("texture", n, key, nearest, sx) for sx in (0.0, 0.5)}
    res["nearest"] = near
    bil = {}
    for n, key in (("mag_a_bil_uv", "tex_A"), ("mag_a_bil_st", "tex_A"), ("mag_a_bil_frac", "tex_A"),
                   ("min_b_bil", "tex_B")):
        r = {}
        for sx in (0.0, 0.5):
            for bits in (4, 8):
                for form in ("sum", "lerp-uv", "lerp-vu", "lerp-uv-div"):
                    r[f"sx={sx} bits={bits} {form}"] = score_sprite_tex("texture", n, key, bilinear, sx, bits=bits,
                                                                         form=form)
        bil[n] = r
    res["bilinear"] = bil
    return res


SECTIONS.update({"texture": analyse_texture})


BIL = dict(bits=4, form="lerp-uv")


def tfx_model(texel, vc, tfx, tcc):
    """GS User's Manual texture functions (per channel, 8-bit):
    MODULATE Cv = Ct*Cf >> 7, DECAL Cv = Ct, HIGHLIGHT Cv = (Ct*Cf >> 7) + Af,
    HIGHLIGHT2 as HIGHLIGHT; alpha: TCC 0 -> Af; TCC 1 -> MODULATE At*Af >> 7,
    DECAL At, HIGHLIGHT At + Af, HIGHLIGHT2 At.  Clamped to 255."""
    Ct, At = texel[..., :3], texel[..., 3]
    Cf, Af = np.array(vc[:3]), vc[3]
    if tfx == 0:
        C = (Ct * Cf) >> 7
        A = (At * Af) >> 7
    elif tfx == 1:
        C, A = Ct, At
    elif tfx == 2:
        C = ((Ct * Cf) >> 7) + Af
        A = At + Af
    else:
        C = ((Ct * Cf) >> 7) + Af
        A = At
    if not tcc:
        A = np.full_like(At, Af)
    out = np.concatenate([np.minimum(C, 255), np.minimum(A, 255)[..., None]], -1)
    return out


def analyse_texture2():
    res = {}
    T = load("texture")
    I = inputs("texture")
    # wrap modes (ST from -1 to 2 over a 64-px sprite on the 16x16 texture A)
    wr = {}
    for wname in ("repeat", "clamp", "region_clamp", "region_repeat"):
        for filt in ("near", "bil"):
            n = f"wrap_{wname}_{filt}"
            t = T[n]
            clamp = int(t["meta"]["tex"]["clamp"], 16)
            minu, maxu = clamp >> 4 & 0x3FF, clamp >> 14 & 0x3FF
            minv, maxv = clamp >> 24 & 0x3FF, clamp >> 34 & 0x3FF
            col = arr("texture", n)
            Y, X = np.mgrid[0:64, 0:64]
            u = (-1.0 + X * 3.0 / 64) * 16
            v = (-1.0 + Y * 3.0 / 64) * 16
            if wname in ("repeat", "clamp"):
                wu = wv = (wname,)
            else:
                wu, wv = (wname, minu, maxu), (wname, minv, maxv)

            def samp(tex, u, v, wu=wu, wv=wv):
                th, tw = tex.shape
                Tt = tex_channels(tex)
                if filt == "near":
                    iu = wrap_idx(np.floor(u).astype(np.int64), tw, *wu)
                    iv = wrap_idx(np.floor(v).astype(np.int64), th, *wv)
                    return Tt[iv, iu]
                one = 16
                U = np.floor((u - 0.5) * one).astype(np.int64)
                V = np.floor((v - 0.5) * one).astype(np.int64)
                fu, fv = (U & 15)[..., None], (V & 15)[..., None]
                i0, j0 = U >> 4, V >> 4
                a0 = Tt[wrap_idx(j0, th, *wv), wrap_idx(i0, tw, *wu)]
                a1 = Tt[wrap_idx(j0, th, *wv), wrap_idx(i0 + 1, tw, *wu)]
                b0 = Tt[wrap_idx(j0 + 1, th, *wv), wrap_idx(i0, tw, *wu)]
                b1 = Tt[wrap_idx(j0 + 1, th, *wv), wrap_idx(i0 + 1, tw, *wu)]
                a = a0 + (((a1 - a0) * fu) >> 4)
                b = b0 + (((b1 - b0) * fu) >> 4)
                return a + (((b - a) * fv) >> 4)
            pred = pack(samp(I["tex_A"], u, v))
            wr[n] = [int((pred == col).sum()), int(col.size)]
    res["wrap"] = wr
    # TFX x TCC (nearest, four vertex-colour bands, UV (0,0)-(16,4) per band)
    tf = {}
    for tfx in range(4):
        for tcc in (0, 1):
            n = f"tfx{tfx}_tcc{tcc}"
            t = T[n]
            col = arr("texture", n)
            ok = tot = 0
            for pm in t["meta"]["prims"]:
                p = [win16(t, v) for v in pm["v"]]
                m = sprite_mask(p, 64, 64)
                Y, X = np.nonzero(m)
                u, v = sprite_uv(t, pm, X, Y)
                texel = nearest(I["tex_A"], u, v)
                pred = pack(tfx_model(texel, pm["v"][1]["rgba"], tfx, tcc))
                ok += int((pred == col[Y, X]).sum())
                tot += len(X)
            tf[n] = [ok, tot]
    res["tfx"] = tf
    # CLUT index -> CLUT image position (identity-index textures, 1:1, DECAL)
    cm = {}
    for key, n, w, h in (("E", "clut_map_E", 16, 16), ("F", "clut_map_F", 8, 2)):
        col = arr("texture", n)[:16, :16].astype(np.int64)
        idx = I[f"tex_{key}_idx"].astype(np.int64)
        px, py = col & 0xFF, (col >> 8) & 0xFF
        pos = py * w + px
        ident = int((pos == idx).sum())
        swap = ((idx & ~0x18) | ((idx & 0x08) << 1) | ((idx & 0x10) >> 1))
        cm[n] = {"identity_row_major": ident, "bits3_4_swapped": int((pos == swap).sum()), "texels": int(idx.size),
                 "index_to_pos_first_32": pos.reshape(-1)[:32].tolist() if key == "E" else pos[0, :16].tolist()}
    col = arr("texture", "clut_csa")[:16, :16].astype(np.int64)
    idx = I["tex_F_idx"].astype(np.int64)
    pos = ((col >> 8) & 0xFF) * 16 + (col & 0xFF)
    cm["clut_csa1_T4"] = {"pos_of_index_0_15": pos[0, :16].tolist()}
    res["clut_map"] = cm
    # T8 / T4 through the measured CLUT map
    cl = {}
    for key, n in (("C", "clut_C_near"), ("C", "clut_C_bil"), ("D", "clut_D_near"), ("D", "clut_D_bil")):
        idx = I[f"tex_{key}_idx"].astype(np.int64)
        clut = I[f"tex_{key}_clut"].reshape(-1)
        if key == "C":
            posi = (idx & ~0x18) | ((idx & 0x08) << 1) | ((idx & 0x10) >> 1)
        else:
            posi = idx
        tex = clut[posi].astype(np.uint32)
        t = T[n]
        sampler = nearest if n.endswith("near") else (lambda tx, u, v: bilinear(tx, u, v, **BIL))
        cl[n] = score_sprite_tex_arr(t, arr("texture", n), tex, sampler)
    res["clut"] = cl
    return res


def score_sprite_tex_arr(t, col, tex, sampler, sx=0.0):
    th, tw = tex.shape
    ok = tot = 0
    for pm in t["meta"]["prims"]:
        p = [win16(t, v) for v in pm["v"]]
        m = sprite_mask(p, *col.shape[::-1])
        Y, X = np.nonzero(m)
        u, v = sprite_uv(t, pm, X, Y, sx, "uv" if "uv" in pm["v"][0] else "st", tw, th)
        pred = pack(sampler(tex, u, v))
        ok += int((pred == col[Y, X]).sum())
        tot += len(X)
    return [ok, tot]


SECTIONS.update({"texture2": analyse_texture2})


# ---------------------------------------------------------------------------
# pixel pipeline
def blend_model(src, dst, a, b, c, d, fix, colclamp=1, shift="floor"):
    """GS User's Manual: Cv = ((A - B) * C >> 7) + D per RGB channel; A/B/D
    select Cs, Cd or 0, C selects As, Ad or FIX.  COLCLAMP 1 clamps to
    0..255, COLCLAMP 0 keeps the low 8 bits."""
    S, D = split(src), split(dst)
    sel = lambda k, ch: S[ch] if k == 0 else D[ch] if k == 1 else np.zeros_like(S[ch])
    cval = S[3] if c == 0 else D[3] if c == 1 else np.full_like(S[3], fix)
    out = []
    for ch in range(3):
        prod = (sel(a, ch) - sel(b, ch)) * cval
        term = prod >> 7 if shift == "floor" else np.trunc(prod / 128).astype(np.int64)
        v = term + sel(d, ch)
        out.append(np.clip(v, 0, 255) if colclamp else v & 0xFF)
    out.append(S[3])
    return np.stack(out, -1)


def analyse_pixel():
    res = {}
    T = load("pixel")
    src = arr("pixel", "src_copy")
    dst = arr("pixel", "dest_only")
    res["dest_upload_exact"] = bool((dst == inputs("pixel")["dest"]).all())
    # blending: all 81 + presets
    bl = {}
    fails = {}
    for name, t in T.items():
        if "blend" not in t["meta"] or name == "src_copy":
            continue
        bm = t["meta"]["blend"]
        al = int(bm["alpha"], 16)
        a, b, c, d, fix = al & 3, al >> 2 & 3, al >> 4 & 3, al >> 6 & 3, al >> 32 & 0xFF
        col = arr("pixel", name)
        S = split(src)
        for shift in ("floor", "trunc"):
            if not bm["abe"]:
                pred = src.copy().astype(np.int64)
            else:
                pred = pack(blend_model(src, dst, a, b, c, d, fix, bm["colclamp"], shift)).astype(np.int64)
                if bm["pabe"]:
                    pred = np.where(S[3] & 0x80, pred, src)
            if bm["fba"]:
                pred = pred | 0x80000000
            if "fbmsk" in t["meta"]:
                mk = int(t["meta"]["fbmsk"], 16)
                pred = (pred & ~mk) | (dst.astype(np.int64) & mk)
            test = None
            for grp in t["items"]:
                for r, v in grp.get("ad", []):
                    if r == "TEST_1" and grp.get("role") != "reset":
                        test = int(v, 16)
            if test is not None:
                date, datm = test >> 14 & 1, test >> 15 & 1
                ate, atst, aref = test & 1, test >> 1 & 7, test >> 4 & 0xFF
                keep = np.zeros(src.shape, bool)
                if date:
                    keep |= ((dst >> 31) & 1) != datm
                if ate and atst == 6:
                    keep |= ~(S[3] > aref)
                pred = np.where(keep, dst, pred)
            n = int((pred.astype(np.uint32) == col).sum())
            bl.setdefault(name, {})[shift] = n
        if bl[name]["floor"] != col.size:
            fails[name] = bl[name]
    res["blend_pixels_per_test"] = int(src.size)
    res["blend_tests"] = len(bl)
    res["blend_exact_floor"] = sum(1 for v in bl.values() if v["floor"] == src.size)
    res["blend_exact_trunc"] = sum(1 for v in bl.values() if v["trunc"] == src.size)
    res["blend_not_exact_floor"] = fails
    # fog: candidate formulas over the recorded columns and the textured bands
    FOGS = {"(F*C + (255-F)*FOGCOL) >> 8": lambda f, c, k: (f * c + (255 - f) * k) >> 8,
            "(F*C + (256-F)*FOGCOL) >> 8": lambda f, c, k: (f * c + (256 - f) * k) >> 8,
            "FOGCOL + ((C - FOGCOL)*F >> 8)": lambda f, c, k: k + (((c - k) * f) >> 8),
            "(F*C + (255-F)*FOGCOL) / 255 (trunc)": lambda f, c, k: (f * c + (255 - f) * k) // 255}
    fg = {}
    I = inputs("pixel")
    for fname, ff in FOGS.items():
        t = T["fog_cols"]
        col = arr("pixel", "fog_cols")
        fc = np.array((0x30, 0x60, 0x90))
        ok = tot = 0
        for pm in t["meta"]["prims"]:
            p = [win16(t, v) for v in pm["v"]]
            m = sprite_mask(p, 64, 64)
            v1 = pm["v"][1]
            c = np.array(v1["rgba"][:3])
            pred = list(ff(v1["f"], c, fc)) + [v1["rgba"][3]]
            w = pred[0] | pred[1] << 8 | pred[2] << 16 | pred[3] << 24
            ok += int((col[m] == w).sum())
            tot += int(m.sum())
        t = T["fog_tex"]
        col = arr("pixel", "fog_tex")
        fc = np.array((0x55, 0xAA, 0x11))
        ok2 = tot2 = 0
        for pm in t["meta"]["prims"]:
            p = [win16(t, v) for v in pm["v"]]
            m = sprite_mask(p, 64, 64)
            Y, X = np.nonzero(m)
            u, v = sprite_uv(t, pm, X, Y)
            tx = tfx_model(nearest(I["tex_A"], u, v), pm["v"][1]["rgba"], 0, 1)
            fogged = np.concatenate([ff(pm["v"][1]["f"], tx[..., :3], fc), tx[..., 3:]], -1)
            ok2 += int((pack(fogged) == col[Y, X]).sum())
            tot2 += len(X)
        fg[fname] = {"fog_cols": [ok, tot], "fog_tex (texture function, then fog)": [ok2, tot2]}
    res["fog"] = fg
    # alpha test
    at = {}
    cmp = {"NEVER": lambda a, r: np.zeros_like(a, bool), "ALWAYS": lambda a, r: np.ones_like(a, bool),
           "LESS": lambda a, r: a < r, "LEQUAL": lambda a, r: a <= r, "EQUAL": lambda a, r: a == r,
           "GEQUAL": lambda a, r: a >= r, "GREATER": lambda a, r: a > r, "NOTEQUAL": lambda a, r: a != r}
    for mode, f in cmp.items():
        n = f"atest_{mode}"
        col = arr("pixel", n)
        z = arr("pixel", n, "z")
        ok = 0
        for band, aref in enumerate((0x40, 0x81)):
            for x in range(64):
                a = min(255, 4 * x + band)
                ps = bool(f(np.array(a), aref))
                cw = 0xC0 | 0x30 << 8 | 0x90 << 16 | a << 24
                cs = col[32 * band:32 * band + 32, x]
                zs = z[32 * band:32 * band + 32, x] & 0xFFFFFF
                if ps:
                    ok += int(((cs == cw) & (zs == 0x1000 + x)).sum())
                else:
                    ok += int(((cs == 0) & (zs == 0)).sum())
        at[n] = [ok, 4096]
    res["alpha_test"] = at
    af = {}
    for mode in ("KEEP", "FB_ONLY", "ZB_ONLY", "RGB_ONLY"):
        n = f"afail_{mode}"
        col = arr("pixel", n).astype(np.int64)
        z = arr("pixel", n, "z").astype(np.int64) & 0xFFFFFF
        ok = 0
        for x in range(64):
            a = 2 * x
            new = 0xE0 | 0x70 << 8 | 0x0F << 16 | a << 24
            old = 0x11223344
            if a < 0x40:
                ec, ez = new, 0xABCD
            elif mode == "KEEP":
                ec, ez = old, 0x777
            elif mode == "FB_ONLY":
                ec, ez = new, 0x777
            elif mode == "ZB_ONLY":
                ec, ez = old, 0xABCD
            else:
                ec, ez = (new & 0xFFFFFF) | (old & 0xFF000000), 0x777
            ok += int(((col[:, x] == ec) & (z[:, x] == ez)).sum())
        af[n] = [ok, 4096]
    res["alpha_fail"] = af
    # Z test (column Z = 0x800000 + x - 32 against a 0x800000 clear)
    zt = {}
    for mode in ("NEVER", "ALWAYS", "GEQUAL", "GREATER"):
        n = f"ztest_{mode}"
        col = arr("pixel", n)
        z = arr("pixel", n, "z").astype(np.int64) & 0xFFFFFF
        zc = 0x800000 + np.arange(64) - 32
        ps = {"NEVER": zc < 0, "ALWAYS": zc >= 0, "GEQUAL": zc >= 0x800000, "GREATER": zc > 0x800000}[mode]
        ec = np.where(ps, 0x40 | 0x80 << 8 | 0xC0 << 16 | 0x80 << 24, 0)
        ez = np.where(ps, zc, 0x800000)
        zt[n] = [int(((col == ec[None, :]) & (z == ez[None, :])).sum()), 4096]
    col = arr("pixel", "zmsk")
    z = arr("pixel", "zmsk", "z").astype(np.int64) & 0xFFFFFF
    zc = 0x800000 + np.arange(64) - 32
    ec = np.where(zc >= 0x800000, 0x40 | 0x80 << 8 | 0xC0 << 16 | 0x80 << 24, 0)
    zt["zmsk (GEQUAL, ZMSK 1)"] = [int(((col == ec[None, :]) & (z == 0x800000)).sum()), 4096]
    res["z_test"] = zt
    return res


SECTIONS.update({"pixel": analyse_pixel})


# ---------------------------------------------------------------------------
# dithering and 16-bit frames
def analyse_dither():
    from gs_conformance_suite import DIMX_ID, DIMX_STD
    res = {}
    pre = arr("frame", "dither_ct32_off_cc1").astype(np.int64)      # the same primitives, CT32, no dither
    res["ct32_dthe1_changes_nothing"] = {n: int((arr("frame", n) == pre).sum()) for n in
                                          ("dither_ct32_std_cc1", "dither_ct32_id_cc1")}
    R, G, B, A = split(pre)
    abit = (A >> 7) & 1
    res["ct16_no_dither (C >> 3, A bit 7)"] = {n: int((arr("frame", n).astype(np.int64) ==
                                                       ((R >> 3) | (G >> 3) << 5 | (B >> 3) << 10 | abit << 15)).sum())
                                               for n in ("dither_ct16_off_cc1", "dither_ct16_off_cc0")}
    al = arr("frame", "dither_ct16_alpha").astype(np.int64)
    res["ct16_alpha_bit_for_A_00_7F_80_FF"] = [int(al[0, 16 * k + 1] >> 15) for k in range(4)]
    Y, X = np.mgrid[0:64, 0:64]
    cands = {}
    for mname, m in (("std", DIMX_STD), ("id", DIMX_ID)):
        M = np.array(m)
        for cc in (1, 0):
            got = arr("frame", f"dither_ct16_{mname}_cc{cc}").astype(np.int64)
            for idx in ("[y&3][x&3]", "[x&3][y&3]"):
                D = M[Y & 3, X & 3] if idx == "[y&3][x&3]" else M[X & 3, Y & 3]
                ch = []
                for c in (R, G, B):
                    v = c + D
                    v = np.clip(v, 0, 255) if cc else v & 0xFF
                    ch.append(v >> 3)
                pred = ch[0] | ch[1] << 5 | ch[2] << 10 | abit << 15
                cands.setdefault(f"C + DIMX{idx}, {'clamp' if cc else 'wrap'} per COLCLAMP, >> 3", {})[
                    f"{mname}_cc{cc}"] = int((pred == got).sum())
    res["ct16_dither_candidates (of 4096)"] = cands
    g = Y >= 36
    res["frame-batch Gouraud rows only (y >= 36), DIMX[y&3][x&3]"] = {}
    for mname, m in (("std", DIMX_STD), ("id", DIMX_ID)):
        M = np.array(m)
        for cc in (1, 0):
            got = arr("frame", f"dither_ct16_{mname}_cc{cc}").astype(np.int64)
            D = M[Y & 3, X & 3]
            ch = [(np.clip(c + D, 0, 255) if cc else (c + D) & 0xFF) >> 3 for c in (R, G, B)]
            pred = ch[0] | ch[1] << 5 | ch[2] << 10 | abit << 15
            res["frame-batch Gouraud rows only (y >= 36), DIMX[y&3][x&3]"][f"{mname}_cc{cc}"] = [
                int((pred == got)[g].sum()), int(g.sum())]
    # which primitive kinds dither (probe2: CT16 vs the same primitives in CT32)
    t = load("probe2")["dither_kinds_ct16"]
    c16 = arr("probe2", "dither_kinds_ct16").astype(np.int64)
    c32 = arr("probe2", "dither_kinds_ct32").astype(np.int64)
    R2, G2, B2, A2 = split(c32)
    D = np.array(DIMX_ID)[Y & 3, X & 3]
    nod = (R2 >> 3) | (G2 >> 3) << 5 | (B2 >> 3) << 10 | ((A2 >> 7) & 1) << 15
    dth = ((np.clip(R2 + D, 0, 255) >> 3) | (np.clip(G2 + D, 0, 255) >> 3) << 5 | (np.clip(B2 + D, 0, 255) >> 3) << 10
           | ((A2 >> 7) & 1) << 15)
    kinds = []
    for pk, vs, i in expand_prims(t):
        p = [win16(t, v) for v in vs]
        m = {"tri": lambda: tri_mask(p, 64, 64), "sprite": lambda: sprite_mask(p, 64, 64),
             "line": lambda: line_mask_diamond(p, 64, 64, boundary="minor-negative")}[pk]()
        kinds.append({"kind": pk, "iip": t["meta"]["prims"][i]["flags"].get("iip", 0), "pixels": int(m.sum()),
                      "equal_dithered": int((c16[m] == dth[m]).sum()), "equal_undithered": int((c16[m] == nod[m]).sum())})
    res["dither_by_primitive_kind"] = kinds
    return res


SECTIONS.update({"dither": analyse_dither})


# ---------------------------------------------------------------------------
# best measured Gouraud model (not exact) and the composite checks
def gouraud_rowstep(p16, cv, X, Y, k=9, full=None):
    """Best model found (99.94 %, not exact): on every row the value at the
    row's first covered pixel is the exact barycentric value; each pixel to
    the right adds dC/dx truncated toward zero to 2^-k; the result is floored."""
    from fractions import Fraction as Fr
    (x0, y0), (x1, y1), (x2, y2) = p16
    A = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0)
    gxn = ((cv[1] - cv[0]) * (y2 - y0) - (cv[2] - cv[0]) * (y1 - y0)) * 16
    qg = Fr(int(Fr(gxn * (1 << k), A)), 1 << k)
    out = np.empty(len(X), np.int64)
    for yy in np.unique(Y):
        sel = np.nonzero(Y == yy)[0]
        xs0 = int(X[sel].min()) if full is None else int(np.nonzero(full[yy])[0].min())
        w0, w1, w2, Ar = bary(p16, np.array([xs0]), np.array([yy]))
        v0 = Fr(int(w0[0]) * cv[0] + int(w1[0]) * cv[1] + int(w2[0]) * cv[2], int(Ar))
        for j in sel:
            out[j] = (v0 + qg * (int(X[j]) - xs0)).__floor__()
    return out


def analyse_gouraud_model():
    cases = [("shade", n) for n in ["gouraud_big", "gouraud_small"] + [f"gouraud_rand_{i}" for i in range(8)]]
    cases += [("probe2", n) for n in [f"ramp_{a}_{k}" for k in range(5) for a in "xy"]
              + [f"gouraud_order_{k}_0" for k in range(2)]]
    cases += [("frame", n) for n in ("field_1936_0", "field_1936_5")]
    res = {}
    tot_ok = tot = 0
    for b, name in cases:
        t = load(b)[name]
        col = arr(b, name).astype(np.int64)
        h, w = col.shape
        claimed = np.zeros((h, w), bool)
        ok = n = 0
        for pk, vs, i in reversed(list(expand_prims(t))):      # the last primitive drawn wins
            p = [win16(t, v) for v in vs]
            full = tri_mask(p, w, h)
            m = full & ~claimed
            claimed |= m
            Y, X = np.nonzero(m)
            for c in range(4):
                cv = [v["rgba"][c] for v in vs]
                pred = gouraud_rowstep(p, cv, X, Y, full=full)
                got = (col[Y, X] >> (8 * c)) & 0xFF
                ok += int((pred == got).sum())
                n += len(got)
        # the field tests: uncovered pixels keep the clear word
        if name.startswith("field"):
            res[f"{b}/{name} uncovered == clear"] = bool((col[~claimed] == 0x80000000).all())
        res[f"{b}/{name}"] = [ok, n]
        tot_ok += ok
        tot += n
    res["total"] = [tot_ok, tot, round(100 * tot_ok / tot, 3)]
    return res


def analyse_xyoffset():
    """The same primitive list under XYOFFSET (1792, 1936), (1792, 1936.5) and
    (1792.5, 1936): coverage follows window = primitive - offset exactly (the
    tri / sprite / line sections include these tests); here: how the outputs
    relate to each other."""
    a = arr("frame", "xyofs_1936_0")
    bq = arr("frame", "xyofs_1936_5")
    c = arr("frame", "xyofs_x_1792_5")
    fa, fb = arr("frame", "field_1936_0"), arr("frame", "field_1936_5")
    return {"ofy_half_vs_whole_pixels_differing": int((a != bq).sum()),
            "ofy_half_equals_whole_shifted_up_one_row": int((bq[:-1] == a[1:]).sum()),
            "ofx_half_vs_whole_pixels_differing": int((a != c).sum()),
            "field_512x224_differing_pixels": int((fa != fb).sum()),
            "note": "a half-pixel offset is not a whole-row shift: coverage is resampled at the new "
                    "window positions (see the tri/sprite/line sections: 0 mismatches with window = "
                    "vertex - XYOFFSET, pixel sampled at integer coordinates)"}


SECTIONS.update({"gouraud_model": analyse_gouraud_model, "xyoffset": analyse_xyoffset})


def analyse_composite():
    """Triangle texturing (exact perspective-correct S/Q, T/Q at the integer
    pixel sample) and the game's level class composed from the measured
    rules: coverage, STQ, bilinear, CLUT, MODULATE, fog, alpha test, Z."""
    res = {}
    T = load("texture")
    I = inputs("texture")
    tex_a = I["tex_A"]
    for n in ("persp_0", "persp_1", "persp_2", "persp_3"):
        t = T[n]
        col = arr("texture", n)
        filt = int(t["meta"]["tex"]["mmag"])
        claimed = np.zeros((64, 64), bool)
        ok = tot = 0
        for pk, vs, i in reversed(list(expand_prims(t))):
            p = [win16(t, v) for v in vs]
            m = tri_mask(p, 64, 64) & ~claimed
            claimed |= m
            Y, X = np.nonzero(m)
            w0, w1, w2, Ar = bary(p, X, Y)
            S = (w0 * vs[0]["st"][0] + w1 * vs[1]["st"][0] + w2 * vs[2]["st"][0]) / Ar
            Tt = (w0 * vs[0]["st"][1] + w1 * vs[1]["st"][1] + w2 * vs[2]["st"][1]) / Ar
            Q = (w0 * vs[0]["qv"] + w1 * vs[1]["qv"] + w2 * vs[2]["qv"]) / Ar
            u, v = S / Q * 16, Tt / Q * 16
            pred = pack(bilinear(tex_a, u, v, **BIL)) if filt else pack(nearest(tex_a, u, v))
            ok += int((pred == col[Y, X]).sum())
            tot += len(X)
        res[n] = [ok, tot]
    # level class
    t = T["level_class"]
    col = arr("texture", "level_class").astype(np.int64)
    z = arr("texture", "level_class", "z").astype(np.int64) & 0xFFFFFF
    idx = I["tex_C_idx"].astype(np.int64)
    posi = (idx & ~0x18) | ((idx & 0x08) << 1) | ((idx & 0x10) >> 1)
    tex = I["tex_C_clut"].reshape(-1)[posi].astype(np.uint32)
    fc = np.array((0x30, 0x40, 0x58))
    claimed = np.zeros((64, 64), bool)
    ok = tot = 0
    written = np.zeros((64, 64), bool)
    pred_all = np.zeros((64, 64), np.int64)
    for pk, vs, i in reversed(list(expand_prims(t))):
        p = [win16(t, v) for v in vs]
        full = tri_mask(p, 64, 64)
        m = full & ~claimed
        Y, X = np.nonzero(m)
        w0, w1, w2, Ar = bary(p, X, Y)
        S = (w0 * vs[0]["st"][0] + w1 * vs[1]["st"][0] + w2 * vs[2]["st"][0]) / Ar
        Tt = (w0 * vs[0]["st"][1] + w1 * vs[1]["st"][1] + w2 * vs[2]["st"][1]) / Ar
        Q = (w0 * vs[0]["qv"] + w1 * vs[1]["qv"] + w2 * vs[2]["qv"]) / Ar
        texel = bilinear(tex, S / Q * 16, Tt / Q * 16, **BIL)
        vcol = np.stack([gouraud_rowstep(p, [v["rgba"][c] for v in vs], X, Y, full=full) for c in range(4)], -1)
        F = gouraud_rowstep(p, [v["f"] for v in vs], X, Y, full=full)
        Ct = texel[:, :3]
        C = np.minimum((Ct * vcol[:, :3]) >> 7, 255)
        A = np.minimum((texel[:, 3] * vcol[:, 3]) >> 7, 255)
        C = fc + (((C - fc) * F[:, None]) >> 8)
        passed = A > 0                                   # TEST 0x5000D: ATST GREATER, AREF 0, AFAIL KEEP
        claimed |= m & np.zeros_like(m)                  # later (reversed) primitives overwrite only where they pass
        for j in range(len(X)):
            if passed[j] and not written[Y[j], X[j]]:
                written[Y[j], X[j]] = True
                pred_all[Y[j], X[j]] = C[j, 0] | C[j, 1] << 8 | C[j, 2] << 16 | A[j] << 24
    ok = int((col[written] == pred_all[written]).sum())
    res["level_class (written pixels)"] = [ok, int(written.sum())]
    res["level_class unwritten pixels keep the clear"] = bool((col[~written] == 0).all())
    return res


SECTIONS.update({"composite": analyse_composite})


def main(argv):
    names = argv or list(SECTIONS)
    out = {}
    path = CAP / "analysis.json"
    if path.exists():
        out = json.loads(path.read_text())
    for n in names:
        out[n] = SECTIONS[n]()
        print(n, json.dumps(out[n])[:1500])
    path.write_text(json.dumps(out, indent=1) + "\n")


if __name__ == "__main__":
    main(sys.argv[1:])
