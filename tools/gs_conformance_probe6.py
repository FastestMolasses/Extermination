#!/usr/bin/env python3
"""gs_conformance_probe6.py - fourth follow-up batch for the port's CPU GS
model (docs/GS_EXACT.md in the port): coverage on edges through pixel
centres, the texture functions with interpolated colour, and Z row starts
of apex-top triangles.

Same harness, rules and clean room as tools/gs_conformance.py (docs/
GS_CONFORMANCE.md). Nothing here is disc-derived; no emulator source read.

- p6_cov: one triangle per id (the additive one-bit id colours of the
  coverage tests), each with one test edge between integer points whose
  slope p/q is not dyadic, so the edge passes exactly through a pixel
  centre every q rows. The test edge is the triangle's left or right edge,
  listed in either direction and from either vertex slot. Which on-edge
  pixels each triangle keeps shows the edge arithmetic.
- p6_tfx: MODULATE / DECAL / HIGHLIGHT / HIGHLIGHT2 x TCC 0 / 1 over
  constant textures with Gouraud colour and alpha on exact k/512 planes
  (p3_start's geometry), with and without fog: which precision the
  texture function's A term and the fog see.
- p6_z: Z24 with dZ/dx = 1/512 exactly on apex-top and apex-bottom
  triangles (p4_z's u2 case) in all three vertex slots.

Usage (decomp root; Renderer = 13 manual switch, docs/GS_CONFORMANCE.md 2):
  .venv/bin/python tools/gs_conformance_probe6.py list | capture [B..] | decode [B..]
Outputs (ignored): build/b16/gscap6/<batch>/.
"""
from __future__ import annotations

import itertools
import os
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
os.environ.setdefault("GSCAP_OUT", str(ROOT / "build/b16/gscap6"))

import gs_conformance_probe3 as P3  # noqa: E402
from gs_conformance import CLAMP, FOGCOL, TEST, TEX0, TEX1, Batch, Test  # noqa: E402
from gs_conformance_suite import ADD, id_rgba, prim  # noqa: E402

OFS = (1024.0, 1024.0)


def batch_cov() -> Batch:
    b = Batch("p6_cov", "coverage on edges through pixel centres: slopes, sides, directions, vertex slots")
    slopes = [(1, 3), (2, 3), (1, 5), (2, 5), (3, 5), (4, 5), (1, 7), (2, 7), (3, 7), (5, 7), (6, 7), (1, 9),
              (2, 9), (4, 9), (5, 9), (7, 9), (1, 6), (5, 6), (3, 11), (7, 12), (0, 1), (1, 1), (1, 2), (3, 4)]
    n = 0
    for side, sgn, order in itertools.product(("left", "right"), (1, -1), range(3)):
        t = b.add(Test(f"cov_{side}_{'p' if sgn > 0 else 'n'}_{order}",
                       f"test edge as the {side} edge, slope sign {sgn}, vertex order {order}", 256, 64, ofs=OFS))
        t.ad(("ALPHA_1", ADD), ("TEST_1", TEST()), ("COLCLAMP", 1))
        t.meta["mode"] = "coverage_ids"
        tris = []
        for k, (dx, dy) in enumerate(slopes):
            col = k % 12
            x0 = 8 + 20 * col + (0 if sgn > 0 else 12)
            y0 = -1 + 34 * (k // 12)
            top = (x0, y0)
            bot = (x0 + sgn * dx * 3, y0 + dy * 3 if dy else y0 + 3)
            if dy == 0:
                bot = (x0, y0 + 30)
            if (dx, dy) == (0, 1):
                bot = (x0, y0 + 30)
            else:
                m = max(1, 30 // (dy * 3)) if dy else 1
                bot = (x0 + sgn * dx * 3 * m, y0 + dy * 3 * m)
            third = (min(top[0], bot[0]) - 9, (top[1] + bot[1]) / 2) if side == "right" else \
                    (max(top[0], bot[0]) + 9, (top[1] + bot[1]) / 2)
            vs = [top, bot, third]
            if order == 1:
                vs = [bot, third, top]
            elif order == 2:
                vs = [third, bot, top]
            tris.append(vs)
            prim(t, "tri", [dict(x=float(x), y=float(y), rgba=id_rgba(k)) for x, y in vs], abe=1)
        t.meta["edges"] = [{"top": list(map(float, tr[0])), "slope": list(s)} for tr, s in zip(tris, slopes)]
        n += 1
    return b


def batch_tfx() -> Batch:
    b = Batch("p6_tfx", "texture functions with Gouraud colour and alpha on exact planes, and fog")
    up = b.add(Test("upload", "constant textures (draws nothing)", 64, 32))
    blks = {}
    for name, word in (("t40", 0x60FF4020), ("tff", 0xFFFFFFFF), ("t80", 0x80808080)):
        blks[name] = b.tex_alloc(1)
        up.image(blks[name], 64, "CT32", 16, 16, np.full(256, word, dtype="<u4").tobytes(), label=name)
    for tfx, tcc, tex, fog in itertools.product(range(4), (0, 1), ("t40", "tff"), (0, 1)):
        if fog and tex == "t40":
            continue
        t = b.add(Test(f"tfx{tfx}_tcc{tcc}_{tex}_f{fog}", f"TFX {tfx} TCC {tcc} over {tex}, Gouraud RGBA planes"
                       + (", fog plane" if fog else ""), 512, 32, ofs=OFS))
        t.ad(("TEX0_1", TEX0(blks[tex], 64, "CT32", 4, 4, tcc, tfx)), ("TEX1_1", TEX1()), ("CLAMP_1", CLAMP(1, 1)))
        if fog:
            t.ad(("FOGCOL", FOGCOL(0x33, 0x99, 0x10)))
        key = ["v1000", "s1", "c1"][(tfx + 2 * tcc) % 3]
        pts = P3.geom_pts(key)
        cols = P3.exact_plane((40, 90, 20, 30), P3.DXK, None, (7, -9, 11, 50))
        fs = [60, 62, 160]
        vs = [dict(x=x, y=y, rgba=tuple(c), uv=(8.0, 8.0), f=fv) for (x, y), c, fv in zip(pts, cols, fs)]
        prim(t, "tri", vs, iip=1, tme=1, fst=1, fge=fog)
        t.meta["geom"] = key
    return b


def batch_z() -> Batch:
    b = Batch("p6_z", "Z24 starts of apex triangles, dZ/dx = 1/512 exactly, all vertex slots")
    apexes = [((250.5, -3.5), (100.75, 60.25)), ((30.3125, -10.0), (-5.0, 40.0)), ((120.0, 45.0), (50.0, -20.0)),
              ((400.25, 60.0), (-100.0, -5.5)), ((200.0, -8.0), (180.0, 36.0)), ((10.0, -2.0), (-300.0, 50.0))]
    for k, (apex, a) in enumerate(apexes):
        bpt = (a[0] + 1024, a[1])
        za = 0x7FF000
        zs = {"A": za, "B": za + 2, "C": za - 77777 + 911 * k}
        pts = {"A": a, "B": bpt, "C": apex}
        for order in ("ABC", "CAB", "BCA", "ACB"):
            t = b.add(Test(f"z_apex{k}_{order}", f"apex {apex}, base from {a}, vertex order {order}", 512, 32,
                           zpsm="Z24", ofs=OFS))
            prim(t, "tri", [dict(x=pts[c][0], y=pts[c][1], z=zs[c], rgba=(9, 9, 9, 0x80)) for c in order], iip=1)
            t.meta.update(order=order, apex=list(apex))
    return b


def batches():
    return [batch_cov(), batch_tfx(), batch_z()]


def main() -> None:
    P3.batches = batches
    P3.OUT = Path(os.environ["GSCAP_OUT"])

    def save_inputs(b, out, pkt, doc):
        import json
        (out / "packet.bin").write_bytes(pkt)
        (out / "batch.json").write_text(json.dumps(doc, indent=1) + "\n")
    P3.save_inputs = save_inputs
    P3.main()


if __name__ == "__main__":
    main()
