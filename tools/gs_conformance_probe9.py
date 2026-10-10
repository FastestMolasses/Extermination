#!/usr/bin/env python3
"""gs_conformance_probe9.py - the half-pixel scissor edge: lines and points
that start within half a pixel outside the scissor.

Same harness, rules and clean room as tools/gs_conformance.py (docs/
GS_CONFORMANCE.md): designed GIF packets drawn by PCSX2's SOFTWARE renderer,
recorded pixel for pixel; every test resets the whole GS state and clears its
own buffers; nothing disc-derived; no emulator source read.

Why (docs/PCSX2_FORK_GS_DIFF.md 3.3): PCSX2 v2.6.3 and the agent-debug fork
(v2.9.114) write identical GS memory for identical input except for one
upstream change (4fa2b8e45, 2026-05-07): the rectangle used to reject
primitives lying entirely outside the scissor grew by half a pixel on every
side.  A primitive whose bounding box starts less than half a pixel past the
scissor, and whose rasterization still lights an in-scissor pixel, was
dropped whole by v2.6.3 and is drawn by the fork.  The game hits it in the
first play segment (alpha-blended vertical line strips with a vertex at y
223.25: the fork lights row 223).  The user's decision (2026-10-09): follow
the fork.  No earlier conformance test placed a primitive there; this set
does, so the port can test the rule.

Design.  Every edge test owns a 128 x 128 CT32 buffer with an INNER scissor
x 32..95, y 32..95, so the pixels just outside the scissor are inside the
buffer and must stay clear (the per-pixel scissor still applies).  Coverage
mode (gs_conformance_suite.cov_setup: additive blend, one colour bit per
primitive), so each pixel says which primitives lit it.
- p9_edges: for each edge (bottom, top, right, left) and each distance d of
  the primitive's first vertex past the edge's last pixel row/column (0,
  1/16, 2/16, 4/16, 6/16, 7/16, 8/16, 9/16, 12/16, 1), seven primitives at
  separate positions along the edge: 0 a line perpendicular to the edge
  going outward (first vertex at d), 1 the same line reversed (last vertex
  at d), 2 a steep diagonal going outward, 3 a shallow diagonal going outward,
  4 a line parallel to the edge at d, 5 a point at d (integer along the
  edge), 6 a point at d (half-integer along the edge).
  Expected rule (fork): with d <= 1/2 a primitive is not rejected as a whole;
  the in-scissor pixels its rasterization lights are drawn.  v2.6.3 dropped
  every primitive whose bounding box lay beyond the edge (d > 0).
- p9_game: the game's case at the field's real edge: a 512 x 224 buffer with
  the full scissor (0..511, 0..223), five alpha-blended Gouraud line strips
  at x 427.9375 + 2k with vertices at y 214, 223.25 and 231.5625 (the shape
  seen in the first play segment); the same with the outside segment as a
  separate line; the same five strips with the vertex at 223.5 and 223.5625
  (inside / just outside the half-pixel band); and controls: a triangle and a
  sprite starting at d = 1/4 past the inner-scissor bottom (they cover no
  in-scissor pixel centre either way).

Usage (macOS arm64, decomp root; the fork by default, software renderer):
  .venv/bin/python tools/gs_conformance_probe9.py list | capture [B..] | decode [B..]
  .venv/bin/python tools/gs_conformance_probe9.py report [--legacy DIR]
The fork pixel reference set (tools/fork_pixel_refs.py gscap gscap9):
  GSCAP_OUT=build/fork_refs/pixels/b16/gscap9
Outputs (ignored): <GSCAP_OUT>/<batch>/ (batch.json, packet.bin, kick.json,
snap/gs.bin, <test>.npz, <test>.png, decode.json) and report.json.
"""
from __future__ import annotations

import json
import os
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
os.environ.setdefault("GSCAP_OUT", str(ROOT / "build/b16/gscap9"))

import gs_conformance_probe3 as P3  # noqa: E402
from gs_conformance import ALPHA, SCISSOR, TEST, Batch, Test  # noqa: E402
from gs_conformance_suite import cov_setup, id_rgba, prim  # noqa: E402

OFS = (1024.0, 1024.0)
SC = (32, 95, 32, 95)                     # inner scissor x0, x1, y0, y1 of the edge tests
DISTANCES = [0, 1, 2, 4, 6, 7, 8, 9, 12, 16]   # sixteenths of a pixel past the edge
EDGES = ["bottom", "top", "right", "left"]
KINDS = ["perp_out", "perp_in", "steep_out", "shallow_out", "parallel", "point", "point_half"]


def edge_prims(edge: str, d16: int) -> list[dict]:
    """The seven primitives of one edge test, in buffer coordinates.  `u` runs
    along the edge (8 px apart, from 36), `n` is the distance outward from the
    edge's last in-scissor pixel row or column."""
    d = d16 / 16
    out = []
    for k, kind in enumerate(KINDS):
        u = 36 + 8 * k
        if kind == "perp_out":
            seg = [(u, d), (u, d + 8)]
        elif kind == "perp_in":
            seg = [(u, d + 8), (u, d)]
        elif kind == "steep_out":
            seg = [(u, d), (u + 3, d + 8)]
        elif kind == "shallow_out":
            seg = [(u, d), (u + 6, d + 2)]
        elif kind == "parallel":
            seg = [(u - 2, d), (u + 4, d)]
        elif kind == "point":
            seg = [(u, d)]
        else:
            seg = [(u + 0.5, d)]
        pts = []
        for uu, n in seg:
            if edge == "bottom":
                pts.append((uu, SC[3] + n))
            elif edge == "top":
                pts.append((uu, SC[2] - n))
            elif edge == "right":
                pts.append((SC[1] + n, uu))
            else:
                pts.append((SC[0] - n, uu))
        out.append({"id": k, "kind": "point" if len(pts) == 1 else "line", "role": kind, "pts": pts,
                    "first_vertex_past_edge": d, "bbox_past_edge": d})
    return out


def batch_edges() -> Batch:
    b = Batch("p9_edges", "lines and points starting 0..1 pixel past each inner-scissor edge (coverage ids)")
    for edge in EDGES:
        for d16 in DISTANCES:
            t = b.add(Test(f"{edge}_{d16:02d}", f"{edge} edge, first vertex {d16}/16 px past scissor "
                           f"{SC}; 7 primitives (coverage ids)", 128, 128, ofs=OFS))
            cov_setup(t)
            t.ad(("SCISSOR_1", SCISSOR(*SC)))
            ps = edge_prims(edge, d16)
            for p in ps:
                prim(t, p["kind"], [dict(x=x, y=y, rgba=id_rgba(p["id"])) for x, y in p["pts"]], abe=1)
            t.meta.update(edge=edge, d16=d16, scissor=list(SC), prims_scissor=ps)
    return b


def batch_game() -> Batch:
    b = Batch("p9_game", "the game's case at the field edge (blended Gouraud line strips) and controls")
    blend = ALPHA(0, 1, 0, 1)               # (Cs - Cd) * As >> 7 + Cd
    for name, ys, split in (("strips_22325", (214.0, 223.25, 231.5625), False),
                            ("lines_22325", (214.0, 223.25, 231.5625), True),
                            ("strips_22350", (214.0, 223.5, 231.5625), False),
                            ("strips_22356", (214.0, 223.5625, 231.5625), False)):
        t = b.add(Test(name, f"512x224, full scissor; 5 blended Gouraud {'lines' if split else 'line strips'} "
                       f"x 427.9375 + 2k, y {ys}", 512, 224, clear=0x80302010, ofs=OFS))
        t.ad(("ALPHA_1", blend), ("TEST_1", TEST()))
        strips = []
        for k in range(5):
            x = 427.9375 + 2 * k
            vs = [dict(x=x, y=ys[0], rgba=(200, 40 * k, 30, 0x60)),
                  dict(x=x + 0.0625 * k, y=ys[1], rgba=(255, 255, 255, 0x80)),
                  dict(x=x + 0.5, y=ys[2], rgba=(90, 200, 255, 0x40))]
            if split:
                prim(t, "line", vs[:2], iip=1, abe=1)
                prim(t, "line", vs[1:], iip=1, abe=1)
            else:
                prim(t, "linestrip", vs, iip=1, abe=1)
            strips.append([[v["x"], v["y"]] for v in vs])
        t.meta.update(scissor=[0, 511, 0, 223], strips=strips, split=split, rows_past_edge=ys[1] - 223)
    t = b.add(Test("controls_fill", "triangle and sprite starting 1/4 px past the inner-scissor bottom and right "
                   f"(scissor {SC}); coverage ids", 128, 128, ofs=OFS))
    cov_setup(t)
    t.ad(("SCISSOR_1", SCISSOR(*SC)))
    ctl = [("tri", [(40, 95.25), (60, 95.25), (50, 110)]), ("sprite", [(64, 95.25), (80, 104)]),
           ("tri", [(95.25, 40), (95.25, 60), (110, 50)]), ("sprite", [(95.25, 64), (104, 80)]),
           ("tri", [(40, 95.0), (60, 95.0), (50, 110)]), ("sprite", [(64, 95.0), (80, 104)])]
    for k, (kind, pts) in enumerate(ctl):
        prim(t, kind, [dict(x=x, y=y, rgba=id_rgba(k)) for x, y in pts], abe=1)
    t.meta.update(scissor=list(SC), controls=[{"id": k, "kind": kd, "pts": p} for k, (kd, p) in enumerate(ctl)])
    return b


def batches():
    return [batch_edges(), batch_game()]


# ---------------------------------------------------------------------------
# report (no emulator): which primitives lit which pixels

def _in_scissor(sc, h: int, w: int) -> np.ndarray:
    m = np.zeros((h, w), bool)
    m[sc[2]:sc[3] + 1, sc[0]:sc[1] + 1] = True
    return m


def report(out_dir: Path, legacy: Path | None) -> dict:
    res: dict = {"set": str(out_dir), "legacy": str(legacy) if legacy else None, "batches": {}}
    for bname in ("p9_edges", "p9_game"):
        bd = out_dir / bname
        if not (bd / "decode.json").exists():
            continue
        doc = json.loads((bd / "batch.json").read_text())
        dec = json.loads((bd / "decode.json").read_text())
        tests = {}
        for t in doc["tests"]:
            c = np.load(bd / f"{t['name']}.npz")["color"]
            meta = t["meta"]
            sc = meta.get("scissor")
            ent: dict = {}
            if sc:
                ins = _in_scissor(sc, t["h"], t["w"])
                cleared = int(t["clear"], 16)
                ent["pixels_outside_scissor_changed"] = int(((c != cleared) & ~ins).sum())
            if "prims_scissor" in meta or "controls" in meta:
                prims = meta.get("prims_scissor") or meta.get("controls")
                lit = {}
                for p in prims:
                    bit = 1 << p["id"]
                    ys, xs = np.nonzero((c & 0xFFFFFF) & bit)
                    lit[p.get("role", p["kind"]) + f"#{p['id']}"] = [[int(x), int(y)] for y, x in zip(ys, xs)]
                ent["lit_pixels"] = lit
            else:
                diff = c != int(t["clear"], 16)
                ys, xs = np.nonzero(diff)
                ent["pixels_drawn"] = int(diff.sum())
                ent["row_223_pixels"] = [[int(x), hex(int(c[223, x]))] for x in np.nonzero(diff[223])[0]] \
                    if t["h"] > 223 else None
            if legacy is not None and (legacy / bname / f"{t['name']}.npz").exists():
                lc = np.load(legacy / bname / f"{t['name']}.npz")["color"]
                dm = lc != c
                ent["v263"] = {"words_differ": int(dm.sum()),
                               "pixels": [[int(x), int(y), hex(int(lc[y, x])), hex(int(c[y, x]))]
                                          for y, x in zip(*np.nonzero(dm))][:64]}
            tests[t["name"]] = ent
        res["batches"][bname] = {"fence_ok": dec.get("fence_ok"), "tests": tests}
    if legacy is not None:
        res["v263_summary"] = {b: {"tests": len(v["tests"]),
                                   "tests_differ": sorted(n for n, e in v["tests"].items()
                                                          if e.get("v263", {}).get("words_differ"))}
                               for b, v in res["batches"].items()}
    (out_dir / "report.json").write_text(json.dumps(res, indent=1) + "\n")
    return res


def main() -> None:
    if len(sys.argv) > 1 and sys.argv[1] == "report":
        import argparse
        ap = argparse.ArgumentParser()
        ap.add_argument("cmd")
        ap.add_argument("--legacy", type=Path, default=None, help="a v2.6.3 capture of this set to compare")
        a = ap.parse_args()
        r = report(Path(os.environ["GSCAP_OUT"]), a.legacy)
        for b, v in r["batches"].items():
            print(b, "fence_ok", v["fence_ok"], "tests", len(v["tests"]))
        if "v263_summary" in r:
            print(json.dumps(r["v263_summary"], indent=1))
        return
    P3.batches = batches
    P3.OUT = Path(os.environ["GSCAP_OUT"])
    P3.main()


if __name__ == "__main__":
    main()
