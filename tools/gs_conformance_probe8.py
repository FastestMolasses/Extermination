#!/usr/bin/env python3
"""gs_conformance_probe8.py - sixth follow-up batch for the port's CPU GS
model (docs/GS_EXACT.md in the port): the B16 review's open claims.

Same harness, rules and clean room as tools/gs_conformance.py (docs/
GS_CONFORMANCE.md). Nothing here is disc-derived; no emulator source read.

Why: the review of the model found rules that rested on documentation or on
an analogy, not on a capture:
- points were modelled as not dithered (GS_EXACT 5.3) with no point case;
- the span boundaries marked "by the same rule" in GS_EXACT 3.7 (texture
  registers, ALPHA / PABE with ABE 1, FOGCOL with FGE 1, DIMX with DTHE 1,
  XYOFFSET, SCISSOR, FRAME, ZBUF, FBA, a class change, a CLUT reload,
  transfers into the span's memory) were never drawn;
- the CLD 4 / 5 compare (load only when CBP differs from CBP0 / CBP1) was
  documentation only;
- the PACKED and REGLIST GIF paths, and the Q a PACKED RGBAQ takes before
  any PACKED ST, were documentation only.

Batches:
- p8_span: the level strips of p3_stq over p3_stq's address texture (S, T
  / 4, CLAMP, bilinear, DECAL), as p7_flush: a varying-Z half strip, one
  write (or a transfer), a constant-Z half strip. If the write ends the
  span, the second half gets the STQ vertex grid; the tests are chosen so
  that the grid changes pixels. The writes change state without changing
  any pixel except through the grid (checked with the model: tools/
  gs_conformance_probe8.py discriminate, below). The reverse order
  (constant Z first) covers the attribute bits whose second half cannot
  use the grid (TME 0, FST 1).
- p8_class: triangles, sprites and points of one attribute set over a
  1024 x 1 address texture (nearest, DECAL, as p4_rcp): a varying-Z strip
  then constant-Z sprites (and the reverse), constant-Z points; sprites
  whose two corners have different Z.
- p8_misc: points on a CT16 frame with DTHE 1 (and the CT32 / DTHE 0
  controls, COLCLAMP 0); CLD 0..5 with CLUT memory rewritten between
  loads; textured points (UV and STQ, nearest and bilinear).
- p8_more (second run): TEX0 field changes (TBP0 to an identical copy, CBP,
  TCC, CLD on a CT32 texture), TEX2 CLD / CSA on CT32, and T8 + CLUT
  strips (the T8 + CLUT reads as the address texture) with TEX0 / TEX2
  CBP changes to an identical CLUT copy (with and without a load), a
  reload of an unchanged CLUT, a transfer into the CLUT memory (same and
  other bytes, no reload) and a reload of changed CLUT memory; strips
  drawn as triangle lists whose triangles each have one Z.
- p8_gif: PACKED tags (every descriptor the model decodes, junk in the
  unused bits, the ADC bit, A+D and NOP descriptors), the Q a PACKED
  RGBAQ uses when no PACKED ST came before it in the tag, and REGLIST tags
  (even and odd register counts).
- p8_gif2 (third run): PACKED descriptors 0 (PRIM, with junk above bit
  10), 6 / 8 (TEX0_1 / CLAMP_1 per sprite) and C / D (XYZF3 / XYZ3 in
  register form, no drawing kick); REGLIST with A+D and NOP descriptors
  whose words would change state if they were written.

Usage (decomp root; Renderer = 13 manual switch, docs/GS_CONFORMANCE.md 2):
  .venv/bin/python tools/gs_conformance_probe8.py list | capture [B..] | decode [B..]
  .venv/bin/python tools/gs_conformance_probe8.py discriminate   # no emulator: model predictions
Outputs (ignored): build/b16/gscap8/<batch>/.
"""
from __future__ import annotations

import hashlib
import os
import struct
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
os.environ.setdefault("GSCAP_OUT", str(ROOT / "build/b16/gscap8"))

import gs_conformance as G  # noqa: E402
import gs_conformance_probe3 as P3  # noqa: E402
import gs_conformance_probe7 as P7  # noqa: E402
from gs_conformance import (ALPHA, BITBLTBUF, CLAMP, DIMX, FOG, FOGCOL, FRAME, PRIM, RGBAQ, SCISSOR,  # noqa: E402
                            ST, TEST, TEX0, TEX1, TEXA, TRXPOS, TRXREG, UV, XYOFFSET, XYZ2, ZBUF, Batch, Test,
                            f32bits, fx4)
from gs_conformance_suite import prim, rand_ct32, vtx  # noqa: E402

G.REG.update({"MIPTBP1_1": 0x34, "MIPTBP2_1": 0x36})
G.REG_NAME.update({0x34: "MIPTBP1_1", 0x36: "MIPTBP2_1"})
OFS = (1024.0, 1024.0)
ZC = 0x2000
# Model-only markers (never in a captured packet): 0x7E holds the span
# across the next writes (1) or releases it (0); 0x7F ends it. The
# discriminate command inserts them to predict "ends" / "does not end".
MARK = {"hold": None, "end": None}
DIMX_STD = [[-4, 2, -3, 3], [0, -2, 1, -1], [-3, 3, -4, 2], [1, -1, 0, -2]]


def mark(t: Test, what: str) -> None:
    if MARK.get("on"):
        t.ad(("MARK_HOLD", 1) if what == "hold" else ("MARK_HOLD", 0) if what == "release" else ("MARK_END", 0))


def f32(v: float) -> float:
    return float(np.float32(v))


# ---------------------------------------------------------------------------
# p8_span
def verts(strip, zs, dx=0.0):
    return [dict(x=v["x"] + dx, y=v["y"], rgba=(0x80,) * 4, qv=v["qv"], z=z,
                 st=(f32(v["st"][0] / 4), f32(v["st"][1] / 4)))
            for v, z in zip(strip, zs)]


def uv_verts(strip, zs):
    out = []
    for v, z in zip(strip, zs):
        s, tt, q = v["st"][0] / 4, v["st"][1] / 4, v["qv"]
        out.append(dict(x=v["x"], y=v["y"], rgba=(0x80,) * 4, z=z,
                        uv=(round(min(max(s / q * 16, 0), 15.9) * 16) / 16, round(min(max(tt / q * 16, 0), 15.9) * 16) / 16)))
    return out


def batch_span() -> Batch:
    b = Batch("p8_span", "varying-Z / constant-Z half strips around one write: the unmeasured span boundaries")
    up = b.add(Test("upload", "address texture (draws nothing)", 64, 32))
    blk = b.tex_alloc(1)
    spare = b.tex_alloc(1)
    b.textures_addr = P3.address_texture()
    up.image(blk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    tex0 = TEX0(blk, 64, "CT32", 4, 4, 1, 1)
    strips = P7.level_strips()
    neutral_alpha = ALPHA(0, 1, 0, 1)                      # (Cs - Cd) * As + Cd, As = 0x80: Cs
    neutral_alpha2 = ALPHA(0, 1, 2, 1, 0x80)               # the same with FIX 0x80
    # case: (pre-A writes, A flags, between writes or action, B flags, order, Z buffer)
    cases = {
        "tex1_k": ([], {}, [("TEX1_1", TEX1(mmag=1, mmin=1, k=5))], {}, "vc"),
        "tex2_same": ([], {}, [("TEX2_1", TEX0(0, 64, "CT32", 0, 0))], {}, "vc"),
        "tex2_cbp": ([], {}, [("TEX2_1", TEX0(0, 64, "CT32", 0, 0, cbp=spare))], {}, "vc"),
        "texa": ([], {}, [("TEXA", TEXA(0x10, 0, 0x70))], {}, "vc"),
        "xyoffset_move": ([], {}, "xyoffset", {}, "vc"),
        "scissor_shrink": ([], {}, [("SCISSOR_1", SCISSOR(0, 239, 0, 31))], {}, "vc"),
        "frame_fbmsk": ([], {}, "fbmsk", {}, "vc"),
        "frame_same": ([], {}, "frame_same", {}, "vc"),
        "zbuf_other": ([], {}, "zbuf", {}, "vc"),
        "fba": ([], {}, [("FBA_1", 1)], {}, "vc"),
        "dimx_dthe1": ([("DTHE", 1)], {}, [("DIMX", DIMX(DIMX_STD))], {}, "vc"),
        "dimx_dthe0": ([], {}, [("DIMX", DIMX(DIMX_STD))], {}, "vc"),
        "alpha_abe1": ([("ALPHA_1", neutral_alpha)], {"abe": 1}, [("ALPHA_1", neutral_alpha2)], {"abe": 1}, "vc"),
        "alpha_same_abe1": ([("ALPHA_1", neutral_alpha)], {"abe": 1}, [("ALPHA_1", neutral_alpha)], {"abe": 1}, "vc"),
        "pabe_abe1": ([("ALPHA_1", neutral_alpha)], {"abe": 1}, [("PABE", 1)], {"abe": 1}, "vc"),
        "pabe_abe0": ([], {}, [("PABE", 1)], {}, "vc"),
        "fogcol_fge1": ([("FOG", FOG(0xFF)), ("FOGCOL", FOGCOL(0x10, 0x20, 0x30))], {"fge": 1},
                        [("FOGCOL", FOGCOL(0x70, 0x60, 0x50))], {"fge": 1}, "vc"),
        "fogcol_same_fge1": ([("FOG", FOG(0xFF)), ("FOGCOL", FOGCOL(0x10, 0x20, 0x30))], {"fge": 1},
                             [("FOGCOL", FOGCOL(0x10, 0x20, 0x30))], {"fge": 1}, "vc"),
        "test_same": ([], {}, [("TEST_1", TEST(zte=1, ztst="ALWAYS"))], {}, "vc"),
        "clamp_same": ([], {}, [("CLAMP_1", CLAMP(1, 1))], {}, "vc"),
        "colclamp_same": ([], {}, [("COLCLAMP", 1)], {}, "vc"),
        "dthe_same": ([], {}, [("DTHE", 0)], {}, "vc"),
        "prim_fge": ([("FOG", FOG(0xFF)), ("FOGCOL", FOGCOL(0x10, 0x20, 0x30))], {}, [], {"fge": 1}, "vc"),
        "prim_abe": ([("ALPHA_1", neutral_alpha)], {}, [], {"abe": 1}, "vc"),
        "rev_none": ([], {}, [], {}, "cv"),
        "rev_prim_iip": ([], {}, [], {"iip": 1}, "cv"),
        "rev_prim_tme": ([], {}, [], {"tme": 0}, "cv"),
        "rev_prim_fst": ([], {}, [], {"fst": 1}, "cv"),
        "trx_tex_same": ([], {}, "trx_tex", {}, "vc"),
        "trx_frame": ([], {}, "trx_frame", {}, "vc"),
        "trx_z": ([], {}, "trx_z", {}, "vc"),
    }
    for n, strip in enumerate(strips[:2]):
        # MIPTBP1 and TEXCLUT are not in the test reset: a value per strip keeps each write a change
        cases["miptbp1"] = ([], {}, [("MIPTBP1_1", 0x123 + 0x40 * n | 1 << 14)], {}, "vc")
        cases["texclut"] = ([], {}, [("TEXCLUT", 4 + n | 1 << 6)], {}, "vc")
        nv = len(strip)
        half = nv // 2 + 1
        var_a = [ZC + 0x100 * k for k in range(half)]
        const_b = [ZC] * (nv - half + 2)
        const_a = [ZC] * half
        var_b = [ZC + 0x100 * k for k in range(nv - half + 2)]
        for case, (pre, aflags, between, bflags, order) in cases.items():
            zw = case == "trx_z"
            t = b.add(Test(f"s{n}_{case}", f"strip {n}: {'varying' if order == 'vc' else 'constant'}-Z half, "
                           f"{case}, {'constant' if order == 'vc' else 'varying'}-Z half", 256, 32, ofs=OFS,
                           zpsm="Z24" if zw else None, zclear=0x100))
            t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(1, 1)), *pre)
            za, zb = (var_a, const_b) if order == "vc" else (const_a, var_b)
            fa = {"tme": 1}
            fa.update(aflags)
            prim(t, "tristrip", verts(strip[:half], za), **fa)
            mark(t, "hold")
            dx = 0.0
            if isinstance(between, list):
                if between:
                    t.ad(*between)
            elif between == "xyoffset":
                t.ad(("XYOFFSET_1", XYOFFSET(OFS[0] + 16, OFS[1])))
                dx = 16.0
            elif between == "fbmsk":
                t.ad(("FRAME_1", FRAME(t.fbp, 256, "CT32", fbmsk=0xFF000000)))
            elif between == "frame_same":
                t.ad(("FRAME_1", FRAME(t.fbp, 256, "CT32")))
            elif between == "zbuf":
                t.ad(("ZBUF_1", ZBUF(b.scratch_zbp, "Z32", zmsk=1)))
            elif between == "trx_tex":
                t.image(blk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address_again")
            elif between == "trx_frame":
                # 8 x 8 zeros (the clear colour) where neither half draws (x >= 232)
                t.image(t.fbp * 32, 256, "CT32", 8, 8, bytes(256), dx=240, dy=8, label="frame_same_bytes")
            elif between == "trx_z":
                zw_word = struct.pack("<I", t.zfill)
                t.image(t.zbp * 32, 256, "CT32", 8, 8, zw_word * 64, dx=240, dy=8, label="z_same_bytes")
            fb = {"tme": 1}
            fb.update(bflags)
            if fb.get("tme") == 0:
                fb.pop("tme")
                vb = [dict(v, rgba=(0x40, 0x80, 0xC0, 0x80)) for v in verts(strip[half - 2:], zb)]
                for v in vb:
                    v.pop("st"), v.pop("qv")
            elif fb.get("fst"):
                vb = uv_verts(strip[half - 2:], zb)
            else:
                vb = verts(strip[half - 2:], zb, dx)
            prim(t, "tristrip", vb, **fb)
            mark(t, "release")
            t.meta["case"] = case
            t.meta["order"] = order
    return b


# ---------------------------------------------------------------------------
# p8_class
def address1024():
    i = np.arange(1024, dtype=np.uint32)
    return (i & 255 | (i >> 8) << 8 | 0x55 << 16 | 0x80 << 24).astype("<u4")


def rcp_sprites(rng, count, z0, z1, y0=0):
    """2 x 2 constant-S sprites (p4_rcp's construction): u * 1024 = m + d."""
    ds = [0.002, 0.004, 0.007, 0.01, 0.015, 0.02, 0.03, 0.05]
    out = []
    for c in range(count):
        q = f32(rng.uniform(0.3, 6.0)) if c % 3 == 0 else f32(1.0 + rng.uniform(0, 1))
        m = int(rng.randint(64, 960))
        d = float(rng.choice(ds))
        s = f32(q * (m + d) / 1024)
        x, y = 2 * (c % 128), 2 * (c // 128) + y0
        za, zb = z0(c), z1(c)
        out.append([dict(x=x, y=y, rgba=(0x80,) * 4, qv=q, st=(s, 0.0), z=za),
                    dict(x=x + 2, y=y + 2, rgba=(0x80,) * 4, qv=q, st=(s, 0.0), z=zb)])
    return out


def strip1024(strip, zs):
    return [dict(x=v["x"], y=v["y"], rgba=(0x80,) * 4, qv=v["qv"], z=z,
                 st=(f32(v["st"][0] / 4), 0.0)) for v, z in zip(strip, zs)]


def batch_class() -> Batch:
    b = Batch("p8_class", "class changes (triangle / sprite / point) inside one span; sprite corner Z")
    up = b.add(Test("upload", "1024 x 1 address texture (draws nothing)", 64, 32))
    blk = b.tex_alloc(16)
    tex = address1024()
    up.image(blk, 1024, "CT32", 1024, 1, tex.tobytes(), label="address1024")
    b.address = tex
    tex0 = TEX0(blk, 1024, "CT32", 10, 0, 1, 1)
    strips = P7.level_strips()
    rng = np.random.RandomState(8801)

    def setup(t):
        t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1()), ("CLAMP_1", CLAMP(1, 1)))

    def sprites(t, sp):
        for a, c in sp:
            prim(t, "sprite", [a, c], tme=1)

    for n, strip in enumerate(strips[:2]):
        nv = len(strip)
        # the strip covers rows 0..36, the sprites and points rows 40..55 of a 256 x 64 buffer
        sp_const = rcp_sprites(rng, 1024, lambda c: ZC, lambda c: ZC, y0=40)
        sp_var = rcp_sprites(rng, 1024, lambda c: ZC + 0x40 * (c % 7), lambda c: ZC + 0x40 * (c % 7), y0=40)
        cases = {
            "tri_sprite": ("strip_var", "sprites_const"),
            "sprite_tri": ("sprites_var", "strip_const"),
            "rev_tri_sprite": ("strip_const", "sprites_var"),
            "rev_sprite_tri": ("sprites_const", "strip_var"),
            "tri_point": ("strip_var", "points_const"),
            "rev_tri_point": ("strip_const", "points_var"),
        }
        for case, (first, second) in cases.items():
            t = b.add(Test(f"c{n}_{case}", f"strip {n}: {first}, then {second}, nothing between", 256, 64, ofs=OFS))
            setup(t)
            for k, part in enumerate((first, second)):
                if k == 1:
                    mark(t, "hold")
                if part == "strip_var":
                    prim(t, "tristrip", strip1024(strip, [ZC + 0x100 * i for i in range(nv)]), tme=1)
                elif part == "strip_const":
                    prim(t, "tristrip", strip1024(strip, [ZC] * nv), tme=1)
                elif part == "sprites_const":
                    sprites(t, sp_const)
                elif part == "sprites_var":
                    sprites(t, sp_var)
                else:
                    pts = [a for a, _ in (sp_const if part == "points_const" else sp_var)]
                    w = [("PRIM", PRIM("point", tme=1))]
                    for v in pts:
                        w += vtx(t, **dict(v, x=v["x"] + 0.5, y=v["y"] + 0.5))
                    t.ad(*w)
                    t.meta.setdefault("prims", []).append({"kind": "point", "flags": {"tme": 1}, "count": len(pts)})
            mark(t, "release")
            t.meta["case"] = case
    # sprite corner Z (no strip): which corners' Z decide the grid
    for case, z0, z1 in (("corner_const", lambda c: ZC, lambda c: ZC),
                         ("corner_v0_var", lambda c: ZC + 0x40 * (c % 7), lambda c: ZC),
                         ("corner_v1_var", lambda c: ZC, lambda c: ZC + 0x40 * (c % 7)),
                         ("corner_pair_diff", lambda c: ZC + 0x40, lambda c: ZC)):
        t = b.add(Test(f"sprite_{case}", f"1024 constant-S sprites, corner Z {case}", 256, 32, ofs=OFS))
        setup(t)
        sprites(t, rcp_sprites(rng, 1024, z0, z1))
        t.meta["case"] = case
    return b


# ---------------------------------------------------------------------------
# p8_misc
def batch_misc() -> Batch:
    b = Batch("p8_misc", "point dithering; CLD 0..5 with CLUT memory rewritten; textured points")
    up = b.add(Test("upload", "address texture, T8 index texture, three CLUTs (draws nothing)", 64, 32))
    rng = np.random.RandomState(8802)
    ablk = b.tex_alloc(1)
    b.textures_addr = P3.address_texture()
    up.image(ablk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    idx = np.arange(256, dtype=np.uint8).reshape(16, 16)
    tb = b.tex_alloc(1)
    up.image(tb, 128, "T8", 16, 16, idx.tobytes(), label="t8_identity")
    cluts = [rand_ct32(rng, 16, 16, amax=0x81) for _ in range(3)]
    cbs = [b.tex_alloc(1) for _ in range(3)]
    for k in range(3):
        up.image(cbs[k], 64, "CT32", 16, 16, cluts[k].tobytes(), label=f"clut{k}")
    b.level_tex = (idx, cluts[0])
    b.cluts = cluts
    # 1. points and dithering
    for psm, dthe, cc in (("CT16", 1, 1), ("CT16", 1, 0), ("CT16", 0, 1), ("CT32", 1, 1)):
        t = b.add(Test(f"dither_points_{psm.lower()}_d{dthe}_cc{cc}", f"{psm} frame, DTHE {dthe}, COLCLAMP {cc}: "
                       "one point per pixel, random colours", 64, 64, psm=psm))
        t.ad(("DTHE", dthe), ("DIMX", DIMX(DIMX_STD)), ("COLCLAMP", cc))
        w = [("PRIM", PRIM("point"))]
        cols = rng.randint(0, 256, (64, 64, 3))
        if cc == 0:
            cols = np.where(rng.rand(64, 64, 3) < 0.5, 255 - (cols & 7), cols)   # near 255: wrap cases
        for y in range(64):
            for x in range(64):
                r, g, bb = (int(c) for c in cols[y, x])
                w += [("RGBAQ", RGBAQ(r, g, bb, 0x80)), ("XYZ2", XYZ2(x + t.ofs[0], y + t.ofs[1]))]
        t.ad(*w)
        t.meta["cols"] = "rng 8802"
        # the same colours as 1x1 sprites (measured undithered) and as tiny triangles (dithered) for reference
    # 2. CLD: draw a 16x16 T8 sprite after each TEX0; CLUT memory rewritten between
    def t8_tex0(cbp, cld):
        return TEX0(tb, 128, "T8", 4, 4, 1, 1, cbp=cbp, cpsm="CT32", cld=cld)

    def draw(t, k):
        prim(t, "sprite", [dict(x=16 * k, y=0, rgba=(0x80,) * 4, uv=(0, 0)),
                           dict(x=16 * k + 16, y=16, rgba=(0x80,) * 4, uv=(16, 16))], tme=1, fst=1)

    seqs = {
        # name: list of steps; a step is ("tex0", cbp_index, cld) + draw, or ("mem", target, source)
        "cld4": [("mem", 0, 0), ("tex0", 0, 2), ("mem", 0, 1), ("tex0", 0, 4), ("tex0", 2, 4), ("tex0", 2, 4)],
        "cld5": [("mem", 0, 0), ("tex0", 0, 3), ("mem", 0, 1), ("tex0", 0, 5), ("tex0", 2, 5), ("tex0", 2, 5)],
        "cld45_cross": [("mem", 0, 0), ("tex0", 0, 2), ("tex0", 2, 3), ("mem", 0, 1), ("tex0", 0, 5), ("tex0", 2, 4)],
        "cld0_cld1": [("mem", 0, 0), ("tex0", 0, 1), ("mem", 0, 1), ("tex0", 0, 0), ("tex0", 0, 1)],
        "cld1_same_value": [("mem", 0, 0), ("tex0", 0, 1), ("mem", 0, 2), ("tex0", 0, 1)],
        "cld2_3": [("mem", 0, 0), ("tex0", 0, 2), ("mem", 0, 1), ("tex0", 0, 3), ("mem", 0, 2), ("tex0", 0, 2)],
    }
    for name, steps in seqs.items():
        t = b.add(Test(f"clut_{name}", f"CLD sequence {name}: T8 16x16 sprites, CLUT memory rewritten", 128, 32))
        t.ad(("TEX1_1", TEX1()), ("CLAMP_1", CLAMP(1, 1)))
        k = 0
        for st in steps:
            if st[0] == "mem":
                t.image(cbs[st[1]], 64, "CT32", 16, 16, cluts[st[2]].tobytes(), label=f"clut{st[2]}_to_{st[1]}")
            else:
                t.ad(("TEX0_1", t8_tex0(cbs[st[1]], st[2])))
                draw(t, k)
                k += 1
        t.meta["steps"] = steps
    # 3. textured points over the address texture
    for mode, filt in (("uv", 0), ("uv", 1), ("stq", 0), ("stq", 1)):
        t = b.add(Test(f"tex_points_{mode}_{'bil' if filt else 'near'}", f"textured points ({mode}), "
                       f"{'bilinear' if filt else 'nearest'}", 64, 64))
        t.ad(("TEX0_1", TEX0(ablk, 64, "CT32", 4, 4, 1, 1)), ("TEX1_1", TEX1(mmag=filt, mmin=filt)),
             ("CLAMP_1", CLAMP(1, 1)))
        w = [("PRIM", PRIM("point", tme=1, fst=1 if mode == "uv" else 0))]
        for y in range(0, 64, 2):
            for x in range(0, 64, 2):
                if mode == "uv":
                    u, v = rng.randint(0, 16 * 16) / 16, rng.randint(0, 16 * 16) / 16
                    w += [("RGBAQ", RGBAQ(0x80, 0x80, 0x80)), ("UV", UV(u, v))]
                else:
                    q = f32(rng.uniform(0.3, 3.0))
                    s, tt = f32(rng.uniform(0, 1) * q), f32(rng.uniform(0, 1) * q)
                    w += [("RGBAQ", RGBAQ(0x80, 0x80, 0x80, q=q)), ("ST", ST(s, tt))]
                w += [("XYZ2", XYZ2(x + 0.5 + t.ofs[0], y + 0.5 + t.ofs[1]))]
        t.ad(*w)
    return b


# ---------------------------------------------------------------------------
# p8_gif: raw GIF tags
class RawTest(Test):
    """A Test whose items may also be raw GIF bytes ('raw')."""

    def raw(self, data: bytes, label: str):
        assert len(data) % 16 == 0
        self.items.append(("raw", {"bytes": data, "label": label}))

    def packet(self, scratch_zbp: int):
        pkt = G.ad_packet(self.reset_writes(scratch_zbp))
        rec = [{"ad": [[r, hex(v)] for r, v in self.reset_writes(scratch_zbp)], "role": "reset"}]
        for kind, body in self.items:
            if kind == "raw":
                pkt += body["bytes"]
                rec.append({"raw": body["label"], "hex": body["bytes"].hex()})
            elif kind == "ad":
                pkt += G.ad_packet(body)
                rec.append({"ad": [[r, hex(v & 0xFFFFFFFFFFFFFFFF)] for r, v in body]})
            else:
                bb = body
                writes = [("BITBLTBUF", BITBLTBUF(bb["dbp"], bb["dbw"], bb["psm"])),
                          ("TRXPOS", TRXPOS(bb["dx"], bb["dy"])), ("TRXREG", TRXREG(bb["w"], bb["h"])),
                          ("TRXDIR", 0)]
                pkt += G.ad_packet(writes) + G.image_packet(bb["data"]) + G.ad_packet([("TEXFLUSH", 0)])
                rec.append({"image": {k: v for k, v in bb.items() if k != "data"},
                            "ad": [[r, hex(v)] for r, v in writes],
                            "data_sha256": hashlib.sha256(bb["data"]).hexdigest(), "data_bytes": len(bb["data"])})
        return pkt, rec


def qw(lo: int, hi: int) -> bytes:
    return struct.pack("<QQ", lo & (2**64 - 1), hi & (2**64 - 1))


def w4(a: int, b: int, c: int, d: int) -> bytes:
    return struct.pack("<IIII", a & 0xFFFFFFFF, b & 0xFFFFFFFF, c & 0xFFFFFFFF, d & 0xFFFFFFFF)


def packed_tag(nloop, regs: list[int], prim_v=None, eop=0) -> bytes:
    r = 0
    for i, d in enumerate(regs):
        r |= d << (4 * i)
    return G.giftag(nloop, eop=eop, pre=1 if prim_v is not None else 0, prim=prim_v or 0, flg=0,
                    nreg=len(regs) & 15, regs=r)


def p_rgbaq(r, g, b_, a, junk=0):
    return w4(r | junk << 8, g | junk << 9, b_ | junk << 10, a | junk << 11)


def p_st(s, t, q):
    return w4(f32bits(s), f32bits(t), f32bits(q), 0)


def p_uv(u, v, junk=0):
    return w4(fx4(u) | junk << 14, fx4(v) | junk << 15, 0, 0)


def p_xyz2(x, y, z, adc=0, junk=0):
    return w4(fx4(x) | junk << 16, fx4(y) | junk << 17, z, adc << 15 | junk)


def p_xyzf2(x, y, z, f, adc=0, junk=0):
    return w4(fx4(x) | junk << 16, fx4(y) | junk << 17, (z & 0xFFFFFF) << 4 | (junk & 15),
              f << 4 | adc << 15 | junk << 16)


def p_fog(f):
    return w4(0, 0, 0, f << 4)


def p_ad(reg: str, value: int):
    return qw(value, G.REG[reg])


def batch_gif() -> Batch:
    b = Batch("p8_gif", "PACKED and REGLIST GIF tags; the Q a PACKED RGBAQ takes")
    up = RawTest("upload", "1024 x 1 address texture and the 16x16 address texture (draws nothing)", 64, 32)
    b.add(up)
    blk = b.tex_alloc(16)
    ablk = b.tex_alloc(1)
    tex = address1024()
    up.image(blk, 1024, "CT32", 1024, 1, tex.tobytes(), label="address1024")
    b.address = tex
    b.textures_addr = P3.address_texture()
    up.image(ablk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    tex1024 = TEX0(blk, 1024, "CT32", 10, 0, 1, 1)
    rng = np.random.RandomState(8803)
    sprite_tme = PRIM("sprite", tme=1)
    ox, oy = OFS

    def tex_setup(t, tex0=tex1024, filt=0):
        t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=filt, mmin=filt)), ("CLAMP_1", CLAMP(1, 1)))

    # 1. PACKED ST / RGBAQ / XYZ2 sprites with Q (PRE sets PRIM)
    t = b.add(RawTest("pk_st_rgbaq_xyz2", "PACKED [ST, RGBAQ, XYZ2] sprites, Q per sprite, PRE", 256, 32, ofs=OFS))
    tex_setup(t)
    body = b""
    for c in range(256):
        q = f32(rng.uniform(0.3, 4.0))
        s = f32(q * (rng.randint(64, 960) + rng.uniform(0.01, 0.99)) / 1024)
        x, y = 4 * (c % 64), 4 * (c // 64) * 2
        for dx, dy in ((0, 0), (4, 4)):
            body += p_st(s, 0.0, q) + p_rgbaq(0x80, 0x80, 0x80, 0x80) + p_xyz2(ox + x + dx, oy + y + dy, 0)
    t.raw(packed_tag(512, [2, 1, 5], prim_v=sprite_tme) + body, "packed sprites")
    # 2. the Q of a PACKED RGBAQ with no PACKED ST before it in its tag
    for case in ("after_packed_st", "after_ad_rgbaq", "after_ad_st"):
        t = b.add(RawTest(f"pk_q_{case}", f"PACKED RGBAQ without ST in its tag: {case}", 256, 32, ofs=OFS))
        tex_setup(t)
        for c in range(64):
            q1 = f32(rng.choice([0.5, 0.25, 2.0, 0.75]))
            s = f32(rng.uniform(0.05, 0.45))
            x, y = 4 * (c % 64), 0
            # tag 1: one sprite with ST Q = q1 (sets S, T and the PACKED Q)
            one = b""
            for dx, dy in ((0, 0), (4, 4)):
                one += p_st(s, 0.0, q1) + p_rgbaq(0x80, 0x80, 0x80, 0x80) + p_xyz2(ox + x + dx, oy + y + dy, 0)
            t.raw(packed_tag(2, [2, 1, 5], prim_v=sprite_tme) + one, "sprite with PACKED ST")
            if case == "after_ad_rgbaq":
                t.raw(G.ad_packet([("RGBAQ", RGBAQ(0x80, 0x80, 0x80, 0x80, q=3.0))]), "A+D RGBAQ Q 3")
            elif case == "after_ad_st":
                t.raw(G.ad_packet([("ST", ST(s * 0.5, 0.0))]), "A+D ST S/2")
            # tag 2: [RGBAQ, XYZ2] only, one sprite in the next row
            two = b""
            for dx, dy in ((0, 16), (4, 20)):
                two += p_rgbaq(0x80, 0x80, 0x80, 0x80) + p_xyz2(ox + x + dx, oy + y + dy, 0)
            t.raw(packed_tag(2, [1, 5], prim_v=sprite_tme) + two, "sprite with PACKED RGBAQ only")
        t.meta["case"] = case
    # 3. PACKED UV, XYZF2 with fog, FOG, ADC, A+D, NOP, junk bits
    t = b.add(RawTest("pk_uv_xyzf2_fog", "PACKED [UV, RGBAQ, XYZF2] fogged Gouraud triangles, FOG + XYZ2 sprites", 256, 32,
                      ofs=OFS))
    tex_setup(t, TEX0(ablk, 64, "CT32", 4, 4, 1, 0), 0)
    t.ad(("FOGCOL", FOGCOL(0x20, 0x90, 0x40)))
    body = b""
    for k in range(24):
        for i in range(3):
            x = float(np.round(rng.uniform(-10, 266) * 16) / 16)
            y = float(np.round(rng.uniform(-6, 38) * 16) / 16)
            body += p_uv(rng.randint(0, 256) / 16, rng.randint(0, 256) / 16) + \
                p_rgbaq(*(int(c) for c in rng.randint(0, 256, 3)), 0x80) + \
                p_xyzf2(ox + x, oy + y, int(rng.randint(0, 1 << 24)), int(rng.randint(0, 256)))
    t.raw(packed_tag(72, [3, 1, 4], prim_v=PRIM("tri", iip=1, tme=1, fge=1, fst=1)) + body, "fogged UV triangles")
    body = b""
    for c in range(32):
        f = int(rng.randint(0, 256))
        x = 8 * c
        body += p_fog(f) + p_rgbaq(0xC0, 0x40, 0x10, 0x80) + p_xyz2(ox + x, oy + 24, 0) + \
            p_fog(255 - f) + p_rgbaq(0x10, 0x40, 0xC0, 0x80) + p_xyz2(ox + x + 8, oy + 32, 0)
    t.raw(packed_tag(32, [0xA, 1, 5, 0xA, 1, 5], prim_v=PRIM("sprite", fge=1)) + body, "FOG + XYZ2 sprites")
    t = b.add(RawTest("pk_junk_adc_ad_nop", "PACKED junk in unused bits, ADC (XYZ3) in a strip, A+D and NOP "
                      "descriptors", 256, 32, ofs=OFS))
    t.ad(("TEST_1", TEST(zte=1, ztst="ALWAYS")))
    body = b""
    for i in range(12):
        x = 24.0 * (i // 2) - 4 + rng.randint(0, 16) / 16
        y = 32.0 * (i % 2) - 1 + rng.randint(0, 16) / 16
        adc = 1 if i in (4, 7) else 0
        col = (int(c) for c in rng.randint(0, 256, 3))
        body += p_rgbaq(*col, 0x80, junk=0x5A5A5) + p_ad("FOG", FOG(0x33)) + w4(0xDEADBEEF, 1, 2, 3) + \
            p_xyz2(ox + x, oy + y, 0x1234, adc=adc, junk=0x3C3C)
    t.raw(packed_tag(12, [1, 0xE, 0xF, 5], prim_v=PRIM("tristrip", iip=1)) + body, "junk strip with ADC")
    # 4. REGLIST
    t = b.add(RawTest("reglist", "REGLIST tags: [PRIM, RGBAQ, XYZ2, XYZ2] sprites and an odd count", 256, 32, ofs=OFS))
    words = []
    for c in range(16):
        x = 16 * c
        words += [PRIM("sprite"), RGBAQ(*(int(v) for v in rng.randint(0, 256, 3)), 0x80),
                  XYZ2(ox + x, oy, 0), XYZ2(ox + x + 16, oy + 12, 0)]
    data = b"".join(struct.pack("<Q", w_) for w_ in words)
    t.raw(G.giftag(16, flg=1, nreg=4, regs=0x5510) + data, "reglist even")
    words = []
    for c in range(15):
        x = 16 * c + 4
        words += [RGBAQ(*(int(v) for v in rng.randint(0, 256, 3)), 0x80), XYZ2(ox + x, oy + 16, 0),
                  XYZ2(ox + x + 8, oy + 30, 0)]
    data = b"".join(struct.pack("<Q", w_) for w_ in words)
    data += b"\0" * ((-len(data)) % 16)
    t.raw(G.ad_packet([("PRIM", PRIM("sprite"))]) + G.giftag(15, flg=1, nreg=3, regs=0x551) + data, "reglist odd")
    return b


# ---------------------------------------------------------------------------
# p8_more (second run, after p8_span / p8_class): the TEX0 / TEX2 fields,
# T8 textures with CLUT reloads and CLUT-memory transfers inside the span,
# and triangle lists whose triangles each have one Z.
def t8_address():
    """An identity T8 index texture and a CLUT whose entry i holds the
    address-texture colour of texel (i & 15, i >> 4): T8 + CLUT reads as the
    address texture. The CLUT image stores entry i at the position with bits
    3 and 4 of i swapped (CSM1)."""
    idx = np.arange(256, dtype=np.uint8).reshape(16, 16)
    addr = P3.address_texture().ravel()
    img = np.zeros(256, dtype="<u4")
    for i in range(256):
        pos = (i & ~0x18) | ((i & 0x08) << 1) | ((i & 0x10) >> 1)
        img[pos] = addr[i]
    return idx, img.reshape(16, 16)


def batch_more() -> Batch:
    b = Batch("p8_more", "TEX0 / TEX2 fields, T8 CLUT reloads and CLUT-memory transfers inside the span, "
              "triangle lists with one Z per triangle")
    up = b.add(Test("upload", "address textures CT32 (two copies), T8 index, CLUT copies (draws nothing)", 64, 32))
    blk, blk2, spare = b.tex_alloc(1), b.tex_alloc(1), b.tex_alloc(1)
    tb, cba, cbb = b.tex_alloc(1), b.tex_alloc(1), b.tex_alloc(1)
    b.textures_addr = P3.address_texture()
    idx, clut = t8_address()
    swapped = ((clut & 0xFF) << 8 | (clut >> 8) & 0xFF | clut & 0xFFFF0000).astype("<u4")   # R <-> G
    b.level_tex = (idx, clut)
    b.cluts = [clut, swapped]
    up.image(blk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    up.image(blk2, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address_copy")
    up.image(tb, 128, "T8", 16, 16, idx.tobytes(), label="t8_identity")
    up.image(cba, 64, "CT32", 16, 16, clut.tobytes(), label="clut_a")
    up.image(cbb, 64, "CT32", 16, 16, clut.tobytes(), label="clut_b_copy")
    ct32 = TEX0(blk, 64, "CT32", 4, 4, 1, 1)
    t8 = lambda cbp=cba, cld=1: TEX0(tb, 128, "T8", 4, 4, 1, 1, cbp=cbp, cpsm="CT32", cld=cld)
    strips = P7.level_strips()
    cases = {
        # name: (setup TEX0, between: list of writes or an action)
        "tex0_tbp": (ct32, [("TEX0_1", TEX0(blk2, 64, "CT32", 4, 4, 1, 1))]),
        "tex0_cbp_ct32": (ct32, [("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 1, cbp=spare))]),
        "tex0_tcc": (ct32, [("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 0, 1))]),
        "tex0_cld_ct32": (ct32, [("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 1, cld=1))]),
        "tex2_cld_ct32": (ct32, [("TEX2_1", TEX0(0, 64, "CT32", 0, 0, cld=1))]),
        "tex2_csa_ct32": (ct32, [("TEX2_1", TEX0(0, 64, "CT32", 0, 0, csa=3))]),
        "t8_none": (t8(), []),
        "t8_tex0_same_reload": (t8(), [("TEX0_1", t8())]),
        "t8_tex0_cbp_reload": (t8(), [("TEX0_1", t8(cbb))]),
        "t8_tex0_cbp_noload": (t8(), [("TEX0_1", t8(cbb, 0))]),
        "t8_tex2_cbp_noload": (t8(), [("TEX2_1", TEX0(0, 64, "T8", 0, 0, cbp=cbb, cpsm="CT32", cld=0))]),
        "t8_tex2_cbp_reload": (t8(), [("TEX2_1", TEX0(0, 64, "T8", 0, 0, cbp=cbb, cpsm="CT32", cld=1))]),
        "t8_clut_mem_same": (t8(), "mem_same"),
        "t8_clut_mem_other": (t8(), "mem_other"),
        "t8_clut_reload_change": (t8(), "reload_change"),
    }
    for n, strip in enumerate(strips[:2]):
        nv = len(strip)
        half = nv // 2 + 1
        var_a = [ZC + 0x100 * k for k in range(half)]
        const_b = [ZC] * (nv - half + 2)
        for case, (setup_tex0, between) in cases.items():
            t = b.add(Test(f"m{n}_{case}", f"strip {n}: varying-Z half, {case}, constant-Z half", 256, 32, ofs=OFS))
            if case.startswith("t8"):
                # restore CLUT A's memory (an earlier test may have rewritten it)
                t.image(cba, 64, "CT32", 16, 16, clut.tobytes(), label="clut_a_restore")
            t.ad(("TEX0_1", setup_tex0), ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(1, 1)))
            prim(t, "tristrip", verts(strip[:half], var_a), tme=1)
            mark(t, "hold")
            if isinstance(between, list):
                if between:
                    t.ad(*between)
            elif between == "mem_same":
                t.image(cba, 64, "CT32", 16, 16, clut.tobytes(), label="clut_a_same_bytes")
            elif between == "mem_other":
                t.image(cba, 64, "CT32", 16, 16, swapped.tobytes(), label="clut_a_swapped")
            elif between == "reload_change":
                t.image(cba, 64, "CT32", 16, 16, swapped.tobytes(), label="clut_a_swapped")
                t.ad(("TEX0_1", t8()))
            prim(t, "tristrip", verts(strip[half - 2:], const_b), tme=1)
            mark(t, "release")
            t.meta["case"] = case
        # triangle lists: each triangle one Z, the Z differ between triangles (and the all-equal control)
        for case, zf in (("trilist_flat", lambda i: ZC + 0x100 * i), ("trilist_same", lambda i: ZC)):
            t = b.add(Test(f"m{n}_{case}", f"strip {n} as a TRI list, triangle i with Z {case}", 256, 32, ofs=OFS))
            t.ad(("TEX0_1", ct32), ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(1, 1)))
            for i in range(nv - 2):
                prim(t, "tri", verts(strip[i:i + 3], [zf(i)] * 3), tme=1)
            t.meta["case"] = case
    return b


# ---------------------------------------------------------------------------
# p8_gif2 (third run): the PACKED descriptors p8_gif did not use (0 PRIM,
# 6 TEX0_1, 8 CLAMP_1, C XYZF3, D XYZ3) and A+D / NOP descriptors inside
# REGLIST.
def batch_gif2() -> Batch:
    b = Batch("p8_gif2", "PACKED descriptors 0, 6, 8, C, D; A+D and NOP descriptors in REGLIST")
    up = RawTest("upload", "address texture and a second 16x16 CT32 texture (draws nothing)", 64, 32)
    b.add(up)
    ablk, xblk = b.tex_alloc(1), b.tex_alloc(1)
    b.textures_addr = P3.address_texture()
    rng = np.random.RandomState(8804)
    other = rand_ct32(rng, 16, 16, amax=0x81)
    up.image(ablk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    up.image(xblk, 64, "CT32", 16, 16, other.tobytes(), label="other")
    ox, oy = OFS
    # 1. descriptor 0 (PRIM, junk above bit 10) + RGBAQ + two XYZ2: one sprite per loop
    t = b.add(RawTest("pk_desc0_prim", "PACKED [PRIM, RGBAQ, XYZ2, XYZ2]: PRIM per loop, junk above bit 10", 256, 32,
                      ofs=OFS))
    body = b""
    for c in range(32):
        x, y = 8 * (c % 32), 0 if c % 2 else 16
        kind = PRIM("sprite") if c % 3 else PRIM("sprite", abe=1)
        body += qw(kind | 0x5A5A5A5A5A5A5 << 11, 0xDEADBEEF) + p_rgbaq(*(int(v) for v in rng.randint(0, 256, 3)), 0x40) + \
            p_xyz2(ox + x, oy + y, 0) + p_xyz2(ox + x + 8, oy + y + 16, 0)
    t.ad(("ALPHA_1", ALPHA(0, 2, 0, 1)))
    t.raw(packed_tag(32, [0, 1, 5, 5]) + body, "PRIM descriptor")
    # 2. descriptors 6 (TEX0_1) and 8 (CLAMP_1) per sprite, UV sprites
    t = b.add(RawTest("pk_desc6_8_tex", "PACKED [TEX0_1, CLAMP_1, UV, RGBAQ, XYZ2, UV, XYZ2]: textured UV sprites "
                      "with their own TEX0 and CLAMP", 256, 32, ofs=OFS))
    t.ad(("TEX1_1", TEX1()))
    body = b""
    for c in range(32):
        x, y = 8 * (c % 32), 0 if c % 2 else 16
        tex0 = TEX0(ablk if c % 2 else xblk, 64, "CT32", 4, 4, 1, 1)
        clamp = CLAMP(1, 1) if c % 3 else CLAMP(0, 0)
        u0, v0 = rng.randint(0, 64) / 4, rng.randint(0, 64) / 4
        body += qw(tex0, 0x77) + qw(clamp, 0x66) + p_uv(u0, v0) + p_rgbaq(0x80, 0x80, 0x80, 0x80) + \
            p_xyz2(ox + x, oy + y, 0) + p_uv(u0 + 20, v0 + 12) + p_xyz2(ox + x + 8, oy + y + 16, 0)
    t.raw(packed_tag(32, [6, 8, 3, 1, 5, 3, 5], prim_v=PRIM("sprite", tme=1, fst=1)) + body, "TEX0/CLAMP descriptors")
    # 3. descriptors D (XYZ3) and C (XYZF3) in register form: no drawing kick
    t = b.add(RawTest("pk_descCD_xyz3", "Gouraud strips whose first vertices (and one middle vertex) come through "
                      "PACKED descriptors D / C", 256, 32, ofs=OFS))
    for k in range(3):
        pts = [(24.0 * (i // 2) + 70 * k - 6 + rng.randint(0, 16) / 16, 32.0 * (i % 2) - 1 + rng.randint(0, 16) / 16)
               for i in range(8)]
        cols = [tuple(int(v) for v in rng.randint(0, 256, 3)) for _ in range(8)]
        raw = b""
        for i, ((x, y), col) in enumerate(zip(pts, cols)):
            if i in (0, 5):
                data = p_rgbaq(*col, 0x80) + qw(XYZ2(ox + x, oy + y, 0x100), 0)
                raw += packed_tag(1, [1, 0xD], prim_v=PRIM("tristrip", iip=1) if i == 0 else None) + data
            elif i == 1:
                xyzf3 = fx4(ox + x) | fx4(oy + y) << 16 | 0x100 << 32 | 0x80 << 56
                raw += packed_tag(1, [1, 0xC]) + p_rgbaq(*col, 0x80) + qw(xyzf3, 0)
            else:
                raw += packed_tag(1, [1, 5]) + p_rgbaq(*col, 0x80) + p_xyz2(ox + x, oy + y, 0x100)
        t.raw(raw, f"strip {k}")
    # 4. REGLIST with A+D (E) and NOP (F) descriptors: their words must be skipped
    t = b.add(RawTest("reglist_e_f", "REGLIST [PRIM, RGBAQ, E, XYZ2, F, XYZ2]: the E and F words carry values "
                      "that would change state if written", 256, 32, ofs=OFS))
    words = []
    for c in range(16):
        x = 16 * c
        words += [PRIM("sprite"), RGBAQ(*(int(v) for v in rng.randint(0, 256, 3)), 0x80),
                  FRAME(0x1B0, 64, "CT32"), XYZ2(ox + x, oy + 4, 0), SCISSOR(0, 3, 0, 3), XYZ2(ox + x + 12, oy + 28, 0)]
    data = b"".join(struct.pack("<Q", w_) for w_ in words)
    t.raw(G.giftag(16, flg=1, nreg=6, regs=0x5F5E10) + data, "reglist E F")
    return b


def batches():
    return [batch_span(), batch_class(), batch_misc(), batch_gif(), batch_more(), batch_gif2()]


# ---------------------------------------------------------------------------
# discriminate: the model's predictions with the boundary ending / not ending the span
def discriminate() -> None:
    import ctypes as C
    import importlib.util
    import json
    import re
    import subprocess
    port = ROOT.parent / "extermination-port"
    scratch = ROOT / "build/b16/gscap8/model"
    scratch.mkdir(parents=True, exist_ok=True)
    src = (port / "src/gs/em_gs_raster.c").read_text()
    # scratch copy with the two model-only marker registers
    hook = "static void span_end(EmGs *gs, int measured)\n{\n    if (g_hold) return;\n"
    src2 = "static int g_hold;\n" + src.replace("static void span_end(EmGs *gs, int measured)\n{\n", hook, 1)
    src2 = src2.replace("void em_gs_write(EmGs *gs, unsigned reg, uint64_t v)\n{\n",
                        "void em_gs_write(EmGs *gs, unsigned reg, uint64_t v)\n{\n"
                        "    if (reg == 0x7E) { g_hold = (int)v; return; }\n"
                        "    if (reg == 0x7F) { int h = g_hold; g_hold = 0; span_end(gs, 1); g_hold = h; return; }\n", 1)
    assert src2.count("g_hold") >= 5
    (scratch / "em_gs_raster_marked.c").write_text(src2)
    spec = importlib.util.spec_from_file_location("tgr", port / "tools/test_gs_raster_reference.py")
    tgr = importlib.util.module_from_spec(spec)
    sys.path.insert(0, str(port / "tools"))
    spec.loader.exec_module(tgr)
    lib_path = scratch / "libmarked.dylib"
    shim = scratch / "shim.c"
    shim.write_text(tgr.SHIM)
    subprocess.run(["cc", "-O2", "-fPIC", "-dynamiclib", "-I", str(port / "src"), str(scratch / "em_gs_raster_marked.c"),
                    str(port / "src/gs/em_gs_frame.c"), str(shim), "-lm", "-o", str(lib_path)], check=True)
    lib = C.CDLL(str(lib_path))
    lib.shim_new.restype = C.c_void_p
    lib.shim_new.argtypes = [C.c_void_p, C.c_int]
    lib.shim_free.argtypes = [C.c_void_p]
    lib.shim_gif.restype = C.c_size_t
    lib.shim_gif.argtypes = [C.c_void_p, C.c_char_p, C.c_size_t]
    lib.shim_refusals.restype = C.c_uint
    lib.shim_refusals.argtypes = [C.c_void_p]
    lib.shim_reason.restype = C.c_char_p
    lib.shim_reason.argtypes = [C.c_void_p]
    maps = dict(np.load(ROOT / "build/b16/gscap/layout/maps.npz"))
    G.REG.update({"MARK_HOLD": 0x7E, "MARK_END": 0x7F})
    preds = {}
    for variant in ("plain", "hold", "end"):
        MARK["on"] = variant != "plain"
        orig_mark = globals()["mark"]
        if variant == "end":
            def mark_end(t, what, _o=orig_mark):   # end the span where the boundary is
                if what == "hold":
                    t.ad(("MARK_END", 0))
            globals()["mark"] = mark_end
        try:
            for bt in batches():
                pkt, doc = bt.build()
                mem = (C.c_uint8 * G.LOCALMEM)()
                g = lib.shim_new(C.addressof(mem), 1)
                lib.shim_gif(g, pkt, len(pkt))
                ref = lib.shim_refusals(g), lib.shim_reason(g).decode()
                lib.shim_free(g)
                got = np.frombuffer(bytes(mem), np.uint8)
                for tt in doc["tests"]:
                    a = tgr.decode(got, tt["fbp"], tt["w"], tt["h"], tt["psm"], maps)
                    preds.setdefault(f"{bt.name}/{tt['name']}", {})[variant] = a
                preds.setdefault(f"{bt.name}/_refusals", {})[variant] = ref
        finally:
            globals()["mark"] = orig_mark
            MARK["on"] = False
    out = {}
    for k, v in preds.items():
        if k.endswith("_refusals"):
            out[k] = {kk: list(vv) for kk, vv in v.items()}
            continue
        out[k] = {"hold_vs_end": int((v["hold"] != v["end"]).sum()), "plain_vs_end": int((v["plain"] != v["end"]).sum()),
                  "plain_vs_hold": int((v["plain"] != v["hold"]).sum()), "drawn": int((v["plain"] != 0).sum())}
        np.savez_compressed(scratch / (k.replace("/", "__") + ".npz"), **v)
    (scratch / "discriminate.json").write_text(json.dumps(out, indent=1) + "\n")
    for k, v in out.items():
        print(k, v)


def main() -> None:
    if len(sys.argv) > 1 and sys.argv[1] == "discriminate":
        discriminate()
        return
    P3.batches = batches
    P3.OUT = Path(os.environ["GSCAP_OUT"])
    orig_save = P3.save_inputs

    def save_inputs(b, out, pkt, doc):
        orig_save(b, out, pkt, doc)
        extra = {}
        if hasattr(b, "address"):
            extra["address1024"] = b.address
        if hasattr(b, "cluts"):
            extra["cluts"] = np.stack(b.cluts)
        if extra:
            import numpy as _np
            prev = dict(_np.load(out / "inputs.npz")) if (out / "inputs.npz").exists() else {}
            prev.update(extra)
            _np.savez_compressed(out / "inputs.npz", **prev)
    P3.save_inputs = save_inputs
    P3.main()


if __name__ == "__main__":
    main()
