#!/usr/bin/env python3
"""gs_conformance_probe4.py - second follow-up batch for the port's CPU GS
model (docs/GS_EXACT.md in the port): the perspective divide, Z row starts,
Gouraud lines, UV triangles and coverage on non-dyadic edges.

Same harness, rules and clean room as tools/gs_conformance.py and
tools/gs_conformance_probe3.py (docs/GS_CONFORMANCE.md). Nothing here is
disc-derived; no emulator source was read.

Design:
- p4_rcp: the texel coordinate of a constant-STQ sprite or triangle is
  S/Q scaled by the texture width. A 1024 x 1 CT32 texture whose texel i
  holds (R, G) = (i & 255, i >> 8), nearest, CLAMP, DECAL, turns each
  2 x 2 sprite into one reading of floor(u * 1024). For every Q in a sweep
  the sprites put u * 1024 at m + d for d from 0.002 to 0.998, so the
  largest d that still reads m - 1 bounds the divide's error at that Q.
  Bilinear sprites and constant-STQ triangles are the path checks.
- p4_z: Z with dZ/dx = 2 per 1024 pixels exactly (as p3_start does for
  colour), so each 512-pixel row shows its start's fraction; vertical,
  sloped and clipped left edges; Z24 and Z32; small and large values.
- p4_misc: Gouraud lines with dC/d(major) = k/512 exactly; UV triangles
  with slow U gradients; triangles whose colour plane is k/128 per pixel
  (every row start an exact 1/128 value); coverage count tests on edges
  with slopes 1/3, 1/5, 1/7, 2/7, 3/7, 5/9 through integer points.

Usage (decomp root; Renderer = 13 manual switch, docs/GS_CONFORMANCE.md 2):
  .venv/bin/python tools/gs_conformance_probe4.py list | capture [B..] | decode [B..]
Outputs (ignored): build/b16/gscap4/<batch>/.
"""
from __future__ import annotations

import os
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
os.environ.setdefault("GSCAP_OUT", str(ROOT / "build/b16/gscap4"))

import gs_conformance_probe3 as P3  # noqa: E402
from gs_conformance import ALPHA, CLAMP, TEST, TEX0, TEX1, Batch, Test  # noqa: E402
from gs_conformance_suite import ADD, id_rgba, prim  # noqa: E402

OFS = (1024.0, 1024.0)


def f32(v: float) -> float:
    return float(np.float32(v))


def batch_rcp() -> Batch:
    b = Batch("p4_rcp", "the perspective divide: constant-STQ sprites over a 1024-texel address texture")
    up = b.add(Test("upload", "1024 x 1 address texture (draws nothing)", 64, 32))
    i = np.arange(1024, dtype=np.uint32)
    tex = (i & 255 | (i >> 8) << 8 | 0x55 << 16 | 0x80 << 24).astype("<u4")
    blk = b.tex_alloc(16)
    up.image(blk, 1024, "CT32", 1024, 1, tex.tobytes(), label="address1024")
    b.address = tex
    tex0 = TEX0(blk, 1024, "CT32", 10, 0, 1, 1)                  # 1024 x 1, DECAL, TCC 1
    rng = np.random.RandomState(4401)
    ds = [0.002, 0.004, 0.007, 0.01, 0.015, 0.02, 0.03, 0.05, 0.08, 0.12, 0.2, 0.5, 0.8, 0.95, 0.99, 0.997]
    # Q sweep: 160 mantissas in [1, 2) plus 8 other binades, 16 d each
    qs = [1.0 + k / 160 for k in range(160)] + [0.25, 0.3, 0.55, 0.77, 2.5, 3.3, 5.0, 7.7]
    per_test = 128 * 16                                          # 2 x 2 sprites filling a 256 x 32 buffer
    cells = [(q, d) for q in qs for d in ds]
    for n in range(0, len(cells), per_test):
        t = b.add(Test(f"rcp_{n // per_test}", "constant-STQ 2x2 sprites, nearest", 256, 32, ofs=OFS))
        t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1()), ("CLAMP_1", CLAMP(1, 1)))
        samples = []
        for c, (q, d) in enumerate(cells[n:n + per_test]):
            m = int(rng.randint(64, 960))
            qf = f32(q)
            s = f32(qf * (m + d) / 1024)
            x, y = 2 * (c % 128), 2 * (c // 128)
            prim(t, "sprite", [dict(x=x, y=y, rgba=(0x80,) * 4, qv=qf, st=(s, 0.0)),
                               dict(x=x + 2, y=y + 2, rgba=(0x80,) * 4, qv=qf, st=(s, 0.0))], tme=1)
            samples.append([qf, s, m, d])
        t.meta["samples"] = samples
    # bilinear and triangle path checks
    for filt, kind in ((1, "sprite"), (0, "tri"), (1, "tri")):
        t = b.add(Test(f"rcp_{kind}_{'bil' if filt else 'near'}", f"constant-STQ {kind}s", 256, 32, ofs=OFS))
        t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=filt, mmin=filt)), ("CLAMP_1", CLAMP(1, 1)))
        samples = []
        for c in range(256 if kind == "sprite" else 64):
            q = f32(1.0 + rng.uniform(0, 1)) if c % 3 else f32(rng.uniform(0.3, 6.0))
            m = int(rng.randint(64, 960)); d = float(rng.choice(ds))
            s = f32(q * (m + d) / 1024)
            if kind == "sprite":
                x, y = 4 * (c % 64), 4 * (c // 64) * 2
                prim(t, "sprite", [dict(x=x, y=y, rgba=(0x80,) * 4, qv=q, st=(s, 0.0)),
                                   dict(x=x + 4, y=y + 4, rgba=(0x80,) * 4, qv=q, st=(s, 0.0))], tme=1)
            else:
                x, y = 8 * (c % 32), 16 * (c // 32)
                vs = [dict(x=x, y=y, rgba=(0x80,) * 4, qv=q, st=(s, 0.0)),
                      dict(x=x + 8, y=y, rgba=(0x80,) * 4, qv=q, st=(s, 0.0)),
                      dict(x=x, y=y + 16, rgba=(0x80,) * 4, qv=q, st=(s, 0.0))]
                prim(t, "tri", vs, tme=1)
            samples.append([q, s, m, d])
        t.meta["samples"] = samples
    return b


def batch_z() -> Batch:
    b = Batch("p4_z", "Z row starts: dZ/dx = 1/512 exactly over 512-pixel rows (Z24, Z32)")
    geoms = ["v1000", "v64", "v333", "s1", "s2", "s4", "c1", "u2"]
    sets = [("Z24", 0x000100, 5000), ("Z24", 0x7FF000, -77777), ("Z24", 0xFFF000, -300001),
            ("Z32", 0x00001000, 99999), ("Z32", 0x80000000, -12345678), ("Z32", 0xFFFF0000, -987654321)]
    for zpsm, za, dzc in sets:
        for key in (geoms if zpsm == "Z24" and za == 0x7FF000 else geoms[:4] if zpsm == "Z24" else ["v1000", "s1"]):
            t = b.add(Test(f"{zpsm.lower()}_{za:x}_{key}", f"{zpsm} Z {za:#x}, dZ/dx 1/512, geometry {key}",
                           512, 32, zpsm=zpsm, ofs=OFS))
            zs = [za, za + 2, za + dzc]
            P3.tri(t, P3.geom_pts(key), [(9, 9, 9, 0x80)] * 3, z=zs, iip=1)
            t.meta.update(geom=key, z=zs)
    for key in ("s1", "c1"):
        t = b.add(Test(f"z24_sc_{key}", f"Z24 dZ/dx 1/512, geometry {key}, SCISSOR x from 77", 512, 32,
                       zpsm="Z24", ofs=OFS))
        t.ad(("SCISSOR_1", P3.SCISSOR(77, 511, 0, 31)))
        zs = [0x345678, 0x345678 + 2, 0x345678 + 54321]
        P3.tri(t, P3.geom_pts(key), [(9, 9, 9, 0x80)] * 3, z=zs, iip=1)
        t.meta.update(geom=key, z=zs)
    return b


def batch_misc() -> Batch:
    b = Batch("p4_misc", "Gouraud lines, UV triangles, exact 1/128 colour planes, coverage on non-dyadic edges")
    # lines with exact dC/dmajor = k/512 (colour differs by 2k over 1024 major units)
    for n in range(3):
        t = b.add(Test(f"line_exact_{n}", "x-major Gouraud lines, dC/dx = 1/512, -1/512, 2/512, 4/512", 512, 32,
                       ofs=OFS))
        rng = np.random.RandomState(4410 + n)
        for k in range(8):
            y0 = 1 + 4 * k + rng.randint(0, 16) / 16
            x0 = -rng.randint(0, 400) - rng.randint(0, 16) / 16
            dy = rng.randint(-2, 3) + rng.randint(0, 16) / 16
            ca = (100, 150, 60, 20)
            cb = (102, 148, 64, 28)
            a, c = (x0, y0), (x0 + 1024, y0 + dy)
            if n == 2 and k % 2:
                a, c, ca, cb = c, a, cb, ca
            prim(t, "line", [dict(x=a[0], y=a[1], rgba=ca, z=0), dict(x=c[0], y=c[1], rgba=cb, z=0)], iip=1)
    t = b.add(Test("line_ymajor", "y-major Gouraud lines, dC/dy = k/16", 512, 32, ofs=OFS))
    for k in range(16):
        x0 = 8 + 30 * k + k / 16
        prim(t, "line", [dict(x=x0, y=-100.0 + k / 8, rgba=(100, 150, 60, 20)),
                         dict(x=x0 + (k - 8) * 0.75, y=412.0 + k / 8, rgba=(132, 118, 124, 148))], iip=1)
    # UV triangles, U gradient 1/512 UV units (1/8192 texel) per pixel
    up = b.add(Test("upload", "address textures (draws nothing)", 64, 32))
    blk = b.tex_alloc(1)
    v, u = np.mgrid[0:16, 0:16]
    addr = (u * 16 | (v * 16) << 8 | 0x33 << 16 | 0x80 << 24).astype("<u4")
    up.image(blk, 64, "CT32", 16, 16, addr.tobytes(), label="address16")
    b.address16 = addr
    for n, (ua, vb) in enumerate(((1.0, 2.0), (3.3125, 7.0625), (14.0, 1.5))):
        for filt in (1, 0):
            t = b.add(Test(f"uv_exact_{n}_{'bil' if filt else 'near'}", "UV triangle, dU/dx = 2/1024 UV units",
                           512, 32, ofs=OFS))
            t.ad(("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 1)), ("TEX1_1", TEX1(mmag=filt, mmin=filt)),
                 ("CLAMP_1", CLAMP(1, 1)))
            pts = P3.geom_pts(["v1000", "s1", "c1"][n])
            uvs = [(ua, vb), (ua + 2 / 16, vb), (ua + 0.75, vb + 5.3125)]
            prim(t, "tri", [dict(x=x, y=y, rgba=(0x80,) * 4, uv=uv) for (x, y), uv in zip(pts, uvs)], tme=1, fst=1)
    # colour planes with gradients k/128 per pixel from vertices 128-aligned apart: every sample is an
    # exact 1/128 value
    for n, (pts, base, ab) in enumerate([
            ([(-128, -64), (384, -64), (-128, 448)], (10, 200, 30, 0), [(1, 3), (-1, -1), (1, 3), (2, 1)]),
            ([(3, -2), (259, 126), (-125, 254)], (20, 20, 20, 20), [(1, 1), (2, -1), (-1, 3), (3, 2)]),
            ([(500, -20), (-12, 108), (244, 236)], (200, 50, 90, 128), [(1, -2), (-1, 1), (2, 1), (-2, -1)])]):
        t = b.add(Test(f"col128_{n}", "colour plane k/128 per pixel from 128-aligned vertices", 512, 32, ofs=OFS))
        cols = []
        for (x, y) in pts:
            dx, dy = x - pts[0][0], y - pts[0][1]
            c = []
            for ch in range(4):
                a_, b_ = ab[ch]
                num = a_ * dx + b_ * dy
                assert num % 128 == 0, (n, ch, num)
                c.append(base[ch] + num // 128)
            assert all(0 <= v <= 255 for v in c), c
            cols.append(tuple(c))
        prim(t, "tri", [dict(x=x, y=y, rgba=c) for (x, y), c in zip(pts, cols)], iip=1)
        t.meta["ab"] = ab
    # coverage on edges through integer points with non-dyadic slopes (count mode)
    slopes = [(1, 3), (1, 5), (1, 7), (2, 7), (3, 7), (5, 9), (4, 9), (2, 3), (3, 5), (5, 7), (7, 9), (6, 7)]
    for n in range(4):
        t = b.add(Test(f"cov_nd_{n}", "count coverage: triangles pairs sharing non-dyadic edges", 256, 32, ofs=OFS))
        t.ad(("ALPHA_1", ADD), ("TEST_1", TEST()), ("COLCLAMP", 1))
        t.meta["mode"] = "coverage_count"
        for k, (dx, dy) in enumerate(slopes[3 * n:3 * n + 3]):
            x0 = 10 + 80 * k
            sx = 1 if n % 2 == 0 else -1
            top, bot = (x0 + (0 if sx > 0 else 20), -3), (x0 + (0 if sx > 0 else 20) + sx * dx * 5, -3 + dy * 5)
            # a pair of triangles sharing the edge top-bot, one each side
            left = (bot[0] - 30, top[1] + 7)
            right = (top[0] + 40, bot[1] - 2)
            prim(t, "tri", [dict(x=top[0], y=top[1], rgba=(1, 0, 0, 0x80)), dict(x=bot[0], y=bot[1], rgba=(1, 0, 0, 0x80)),
                            dict(x=left[0], y=left[1], rgba=(1, 0, 0, 0x80))], abe=1)
            prim(t, "tri", [dict(x=bot[0], y=bot[1], rgba=(1, 0, 0, 0x80)), dict(x=top[0], y=top[1], rgba=(1, 0, 0, 0x80)),
                            dict(x=right[0], y=right[1], rgba=(1, 0, 0, 0x80))], abe=1)
    return b


def batches():
    return [batch_rcp(), batch_z(), batch_misc()]


def main() -> None:
    P3.batches = batches
    P3.OUT = Path(os.environ["GSCAP_OUT"])

    def save_inputs(b, out, pkt, doc):
        import json
        (out / "packet.bin").write_bytes(pkt)
        (out / "batch.json").write_text(json.dumps(doc, indent=1) + "\n")
        extra = {k: getattr(b, k) for k in ("address", "address16") if hasattr(b, k)}
        if extra:
            np.savez_compressed(out / "inputs.npz", **extra)
    P3.save_inputs = save_inputs
    P3.main()


if __name__ == "__main__":
    main()
