#!/usr/bin/env python3
"""export_props.py — chunk27 equipment-library models for the native port:
attach the player's held weapons to the player skeleton, and export placed
world pickups as a static scene EMDL.

THE LIBRARY (decoded 2026-06-09 s7). `extract/chunk27/f01_id37.bin` is a
directory of 126 standalone mesh blobs:

  +0x00  u32 count (0x7E = 126)
  +0x04  u32 offset[count]      byte offsets into the file (0x80-aligned,
                                terminated by 0xFFFFFFFF)
  each offset -> a "raw mesh blob" in exactly the chunk28-character format
  (header: n_blocks, total_qwc, n_nodes, size; first STCYCL+UNPACK at
  +0x48; 130-qw VIF blocks of 32 records [TEX0][ST][normal][pos+W]) —
  `export_native._walk_blob_blocks` walks it unchanged.

ATTACHMENT (decoded 2026-06-09 s8; CORRECTED 2026-06-10 s9 against three
fresh live frames at different player positions). Each held item is one
object-kernel draw unit (CALL 0x23C750) whose dmem matrix upload is
BYTE-IDENTICAL to one of the 21 matrices of the player's own skin draw
unit — i.e. the engine parents equipment to a skeleton node with an
IDENTITY local offset. Verified RAW byte-exact in s9:

  rifle = models 47, 48, 49, 50, 56, 64  -> node  4 (right hand)
  knife = model 106                      -> node 14 (hip HOLSTER node)

s8's "model 48 drawn a second time -> node 14" was WRONG: node 14 carries
the HOLSTERED KNIFE every frame (one knife draw per frame, matrix ==
bone 14 byte-exact in every s9 capture; note the skin palette must be
read from the PRIMARY skin unit — later 21-set player-blob units in the
frame are shadow/secondary passes with different matrices). Model 48's
occasional second REF is a second PASS of the same draw unit (same
matrix, re-kick) — never a second placement. Binding 48 to node 14 is
what produced the phantom barrel by the right leg in the port.

So --attach merges the rifle models onto node 4 and the knife onto node
14 (positions stay model-space = bone-local) and they animate with the
skeleton automatically.

The default (no --attach) export bakes PLACED world models (the pickups)
into a static EMDL v2 with the placements applied — currently model 106
at its live floor pose (115.0, 1.5, -269.3), which is a REAL separate
pickup instance (type-0xB entry in the live placement table @0x828330),
independent of the holstered knife.

Disc-derived output: write only into git-ignored locations.

Usage (macOS arm64, decomp repo root):
  # player with attached weapons:
  .venv/bin/python tools/export_props.py --attach \
      --mesh extract/chunk28/f00_id3b.bin \
      --anim extract/chunk28/f01_id3c.bin --clip 346 \
      --gsdump extract/gsdump/frame1.gs \
      --out ../extermination-port/assets/player.emdl
  # world pickups:
  .venv/bin/python tools/export_props.py \
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
# Live capture (2026-06-09 s8, corrected 2026-06-10 s9): held equipment ->
# player node (identity local offset, M_equip == M_node byte-exact in the
# frame's DMA chain; rifle -> node 4, knife -> node 14 hip holster), plus
# world-placed pickups (W = K_L^-1 * M, 3x4 [R | t] rows).

ATTACHMENTS = [(47, 4), (48, 4), (49, 4), (50, 4), (56, 4), (64, 4),
               (106, 14)]

LIVE_PLACEMENTS: dict[int, list] = {
    # model 106 (knife): floor pickup near the office's far wall
    106: [[[0.988756, -0.0, 0.149536, 115.000011],
           [-0.0, 1.0, 0.0, 1.50013],
           [-0.149536, 0.0, 0.988756, -269.30001]]],
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


def attr_color(attr, m) -> tuple:
    """attr row -> baked color. Unit normals (|xyz|~1, w~0) are rotated
    into world space and lit with the port's stand-in light; anything
    else is already a color."""
    x, y, z, w = attr
    n = math.sqrt(x * x + y * y + z * z)
    if abs(n - 1.0) < 0.05 and abs(w) < 0.1:
        if m is not None:
            x, y, z = lvl.mat_rotate(m, (x, y, z))
        lx, ly, lz = 0.4, 0.8, 0.45
        ll = math.sqrt(lx * lx + ly * ly + lz * lz)
        d = max((x * lx + y * ly + z * lz) / ll, 0.0)
        s = 0.30 + 0.70 * d
        return (s, s, s)
    return (min(max(x, 0.0), 1.0), min(max(y, 0.0), 1.0),
            min(max(z, 0.0), 1.0))


def model_tris(d: bytes, off: int):
    """GS tristrip assembly of one library model: yield
    (tex0_qword, [3 x (pos, attr, uv)], parity) triangles. Every record
    enters the strip; bit-15-clear records with two predecessors kick;
    triangle texture = the kick vertex's TEX0 (it may change mid-strip)."""
    for recs in model_records(d, off):
        run = []
        for q, uv, attr, pos, wbits in recs:
            run.append((pos, attr, uv, q))
            if len(run) > 3:
                run.pop(0)
            if (wbits & 0x8000) == 0 and len(run) == 3:
                if run[0][0] != run[1][0] and run[1][0] != run[2][0] \
                        and run[0][0] != run[2][0]:
                    yield q, list(run), (wbits >> 14) & 1


# ---------------------------------------------------------------------------
# Shared texture-id helper

def make_tex_of(tex_table, tex_index):
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
    return tex_of


# ---------------------------------------------------------------------------
# Mode 1: static world pickups (placements baked)

def build_placed_mesh(d: bytes, placements: dict[int, list]):
    offs = read_directory(d)
    raw_pos, raw_col, raw_bone, raw_uv, raw_tex = [], [], [], [], []
    tris = []
    weld = {}
    tex_table: list[dict] = []
    tex_of = make_tex_of(tex_table, {})

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

    n_inst = 0
    for mi, mats in sorted(placements.items()):
        if mi >= len(offs):
            print(f"  ! model {mi} out of range ({len(offs)} models), skipped")
            continue
        for m in mats:
            n_inst += 1
            ntri = 0
            for q, corners, parity in model_tris(d, offs[mi]):
                t = tex_of(q)
                ids = [vid_of(lvl.mat_apply(m, p), attr_color(a, m), uv, t)
                       for p, a, uv, _q in corners]
                a, b, c = ids
                if parity:
                    tris.extend((c, b, a))
                else:
                    tris.extend((c, a, b))
                ntri += 1
            print(f"  model {mi:3d}: {ntri} tris at "
                  f"({m[0][3]:.1f}, {m[1][3]:.1f}, {m[2][3]:.1f})")

    sections = [(raw_pos, raw_col, tris, raw_bone, raw_uv, raw_tex)]
    return sections, tex_table, n_inst


# ---------------------------------------------------------------------------
# Mode 2: --attach — player EMDL with equipment merged onto skeleton nodes

def build_attached_player(args):
    mesh_path = Path(args.mesh)
    sections, max_slot, tex_table = en.load_mesh_sections(mesh_path,
                                                          args.segment)
    pos, nrm, tris, bones, uvs, texs = sections[0]
    print(f"player mesh: {len(pos)} verts, {len(tris)//3} tris, "
          f"max node slot {max_slot}, {len(tex_table)} textures")

    tex_index = {t["key"]: i for i, t in enumerate(tex_table)}
    tex_of = make_tex_of(tex_table, tex_index)
    weld = {}

    def vid_of(p, n, b, uv, t):
        key = (round(p[0], 4), round(p[1], 4), round(p[2], 4),
               round(n[0], 3), round(n[1], 3), round(n[2], 3), b,
               round(uv[0], 5), round(uv[1], 5), t)
        i = weld.get(key)
        if i is None:
            i = len(pos)
            weld[key] = i
            pos.append(p)
            nrm.append(n)
            bones.append(b)
            uvs.append(uv)
            texs.append(t)
        return i

    lib = Path(args.library).read_bytes()
    offs = read_directory(lib)
    base_tris = len(tris) // 3
    for mi, node in ATTACHMENTS:
        ntri = 0
        for q, corners, parity in model_tris(lib, offs[mi]):
            t = tex_of(q)
            # positions/normals stay MODEL-LOCAL: the live frame shows the
            # equipment draw matrix == the node matrix (identity offset),
            # so model space IS the node's bone-local space.
            ids = [vid_of(tuple(p), (a[0], a[1], a[2]), node, uv, t)
                   for p, a, uv, _q in corners]
            a, b, c = ids
            if parity:
                tris.extend((c, b, a))
            else:
                tris.extend((c, a, b))
            ntri += 1
        print(f"  attached model {mi:3d} -> node {node:2d}: {ntri} tris")
    print(f"player+equipment: {len(pos)} verts, {len(tris)//3} tris "
          f"(+{len(tris)//3 - base_tris}), {len(tex_table)} textures")

    max_slot = max([max_slot] + [n for _m, n in ATTACHMENTS])
    return sections, max_slot, tex_table


# ---------------------------------------------------------------------------

def main(argv):
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--library", default="extract/chunk27/f01_id37.bin",
                    help="model library (directory of mesh blobs)")
    ap.add_argument("--gsdump", help="PCSX2 1-frame GS dump (.gs): source of "
                    "colored texels (grey 1x1 without it)")
    ap.add_argument("--placements", help="JSON {model_index: [3x4 row-major "
                    "world matrices]}; default = live-captured pickups")
    ap.add_argument("--attach", action="store_true",
                    help="export the PLAYER EMDL (--mesh/--anim/--clip) with "
                    "the held equipment merged onto its skeleton nodes")
    ap.add_argument("--mesh", default="extract/chunk28/f00_id3b.bin",
                    help="(--attach) player mesh blob")
    ap.add_argument("--anim", default="extract/chunk28/f01_id3c.bin",
                    help="(--attach) id 0x74 animation library")
    ap.add_argument("--clip", type=int, default=346,
                    help="(--attach) clip index to bake (default 346, idle)")
    ap.add_argument("--segment", type=int, default=0)
    ap.add_argument("--out", required=True)
    args = ap.parse_args(argv)

    if args.attach:
        sections, max_slot, tex_table = build_attached_player(args)
        tex_entries, tex_blob = lvl.build_texture_blob(
            Path(args.gsdump) if args.gsdump else None, tex_table)
        parents, frames, fps = en.bake_id74_palettes(Path(args.anim),
                                                     args.clip)
        frames = en.recentre(frames)
        print(f"palette: clip {args.clip} -> {len(frames)} frames, "
              f"{len(frames[0])} nodes @ {fps} fps")
        en.write_emdl(Path(args.out), sections, [], parents, frames, fps,
                      tex_entries, tex_blob)
        return 0

    placements = LIVE_PLACEMENTS
    if args.placements:
        raw = json.loads(Path(args.placements).read_text())
        placements = {int(k): v for k, v in raw.items()}

    d = Path(args.library).read_bytes()
    sections, tex_table, n_inst = build_placed_mesh(d, placements)
    pos = sections[0][0]
    ntris = len(sections[0][2]) // 3
    if not pos:
        raise SystemExit("no geometry produced")
    xs = [p[0] for p in pos]; ys = [p[1] for p in pos]; zs = [p[2] for p in pos]
    print(f"props: {n_inst} placed instances, {len(pos)} verts, {ntris} tris, "
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
