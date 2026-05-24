#!/usr/bin/env python3
"""CLUT (palette) helpers for Extermination texture extraction.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing.

WHAT THIS MODULE DOES
---------------------
PSMT8 textures on the PS2 GS always sample through a 256-entry palette (CLUT).
The Extermination data files do NOT package CLUTs alongside textures inside
the GS upload packet -- there is exactly one BITBLTBUF/TRXREG/IMAGE-GIF per
texture transfer (verified by scanning every transfer in DATA.DAT), with no
secondary 16x16 PSMCT32 CLUT transfer and no 1024-byte tail. The engine
uploads CLUTs separately at runtime: a `TEX0` register with `CLD>=1` and a
`CBP` field pointing into GS VRAM, with the palette bytes coming from a raw
1024-byte (256 x PSMCT32) blob copied from main RAM.

These raw blobs ARE in the data files -- not framed as GIF packets, just
sitting as raw 1024-byte regions inside model/level files (id 0x6e/0x70/0x72
character/model files, and some id 0x44 level files). This module locates
them heuristically and applies them.

RECOGNITION HEURISTIC
---------------------
A 256-entry PSMCT32 CLUT has the form `[R G B A] * 256 = 1024 bytes`. PS2
convention puts A in [0..0x80] with 0x80 = fully opaque. A real palette also
has many distinct RGB triples (a mostly-flat region is uninteresting). We
require:
  * Every alpha byte (offsets 3, 7, ..., 1023) is in [0, 0x80].
  * At least `min_alpha_80` bytes equal 0x80 (most entries fully opaque).
  * At least `min_distinct_rgb` distinct (R,G,B) triples.
  * 16-byte aligned start.
Adjacent overlapping candidate windows are deduplicated by keeping the first
of each 1024-byte cluster.

PSMT8 CSM1 CLUT SWIZZLE
-----------------------
When a CLUT is uploaded to GS VRAM in CSM1 mode (the common case), the GS
reads it back through a swizzle. For PSMT8 (8-bit indices, 256-entry CLUT
in PSMCT32 layout), entries 8..15 and 16..23 are exchanged in every group
of 32 (the GS treats them as two interleaved 8x2 blocks of a 16x2 region).
A CLUT stored linearly in source needs this swap applied before indexing;
a CLUT stored already-swizzled needs nothing. We don't know a priori which
form the game stores, so we offer both: see `apply_clut`.

GRAYSCALE FALLBACK
------------------
The PSMT8 indices in Extermination are luminance-ordered (adjacent-index
delta ~7-22 vs ~85 for a random palette -- see docs/FINDINGS.md). So the
identity grayscale palette `clut[i] = (i, i, i, 255)` produces a faithful
luminance preview even without the real CLUT.
"""
from __future__ import annotations

import struct
import zlib
from pathlib import Path


__all__ = [
    "find_clut_candidates",
    "identity_grayscale_clut",
    "psmct8_csm1_swizzle",
    "apply_clut",
    "write_png_rgba",
    "write_png_gray",
]


# ---------------------------------------------------------------------------
# CLUT location
# ---------------------------------------------------------------------------
def find_clut_candidates(
    d: bytes,
    align: int = 16,
    min_alpha_80: int = 100,
    min_distinct_rgb: int = 32,
) -> list[int]:
    """Return file offsets of 1024-byte regions that look like PSMCT32 CLUTs.

    Adjacent overlapping windows are deduplicated (kept the first of each
    1024-byte cluster). See module docstring for the heuristic details.
    """
    n = len(d)
    if n < 1024:
        return []
    hits: list[int] = []
    for start in range(0, n - 1024 + 1, align):
        # Alpha sanity: every 4th byte must be in [0, 0x80].
        ok = True
        a80 = 0
        for i in range(3, 1024, 4):
            a = d[start + i]
            if a > 0x80:
                ok = False
                break
            if a == 0x80:
                a80 += 1
        if not ok or a80 < min_alpha_80:
            continue
        # Distinct RGB triples: avoids matching flat/padding regions.
        rgb_set = set()
        for i in range(0, 1024, 4):
            rgb_set.add(d[start + i: start + i + 3])
            if len(rgb_set) >= min_distinct_rgb:
                break
        else:
            continue
        hits.append(start)
    # Collapse overlapping 1024-byte windows to one representative each.
    out: list[int] = []
    for s in hits:
        if not out or s - out[-1] >= 1024:
            out.append(s)
    return out


# ---------------------------------------------------------------------------
# Default identity grayscale CLUT
# ---------------------------------------------------------------------------
def identity_grayscale_clut() -> bytes:
    """256-entry RGBA palette where entry i = (i, i, i, 255)."""
    out = bytearray(256 * 4)
    for i in range(256):
        out[i * 4 + 0] = i
        out[i * 4 + 1] = i
        out[i * 4 + 2] = i
        out[i * 4 + 3] = 0xFF
    return bytes(out)


# ---------------------------------------------------------------------------
# PSMT8 / CSM1 CLUT entry swizzle
# ---------------------------------------------------------------------------
# In every group of 32 consecutive entries, indices 8..15 and 16..23 are
# exchanged. This converts between "linear" and "GS CSM1" CLUT orderings.
_CLUT_PSMT8_PERM = []
for base in range(0, 256, 32):
    for i in range(32):
        if 8 <= i < 16:
            _CLUT_PSMT8_PERM.append(base + i + 8)
        elif 16 <= i < 24:
            _CLUT_PSMT8_PERM.append(base + i - 8)
        else:
            _CLUT_PSMT8_PERM.append(base + i)
_CLUT_PSMT8_PERM = tuple(_CLUT_PSMT8_PERM)


def psmct8_csm1_swizzle(clut1024: bytes) -> bytes:
    """Swap entries 8..15 with 16..23 within each 32-entry block (256 -> 256)."""
    if len(clut1024) != 1024:
        raise ValueError(f"CLUT must be 1024 bytes, got {len(clut1024)}")
    out = bytearray(1024)
    for dst, src in enumerate(_CLUT_PSMT8_PERM):
        out[dst * 4: dst * 4 + 4] = clut1024[src * 4: src * 4 + 4]
    return bytes(out)


# ---------------------------------------------------------------------------
# Apply a CLUT to an 8-bit index image
# ---------------------------------------------------------------------------
def apply_clut(indices: bytes, clut_rgba: bytes) -> bytes:
    """Map each 8-bit index through a 1024-byte RGBA CLUT -> RGBA pixel stream.

    `clut_rgba` must be 1024 bytes (256 * 4). Returns `len(indices) * 4` bytes.
    """
    if len(clut_rgba) != 1024:
        raise ValueError(f"CLUT must be 1024 bytes RGBA, got {len(clut_rgba)}")
    # Build a 256-entry tuple of 4-byte slices for fast lookup.
    table = [clut_rgba[i * 4: i * 4 + 4] for i in range(256)]
    out = bytearray(len(indices) * 4)
    for i, idx in enumerate(indices):
        out[i * 4: i * 4 + 4] = table[idx]
    return bytes(out)


# ---------------------------------------------------------------------------
# PNG writers (no external dependencies)
# ---------------------------------------------------------------------------
def _png_chunk(typ: bytes, data: bytes) -> bytes:
    return (struct.pack(">I", len(data)) + typ + data +
            struct.pack(">I", zlib.crc32(typ + data) & 0xFFFFFFFF))


def write_png_rgba(path: Path, width: int, height: int, rgba: bytes) -> None:
    """Write an 8-bit RGBA PNG."""
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    raw = bytearray()
    row_bytes = width * 4
    for y in range(height):
        raw.append(0)
        raw += rgba[y * row_bytes:(y + 1) * row_bytes]
    with path.open("wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + _png_chunk(b"IHDR", ihdr) +
                _png_chunk(b"IDAT", zlib.compress(bytes(raw), 6)) +
                _png_chunk(b"IEND", b""))


def write_png_gray(path: Path, width: int, height: int, pixels: bytes) -> None:
    """Write an 8-bit grayscale PNG (preserves existing behavior)."""
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 0, 0, 0, 0)
    raw = bytearray()
    for y in range(height):
        raw.append(0)
        raw += pixels[y * width:(y + 1) * width]
    with path.open("wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + _png_chunk(b"IHDR", ihdr) +
                _png_chunk(b"IDAT", zlib.compress(bytes(raw), 6)) +
                _png_chunk(b"IEND", b""))


# ---------------------------------------------------------------------------
# Convenience: pick a CLUT for a texture by scanning its file (and dir).
# ---------------------------------------------------------------------------
def find_best_clut_for_file(
    file_bytes: bytes,
    image_payload_range: tuple[int, int] | None = None,
) -> bytes | None:
    """Return the first non-image-overlapping 1024-byte CLUT in this file.

    If `image_payload_range = (start, end)` is given, CLUT candidates that fall
    inside the image payload are rejected (those are almost always coincidental
    matches inside texture pixel data, not real palettes).
    """
    cands = find_clut_candidates(file_bytes, min_alpha_80=128, min_distinct_rgb=64)
    if image_payload_range:
        lo, hi = image_payload_range
        cands = [c for c in cands if c + 1024 <= lo or c >= hi]
    if not cands:
        return None
    return file_bytes[cands[0]:cands[0] + 1024]


def scan_dir_for_cluts(directory: Path) -> dict[Path, list[int]]:
    """Map each *.bin file in `directory` to its CLUT-candidate offsets."""
    out: dict[Path, list[int]] = {}
    for f in sorted(directory.glob("*.bin")):
        d = f.read_bytes()
        cands = find_clut_candidates(d, min_alpha_80=128, min_distinct_rgb=64)
        if cands:
            out[f] = cands
    return out
