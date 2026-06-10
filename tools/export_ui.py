#!/usr/bin/env python3
"""export_ui.py - extract the status-screen DECOR textures to assets/ui.emui.

Original tooling for the Extermination decompilation project. Ingests only a
GS-VRAM dump the user produced locally from their own copy of the game (via
tools/parse_pcsx2_state.py on a PCSX2 save state); nothing disc-derived is
redistributed. Output lands in git-ignored asset directories.

WHERE THE UI TEXTURES LIVE (docs/FINDINGS.md "STATUS SCREEN UI TEXTURES")
-------------------------------------------------------------------------
The status-screen hub's textured decor sprites are PSMT4 textures RESIDENT
in GS VRAM (TBW 8 = one 512-texel-wide UI buffer), each with a 16-entry
CSM1 CT32 CLUT, stored VERTICALLY FLIPPED (the engine's sprite UVs flip V
on draw). They are present in an ordinary gameplay save state (uploaded by
the boot-time asset chunk path, not re-streamed per frame), so a plain
in-game state dump is a sufficient source. TEX0 tokens come from the hub
master drawer func_00209DF0's inline constants:

  sprite      TEX0 (lo_hi)          TBP    CBP    size     canvas pos
  title       0x9D421E50_200453A5   0x1E50 0x229D 128x64   (16,0)    "MAIN"
  legend      0xDD421D40_20045505   0x1D40 0x22A8 128x128  (0,320)   pad legend
  icon TL     0x55422186_20045EC5   0x2186 0x22F6 32x32    (11,304)
  icon TR     0x554221F0_20045EC5   0x21F0 0x22F6 32x32    (484,304)
  icon BL     0x55422192_20045EC5   0x2192 0x22F6 32x32    (417,10)
  icon BR     0x554221F4_20045EC5   0x21F4 0x22F6 32x32    (417,406)
  bullet      0x55422196_20045185   0x2196 0x228C 32x32    (16,262) dw/dh 24
              (func_00209860's SPR4 ammo icon — GS-scaled to 24x24 on draw)

("legend" is the 128x128 quad the session-25 audit provisionally called
the "portrait" - the texture is the button legend: triangle EXIT, circle
BACK, cross OK. The audit's "help panel" is a FLAT translucent rect, not a
texture; it needs no asset.)

OUTPUT FORMAT .emui v1 (little-endian) - keep in sync with the port loader
(extermination-port/src/game/em_hud.c, ui_parse):

  off  size
  0    4    magic "EMUI"
  4    4    u32 version = 1
  8    4    u32 sheet_w
  12   4    u32 sheet_h
  16   4    u32 sprite_count
  20   16*sprite_count  per sprite:
              u16 u, v               (texel position in the sheet)
              u16 w, h               (texel size)
              s16 x, y               (draw position on the 512x448 status
                                      canvas - the audited anchors above)
              u16 dw, dh             (draw size on the canvas; 1:1 = w,h
                                      for all but the bullet icon, which
                                      GS-scales its 32x32 texture to 24x24)
  ...  sheet_w*sheet_h*4  RGBA8 sheet, rows top-down, SCREEN-oriented
              (the VRAM v-flip is undone at export)

The port draws every record as one textured overlay sprite at (x,y,dw,dh)
sampling (u,v)-(u+w,v+h), white modulate - so the layout data travels with
the asset and em_hud stays free of per-sprite position tables.

USAGE (macOS arm64, repo root)
    python3 tools/parse_pcsx2_state.py <state.p2s> --out scratch/state01
    python3 tools/export_ui.py --gs scratch/state01/gs.bin \
        --out ../extermination-port/assets/ui.emui
"""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))

from clut import write_png_rgba          # noqa: E402
from clut_pair import read_clut16_rgba, read_psmt4  # noqa: E402

# PCSX2 GS freeze blob: header, 4 MB local memory, 84 trailing state bytes
# (see tools/gs_vram.py - the local-memory base is len - 0x400000 - 84).
GS_LM_SIZE = 0x400000
GS_TRAILER = 84

TBW = 8           # all UI sprites live in the 512-texel-wide UI buffer

# (name, TBP0, CBP, w, h, canvas_x, canvas_y, draw_w, draw_h) -
# func_00209DF0/func_00209860 TEX0 tokens + audited draw anchors
# (FINDINGS.md "STATUS SCREEN LAYOUT" items 3/5/6/10). The bullet icon's
# 32x32 texture GS-scales to a 24x24 sprite; everything else draws 1:1.
SPRITES = [
    ("title",   0x1E50, 0x229D, 128,  64,  16,   0, 128,  64),
    ("legend",  0x1D40, 0x22A8, 128, 128,   0, 320, 128, 128),
    ("icon_tl", 0x2186, 0x22F6,  32,  32,  11, 304,  32,  32),
    ("icon_tr", 0x21F0, 0x22F6,  32,  32, 484, 304,  32,  32),
    ("icon_bl", 0x2192, 0x22F6,  32,  32, 417,  10,  32,  32),
    ("icon_br", 0x21F4, 0x22F6,  32,  32, 417, 406,  32,  32),
    ("bullet",  0x2196, 0x228C,  32,  32,  16, 262,  24,  24),
]

# Sheet packing (2-px transparent gutters so bilinear sampling never
# bleeds between neighbors): legend left, title top-right, icons in rows
# under the title.
SHEET_W, SHEET_H = 272, 144
PACK = {            # name -> (u, v)
    "title":   (136,  0),
    "legend":  (  0,  0),
    "icon_tl": (136, 72),
    "icon_tr": (170, 72),
    "icon_bl": (204, 72),
    "icon_br": (238, 72),
    "bullet":  (136, 108),
}


def decode_sprite(lm: bytes, tbp: int, cbp: int, w: int, h: int) -> bytes:
    """PSMT4 texture + 16-entry CSM1 CLUT -> RGBA8, v-flipped back to
    screen orientation (rows top-down as drawn)."""
    idx = read_psmt4(lm, tbp, TBW, w, h)
    clut = read_clut16_rgba(lm, cbp)        # GS alpha 0x80 -> 255 inside
    out = bytearray(w * h * 4)
    for y in range(h):
        src = (h - 1 - y) * w               # undo the stored v-flip
        dst = y * w * 4
        for x in range(w):
            c = idx[src + x] * 4
            out[dst:dst + 4] = clut[c:c + 4]
            dst += 4
    return bytes(out)


def ink_coverage(rgba: bytes) -> float:
    n = sum(1 for i in range(3, len(rgba), 4) if rgba[i] > 8)
    return n / (len(rgba) // 4)


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--gs", default="scratch/state01/gs.bin",
                    help="GS freeze blob from parse_pcsx2_state.py "
                         "(default: scratch/state01/gs.bin)")
    ap.add_argument("--out", default="assets/ui.emui",
                    help="output .emui path (default: assets/ui.emui)")
    ap.add_argument("--png", metavar="DIR",
                    help="also write per-sprite debug PNGs into DIR")
    args = ap.parse_args(argv)

    gs = Path(args.gs).read_bytes()
    base = len(gs) - GS_LM_SIZE - GS_TRAILER
    if base < 0:
        sys.exit(f"error: {args.gs} is smaller than a GS freeze blob")
    lm = gs[base:base + GS_LM_SIZE]

    sheet = bytearray(SHEET_W * SHEET_H * 4)
    records = []
    for name, tbp, cbp, w, h, cx, cy, dw, dh in SPRITES:
        rgba = decode_sprite(lm, tbp, cbp, w, h)
        cov = ink_coverage(rgba)
        print(f"{name:8s} TBP {tbp:#06x} CBP {cbp:#06x} {w}x{h} "
              f"-> ({cx},{cy}) {dw}x{dh}  ink {cov * 100:.0f}%")
        if cov == 0.0:
            sys.exit(f"error: {name} decodes fully transparent - is this "
                     "an in-game save state with the UI chunk loaded?")
        u, v = PACK[name]
        for y in range(h):
            o = ((v + y) * SHEET_W + u) * 4
            sheet[o:o + w * 4] = rgba[y * w * 4:(y + 1) * w * 4]
        records.append((u, v, w, h, cx, cy, dw, dh))
        if args.png:
            d = Path(args.png)
            d.mkdir(parents=True, exist_ok=True)
            write_png_rgba(d / f"{name}.png", w, h, rgba)

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("wb") as f:
        f.write(b"EMUI")
        f.write(struct.pack("<4I", 1, SHEET_W, SHEET_H, len(records)))
        for (u, v, w, h, cx, cy, dw, dh) in records:
            f.write(struct.pack("<4H2h2H", u, v, w, h, cx, cy, dw, dh))
        f.write(sheet)
    print(f"wrote {out} - sheet {SHEET_W}x{SHEET_H}, "
          f"{len(records)} sprites")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
