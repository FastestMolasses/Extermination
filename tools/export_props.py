#!/usr/bin/env python3
"""export_props.py — export PLACED models from the chunk27 model LIBRARY
(`extract/chunk27/f01_id37.bin`) as a static, world-baked EMDL v2 asset
for the native port.

THE LIBRARY (decoded 2026-06-09, session 7). f01_id37.bin is a directory
of 126 standalone mesh blobs:

  +0x00  u32 count (0x7E = 126)
  +0x04  u32 offset[count]      byte offsets into the file (0x80-aligned),
                                terminated by 0xFFFFFFFF
  each offset -> a "raw mesh blob" in exactly the chunk28-character
  format (header: n_blocks, total_qwc, n_nodes, size; first STCYCL+UNPACK
  at +0x48; 130-qw VIF blocks of 32 records [TEX0][ST][normal][pos+W]) —
  `export_native._walk_blob_blocks` walks it unchanged.

PLACEMENTS (live-debug session 7). The engine draws library models with
one DMA "draw unit" per placed model in the per-frame packet arena:

  [CNT 9qw : VIF STCYCL+UNPACK V4-32 8qw -> VU1 dmem 0]  4 qw MVP matrix
                                                         + 4 qw light rows
  [REF 9qw @0x815360] [REF 8qw @0x816440] [REF 1qw @0x814220]  shared env
  [CALL @0x0023C750]                       kernel-kick packet (MSCAL)
  [REF n qw @ library_base + offset[i] + 0x40]   the model's block stream
  [CNT 5qw]                                per-object GS constants

The uploaded matrix is MVP = K x W (K = camera/clip transform, W = the
model's world placement; matrix qwords are the columns/axis images).
The level render mesh streams in WORLD space through its own kernel
with matrix K_level, and K's first three columns are byte-identical to
(-K_level.col2, K_level.col1, K_level.col0) in every live sample — the
two kernels share the camera, so W = K_level^-1 x MVP recovers the
ABSOLUTE world placement (validated: bottom row (0,0,0,1), orthonormal
rotation, translations inside the room bbox at the player).

Live office-scene result: the only chunk27 models drawn are the PLAYER'S
EQUIPMENT — models 47/48/49/50/56/64 (the rifle, a six-model composite
sharing one transform), model 106 (carried gear), models 20/21 (glow
billboards, non-rigid scale matrices — skipped here). The office desk
items are baked into the level render mesh; chunk27 is the equipment /
pickup library, not room furniture.

Output: one EMDL v2 (static, identity palette, flags bit 0 = baked
vertex color) with the placements baked into the vertices. Textures
resolve through the same GS-dump VRAM machinery as the level exporter
(all 30 equipment TEX0 keys appear in the office dump frame).

Disc-derived output: write only into git-ignored locations.

Usage (macOS arm64, decomp repo root):
  .venv/bin/python tools/export_props.py \
      --library extract/chunk27/f01_id37.bin \
      --gsdump extract/gsdump/frame1.gs \
      --out ../extermination-port/assets/scene/01_props.emdl
"""
from __future__ import annotations

import argparse
import importlib.util
import json
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


en = _load("_export_native_props", "export_native.py")
lvl = _load("_export_level_props", "export_level.py")

# ---------------------------------------------------------------------------
# Live placements, recovered 2026-06-09 from the running office scene
# (PCSX2 DebugServer; per-model draw-unit MVPs at packet-arena 0x299xxx,
# K_level anchor from the level draw chain at 0x297410). Rows are the
# first three rows of the affine world matrix [R | t]; bottom row 0001.
# Model keys are library directory indices.

LIVE_PLACEMENTS: dict[int, list[list[float]]] = {
    # the rifle: a six-model composite, one shared transform (in the
    # player's hands, idle pose)
    47: [[0.026218, -0.029295, -0.998309, 104.573173],
         [0.613704,  0.788837, -0.006362,  12.934228],
         [0.788679, -0.612222,  0.039197, -183.358210]],
    48: [[0.026218, -0.029295, -0.998309, 104.573173],
         [0.613704,  0.788837, -0.006362,  12.934228],
         [0.788679, -0.612222,  0.039197, -183.358210]],
    49: [[0.026218, -0.029295, -0.998309, 104.573173],
         [0.613704,  0.788837, -0.006362,  12.934228],
         [0.788679, -0.612222,  0.039197, -183.358210]],
    50: [[0.026218, -0.029295, -0.998309, 104.573173],
         [0.613704,  0.788837, -0.006362,  12.934228],
         [0.788679, -0.612222,  0.039197, -183.358210]],
    56: [[0.026218, -0.029295, -0.998309, 104.573173],
         [0.613704,  0.788837, -0.006362,  12.934228],
         [0.788679, -0.612222,  0.039197, -183.358210]],
    64: [[0.026218, -0.029295, -0.998309, 104.573173],
         [0.613704,  0.788837, -0.006362,  12.934228],
         [0.788679, -0.612222,  0.039197, -183.358210]],
    # carried gear (shoulder-height, mirrored axes in the live matrix)
    106: [[-0.987142,  0.040452,  0.146947, 106.109454],
          [-0.032970, -0.996965,  0.059144,   9.182241],
          [ 0.149576,  0.054252,  0.985992, -181.974180]],
}


# ---------------------------------------------------------------------------
# Library access

def read_directory(d: bytes) -> list[int]:
    n = struct.unpack_from("<I", d, 0)[0]
    return list(struct.unpack_from(f"<{n}I", d, 4))


def model_records(d: bytes, off: int):
    """Yield per-block lists of (tex0_qword, uv, attr, pos, wbits) records
    for the library model blob at byte offset `off`."""
    payloads, _n_nodes = en._walk_blob_blocks(d[off:])
    for payload in payloads:
        recs = []
        for r in range(0, len(payload) - 63, 64):
            w = struct.unpack_from("<f", payload, r + 0x3C)[0]
            if abs(abs(w) - 1.0) > 0.25:
                break
            wbits = struct.unpack_from("<I", payload, r + 0x3C)[0]
            q = int.from_bytes(payload[r:r + 8], "little")
            uv = struct.unpack_from("<2f", payload, r + 0x10)
            attr = struct.unpack_from("<4f", payload, r + 0x20)
            pos = struct.unpack_from("<3f", payload, r + 0x30)
            recs.append((q, uv, attr, pos, wbits))
        yield recs


def mat_apply(m, v):
    return (m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3],
            m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3],
            m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3])


def mat_rotate(m, v):
    return (m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2],
            m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2],
            m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2])


def attr_color(attr, m) -> tuple:
    """attr row -> baked color. Unit normals (|xyz|~1, w~0) are rotated
    into world space and lit with the port's stand-in light; anything
    else is already a color."""
    x, y, z, w = attr
    n = math.sqrt(x * x + y * y + z * z)
    if abs(n - 1.0) < 0.05 and abs(w) < 0.1:
        wx, wy, wz = mat_rotate(m, (x, y, z))
        lx, ly, lz = 0.4, 0.8, 0.45
        ll = math.sqrt(lx * lx + ly * ly + lz * lz)
        d = max((wx * lx + wy * ly + wz * lz) / ll, 0.0)
        s = 0.30 + 0.70 * d
        return (s, s, s)
    return (min(max(x, 0.0), 1.0), min(max(y, 0.0), 1.0),
            min(max(z, 0.0), 1.0))


# ---------------------------------------------------------------------------

def build_mesh(d: bytes, placements: dict[int, list]):
    offs = read_directory(d)
    raw_pos, raw_col, raw_bone, raw_uv, raw_tex = [], [], [], [], []
    tris = []
    weld = {}
    tex_table: list[dict] = []
    tex_index: dict[int, int] = {}
    NO_TEX = 0xFFFFFFFF

    def tex_of(q0: int) -> int:
        key = q0 & en.TEX0_KEY_MASK
        ti = tex_index.get(key)
        if ti is None:
            f = en.tex0_fields(key)
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

    n_models = 0
    for mi, m in sorted(placements.items()):
        if mi >= len(offs):
            print(f"  ! model {mi} out of range ({len(offs)} models), skipped")
            continue
        n_models += 1
        nverts = 0
        for recs in model_records(d, offs[mi]):
            run = []
            prev_adc = None
            prev_key = None
            for q, uv, attr, pos, wbits in recs:
                adc = (wbits >> 15) & 1
                key56 = q & en.TEX0_KEY_MASK
                if (adc and prev_adc == 0) or key56 != prev_key:
                    run = []
                prev_adc, prev_key = adc, key56
                t = tex_of(q)
                vi = vid_of(mat_apply(m, pos), attr_color(attr, m), uv, t)
                nverts += 1
                run.append(vi)
                k = len(run)
                if k >= 3 and not adc:
                    a, b, c = run[k - 3], run[k - 2], run[k - 1]
                    if a != b and b != c and a != c:
                        if (k & 1) == 0:
                            tris.extend((a, b, c))
                        else:
                            tris.extend((b, a, c))
        print(f"  model {mi:3d}: {nverts} records")

    sections = [(raw_pos, raw_col, tris, raw_bone, raw_uv, raw_tex)]
    return sections, tex_table, n_models


def main(argv):
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--library", default="extract/chunk27/f01_id37.bin",
                    help="model library (directory of mesh blobs)")
    ap.add_argument("--gsdump", help="PCSX2 1-frame GS dump (.gs): source of "
                    "colored texels (grey 1x1 without it)")
    ap.add_argument("--placements", help="JSON {model_index: 3x4 row-major "
                    "world matrix}; default = live-captured office equipment")
    ap.add_argument("--out", required=True)
    args = ap.parse_args(argv)

    placements = LIVE_PLACEMENTS
    if args.placements:
        raw = json.loads(Path(args.placements).read_text())
        placements = {int(k): v for k, v in raw.items()}

    d = Path(args.library).read_bytes()
    sections, tex_table, n_models = build_mesh(d, placements)
    pos = sections[0][0]
    ntris = len(sections[0][2]) // 3
    if not pos:
        raise SystemExit("no geometry produced")
    xs = [p[0] for p in pos]; ys = [p[1] for p in pos]; zs = [p[2] for p in pos]
    print(f"props: {n_models} models, {len(pos)} verts, {ntris} tris, "
          f"{len(tex_table)} textures")
    print(f"  world bbox X[{min(xs):.1f},{max(xs):.1f}] "
          f"Y[{min(ys):.1f},{max(ys):.1f}] Z[{min(zs):.1f},{max(zs):.1f}]")

    tex_entries, tex_blob = lvl.build_texture_blob(
        Path(args.gsdump) if args.gsdump else None, tex_table)

    frames = [[en.mat_identity()]]
    parents = [-1]
    en.write_emdl(Path(args.out), sections, [], parents, frames, 30.0,
                  tex_entries, tex_blob, flags=1)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
