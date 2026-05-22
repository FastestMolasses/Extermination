#!/usr/bin/env python3
"""Extermination (SCUS-97112) texture extractor.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing.

Textures are PS2 GS texture-upload packets: a `07 XX 00 60` GIF/DMA packet
carrying GS register writes (BITBLTBUF 0x50 / TRXPOS 0x51 / TRXREG 0x52 /
TRXDIR 0x53) then an IMAGE-mode GIF tag with the pixel payload.

The payload is an 8-bit indexed (PSMT8) texture uploaded through a PSMCT32
host->local transfer. To recover the image this tool runs the real PS2 GS
memory pipeline: it writes the transfer payload into a simulated VRAM at the
PSMCT32 swizzled addresses, then reads it back at the PSMT8 swizzled addresses.
The GS page/block/column swizzle tables are the documented hardware tables
(verified: this produces output byte-identical to the standard `unswizzle8`).
A TRXREG of w x h (32-bit) yields a (w*2) x (h*2) 8-bit texture.

NOTES:
- The textures are 8-bit INTENSITY (luminance), not color-indexed -- there is
  no CLUT (smoothness test + zero CLUT packets in the game data). Grayscale
  output is the correct texture data; the renderer tints via vertex color.
- Each decoded sheet is a texture ATLAS: many individual textures packed into
  one sheet, some stored flipped/rotated to pack tighter, plus non-texture
  padding. Cutting it into clean, correctly-oriented individual textures needs
  each texture's UV rectangle from the geometry/draw data (not yet decoded).
See docs/FINDINGS.md.

Usage:
  extract_textures.py --in extract --out textures
"""
from __future__ import annotations

import argparse
import struct
import sys
import zlib
from pathlib import Path

# Documented PS2 GS swizzle tables (page/block/column). PSMT8 reuses the
# PSMCT32 page swizzle. Source: the GS hardware layout.
PAGE32 = [0, 1, 4, 5, 16, 17, 20, 21, 2, 3, 6, 7, 18, 19, 22, 23,
          8, 9, 12, 13, 24, 25, 28, 29, 10, 11, 14, 15, 26, 27, 30, 31]
COL32 = [0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15]
COL8 = [
    0, 4, 16, 20, 32, 36, 48, 52, 2, 6, 18, 22, 34, 38, 50, 54,
    8, 12, 24, 28, 40, 44, 56, 60, 10, 14, 26, 30, 42, 46, 58, 62,
    33, 37, 49, 53, 1, 5, 17, 21, 35, 39, 51, 55, 3, 7, 19, 23,
    41, 45, 57, 61, 9, 13, 25, 29, 43, 47, 59, 63, 11, 15, 27, 31,
    32, 36, 48, 52, 0, 4, 16, 20, 34, 38, 50, 54, 2, 6, 18, 22,
    40, 44, 56, 60, 8, 12, 24, 28, 42, 46, 58, 62, 10, 14, 26, 30,
    1, 5, 17, 21, 33, 37, 49, 53, 3, 7, 19, 23, 35, 39, 51, 55,
    9, 13, 25, 29, 41, 45, 57, 61, 11, 15, 27, 31, 43, 47, 59, 63,
]


def psmct32_word(x: int, y: int, ppr: int) -> int:
    """GS VRAM word address of a PSMCT32 pixel in a buffer `ppr` pages wide."""
    page = (y // 32) * ppr + (x // 64)
    px = (y % 32 % 8) * 8 + (x % 64 % 8)
    block = PAGE32[(y % 32 // 8) * 8 + (x % 64 // 8)]
    return page * 2048 + block * 64 + (px // 16) * 16 + COL32[px % 16]


def psmt8_byte(x: int, y: int, ppr: int) -> int:
    """GS VRAM byte address of a PSMT8 texel in a buffer `ppr` pages wide."""
    page = (y // 64) * ppr + (x // 128)
    px = (y % 64 % 16) * 16 + (x % 128 % 16)
    block = PAGE32[(y % 64 // 16) * 8 + (x % 128 // 16)]
    return page * 8192 + block * 256 + (px // 64) * 64 + COL8[px % 128]


def deswizzle(payload: bytes, tw: int, th: int) -> tuple[int, int, bytes]:
    """Run the GS pipeline: PSMCT32 transfer payload -> VRAM -> PSMT8 read.

    `payload` is the raster PSMCT32 transfer (tw*th*4 bytes). Returns the
    (width, height, pixels) of the resulting (tw*2) x (th*2) 8-bit texture.
    """
    ow, oh = tw * 2, th * 2
    word_src = {}  # VRAM word -> source raster pixel
    ct_ppr = max(1, tw // 64)
    for y in range(th):
        for x in range(tw):
            word_src[psmct32_word(x, y, ct_ppr)] = (x, y)
    t8_ppr = max(1, ow // 128)
    out = bytearray(ow * oh)
    for ty in range(oh):
        for tx in range(ow):
            vb = psmt8_byte(tx, ty, t8_ppr)
            src = word_src.get(vb >> 2)
            if src is not None:
                sx, sy = src
                out[ty * ow + tx] = payload[(sy * tw + sx) * 4 + (vb & 3)]
    return ow, oh, bytes(out)


def find_packet_starts(d: bytes) -> list[int]:
    """Offsets of `07 XX 00 60` GS texture packets (XX a non-zero multiple of 8)."""
    return [i for i in range(0, len(d) - 16, 16)
            if d[i] == 0x07 and d[i + 2] == 0x00 and d[i + 3] == 0x60
            and d[i + 1] and d[i + 1] % 8 == 0]


def first_trxreg(d: bytes, start: int) -> tuple[int, int] | None:
    for pos in range(start, len(d) - 16, 16):
        addr = int.from_bytes(d[pos + 8:pos + 16], "little")
        if addr >> 8 == 0 and addr & 0xFF == 0x52:
            data = int.from_bytes(d[pos:pos + 8], "little")
            return data & 0xFFF, (data >> 32) & 0xFFF
    return None


def find_image_block(d: bytes, nbytes: int, start: int) -> int | None:
    for pos in range(start, len(d) - 16, 16):
        if (d[pos + 7] >> 2) & 3 == 2:  # GIF tag FLG == IMAGE
            nloop = (d[pos] | (d[pos + 1] << 8)) & 0x7FFF
            if nloop * 16 == nbytes and pos + 16 + nbytes <= len(d):
                return pos + 16
    return None


def write_png_gray(path: Path, width: int, height: int, pixels: bytes) -> None:
    def chunk(typ: bytes, data: bytes) -> bytes:
        return (struct.pack(">I", len(data)) + typ + data +
                struct.pack(">I", zlib.crc32(typ + data) & 0xFFFFFFFF))
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 0, 0, 0, 0)
    raw = bytearray()
    for y in range(height):
        raw.append(0)
        raw += pixels[y * width:(y + 1) * width]
    with path.open("wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", ihdr) +
                chunk(b"IDAT", zlib.compress(bytes(raw), 6)) + chunk(b"IEND", b""))


def convert(path: Path, d: bytes, start: int, out_dir: Path) -> str | None:
    trx = first_trxreg(d, start)
    if trx is None:
        return None
    tw, th = trx
    if tw == 0 or th == 0 or tw % 64 or th % 32:
        return None
    off = find_image_block(d, tw * th * 4, start)
    if off is None:
        return None
    width, height, pixels = deswizzle(d[off:off + tw * th * 4], tw, th)
    suffix = "" if start == 0 else f"_{start:06x}"
    name = f"{path.parent.name}_{path.stem}{suffix}.png"
    write_png_gray(out_dir / name, width, height, pixels)
    return f"{path.parent.name}/{path.name}@{start:#x}: {width}x{height} -> {name}"


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination texture extractor")
    p.add_argument("--in", dest="input", default="extract", help="extraction directory")
    p.add_argument("--out", default="textures", help="output directory (default: textures/)")
    args = p.parse_args(argv)

    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)
    done = 0
    for path in sorted(Path(args.input).rglob("*.bin")):
        d = path.read_bytes()
        for start in find_packet_starts(d):
            line = convert(path, d, start, out_dir)
            if line:
                print(line)
                done += 1
    print(f"\n{done} texture packets converted -> {out_dir}/")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
