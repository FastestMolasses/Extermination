#!/usr/bin/env python3
"""gs_conformance_probe3.py - follow-up conformance batches for the port's
CPU GS model (extermination-port src/gs/em_gs_raster.c, docs/GS_EXACT.md):
the arithmetic of attribute interpolation inside triangles and lines.

Same harness, rules and clean room as tools/gs_conformance.py
(docs/GS_CONFORMANCE.md): designed GIF packets drawn by PCSX2's SOFTWARE
renderer, recorded pixel for pixel; every test resets the whole GS state
and clears its own buffers; nothing disc-derived; no emulator source read.

Design (why each batch looks as it does):
- p3_start: colour and fog-weight row starts. Every triangle has two
  vertices on one row 1024 pixels apart whose colours differ by exactly
  2k, so dC/dx = k/512 is exact and per-pixel stepping adds no error of its
  own. Along a 512-pixel row the floored value then crosses integers at
  positions that give the row's start value to 1/512 exactly. The left
  edges are vertical, sloped both ways, off-screen (scissor-clipped starts)
  and under an inner SCISSOR. The fog tests put the weight F on the same
  geometry, seen through FOGCOL: R = 0 with FOGCOL R 255 gives 255 - F,
  G = 255 with FOGCOL G 0 gives 255 * F >> 8. The MODULATE tests put the
  colour on a constant 0xFF texture (Ct * Cf >> 7). Gouraud lines last.
- p3_z: Z24 / Z32 / Z16 interpolation with vertical, sloped and clipped
  left edges, exact and fractional gradients, small and large values, and
  the same window-space triangle under three XYOFFSETs.
- p3_stq: texture coordinates through an address texture (texel (u, v) =
  R 16u, G 16v; CLAMP): bilinear then shows floor((u - 1/2) * 16) and
  nearest floor(u) directly; perspective triangles (Q per vertex), sprites
  with Q other than 1, UV triangles.

Usage (macOS arm64, decomp root; Renderer = 13 manual switch as
docs/GS_CONFORMANCE.md section 2):
  .venv/bin/python tools/gs_conformance_probe3.py list
  .venv/bin/python tools/gs_conformance_probe3.py capture [BATCH ...]
  .venv/bin/python tools/gs_conformance_probe3.py decode  [BATCH ...]
Outputs (ignored): build/b16/gscap3/<batch>/ (batch.json, packet.bin,
inputs.npz, kick.json, snap/gs.bin, <test>.npz, decode.json).
"""
from __future__ import annotations

import argparse
import json
import os
import shutil
import sys
import time
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
os.environ.setdefault("GSCAP_OUT", str(ROOT / "build/b16/gscap3"))

import gs_conformance as G  # noqa: E402
from gs_conformance import (ALPHA, CLAMP, FOGCOL, FRAME, SCISSOR, TEST, TEX0, TEX1, XYOFFSET,  # noqa: E402
                            ZBUF, Batch, Test)
from gs_conformance_suite import prim, rand_ct32  # noqa: E402

OUT = Path(os.environ["GSCAP_OUT"])
MAPS = ROOT / "build/b16/gscap/layout/maps.npz"      # the layout batch's measured maps
OFS = (1024.0, 1024.0)


def tri(t: Test, pts, cols, z=None, f=None, **flags):
    vs = []
    for i, (x, y) in enumerate(pts):
        v = dict(x=x, y=y, rgba=tuple(cols[i]))
        if z is not None:
            v["z"] = z[i]
        if f is not None:
            v["f"] = f[i]
        vs.append(v)
    prim(t, "tri", vs, **flags)


def exact_plane(a, dxk, top, c_c):
    """Colours for A, B = A + (1024, 0), C: B = A + 2k per channel."""
    ca = list(a)
    cb = [ca[i] + dxk[i] for i in range(4)]
    cc = [ca[i] + c_c[i] for i in range(4)]
    for c in (ca, cb, cc):
        assert all(0 <= v <= 255 for v in c), (ca, cb, cc)
    return [ca, cb, cc]


# geometry sets: (A, C) with B = A + (1024, 0); window coordinates
GEOM = {
    "v1000": ((0, 0), (0, 1000)), "v1024": ((0, 0), (0, 1024)), "v999": ((0, 0), (0, 999)),
    "v64": ((0, 0), (0, 64)), "v37": ((0, 0), (0, 37.5)), "v333": ((0, 0), (0, 333.3125)),
    "vhalf": ((0, 0.5), (0, 1000.5)), "vneg": ((0, -0.3125), (0, 700.6875)),
    "s1": ((3.25, -1.5), (40.75, 120.0)), "s2": ((200.5, -2.0625), (10.4375, 90.25)),
    "s3": ((50.125, 0.25), (55.0, 300.0)), "s4": ((120.0, -3.0), (-60.0, 45.0)),
    "s5": ((7.8125, -0.9375), (407.8125, 33.0625)), "s6": ((300.0625, -1.0), (0.0625, 29.0)),
    "s7": ((0.5, 0.5), (31.5, 31.5)), "s8": ((16.0, 0.0), (48.0, 32.0)),
    "c1": ((-100.25, -5.0), (80.5, 60.0)), "c2": ((-300.0, -2.0), (-250.0, 200.0)),
}
# bottom-flat triangles: (C top, A bottom-left), B = A + (1024, 0)
GEOM_UP = {"u1": ((30.3125, -10.0), (-5.0, 40.0)), "u2": ((250.5, -3.5), (100.75, 60.25))}


def geom_pts(key):
    if key in GEOM:
        (ax, ay), (cx, cy) = GEOM[key]
        return [(ax, ay), (ax + 1024, ay), (cx, cy)]
    (cx, cy), (ax, ay) = GEOM_UP[key]
    return [(ax, ay), (ax + 1024, ay), (cx, cy)]


BASE = (100, 150, 60, 20)
DXK = (2, -2, 4, 8)                      # dC/dx = 1/512, -1/512, 2/512, 4/512
CC = {
    "v1000": (7, 13, -29, 57), "v1024": (1, 3, 5, 9), "v999": (100, -120, 150, 200),
    "v64": (3, -7, 11, 17), "v37": (1, -1, 2, 3), "v333": (61, -77, 33, 99), "vhalf": (7, 13, -29, 57),
    "vneg": (55, -44, 33, 22), "s1": (5, -9, 17, 33), "s2": (-13, 21, -8, 40), "s3": (60, -80, 70, 150),
    "s4": (9, 4, -6, 11), "s5": (2, -3, 5, 7), "s6": (1, 2, -1, 3), "s7": (4, -4, 8, 16), "s8": (1, 1, 1, 1),
    "c1": (13, -17, 19, 23), "c2": (50, -50, 50, 50), "u1": (-7, 9, -11, 13), "u2": (21, -19, 17, 15),
}


def batch_start() -> Batch:
    b = Batch("p3_start", "colour / F row starts with exact dC/dx = k/512 over 512-pixel rows; "
                          "Gouraud lines")
    for key in list(GEOM) + list(GEOM_UP):
        t = b.add(Test(f"col_{key}", f"colour, geometry {key}", 512, 32, ofs=OFS))
        tri(t, geom_pts(key), exact_plane(BASE, DXK, None, CC[key]), iip=1)
        t.meta["geom"] = key
    for key, sc in (("s1", (100, 511, 4, 31)), ("s3", (64, 511, 0, 31)), ("c1", (37, 400, 0, 31))):
        t = b.add(Test(f"col_{key}_sc", f"colour, geometry {key}, SCISSOR {sc}", 512, 32, ofs=OFS))
        t.ad(("SCISSOR_1", SCISSOR(*sc)))
        tri(t, geom_pts(key), exact_plane(BASE, DXK, None, CC[key]), iip=1)
        t.meta.update(geom=key, scissor=sc)
    # fog weight: R = 0 / FOGCOL 255 -> 255 - F; G = 255 / FOGCOL 0; B = 128 / FOGCOL 0
    for key, fa, dk, fc in (("v1000", 100, 2, 37), ("v64", 20, 2, 29), ("s1", 60, 2, -41), ("s2", 150, -2, 77),
                            ("s4", 90, 4, 13), ("c1", 40, 2, 101), ("v333", 30, 8, 150), ("u2", 200, -4, -90)):
        t = b.add(Test(f"fog_{key}_{dk}", f"fog weight, geometry {key}, F_B = F_A + {dk}", 512, 32, ofs=OFS))
        t.ad(("FOGCOL", FOGCOL(255, 0, 0)))
        cols = [(0, 255, 128, 0x80)] * 3
        tri(t, geom_pts(key), cols, f=[fa, fa + dk, fa + fc], iip=1, fge=1)
        t.meta.update(geom=key, f=[fa, fa + dk, fa + fc])
    # MODULATE: a constant 0xFF texture, colour on the exact planes
    up = b.add(Test("upload", "constant textures (draws nothing)", 64, 32))
    blk = b.tex_alloc(1)
    up.image(blk, 64, "CT32", 16, 16, np.full(256, 0xFFFFFFFF, dtype="<u4").tobytes(), label="white")
    for key in ("v1000", "s1", "s4", "c1"):
        t = b.add(Test(f"mod_{key}", f"MODULATE on 0xFF texels, geometry {key}", 512, 32, ofs=OFS))
        t.ad(("TEX0_1", TEX0(blk, 64, "CT32", 4, 4, 1, 0)), ("TEX1_1", TEX1()), ("CLAMP_1", CLAMP(1, 1)))
        pts = geom_pts(key)
        cols = exact_plane((40, 90, 20, 100), DXK, None, (7, -9, 11, 3))
        vs = [dict(x=x, y=y, rgba=tuple(c), uv=(8.0, 8.0)) for (x, y), c in zip(pts, cols)]
        prim(t, "tri", vs, iip=1, tme=1, fst=1)
        t.meta["geom"] = key
    # Gouraud lines: x-major (long) and y-major, colour differing by small amounts
    t = b.add(Test("lines_x", "x-major Gouraud lines", 512, 32, ofs=OFS))
    lines = [((0.5, 1.25), (511.5, 3.75), (10, 20, 30, 40), (12, 17, 31, 44)),
             ((3.3125, 6.0), (509.0625, 7.0), (200, 100, 50, 0), (201, 97, 58, 255)),
             ((510.0, 11.5), (1.75, 12.0), (0, 0, 0, 0), (255, 128, 64, 32)),
             ((20.0625, 16.9375), (400.5, 21.4375), (77, 66, 55, 44), (80, 60, 59, 40)),
             ((0.0, 26.0), (512.0, 26.0), (0, 255, 1, 2), (2, 253, 5, 10))]
    for a, c, ca, cc in lines:
        prim(t, "line", [dict(x=a[0], y=a[1], rgba=ca), dict(x=c[0], y=c[1], rgba=cc)], iip=1)
    t = b.add(Test("lines_y", "y-major Gouraud lines", 512, 32, ofs=OFS))
    for k in range(12):
        x0 = 10 + 40 * k + (k % 5) * 0.1875
        a, c = (x0, 0.5 + (k % 3) * 0.25), (x0 + (k - 6) * 1.3125, 31.25 - (k % 4) * 0.5)
        if k % 2:
            a, c = c, a
        prim(t, "line", [dict(x=a[0], y=a[1], rgba=(10 * k, 255 - 9 * k, 3 * k, 128)),
                         dict(x=c[0], y=c[1], rgba=(10 * k + 5 + k, 255 - 9 * k - 7, 3 * k + 31, 0))], iip=1)
    return b


ZGEOM = ["v1000", "v64", "s1", "s2", "s4", "s5", "c1", "u1", "u2"]


def batch_z() -> Batch:
    b = Batch("p3_z", "Z interpolation: Z24 / Z32 / Z16, exact and fractional gradients, three XYOFFSETs")
    zsets = {
        # (zA, dz over 1024 px (B - A), dz at C - zA)
        "int_small": (0x001000, 1024 * 37, 5000), "frac_small": (0x002345, 1024 * 37 + 333, 77777),
        "frac_large": (0xE00000, 1024 * 900 + 777, -1234567), "neg": (0xF00000, -1024 * 1500 - 13, 999),
        "yonly": (0x400000, 0, 6543210),
    }
    for zname, (za, dzb, dzc) in zsets.items():
        for key in (ZGEOM if zname in ("frac_small", "frac_large") else ["v1000", "s1", "c1"]):
            t = b.add(Test(f"z24_{zname}_{key}", f"Z24 {zname}, geometry {key}", 256, 32, zpsm="Z24", ofs=OFS))
            pts = geom_pts(key)
            zs = [za, za + dzb, za + dzc]
            assert all(0 <= z <= 0xFFFFFF for z in zs), (zname, zs)
            tri(t, pts, [(9, 9, 9, 0x80)] * 3, z=zs, iip=1)
            t.meta.update(geom=key, z=zs)
    # the same window triangle under three XYOFFSETs (colour and Z)
    for ofs in ((1024.0, 1024.0), (1792.0, 1936.0), (2048.0, 2048.0), (1792.0, 1936.5)):
        for key in ("s1", "s4"):
            t = b.add(Test(f"ofs_{int(ofs[0])}_{ofs[1]}_{key}", f"XYOFFSET {ofs}, geometry {key}, Z24 + colour",
                           256, 32, zpsm="Z24", ofs=ofs))
            zs = [0x123456, 0x123456 + 1024 * 91 + 555, 0x123456 + 432109]
            tri(t, geom_pts(key), exact_plane(BASE, DXK, None, CC[key]), z=zs, iip=1)
            t.meta.update(geom=key, z=zs)
    for key, zs in (("v1000", [0x10000000, 0x10000000 + 1024 * 70001 + 77, 0x10000000 + 123456789]),
                    ("s1", [0x00001234, 0x00001234 + 1024 * 5 + 3, 0x00001234 + 99]),
                    ("s4", [0xF0000000, 0xF0000000 - 1024 * 100003 - 9, 0xF0000000 - 7654321]),
                    ("c1", [0x7FFFFFFF, 0x7FFFFFFF + 1024 * 12345 + 1, 0x7FFFFFFF - 12345678])):
        t = b.add(Test(f"z32_{key}", f"Z32, geometry {key}", 256, 32, zpsm="Z32", ofs=OFS))
        tri(t, geom_pts(key), [(9, 9, 9, 0x80)] * 3, z=zs, iip=1)
        t.meta.update(geom=key, z=zs)
    for key, zs in (("v1000", [0x1000, 0x1000 + 1024 * 7 + 5, 0x1000 + 3000]),
                    ("s1", [0x8000, 0x8000 + 1024 * 20 + 101, 0x8000 - 9999]),
                    ("c1", [0xFF00, 0xFF00 - 1024 * 30 - 1, 0xFF00 - 30000])):
        t = b.add(Test(f"z16_{key}", f"Z16, geometry {key}", 256, 32, zpsm="Z16", ofs=OFS))
        tri(t, geom_pts(key), [(9, 9, 9, 0x80)] * 3, z=zs, iip=1)
        t.meta.update(geom=key, z=zs)
    # lines with Z
    t = b.add(Test("zlines", "Z along lines (Z24)", 256, 32, zpsm="Z24", ofs=OFS))
    for k, (a, c, za, zc) in enumerate([((0.5, 1.25), (255.5, 3.75), 0x100000, 0x100000 + 3333333),
                                        ((250.0, 8.5), (3.25, 9.0), 0x800000, 0x800000 - 777777),
                                        ((5.0, 12.0), (15.5, 30.75), 0x200000, 0x200000 + 99999),
                                        ((100.0625, 14.9375), (220.5, 19.4375), 0xABCDEF, 0x123456)]):
        prim(t, "line", [dict(x=a[0], y=a[1], z=za, rgba=(9, 9, 9, 0x80)),
                         dict(x=c[0], y=c[1], z=zc, rgba=(9, 9, 9, 0x80))], iip=1)
    return b


def address_texture() -> np.ndarray:
    v, u = np.mgrid[0:16, 0:16]
    return (u * 16 | (v * 16) << 8 | 0x33 << 16 | 0x80 << 24).astype("<u4")


def batch_stq() -> Batch:
    b = Batch("p3_stq", "texture coordinates through an address texture: perspective STQ, sprites with Q, UV")
    up = b.add(Test("upload", "address texture (draws nothing)", 64, 32))
    blk = b.tex_alloc(1)
    b.textures_addr = address_texture()
    up.image(blk, 64, "CT32", 16, 16, b.textures_addr.tobytes(), label="address")
    tex0 = TEX0(blk, 64, "CT32", 4, 4, 1, 1)                      # DECAL, TCC 1
    rng = np.random.RandomState(3301)
    for filt in (1, 0):
        for n in range(6):
            t = b.add(Test(f"persp_{'bil' if filt else 'near'}_{n}", "perspective triangle, address texture",
                           256, 32, ofs=OFS))
            t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=filt, mmin=filt)), ("CLAMP_1", CLAMP(1, 1)))
            box = [((-60, 0), (-30, -2)), ((260, 360), (-20, 10)), ((-20, 280), (36, 90))]
            pts = [(float(np.round(rng.uniform(*bx) * 16) / 16), float(np.round(rng.uniform(*by) * 16) / 16))
                   for bx, by in box]
            qs = [float(np.float32(rng.uniform(0.2, 2.5))) for _ in range(3)]
            sts = [(float(np.float32(rng.uniform(0.03, 0.97))), float(np.float32(rng.uniform(0.03, 0.97)))) for _ in range(3)]
            vs = [dict(x=x, y=y, rgba=(0x80,) * 4, qv=qq, st=(s * qq, tt * qq)) for (x, y), qq, (s, tt) in zip(pts, qs, sts)]
            prim(t, "tri", vs, tme=1)
            t.meta["q"] = qs
    for filt in (1, 0):
        t = b.add(Test(f"sprite_q_{'bil' if filt else 'near'}", "sprites with Q other than 1", 256, 32, ofs=OFS))
        t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=filt, mmin=filt)), ("CLAMP_1", CLAMP(1, 1)))
        for k, (q0, q1) in enumerate([(0.5, 0.5), (2.0, 0.25), (0.75, 1.5), (1.0, 3.0)]):
            x0 = 64 * k + 1.5
            vs = [dict(x=x0, y=0.5, rgba=(0x80,) * 4, qv=q0, st=(0.05 * q0, 0.1 * q0)),
                  dict(x=x0 + 60, y=31.5, rgba=(0x80,) * 4, qv=q1, st=(0.95 * q1, 0.9 * q1))]
            prim(t, "sprite", vs, tme=1)
    for filt in (1, 0):
        t = b.add(Test(f"uv_tri_{'bil' if filt else 'near'}", "UV triangles (FST 1)", 256, 32, ofs=OFS))
        t.ad(("TEX0_1", tex0), ("TEX1_1", TEX1(mmag=filt, mmin=filt)), ("CLAMP_1", CLAMP(1, 1)))
        prim(t, "tri", [dict(x=-3.25, y=-2.5, rgba=(0x80,) * 4, uv=(0.5, 0.5)),
                        dict(x=250.75, y=4.125, rgba=(0x80,) * 4, uv=(15.4375, 1.0625)),
                        dict(x=20.0625, y=40.0, rgba=(0x80,) * 4, uv=(2.3125, 15.5))], tme=1, fst=1)
    # level-class composites for validation (T8 + CLUT, MODULATE, fog, Z24 GEQUAL)
    idx = rng.randint(0, 256, (16, 16)).astype(np.uint8)
    clut = rand_ct32(rng, 16, 16, amax=0x81)
    tb, cb = b.tex_alloc(1), b.tex_alloc(1)
    up.image(cb, 64, "CT32", 16, 16, clut.tobytes(), label="clut")
    up.image(tb, 128, "T8", 16, 16, idx.tobytes(), label="t8")
    b.level_tex = (idx, clut)
    for n in range(4):
        t = b.add(Test(f"level_{n}", "level class: tristrip IIP TME FGE, T8 CLUT MODULATE, fog, Z GEQUAL", 256, 32,
                       zpsm="Z24", ofs=OFS, zclear=0x100))
        t.ad(("TEST_1", 0x5000D), ("FOGCOL", FOGCOL(0x30, 0x40, 0x58)),
             ("TEX0_1", TEX0(tb, 128, "T8", 4, 4, 1, 0, cbp=cb, cpsm="CT32", cld=1)),
             ("TEX1_1", TEX1(mmag=1, mmin=1)), ("CLAMP_1", CLAMP(0, 0)))
        vs = []
        for i in range(8):
            x = float(np.round((i // 2) * 80 - 20 + rng.uniform(-10, 10)) * 1.0) + rng.randint(0, 16) / 16
            y = float((i % 2) * 40 - 4 + rng.randint(0, 16) / 16)
            qq = float(np.float32(rng.uniform(0.3, 1.5)))
            vs.append(dict(x=x, y=y, z=int(rng.randint(0x1000, 0x800000)), qv=qq,
                           rgba=tuple(int(c) for c in rng.randint(0, 256, 3)) + (0x80,),
                           st=(float(np.float32(rng.uniform(0, 3))) * qq, float(np.float32(rng.uniform(0, 3))) * qq),
                           f=int(rng.randint(0, 256))))
        prim(t, "tristrip", vs, iip=1, tme=1, fge=1)
    return b


def batches():
    return [batch_start(), batch_z(), batch_stq()]


def save_inputs(b: Batch, out: Path, pkt: bytes, doc: dict) -> None:
    (out / "packet.bin").write_bytes(pkt)
    (out / "batch.json").write_text(json.dumps(doc, indent=1) + "\n")
    extra = {}
    if hasattr(b, "textures_addr"):
        extra["tex_address"] = b.textures_addr
    if hasattr(b, "level_tex"):
        extra["level_idx"], extra["level_clut"] = b.level_tex
    if extra:
        np.savez_compressed(out / "inputs.npz", **extra)


def capture(names: list[str]) -> None:
    G.require_software_renderer()
    todo = [b for b in batches() if not names or b.name in names]
    OUT.mkdir(parents=True, exist_ok=True)
    try:
        with G.open_session(OUT / "logs") as s:
            for b in todo:
                t0 = time.monotonic()
                out = OUT / b.name
                if out.exists():
                    shutil.rmtree(out)
                out.mkdir(parents=True)
                pkt, doc = b.build()
                save_inputs(b, out, pkt, doc)
                G._run_to(s, G.VSYNC_WAIT)
                G._run_to(s, G.VSYNC_ISR)
                at = {"counter": s.u32(G.FRAME_COUNTER), "pc": hex(G._pc(s))}
                ev = G.kick_and_wait(s, pkt)
                done = int(ev["after"]["D2_CHCR"], 16) & 0x100 == 0
                snap = G.snapshot(s, out / "snap")
                G.restore_after_kick(s, ev)
                rec = {"at": at, "dma_done": done, "before": ev["before"], "after": ev["after"],
                       "seconds_in_kick": ev["seconds"], "tags": ev["tags"], "packet_qw": ev["packet_qw"],
                       "snapshot": snap, "renderer": G.ini_renderer(), "seconds": round(time.monotonic() - t0, 1)}
                (out / "kick.json").write_text(json.dumps(rec, indent=1) + "\n")
                print(b.name, "tests", len(b.tests), "qw", ev["packet_qw"], "dma_done", done,
                      "counter", at["counter"], rec["seconds"], "s", flush=True)
    finally:
        print("no emulator process left:", G.no_emulator_left())
        print(G.RESTORE_HINT)


def decode(names: list[str]) -> None:
    maps = dict(np.load(MAPS))
    for b in batches():
        if names and b.name not in names:
            continue
        out = OUT / b.name
        if not (out / "snap/gs.bin").exists():
            print(b.name, "not captured")
            continue
        doc = json.loads((out / "batch.json").read_text())
        vram = G.vram_of((out / "snap/gs.bin").read_bytes())
        fence = G.decode_buffer(vram, doc["fence"]["page"], 64, 32, "CT32", maps)
        res = {"batch": b.name, "fence_ok": bool((fence == int(doc["fence"]["word"], 16)).all()), "tests": {}}
        for t in doc["tests"]:
            arrays = {"color": G.decode_buffer(vram, t["fbp"], t["w"], t["h"], t["psm"], maps)}
            if t["zpsm"]:
                arrays["z"] = G.decode_buffer(vram, t["zbp"], t["w"], t["h"], t["zpsm"], maps)
            np.savez_compressed(out / f"{t['name']}.npz", **arrays)
            res["tests"][t["name"]] = {"distinct": int(len(np.unique(arrays["color"])))}
        (out / "decode.json").write_text(json.dumps(res, indent=1) + "\n")
        print(b.name, "fence_ok", res["fence_ok"], "tests", len(doc["tests"]))


def list_batches() -> None:
    for b in batches():
        pkt, doc = b.build()
        json.dumps(doc)
        print(f"{b.name:10s} tests {len(b.tests):3d}  pages {b.next_page:3d}  tex pages "
              f"{b.tex_next - Batch.TEX_PAGE0:3d}  packet {len(pkt) // 16} qw  - {b.desc}")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    for c in ("capture", "decode"):
        p = sub.add_parser(c)
        p.add_argument("batches", nargs="*")
    sub.add_parser("list")
    a = ap.parse_args()
    {"capture": lambda: capture(a.batches), "decode": lambda: decode(a.batches), "list": list_batches}[a.cmd]()


if __name__ == "__main__":
    main()
