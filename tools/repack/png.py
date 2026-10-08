"""Small PNG interchange reader/writer for editable native texture assets.

Accepts noninterlaced 8-bit gray, gray-alpha, RGB, RGBA and indexed PNGs,
including all five row filters. Pixel results are RGBA; no gamma correction
or color-space conversion changes stored channel values.
"""
from __future__ import annotations

from pathlib import Path
import struct
import zlib

SIGNATURE = b"\x89PNG\r\n\x1a\n"
CHANNELS = {0: 1, 2: 3, 3: 1, 4: 2, 6: 4}


def _chunk(kind: bytes, data: bytes) -> bytes:
    return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data))


def write(path: Path, width: int, height: int, pixels: bytes, *, mode: str = "RGBA") -> None:
    channels, color = (1, 0) if mode == "L" else (4, 6)
    if mode not in ("L", "RGBA") or len(pixels) != width * height * channels:
        raise ValueError("PNG shape does not match pixel data")
    header = struct.pack(">IIBBBBB", width, height, 8, color, 0, 0, 0)
    stride = width * channels
    raw = b"".join(b"\0" + pixels[y * stride:(y + 1) * stride] for y in range(height))
    Path(path).write_bytes(SIGNATURE + _chunk(b"IHDR", header) +
                           _chunk(b"IDAT", zlib.compress(raw)) + _chunk(b"IEND", b""))


def read(path: Path) -> tuple[int, int, bytes]:
    """Return width, height and unmodified 8-bit RGBA channel values."""
    raw = Path(path).read_bytes()
    if not raw.startswith(SIGNATURE):
        raise ValueError("not a PNG image")
    at, header, palette, transparency, compressed = 8, None, None, None, bytearray()
    ended = False
    while at < len(raw):
        if at + 12 > len(raw):
            raise ValueError("truncated PNG chunk")
        length = struct.unpack_from(">I", raw, at)[0]
        kind = raw[at + 4:at + 8]
        end = at + 12 + length
        if end > len(raw):
            raise ValueError("PNG chunk exceeds file length")
        data = raw[at + 8:at + 8 + length]
        checksum = struct.unpack_from(">I", raw, end - 4)[0]
        if zlib.crc32(kind + data) != checksum:
            raise ValueError("PNG chunk checksum mismatch")
        if header is None and kind != b"IHDR":
            raise ValueError("PNG must begin with IHDR")
        if kind == b"IHDR":
            if header is not None or length != 13:
                raise ValueError("invalid PNG IHDR")
            header = struct.unpack(">IIBBBBB", data)
        elif kind == b"PLTE":
            if palette is not None or not length or length % 3 or length > 768:
                raise ValueError("invalid PNG palette")
            palette = data
        elif kind == b"tRNS":
            if transparency is not None:
                raise ValueError("duplicate PNG transparency")
            transparency = data
        elif kind == b"IDAT":
            compressed.extend(data)
        elif kind == b"IEND":
            if length or end != len(raw):
                raise ValueError("invalid PNG end marker")
            ended = True
            break
        elif not kind[0] & 32:
            raise ValueError(f"unsupported critical PNG chunk {kind!r}")
        elif kind in (b"acTL", b"fcTL", b"fdAT"):
            raise ValueError("animated PNGs are unsupported")
        at = end
    if not ended or header is None:
        raise ValueError("incomplete PNG image")
    width, height, depth, color, compression, filtering, interlace = header
    if (depth != 8 or color not in CHANNELS or compression or filtering or interlace or
            not 0 < width <= 4096 or not 0 < height <= 4096 or width * height > 4 * 1024 * 1024):
        raise ValueError("expected noninterlaced 8-bit PNG, at most 4 million pixels")
    if color == 3 and (palette is None or len(transparency or b"") > len(palette) // 3):
        raise ValueError("indexed PNG requires a complete palette")
    if transparency is not None and ((color == 0 and len(transparency) != 2) or
                                    (color == 2 and len(transparency) != 6) or color in (4, 6)):
        raise ValueError("invalid PNG transparency for color type")
    channels = CHANNELS[color]
    stride = width * channels
    expected = (stride + 1) * height
    inflater = zlib.decompressobj()
    try:
        filtered = inflater.decompress(compressed, expected + 1)
    except zlib.error as exc:
        raise ValueError("invalid PNG compressed pixels") from exc
    if len(filtered) != expected or not inflater.eof or inflater.unused_data or inflater.unconsumed_tail:
        raise ValueError("PNG decompressed length disagrees with dimensions")
    scanlines, previous = bytearray(), bytearray(stride)
    for y in range(height):
        method = filtered[y * (stride + 1)]
        if method > 4:
            raise ValueError("invalid PNG row filter")
        row = bytearray(filtered[y * (stride + 1) + 1:(y + 1) * (stride + 1)])
        for x in range(stride):
            left = row[x - channels] if x >= channels else 0
            above = previous[x]
            upper_left = previous[x - channels] if x >= channels else 0
            if method == 1:
                predictor = left
            elif method == 2:
                predictor = above
            elif method == 3:
                predictor = (left + above) // 2
            elif method == 4:
                estimate = left + above - upper_left
                distances = (abs(estimate - left), abs(estimate - above), abs(estimate - upper_left))
                predictor = (left, above, upper_left)[distances.index(min(distances))]
            else:
                predictor = 0
            row[x] = (row[x] + predictor) & 255
        scanlines.extend(row)
        previous = row
    if color == 6:
        return width, height, bytes(scanlines)
    pixels = bytearray(width * height * 4)
    transparent = (struct.unpack(">H", transparency)[0] if transparency and color == 0 else
                   struct.unpack(">3H", transparency) if transparency and color == 2 else None)
    for i in range(width * height):
        start = i * channels
        if color == 0:
            r = g = b = scanlines[start]
            alpha = 0 if r == transparent else 255
        elif color == 4:
            r = g = b = scanlines[start]
            alpha = scanlines[start + 1]
        elif color == 2:
            r, g, b = scanlines[start:start + 3]
            alpha = 0 if (r, g, b) == transparent else 255
        else:
            index = scanlines[start]
            if index * 3 + 3 > len(palette):
                raise ValueError("PNG pixel references an absent palette entry")
            r, g, b = palette[index * 3:index * 3 + 3]
            alpha = transparency[index] if transparency and index < len(transparency) else 255
        pixels[i * 4:i * 4 + 4] = bytes((r, g, b, alpha))
    return width, height, bytes(pixels)
