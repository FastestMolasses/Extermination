#!/usr/bin/env python3
"""export_level.py — export a LEVEL RENDER MESH as a static EMDL v2 (.emdl)
asset for the native port (extermination-port).

THE LEVEL RENDER MESH (decoded 2026-06-09, session 6). The 32 `id 0x44`
files are NOT what the engine draws: each level chunk also carries a
VU1-ready *render mesh* file — a flat array of 64-byte records framed in
0x880-byte blocks — and THAT is the drawn level. For the office scene
(the GS-dump frame) it is `extract/chunk06.n1/f03_id43.bin`:

  record = 4 rows of 16 bytes, in this order (NOT the id44/character order):
    +0x00  vec4 world-space position (x, y, z, w);  |w| ~= 1.0.
           The position W read as an integer carries flags exactly like the
           character kernel: bits 0..9 = VU1 dmem matrix slot (0 = static,
           nonzero = movable sub-object), bit 15 = ADC/restart — SET on a
           strip's priming vertices and on the duplicate-record padding
           that fills each 0x880 block, CLEAR on triangle-emitting
           vertices (matches the GS dump: parked/culled blocks stream as
           all-ADC writes, visible strips as all-kick runs).
    +0x10  the draw's full TEX0 register value (8 bytes, CBP included,
           verbatim — all 84 level TEX0 pairings of the live GS dump appear
           in this file) + 8 zero bytes.
    +0x20  vec4 (u, v, 1.0, 0.0) — normalized per-texture ST, REPEAT.
    +0x30  vec4 baked vertex color (r, g, b, 1.0), the level's baked
           lighting — OR a unit normal (nx, ny, nz, 0.0) on some records
           (same |xyz|~=1 disambiguation as the id44 geometry).

  Every 0x880 bytes a 2-row block header interrupts the record grid
  (`01 00 00 00 ..` word quad + an AABB row); the walker just resyncs on
  the POS+TEX0 validity test, so exact framing is not load-bearing.

The output is one EMDL v2 file with bone_count 1 (identity palette),
frame_count 1, per-vertex texture ids, and the baked vertex color stored
in the EMD2 *normal* slot (the port's fragment shader distinguishes
color-vs-normal per vertex by |v| ~= 1 — the same test the disc data
itself requires). Textures resolve to RGBA8 through a one-frame PCSX2 GS
dump's VRAM snapshot (PSMT4 + 16-entry CLUT / PSMT8 + CLUT / PSMCT32),
reusing parse_gsdump + clut_pair + extract_textures as libraries.

Disc-derived output: write only into git-ignored locations
(extermination-port/assets/ is ignored there).

Usage (macOS arm64, decomp repo root):
  .venv/bin/python tools/export_level.py \
      --level extract/chunk06.n1/f03_id43.bin \
      --gsdump extract/gsdump/frame1.gs \
      --out ../extermination-port/assets/scene/00_level.emdl
"""
from __future__ import annotations

import argparse
import importlib.util
import math
import struct
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parent


def _load(name: str, fname: str):
    spec = importlib.util.spec_from_file_location(name, TOOLS / fname)
    mod = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[name] = mod
    spec.loader.exec_module(mod)
    return mod


en = _load("_export_native", "export_native.py")   # tex0_fields, write_emdl

REC = 64
W_TOL = 0.25


# ---------------------------------------------------------------------------
# Record walker

def walk_records(d: bytes):
    """Yield (pos, wbits, tex0_qword, uv, attr) for every valid 64-byte
    record, or None once per skipped-row run (the 0x880 block headers) so
    the caller can break the running strip; resyncs by 16 bytes."""
    o, n = 0, len(d)
    skipping = False
    while o + REC <= n:
        w = struct.unpack_from("<f", d, o + 0x0C)[0]
        is_tex = (d[o + 0x18:o + 0x20] == b"\x00" * 8 and
                  d[o + 0x10:o + 0x18] != b"\x00" * 8)
        st_zw = struct.unpack_from("<2f", d, o + 0x28)
        is_st = st_zw[0] == 1.0 and st_zw[1] == 0.0
        if abs(abs(w) - 1.0) < W_TOL and is_tex and is_st:
            pos = struct.unpack_from("<3f", d, o)
            wbits = struct.unpack_from("<I", d, o + 0x0C)[0]
            q = int.from_bytes(d[o + 0x10:o + 0x18], "little")
            uv = struct.unpack_from("<2f", d, o + 0x20)
            attr = struct.unpack_from("<4f", d, o + 0x30)
            yield pos, wbits, q, uv, attr
            o += REC
            skipping = False
        else:
            if not skipping:
                yield None
                skipping = True
            o += 16     # header / padding row — resync
    return


def attr_to_color(attr) -> tuple:
    """Baked color, or a stand-in shade for normal-carrying records.

    |xyz| ~= 1 with w ~= 0 marks a unit NORMAL (dynamic sub-meshes inside
    the level file); convert it to a grayscale color with the same
    directional stand-in light the port uses, so the EMD2 normal slot can
    carry colors uniformly."""
    x, y, z, w = attr
    n = math.sqrt(x * x + y * y + z * z)
    if abs(n - 1.0) < 0.02 and abs(w) < 0.1:
        lx, ly, lz = 0.4, 0.8, 0.45     # port's stand-in light direction
        ll = math.sqrt(lx * lx + ly * ly + lz * lz)
        d = max((x * lx + y * ly + z * lz) / ll, 0.0)
        s = 0.30 + 0.70 * d
        return (s, s, s)
    return (min(max(x, 0.0), 1.0), min(max(y, 0.0), 1.0),
            min(max(z, 0.0), 1.0))


# ---------------------------------------------------------------------------
# Mesh build (strips -> welded indexed triangles, per-vertex texture id)

def load_level_mesh(level_path: Path):
    """Decode the render mesh into one EMDL section.

    Returns (sections, tex_table): sections = [(pos, col, tris, bones,
    uvs, texids)] matching export_native.write_emdl's expectations (the
    color list rides in the normal slot)."""
    d = level_path.read_bytes()

    raw_pos, raw_col, raw_bone, raw_uv, raw_tex = [], [], [], [], []
    tris = []
    weld = {}
    tex_table: list[dict] = []
    tex_index: dict[int, int] = {}
    NO_TEX = 0xFFFFFFFF
    n_strips = 0

    def tex_of(q0: int) -> int:
        key = q0 & en.TEX0_KEY_MASK
        ti = tex_index.get(key)
        if ti is None:
            f = en.tex0_fields(key)
            # PSMT4 / PSMT8 indexed plus PSMCT32 (the backdrop texture)
            if f["psm"] not in (0x00, 0x13, 0x14) or not (4 <= f["tw"] <= 10) \
                    or not (4 <= f["th"] <= 10):
                ti = NO_TEX
            else:
                ti = len(tex_table)
                f["key"] = key
                tex_table.append(f)
            tex_index[key] = ti
        return ti

    def vid_of(p, c, uv, t):
        key = (round(p[0], 4), round(p[1], 4), round(p[2], 4),
               round(c[0], 3), round(c[1], 3), round(c[2], 3),
               round(uv[0], 5), round(uv[1], 5), t)
        i = weld.get(key)
        if i is None:
            i = len(raw_pos)
            weld[key] = i
            raw_pos.append(p)
            raw_col.append(c)
            raw_bone.append(0)
            raw_uv.append(uv)
            raw_tex.append(t)
        return i

    run = []          # welded ids of the running strip
    prev_adc = None
    prev_key = None
    for rec in walk_records(d):
        if rec is None:           # block header — the strip cannot continue
            run = []
            prev_adc = prev_key = None
            continue
        pos, wbits, q, uv, attr = rec
        adc = (wbits >> 15) & 1     # set = priming/padding, clear = kick
        key56 = q & en.TEX0_KEY_MASK
        # an ADC vertex after a kick vertex primes a NEW strip; a TEX0
        # change always does
        if (adc and prev_adc == 0) or key56 != prev_key:
            run = []
            n_strips += 1
        prev_adc, prev_key = adc, key56
        t = tex_of(q)
        vi = vid_of(pos, attr_to_color(attr), uv, t)
        run.append(vi)
        k = len(run)
        if k >= 3 and not adc:
            a, b, c = run[k - 3], run[k - 2], run[k - 1]
            if a != b and b != c and a != c:
                if (k & 1) == 0:
                    tris.extend((a, b, c))
                else:
                    tris.extend((b, a, c))

    sections = [(raw_pos, raw_col, tris, raw_bone, raw_uv, raw_tex)]
    return sections, tex_table, n_strips


# ---------------------------------------------------------------------------
# Texture resolution from the GS dump's VRAM snapshot

def read_psmct32_rgba(lm: bytes, tbp0: int, tbw: int, w: int, h: int) -> bytes:
    """Direct PSMCT32 read (the level's 32-bit backdrop texture)."""
    est = _load("_est_lvl", "extract_subtextures.py")
    bw = max(tbw, 1) * 64
    out = bytearray(w * h * 4)
    base = tbp0 * 256
    for y in range(h):
        for x in range(w):
            a = base + est.psmct32_word(x, y, bw) * 4
            o = (y * w + x) * 4
            if a + 4 <= len(lm):
                r, g, b, al = lm[a], lm[a + 1], lm[a + 2], lm[a + 3]
                out[o:o + 4] = bytes((r, g, b, min(255, al * 2)))
            else:
                out[o:o + 4] = b"\x00\x00\x00\xff"
    return bytes(out)


def build_texture_blob(gsdump: Path | None, tex_table: list[dict]):
    """Like export_native.build_texture_blob but adds PSMCT32 support."""
    entries, blob = [], bytearray()
    lm = None
    if gsdump is not None:
        pg = _load("_parse_gsdump_lvl", "parse_gsdump.py")
        state_data, _r, _p, _s, _c = pg.parse(gsdump, quiet=True)
        lm = pg.dump_vram(state_data)
        cp = _load("_clut_pair_lvl", "clut_pair.py")
        from clut import apply_clut
    for f in tex_table:
        w, h = 1 << f["tw"], 1 << f["th"]
        if lm is None:
            entries.append({"w": 1, "h": 1, "off": len(blob)})
            blob += b"\x80\x80\x80\xff"
            continue
        if f["psm"] == 0x00:
            rgba = read_psmct32_rgba(lm, f["tbp0"], f["tbw"], w, h)
        elif f["psm"] == 0x14:
            idx = cp.read_psmt4(lm, f["tbp0"], f["tbw"], w, h)
            pal = cp.read_clut16_rgba(lm, f["cbp"]) + bytes(1024 - 64)
            rgba = apply_clut(idx, pal)
        else:
            idx = cp.read_psmt8(lm, f["tbp0"], f["tbw"], w, h)
            pal = cp.read_clut_rgba(lm, f["cbp"])
            rgba = apply_clut(idx, pal)
        entries.append({"w": w, "h": h, "off": len(blob)})
        blob += rgba
    return entries, bytes(blob)


# ---------------------------------------------------------------------------

def main(argv):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--level", default="extract/chunk06.n1/f03_id43.bin",
                    help="level RENDER-MESH file (64-byte [pos][tex0][st]"
                    "[color] records; the office scene's is "
                    "extract/chunk06.n1/f03_id43.bin)")
    ap.add_argument("--gsdump", help="PCSX2 1-frame GS dump (.gs) of a scene "
                    "inside this level: source of colored texels. Without "
                    "it textures are grey 1x1.")
    ap.add_argument("--out", required=True)
    args = ap.parse_args(argv)

    sections, tex_table, n_strips = load_level_mesh(Path(args.level))
    pos = sections[0][0]
    ntris = len(sections[0][2]) // 3
    xs = [p[0] for p in pos]
    ys = [p[1] for p in pos]
    zs = [p[2] for p in pos]
    print(f"level mesh: {len(pos)} verts, {ntris} tris, {n_strips} strips, "
          f"{len(tex_table)} textures")
    print(f"  bbox X[{min(xs):.1f},{max(xs):.1f}] "
          f"Y[{min(ys):.1f},{max(ys):.1f}] Z[{min(zs):.1f},{max(zs):.1f}]")

    tex_entries, tex_blob = build_texture_blob(
        Path(args.gsdump) if args.gsdump else None, tex_table)

    frames = [[en.mat_identity()]]   # 1 bone, 1 identity frame
    parents = [-1]
    en.write_emdl(Path(args.out), sections, [], parents, frames, 30.0,
                  tex_entries, tex_blob, flags=1)   # bit 0: vertex-color
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
