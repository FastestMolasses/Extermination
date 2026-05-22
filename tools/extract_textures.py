#!/usr/bin/env python3
"""Extermination (SCUS-97112) texture extractor.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing.

Textures live in `DATA.DAT` as PS2 GS texture-upload packets: a file beginning
`07 XX 00 60` is a GIF/DMA packet that DMAs a texture into GS VRAM. The packet
carries GS register writes (BITBLTBUF 0x50, TRXPOS 0x51, TRXREG 0x52, TRXDIR
0x53) followed by an IMAGE-mode GIF tag with the pixel payload.

The payload is an **8-bit indexed (PSMT8) texture** uploaded through a PSMCT32
transfer: a TRXREG of w x h (32-bit) carries a (w*2) x (h*2) 8-bit image, in
PS2 swizzled VRAM order. This tool parses the packet, un-swizzles the index
data, and writes a PNG.

STATUS: the CLUT (256-colour palette) is not stored in the texture file itself
(file size == header + index data exactly) -- palettes appear to be uploaded
separately/shared across an area's textures. Until that is reverse-engineered,
output is 8-bit grayscale (index value as gray): shapes and layout are
correct, colours are not. See docs/PROGRESS.md.

Usage:
  extract_textures.py --in extract --out textures
"""
from __future__ import annotations

import argparse
import struct
import sys
import zlib
from pathlib import Path


def is_texture_packet(path: Path) -> bool:
    with path.open("rb") as f:
        h = f.read(4)
    return len(h) >= 4 and h[0] == 0x07 and h[2] == 0x00 and h[3] == 0x60


def first_trxreg(d: bytes) -> tuple[int, int] | None:
    """Return (width, height) of the first GS TRXREG (0x52) A+D register write."""
    for pos in range(0, len(d) - 16, 16):
        addr = int.from_bytes(d[pos + 8:pos + 16], "little")
        if addr >> 8 == 0 and addr & 0xFF == 0x52:
            data = int.from_bytes(d[pos:pos + 8], "little")
            return data & 0xFFF, (data >> 32) & 0xFFF
    return None


def find_image_block(d: bytes, nbytes: int) -> int | None:
    """Offset of the IMAGE-mode GIF payload of exactly nbytes (16-byte aligned)."""
    for pos in range(0, len(d) - 16, 16):
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
            out[y * width + x] = src[block + column + byte]
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


def convert(path: Path, out_dir: Path) -> str:
    d = path.read_bytes()
    trx = first_trxreg(d)
    if trx is None:
        return f"{path.name}: no TRXREG found"
    tw, th = trx
    index_bytes = tw * th * 4  # PSMCT32 transfer payload = 8-bit index data
    off = find_image_block(d, index_bytes)
    if off is None:
        return f"{path.name}: no {index_bytes}-byte IMAGE block (TRXREG {tw}x{th})"
    width, height = tw * 2, th * 2  # PSMT8 dimensions
    pixels = unswizzle8(d[off:off + index_bytes], width, height)
    name = f"{path.parent.name}_{path.stem}.png"
    write_png_gray(out_dir / name, width, height, pixels)
    return f"{path.parent.name}/{path.name}: {width}x{height} 8-bit -> {name}"


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination texture extractor")
    p.add_argument("--in", dest="input", default="extract", help="extraction directory")
    p.add_argument("--out", default="textures", help="output directory (default: textures/)")
    args = p.parse_args(argv)

    root = Path(args.input)
    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)
    packets = sorted(p for p in root.rglob("*.bin") if is_texture_packet(p))
    done = 0
    for path in packets:
        line = convert(path, out_dir)
        print(line)
        done += ".png" in line
    print(f"\n{done} of {len(packets)} texture packets converted -> {out_dir}/")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
