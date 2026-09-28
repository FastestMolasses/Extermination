#!/usr/bin/env python3
"""gs_conformance_probe5.py - third follow-up batch for the port's CPU GS
model (docs/GS_EXACT.md in the port): how S, T and Q are interpolated
inside triangles before the perspective divide.

Same harness, rules and clean room as tools/gs_conformance.py (docs/
GS_CONFORMANCE.md). Nothing here is disc-derived; no emulator source read.

Design. p4 settled the divide for constant S, T, Q (the port's
GS_EXACT.md section 4.4): with E the exponent of Q, S is floored to
2^-(14-E) and Q keeps 16 significant bits. A 1024 x 1 texture whose texel i
holds R = 16 i & 255, G = i >> 4 turns BILINEAR sampling into a direct
reading of U16 = floor((u - 1/2) * 16) over the whole width (R = U16 & 255,
G = U16 >> 8, while U16 < 16368). With Q = 1 exactly the reading is
floor(S * 2^14) - 8, the interpolated S itself at the divide's resolution.
- p5_s: Q = 1 at all vertices, S planes with slow exact gradients
  (dS/dx = 2^-14 / 512 and friends, as p3_start does for colour), vertical,
  sloped, clipped left edges; plus general S planes.
- p5_q: S constant, Q planes (within one binade, and crossing 1.0, 2.0);
  S and Q both varying (the level class's case); 16 x 16 address texture
  checks.

Usage (decomp root; Renderer = 13 manual switch, docs/GS_CONFORMANCE.md 2):
  .venv/bin/python tools/gs_conformance_probe5.py list | capture [B..] | decode [B..]
Outputs (ignored): build/b16/gscap5/<batch>/.
"""
from __future__ import annotations

import os
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
os.environ.setdefault("GSCAP_OUT", str(ROOT / "build/b16/gscap5"))

import gs_conformance_probe3 as P3  # noqa: E402
from gs_conformance import CLAMP, TEX0, TEX1, Batch, Test  # noqa: E402
from gs_conformance_suite import prim  # noqa: E402

OFS = (1024.0, 1024.0)


def f32(v: float) -> float:
    return float(np.float32(v))


def upload_address(b: Batch):
    up = b.add(Test("upload", "1024 x 1 U16 address texture (draws nothing)", 64, 32))
    i = np.arange(1024, dtype=np.uint32)
    tex = ((16 * i) & 255 | (i >> 4) << 8 | 0x55 << 16 | 0x80 << 24).astype("<u4")
    blk = b.tex_alloc(16)
    up.image(blk, 1024, "CT32", 1024, 1, tex.tobytes(), label="u16address1024")
    b.address = tex
    return TEX0(blk, 1024, "CT32", 10, 0, 1, 1)                # DECAL, TCC 1


def stq_tri(t: Test, pts, st, qs):
    vs = [dict(x=x, y=y, rgba=(0x80,) * 4, qv=f32(q), st=(f32(s * q), 0.0)) for (x, y), s, q in zip(pts, st, qs)]
    prim(t, "tri", vs, tme=1)
    t.meta.setdefault("stq", []).append({"pts": pts, "s": [v["st"][0] for v in vs], "q": [v["qv"] for v in vs]})


def batch_s() -> Batch:
    b = Batch("p5_s", "S interpolation with Q = 1: exact slow S planes over 512-pixel rows")
    tex0 = upload_address(b)
    base = 0.25                                  # S at A; u * 1024 = 256 texels
    lsb = 2.0 ** -14
    keys = ["v1000", "v1024", "v64", "v333", "vhalf", "s1", "s2", "s3", "s4", "s5", "s6", "c1", "c2", "u1", "u2"]
    for key in keys:
        for k, dsc in ((1, 7), (4, -3)):         # S_B - S_A = 2k LSB (dS/dx = k/512 LSB), S_C - S_A = dsc*37 LSB
            t = b.add(Test(f"s_{key}_{k}", f"Q = 1, dS/dx = {k}/512 of 2^-14, geometry {key}", 512, 32, ofs=OFS))
            t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(1, 1)))
            pts = P3.geom_pts(key)
            s = [base, base + 2 * k * lsb, base + dsc * 37 * lsb]
            stq_tri(t, pts, s, [1.0, 1.0, 1.0])
            t.meta["geom"] = key
    for key, sc in (("s1", (100, 511, 4, 31)), ("c1", (37, 400, 0, 31))):
        t = b.add(Test(f"s_{key}_sc", f"Q = 1, dS/dx = 1/512 LSB, geometry {key}, SCISSOR {sc}", 512, 32, ofs=OFS))
        t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(1, 1)),
             ("SCISSOR_1", P3.SCISSOR(*sc)))
        stq_tri(t, P3.geom_pts(key), [base, base + 2 * lsb, base + 7 * 37 * lsb], [1.0, 1.0, 1.0])
        t.meta.update(geom=key, scissor=sc)
    rng = np.random.RandomState(5501)
    for n in range(6):
        t = b.add(Test(f"s_gen_{n}", "Q = 1, general S planes (fast gradients)", 512, 32, ofs=OFS))
        t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(1, 1)))
        box = [((-60, 0), (-30, -2)), ((560, 700), (-20, 10)), ((-20, 520), (36, 90))]
        pts = [(float(np.round(rng.uniform(*bx) * 16) / 16), float(np.round(rng.uniform(*by) * 16) / 16))
               for bx, by in box]
        s = [f32(rng.uniform(0.05, 0.9)) for _ in range(3)]
        stq_tri(t, pts, s, [1.0, 1.0, 1.0])
    return b


def batch_q() -> Batch:
    b = Batch("p5_q", "Q interpolation: constant S / varying Q, both varying, binade crossings")
    tex0 = upload_address(b)
    lsbq = 2.0 ** -15
    rng = np.random.RandomState(5502)
    for key in ["v1000", "v64", "s1", "s2", "s4", "c1", "u2"]:
        for qa, dq in ((1.25, 2), (1.5, -2), (0.75, 1), (3.0, 8)):
            t = b.add(Test(f"q_{key}_{qa}_{dq}", f"S/Q = const at A, Q plane (dQ/dx = {dq}/1024 LSB), {key}",
                           512, 32, ofs=OFS))
            t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(1, 1)))
            pts = P3.geom_pts(key)
            # S constant = 0.3 (vertex S words are S itself here: pass u = S / Q)
            e = int(np.floor(np.log2(qa)))
            qs = [qa, qa + dq * lsbq * 2 ** e, qa + 13 * dq * lsbq * 2 ** e * 37]
            svals = [0.3 / q for q in qs]                 # stq_tri multiplies by q: S = 0.3 at every vertex
            stq_tri(t, pts, svals, qs)
            t.meta["geom"] = key
    for n in range(10):
        t = b.add(Test(f"sq_gen_{n}", "S and Q both varying (perspective), some crossing a binade", 512, 32,
                       ofs=OFS))
        t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(1, 1)))
        box = [((-60, 0), (-30, -2)), ((560, 700), (-20, 10)), ((-20, 520), (36, 90))]
        pts = [(float(np.round(rng.uniform(*bx) * 16) / 16), float(np.round(rng.uniform(*by) * 16) / 16))
               for bx, by in box]
        qs = [f32(rng.uniform(0.4, 3.0)) for _ in range(3)]
        us = [f32(rng.uniform(0.05, 0.9)) for _ in range(3)]
        stq_tri(t, pts, us, qs)
    return b


def batches():
    return [batch_s(), batch_q()]


def main() -> None:
    P3.batches = batches
    P3.OUT = Path(os.environ["GSCAP_OUT"])

    def save_inputs(b, out, pkt, doc):
        import json
        (out / "packet.bin").write_bytes(pkt)
        (out / "batch.json").write_text(json.dumps(doc, indent=1) + "\n")
        if hasattr(b, "address"):
            np.savez_compressed(out / "inputs.npz", address=b.address)
    P3.save_inputs = save_inputs
    P3.main()


if __name__ == "__main__":
    main()
