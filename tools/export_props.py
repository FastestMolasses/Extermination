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

PLAYER AURA / GLOW BILLBOARDS (decoded 2026-06-10 from save state 01 +
the office GS dump). Library models 20/21 are +-5-unit CUBES (6 quads,
axis normals) sharing one 16x16 PSMT8 texture (TEX0 key
0x041695113222E9: a bright border ring, faint ~6%% interior; the cube
faces' UV box 0.281..0.719 samples only the faint interior). The engine
draws them through the LEVEL kernel (CALL 0x237180) as the player's
aura: one shared world matrix = pure scale diag(1.6, 4, 1.6) translated
to the player ROOT's exact X/Z with y = root_y - 9.1, identity rotation
regardless of player yaw (model 21 gets an extra slowly-rotating pass).
GS state of every glow draw (office dump runs 3533+, all four captured
frames): PRIM ABE=1; ALPHA A=0 B=2 C=2 D=1 FIX=0x80 -> Cv = Cs*1.0 + Cd
(pure ADDITIVE); ZBUF ZMSK=1 (no depth write) + TEST ATST=NEVER/AFAIL=
RGB_ONLY (belt & braces: no A/Z update); TEST ZTE=1 ZTST=2 (depth test
ON, GEQUAL) — so the below-floor half of the 40-unit-tall box is depth-
clipped, leaving a soft column over the body. Vertex color comes from
EE-side RGBAQ: green, pulsing per frame (office: (1, 20..215, 1)/0x80;
snow state 01: model 20 (0,0x80,0), model 21's rotated pass red-ish).

The port renders these as camera-facing QUADS (the standard equivalent
of the PS2's additive box trick): GLOW_ATTACHMENTS bakes, per model, one
quad anchored to skeleton node 0 at local offset (0,-9.1,0) with corner
half-extents (8, 20) = cube half-size 5 x engine scale (1.6, 4), the
face UV box, and a fixed mid-pulse green tint pre-multiplied into a
dedicated copy of the texture. The vertex record carries the anchor in
the position slot, the camera-plane corner offset in the normal slot,
and bit 31 of the bone word = BILLBOARD+ADDITIVE (see export_native.
write_emdl and the port's em_model.h/em_gfx_metal.m).

The default (no --attach) export bakes PLACED world models (the pickups)
into a static EMDL v2 with the placements applied — currently model 106
at its live floor pose (115.0, 1.5, -269.3), which is a REAL separate
pickup instance (type-0xB entry in the live placement table @0x828330),
independent of the holstered knife.

DOORS (--doors, 2026-06-10 interactive-objects session). The office
double doors (placement-table class 5, model 3, behavior func_001BC350 —
FINDINGS "FIRST INTERACTIVE OBJECTS") become the port's first interactive
objects, so they must stop being baked statically into 00_level.emdl and
ship as a SEPARATE articulated EMDL instead:

  * door mesh = the level render file's RGN_DOOR blob (0xA05C0..0xA2640),
    exported in DOOR-LOCAL space: slot 0 is the full 9x21 door panel at
    the placement origin (hinge edge at local x=0, panel spans x -9..0),
    slot 1 a small lock/mechanism fixture at local (-7.69, 9.0, -0.25)
    (= L0^-1 * L1 of the live-captured closed slot matrices — per-slot
    extents measured this session). Two bones (slot 0/1), one closed-pose
    palette frame; vertices stay slot-model-space so the frame-0 palette
    reproduces the captured closed pose exactly.
  * the per-area placement TRS + the s17 use-scan trigger radius (12.0,
    func_00184BA0's dist^2 <= 144) go to the SCENE MANIFEST as one line
    per door instance:  door <file> <x> <y> <z> <yaw> <radius>
    (files live in <scene>/doors/ so the port's static scene_load —
    which slurps every .emdl in the scene dir — does not double-draw
    them; the port's em_door.c owns them instead).
  * open/close clip: the engine articulates doors with a keyframe clip on
    the door skeleton (s17, advanced 1.0/frame via func_001C68C0). A
    GLOBAL 3-node object-anim bank exists (chunk27/f02_id39.bin, 5 clips,
    resident in EE RAM across save states of three different areas), but
    none of its rest poses matches the double door's captured closed pose
    (slot-1 local (-7.69, 9.0, -0.25)), and no other decodable id 0x74
    bank in the extraction carries a matching small-rig container. The
    hunt is automated below (find_door_clip) and re-runs on every export;
    until it finds one the EMDL ships the closed pose only and the port
    plays an honestly-flagged placeholder 90-degree hinge swing.
  * 00_level.emdl is REBUILT in the same run from export_level's region
    machinery (imported, not copied) with the RGN_DOOR replays dropped,
    so the static bake no longer contains the doors.

GIBS (--gibs, 2026-06-10 s24). The crawler's burst death REBINDS the actor's
model to library entry 0x22 or 0x29 (FINDINGS "CRAWLER RESOLVED": D_0028A56C
is THIS library; func_001C6120 + func_001CA6E0 swap actor+0x44). Surveying the
library around those ids by TEX0-key sharing identifies the whole burst set —
each husk has small chunk/shard meshes textured with ITS OWN skin:

  0x22  burst husk A (160 v/300 t, 14x14 footprint, 2.8 tall — the opened
        crate base), texture 0x22F9 shared with shards 0x1C/0x1D/0x1E
        (1.5-3 u flat splinters)
  0x29  burst husk B (100 v/252 t, 14x14, 8 tall), texture 0x229B shared
        with chunks 0x26/0x27 (~2 u lumps); 0x28 = the same husk at exactly
        half size (3.5/4.0 extents, same texture)
  0x1F/0x20/0x21  a second shard triplet (same three shapes as 0x1C-0x1E,
        own texture 0x22FD — the other husk variant's splinters)
  (0x2C-0x2E nearby are NOT gibs: they sample the additive glow-billboard
  texture 0x...3222E9 — effect shells; 0x23/0x24 pair on a 64x64 skin and
  0x25/0x2A/0x2B pair elsewhere — unrelated pickups/props.)

--gibs exports each entry as its own STATIC 1-node EMDL v2 (model-local
space, identity palette frame, normals -> baked colors with the standard
stand-in light, texels resolved from the office GS dump exactly like the
props above) into <outdir>/gib_<id>.emdl. The port's em_enemy.c launches the
small chunks on the crawler's burst death and falls back to its old sink
placeholder when the files are absent.

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
  # office doors as articulated EMDLs + door-less level rebake:
  .venv/bin/python tools/export_props.py --doors \
      --level extract/chunk06.n1/f03_id43.bin \
      --gsdump extract/gsdump/frame1.gs \
      --outdir ../extermination-port/assets/scene
  # crawler burst-death gib set (one EMDL per library entry):
  .venv/bin/python tools/export_props.py --gibs \
      --gsdump extract/gsdump/frame1.gs \
      --gibs-outdir ../extermination-port/assets/gibs
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

# Player aura (see "PLAYER AURA / GLOW BILLBOARDS" above): camera-facing
# additive quads anchored to a skeleton node. (model, node, local anchor,
# corner half-extents (x, y) = cube half-size 5 * live scale (1.6, 4),
# RGB tint /128 from live office RGBAQ samples — the engine pulses the
# green channel; we bake one mid-pulse value per layer.)
VERT_BILLBOARD = 0x80000000          # bone-word bit 31 (write_emdl flag)
GLOW_ATTACHMENTS = [
    (20, 0, (0.0, -9.1, 0.0), (8.0, 20.0), (0, 110, 0)),
    (21, 0, (0.0, -9.1, 0.0), (8.0, 20.0), (1, 58, 1)),
]

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

    # Player aura: one camera-facing additive quad per glow model, anchored
    # to a skeleton node (positions = anchor, normals = corner offsets,
    # bone word bit 31 = billboard+additive; tint pre-multiplied into a
    # DEDICATED texture entry so each layer keeps its live color).
    for mi, node, anchor, half, tint in GLOW_ATTACHMENTS:
        recs = [r for blk in model_records(lib, offs[mi]) for r in blk]
        key = recs[0][0] & en.TEX0_KEY_MASK
        us = [r[1][0] for r in recs]
        vs = [r[1][1] for r in recs]
        u0, u1, v0, v1 = min(us), max(us), min(vs), max(vs)
        tf = en.tex0_fields(key)
        tf["key"] = key
        tf["tint"] = tint            # applied by finish_textures
        ti = len(tex_table)
        tex_table.append(tf)
        base = len(pos)
        hx, hy = half
        for cx, cy, uv in ((-hx, -hy, (u0, v1)), (hx, -hy, (u1, v1)),
                           (-hx, hy, (u0, v0)), (hx, hy, (u1, v0))):
            pos.append(tuple(anchor))
            nrm.append((cx, cy, 0.0))
            bones.append(node | VERT_BILLBOARD)
            uvs.append(uv)
            texs.append(ti)
        tris.extend((base, base + 1, base + 2, base + 2, base + 1, base + 3))
        print(f"  glow model {mi:3d} -> node {node:2d}: billboard quad "
              f"{2*hx:.0f}x{2*hy:.0f} @ {anchor}, tint {tint}")

    max_slot = max([max_slot] + [n for _m, n in ATTACHMENTS]
                   + [n for _m, n, _a, _h, _t in GLOW_ATTACHMENTS])
    return sections, max_slot, tex_table


def finish_textures(args, tex_table):
    """build_texture_blob + the glow tints: entries whose tex_table dict
    carries `tint` (R,G,B, /128 fixed point — the live RGBAQ convention)
    get their texels multiplied in place. Used by both --attach drivers
    (here and export_native --attach)."""
    tex_entries, tex_blob = lvl.build_texture_blob(
        Path(args.gsdump) if args.gsdump else None, tex_table)
    blob = bytearray(tex_blob)
    for i, tf in enumerate(tex_table):
        tint = tf.get("tint")
        if not tint or i >= len(tex_entries):
            continue
        e = tex_entries[i]
        for p in range(e["off"], e["off"] + e["w"] * e["h"] * 4, 4):
            blob[p + 0] = min(255, blob[p + 0] * tint[0] // 128)
            blob[p + 1] = min(255, blob[p + 1] * tint[1] // 128)
            blob[p + 2] = min(255, blob[p + 2] * tint[2] // 128)
    return tex_entries, bytes(blob)


# ---------------------------------------------------------------------------
# Mode 3: --doors — placement-table doors as separate articulated EMDLs
# (see "DOORS" in the module docstring).

DOOR_TRIGGER_RADIUS = 12.0   # func_00184BA0: use-scan distance^2 <= 144

# Anim banks the clip hunt can decode today. chunk27/f02_id39 is the
# globally-resident 3-node object bank (verified at EE 0xd1a750 in save
# states of areas 0x11/0x06/0x02-family — same bytes in all three).
DOOR_CLIP_BANKS = [
    "extract/chunk27/f02_id39.bin",
]


def door_local_slots():
    """Door-local slot matrices [L0^-1 * Lk] of the captured closed pose
    (live slot 0 == the placement base for every captured assembly —
    export_level.anchor_slots' invariant)."""
    l0inv = lvl.mat34_inv_rigid(lvl.BLOB_A05C0[0])
    return [lvl.mat34_mul(l0inv, lk) for lk in lvl.BLOB_A05C0]


def find_door_clip(locals34, tol=1.0):
    """Hunt the decodable id 0x74 banks for the double door's open/close
    clip: a small-rig container whose FRAME-0 node translations include
    the door's closed slot-1 local offset. Returns (path, clip_index,
    parents, frames, fps) or None — honestly None today (documented in
    the module docstring); re-runs each export so a future extraction
    win is picked up automatically."""
    want = [row[3] for row in locals34[1]]      # slot-1 closed local t
    for bank in DOOR_CLIP_BANKS:
        p = Path(bank)
        if not p.exists():
            continue
        em = en._load("_extract_models_doors", "extract_models.py")
        d = p.read_bytes()
        try:
            hdrs = em.find_id74_headers(d)
        except Exception:
            continue
        for ci in range(len(hdrs)):
            try:
                parents, frames, fps = en.bake_id74_palettes(p, ci)
            except SystemExit:
                continue
            if not (2 <= len(parents) <= 4):
                continue
            f0 = frames[0]
            for node in f0:
                t = node[3]
                if all(abs(t[k] - want[k]) <= tol for k in range(3)):
                    return p, ci, parents, frames, fps
    return None


def build_door_mesh(level: bytes, locals34):
    """RGN_DOOR records -> one door-local EMDL section: per-record slot
    bits become the vertex bone, positions stay slot-model-space (the
    closed-pose palette frame supplies the slot offsets), colors follow
    export_level's normal-or-baked-color rule with the slot's local
    rotation."""
    raw_pos, raw_col, raw_bone, raw_uv, raw_tex = [], [], [], [], []
    tris = []
    weld = {}
    tex_table: list[dict] = []
    tex_of = make_tex_of(tex_table, {})

    def vid_of(p, c, b, uv, t):
        key = (round(p[0], 4), round(p[1], 4), round(p[2], 4),
               round(c[0], 3), round(c[1], 3), round(c[2], 3), b,
               round(uv[0], 5), round(uv[1], 5), t)
        i = weld.get(key)
        if i is None:
            i = len(raw_pos)
            weld[key] = i
            raw_pos.append(p)
            raw_col.append(c)
            raw_bone.append(b)
            raw_uv.append(uv)
            raw_tex.append(t)
        return i

    run = []
    for rec in lvl.walk_records(level, *lvl.RGN_DOOR):
        if rec is None:
            run = []
            continue
        _off, pos, wbits, q, uv, attr = rec
        slot = (wbits & 0x3FF) >> 3
        m = locals34[slot] if slot < len(locals34) else lvl.IDENT34
        col = lvl.attr_to_color(attr, m)     # rotate normals slot-locally
        run.append((tuple(pos), col, slot, uv, q))
        if len(run) > 3:
            run.pop(0)
        if (wbits & 0x8000) == 0 and len(run) == 3:
            t = tex_of(q)
            (pa, ca, ba, ua, _qa), (pb, cb, bb, ub, _qb), \
                (pc, cc, bc, uc, _qc) = run
            if pa != pb and pb != pc and pa != pc:
                a = vid_of(pa, ca, ba, ua, t)
                b = vid_of(pb, cb, bb, ub, t)
                c = vid_of(pc, cc, bc, uc, t)
                if wbits & 0x4000:
                    tris.extend((c, b, a))
                else:
                    tris.extend((c, a, b))

    return [(raw_pos, raw_col, tris, raw_bone, raw_uv, raw_tex)], tex_table


def mat34_to_cols(m):
    """3x4 [R|t] rows -> the EMDL palette's column-major mat4."""
    return ((m[0][0], m[1][0], m[2][0], 0.0),
            (m[0][1], m[1][1], m[2][1], 0.0),
            (m[0][2], m[1][2], m[2][2], 0.0),
            (m[0][3], m[1][3], m[2][3], 1.0))


def rebuild_level_without_doors(args, level_path: Path, out: Path):
    """00_level.emdl minus the RGN_DOOR replays — export_level's own
    region machinery (table-driven placements + the id44 sibling static
    section), imported, with the door regions filtered out."""
    d = level_path.read_bytes()
    if not (len(d) == lvl.CHUNK06N1_SIZE
            and level_path.name == "f03_id43.bin"):
        raise SystemExit("--doors knows only the office level render file "
                         "(chunk06.n1/f03_id43.bin) today")
    regions = lvl.table_driven_regions(level_path)
    if regions is None:
        print("note: OVERLAY/AREA02.BIN not found — using embedded "
              "live-captured placements")
        regions = lvl.CHUNK06N1_REGIONS
    n_all = len(regions)
    regions = [r for r in regions if (r[0], r[1]) != lvl.RGN_DOOR]
    print(f"level rebake: {n_all - len(regions)} door region replays "
          f"dropped ({len(regions)} regions kept)")

    b = lvl.MeshBuilder()
    sib_name, sib_lo, sib_hi = lvl.CHUNK06N1_SIBLING
    sib_path = level_path.parent / sib_name
    if sib_path.exists():
        sd = sib_path.read_bytes()
        b.add_stream((r if r is None else r[1:] for r in
                      lvl.walk_records(sd, sib_lo, sib_hi)), None)
    else:
        print(f"warning: {sib_path} missing — the western half of the "
              "area will be absent")
    for lo, hi, mode, mats in regions:
        if mode == "world":
            b.add_stream((r if r is None else r[1:] for r in
                          lvl.walk_records(d, lo, hi)), None)
        elif mode == "slots":
            def placement(wbits, mats=mats):
                s = (wbits & 0x3FF) >> 3
                return mats[s] if s < len(mats) else lvl.IDENT34
            b.add_stream((r if r is None else r[1:] for r in
                          lvl.walk_records(d, lo, hi)), placement)
        elif mode == "instances":
            for m in mats:
                b.add_stream((r if r is None else r[1:] for r in
                              lvl.walk_records(d, lo, hi)),
                             lambda wbits, m=m: m)
    sections = [(b.pos, b.col, b.tris, b.bone, b.uv, b.tex)]
    print(f"level mesh (door-less): {len(b.pos)} verts, "
          f"{len(b.tris) // 3} tris, {len(b.tex_table)} textures")
    tex_entries, tex_blob = lvl.build_texture_blob(
        Path(args.gsdump) if args.gsdump else None, b.tex_table,
        Path(args.p2s) if args.p2s else None)
    en.write_emdl(out, sections, [], [-1], [[en.mat_identity()]], 30.0,
                  tex_entries, tex_blob, flags=1)


def update_doors_manifest(scene_dir: Path, lines: list[str]) -> None:
    """Replace the manifest's doors section (all `door ` lines) with
    `lines`; every other key/comment is preserved (same convention as
    export_level.update_manifest, which replaces single-value keys)."""
    mf = scene_dir / "scene.txt"
    kept = [ln for ln in (mf.read_text().splitlines() if mf.exists() else [])
            if not ln.startswith("door ")]
    mf.write_text("\n".join(kept + lines) + "\n")
    for ln in lines:
        print(f"manifest: {mf}: {ln}")


def export_doors(args):
    level_path = Path(args.level)
    ov_path = Path(args.overlay)
    scene_dir = Path(args.outdir)
    if not ov_path.exists():
        raise SystemExit(f"{ov_path}: the doors export is placement-table "
                         "driven; extract OVERLAY/AREA02.BIN first")
    pl = _load("_placements_doors", "placements.py")
    entries = pl.parse_table(ov_path.read_bytes(), lvl.OFFICE_TABLE_VADDR)
    doors = [e for e in entries
             if (e.spawn_class & 0x1F) == 5 and e.model == 3]
    if not doors:
        raise SystemExit("no class-5 model-3 doors in the placement table")

    locals34 = door_local_slots()
    level = level_path.read_bytes()
    sections, tex_table = build_door_mesh(level, locals34)
    pos = sections[0][0]
    ntris = len(sections[0][2]) // 3
    print(f"door mesh (model 3, door-local): {len(pos)} verts, {ntris} "
          f"tris, {len(tex_table)} textures, "
          f"{1 + max(sections[0][3])} slots")

    # Open/close clip hunt (None today — port plays its flagged placeholder).
    clip = find_door_clip(locals34)
    if clip is None:
        print("door clip: NOT FOUND in the decodable anim banks — EMDL "
              "carries the closed pose only; the port's em_door.c plays "
              "its placeholder hinge swing (flagged there)")
        frames = [[mat34_to_cols(m) for m in locals34]]
        parents = [-1] * len(locals34)
        fps, clips = 60.0, None
    else:
        bank, ci, parents, frames, fps = clip
        print(f"door clip: {bank} container {ci} — {len(frames)} frames, "
              f"{len(parents)} nodes")
        clips = [{"id": ci, "first": 0, "count": len(frames), "fps": fps}]

    tex_entries, tex_blob = lvl.build_texture_blob(
        Path(args.gsdump) if args.gsdump else None, tex_table,
        Path(args.p2s) if args.p2s else None)

    door_file = "doors/door_m03.emdl"
    out = scene_dir / door_file
    en.write_emdl(out, sections, [], parents, frames, fps,
                  tex_entries, tex_blob, flags=1, clips=clips)

    lines = []
    for e in doors:
        # s17 door contract: per-instance placement TRS + the use-scan
        # radius. (flags2/link semantics — door id, area-change bit,
        # 1.5x/2.0x anim scale — land with the destination tables.)
        lines.append(f"door {door_file} {e.pos[0]:g} {e.pos[1]:g} "
                     f"{e.pos[2]:g} {e.rot[1]:g} {DOOR_TRIGGER_RADIUS:g}")
        print(f"  door uid {e.uid:#06x} fl={e.flags2:#04x} "
              f"link={e.link:#06x} at ({e.pos[0]:.1f}, {e.pos[1]:.1f}, "
              f"{e.pos[2]:.1f}) yaw {e.rot[1]:.3f}")
    update_doors_manifest(scene_dir, lines)

    rebuild_level_without_doors(args, level_path,
                                scene_dir / "00_level.emdl")
    return 0


# ---------------------------------------------------------------------------
# Mode 4: --gibs — the crawler burst-death model set, one static EMDL each
# (see "GIBS" in the module docstring for the survey that picked these).

GIB_ENTRIES = [
    # (lib id, role) — kept in the FINDINGS "GIB SET" table.
    (0x22, "burst husk A (crate base, 14x14x2.8)"),
    (0x29, "burst husk B (14x14x8)"),
    (0x28, "husk B at half size (7x7x4)"),
    (0x1C, "shard A1 (husk-A skin)"),
    (0x1D, "shard A2 (husk-A skin)"),
    (0x1E, "shard A3 (husk-A skin)"),
    (0x1F, "shard B1 (variant skin)"),
    (0x20, "shard B2 (variant skin)"),
    (0x21, "shard B3 (variant skin)"),
    (0x26, "chunk 1 (husk-B skin)"),
    (0x27, "chunk 2 (husk-B skin)"),
]


def export_gibs(args):
    d = Path(args.library).read_bytes()
    offs = read_directory(d)
    outdir = Path(args.gibs_outdir)
    outdir.mkdir(parents=True, exist_ok=True)
    for mi, role in GIB_ENTRIES:
        # model-local export: identity placement, baked stand-in lighting
        sections, tex_table, _n = build_placed_mesh(d, {mi: [lvl.IDENT34]})
        pos = sections[0][0]
        if not pos:
            print(f"  ! gib 0x{mi:02x}: no geometry, skipped")
            continue
        tex_entries, tex_blob = lvl.build_texture_blob(
            Path(args.gsdump) if args.gsdump else None, tex_table)
        out = outdir / f"gib_{mi:02x}.emdl"
        en.write_emdl(out, sections, [], [-1], [[en.mat_identity()]], 30.0,
                      tex_entries, tex_blob, flags=1)
        ys = [p[1] for p in pos]
        print(f"gib 0x{mi:02x} -> {out.name}: {len(pos)} verts, "
              f"{len(sections[0][2]) // 3} tris, Y[{min(ys):.1f},"
              f"{max(ys):.1f}] — {role}")
    return 0


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
    ap.add_argument("--gibs", action="store_true",
                    help="export the crawler burst-death gib set (library "
                    "entries in GIB_ENTRIES) as one static 1-node EMDL each "
                    "into --gibs-outdir")
    ap.add_argument("--gibs-outdir", default="../extermination-port/assets/gibs",
                    help="(--gibs) output directory (git-ignored, "
                    "disc-derived)")
    ap.add_argument("--doors", action="store_true",
                    help="export the placement-table doors as separate "
                    "articulated EMDLs into <outdir>/doors/, write the "
                    "manifest doors section, and rebake 00_level.emdl "
                    "without the static door geometry")
    ap.add_argument("--level", default="extract/chunk06.n1/f03_id43.bin",
                    help="(--doors) level render-mesh file")
    ap.add_argument("--overlay", default="extract/OVERLAY/AREA02.BIN",
                    help="(--doors) area overlay holding the placement table")
    ap.add_argument("--p2s", help="(--doors) PCSX2 save state as the VRAM "
                    "texel source (--gsdump wins if both)")
    ap.add_argument("--outdir", default="../extermination-port/assets/scene",
                    help="(--doors) scene directory (manifest + EMDLs)")
    ap.add_argument("--mesh", default="extract/chunk28/f00_id3b.bin",
                    help="(--attach) player mesh blob")
    ap.add_argument("--anim", default="extract/chunk28/f01_id3c.bin",
                    help="(--attach) id 0x74 animation library")
    ap.add_argument("--clip", type=int, default=346,
                    help="(--attach) clip index to bake (default 346, idle)")
    ap.add_argument("--segment", type=int, default=0)
    ap.add_argument("--out", help="output EMDL (required except --doors, "
                    "which writes into --outdir)")
    args = ap.parse_args(argv)

    if args.gibs:
        return export_gibs(args)
    if args.doors:
        return export_doors(args)
    if not args.out:
        ap.error("--out is required (except with --doors)")

    if args.attach:
        sections, max_slot, tex_table = build_attached_player(args)
        tex_entries, tex_blob = finish_textures(args, tex_table)
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
