#!/usr/bin/env python3
"""Compose startup screens from the user's extracted SCUS-97112 modules.

Unlike the atlas-preview exporter, this follows the TEX0 values and draw
positions in 001AB9D0/001ABC60/001ABE10/001AC7F0 -> 001ABF90/00207E40.
Palettes are inside the same PSMCT32 uploads as the indexed art. CSM1 needs
both its index-bit permutation AND the PSMCT32 address mapping; interpreting
the palette bytes as a linear array loses this second mapping.

Outputs are local disc-derived assets, never source-controlled. Python stdlib
only. The optional PNGs are composed screens, not the packed source atlases.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path

from clut import write_png_rgba
from extract_textures import (find_image_block, first_trxreg, psmct32_word,
                              psmt4_nibble, psmt8_byte)

WIDTH, HEIGHT = 512, 448

# Argument order of 001ABF90: top-left, top-right, bottom-left, bottom-right.
LOGOS = (
    (40, (0x2005C40621322A00, 0x2005C48621322A40,
          0x2005C60621322C00, 0x2005C68621322C40)),
    (41, (0x2006040621422E00, 0x2006042621422E40,
          0x2006048621422F00, 0x2006048621422F40)),
    (41, (0x2006000621322A00, 0x2006008621322A40,
          0x2006020621322C00, 0x2006028621322C40)),
)
TITLE = (0x2005F00621322A00, 0x2005F08621322A40,
         0x2005F20621322C00, 0x2005F28621322C40)
# The menu words have distinct selected/unselected TEX0s, not vertex tints.
MENU = (
    (0x2005F425E1422E40, 0x2005F405E1422E80, 0x2005F405E1422F00),
    (0x2005F405E1422E00, 0x2005F485E1422EC0, 0x2005F405E1422F00),
    (0x2005F405E1422E00, 0x2005F405E1422E80, 0x2005F4A5E1422F40),
)


def upload_module(path: Path) -> tuple[bytes, set[int]]:
    """Replay the module's initial, bounded PSMCT32 image transfer."""
    data = path.read_bytes()
    if len(data) < 128:
        raise ValueError(f"{path}: truncated screen module")
    regs = {struct.unpack_from('<Q', data, i + 8)[0]:
            struct.unpack_from('<Q', data, i)[0] for i in range(16, 112, 16)}
    bb, pos = regs.get(0x50), regs.get(0x51)
    if bb is None or pos is None or regs.get(0x53) != 0:
        raise ValueError(f"{path}: missing host-to-local image registers")
    if (bb >> 56) & 63 or pos:
        raise ValueError(f"{path}: expected PSMCT32 transfer at (0,0)")
    shape = first_trxreg(data, 0)
    if shape is None:
        raise ValueError(f"{path}: missing TRXREG")
    w, h = shape
    base, ppr = ((bb >> 32) & 16383) * 256, (bb >> 48) & 63
    if not 0 < w <= 1024 or not 0 < h <= 1024 or not ppr:
        raise ValueError(f"{path}: invalid transfer geometry")
    start = find_image_block(data, w * h * 4, 0)
    if start is None:
        raise ValueError(f"{path}: missing or truncated IMAGE payload")
    local = bytearray(4 * 1024 * 1024)
    covered: set[int] = set()
    for y in range(h):
        for x in range(w):
            dst = base + psmct32_word(x, y, ppr) * 4
            if dst + 4 > len(local):
                raise ValueError(f"{path}: transfer exceeds local memory")
            src = start + (y * w + x) * 4
            local[dst:dst + 4] = data[src:src + 4]
            covered.add(dst // 256)
    return bytes(local), covered


def palette_address(cbp: int, index: int, psm: int) -> int:
    if psm == 0x13:
        index = (index & ~24) | ((index & 8) << 1) | ((index & 16) >> 1)
    # PSMT4 consumes the first 8x2 region; PSMT8 consumes all 16x16.
    x, y = (index % 16, index // 16) if psm == 0x13 else (index % 8, index // 8)
    return cbp * 256 + psmct32_word(x, y, 1) * 4


def texture(local: bytes, covered: set[int], tex0: int) -> tuple[int, int, bytes]:
    base, bw = tex0 & 16383, (tex0 >> 14) & 63
    psm, cbp = (tex0 >> 20) & 63, (tex0 >> 37) & 16383
    w, h = 1 << ((tex0 >> 26) & 15), 1 << ((tex0 >> 30) & 15)
    if psm not in (0x13, 0x14) or (tex0 >> 51) & 0x3ff:
        raise ValueError(f"unsupported texture/palette format {tex0:#x}")
    if not bw or w > 1024 or h > 1024:
        raise ValueError("invalid texture geometry")
    rgba = bytearray(w * h * 4)
    for y in range(h):
        for x in range(w):
            if psm == 0x13:
                at = base * 256 + psmt8_byte(x, y, bw // 2)
                if at // 256 not in covered:
                    raise ValueError(f"texture reads outside module upload: {at:#x}")
                index = local[at]
            else:
                nibble = base * 512 + psmt4_nibble(x, y, bw // 2)
                at = nibble // 2
                if at // 256 not in covered:
                    raise ValueError(f"texture reads outside module upload: {at:#x}")
                index = (local[at] >> (4 * (nibble & 1))) & 15
            at = palette_address(cbp, index, psm)
            if at // 256 not in covered:
                raise ValueError(f"palette reads outside module upload: {at:#x}")
            r, g, b, alpha = local[at:at + 4]
            dst = (y * w + x) * 4
            rgba[dst:dst + 4] = bytes((r, g, b, min(255, alpha * 2)))
    return w, h, bytes(rgba)


def sprite(canvas: bytearray, tex: tuple[int, int, bytes],
           x: int, y: int, w: int, h: int) -> None:
    """00207E40 uses UV(0,0) at bottom-left: vertically flip each tile.

    Native canvas doubles the PS2's 224 field lines to 448 display lines.
    These UI draws use integral, unity-scale sampling in that display space.
    """
    tw, th, pixels = tex
    if (tw, th) != (w, h):
        raise ValueError(f"unexpected scaled startup sprite: {(tw, th)} -> {(w, h)}")
    for dy in range(max(y, 0), min(y + h, HEIGHT)):
        for dx in range(max(x, 0), min(x + w, WIDTH)):
            src = ((h - 1 - (dy - y)) * w + dx - x) * 4
            dst = (dy * WIDTH + dx) * 4
            # GS alpha domain is 0..128; exported 255 means original 128.
            alpha = (pixels[src + 3] + 1) // 2
            for c in range(3):
                canvas[dst + c] = max(0, min(255, canvas[dst + c] +
                    ((pixels[src + c] - canvas[dst + c]) * alpha >> 7)))


def compose(local: bytes, covered: set[int], background: tuple[int, ...],
            cursor: int | None = None) -> bytes:
    canvas = bytearray(b'\0\0\0\xff' * (WIDTH * HEIGHT))
    for k in (2, 3, 0, 1):  # original 001ABF90 draw ordering
        sprite(canvas, texture(local, covered, background[k]),
               (k & 1) * 256, (k >> 1) * 256, 256, 256)
    if cursor is not None:
        for tex0, y in zip(MENU[cursor], (260, 294, 324)):
            sprite(canvas, texture(local, covered, tex0), 127, y, 256, 128)
    return bytes(canvas)


def write_emui(path: Path, pixels: bytes) -> None:
    header = struct.pack('<4s4I8H', b'EMUI', 1, WIDTH, HEIGHT, 1,
                         0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT)
    path.write_bytes(header + pixels)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--extract', type=Path, default=Path('extract'))
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--png', action='store_true')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    sources = {}
    modules = {}
    for number in (1, 40, 41):
        paths = sorted((args.extract / f'chunk{number:02d}').glob('*.bin'))
        if len(paths) != 1:
            parser.error(f'expected one section in chunk{number:02d}; got {len(paths)}')
        modules[number] = upload_module(paths[0])
        sources[str(number)] = {'path': str(paths[0]),
                               'sha256': hashlib.sha256(paths[0].read_bytes()).hexdigest()}
    screens = {f'logo_{i}': compose(*modules[module], tex0s)
               for i, (module, tex0s) in enumerate(LOGOS)}
    screens.update({f'title_{i}': compose(*modules[1], TITLE, i) for i in range(3)})
    for name, pixels in screens.items():
        write_emui(args.out / f'{name}.emui', pixels)
        if args.png:
            write_png_rgba(args.out / f'{name}.png', WIDTH, HEIGHT, pixels)
        print(f'{name}: {WIDTH}x{HEIGHT}')
    (args.out / 'manifest.json').write_text(json.dumps({'sources': sources,
        'screens': list(screens), 'canvas': [WIDTH, HEIGHT]}, indent=2) + '\n')


if __name__ == '__main__':
    main()
