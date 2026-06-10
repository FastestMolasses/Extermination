#!/usr/bin/env python3
"""export_level.py — export a LEVEL RENDER MESH as a static EMDL v2 (.emdl)
asset for the native port (extermination-port).

THE LEVEL RENDER MESH (decoded 2026-06-09 s6; record phase + region map
CORRECTED 2026-06-09 s8 against the live resident stream). Each level chunk
carries a VU1-ready render-mesh file of 64-byte records framed in VIF blocks
(`STCYCL(4,4)` + `UNPACK V4-32 0x80qw`, 32 records per block, padded by
duplicating the last record). The record's four 16-byte rows are in the
CHARACTER/OBJECT order — the file just begins mid-record (a stray pos row at
offset 0), which previously made the walker pair every position with the
NEXT record's TEX0/UV/color (off-by-one-row bug — wrong textures, broken
strips):

    +0x00  the draw's full TEX0 register value (8 bytes, CBP included,
           verbatim) + 8 zero bytes
    +0x10  vec4 (u, v, 1.0, 0.0) — normalized per-texture ST, REPEAT
    +0x20  vec4 baked vertex color (r, g, b, 1.0) OR unit normal
           (|xyz|~=1, w~=0)
    +0x30  vec4 position (x, y, z, w); |w| ~= 1.0.  W as an int: bits
           0..9 = VU1 dmem matrix-slot address (slot = bits>>3), bit 15 =
           the GS PACKED XYZ2 ADC bit verbatim (the W word lands in output
           word 3, so bit 15 == ADC bit 111: SET = prime/no kick, CLEAR =
           the vertex kicks a triangle), bit 14 (mirrored in the float
           sign) = strip parity (winding), bit 13 = per-vertex flag of
           unconfirmed meaning (not topology).

Triangle assembly is plain GS tristrip semantics per VIF block: every
record enters the running strip; a CLEAR bit-15 vertex with two
predecessors emits a triangle; nothing else breaks the strip (TEX0 may
change MID-STRIP — the triangle belongs to the TEX0 of its kicking
vertex).

REGION MAP (chunk06.n1/f03_id43.bin, live-verified frames, office scene +
west rooms — sessions 8 and 9). The file is not all world-space:

  * 0x000000..0x0820C8  static world (slot 0, world space) — drawn through
                        the level kernel (CALL 0x237180) with camera K_L.
  * 0x088840..0x099C80  a 13-slot SUB-OBJECT assembly (the corridor door at
                        (75, 0, -188)) — object-space records whose slot
                        bits select one of 13 live matrices uploaded to
                        VU1 dmem (W = K_L^-1 * M per set, validated
                        orthonormal).
  * 0x09A140..0x09A960  3-slot door CONTROL PANEL, all slots at the
                        corridor door (75, 0, -188.2) (s9; drawn through a
                        late-pass call packet alongside glow quads).
  * 0x09AAC0..0x0A0420  3-slot wall-mounted STATION (ammo/refill unit) at
                        (57.5..60.8, ~15, -290..-296) (s9).
  * standalone object blobs, drawn through the OBJECT kernel (CALL
                        0x23C750) with per-unit placements:
                        0xA05C0 double door — TWO doorway instances:
                        office (109, 0, -252.2)/(101.3, 9, -252.5) and
                        west (57, 0, -220.5)/(57.25, 9, -228.2) (s9);
                        0xA2740 supply crate, drawn twice (s9);
                        0xA3040/0xA3940 pickup items, each drawn twice
                        (these match type-0xB entries in the live
                        placement table at 0x828170 — item pickups, baked
                        statically here);
                        0xA4240 table-top device ("battery bank") at
                        (80.1, 8.2, -244) (s9);
                        0xA8440 lockers at (116.2, 8, -184).
  * everything else     non-record data / tiny tail [0xAC540,end) —
                        skipped.

The exporter bakes the live placements statically. Glow/billboard quads
(library models 20/21/110-118, non-rigid or 2-tri overlays) are NOT
exported. Other level files fall back to "whole file static" (their
movables need their own live capture).

Output: one EMDL v2, bone_count 1 (identity palette), baked vertex color
in the EMD2 normal slot (header flags bit 0). Textures resolve to RGBA8
through a one-frame PCSX2 GS dump's VRAM snapshot (PSMT4+CLUT16 / PSMT8 /
PSMCT32), or — `--p2s` — through a PCSX2 SAVE STATE's GS local memory
(the s15 path proven on the chunk15 NPC: 4 MB at `len(gs.bin) - 0x400000
- 84`, gs_vram.read_localmem). Texture residency is the caller's
responsibility: pass a state captured INSIDE this level.

SCENE MANIFEST: every scene exports in TRUE world coordinates and carries
its own boot config in a plain-text `scene.txt` next to the .emdl files
(read by the port's em_game.c at boot; missing file = office defaults):

    spawn <x> <y> <z> <yaw>     player spawn, world coords + facing (rad)
    collision <file.emcl>       collision filename (export_collision.py
                                writes this key)
    bgm <file.wav>              optional looping level-music cue WAV
    enemy crate <x> <y> <z> <yaw>   placed disguised-container crawler
                                (em_enemy.c EM_ENEMY_KIND_CRATE)
    enemy generator <x> <y> <z> <yaw> kind <k> link <n>
                                generator floor pad (fn 0x0015A2C0,
                                em_enemy.c "GENERATOR KIND")

`--spawn x,y,z[,yaw]` and `--bgm name.wav` write/update this exporter's
keys (other keys and lines are preserved). The old `--offset` spawn-
anchoring bake is gone — the manifest spawn replaced it.

ENEMY LINES (2026-06-10 s27): for the office level the exporter also
rewrites a marker-delimited "enemies" block in scene.txt from the AREA02
placement tables (FINDINGS "ENEMY AI ARCHITECTURE" census): records with
behavior func_001551B0 (placed crawler — in the office a DISGUISED
CONTAINER, param 0x000D = the cardboard-box model) become `enemy crate`
lines; generator records with behavior func_0015A2C0 become active
`enemy generator` lines carrying the decoded placement fields (s28,
FINDINGS "GENERATOR — func_0015A2C0 RESOLVED": kind = +0x08 config
index 0-6 into the D_00248120 footprint recs, link = +0x0A mode-table
selector 0/1/2 where 0 locks the pad inert); the type-2 generator
(func_001E3D90) and the other creature-family behaviors stay comment
lines (unimplemented natively).
CENSUS RESULT: the captured office scene is area SUB-STATE 1 (table
@0x828170, 14 records) and that table places NO enemies at all — 2 doors,
7 pickups, 5 fixtures. The area's crawlers live in sub-state 0 (@0x827830:
17 crawlers + 8 generators) and sub-state 2 (@0x8283D0: same 17 + 8), the
other story states of the same floor. The faithful default scene therefore
has ZERO active enemy lines; the sub-state-0 crawler placements are
emitted as a COMMENTED TOGGLE (`#enemy crate ...`) — uncommenting them
populates the floor, but manifest enemies load before the EM_ENEMY_TEST
spawn and take enemy slot 0, breaking the self-tests' slot-0 asserts, so
they stay off by default.

Multi-zone levels (e.g. the chunk15 snow level): each zone render file is
its own whole-file-static export — run this tool once per file into
scene_snow/NN_zone.emdl; the port's scene loader draws all of them.

Disc-derived output: write only into git-ignored locations
(extermination-port/assets/ is ignored there).

Usage (macOS arm64, decomp repo root):
  .venv/bin/python tools/export_level.py \
      --level extract/chunk06.n1/f03_id43.bin \
      --gsdump extract/gsdump/frame1.gs \
      --out ../extermination-port/assets/scene/00_level.emdl
  # snow level main zone, textures from save state 01, true world coords,
  # manifest spawn = the live state-01 player position:
  .venv/bin/python tools/export_level.py \
      --level extract/chunk15/f12_id44.bin \
      --p2s /tmp/exterm_s01 --spawn 218.592,229.85,201.789,0 \
      --out ../extermination-port/assets/scene_snow/00_zone_main.emdl
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


en = _load("_export_native", "export_native.py")   # tex0_fields, write_emdl


def update_manifest(scene_dir: Path, key: str, value: str) -> None:
    """Write/update one key of the SCENE MANIFEST (scene.txt) in the scene
    directory — the port's per-scene boot config (see module docstring).
    Existing lines for other keys (and comments) are preserved; this key's
    line is replaced in place or appended. Plain text, zero deps, mirrors
    the port's zero-dep parser (em_game.c scene_manifest_load)."""
    mf = scene_dir / "scene.txt"
    lines = mf.read_text().splitlines() if mf.exists() else []
    lines = [ln for ln in lines if not ln.startswith(key + " ")]
    lines.append(f"{key} {value}")
    mf.write_text("\n".join(lines) + "\n")
    print(f"manifest: {mf}: {key} {value}")

REC = 64
W_TOL = 0.25

IDENT34 = [[1.0, 0.0, 0.0, 0.0], [0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 1.0, 0.0]]

# ---------------------------------------------------------------------------
# Placements for chunk06.n1/f03_id43.bin (office scene).
#
# PRIMARY (disc-faithful, 2026-06-10 s11): the engine's per-area object
# PLACEMENT TABLE, static data inside OVERLAY/AREA02.BIN (state-1 table at
# overlay vaddr 0x828170; see tools/placements.py for the registry, record
# layout and the spawner func_001B6990). One record per placed object /
# pickup: pos + Euler rot + class/model/kind. Single-matrix instances are
# fully table-driven; multi-slot assemblies (doors, station, panel) take
# their BASE from the table and their internal slot articulation from the
# live-captured matrices below (slot offsets are runtime animation state,
# not in the table; live slot 0 == table base for every such object).
#
# FALLBACK (no overlay file): the hand-recovered live matrices below,
# captured 2026-06-09 s8/s9 from the running game's per-frame DMA chain:
# per draw unit, W = K_L^-1 * M (K_L = the static level draw's camera
# upload; M = the unit's dmem matrix set). Rows are the first three rows
# of the affine world matrix [R | t].

OFFICE_OVERLAY = "AREA02.BIN"          # office = area 02, sub-state 1
OFFICE_TABLE_VADDR = 0x828170

SUBOBJ_SLOTS = [   # 13-set palette, REF 4420qw @ file 0x88840 (corridor door)
    [[0.0, -0.0, 1.0, 74.999991], [0.0, 1.0, 0.0, 0.000004], [-1.0, 0.0, 0.0, -188.199997]],
    [[0.0, -0.0, 1.0, 74.999991], [0.0, 1.0, 0.0, 0.000004], [-1.0, 0.0, 0.0, -188.199997]],
    [[0.0, -0.0, 1.0, 72.299978], [0.0, 1.0, 0.0, 5.010016], [-1.0, 0.0, 0.0, -184.699261]],
    [[-0.0, -0.0, -1.0, 72.299981], [-0.0, 1.0, -0.0, 5.010055], [1.0, 0.0, -0.0, -191.700766]],
    [[0.0, -0.0, 1.0, 79.191377], [0.0, 1.0, 0.0, 5.01002], [-1.0, 0.0, 0.0, -185.899879]],
    [[0.0, -0.0, 1.0, 74.999991], [0.0, 1.0, 0.0, 0.000004], [-1.0, 0.0, 0.0, -188.199997]],
    [[-0.0, -0.0, -1.0, 67.849986], [-0.0, 1.0, -0.0, 5.010003], [1.0, 0.0, -0.0, -189.299983]],
    [[0.0, -0.0, 1.0, 67.849986], [0.0, 1.0, 0.0, 5.010006], [-1.0, 0.0, 0.0, -187.099964]],
    [[0.0, -0.0, 1.0, 74.999997], [0.0, 1.0, 0.0, 0.000066], [-1.0, 0.0, 0.0, -189.550035]],
    [[-0.0, -0.0, -1.0, 79.19138], [-0.0, 1.0, -0.0, 5.010064], [1.0, 0.0, -0.0, -190.500127]],
    [[0.0, -0.0, 1.0, 72.313878], [0.0, 1.0, 0.0, 4.609347], [-1.0, 0.0, 0.0, -186.549028]],
    [[-0.0, -0.0, -1.0, 72.313875], [-0.0, 1.0, -0.0, 4.609303], [1.0, 0.0, -0.0, -189.851001]],
    [[0.0, -0.0, 1.0, 74.999991], [0.0, 1.0, 0.0, 0.000004], [-1.0, 0.0, 0.0, -188.199997]],
]

BLOB_A8440 = [   # 1040qw @ 0xA8440, one instance
    [[-0.0, -0.0, -1.0, 116.199986], [-0.0, 1.0, -0.0, 8.000108], [1.0, 0.0, 0.0, -184.000062]],
]
BLOB_A05C0 = [   # 520qw @ 0xA05C0, ONE unit with 2 slots (records pick 0/1)
    [[1.0, -0.0, -0.0, 108.999973], [0.0, 1.0, -0.0, 0.000004], [0.0, 0.0, 1.0, -252.200051]],
    [[1.0, -0.0, 0.0, 101.310513], [0.0, 1.0, -0.0, 8.999992], [0.0, 0.0, 1.0, -252.451882]],
]
BLOB_A05C0_W = [   # the SAME 2-slot double door at the WEST doorway (s9 live)
    [[0.0, 0.0, -1.0, 56.999988], [-0.0, 1.0, 0.0, 0.000006], [1.0, -0.0, 0.0, -220.499989]],
    [[0.0, -0.0, -1.0, 57.251972], [-0.0, 1.0, 0.0, 9.000002], [1.0, -0.0, 0.0, -228.189473]],
]
SUBOBJ_9A100 = [   # 130qw 3-slot door control panel; all slots at the corridor door (s9)
    [[-0.0, 0.0, 1.0, 74.999983], [0.0, 1.0, -0.0, -0.000008], [-1.0, -0.0, 0.0, -188.199956]],
] * 3
SUBOBJ_9AA80 = [   # 1430qw 3-slot wall station ("ammo refill"), SW room (s9)
    [[-0.0, 0.0, 1.0, 57.500001], [0.0, 1.0, -0.0, 14.999998], [-1.0, -0.0, 0.0, -292.600003]],
    [[-0.0, 0.0, 1.0, 59.999936], [0.0, 1.0, -0.0, 14.900021], [-1.0, -0.0, 0.0, -295.999924]],
    [[-0.0, 0.0, 1.0, 60.799965], [-0.0, 1.0, -0.0, 15.350014], [-1.0, -0.0, -0.0, -289.999945]],
]
BLOB_A2700 = [   # 130qw supply crate, TWO instances, south room floor (s9)
    [[0.962218, 0.0, 0.27228, 75.699979], [0.0, 1.0, 0.0, -0.000012], [-0.27228, -0.0, 0.962218, -301.999949]],
    [[0.999194, 0.0, -0.040133, 82.500003], [0.0, 1.0, -0.0, -0.000025], [0.040133, -0.0, 0.999194, -302.399962]],
]
BLOB_A4240 = [   # 1040qw table-top device ("battery bank"), one instance (s9)
    [[1.0, 0.0, 0.0, 80.099981], [-0.0, 1.0, -0.0, 8.199997], [0.0, -0.0, 1.0, -243.999971]],
]
BLOB_A3940 = [   # 0x82qw @ 0xA3940, TWO instances
    [[0.99956, -0.0, 0.029672, 115.599991], [-0.0, 1.0, -0.0, 9.399987], [-0.029672, 0.0, 0.99956, -280.09995]],
    [[0.999945, -0.0, -0.010472, 116.299984], [0.0, 1.0, -0.0, 1.499905], [0.010472, 0.0, 0.999945, -289.899902]],
]
BLOB_A3040 = [   # 0x82qw @ 0xA3040, TWO instances
    [[0.978147, -0.0, 0.207912, 116.300018], [-0.0, 1.0, -0.0, 9.400155], [-0.207912, 0.0, 0.978148, -266.600032]],
    [[0.999194, -0.0, 0.040133, 116.599989], [0.0, 1.0, -0.0, 1.500027], [-0.040133, 0.0, 0.999194, -264.199968]],
]

# region = (lo, hi, mode, mats); mode "world" ignores mats, "slots" indexes
# mats by the record slot bits, "instances" replays the region once per mat.
# (lo, hi) extents of the movable-object blob regions, shared by the
# table-driven and fallback region builders:
RGN_CORRIDOR = (0x088840, 0x099C80)    # 13-slot corridor door assembly
RGN_PANEL    = (0x09A140, 0x09A960)    # 3-slot door control panel
RGN_STATION  = (0x09AAC0, 0x0A0420)    # 3-slot wall station (ammo refill)
RGN_DOOR     = (0x0A05C0, 0x0A2640)    # 2-slot double door (per doorway)
RGN_CRATE    = (0x0A2740, 0x0A2F60)    # supply crate        (item 0x0B)
RGN_AMMO_C   = (0x0A3040, 0x0A3860)    # ammo box            (item 0x0C)
RGN_AMMO_D   = (0x0A3940, 0x0A41C0)    # ammo box            (item 0x0D)
RGN_BATTERY  = (0x0A4240, 0x0A8340)    # table-top device, model 0x2C
RGN_LOCKERS  = (0x0A8440, 0x0AC540)    # lockers, model 0x38

CHUNK06N1_REGIONS = [
    (0x000000, 0x0820C8, "world", None),
    (*RGN_CORRIDOR, "slots", SUBOBJ_SLOTS),
    (*RGN_PANEL,    "slots", SUBOBJ_9A100),
    (*RGN_STATION,  "slots", SUBOBJ_9AA80),
    (*RGN_DOOR,     "slots", BLOB_A05C0),
    (*RGN_DOOR,     "slots", BLOB_A05C0_W),   # second doorway instance
    (*RGN_CRATE,    "instances", BLOB_A2700),
    (*RGN_AMMO_C,   "instances", BLOB_A3040),
    (*RGN_AMMO_D,   "instances", BLOB_A3940),
    (*RGN_BATTERY,  "instances", BLOB_A4240),
    (*RGN_LOCKERS,  "instances", BLOB_A8440),
]
CHUNK06N1_SHA1 = "unpinned"   # match by name+size instead (disc data stays local)
CHUNK06N1_SIZE = 0xAC800

# The chunk's id 0x44 file is the collision world (FINDINGS s7) BUT its
# tail carries a SECOND STATIC RENDER-MESH SECTION (448 VIF blocks, same
# record format, slot 0, world space, X[37,120] — the western part of the
# area). The live static level draw REFs blocks from BOTH files (the
# session-6 "id44 markers are not TEX0s" verdict predates the record-phase
# fix and is wrong for this section). (lo, hi) below = the section extent.
CHUNK06N1_SIBLING = ("f02_id44.bin", 0x086B40, 0x173800)

# ---------------------------------------------------------------------------
# AREA02 SUB-STATE 0 — the office MAIN FLOOR (chunk leaf chunk06.n0).
#
# Leaf identity: the soundmap's area_scene_map pins (area 2, sub 0) ->
# chunk06.n0 and (2, 2) -> chunk06.n2; the live-captured back-office scene
# (sub-state 1) is chunk06.n1. Geometry survey (2026-06-10 s28,
# walk_records over every chunk06* file):
#
#   f03_id43.bin (0x13D800)  [0x10,0x13D750)   static world, all slot 0,
#                            X[-35,546] Z[-195,185] — east + center floor.
#   f02_id44.bin (0x1EA000)  collision world (head) + static render TAIL
#                            [0x14FCD0,0x1EA000) X[-349,35] — west floor
#                            (same two-section shape as chunk06.n1's id44).
#   f04_id72.bin (0x22000)   [0x10,0xD1D0) static far-east annex
#                            X[520,550]; +0xD800 = the per-area MODEL
#                            TABLE (27 entries, func_001C6120 directory;
#                            entries span across into f05_id41 — the leaf's
#                            files load contiguously, like chunk15).
#   f05_id41.bin (0x14B800)  model-table entries ONLY (object-space blobs;
#                            no world-space geometry — do not export
#                            whole-file).
#
# Placed objects: OVERLAY/AREA02.BIN sub-state-0 placement table @0x827830
# (58 records) binds actor meshes BY PARAM into the model table (the s23
# rule, validated here: the n1 table's entries 0x0B/0x0C/0x0D/0x0E/0x0F sit
# exactly at the s8/s9 live blob regions for pickup params 0xB/0xC/0xD and
# fixture entries). NOTE the n0 table's entry 0x0D (the crawler disguise,
# param 0x000D) is a 14x14x14 crate — NOT the n1 cardboard box; the s26
# "office crate" came from the SUB-STATE-1 table, which never places a
# crawler.
#
# Doors (class 5, fn 0x001BC350 / 0x001BB860): param 0x16 (model 0x15) at
# (-30.5, 0, -187.3) and param 0x19 (model 0x17) at (440.2, 15, 109.9).
# Spawn: the engine's area-2 spawn tables (boot ELF 0x24B2F4/0x24B444/
# 0x24B594, one 7-record copy per sub-state, record = {f32 pos[3], yaw,
# ...} 0x30 bytes) — entry 5 = (40, 0, -146) yaw -pi/2 lands on the main
# floor next to the rec-0 trigger at (43, 3.5, -147).
#
# TEXTURES: the office GS dump / AREA02 save states are all SUB-STATE 1 —
# zero TEX0-key overlap with this leaf's 242 keys. The leaf carries its own
# GS upload packets instead: chunk06.n0/f02_id44.bin uploads blocks
# 0x2A00..0x3500 (two PSMCT32 IMAGE transfers) and the global library pack
# chunk27/f00_id35.bin uploads 0x1D00..0x2480; together they cover every
# key (1 global + 241 leaf). --uploads replays them into a synthetic VRAM.

OFFICE0_DIRNAME = "chunk06.n0"
OFFICE0_TABLE_OFF = 0x373800        # model table at concat f04_id72+0xD800
OFFICE0_SUBSTATE = 0
OFFICE0_SPAWN = (40.0, 0.0, -146.0, -1.5708)   # engine spawn rec 5
OFFICE0_F03_SIZE = 0x13D800

# Static world regions keyed by (file name, file size) — leaves whose
# render files need more than the whole-file-static fallback.
KNOWN_STATIC_REGIONS = {
    ("f03_id43.bin", 0x13D800): [(0x000010, 0x13D750, "world", None)],
    ("f02_id44.bin", 0x1EA000): [(0x14FCD0, 0x1EA000, "world", None)],
    ("f04_id72.bin", 0x022000): [(0x000010, 0x00D1D0, "world", None)],
}


# ---------------------------------------------------------------------------
# Record walker (corrected phase: [TEX0][ST][color/normal][pos+W])

def walk_records(d: bytes, lo: int = 0, hi: int | None = None):
    """Yield (off, pos, wbits, tex0_qword, uv, attr) for every valid record
    in [lo, hi), or None once per skipped-row run (block headers / VIF tags)
    so the caller can break the running strip; resyncs by 16 bytes."""
    o = lo
    n = len(d) if hi is None else hi
    skipping = False
    while o + REC <= n:
        is_tex = (d[o + 8:o + 16] == b"\x00" * 8 and
                  d[o:o + 8] != b"\x00" * 8)
        st_zw = struct.unpack_from("<2f", d, o + 0x18)
        w = struct.unpack_from("<f", d, o + 0x3C)[0]
        if is_tex and st_zw[0] == 1.0 and st_zw[1] == 0.0 \
                and abs(abs(w) - 1.0) < W_TOL:
            q = int.from_bytes(d[o:o + 8], "little")
            uv = struct.unpack_from("<2f", d, o + 0x10)
            attr = struct.unpack_from("<4f", d, o + 0x20)
            pos = struct.unpack_from("<3f", d, o + 0x30)
            wbits = struct.unpack_from("<I", d, o + 0x3C)[0]
            yield o, pos, wbits, q, uv, attr
            o += REC
            skipping = False
        else:
            if not skipping:
                yield None
                skipping = True
            o += 16     # header / VIF-tag row — resync
    return


def mat_apply(m, v):
    return (m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3],
            m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3],
            m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3])


def mat_rotate(m, v):
    return (m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2],
            m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2],
            m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2])


def attr_to_color(attr, m=None) -> tuple:
    """Baked color, or a stand-in shade for normal-carrying records.

    |xyz| ~= 1 with w ~= 0 marks a unit NORMAL; rotate it by the record's
    placement and light it with the port's stand-in directional light so
    the EMD2 normal slot can carry colors uniformly."""
    x, y, z, w = attr
    n = math.sqrt(x * x + y * y + z * z)
    if abs(n - 1.0) < 0.02 and abs(w) < 0.1:
        if m is not None:
            x, y, z = mat_rotate(m, (x, y, z))
        lx, ly, lz = 0.4, 0.8, 0.45     # port's stand-in light direction
        ll = math.sqrt(lx * lx + ly * ly + lz * lz)
        d = max((x * lx + y * ly + z * lz) / ll, 0.0)
        s = 0.30 + 0.70 * d
        return (s, s, s)
    return (min(max(x, 0.0), 1.0), min(max(y, 0.0), 1.0),
            min(max(z, 0.0), 1.0))


# ---------------------------------------------------------------------------
# Mesh build (GS tristrips -> welded indexed triangles, per-TRIANGLE texture)

class MeshBuilder:
    def __init__(self):
        self.pos, self.col, self.uv, self.tex = [], [], [], []
        self.bone = []
        self.tris = []
        self.weld = {}
        self.tex_table: list[dict] = []
        self.tex_index: dict[int, int] = {}
        self.NO_TEX = 0xFFFFFFFF
        self.n_strip_tris = 0

    def tex_of(self, q0: int) -> int:
        key = q0 & en.TEX0_KEY_MASK
        ti = self.tex_index.get(key)
        if ti is None:
            f = en.tex0_fields(key)
            # PSMT4 / PSMT8 indexed plus PSMCT32 (the backdrop texture)
            if f["psm"] not in (0x00, 0x13, 0x14) or not (4 <= f["tw"] <= 10) \
                    or not (4 <= f["th"] <= 10):
                ti = self.NO_TEX
            else:
                ti = len(self.tex_table)
                f["key"] = key
                self.tex_table.append(f)
            self.tex_index[key] = ti
        return ti

    def vid_of(self, p, c, uv, t):
        key = (round(p[0], 4), round(p[1], 4), round(p[2], 4),
               round(c[0], 3), round(c[1], 3), round(c[2], 3),
               round(uv[0], 5), round(uv[1], 5), t)
        i = self.weld.get(key)
        if i is None:
            i = len(self.pos)
            self.weld[key] = i
            self.pos.append(p)
            self.col.append(c)
            self.bone.append(0)
            self.uv.append(uv)
            self.tex.append(t)
        return i

    def add_stream(self, records, placement):
        """Consume an iterable of record tuples (pos, wbits, q, uv, attr)
        or None (strip break). GS tristrip semantics: EVERY record enters
        the strip queue; a record with bit 15 CLEAR and >= 2 predecessors
        kicks a triangle, textured by the KICK vertex's TEX0. `placement`
        maps a record to its 3x4 world matrix (or None = as-is)."""
        run = []        # (raw pos, color, uv) of the last <=2 entries + ids
        for rec in records:
            if rec is None:
                run = []
                continue
            pos, wbits, q, uv, attr = rec
            m = placement(wbits) if placement else None
            wp = mat_apply(m, pos) if m else tuple(pos)
            col = attr_to_color(attr, m)
            run.append((wp, col, uv, q))
            if len(run) > 3:
                run.pop(0)
            if (wbits & 0x8000) == 0 and len(run) == 3:
                t = self.tex_of(q)            # kick vertex's TEX0
                (pa, ca, ua, _qa), (pb, cb, ub, _qb), (pc, cc, uc, _qc) = run
                if pa != pb and pb != pc and pa != pc:
                    a = self.vid_of(pa, ca, ua, t)
                    b = self.vid_of(pb, cb, ub, t)
                    c = self.vid_of(pc, cc, uc, t)
                    # parity bit 14 = winding; put the kick vertex first
                    # (Metal flat-shading provoking vertex) and flip by
                    # parity so consistent-winding backends stay correct.
                    if wbits & 0x4000:
                        self.tris.extend((c, b, a))
                    else:
                        self.tris.extend((c, a, b))
                    self.n_strip_tris += 1


# ---------------------------------------------------------------------------
# Table-driven placement (disc-faithful path)

def mat34_mul(a, b):
    """3x4 affine compose a*b (both [R|t] as 3 rows of 4)."""
    return [[a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j]
             + (a[i][3] if j == 3 else 0.0) for j in range(4)]
            for i in range(3)]


def mat34_inv_rigid(m):
    """Inverse of a rigid 3x4 [R|t]: [R^T | -R^T t]."""
    r = [[m[j][i] for j in range(3)] for i in range(3)]
    t = [-(r[i][0] * m[0][3] + r[i][1] * m[1][3] + r[i][2] * m[2][3])
         for i in range(3)]
    return [[r[0][0], r[0][1], r[0][2], t[0]],
            [r[1][0], r[1][1], r[1][2], t[1]],
            [r[2][0], r[2][1], r[2][2], t[2]]]


def anchor_slots(live_slots, base34):
    """Re-anchor a live-captured slot-matrix set on a table base placement:
    slot_k = B * L0^-1 * L_k. The internal articulation (door panels,
    station sub-parts) is runtime state, not in the table; live slot 0
    equals the table base for every captured assembly, so this reproduces
    the capture exactly while taking the placement from the disc."""
    l0inv = mat34_inv_rigid(live_slots[0])
    return [mat34_mul(base34, mat34_mul(l0inv, lk)) for lk in live_slots]


def table_driven_regions(level_path: Path):
    """Build the chunk06.n1 region list from the disc placement table
    (OVERLAY/AREA02.BIN state-1 table @0x828170; see tools/placements.py).
    Returns None when the overlay file is missing (caller falls back to
    the embedded live matrices)."""
    ov_path = level_path.parent.parent / "OVERLAY" / OFFICE_OVERLAY
    if not ov_path.exists():
        return None
    pl = _load("_placements", "placements.py")
    entries = pl.parse_table(ov_path.read_bytes(), OFFICE_TABLE_VADDR)

    doors = [e for e in entries
             if (e.spawn_class & 0x1F) == 5 and e.model == 3]
    corridor = [e for e in entries if (e.spawn_class & 0xFF) == 8]
    fixture = {e.model: e for e in entries
               if e.kind == 4 and (e.spawn_class & 0x1F) in (4, 6)
               and e.spawn_class & 0x80}
    pickups = lambda item: [e for e in entries
                            if e.kind == 0xB and e.param == item]

    need = (doors and corridor and
            all(m in fixture for m in (0x36, 0x37, 0x2C, 0x38)))
    if not need:
        print("warning: placement table lacks expected office objects — "
              "falling back to embedded live matrices")
        return None

    regions = [(0x000000, 0x0820C8, "world", None)]
    regions.append((*RGN_CORRIDOR, "slots",
                    anchor_slots(SUBOBJ_SLOTS, corridor[0].matrix34())))
    regions.append((*RGN_PANEL, "slots",
                    anchor_slots(SUBOBJ_9A100, fixture[0x36].matrix34())))
    regions.append((*RGN_STATION, "slots",
                    anchor_slots(SUBOBJ_9AA80, fixture[0x37].matrix34())))
    for e in doors:           # one 2-slot replay per doorway in the table
        regions.append((*RGN_DOOR, "slots",
                        anchor_slots(BLOB_A05C0, e.matrix34())))
    for rgn, item in ((RGN_CRATE, 0x0B), (RGN_AMMO_C, 0x0C),
                      (RGN_AMMO_D, 0x0D)):
        ents = pickups(item)
        if ents:
            regions.append((*rgn, "instances",
                            [e.matrix34() for e in ents]))
    regions.append((*RGN_BATTERY, "instances", [fixture[0x2C].matrix34()]))
    regions.append((*RGN_LOCKERS, "instances", [fixture[0x38].matrix34()]))
    print(f"placements: table-driven from {ov_path.name} @"
          f"{OFFICE_TABLE_VADDR:#x} ({len(entries)} entries: "
          f"{len(doors)} doors, {sum(len(pickups(i)) for i in (0xB, 0xC, 0xD))}"
          " pickups)")
    return regions


# ---------------------------------------------------------------------------
# Enemy manifest emission (scene.txt enemy lines from the placement tables)

OFFICE_SCENE_SUBSTATE = 1      # the captured office scene's story sub-state

FN_CRAWLER = 0x001551B0        # placed crawler / disguised container
FN_DOORS = (0x001BC350, 0x001BB860)
FN_GENERATOR = 0x0015A2C0      # generator floor pad — implemented in the port
FN_GENERATORS = {               # class-0x0D runtime enemy spawn points
    FN_GENERATOR: "generator (leech spawn point, D_00248120 config)",
    0x001E3D90: "generator type 2 (class 0x0D model 1)",
}
FN_ENEMY_MISC = {               # other creature-family behaviors (s22 census)
    0x00156620: "destructible nest/egg fixture",
    0x00156F30: "destructible fixture variant",
    0x00158BD0: "class-8 creature",
    0x00158D30: "class-8 creature (sibling)",
    0x00158810: "class-0x86 creature",
    0x001581A0: "class-0x44 creature",
    0x001582E0: "class-0x44 creature",
    0x00158430: "class-0x44 creature",
    0x00158EC0: "class-0x84 creature-family fixture",
    0x0015A070: "minor creature",
    0x00159E70: "minor creature",
}
ENEMY_BLOCK_BEGIN = "# --- enemies (export_level.py, AREA02 placement tables) ---"
ENEMY_BLOCK_END = "# --- end enemies ---"


def _placement_census(entries) -> dict:
    """Bucket one placement table's records (FINDINGS s11/s22 categories)."""
    c = {"crawler": 0, "generator": 0, "enemy_misc": 0, "door": 0,
         "pickup": 0, "deferred": 0, "prop": 0}
    for e in entries:
        if e.behavior == FN_CRAWLER:
            c["crawler"] += 1
        elif e.behavior in FN_GENERATORS:
            c["generator"] += 1
        elif e.behavior in FN_ENEMY_MISC:
            c["enemy_misc"] += 1
        elif e.behavior in FN_DOORS:
            c["door"] += 1
        elif (e.spawn_class & 0xFF) == 0x0B:
            c["deferred"] += 1      # scripted/deferred, skipped at area load
        elif e.kind == 0xB:
            c["pickup"] += 1
        else:
            c["prop"] += 1
    return c


def _enemy_lines(entries, prefix: str = "") -> list[str]:
    """Manifest lines for one table's enemy-class records. Crawlers become
    `enemy crate` lines (the office placed crawler is a DISGUISED CONTAINER;
    param 0x000D binds the cardboard-box model — FINDINGS s22/s23);
    fn-0x0015A2C0 generators become `enemy generator` lines with the
    decoded kind/link placement fields (FINDINGS "GENERATOR —
    func_0015A2C0 RESOLVED"); the type-2 generator and the misc
    creature-family behaviors are unimplemented in the port and become
    comment lines."""
    out = []
    for e in entries:
        x, y, z = (f"{v:.6g}" for v in e.pos)
        yaw = f"{e.rot[1]:.6g}"
        if e.behavior == FN_CRAWLER:
            out.append(f"{prefix}enemy crate {x} {y} {z} {yaw}")
        elif e.behavior == FN_GENERATOR:
            out.append(f"{prefix}enemy generator {x} {y} {z} {yaw} "
                       f"kind {e.kind} link {e.link}")
        elif e.behavior in FN_GENERATORS:
            out.append(f"# generator (fn {e.behavior:#08x}, unimplemented) "
                       f"pos ({x}, {y}, {z}) kind {e.kind}")
        elif e.behavior in FN_ENEMY_MISC:
            out.append(f"# enemy-family (fn {e.behavior:#08x}, "
                       f"{FN_ENEMY_MISC[e.behavior]}, unimplemented) "
                       f"pos ({x}, {y}, {z})")
    return out


def emit_enemy_manifest(scene_dir: Path, ov_path: Path,
                        substate: int = OFFICE_SCENE_SUBSTATE,
                        cap: int | None = None,
                        spawn: tuple | None = None) -> None:
    """Rewrite the marker-delimited enemies block of scene.txt from the
    AREA02 placement tables. The given sub-state's table drives the ACTIVE
    lines; when it places no crawlers (the sub-state-1 census result — see
    the module docstring) the sub-state-0 crawler placements are appended
    as a commented toggle instead. `cap` (the port's EM_ENEMY_MAX) bounds
    the active crawler lines: the placements FARTHEST from `spawn` (XZ
    distance) overflow into commented lines with a note."""
    pl = sys.modules.get("_placements") or _load("_placements",
                                                 "placements.py")
    data = ov_path.read_bytes()
    tables = pl.KNOWN_TABLES[OFFICE_OVERLAY]
    per = [pl.parse_table(data, v) for v in tables]
    scene_entries = per[substate]
    cen = _placement_census(scene_entries)

    desc = ("the captured office scene" if substate == 1
            else "the office MAIN-FLOOR story beat" if substate == 0
            else "the office post-event floor")
    block = [ENEMY_BLOCK_BEGIN]
    block.append(f"# scene table = sub-state {substate} "
                 f"@{tables[substate]:#x} ({desc}), "
                 f"{len(scene_entries)} records:")
    block.append(f"#   {cen['crawler']} crawlers (fn 0x001551B0), "
                 f"{cen['generator']} generators (fn 0x0015A2C0), "
                 f"{cen['door']} doors, {cen['pickup']} pickups, "
                 f"{cen['prop']} fixtures/props"
                 + (f", {cen['deferred']} deferred" if cen["deferred"]
                    else ""))
    for ss, ents in enumerate(per):
        if ss == substate:
            continue
        c = _placement_census(ents)
        block.append(f"# sub-state {ss} @{tables[ss]:#x}: {len(ents)} "
                     f"records — {c['crawler']} crawlers, "
                     f"{c['generator']} generators, "
                     f"{c['enemy_misc']} misc creature-family, "
                     f"{c['door']} doors, {c['deferred']} deferred(0x0B)")
    if cen["generator"]:
        block.append('# enemy generator lines (fn 0x15a2c0, em_enemy.c '
                     '"GENERATOR KIND"): kind = the')
        block.append("# D_00248120 footprint config 0-6, link = mode-table "
                     "selector (0 = locked inert,")
        block.append("# 1/2 = the engine's count-table draw). Pads live in "
                     "their own EM_GENERATOR_MAX")
        block.append('# pool — no crate-slot impact. FINDINGS "GENERATOR — '
                     'func_0015A2C0 RESOLVED".')

    overflow = []
    emit_entries = scene_entries
    crawlers = [e for e in scene_entries if e.behavior == FN_CRAWLER]
    if cap is not None and spawn is not None and len(crawlers) > cap:
        ranked = sorted(crawlers, key=lambda e:
                        (e.pos[0] - spawn[0]) ** 2 + (e.pos[2] - spawn[2]) ** 2)
        dropped = {id(e) for e in ranked[cap:]}
        overflow = [e for e in scene_entries if id(e) in dropped]
        emit_entries = [e for e in scene_entries if id(e) not in dropped]
    active = _enemy_lines(emit_entries)
    block += active
    if overflow:
        block.append(f"# OVERFLOW: the table places {len(crawlers)} "
                     f"crawler crates but the port caps at EM_ENEMY_MAX "
                     f"{cap} manifest slots —")
        block.append("# the placement(s) FARTHEST from the spawn stay "
                     "commented out:")
        block += _enemy_lines(overflow, prefix="#")
    if cen["crawler"] == 0:
        block.append("# this sub-state places NO enemies — the faithful "
                     "default scene has none.")
        block.append("# TOGGLE (off by default): the sub-state-0 crawler "
                     "placements (disguised")
        block.append("# cardboard-box containers, param 0x000D). Uncomment "
                     "the #enemy lines to")
        block.append("# populate the floor. NOTE: manifest enemies load "
                     "before the EM_ENEMY_TEST")
        block.append("# spawn and take enemy slot 0, breaking the "
                     "self-tests' slot-0 asserts —")
        block.append("# keep them commented for deterministic test runs.")
        block += _enemy_lines(per[0], prefix="#")
    block.append(ENEMY_BLOCK_END)

    mf = scene_dir / "scene.txt"
    lines = mf.read_text().splitlines() if mf.exists() else []
    if ENEMY_BLOCK_BEGIN in lines:
        b = lines.index(ENEMY_BLOCK_BEGIN)
        e = lines.index(ENEMY_BLOCK_END) if ENEMY_BLOCK_END in lines \
            else len(lines) - 1
        lines = lines[:b] + lines[e + 1:]
    while lines and not lines[-1].strip():
        lines.pop()
    lines += block
    mf.write_text("\n".join(lines) + "\n")
    print(f"manifest: {mf}: enemies block — sub-state "
          f"{substate}: {cen['crawler'] - len(overflow)} active crawler "
          f"line(s), {cen['generator']} generator line(s), "
          f"{len(overflow)} overflow"
          + (f"; toggle lines: {_placement_census(per[0])['crawler']} "
             f"(sub-state 0)" if cen["crawler"] == 0 else ""))


# ---------------------------------------------------------------------------
# Door DESTINATION links (scene.txt `door ... goto` annotation).
#
# Decoded 2026-06-10 (this session) from the BOOT ELF's static tables —
# the area-transition authoring data of FINDINGS "AREA TRANSITION
# LIFECYCLE" s22, now read offline (no live capture):
#
#   D_0024E140[area]      per-area DOOR DESTINATION TABLE base (static
#                         .data in the boot ELF — NOT overlay-resident).
#                         AREA02 -> 0x24DFC0, 4 records (door ids 0..3).
#                         One 4-byte record per door id; read two ways by
#                         the commit func_001BC150 depending on the door
#                         id's bit7 (= placement flags2 bit7):
#                           bit7 SET  {next_area, entry, has_sub, sub}
#                           bit7 CLEAR {entry_from_side0, entry_from_side1}
#   D_0024D650[area]      spawn DESC -> per-sub-state spawn table vaddrs.
#                         AREA02 desc 0x24D610 -> sub tables 0x24B560 /
#                         0x24B6B0 / 0x24B800 (0x30-byte records
#                         {f32 pos[3], f32 yaw, ...}; the s22 live-read
#                         entries byte-match these static tables).
#
# AREA02 census (placement flags2 -> dest record):
#   sub 0: door id 0|0x80 (model 0x15) -> AREA 1 sub 0 entry 3
#          door id 3|0x80 (model 0x17) -> AREA 4 sub 0 entry 0
#   sub 1: door id 1|0x80 (west m03)   -> AREA 1 sub 0 entry 5
#          door id 2      (office m03) -> room move, entries {3, 2}
#   sub 2: door id 0|0x80 (same as sub 0)
#
# => NO real door pair links AREA02 sub-state 1 <-> sub-state 0: the
# engine routes between the two office story states through OTHER areas
# (AREA01 door 1 -> sub 0 entry 0; AREA01 door 3 -> sub 1 entry 1). The
# optional --synthetic-link therefore wires the two exported scenes'
# NEAREST doors with a FLAGGED synthetic goto (arrival = the target
# sub-state's real spawn entry nearest its partner door) so the port's
# scene-switch machinery has a testable in-game path.

AREA02 = 2
DEST_PTR_ARRAY = 0x0024E140      # D_0024E140: per-area dest-table base
SPAWN_DESC_ARRAY = 0x0024D650    # D_0024D650: per-area spawn desc
SPAWN_REC_SIZE = 0x30
GOTO_MARK = "# door-goto:"       # idempotency marker for our comments


class BootElf:
    """Minimal vaddr reader over the user's local boot ELF (read-only;
    nothing disc-derived is emitted beyond the scene.txt coordinates,
    which live in the git-ignored assets tree)."""

    def __init__(self, path: Path):
        self.data = path.read_bytes()
        e_phoff, = struct.unpack_from("<I", self.data, 0x1C)
        e_phnum, = struct.unpack_from("<H", self.data, 0x2C)
        self.segs = []
        for i in range(e_phnum):
            p_type, p_off, p_vaddr, _, p_filesz = struct.unpack_from(
                "<5I", self.data, e_phoff + 32 * i)
            if p_type == 1 and p_filesz:
                self.segs.append((p_vaddr, p_off, p_filesz))

    def read(self, vaddr: int, n: int) -> bytes:
        for va, off, sz in self.segs:
            if va <= vaddr and vaddr + n <= va + sz:
                o = off + (vaddr - va)
                return self.data[o:o + n]
        raise ValueError(f"vaddr {vaddr:#x} not in a LOAD segment")

    def u32(self, vaddr: int) -> int:
        return struct.unpack("<I", self.read(vaddr, 4))[0]


def area2_dest_table(elf: BootElf) -> list[bytes]:
    """AREA02's destination records (door id -> 4 bytes). The table runs
    from D_0024E140[2] to the next area's base (AREA03's), giving the
    record count without a stored length."""
    base = elf.u32(DEST_PTR_ARRAY + 4 * AREA02)
    nxt = min(p for a in range(16)
              if (p := elf.u32(DEST_PTR_ARRAY + 4 * a)) > base)
    return [elf.read(base + 4 * d, 4) for d in range((nxt - base) // 4)]


def area2_spawn_tables(elf: BootElf) -> list[list[tuple]]:
    """AREA02's per-sub-state spawn tables as [(x, y, z, yaw), ...].
    Record = {f32 pos[3], f32 yaw @+0xC, ...} (s22, byte-verified)."""
    desc = elf.u32(SPAWN_DESC_ARRAY + 4 * AREA02)
    tbls = []
    ptrs = [elf.u32(desc + 4 * s) for s in range(4)]
    for si, tbl in enumerate(ptrs):
        if not tbl:
            break
        end = min([p for p in ptrs if p > tbl] + [tbl + 7 * SPAWN_REC_SIZE])
        recs = []
        for e in range((end - tbl) // SPAWN_REC_SIZE):
            recs.append(struct.unpack(
                "<4f", elf.read(tbl + e * SPAWN_REC_SIZE, 0x10)))
        tbls.append(recs)
    return tbls


def _sub_doors(ov_data: bytes, sub: int):
    """Door placement records of one AREA02 sub-state table."""
    pl = sys.modules.get("_placements") or _load("_placements",
                                                 "placements.py")
    tables = pl.KNOWN_TABLES[OFFICE_OVERLAY]
    return [e for e in pl.parse_table(ov_data, tables[sub])
            if e.behavior in FN_DOORS]


def annotate_door_goto(args) -> int:
    """--door-goto DIR: annotate DIR/scene.txt's `door` lines with the
    decoded transition destination:

        door <file> <x> <y> <z> <yaw> <r> goto <scene-dir> <sx> <sy> <sz> <syaw>

    <scene-dir> is the target scene's SIBLING DIRECTORY NAME (the port
    resolves it against the current scene dir's parent) and the spawn is
    the target sub-state's real spawn-table record (pos + exit yaw).
    A goto is emitted only when the decoded destination is an EXPORTED
    sub-state (--exported sub=dirname,...); inter-area destinations and
    intra-area room moves (bit7 clear — the port re-places in the same
    scene) keep the plain 6-field line. Lines are matched to placement
    records by position; existing goto fields and our marker comments are
    stripped first (idempotent)."""
    scene_dir = Path(args.door_goto)
    mf = scene_dir / "scene.txt"
    if not mf.exists():
        sys.exit(f"{mf} not found")
    if args.sub is None:
        sys.exit("--door-goto needs --sub (the scene's AREA02 sub-state)")
    exported = {}
    for kv in (args.exported or "").split(","):
        if kv:
            k, _, v = kv.partition("=")
            exported[int(k)] = v
    elf = BootElf(Path(args.elf))
    ov_data = Path(args.overlay).read_bytes()
    dest = area2_dest_table(elf)
    spawns = area2_spawn_tables(elf)
    doors = _sub_doors(ov_data, args.sub)

    out, n_goto, syn_done = [], 0, False
    lines = mf.read_text().splitlines()
    lines = [ln for ln in lines if not ln.startswith(GOTO_MARK)]

    # --synthetic-link pre-pass: pick THIS scene's door nearest the
    # partner exported sub-state's nearest door (see the header note).
    syn = None
    if args.synthetic_link:
        partner = next((s for s in exported if s != args.sub), None)
        if partner is None:
            sys.exit("--synthetic-link needs the partner sub-state in "
                     "--exported")
        pdoors = _sub_doors(ov_data, partner)
        pair = min(((d, p) for d in doors for p in pdoors),
                   key=lambda dp: (dp[0].pos[0] - dp[1].pos[0]) ** 2 +
                                  (dp[0].pos[2] - dp[1].pos[2]) ** 2)
        tgt_door = pair[1]
        ent = min(range(len(spawns[partner])),
                  key=lambda e: (spawns[partner][e][0] - tgt_door.pos[0]) ** 2
                              + (spawns[partner][e][2] - tgt_door.pos[2]) ** 2)
        syn = (pair[0], partner, ent)

    for ln in lines:
        f = ln.split()
        if not (f and f[0] == "door" and len(f) >= 7):
            out.append(ln)
            continue
        f = f[:7]                      # strip any previous goto fields
        x, z = float(f[2]), float(f[4])
        rec = next((e for e in doors
                    if abs(e.pos[0] - x) < 0.5 and abs(e.pos[2] - z) < 0.5),
                   None)
        if rec is None:
            out.append(" ".join(f))
            print(f"door-goto: no placement record matches door at "
                  f"({f[2]}, {f[4]}) — line left plain")
            continue
        door_id = rec.flags2 & 0x7F
        drec = dest[door_id] if door_id < len(dest) else b"\0\0\0\0"
        if not (rec.flags2 & 0x80):
            out.append(f"{GOTO_MARK} door id {door_id} = intra-area room "
                       f"move, entries {{{drec[0]}, {drec[1]}}} — same "
                       f"scene, no goto (em_door re-places)")
            out.append(" ".join(f))
            continue
        n_area, entry = drec[0], drec[1]
        n_sub = drec[3] if drec[2] else 0
        if n_area == AREA02 and n_sub in exported:
            sx, sy, sz, syaw = spawns[n_sub][entry]
            out.append(f"{GOTO_MARK} door id {door_id}|0x80 -> AREA02 "
                       f"sub {n_sub} entry {entry} (dest table 0x24DFC0)")
            out.append(" ".join(f) + f" goto {exported[n_sub]} {sx:.6g} "
                       f"{sy:.6g} {sz:.6g} {syaw:.6g}")
            n_goto += 1
            continue
        if syn and rec is syn[0]:
            _, psub, ent = syn
            sx, sy, sz, syaw = spawns[psub][ent]
            out.append(f"{GOTO_MARK} SYNTHETIC LINK (flagged): the real "
                       f"dest of door id {door_id}|0x80 is AREA "
                       f"{n_area:02d} sub {n_sub} entry {entry} (not "
                       f"exported); no real door pair links AREA02 sub "
                       f"{args.sub} <-> sub {psub} (the engine routes "
                       f"via other areas), so this wires the two "
                       f"exported scenes' nearest doors. Arrival = sub "
                       f"{psub}'s real spawn entry {ent}.")
            out.append(" ".join(f) + f" goto {exported[psub]} {sx:.6g} "
                       f"{sy:.6g} {sz:.6g} {syaw:.6g}")
            n_goto += 1
            syn_done = True
            continue
        out.append(f"{GOTO_MARK} door id {door_id}|0x80 -> AREA "
                   f"{n_area:02d} sub {n_sub} entry {entry} — target not "
                   f"exported, no goto")
        out.append(" ".join(f))

    mf.write_text("\n".join(out) + "\n")
    print(f"manifest: {mf}: {n_goto} goto link(s)"
          + (" (1 SYNTHETIC, flagged)" if syn_done else ""))
    return 0


# ---------------------------------------------------------------------------
# AREA02 sub-state 0: placed objects + doors from the per-area model table
# (see the OFFICE0 constants above for the leaf/table identification).

OFFICE0_FN_GLOW = 0x001C4960            # billboard family (no port path)
OFFICE0_FN_ENV = (0x001C4CB0, 0x0022DCD0)   # camera/env objects, no mesh


def office0_concat(dirp: Path) -> bytes:
    """Byte-concatenation of the leaf's files in index order — the engine
    loads them contiguously (the chunk15 finding), and the n0 model
    table's entries run across the f04_id72/f05_id41 boundary."""
    blob = bytearray()
    for f in sorted(dirp.glob("*.bin")):
        blob += f.read_bytes()
    return bytes(blob)


def office0_uploads(dirp: Path, args) -> list:
    """Default texel source for the office0 modes: the leaf's own GS
    upload packets (blocks 0x2A00..0x3500) + the global library pack
    (chunk27/f00_id35.bin, 0x1D00..0x2480)."""
    if args.uploads:
        return [Path(s) for s in args.uploads.split(",")]
    ups = [dirp / "f02_id44.bin",
           dirp.parent / "chunk27" / "f00_id35.bin"]
    return [p for p in ups if p.exists()]


def office0_bake_placed(d: bytes, placements):
    """Bake model-table entries at world placements: `placements` is a
    list of (param, mat34-or-None). All node slots are baked with the
    placement matrix — per-node rest offsets are runtime state we have no
    live capture of for this sub-state (FLAGGED: articulated sub-parts of
    multi-node entries may sit at their model-local origin)."""
    props = sys.modules.get("_export_props_lvl") or _load(
        "_export_props_lvl", "export_props.py")
    raw_pos, raw_col, raw_bone, raw_uv, raw_tex = [], [], [], [], []
    tris = []
    weld = {}
    tex_table: list[dict] = []
    tex_of = props.make_tex_of(tex_table, {})

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

    for param, m in placements:
        off = props.table_entry_offset(d, OFFICE0_TABLE_OFF, param)
        ntri = 0
        for q, corners, parity in props.model_tris(d, off):
            t = tex_of(q)
            ids = [vid_of(mat_apply(m, p) if m else tuple(p),
                          props.attr_color(a, m), uv, t)
                   for p, a, uv, _q in corners]
            a, b, c = ids
            if parity:
                tris.extend((c, b, a))
            else:
                tris.extend((c, a, b))
            ntri += 1
        where = (f"({m[0][3]:.1f}, {m[1][3]:.1f}, {m[2][3]:.1f})"
                 if m else "model-local")
        print(f"  entry {param:#04x}: {ntri} tris at {where}")
    return [(raw_pos, raw_col, tris, raw_bone, raw_uv, raw_tex)], tex_table


def _office0_placements(dirp: Path):
    """(concat, model-table count, placement records of sub-state 0)."""
    d = office0_concat(dirp)
    cnt = struct.unpack_from("<I", d, OFFICE0_TABLE_OFF)[0]
    if not (0 < cnt <= 0x100):
        sys.exit(f"{dirp}: no model table at concat {OFFICE0_TABLE_OFF:#x} "
                 f"(count {cnt:#x}) — wrong leaf?")
    ov_path = dirp.parent / "OVERLAY" / OFFICE_OVERLAY
    if not ov_path.exists():
        sys.exit(f"{ov_path} missing — the office0 modes are placement-"
                 "table driven")
    pl = sys.modules.get("_placements") or _load("_placements",
                                                 "placements.py")
    entries = pl.parse_table(
        ov_path.read_bytes(),
        pl.KNOWN_TABLES[OFFICE_OVERLAY][OFFICE0_SUBSTATE])
    return d, cnt, entries


def export_office0_placed(args):
    """--office0-placed DIR: every static placed object of the AREA02
    sub-state-0 table baked at its placement matrix into one EMDL.
    Excluded (each reported): crawlers + generators (the scene.txt enemy
    block), doors (interactive, --office0-doors), class-0x0B deferred
    spawns, glow/billboard records, env/camera objects, param-0 records
    (no model bind), params absent from the 27-entry table. The misc
    creature-family destructibles (nest/egg fixtures) ARE baked — they
    have authentic meshes; natively they are static scenery (flagged)."""
    dirp = Path(args.office0_placed)
    d, cnt, entries = _office0_placements(dirp)
    dirview = struct.unpack_from(f"<{cnt}I", d, OFFICE0_TABLE_OFF + 4)

    placements, skipped = [], []
    for e in entries:
        cls = e.spawn_class & 0xFF
        if e.behavior == FN_CRAWLER or e.behavior in FN_GENERATORS:
            continue                          # scene.txt enemies block
        if e.behavior in FN_DOORS:
            continue                          # --office0-doors
        if cls == 0x0B:
            skipped.append((e, "deferred class-0x0B scripted spawn"))
        elif e.behavior == OFFICE0_FN_GLOW:
            skipped.append((e, "glow/billboard (no billboard path)"))
        elif e.behavior in OFFICE0_FN_ENV:
            skipped.append((e, "env/camera object (no geometry)"))
        elif e.param == 0:
            skipped.append((e, "param 0 (no model bind)"))
        elif e.param >= cnt or dirview[e.param] == 0xFFFFFFFF:
            skipped.append((e, f"param {e.param:#x} absent from the "
                            f"{cnt}-entry table"))
        else:
            placements.append((e.param, e.matrix34()))
    for e, why in skipped:
        print(f"  skipped [{e.index:2d}] cls={e.spawn_class:04x} "
              f"param={e.param:04x} at ({e.pos[0]:.1f}, {e.pos[1]:.1f}, "
              f"{e.pos[2]:.1f}): {why}")

    sections, tex_table = office0_bake_placed(d, placements)
    pos = sections[0][0]
    if not pos:
        sys.exit("no geometry produced")
    xs = [p[0] for p in pos]
    ys = [p[1] for p in pos]
    zs = [p[2] for p in pos]
    print(f"placed objects: {len(placements)} instances, {len(pos)} verts, "
          f"{len(sections[0][2]) // 3} tris, {len(tex_table)} textures")
    print(f"  bbox X[{min(xs):.1f},{max(xs):.1f}] "
          f"Y[{min(ys):.1f},{max(ys):.1f}] Z[{min(zs):.1f},{max(zs):.1f}]")

    tex_entries, tex_blob = build_texture_blob(
        None, tex_table, None, office0_uploads(dirp, args))
    en.write_emdl(Path(args.out), sections, [], [-1], [[en.mat_identity()]],
                  30.0, tex_entries, tex_blob, flags=1)
    return 0


def export_office0_doors(args):
    """--office0-doors DIR: the sub-state-0 table's two interactive doors
    (class 5; fn 0x001BC350 west / 0x001BB860 east) carved MODEL-LOCAL
    from the per-area model table into <out>/doors/door_mXX.emdl (XX = the
    record's model byte) and written as manifest `door` lines (positions /
    yaw from the table, the 12.0-unit use-scan radius). --out is the SCENE
    DIRECTORY for this mode. All node slots bake at identity — the leaf
    rest offsets are unknown without a live capture (flagged; the n1
    evidence says they can be nonzero). The doorsfx pair stays the s26
    family-2 office pair (the west door's link is 0x02xx); the east
    door's family-6 pair is unresolved (D_0024DB80 is BSS — needs live)."""
    dirp = Path(args.office0_doors)
    scene_dir = Path(args.out)
    d, cnt, entries = _office0_placements(dirp)
    doors = [e for e in entries if e.behavior in FN_DOORS]
    if not doors:
        sys.exit("no door records in the sub-state-0 table")
    (scene_dir / "doors").mkdir(parents=True, exist_ok=True)

    ups = office0_uploads(dirp, args)
    lines = []
    for e in doors:
        name = f"doors/door_m{e.model:02x}.emdl"
        sections, tex_table = office0_bake_placed(d, [(e.param, None)])
        tex_entries, tex_blob = build_texture_blob(None, tex_table, None,
                                                   ups)
        en.write_emdl(scene_dir / name, sections, [], [-1],
                      [[en.mat_identity()]], 30.0, tex_entries, tex_blob,
                      flags=1)
        pos = sections[0][0]
        print(f"door [{e.index}] model {e.model:#04x} param {e.param:#04x} "
              f"link {e.link:#06x} -> {name}: {len(pos)} verts, "
              f"{len(sections[0][2]) // 3} tris")
        lines.append(f"door {name} {e.pos[0]:.6g} {e.pos[1]:.6g} "
                     f"{e.pos[2]:.6g} {e.rot[1]:.6g} 12")

    mf = scene_dir / "scene.txt"
    old = mf.read_text().splitlines() if mf.exists() else []
    old = [ln for ln in old if not ln.startswith("door ")]
    mf.write_text("\n".join(old + lines) + "\n")
    print(f"manifest: {mf}: {len(lines)} door line(s)")
    update_manifest(scene_dir, "doorsfx", "0x3FD 0x3FE")
    return 0


def load_level_mesh(level_path: Path):
    """Decode the render mesh into one EMDL section using the per-file
    region map (live placements baked in). Returns (sections, tex_table,
    n_tris)."""
    d = level_path.read_bytes()
    sibling = None
    if len(d) == CHUNK06N1_SIZE and level_path.name == "f03_id43.bin":
        regions = table_driven_regions(level_path)
        if regions is None:
            print("note: OVERLAY/AREA02.BIN not found — using embedded "
                  "live-captured placements")
            regions = CHUNK06N1_REGIONS
        sib_name, sib_lo, sib_hi = CHUNK06N1_SIBLING
        sib_path = level_path.parent / sib_name
        if sib_path.exists():
            sibling = (sib_path.read_bytes(), sib_lo, sib_hi)
        else:
            print(f"warning: {sib_path} missing — the western half of the "
                  "area will be absent")
    elif (level_path.name, len(d)) in KNOWN_STATIC_REGIONS:
        regions = KNOWN_STATIC_REGIONS[(level_path.name, len(d))]
        print(f"regions: known static map for {level_path.name} "
              f"({len(regions)} region(s))")
    else:
        print("note: no live region map for this file — exporting the whole "
              "file as static world (movable sub-objects may be missing or "
              "mis-placed)")
        regions = [(0, len(d), "world", None)]

    b = MeshBuilder()
    if sibling is not None:
        sd, slo, shi = sibling
        b.add_stream((r if r is None else r[1:] for r in
                      walk_records(sd, slo, shi)), None)
    for lo, hi, mode, mats in regions:
        if mode == "world":
            b.add_stream((r if r is None else r[1:] for r in
                          walk_records(d, lo, hi)), None)
        elif mode == "slots":
            def placement(wbits, mats=mats):
                s = (wbits & 0x3FF) >> 3
                return mats[s] if s < len(mats) else IDENT34
            b.add_stream((r if r is None else r[1:] for r in
                          walk_records(d, lo, hi)), placement)
        elif mode == "instances":
            for m in mats:
                b.add_stream((r if r is None else r[1:] for r in
                              walk_records(d, lo, hi)),
                             lambda wbits, m=m: m)
    sections = [(b.pos, b.col, b.tris, b.bone, b.uv, b.tex)]
    return sections, b.tex_table, b.n_strip_tris


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


def read_uploads_localmem(paths):
    """Synthetic GS local memory: replay the GS texture-upload packets
    (BITBLTBUF reg 0x50 / TRXPOS 0x51 / TRXREG 0x52 + the IMAGE-mode GIF
    payload) of the given disc files into a zeroed 4 MB buffer — exactly
    the writes the engine's streaming loader performs, so the standard
    clut_pair readers resolve TEX0 keys against the result. Only PSMCT32
    transfers are replayed (every observed level/library upload is one;
    anything else is reported and skipped). Returns (localmem, covered):
    `covered` flags each 256-byte GS block a replay wrote, so the caller
    can tell authentic texels from unreplayed zero-fill (validated: the
    chunk27 pack replay is byte-identical to the live office GS dump's
    VRAM over all 1920 blocks). The replay-exactness makes content
    heuristics unnecessary — coverage IS residency here."""
    est = _load("_est_lvl_up", "extract_subtextures.py")
    lm = bytearray(4 * 1024 * 1024)
    covered = bytearray(4 * 1024 * 1024 // 256)
    for p in paths:
        p = Path(p)
        d = p.read_bytes()
        bb = None
        tp = (0, 0)
        for off in range(0, len(d) - 16, 16):
            reg = est._gs_regwrite(d, off)
            if reg == 0x50:                       # BITBLTBUF
                v = int.from_bytes(d[off:off + 8], "little")
                bb = ((v >> 32) & 0x3FFF, (v >> 48) & 0x3F,
                      (v >> 56) & 0x3F)
                tp = (0, 0)
            elif reg == 0x51 and bb is not None:  # TRXPOS (dest x/y)
                v = int.from_bytes(d[off:off + 8], "little")
                tp = ((v >> 32) & 0x7FF, (v >> 48) & 0x7FF)
            elif reg == 0x52 and bb is not None:  # TRXREG -> replay
                v = int.from_bytes(d[off:off + 8], "little")
                w, h = v & 0xFFF, (v >> 32) & 0xFFF
                dbp, dbw, dpsm = bb
                bb = None
                if not (0 < w <= 2048 and 0 < h <= 2048 and w % 64 == 0):
                    continue
                if dpsm != 0x00:
                    print(f"  uploads: {p.name}: non-PSMCT32 transfer "
                          f"(psm {dpsm:#x}) skipped")
                    continue
                payload = est.find_image_payload(d, w * h * 4, off)
                if payload is None:
                    print(f"  uploads: {p.name}: dbp {dbp:#x} {w}x{h} — "
                          "IMAGE payload not found, skipped")
                    continue
                dx, dy = tp
                ppr = max(dbw, 1)      # DBW is in 64-px units = PSMCT32
                base = dbp * 256       # pages-per-row (psmct32_word arg)
                for y in range(h):
                    row = payload + y * w * 4
                    sy = y + dy
                    for x in range(w):
                        a = base + est.psmct32_word(x + dx, sy, ppr) * 4
                        if a + 4 <= len(lm):
                            lm[a:a + 4] = d[row + x * 4:row + x * 4 + 4]
                            covered[a >> 8] = 1
                print(f"  uploads: {p.name}: dbp {dbp:#x} dbw {dbw} "
                      f"{w}x{h} replayed")
    return bytes(lm), covered


def _blocks_covered(covered, blk: int, nbytes: int) -> bool:
    """True when every 256-byte block of [blk*256, blk*256+nbytes) was
    written by a replayed upload."""
    for b in range(blk, blk + (nbytes + 255) // 256):
        if b >= len(covered) or not covered[b]:
            return False
    return True


def build_texture_blob(gsdump: Path | None, tex_table: list[dict],
                       p2s: Path | None = None, uploads=None):
    """Like export_native.build_texture_blob but adds PSMCT32 support.

    VRAM sources (gsdump wins if both given, mirroring export_native):
      gsdump  — PCSX2 1-frame GS dump (.gs): replayed register writes.
      p2s     — PCSX2 save state (.p2s, or a pre-extracted state dir, or a
                bare gs.bin freeze blob): GS local memory at
                len(gs.bin) - 0x400000 - 84 (gs_vram.read_localmem).
      uploads — list of disc files whose GS texture-upload packets are
                replayed into a synthetic VRAM (read_uploads_localmem; the
                only texel source for scenes with no captured state, e.g.
                AREA02 sub-state 0). This path checks each texture's
                texel + CLUT blocks against the replay COVERAGE map and
                falls back to flat grey when uncovered — resolution is
                counted honestly."""
    entries, blob = [], bytearray()
    lm = None
    covered = None
    n_ok = n_flat = 0
    if uploads:
        lm, covered = read_uploads_localmem(uploads)
    elif gsdump is not None:
        pg = _load("_parse_gsdump_lvl", "parse_gsdump.py")
        state_data, _r, _p, _s, _c = pg.parse(gsdump, quiet=True)
        lm = pg.dump_vram(state_data)
    elif p2s is not None:
        gv = _load("_gs_vram_lvl", "gs_vram.py")
        if p2s.is_dir():                       # pre-extracted state dir
            gs_path = p2s / "gs.bin"
        elif p2s.suffix.lower() == ".p2s":     # save state: pull gs.bin
            import tempfile
            pps = _load("_parse_pcsx2_state_lvl", "parse_pcsx2_state.py")
            tmp = Path(tempfile.mkdtemp(prefix="emdl_p2s_lvl_"))
            gs_path = pps.extract_all(p2s, tmp)["gs.bin"]
        else:                                  # bare gs.bin freeze blob
            gs_path = p2s
        _base, lm = gv.read_localmem(gs_path)
    if lm is not None:
        cp = _load("_clut_pair_lvl", "clut_pair.py")
        from clut import apply_clut
    for f in tex_table:
        w, h = 1 << f["tw"], 1 << f["th"]
        if lm is None:
            entries.append({"w": 1, "h": 1, "off": len(blob)})
            blob += b"\x80\x80\x80\xff"
            continue
        if covered is not None:
            # coverage gate (uploads path): texel base + CLUT block must
            # have been written by a replayed transfer
            clut_bytes = 64 if f["psm"] == 0x14 else 1024
            ok = _blocks_covered(covered, f["tbp0"], 256) and \
                (f["psm"] == 0x00
                 or _blocks_covered(covered, f["cbp"], clut_bytes))
            if not ok:
                entries.append({"w": 1, "h": 1, "off": len(blob)})
                blob += b"\x80\x80\x80\xff"
                n_flat += 1
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
        n_ok += 1
    if covered is not None:
        print(f"textures: {n_ok}/{len(tex_table)} resolved from the "
              f"replayed uploads, {n_flat} flat-grey fallback(s)")
    return entries, bytes(blob)


# ---------------------------------------------------------------------------

def main(argv):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--level", default="extract/chunk06.n1/f03_id43.bin",
                    help="level RENDER-MESH file (64-byte [tex0][st][color]"
                    "[pos] records; the office scene's is "
                    "extract/chunk06.n1/f03_id43.bin)")
    ap.add_argument("--gsdump", help="PCSX2 1-frame GS dump (.gs) of a scene "
                    "inside this level: source of colored texels. Without "
                    "it textures are grey 1x1.")
    ap.add_argument("--p2s", help="PCSX2 save state (.p2s, pre-extracted "
                    "state dir, or bare gs.bin) captured INSIDE this level: "
                    "alternative VRAM texel source (--gsdump wins if both)")
    ap.add_argument("--uploads", help="comma-separated disc files whose GS "
                    "texture-upload packets are replayed into a synthetic "
                    "VRAM (texel source for scenes with NO captured state, "
                    "e.g. AREA02 sub-state 0; wins over --gsdump/--p2s)")
    ap.add_argument("--office0-placed", metavar="DIR",
                    help="export the AREA02 sub-state-0 PLACED OBJECTS "
                    "(model-table fixtures at their placement matrices) "
                    "from this chunk06.n0 leaf dir into --out")
    ap.add_argument("--office0-doors", metavar="DIR",
                    help="carve the sub-state-0 interactive door meshes "
                    "from this chunk06.n0 leaf dir into --out/doors/ and "
                    "write the manifest door lines (--out = the scene dir)")
    ap.add_argument("--spawn", default=None,
                    help="x,y,z[,yaw] player spawn in TRUE world "
                    "coordinates; written to the scene manifest "
                    "(scene.txt) next to --out (see module docstring)")
    ap.add_argument("--bgm", default=None,
                    help="looping level-music cue WAV filename (relative "
                    "to the scene dir); written to the scene manifest")
    ap.add_argument("--door-goto", metavar="DIR",
                    help="annotate DIR/scene.txt door lines with decoded "
                    "transition destinations (goto fields) from the boot "
                    "ELF's dest/spawn tables; see annotate_door_goto")
    ap.add_argument("--sub", type=int, default=None,
                    help="--door-goto: the scene's AREA02 sub-state (0-2)")
    ap.add_argument("--exported", default=None,
                    help="--door-goto: sub=dirname,... map of exported "
                    "sub-states (sibling scene dir names), e.g. "
                    "0=scene_office0,1=scene")
    ap.add_argument("--elf", default="elf/SCUS_971.12.elf",
                    help="--door-goto: the user's local boot ELF (read "
                    "only; never committed)")
    ap.add_argument("--overlay", default="extract/OVERLAY/AREA02.BIN",
                    help="--door-goto: the user's extracted AREA02 overlay")
    ap.add_argument("--synthetic-link", action="store_true",
                    help="--door-goto: ALSO wire the two exported "
                    "sub-states' nearest doors with a FLAGGED synthetic "
                    "goto (no real intra-area pair exists — see the "
                    "door-goto header note)")
    ap.add_argument("--out", default=None)
    args = ap.parse_args(argv)

    if args.door_goto:
        return annotate_door_goto(args)
    if not args.out:
        ap.error("--out is required")
    if args.office0_placed:
        return export_office0_placed(args)
    if args.office0_doors:
        return export_office0_doors(args)

    sections, tex_table, n_tris = load_level_mesh(Path(args.level))
    pos = sections[0][0]
    ntris = len(sections[0][2]) // 3
    xs = [p[0] for p in pos]
    ys = [p[1] for p in pos]
    zs = [p[2] for p in pos]
    print(f"level mesh: {len(pos)} verts, {ntris} tris "
          f"({n_tris} strip kicks), {len(tex_table)} textures")
    print(f"  bbox X[{min(xs):.1f},{max(xs):.1f}] "
          f"Y[{min(ys):.1f},{max(ys):.1f}] Z[{min(zs):.1f},{max(zs):.1f}]")

    tex_entries, tex_blob = build_texture_blob(
        Path(args.gsdump) if args.gsdump else None, tex_table,
        Path(args.p2s) if args.p2s else None,
        [Path(s) for s in args.uploads.split(",")] if args.uploads
        else None)

    frames = [[en.mat_identity()]]   # 1 bone, 1 identity frame
    parents = [-1]
    en.write_emdl(Path(args.out), sections, [], parents, frames, 30.0,
                  tex_entries, tex_blob, flags=1)   # bit 0: vertex-color

    if args.spawn:
        vals = [float(v) for v in args.spawn.split(",")]
        if len(vals) == 3:
            vals.append(0.0)                        # default facing: +Z
        if len(vals) != 4:
            sys.exit("--spawn wants x,y,z or x,y,z,yaw")
        update_manifest(Path(args.out).parent, "spawn",
                        " ".join(f"{v:g}" for v in vals))
    if args.bgm:
        update_manifest(Path(args.out).parent, "bgm", args.bgm)

    # Office level: rewrite the scene.txt enemies block from the AREA02
    # placement tables (see the module docstring, ENEMY LINES). The two
    # office render files share a name; the leaf disambiguates them:
    # chunk06.n1 (sub-state 1, the captured scene — commented toggle) vs
    # chunk06.n0 (sub-state 0, the main floor — ACTIVE lines, capped at
    # the port's EM_ENEMY_MAX=16 by distance from the engine spawn).
    level_path = Path(args.level)
    ov_path = level_path.parent.parent / "OVERLAY" / OFFICE_OVERLAY
    if level_path.name == "f03_id43.bin" and ov_path.exists():
        if level_path.stat().st_size == OFFICE0_F03_SIZE \
                and level_path.parent.name == OFFICE0_DIRNAME:
            emit_enemy_manifest(Path(args.out).parent, ov_path,
                                substate=OFFICE0_SUBSTATE, cap=16,
                                spawn=OFFICE0_SPAWN)
        else:
            emit_enemy_manifest(Path(args.out).parent, ov_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
