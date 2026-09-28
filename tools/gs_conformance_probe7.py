#!/usr/bin/env python3
"""gs_conformance_probe7.py - fifth follow-up batch for the port's CPU GS
model (docs/GS_EXACT.md in the port): which state decides whether the
perspective S, T, Q of a vertex are put on the Q-exponent grid.

Same harness, rules and clean room as tools/gs_conformance.py (docs/
GS_CONFORMANCE.md). Nothing here is disc-derived; no emulator source read.

Why: the model puts every vertex's S and T on a 2^-(14 - E) grid and its
Q on 2^-(15 - E) (E = the exponent of that vertex's Q). The single
perspective triangles of p3_stq / p4_rcp / p5 need that grid (without it
p5 misses 182,510 values). The four level-class strips of p3_stq are the
opposite: without the grid they are exact to 2 values, with it they miss
183. The strips differ from the single triangles in several ways at once:
a strip, Gouraud colour, fog, MODULATE, a T8 texture with a CLUT, REPEAT
wrapping with S/Q up to 3, and a Z test. p7_lvl draws the same four
strips with one of those features changed at a time.

- ref:     p3_stq's level_<n> again (control)
- nofog:   FGE 0
- flat:    IIP 0
- decal:   TFX DECAL instead of MODULATE
- ct32:    a CT32 16x16 texture instead of T8 + CLUT
- zalways: Z test ALWAYS instead of GEQUAL
- tris:    the strip's triangles as a triangle list (same vertices)
- small:   S and T divided by 4 (S/Q below 0.75)
- plain:   all of IIP 0, FGE 0, DECAL, CT32, Z ALWAYS

p7_wrap (second run, after p7_lvl showed that none of those features
matters): p3_stq's twelve perspective triangles and the four strips with
S and T / 4, all over p3_stq's address texture, each under CLAMP and under
REPEAT.

p7_z (third run: under both wrap modes the address-texture strips need the
grid, so the level class's state does matter, but no single feature of
p7_lvl): the same strips and the six bilinear perspective triangles with Z
written (ZTST ALWAYS or GEQUAL), with Z written but all vertex Z 0, with
vertex Z but no Z write (ZMSK 1), and with only the alpha test on.

p7_zc (fourth run: vertex Z removes the grid, Z writes and the alpha test
do not): the same geometry without Z writes and with vertex Z constant
(0x123456, 1), with one vertex's Z 1 and the others 0 (strips: the third
vertex only), and with Z planes that vary only in y or only in x.

p7_scope (fifth run: over what the decision is taken): two strips drawn as
TRI lists with only the first or only the last triangle's Z varying; a
strip whose last vertex alone differs; each strip split in two (constant
Z, then varying Z) with nothing, a PRIM rewrite, a TEX1 rewrite with the
same value, or a FOGCOL write between; and the reverse order.

p7_flush (sixth run: none of those ends the span): a varying-Z strip, then
one register write (TEX0 same value / other TFX, CLAMP, TEST, ALPHA,
COLCLAMP, DTHE, XYOFFSET and SCISSOR with their values, TEXFLUSH), a PRIM
with other flags, a switch to a TRI list, or a HOST -> LOCAL transfer,
then a constant-Z strip. All the changes leave the image unchanged.

Usage (decomp root; Renderer = 13 manual switch, docs/GS_CONFORMANCE.md 2):
  .venv/bin/python tools/gs_conformance_probe7.py list | capture | decode
Outputs (ignored): build/b16/gscap7/<batch>/.
"""
from __future__ import annotations

import os
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
os.environ.setdefault("GSCAP_OUT", str(ROOT / "build/b16/gscap7"))

import gs_conformance_probe3 as P3  # noqa: E402
from gs_conformance import CLAMP, FOGCOL, TEX0, TEX1, Batch, Test  # noqa: E402
from gs_conformance_suite import prim, rand_ct32  # noqa: E402

OFS = (1024.0, 1024.0)
VARIANTS = ("ref", "nofog", "flat", "decal", "ct32", "zalways", "tris", "small", "plain")
TEST_GEQUAL, TEST_ALWAYS = 0x5000D, 0x3000D      # ATE GREATER 0, ZTE 1, ZTST GEQUAL / ALWAYS


def level_strips() -> list[list[dict]]:
    """The vertex lists of p3_stq's level_0..3 (same generator, same seed)."""
    b = P3.batch_stq()
    return [t.meta["prims"][0]["v"] for t in b.tests if t.name.startswith("level_")]


def batch_lvl() -> Batch:
    b = Batch("p7_lvl", "level-class strips with one feature changed at a time: which state sets the STQ grid")
    up = b.add(Test("upload", "T8 + CLUT and CT32 textures (draws nothing)", 64, 32))
    rng = np.random.RandomState(7707)
    idx = rng.randint(0, 256, (16, 16)).astype(np.uint8)
    clut = rand_ct32(rng, 16, 16, amax=0x81)
    ct = rand_ct32(rng, 16, 16, amax=0x81)
    tb, cb, xb = b.tex_alloc(1), b.tex_alloc(1), b.tex_alloc(1)
    up.image(cb, 64, "CT32", 16, 16, clut.tobytes(), label="clut")
    up.image(tb, 128, "T8", 16, 16, idx.tobytes(), label="t8")
    up.image(xb, 64, "CT32", 16, 16, ct.tobytes(), label="ct32")
    b.level_tex = (idx, clut)
    b.textures_addr = ct
    for n, strip in enumerate(level_strips()):
        for var in VARIANTS:
            t = b.add(Test(f"{var}_{n}", f"level strip {n}, variant {var}", 256, 32, zpsm="Z24", ofs=OFS,
                           zclear=0x100))
            plain = var == "plain"
            tfx = 1 if var in ("decal",) or plain else 0
            if var in ("ct32",) or plain:
                tex0 = TEX0(xb, 64, "CT32", 4, 4, 1, tfx)
            else:
                tex0 = TEX0(tb, 128, "T8", 4, 4, 1, tfx, cbp=cb, cpsm="CT32", cld=1)
            t.ad(("TEST_1", TEST_ALWAYS if var == "zalways" or plain else TEST_GEQUAL),
                 ("FOGCOL", FOGCOL(0x30, 0x40, 0x58)), ("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=1, mmin=1)),
                 ("CLAMP_1", CLAMP(0, 0)))
            vs = []
            for v in strip:
                w = dict(x=v["x"], y=v["y"], z=v["z"], qv=v["qv"], rgba=tuple(v["rgba"]), st=tuple(v["st"]),
                         f=v["f"])
                if var == "small":
                    w["st"] = (float(np.float32(w["st"][0] / 4)), float(np.float32(w["st"][1] / 4)))
                vs.append(w)
            iip = 0 if var == "flat" or plain else 1
            fge = 0 if var == "nofog" or plain else 1
            if var == "tris":
                for i in range(len(vs) - 2):
                    prim(t, "tri", vs[i:i + 3], iip=iip, tme=1, fge=fge)
            else:
                prim(t, "tristrip", vs, iip=iip, tme=1, fge=fge)
            t.meta["variant"] = var
    return b


def batch_wrap() -> Batch:
    """p3_stq's perspective triangles and the level strips (S, T / 4) under
    CLAMP and REPEAT: does the wrap mode decide the grid?"""
    b = Batch("p7_wrap", "perspective triangles and level strips under CLAMP and REPEAT")
    up = b.add(Test("upload", "address texture (draws nothing)", 64, 32))
    blk = b.tex_alloc(1)
    b.textures_addr = P3.address_texture()
    up.image(blk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    p3 = P3.batch_stq()
    persp = [(t.name, t.meta["prims"][0]["v"]) for t in p3.tests if t.name.startswith("persp_")]
    strips = level_strips()
    for wrap, (wms, wmt) in (("clamp", (1, 1)), ("repeat", (0, 0))):
        for name, vs0 in persp:
            filt = 1 if "_bil_" in name else 0
            t = b.add(Test(f"{name}_{wrap}", f"p3_stq {name} under {wrap.upper()}", 256, 32, ofs=OFS))
            t.ad(("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 1)), ("TEX1_1", TEX1(mmag=filt, mmin=filt)),
                 ("CLAMP_1", CLAMP(wms, wmt)))
            prim(t, "tri", [dict(x=v["x"], y=v["y"], rgba=tuple(v["rgba"]), qv=v["qv"], st=tuple(v["st"]))
                            for v in vs0], tme=1)
        for n, strip in enumerate(strips):
            t = b.add(Test(f"level_small_{n}_{wrap}", f"level strip {n}, S and T / 4, address texture, {wrap.upper()}",
                           256, 32, ofs=OFS))
            t.ad(("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 1)), ("TEX1_1", TEX1(mmag=1, mmin=1)),
                 ("CLAMP_1", CLAMP(wms, wmt)))
            prim(t, "tristrip", [dict(x=v["x"], y=v["y"], rgba=(0x80,) * 4, qv=v["qv"],
                                      st=(float(np.float32(v["st"][0] / 4)), float(np.float32(v["st"][1] / 4))))
                                 for v in strip], tme=1)
    return b


def batch_z() -> Batch:
    """p7_wrap's CLAMP strips and perspective triangles with Z written or
    not, vertex Z or not, and the alpha test: which of them removes the
    grid (third run, after p7_wrap: the strips need the grid there)."""
    from gs_conformance import TEST
    b = Batch("p7_z", "address-texture strips and triangles with and without Z writes, vertex Z, alpha test")
    up = b.add(Test("upload", "address texture (draws nothing)", 64, 32))
    blk = b.tex_alloc(1)
    b.textures_addr = P3.address_texture()
    up.image(blk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    p3 = P3.batch_stq()
    persp = [(t.name, t.meta["prims"][0]["v"]) for t in p3.tests if t.name.startswith("persp_bil_")]
    strips = level_strips()
    cases = {                     # name: (Z buffer written, vertex Z, TEST)
        "zw": (True, True, TEST(zte=1, ztst="ALWAYS")),
        "zw0": (True, False, TEST(zte=1, ztst="ALWAYS")),
        "zge": (True, True, TEST(zte=1, ztst="GEQUAL")),
        "vz": (False, True, TEST(zte=1, ztst="ALWAYS")),
        "ate": (False, False, TEST(ate=1, atst="GREATER", aref=0, zte=1, ztst="ALWAYS")),
    }
    geoms = [(f"strip{n}", "tristrip", [dict(x=v["x"], y=v["y"], z=v["z"], rgba=(0x80,) * 4, qv=v["qv"],
              st=(float(np.float32(v["st"][0] / 4)), float(np.float32(v["st"][1] / 4)))) for v in strip])
             for n, strip in enumerate(strips)]
    geoms += [(name.replace("persp_bil_", "persp"), "tri",
               [dict(x=v["x"], y=v["y"], z=0x400000 + 0x1234 * i, rgba=tuple(v["rgba"]), qv=v["qv"],
                     st=tuple(v["st"])) for i, v in enumerate(vs0)]) for name, vs0 in persp]
    for case, (zw, vz, test) in cases.items():
        for gname, kind, vs in geoms:
            t = b.add(Test(f"{gname}_{case}", f"{gname} ({kind}), case {case}", 256, 32, ofs=OFS,
                           zpsm="Z24" if zw else None, zclear=0x100))
            t.ad(("TEST_1", test), ("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 1)), ("TEX1_1", TEX1(mmag=1, mmin=1)),
                 ("CLAMP_1", CLAMP(1, 1)))
            prim(t, kind, [dict(v, z=v["z"] if vz else 0) for v in vs], tme=1)
            t.meta["case"] = case
    return b


def batch_zc() -> Batch:
    """Constant and planar vertex Z (fourth run, after p7_z: with vertex Z
    the grid is gone, with Z 0 it is there, whether Z is written or not)."""
    b = Batch("p7_zc", "address-texture strips and triangles: constant Z, Z varying by 1, Z planes in x or y only")
    up = b.add(Test("upload", "address texture (draws nothing)", 64, 32))
    blk = b.tex_alloc(1)
    b.textures_addr = P3.address_texture()
    up.image(blk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    p3 = P3.batch_stq()
    persp = [(t.name, t.meta["prims"][0]["v"]) for t in p3.tests if t.name.startswith("persp_bil_")]
    strips = level_strips()
    geoms = [(f"strip{n}", "tristrip", [dict(x=v["x"], y=v["y"], rgba=(0x80,) * 4, qv=v["qv"],
              st=(float(np.float32(v["st"][0] / 4)), float(np.float32(v["st"][1] / 4)))) for v in strip])
             for n, strip in enumerate(strips)]
    geoms += [(name.replace("persp_bil_", "persp"), "tri",
               [dict(x=v["x"], y=v["y"], rgba=tuple(v["rgba"]), qv=v["qv"], st=tuple(v["st"])) for v in vs0])
              for name, vs0 in persp]
    cases = {
        "c123456": lambda i, v: 0x123456,
        "c1": lambda i, v: 1,
        "last1": lambda i, v: 1 if i == 2 else 0,
        "yplane": lambda i, v: 0x400000 + int(round(v["y"] * 16)) * 64,
        "xplane": lambda i, v: 0x400000 + int(round(v["x"] * 16)) * 64,
    }
    for case, zf in cases.items():
        for gname, kind, vs in geoms:
            t = b.add(Test(f"{gname}_{case}", f"{gname} ({kind}), vertex Z {case}", 256, 32, ofs=OFS))
            t.ad(("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 1)), ("TEX1_1", TEX1(mmag=1, mmin=1)),
                 ("CLAMP_1", CLAMP(1, 1)))
            prim(t, kind, [dict(v, z=zf(i, v)) for i, v in enumerate(vs)], tme=1)
            t.meta["case"] = case
    return b


def batch_scope() -> Batch:
    """Over what the constant-Z decision is taken (fifth run, after p7_zc:
    one strip vertex with Z 1 removes the grid from every triangle of the
    strip, also from triangles whose own three Z are equal)."""
    from gs_conformance import PRIM
    b = Batch("p7_scope", "the scope of the constant-Z decision: triangle lists, two strips, state writes between")
    up = b.add(Test("upload", "address texture (draws nothing)", 64, 32))
    blk = b.tex_alloc(1)
    b.textures_addr = P3.address_texture()
    up.image(blk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    strips = level_strips()
    ZC, ZV = 0x2000, (0x2000, 0x2400, 0x2800)

    def verts(strip, zs):
        return [dict(x=v["x"], y=v["y"], rgba=(0x80,) * 4, qv=v["qv"], z=z,
                     st=(float(np.float32(v["st"][0] / 4)), float(np.float32(v["st"][1] / 4))))
                for v, z in zip(strip, zs)]
    for n, strip in enumerate(strips[:2]):
        nv = len(strip)
        def setup(t):
            t.ad(("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 1)), ("TEX1_1", TEX1(mmag=1, mmin=1)),
                 ("CLAMP_1", CLAMP(1, 1)))
        # triangle lists: the strip's triangles as separate TRI primitives, one of them with varying Z
        for which, label in ((0, "first"), (nv - 3, "last")):
            t = b.add(Test(f"list{n}_var_{label}", f"strip {n} as a TRI list, only the {label} triangle's Z varies",
                           256, 32, ofs=OFS))
            setup(t)
            for i in range(nv - 2):
                prim(t, "tri", verts(strip[i:i + 3], ZV if i == which else (ZC,) * 3), tme=1)
            t.meta["case"] = f"list_var_{label}"
        # a strip whose last vertex alone has another Z (only the last triangle varies)
        t = b.add(Test(f"strip{n}_var_lastv", f"strip {n}, only the last vertex's Z differs", 256, 32, ofs=OFS))
        setup(t)
        prim(t, "tristrip", verts(strip, [ZC] * (nv - 1) + [ZC + 0x400]), tme=1)
        # two strips (first half constant, second half varying) with nothing / a PRIM rewrite / state writes between
        half = nv // 2 + 1
        a_v = verts(strip[:half], [ZC] * half)
        b_v = verts(strip[half - 2:], [ZC + 0x100 * k for k in range(nv - half + 2)])
        for between in ("none", "prim", "tex1", "fogcol"):
            t = b.add(Test(f"two{n}_{between}", f"strip {n} split in two: constant-Z strip, then varying-Z strip; "
                           f"between: {between}", 256, 32, ofs=OFS))
            setup(t)
            prim(t, "tristrip", a_v, tme=1)
            if between == "tex1":
                t.ad(("TEX1_1", TEX1(mmag=1, mmin=1)))
            elif between == "fogcol":
                t.ad(("FOGCOL", FOGCOL(1, 2, 3)))
            if between == "none":
                from gs_conformance_suite import vtx
                w = []
                for v in b_v[2:]:
                    w += vtx(t, **v)
                # continue the SAME strip: the second strip's first two vertices are the first's last two
                t.ad(*w)
                t.meta.setdefault("prims", [])[-1]["v"] += [{k: (list(x) if isinstance(x, tuple) else x)
                                                              for k, x in v.items()} for v in b_v[2:]]
            else:
                prim(t, "tristrip", b_v, tme=1)
            t.meta["case"] = f"two_{between}"
        # the reverse: varying strip first, then the constant one, state write between
        t = b.add(Test(f"rev{n}_fogcol", f"strip {n} split in two: varying-Z strip, FOGCOL write, constant strip",
                       256, 32, ofs=OFS))
        setup(t)
        prim(t, "tristrip", verts(strip[:half], [ZC + 0x100 * k for k in range(half)]), tme=1)
        t.ad(("FOGCOL", FOGCOL(1, 2, 3)))
        prim(t, "tristrip", verts(strip[half - 2:], [ZC] * (nv - half + 2)), tme=1)
        t.meta["case"] = "rev_fogcol"
    return b


def batch_flush() -> Batch:
    """Which writes end the span the constant-Z decision covers (sixth run,
    after p7_scope: PRIM, TEX1 (same value) and FOGCOL writes do not). A
    varying-Z strip, one write (or a transfer), then a constant-Z strip; if
    the write ends the span, the second strip gets the grid."""
    from gs_conformance import ALPHA, FRAME, SCISSOR, TEST, XYOFFSET, PRIM
    b = Batch("p7_flush", "varying-Z strip, one register write or transfer, constant-Z strip")
    up = b.add(Test("upload", "address texture (draws nothing)", 64, 32))
    blk = b.tex_alloc(1)
    spare = b.tex_alloc(1)
    b.textures_addr = P3.address_texture()
    up.image(blk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    strips = level_strips()
    ZC = 0x2000

    def verts(strip, zs):
        return [dict(x=v["x"], y=v["y"], rgba=(0x80,) * 4, qv=v["qv"], z=z,
                     st=(float(np.float32(v["st"][0] / 4)), float(np.float32(v["st"][1] / 4))))
                for v, z in zip(strip, zs)]
    tex0 = TEX0(blk, 64, "CT32", 4, 4, 1, 1)
    for n, strip in enumerate(strips[:2]):
        nv = len(strip)
        half = nv // 2 + 1
        a_v = verts(strip[:half], [ZC + 0x100 * k for k in range(half)])
        b_v = verts(strip[half - 2:], [ZC] * (nv - half + 2))
        cases = {
            "none": [],
            "tex0_same": [("TEX0_1", tex0)],
            "tex0_tfx": [("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 0))],
            "clamp": [("CLAMP_1", CLAMP(0, 0))],
            "test": [("TEST_1", TEST(ate=1, atst="ALWAYS", zte=1, ztst="ALWAYS"))],
            "alpha": [("ALPHA_1", ALPHA(0, 1, 2, 2, 0x40))],
            "colclamp": [("COLCLAMP", 0)],
            "dthe": [("DTHE", 1)],
            "xyoffset_same": [("XYOFFSET_1", XYOFFSET(*OFS))],
            "scissor_same": [("SCISSOR_1", SCISSOR(0, 255, 0, 31))],
            "texflush": [("TEXFLUSH", 0)],
            "prim_iip": "iip",
            "prim_list": "list",
            "transfer": "transfer",
        }
        for case, what in cases.items():
            t = b.add(Test(f"f{n}_{case}", f"strip {n}: varying-Z strip, {case}, constant-Z strip", 256, 32, ofs=OFS))
            t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(1, 1)))
            prim(t, "tristrip", a_v, tme=1)
            if isinstance(what, list):
                if what:
                    t.ad(*what)
                prim(t, "tristrip", b_v, tme=1)
            elif what == "iip":
                prim(t, "tristrip", b_v, tme=1, iip=1)
            elif what == "list":
                for i in range(len(b_v) - 2):
                    prim(t, "tri", b_v[i:i + 3], tme=1)
            else:
                t.image(spare, 64, "CT32", 8, 8, np.full(64, 0x11223344, dtype="<u4").tobytes(), label="spare")
                prim(t, "tristrip", b_v, tme=1)
            t.meta["case"] = case
    return b


def batches():
    return [batch_lvl(), batch_wrap(), batch_z(), batch_zc(), batch_scope(), batch_flush()]


def main() -> None:
    P3.batches = batches
    P3.OUT = Path(os.environ["GSCAP_OUT"])
    P3.main()


if __name__ == "__main__":
    main()
