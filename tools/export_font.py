#!/usr/bin/env python3
"""export_font.py - extract the engine's two UI fonts to assets/font.emfn.

Original tooling for the Extermination decompilation project. Ingests only an
EE-RAM dump the user produced locally from their own copy of the game (via
tools/parse_pcsx2_state.py on a PCSX2 save state); nothing disc-derived is
redistributed. Output lands in git-ignored asset directories.

WHERE THE FONT LIVES (docs/FINDINGS.md "UI FONT")
-------------------------------------------------
The fonts are NOT VRAM-resident sheets. The text functions stream glyphs
per-string from EE RAM into a PSMT4 staging strip in GS VRAM (block 0x1B00,
TBW 8 = 512 texels wide) and then draw ONE batched sprite over the strip.
The pointer block at EE 0x0028A490 (filled by the boot-asset loader; the
data sits at 0xB00000 in the test states) is:

    [0x0028A490]  tall font  - 1bpp, 30 bytes/glyph, 12x20 texels
    [0x0028A494]  small font - 1bpp, 32 bytes/glyph, 16x16 texels
    [0x0028A498]  string-table file
    [0x0028A49C]  (one more loader slot)

GLYPH RULE (from func_001CBA50 / func_001CC1E0 / func_001CC8A0)
---------------------------------------------------------------
  glyph_index = ascii - 0x20         (chars < 0x20 draw glyph 0 = blank)
  '$' (0x24) remaps to glyph_index 0x89 (tall) / 0x87 (small)
  tall  glyph bytes = font_base + glyph_index * 30
  small glyph bytes = font_base + glyph_index * 32

1bpp -> PSMT4 expansion (func_001CC8A0 via the 16-entry u16 LUT at
0x0026E350, LUT[n] spreads bit i of the nibble into texel nibble i):
within every nibble bit 0 is the LEFTMOST pixel; nibbles are consumed
low-then-high per byte.

  small: 32 bytes consumed linearly = 16 rows x 16 px (2 bytes/row).
  tall:  3 bytes per TWO rows (12 px each): byte0.lo byte0.hi byte1.lo
         = row N px 0-11, byte1.hi byte2.lo byte2.hi = row N+1 px 0-11;
         10 iterations = 20 rows.

DRAW METRICS
------------
  tall:  drawn 1:1 texel:pixel, height 20; PROPORTIONAL advance from
         func_001CBE10 (default 9; exceptions encoded below, keyed by the
         raw char code before the -0x20 remap).
  small: fixed 16x16 texel cell, GS-scaled to the caller's glyph_w/glyph_h
         (12x12 labels, 12x16 numbers on the status screen); advance =
         glyph_w per char. Bilinear (TEX1 MMAG/MMIN=1), modulate, standard
         alpha blend (TEX0 0x20036CE5E5421B00 / ALPHA 0x44 in the prebuilt
         packets D_00250FC0 / D_002511C0).

OUTPUT FORMAT .emfn v1 (little-endian) - keep in sync with the port loader
(extermination-port/src/game/em_hud.c, font_load):

  off  size
  0    4    magic "EMFN"
  4    4    u32 version = 1
  8    4    u32 sheet_w
  12   4    u32 sheet_h
  16   4    u32 font_count = 2
  20   4    u32 glyph_count          (total glyph records)
  24   16*font_count  per font:
              u32 first              (index of the font's first glyph record)
              u32 count
              u32 cell_w, cell_h     (nominal texel cell)
  ...  8*glyph_count  per glyph:
              u16 u, v               (texel position in the sheet)
              u8  w, h               (texel size)
              u8  advance            (tall: proportional px; small: 16)
              u8  pad = 0
  ...  sheet_w*sheet_h*4  RGBA8 sheet, row-major, top-down
              (white RGB, alpha = ink coverage 0/255)

Font 0 = tall (12x20), font 1 = small (16x16).

USAGE (macOS arm64, repo root)
    python3 tools/parse_pcsx2_state.py <state.p2s> --out scratch/state01
    python3 tools/export_font.py --ee scratch/state01/ee.bin \
        --out ../extermination-port/assets/font.emfn
"""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

PTR_BLOCK   = 0x0028A490
TALL_BYTES  = 30          # bytes per tall glyph (12x20 @ 1bpp)
SMALL_BYTES = 32          # bytes per small glyph (16x16 @ 1bpp)
TALL_W, TALL_H   = 12, 20
SMALL_W, SMALL_H = 16, 16
SHEET_W     = 512         # 1 GS strip width; glyph records carry exact UVs

# func_001CBE10 - tall-font proportional advance, keyed by the RAW char
# code (looked up before the '$' remap). Everything else advances 9.
TALL_ADVANCE_DEFAULT = 9
TALL_ADVANCE = {
    0x20: 5, 0x21: 4, 0x27: 4, 0x28: 8, 0x29: 8, 0x2C: 6, 0x2E: 5,
    0x2F: 8, 0x3A: 7, 0x3B: 8, 0x49: 4, 0x4A: 7, 0x4D: 12, 0x57: 12,
    0x5B: 8, 0x5C: 8, 0x5D: 8, 0x60: 5, 0x66: 8, 0x69: 4, 0x6A: 6,
    0x6C: 4, 0x6D: 12, 0x72: 9, 0x77: 12, 0x82: 6, 0x84: 8, 0x8B: 9,
    0x91: 6, 0x92: 6, 0x93: 8, 0x94: 8, 0x9B: 9, 0xA1: 4, 0xA6: 6,
}


def nibble_pixels(n: int) -> list[int]:
    """One source nibble -> 4 pixels, bit 0 = leftmost (the 0x26E350 LUT)."""
    return [(n >> b) & 1 for b in range(4)]


def decode_tall(raw: bytes) -> list[list[int]]:
    """30 bytes -> 20 rows x 12 px (func_001CC8A0 path 1)."""
    rows = []
    for i in range(10):
        b0, b1, b2 = raw[i * 3:i * 3 + 3]
        even = nibble_pixels(b0 & 0xF) + nibble_pixels(b0 >> 4) \
            + nibble_pixels(b1 & 0xF)
        odd = nibble_pixels(b1 >> 4) + nibble_pixels(b2 & 0xF) \
            + nibble_pixels(b2 >> 4)
        rows.append(even)
        rows.append(odd)
    return rows


def decode_small(raw: bytes) -> list[list[int]]:
    """32 bytes -> 16 rows x 16 px (func_001CC8A0 path 0, linear)."""
    rows = []
    for r in range(16):
        b0, b1 = raw[r * 2], raw[r * 2 + 1]
        rows.append(nibble_pixels(b0 & 0xF) + nibble_pixels(b0 >> 4)
                    + nibble_pixels(b1 & 0xF) + nibble_pixels(b1 >> 4))
    return rows


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--ee", default="scratch/state01/ee.bin",
                    help="EE RAM dump from parse_pcsx2_state.py "
                         "(default: scratch/state01/ee.bin)")
    ap.add_argument("--out", default="assets/font.emfn",
                    help="output .emfn path (default: assets/font.emfn)")
    ap.add_argument("--preview", metavar="CH",
                    help="ASCII-art one character from both fonts and exit")
    args = ap.parse_args(argv)

    ee = Path(args.ee).read_bytes()
    tall_base, small_base, strtab, _ = struct.unpack_from("<4I", ee,
                                                          PTR_BLOCK)
    if not (0 < tall_base < small_base < strtab <= len(ee)):
        sys.exit(f"error: implausible font pointers at {PTR_BLOCK:#x}: "
                 f"{tall_base:#x} {small_base:#x} {strtab:#x} - is this an "
                 "in-game EE RAM dump?")
    tall_count = (small_base - tall_base) // TALL_BYTES
    small_count = (strtab - small_base) // SMALL_BYTES
    print(f"tall  font @ {tall_base:#x}: {tall_count} glyph slots")
    print(f"small font @ {small_base:#x}: {small_count} glyph slots")

    def tall_glyph(i):
        return decode_tall(ee[tall_base + i * TALL_BYTES:
                              tall_base + (i + 1) * TALL_BYTES])

    def small_glyph(i):
        return decode_small(ee[small_base + i * SMALL_BYTES:
                               small_base + (i + 1) * SMALL_BYTES])

    if args.preview:
        idx = ord(args.preview[0]) - 0x20
        for name, rows in (("tall", tall_glyph(idx)),
                           ("small", small_glyph(idx))):
            print(f"--- {name} {args.preview[0]!r} (glyph {idx})")
            for r in rows:
                print("".join("#" if p else "." for p in r))
        return 0

    # ---- compose the sheet: tall rows first, then small rows ----------
    per_row_tall = SHEET_W // TALL_W
    per_row_small = SHEET_W // SMALL_W
    tall_rows = (tall_count + per_row_tall - 1) // per_row_tall
    small_rows = (small_count + per_row_small - 1) // per_row_small
    small_y0 = tall_rows * TALL_H
    sheet_h = small_y0 + small_rows * SMALL_H
    sheet = bytearray(SHEET_W * sheet_h * 4)

    def blit(rows, x0, y0):
        for dy, row in enumerate(rows):
            o = ((y0 + dy) * SHEET_W + x0) * 4
            for px in row:
                sheet[o:o + 4] = b"\xff\xff\xff\xff" if px \
                    else b"\xff\xff\xff\x00"
                o += 4

    glyphs = []   # (u, v, w, h, advance)
    for i in range(tall_count):
        u = (i % per_row_tall) * TALL_W
        v = (i // per_row_tall) * TALL_H
        blit(tall_glyph(i), u, v)
        adv = TALL_ADVANCE.get(i + 0x20, TALL_ADVANCE_DEFAULT)
        glyphs.append((u, v, TALL_W, TALL_H, adv))
    for i in range(small_count):
        u = (i % per_row_small) * SMALL_W
        v = small_y0 + (i // per_row_small) * SMALL_H
        blit(small_glyph(i), u, v)
        glyphs.append((u, v, SMALL_W, SMALL_H, SMALL_W))

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("wb") as f:
        f.write(b"EMFN")
        f.write(struct.pack("<5I", 1, SHEET_W, sheet_h, 2, len(glyphs)))
        f.write(struct.pack("<4I", 0, tall_count, TALL_W, TALL_H))
        f.write(struct.pack("<4I", tall_count, small_count,
                            SMALL_W, SMALL_H))
        for (u, v, w, h, adv) in glyphs:
            f.write(struct.pack("<2H4B", u, v, w, h, adv, 0))
        f.write(sheet)
    print(f"wrote {out} - sheet {SHEET_W}x{sheet_h}, "
          f"{len(glyphs)} glyphs ({tall_count} tall + {small_count} small)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
