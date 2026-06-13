#!/usr/bin/env python3
"""export_screen_modules.py - extract the game's full-screen UI "screen
modules" (the TITLE / continue screen and the GAME-OVER screen) into
port-ready `.emui` assets, plus optional PNG previews.

Original tooling for the Extermination decompilation project. Reads only the
user's own locally extracted disc data (extract/) and, optionally, a GS-VRAM
dump from the user's own save state; redistributes nothing. Output lands in
git-ignored asset directories.

WHAT A "SCREEN MODULE" IS (docs/FINDINGS.md "STATUS SCREEN UI TEXTURES")
-----------------------------------------------------------------------
The engine loads a full-screen UI image via `func_001FF080(0, id)`. The
loader (`func_001FF1E0`) reads INDEX.IDX sector `id` as a module header and
pulls the chunk's sections from DATA.DAT, so **module id == DATA.DAT chunk
index** (extract_data.py: "sector i is the descriptor for chunk i"). Two of
these modules are full-screen art screens:

  module 1     = the TITLE / continue screen  (extract/chunk01/f00_id06.bin)
                 EXTERMINATION wordmark + X-ray hand + menu strings.
                 Launched by func_001AC480 sub 0: func_001FF080(0, 1).
  module 0x27  = the GAME-OVER art screen     (extract/chunk39/f00_id00.bin)
                 Launched by func_001AD4E0 sub 1: func_001FF080(0, 0x27).

Each module file carries exactly ONE GS texture-upload packet (a
`07 XX 00 60` GIF/DMA packet: BITBLTBUF/TRXREG/TRXDIR regs then an IMAGE-mode
GIF tag) describing an 8-bit indexed (PSMT8) atlas uploaded through a PSMCT32
host->local transfer. We reuse the exact decode in extract_textures.py:
TRXREG gives w x h (32-bit); deswizzle() runs the GS memory pipeline to
recover the (w*2) x (h*2) PSMT8 indexed atlas.

  module 1     TRXREG 256x384 (32bpp) -> 512x768 indexed atlas.
  module 0x27  TRXREG 256x288 (32bpp) -> 512x576 indexed atlas.

ORIENTATION (verified) — V is correct, H is MIRRORED on disc
------------------------------------------------------------
Two independent axes, and they differ from the resident-VRAM convention:

  * VERTICAL: unlike the resident PSMT4 hub-decor sprites in export_ui.py
    (which the engine stores vertically flipped and the exporter un-flips),
    these DATA.DAT full-screen atlases are stored ALREADY SCREEN-ORIENTED
    top-down: the raw deswizzle output reads correctly (EXTERMINATION
    wordmark at the top, footer text mid-screen). This matches
    extract_textures.py (NO V-flip) and is confirmed byte-for-byte against
    textures/chunk01_f00_id06.png. Default: NO vertical flip. `--vflip` is
    offered for completeness but is not needed for the known modules.

  * HORIZONTAL: the on-disc atlas is a SPRITE SHEET, not a single
    full-screen background — different regions are blitted with their own UVs
    at draw time and some are stored horizontally MIRRORED. In the raw decode
    the footer copyright line reads left-right mirrored while the
    EXTERMINATION wordmark / "GAME OVER" headings read correct (and a global
    H-flip swaps which set is mirrored), so NO single global flip makes the
    whole atlas read correctly. The faithful, reproducible decode is the raw
    deswizzle, which is exactly what the reproduce anchor
    (textures/chunk01_f00_id06.png) holds — that is why our default decode
    matches it byte-for-byte. So we DEFAULT to NO horizontal flip and export
    the atlas verbatim; the port's title-menu wiring (added later) is
    responsible for the per-sprite UV/flip layout when it composites this
    sheet. `--hflip` is offered for inspection of the mirrored regions.

COLOR (important limitation, docs/FINDINGS.md)
----------------------------------------------
The title atlas is MULTI-PALETTE and its CLUTs are engine-synthesised into GS
VRAM at runtime; session 26 proved there is NO matching CLUT blob on disc.
Therefore color is only recoverable from a live GS-VRAM dump (the same model
as export_ui.py's `--gs`). There is currently NO such title-screen capture
available, so color CANNOT be fully verified at this time.

  * DEFAULT output is the IDENTITY-GRAYSCALE atlas (clut[i] = (i,i,i,255)).
    The PSMT8 indices in this game are luminance-ordered, so this is a
    faithful luminance preview and the established, fully-reproducible-from-
    disc convention (it matches textures/chunk01_f00_id06.png exactly).
  * `--gs <gs.bin> --cbp <CBP>` enables an optional color path that mirrors
    export_ui's resident-CLUT read: it locates the 4 MB GS local memory in
    the freeze blob (base = len - 0x400000 - 84), reads the 256-entry
    PSMCT32 CLUT at CBP (256-byte blocks), CSM1-unswizzles it (entries
    8..15 <-> 16..23 per 32-entry group), scales GS alpha 0..0x80 -> 0..255,
    and applies it. UNVERIFIED for these modules (no capture exists yet);
    use only when you have a genuine title/game-over VRAM dump and have
    identified the module's CBP from a TEX0 in that frame.

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
              s16 x, y               (draw position on the canvas)
              u16 dw, dh             (draw size on the canvas)
  ...  sheet_w*sheet_h*4  RGBA8 sheet, rows top-down, SCREEN-oriented

A screen module is written as ONE full-screen sprite record: the sheet is the
whole atlas, (u,v) = (0,0), (w,h) = atlas size, canvas (x,y) = (0,0), and
draw (dw,dh) = atlas size. The port draws it as a single full-screen overlay.

USAGE (macOS arm64, repo root)
    # grayscale (default, fully reproducible from your own disc):
    python3 tools/export_screen_modules.py --module 1 \
        --out-dir ../extermination-port/assets --png scratch/screens
    python3 tools/export_screen_modules.py --module 0x27 \
        --out-dir ../extermination-port/assets --png scratch/screens

    # color (only with a real title/game-over GS-VRAM dump + its CBP):
    python3 tools/export_screen_modules.py --module 1 \
        --gs scratch/state_title/gs.bin --cbp 0x1234 \
        --out-dir ../extermination-port/assets

See docs/FINDINGS.md.
"""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))

from clut import (                                    # noqa: E402
    apply_clut,
    identity_grayscale_clut,
    write_png_rgba,
)
from extract_textures import (                        # noqa: E402
    deswizzle,
    find_image_block,
    find_packet_starts,
    first_trxreg,
)

# PCSX2 GS freeze blob: 4 MB local memory + 84 trailing state bytes
# (gs_vram.py rule, mirrored by export_ui.py). Local-memory base is
# len(gs.bin) - 0x400000 - 84.
GS_LM_SIZE = 0x400000
GS_TRAILER = 84

# Friendly default names for the two known full-screen modules.
KNOWN_NAMES = {
    1: "title",         # TITLE / continue screen  (chunk01)
    0x27: "gameover",   # GAME-OVER art screen     (chunk39)
}


def find_module_file(extract_dir: Path, module_id: int) -> Path:
    """Locate the chunk file for a screen module id (== chunk index).

    Each screen-module chunk dir holds the module's section files (f<NN>_id<II>
    .bin). The full-screen art is the section carrying the GS texture packet;
    in practice these chunks contain a single .bin, so we pick the (only)
    file that has a `07 XX 00 60` packet at all.
    """
    chunk_dir = extract_dir / f"chunk{module_id:02d}"
    if not chunk_dir.is_dir():
        sys.exit(
            f"error: {chunk_dir} not found - module {module_id} (chunk "
            f"{module_id}) has not been extracted. Run tools/extract_data.py "
            "against your own disc first."
        )
    bins = sorted(chunk_dir.glob("*.bin"))
    if not bins:
        sys.exit(f"error: {chunk_dir} has no .bin section files")
    # Prefer the file(s) that actually carry a GS texture packet.
    with_packet = [p for p in bins if find_packet_starts(p.read_bytes())]
    if not with_packet:
        sys.exit(
            f"error: no GS texture packet ('07 XX 00 60') found in any "
            f"section of {chunk_dir} - is module {module_id} really a "
            "full-screen image module?"
        )
    if len(with_packet) > 1:
        names = ", ".join(p.name for p in with_packet)
        print(
            f"note: {chunk_dir.name} has multiple sections with a GS packet "
            f"({names}); using the first ({with_packet[0].name}).",
            file=sys.stderr,
        )
    return with_packet[0]


def decode_atlas(data: bytes) -> tuple[int, int, int, int, int, bytes]:
    """Decode the module's single GS texture packet into a PSMT8 atlas.

    Returns (packet_off, trx_w, trx_h, atlas_w, atlas_h, indices) where
    `indices` is the (atlas_w*atlas_h) 8-bit index plane (no CLUT applied,
    no flip applied).
    """
    starts = find_packet_starts(data)
    if not starts:
        sys.exit("error: no '07 XX 00 60' GS texture packet in the module")
    start = starts[0]
    if len(starts) > 1:
        print(
            f"note: module has {len(starts)} GS packets "
            f"({', '.join(hex(s) for s in starts)}); decoding the first "
            f"({start:#x}) as the full-screen image.",
            file=sys.stderr,
        )
    trx = first_trxreg(data, start)
    if trx is None:
        sys.exit(f"error: no TRXREG (0x52) after the packet at {start:#x}")
    tw, th = trx
    if tw == 0 or th == 0 or tw % 64 or th % 32:
        sys.exit(f"error: implausible TRXREG {tw}x{th} (must be 64x32 aligned)")
    off = find_image_block(data, tw * th * 4, start)
    if off is None:
        sys.exit(f"error: no matching IMAGE GIF block for TRXREG {tw}x{th}")
    aw, ah, idx = deswizzle(data[off:off + tw * th * 4], tw, th)
    return start, tw, th, aw, ah, idx


def vflip_plane(idx: bytes, w: int, h: int) -> bytes:
    """Vertically flip an 8-bit index plane (row 0 <-> row h-1)."""
    out = bytearray(len(idx))
    for y in range(h):
        out[y * w:(y + 1) * w] = idx[(h - 1 - y) * w:(h - 1 - y + 1) * w]
    return bytes(out)


def hflip_plane(idx: bytes, w: int, h: int) -> bytes:
    """Horizontally flip an 8-bit index plane (col 0 <-> col w-1).

    Un-mirrors the on-disc H-mirroring so the screen reads left-to-right.
    """
    out = bytearray(len(idx))
    for y in range(h):
        row = idx[y * w:(y + 1) * w]
        out[y * w:(y + 1) * w] = row[::-1]
    return bytes(out)


def read_gs_clut(gs_path: Path, cbp: int) -> bytes:
    """Read a resident 256-entry PSMCT32 CLUT at `cbp` from a GS freeze blob.

    Mirrors export_ui.py's resident-CLUT path: locate the 4 MB GS local
    memory (base = len - 0x400000 - 84), read 1024 bytes at cbp*256, CSM1-
    unswizzle, and scale GS alpha 0..0x80 -> 0..255. Returns 1024 RGBA bytes.
    """
    from clut_pair import read_clut_rgba  # noqa: E402  (csm1-unswizzle + a*2)

    gs = gs_path.read_bytes()
    base = len(gs) - GS_LM_SIZE - GS_TRAILER
    if base < 0:
        sys.exit(f"error: {gs_path} is smaller than a GS freeze blob")
    lm = gs[base:base + GS_LM_SIZE]
    if cbp * 256 + 1024 > len(lm):
        sys.exit(f"error: CBP {cbp:#x} is out of GS local memory range")
    return read_clut_rgba(lm, cbp)


def write_emui(out_path: Path, atlas_w: int, atlas_h: int, rgba: bytes) -> None:
    """Write a one-record full-screen .emui v1 (sheet == the whole atlas)."""
    out_path.parent.mkdir(parents=True, exist_ok=True)
    # single record: u=v=0, w/h = atlas, canvas (0,0), draw = atlas size.
    record = struct.pack("<4H2h2H", 0, 0, atlas_w, atlas_h, 0, 0,
                         atlas_w, atlas_h)
    with out_path.open("wb") as f:
        f.write(b"EMUI")
        f.write(struct.pack("<4I", 1, atlas_w, atlas_h, 1))
        f.write(record)
        f.write(rgba)


def parse_module_id(spec: str) -> int:
    """Parse a module id given as decimal or 0x-prefixed hex."""
    try:
        return int(spec, 0)
    except ValueError:
        sys.exit(f"error: --module must be an int (decimal or 0x-hex): {spec}")


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--module", required=True,
                    help="screen-module id == DATA.DAT chunk index "
                         "(decimal or 0x-hex; e.g. 1 or 0x27)")
    ap.add_argument("--in", dest="input", default="extract",
                    help="extract/ root with the chunk dirs (default: extract)")
    ap.add_argument("--out-dir", default="../extermination-port/assets",
                    help="output dir for the .emui "
                         "(default: ../extermination-port/assets)")
    ap.add_argument("--png", metavar="DIR",
                    help="also write an RGBA PNG preview into DIR")
    ap.add_argument("--name",
                    help="output basename (default: 'title' for module 1, "
                         "'gameover' for module 0x27, else 'screen_<id>')")
    ap.add_argument("--gs", metavar="PATH",
                    help="optional GS freeze blob (gs.bin) for the COLOR path "
                         "(requires --cbp); default is grayscale")
    ap.add_argument("--cbp", metavar="CBP",
                    help="resident-CLUT base pointer (256-byte blocks) inside "
                         "--gs; decimal or 0x-hex. Required with --gs.")
    ap.add_argument("--vflip", action="store_true",
                    help="vertically flip the atlas (NOT needed for the known "
                         "full-screen modules - they are stored screen-"
                         "oriented; offered for completeness)")
    ap.add_argument("--hflip", action="store_true",
                    help="horizontally flip the whole atlas (NOT a correct "
                         "global fix - the sheet has per-sprite mirroring; "
                         "offered only to inspect the mirrored regions). "
                         "Default exports the raw, anchor-identical decode.")
    args = ap.parse_args(argv)

    module_id = parse_module_id(args.module)
    extract_dir = Path(args.input)

    # --- locate + decode the module's atlas (index plane) -----------------
    src = find_module_file(extract_dir, module_id)
    data = src.read_bytes()
    packet_off, tw, th, atlas_w, atlas_h, idx = decode_atlas(data)

    if args.vflip:
        idx = vflip_plane(idx, atlas_w, atlas_h)
    if args.hflip:
        idx = hflip_plane(idx, atlas_w, atlas_h)

    # --- choose the CLUT --------------------------------------------------
    if args.gs:
        if not args.cbp:
            sys.exit("error: --gs requires --cbp (the resident CLUT base)")
        cbp = parse_module_id(args.cbp)
        clut = read_gs_clut(Path(args.gs), cbp)
        clut_desc = f"resident GS CLUT @cbp {cbp:#x} from {args.gs} (UNVERIFIED)"
    else:
        clut = identity_grayscale_clut()
        clut_desc = "identity grayscale (default; reproducible from disc)"

    rgba = apply_clut(idx, clut)

    # --- output names -----------------------------------------------------
    name = args.name or KNOWN_NAMES.get(module_id, f"screen_{module_id}")
    out_dir = Path(args.out_dir)
    out_path = out_dir / f"{name}.emui"

    write_emui(out_path, atlas_w, atlas_h, rgba)

    png_note = ""
    if args.png:
        png_dir = Path(args.png)
        png_dir.mkdir(parents=True, exist_ok=True)
        png_path = png_dir / f"{name}.png"
        write_png_rgba(png_path, atlas_w, atlas_h, rgba)
        png_note = f"  + PNG {png_path}"

    emui_bytes = out_path.stat().st_size
    print(
        f"module {module_id} (0x{module_id:02x}, {src.parent.name}/{src.name}): "
        f"packet @{packet_off:#x}  TRXREG {tw}x{th} (32bpp)  -> atlas "
        f"{atlas_w}x{atlas_h}  vflip={'yes' if args.vflip else 'no'} "
        f"hflip={'yes' if args.hflip else 'no'}  clut={clut_desc}"
    )
    print(f"  wrote {out_path} ({emui_bytes} bytes){png_note}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
