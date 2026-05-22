#!/usr/bin/env python3
"""Extermination (SCUS-97112) per-texture (sub-texture) extractor.

Original tooling for the Extermination decompilation project. Reads only the
user's locally extracted disc data; redistributes nothing.

WHAT THIS TOOL DOES
-------------------
`extract_textures.py` decodes the game's GS texture-upload packets into wide
texture-atlas SHEETS. This tool goes one step further: it binds each mesh
material to the specific sheet it samples, then cuts that material's UV
sub-rectangle out of the sheet as an individual texture PNG.

It needs two things from the geometry:
  * the per-strip MARKER (m0/m1) -- the material key, and
  * the per-vertex UVs,
both re-parsed here directly from the `id 0x44` level files using the format
documented in `docs/FINDINGS.md` and the `extract_models.py` docstring. This
tool does NOT import or modify `extract_models.py`.

----------------------------------------------------------------------------
THE MARKER -> TEXTURE-PACKET BINDING  (reverse-engineered, validated)
----------------------------------------------------------------------------
A triangle strip's 64-byte marker row is `<u32 m0> <u32 m1> 00*8`. m0 and m1
are constant within a strip. m1's high byte is the strip flag (0x00/0x20) and
is masked off; m0 is the load-bearing field for texturing.

m0 decomposes as a bitfield. The middle 14 bits

      sheet_field = (m0 >> 15) & 0x3FFF

is a SHEET-SELECT key: a GS VRAM base address. Empirically it takes only a
handful of distinct values per level (3-8), and it partitions every material
in the level into that many groups. The remaining m0 bits (low 15) and m1 are
near-unique per material -- running indices / pointers -- and carry no
sub-rectangle, so the within-sheet placement of a material comes from its UVs
(below), not from the marker.

A GS texture upload is a BITBLTBUF (reg 0x50, carrying the destination base
pointer DBP) + TRXREG (reg 0x52, carrying the transfer w x h) pair. Scanning
a level directory for those pairs yields the level's texture transfers.

THE BINDING. A transfer's GS destination pointer DBP and the marker's
`sheet_field` are two encodings of the same VRAM base address, related by a
fixed affine map

      predicted_sheet_field = DBP * (2048 / 1920) - 584.8

(derived from, and exact on, the universal trio of {DBP, sheet_field} pairs
{10752: 10884, 12672: 12932, 14592: 14980} -- the slot triple every level
reuses). Each decoded transfer is therefore bound to the marker group whose
`sheet_field` matches its predicted value (snapped to the nearest actual
group, with a small tolerance to reject a bad match).

Lower sheet_field groups (e.g. 8804, 1811, 644 ...) reference textures that
are resident in GS VRAM from an upload made elsewhere (a standalone UI/common
packet, or a model file) -- the level directory has no transfer for them, so
those materials are reported as UNBOUND and skipped. This is a real, honest
limitation: resolving them needs the cross-file VRAM map from the engine code.

----------------------------------------------------------------------------
PER-TEXTURE EXTRACTION
----------------------------------------------------------------------------
Each bound material's vertices carry UVs. About 79% of materials sample the
full [0,1]x[0,1] of their sheet (UVs >1 are hardware wrap/tiling); ~21% use a
genuine sub-rectangle. For every bound material this tool:

  * computes the UV bounding box, clamps it to [0,1] (tiled UVs would
    otherwise run off the sheet -- the visible texel set is still [0,1]),
  * scales it by the bound sheet's pixel size to a pixel rectangle,
  * crops that rectangle out of the decoded sheet and writes it as a PNG.

A material whose clamped UV box still covers ~the whole sheet yields a copy of
the sheet; a material with a real sub-range yields a clean individual texture.
Output PNGs are grayscale 8-bit -- faithful to the index data, exactly as
`extract_textures.py` (the runtime CLUT is unresolved; see docs/FINDINGS.md).

The GS deswizzle pipeline is re-implemented here (small, identical to
`extract_textures.py`'s -- verified byte-exact against the standard
`unswizzle8`) so this tool stands alone among the texture tooling.

Usage:
  extract_subtextures.py --in extract --out subtextures
  extract_subtextures.py --in extract --out subtextures --sheets   # also dump full sheets
"""
from __future__ import annotations

import argparse
import struct
import sys
import zlib
from collections import defaultdict
from pathlib import Path

# ---------------------------------------------------------------------------
# GS swizzle pipeline -- documented hardware tables. Identical to the tables in
# extract_textures.py (proven byte-exact vs. the standard unswizzle8).
# ---------------------------------------------------------------------------
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
    page = (y // 32) * ppr + (x // 64)
    px = (y % 32 % 8) * 8 + (x % 64 % 8)
    block = PAGE32[(y % 32 // 8) * 8 + (x % 64 // 8)]
    return page * 2048 + block * 64 + (px // 16) * 16 + COL32[px % 16]


def psmt8_byte(x: int, y: int, ppr: int) -> int:
    page = (y // 64) * ppr + (x // 128)
    px = (y % 64 % 16) * 16 + (x % 128 % 16)
    block = PAGE32[(y % 64 // 16) * 8 + (x % 128 // 16)]
    return page * 8192 + block * 256 + (px // 64) * 64 + COL8[px % 128]


def deswizzle(payload: bytes, tw: int, th: int) -> tuple[int, int, bytes]:
    """PSMCT32 transfer payload -> simulated VRAM -> PSMT8 read.

    `payload` is the raster PSMCT32 transfer (tw*th*4 bytes). Returns the
    (width, height, pixels) of the resulting (tw*2) x (th*2) 8-bit texture.
    """
    ow, oh = tw * 2, th * 2
    word_src: dict[int, tuple[int, int]] = {}
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


def write_png_gray(path: Path, width: int, height: int, pixels: bytes) -> None:
    """Write an 8-bit grayscale PNG."""
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


# ---------------------------------------------------------------------------
# GS texture-transfer scanning
# ---------------------------------------------------------------------------
class Transfer:
    """One GS texture upload: a BITBLTBUF + TRXREG pair, with decoded sheet."""

    __slots__ = ("src_file", "dbp", "tw", "th", "trxreg_off",
                 "width", "height", "pixels")

    def __init__(self, src_file: str, dbp: int, tw: int, th: int,
                 trxreg_off: int):
        self.src_file = src_file
        self.dbp = dbp              # GS destination base pointer (sort key)
        self.tw = tw                # TRXREG transfer width  (PSMCT32 texels)
        self.th = th                # TRXREG transfer height (PSMCT32 texels)
        self.trxreg_off = trxreg_off  # file offset of this transfer's TRXREG
        self.width = tw * 2         # decoded 8-bit texture dimensions
        self.height = th * 2
        self.pixels: bytes | None = None


def _gs_regwrite(d: bytes, pos: int) -> int | None:
    """If the 16-byte row at `pos` is a GS register write, return the reg id."""
    addr = int.from_bytes(d[pos + 8:pos + 16], "little")
    if addr >> 8 == 0:
        return addr & 0xFF
    return None


def scan_transfers(d: bytes, src_file: str) -> list[Transfer]:
    """Find every GS texture upload in a file.

    A texture upload is a BITBLTBUF (reg 0x50, carrying DBP) followed within a
    few rows by a TRXREG (reg 0x52, carrying transfer w x h). This catches
    both `07 XX 00 60` and `07 XX 00 10` DMA-tagged packets (the latter are
    missed by extract_textures.py's narrower `60`-only signature scan).
    """
    out: list[Transfer] = []
    for pos in range(0, len(d) - 16, 16):
        if _gs_regwrite(d, pos) != 0x50:        # BITBLTBUF
            continue
        v = int.from_bytes(d[pos:pos + 8], "little")
        dbp = (v >> 32) & 0x3FFF
        # TRXREG should follow within ~8 rows.
        for q in range(pos + 16, min(pos + 16 * 8, len(d) - 16), 16):
            if _gs_regwrite(d, q) != 0x52:      # TRXREG
                continue
            data = int.from_bytes(d[q:q + 8], "little")
            tw, th = data & 0xFFF, (data >> 32) & 0xFFF
            if 0 < tw <= 2048 and 0 < th <= 2048 and tw % 64 == 0 and th % 32 == 0:
                out.append(Transfer(src_file, dbp, tw, th, q))
            break
    return out


def find_image_payload(d: bytes, nbytes: int, start: int) -> int | None:
    """Offset of the IMAGE-mode GIF payload of `nbytes` bytes after `start`.

    Accepts a payload whose declared `nloop*16` equals `nbytes` even if the
    file is too short to hold all of it -- a few level files truncate their
    second texture sheet at the archive region boundary. The caller pads the
    missing tail; the visible top portion of the sheet still decodes.
    """
    for pos in range(start, len(d) - 16, 16):
        if (d[pos + 7] >> 2) & 3 == 2:          # GIF tag FLG == IMAGE
            nloop = (d[pos] | (d[pos + 1] << 8)) & 0x7FFF
            if nloop * 16 == nbytes:
                return pos + 16
    return None


def decode_transfer(d: bytes, t: Transfer) -> bool:
    """Decode a transfer's pixels into t.pixels. Returns True on success.

    Anchors the search at the transfer's own TRXREG offset (recorded during
    scanning, so transfers with identical dimensions are not confused), finds
    the IMAGE GIF payload after it, and runs the GS deswizzle. `d` must be the
    file `t` was scanned from. A payload truncated by the file boundary is
    zero-padded to its declared size before deswizzling.
    """
    nbytes = t.tw * t.th * 4
    off = find_image_payload(d, nbytes, t.trxreg_off)
    if off is None:
        return False
    payload = d[off:off + nbytes]
    if len(payload) < nbytes:
        payload = payload + b"\x00" * (nbytes - len(payload))
    _, _, t.pixels = deswizzle(payload, t.tw, t.th)
    return True


# ---------------------------------------------------------------------------
# Geometry re-parse: markers + UVs (level `id 0x44` files)
#
# Re-implemented per docs/FINDINGS.md / the extract_models.py docstring; this
# tool does not import extract_models.py. Only the data needed for texturing
# is decoded: each strip's marker key and its per-vertex UVs.
# ---------------------------------------------------------------------------
SEPARATOR = b"\x00\x00\x00\x17" + b"\x00" * 12
MESH_SIG = bytes.fromhex("040400010080806c")
SUBMESH_SIG = bytes.fromhex("4d040000")
VERT_SIZE = 0x40
W_TOLERANCE = 0.05


def _block_bounds(d: bytes) -> list[tuple[int, int]]:
    seps = []
    i = d.find(SEPARATOR)
    while i != -1:
        seps.append(i)
        i = d.find(SEPARATOR, i + 1)
    if not seps:
        return []
    edges = [0] + seps + [len(d)]
    return [(edges[i], edges[i + 1]) for i in range(len(edges) - 1)]


def _mesh_vertex_start(d: bytes, start: int, end: int, is_first: bool) -> int | None:
    if is_first:
        return start
    desc = d[start + 0x10:start + 0x20]
    if len(desc) < 16:
        return None
    if desc[8:16] == MESH_SIG:
        return start + 0x20
    if desc[8:12] == SUBMESH_SIG:
        p = d.find(MESH_SIG, start, end)
        return None if p < 0 else p + 8
    return None


class MatUV:
    """Accumulated UV bounding box for one material (marker key)."""

    __slots__ = ("u0", "v0", "u1", "v1", "verts")

    def __init__(self):
        self.u0 = self.v0 = 1e9
        self.u1 = self.v1 = -1e9
        self.verts = 0

    def add(self, u: float, v: float) -> None:
        self.u0 = min(self.u0, u)
        self.v0 = min(self.v0, v)
        self.u1 = max(self.u1, u)
        self.v1 = max(self.v1, v)
        self.verts += 1


# A material is only counted if it is carried by at least one real triangle
# strip -- a run of >= MIN_STRIP_VERTS valid vertices sharing a marker key.
# This drops isolated rows that coincidentally pass the |w|~=1 validity test
# but are header/padding, matching extract_models.py's >=3-vertex strip rule.
MIN_STRIP_VERTS = 3


def _block_strips(d: bytes, vstart: int, end: int):
    """Yield (marker_key, [(u, v), ...]) for each triangle strip in a block.

    Mirrors extract_models.py's strip segmentation: a new strip begins at a
    flag-0x00 vertex after a non-0x00 one, or when the marker key changes, or
    after an invalid (|w| != 1) row. Strips shorter than MIN_STRIP_VERTS are
    dropped.
    """
    cur_key = None
    cur_uvs: list[tuple[float, float]] = []
    prev_flag: int | None = None

    def flush():
        if cur_key is not None and len(cur_uvs) >= MIN_STRIP_VERTS:
            yield cur_key, cur_uvs

    o = vstart
    while o + VERT_SIZE <= end:
        w = struct.unpack_from("<f", d, o + 0x3C)[0]
        if abs(abs(w) - 1.0) < W_TOLERANCE:
            flag = d[o + 7]
            m0, m1 = struct.unpack_from("<II", d, o)
            key = (m0, m1 & 0x00FFFFFF)
            u, v = struct.unpack_from("<2f", d, o + 0x10)
            new_strip = (
                cur_key is None
                or (flag == 0 and prev_flag is not None and prev_flag != 0)
                or key != cur_key
            )
            if new_strip:
                yield from flush()
                cur_key, cur_uvs = key, []
            cur_uvs.append((u, v))
            prev_flag = flag
        else:
            yield from flush()
            cur_key, cur_uvs, prev_flag = None, [], None
        o += VERT_SIZE
    yield from flush()


def parse_level_materials(d: bytes) -> dict[int, dict[tuple[int, int], MatUV]]:
    """Map sheet_field -> {marker_key -> MatUV} for a level `id 0x44` file.

    sheet_field = (m0 >> 15) & 0x3FFF -- the texture-sheet-select key.
    marker_key  = (m0, m1 & 0xFFFFFF) -- the per-material identity.

    Only materials carried by a real triangle strip are kept (see
    MIN_STRIP_VERTS), so spurious lone rows do not invent phantom sheets.
    """
    out: dict[int, dict[tuple[int, int], MatUV]] = defaultdict(dict)
    for idx, (start, end) in enumerate(_block_bounds(d)):
        vstart = _mesh_vertex_start(d, start, end, is_first=(idx == 0))
        if vstart is None:
            continue
        for key, uvs in _block_strips(d, vstart, end):
            sheet = (key[0] >> 15) & 0x3FFF
            mats = out[sheet]
            mat = mats.get(key)
            if mat is None:
                mat = mats[key] = MatUV()
            for u, v in uvs:
                mat.add(u, v)
    return out


# ---------------------------------------------------------------------------
# Per-texture extraction
# ---------------------------------------------------------------------------
def crop(t: Transfer, px0: int, py0: int, px1: int, py1: int) -> tuple[int, int, bytes]:
    """Crop a pixel rectangle out of a decoded transfer's sheet."""
    px0 = max(0, min(t.width, px0))
    px1 = max(0, min(t.width, px1))
    py0 = max(0, min(t.height, py0))
    py1 = max(0, min(t.height, py1))
    cw, ch = px1 - px0, py1 - py0
    if cw <= 0 or ch <= 0:
        return 0, 0, b""
    src = t.pixels
    out = bytearray(cw * ch)
    for y in range(ch):
        s = (py0 + y) * t.width + px0
        out[y * cw:(y + 1) * cw] = src[s:s + cw]
    return cw, ch, bytes(out)


# Affine map from a transfer's GS DBP to the marker's sheet_field key, and
# the tolerance for snapping a prediction to an actual marker group. Derived
# from the universal {DBP: sheet_field} trio {10752:10884, 12672:12932,
# 14592:14980}; exact on it.
DBP_TO_SHEET_SCALE = 2048.0 / 1920.0
DBP_TO_SHEET_BIAS = -584.8
SHEET_SNAP_TOLERANCE = 64        # sheet_field units


def predict_sheet_field(dbp: int) -> float:
    """The marker sheet_field a transfer with this DBP should bind to."""
    return dbp * DBP_TO_SHEET_SCALE + DBP_TO_SHEET_BIAS


def process_level(level_file: Path, out_dir: Path,
                   dump_sheets: bool) -> tuple[int, int, list[str]]:
    """Bind markers to texture packets and cut per-material sub-textures.

    Returns (textures_written, materials_unbound, log_lines).
    """
    log: list[str] = []
    d_level = level_file.read_bytes()
    by_sheet = parse_level_materials(d_level)
    if not by_sheet:
        return 0, 0, log

    # Collect every texture transfer in the level's directory, decode each.
    transfers: list[Transfer] = []
    for q in sorted(level_file.parent.glob("*.bin")):
        dq = q.read_bytes()
        for t in scan_transfers(dq, q.name):
            if decode_transfer(dq, t):
                transfers.append(t)
    # Each scanned transfer is distinct (anchored at its own TRXREG offset);
    # keep all decoded ones, sorted by DBP.
    uniq = sorted(transfers, key=lambda t: t.dbp)

    # THE BINDING: map each transfer's DBP through the affine relation to a
    # predicted sheet_field, then snap to the nearest actual marker group.
    sheet_fields = sorted(by_sheet.keys())
    bound: dict[int, Transfer] = {}
    for t in uniq:
        pred = predict_sheet_field(t.dbp)
        best = min(sheet_fields, key=lambda f: abs(f - pred))
        if abs(best - pred) <= SHEET_SNAP_TOLERANCE and best not in bound:
            bound[best] = t

    tag = f"{level_file.parent.name}_{level_file.stem}"
    written = 0
    unbound = 0
    for sheet_field in sheet_fields:
        mats = by_sheet[sheet_field]
        t = bound.get(sheet_field)
        if t is None:
            unbound += len(mats)
            log.append(f"  sheet {sheet_field}: {len(mats)} material(s) "
                       f"UNBOUND (texture resident from another file) -- skipped")
            continue
        if dump_sheets:
            sp = out_dir / f"{tag}_sheet_{sheet_field}.png"
            write_png_gray(sp, t.width, t.height, t.pixels)
        log.append(f"  sheet {sheet_field}: {len(mats)} material(s) -> "
                   f"{t.src_file} {t.width}x{t.height} (DBP {t.dbp})")
        for mi, (key, mat) in enumerate(sorted(mats.items())):
            # Clamp UV box to [0,1]: UVs >1 are hardware wrap/tiling; the
            # visible texel set is still the [0,1] sheet.
            u0, v0 = max(0.0, mat.u0), max(0.0, mat.v0)
            u1, v1 = min(1.0, mat.u1), min(1.0, mat.v1)
            if u1 <= u0 or v1 <= v0:
                continue
            px0 = int(round(u0 * t.width))
            px1 = int(round(u1 * t.width))
            py0 = int(round(v0 * t.height))
            py1 = int(round(v1 * t.height))
            cw, ch, pix = crop(t, px0, py0, px1, py1)
            if cw < 2 or ch < 2:
                continue
            name = (f"{tag}_s{sheet_field}_m{mi:03d}_"
                    f"{key[0]:08x}_{cw}x{ch}.png")
            write_png_gray(out_dir / name, cw, ch, pix)
            written += 1
    return written, unbound, log


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(
        description="Extermination per-texture (sub-texture) extractor")
    p.add_argument("--in", dest="input", default="extract",
                   help="extraction directory (default: extract)")
    p.add_argument("--out", default="subtextures",
                   help="output directory (default: subtextures/)")
    p.add_argument("--sheets", action="store_true",
                   help="also dump each full bound sheet alongside the crops")
    args = p.parse_args(argv)

    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    total_written = 0
    total_unbound = 0
    levels = 0
    for level_file in sorted(Path(args.input).rglob("*_id44.bin")):
        written, unbound, log = process_level(level_file, out_dir, args.sheets)
        if not log:
            continue
        levels += 1
        print(f"{level_file.parent.name}/{level_file.name}:")
        for line in log:
            print(line)
        total_written += written
        total_unbound += unbound

    print(f"\n{total_written} per-texture PNG(s) from {levels} level(s) "
          f"-> {out_dir}/")
    print(f"{total_unbound} material(s) unbound (texture uploaded by another "
          f"file -- needs the engine VRAM map)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
