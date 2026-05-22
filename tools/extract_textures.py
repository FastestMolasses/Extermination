#!/usr/bin/env python3
"""Extermination (SCUS-97112) texture extractor.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing.

Textures are PS2 GS texture-upload packets: a `07 XX 00 60` GIF/DMA packet
that DMAs a texture into GS VRAM. The packet carries GS register writes
(BITBLTBUF 0x50, TRXPOS 0x51, TRXREG 0x52, TRXDIR 0x53) then an IMAGE-mode
GIF tag with the pixel payload.

The payload is an 8-bit indexed (PSMT8) texture uploaded through a PSMCT32
transfer: a TRXREG of w x h (32-bit) carries a (w*2) x (h*2) 8-bit image in
PS2 swizzled order. This tool finds every packet -- the standalone `07../60`
files (UI/common art) AND packets embedded inside larger level files
(id 0x44) -- un-swizzles, and writes a PNG.

STATUS: approximate -- two things are not yet correct:
  1. Unswizzle. The standard combined 8-bit unswizzle (used here) decodes
     multi-page textures cleanly; a from-memory 2-step GS swizzle came out
     worse and was discarded. Any residual oddities may be atlas layout.
  2. CLUT. The 256-color palette is not in the texture packets; output is
     8-bit grayscale until the palettes are located.
See docs/PROGRESS.md.

Usage:
  extract_textures.py --in extract --out textures
"""
from __future__ import annotations

import argparse
import struct
import sys
import zlib
from pathlib import Path


def find_packet_starts(d: bytes) -> list[int]:
    """Offsets of `07 XX 00 60` GS texture packets (XX a non-zero multiple of 8)."""
    starts = []
    for i in range(0, len(d) - 16, 16):
        if d[i] == 0x07 and d[i + 2] == 0x00 and d[i + 3] == 0x60 \
                and d[i + 1] and d[i + 1] % 8 == 0:
            starts.append(i)
    return starts


def first_trxreg(d: bytes, start: int) -> tuple[int, int] | None:
    """(width, height) of the first GS TRXREG (0x52) A+D write at/after `start`."""
    for pos in range(start, len(d) - 16, 16):
        addr = int.from_bytes(d[pos + 8:pos + 16], "little")
        if addr >> 8 == 0 and addr & 0xFF == 0x52:
            data = int.from_bytes(d[pos:pos + 8], "little")
            return data & 0xFFF, (data >> 32) & 0xFFF
    return None


def find_image_block(d: bytes, nbytes: int, start: int) -> int | None:
    """Offset of the IMAGE-mode GIF payload of exactly `nbytes` at/after `start`."""
    for pos in range(start, len(d) - 16, 16):
        if (d[pos + 7] >> 2) & 3 == 2:  # GIF tag FLG == IMAGE
            nloop = (d[pos] | (d[pos + 1] << 8)) & 0x7FFF
            if nloop * 16 == nbytes and pos + 16 + nbytes <= len(d):
                return pos + 16
    return None


def unswizzle8(src: bytes, width: int, height: int) -> bytes:
    """Un-swizzle an 8-bit PS2 texture (the standard PSMT8 unswizzle)."""
    out = bytearray(width * height)
    for y in range(height):
        for x in range(width):
            block = (y & ~0xF) * width + (x & ~0xF) * 2
            swap = (((y + 2) >> 2) & 1) * 4
            pos_y = (((y & ~3) >> 1) + (y & 1)) & 7
            column = pos_y * width * 2 + ((x + swap) & 7) * 4
            byte = ((y >> 1) & 1) + ((x >> 2) & 2)
            idx = block + column + byte
            out[y * width + x] = src[idx] if idx < len(src) else 0
    return bytes(out)


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
    """Convert one GS texture packet at offset `start`; None if not a valid packet."""
    trx = first_trxreg(d, start)
    if trx is None:
        return None
    tw, th = trx
    index_bytes = tw * th * 4  # PSMCT32 transfer payload == 8-bit index data
    off = find_image_block(d, index_bytes, start)
    if off is None or tw == 0 or th == 0:
        return None
    width, height = tw * 2, th * 2  # PSMT8 dimensions
    pixels = unswizzle8(d[off:off + index_bytes], width, height)
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
