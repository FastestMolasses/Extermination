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

CLUT (palette) HANDLING
-----------------------
PSMT8 always samples through a 256-entry CLUT. The standalone texture packets
in DATA.DAT do not carry a CLUT inside the same GS upload packet (verified:
exactly one BITBLTBUF/TRXREG/IMAGE-GIF per packet, with only sector-alignment
zero padding afterwards). The engine uploads CLUTs separately at runtime.

Raw 1024-byte CLUT blobs ARE present in the asset files -- not framed as GIF
packets, just plain RGBA data -- inside character/model files (ids 0x6e, 0x70,
0x72) and some level files (id 0x44). `tools/clut.py` locates them by their
structural shape (alpha in [0, 0x80], many distinct RGB triples). Standalone
texture files (ids 0x06..0x0c, 0x35, 0x38) carry no CLUT in the same file at
all -- their palette must come from elsewhere (boot ELF data section, another
data file loaded at the same time, or a runtime grayscale ramp).

CLUT MODES (--clut)
-------------------
  gray (default)      -- identity grayscale CLUT: i -> (i, i, i, 255). For
                         this game's luminance-ordered indices this is a
                         faithful preview; vertex-color modulation would tint
                         it at draw time.
  auto                -- search the texture's own file for a candidate CLUT
                         and apply it (with PS2 CSM1 swizzle attempted).
                         Falls back to gray when no candidate is found.
  <path>              -- read 1024 raw RGBA bytes from `<path>` and use them.

Use `--no-clut` to force the original grayscale 8-bit PNG output (no color
channel). With `--clut gray` the output is an RGBA PNG that displays
identically but uses color-type 6, suitable for future CLUT remapping.

See docs/FINDINGS.md.

Usage:
  extract_textures.py --in extract --out textures
  extract_textures.py --in extract --out textures --clut auto
  extract_textures.py --in extract --out textures --no-clut   # legacy gray
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

from clut import (
    apply_clut,
    find_best_clut_for_file,
    identity_grayscale_clut,
    psmct8_csm1_swizzle,
    write_png_gray,
    write_png_rgba,
)

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


def _load_clut_arg(spec: str) -> bytes:
    """Resolve a `--clut` argument to a 1024-byte RGBA palette."""
    if spec == "gray":
        return identity_grayscale_clut()
    if spec == "auto":
        return b""  # sentinel: resolved per-file in convert()
    path = Path(spec)
    if not path.is_file():
        raise SystemExit(f"--clut: not a known mode and not a file: {spec}")
    data = path.read_bytes()
    if len(data) != 1024:
        raise SystemExit(f"--clut file must be 1024 bytes (256xRGBA), got {len(data)}")
    return data


def convert(path: Path, d: bytes, start: int, out_dir: Path,
            clut_mode: str, fixed_clut: bytes,
            apply_csm1_swizzle: bool, want_gray_png: bool) -> str | None:
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

    if want_gray_png:
        write_png_gray(out_dir / name, width, height, pixels)
        return f"{path.parent.name}/{path.name}@{start:#x}: {width}x{height} -> {name} (gray)"

    # Color path -- RGBA PNG with a CLUT applied.
    clut = fixed_clut
    clut_source = clut_mode
    if clut_mode == "auto":
        found = find_best_clut_for_file(d, image_payload_range=(off, off + tw * th * 4))
        if found is None:
            clut = identity_grayscale_clut()
            clut_source = "auto -> gray (no candidate)"
        else:
            clut = psmct8_csm1_swizzle(found) if apply_csm1_swizzle else found
            clut_source = f"auto -> file-internal CLUT"
    elif clut_mode != "gray":
        clut = psmct8_csm1_swizzle(fixed_clut) if apply_csm1_swizzle else fixed_clut

    rgba = apply_clut(pixels, clut)
    write_png_rgba(out_dir / name, width, height, rgba)
    return (f"{path.parent.name}/{path.name}@{start:#x}: "
            f"{width}x{height} -> {name} ({clut_source})")


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Extermination texture extractor")
    p.add_argument("--in", dest="input", default="extract", help="extraction directory")
    p.add_argument("--out", default="textures", help="output directory (default: textures/)")
    p.add_argument("--clut", default="gray",
                   help="CLUT source: 'gray' (identity ramp, default), 'auto' "
                        "(search file for a candidate), or a path to a "
                        "1024-byte RGBA palette file")
    p.add_argument("--csm1-swizzle", action="store_true",
                   help="apply the PSMT8 CSM1 CLUT entry-swap (8..15 <-> 16..23 "
                        "in every 32-entry block) before indexing")
    p.add_argument("--no-clut", action="store_true",
                   help="legacy: write 8-bit grayscale PNGs (no CLUT applied, "
                        "no RGBA output -- the original behavior)")
    args = p.parse_args(argv)

    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    if args.no_clut:
        clut_mode = "_gray_png_"
        fixed_clut = b""
    else:
        clut_mode = args.clut
        fixed_clut = _load_clut_arg(args.clut)

    done = 0
    for path in sorted(Path(args.input).rglob("*.bin")):
        d = path.read_bytes()
        for start in find_packet_starts(d):
            line = convert(
                path, d, start, out_dir,
                clut_mode=clut_mode, fixed_clut=fixed_clut,
                apply_csm1_swizzle=args.csm1_swizzle,
                want_gray_png=args.no_clut,
            )
            if line:
                print(line)
                done += 1
    print(f"\n{done} texture packets converted -> {out_dir}/")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
