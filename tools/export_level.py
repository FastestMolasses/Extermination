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
  # scene.txt pickups block (collectible items from the deferred-spawn
  # registry D_0024D820 + the table's kind-0xB display props; s63):
  .venv/bin/python tools/export_level.py \
      --pickups ../extermination-port/assets/scene --area 2 --sub 1 \
      --overlay extract/OVERLAY/AREA02.BIN
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
    # 2026-06-11 (pickup decode): the kind-0xB box/ammo-stack records are
    # NO LONGER baked into the static level mesh — they are emitted as
    # `pickup ... prop` scene.txt lines (--pickups) and drawn by the
    # port's em_pickup actor module instead (state-carrying placements,
    # same positions; models = the per-area table carves
    # props/box_0b/0c/0d.emdl from export_props.py --crate --crate-id).
    n_pick = sum(len(pickups(i)) for i in (0x0B, 0x0C, 0x0D))
    if n_pick:
        print(f"placements: {n_pick} kind-0xB pickup-prop instance(s) "
              f"left OUT of the bake (em_pickup actors — scene.txt "
              f"pickups block)")
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
# Pickup manifest emission (scene.txt `pickup` lines) — decoded 2026-06-11
# (FINDINGS "ITEM PICKUP SYSTEM FULLY DECODED").
#
# The engine's COLLECTIBLE ITEMS do NOT come from the main placement
# tables (D_0024D7C0): they live in the per-area DEFERRED-SPAWN REGISTRY
#
#   D_0024D820[area] -> per-sub-state pointer -> 0-terminated GROUP list
#   -> 0x2C-byte records walked by func_001B6660 (spawner func_001B6910):
#
#   +0x00 s16  spawn CONDITION (jtbl_0026DEE0; -1 = end of group):
#                0 always; 1 if !taken(puid); 2 if event[sidx] != 0xFF;
#                3 if event[sidx] == 0xFF && !taken; 4 if counter[sidx]
#                != 0 && !taken; 5 if event != 0xFF && counter == 1;
#                6 = event == 0xFF + the D_00810778/param-bit7 pair +
#                !taken.  taken(uid) = bit uid of the per-area TAKEN
#                ARRAY D_00810860 + 32*area (set by func_001B1190 when
#                a collected item frees, tested by func_001B11E0).
#   +0x02 u8   puid (persistence uid; 0 = never persists)  +0x03 u8 sidx
#   +0x04 u16  cls   +0x06 u8 model (take family)  +0x07 u8 ITEM TYPE
#   +0x08 u16  param = MODEL id  (model&0xF == 1: per-area table
#              *(D_0028A59C); else the GLOBAL chunk27 library
#              *(D_0028A56C) via func_001B1020/func_001B0DC0)
#   +0x0A..    uid/kind/link/pos[3]/rot[3] as the 0x28 placement record
#   +0x28 u32  behavior fn — items are func_0015AFA0 / func_0015B030
#              (linked variant) / func_00219550
#
# Emitted manifest line (consumed by the port's em_pickup.c):
#
#   pickup <type> <x> <y> <z> <yaw> <uid> [<model.emdl>] [prop]
#
# <uid> is GLOBALIZED as (area << 8) | puid so the port can keep one
# flat taken-bit set with the engine's per-area semantics; uid 0 =
# no persistence (the engine's own rule). Records whose spawn condition
# is story-gated false at a fresh visit (3/4/5/6) are emitted as
# comment lines; 0/1/2 are active (event flags boot to 0 != 0xFF).
#
# The main placement tables' kind-0xB records (class 0x0004, behavior
# func_001C4820 — the office supply-room ammo-box/crate STACKS) are
# DISPLAY PROPS in the engine: no interactive class flag 0x80, never on
# the use-scan's interactive list, and func_001C4820 has no take path
# (s11/s15/s17's "item pickups" framing is OVERTURNED — see FINDINGS).
# They are emitted as `pickup ... prop` render-only lines (model =
# props/box_<param>.emdl, the per-area model-table carve;
# export_props.py --crate --crate-id) so they render as state-carrying
# actors instead of baked level geometry (table_driven_regions no
# longer bakes them).

DEFER_PTR_ARRAY = 0x0024D820     # D_0024D820: per-area deferred registry
OVERLAY_VADDR = 0x00823500       # runtime overlay arena base
FN_PICKUPS = (0x0015AFA0, 0x0015B030, 0x00219550)
FN_PICKUP_PROP = 0x001C4820      # display-prop behavior (kind-0xB records)
PICKUP_COND = {0: "always", 1: "if not taken", 2: "if event[%d] != 0xFF",
               3: "if event[%d] == 0xFF && !taken",
               4: "if counter[%d] != 0 && !taken",
               5: "if event[%d] != 0xFF && counter == 1",
               6: "if event[%d] == 0xFF (+param bit7 pair) && !taken"}
PICKUP_BLOCK_BEGIN = ("# --- pickups (export_level.py --pickups, "
                      "deferred-spawn registry D_0024D820) ---")
PICKUP_BLOCK_END = "# --- end pickups ---"


class AreaMem:
    """vaddr reader spanning the boot ELF + one loaded area overlay."""

    def __init__(self, elf: BootElf, ov_path: Path):
        self.elf = elf
        self.ov = ov_path.read_bytes()

    def read(self, vaddr: int, n: int) -> bytes:
        if vaddr >= OVERLAY_VADDR:
            o = vaddr - OVERLAY_VADDR
            if o + n > len(self.ov):
                raise ValueError(f"vaddr {vaddr:#x} beyond overlay")
            return self.ov[o:o + n]
        return self.elf.read(vaddr, n)

    def u32(self, vaddr: int) -> int:
        return struct.unpack("<I", self.read(vaddr, 4))[0]


def defer_records(mem: AreaMem, area: int, sub: int) -> list[dict]:
    """All deferred-spawn records of (area, sub), parsed (see the block
    comment above for the 0x2C layout)."""
    reg = mem.elf.u32(DEFER_PTR_ARRAY + 4 * area)
    if not reg:
        return []
    sub_ptr = mem.u32(reg + 4 * sub)
    if not sub_ptr:
        return []
    out = []
    li = sub_ptr
    while True:
        grp = mem.u32(li)
        li += 4
        if not grp:
            break
        r = grp
        while True:
            cond, = struct.unpack("<h", mem.read(r, 2))
            if cond == -1:
                break
            raw = mem.read(r, 0x2C)
            puid, sidx = raw[2], raw[3]
            cls, = struct.unpack_from("<H", raw, 4)
            model, itype = raw[6], raw[7]
            param, uid, kind, link = struct.unpack_from("<4H", raw, 8)
            pos = struct.unpack_from("<3f", raw, 0x10)
            rot = struct.unpack_from("<3f", raw, 0x1C)
            fn, = struct.unpack_from("<I", raw, 0x28)
            out.append(dict(vaddr=r, cond=cond, puid=puid, sidx=sidx,
                            cls=cls, model=model, type=itype, param=param,
                            uid=uid, kind=kind, link=link, pos=pos,
                            rot=rot, fn=fn))
            r += 0x2C
    return out


def emit_pickup_manifest(scene_dir: Path, elf: BootElf, ov_path: Path,
                         area: int, sub: int) -> int:
    """--pickups DIR: rewrite DIR/scene.txt's marker-delimited pickups
    block from the deferred-spawn registry (collectible items) plus the
    main placement table's kind-0xB display props (AREA02 tables only —
    placements.py KNOWN_TABLES)."""
    mem = AreaMem(elf, ov_path)
    recs = defer_records(mem, area, sub)
    items = [r for r in recs if r["fn"] in FN_PICKUPS]

    block = [PICKUP_BLOCK_BEGIN]
    block.append(f"# AREA{area:02d} sub-state {sub}: deferred-spawn "
                 f"registry D_0024D820[{area}] — {len(recs)} record(s), "
                 f"{len(items)} collectible item(s) (fn 0015AFA0/15B030/"
                 f"219550).")
    block.append("# pickup <type> <x> <y> <z> <yaw> <uid> [<model.emdl>] "
                 "[prop] — type names = message")
    block.append("# bank group 3; uid = (area<<8)|puid, the engine "
                 "taken-bit key (D_00810860).")
    n_active = n_gated = 0
    for r in items:
        x, y, z = (f"{v:.6g}" for v in r["pos"])
        yaw = f"{r['rot'][1]:.6g}"
        uid = (area << 8) | r["puid"] if r["puid"] else 0
        if r["model"] & 0xF == 1:
            model = f"props/area_item_{r['param']:02x}.emdl"
            src = "per-area model table"
        else:
            model = f"props/item_{r['param']:02x}.emdl"
            src = "chunk27 library"
        cond = r["cond"]
        cdesc = PICKUP_COND.get(cond, "?")
        if "%d" in cdesc:
            cdesc = cdesc % r["sidx"]
        line = (f"pickup {r['type']:#04x} {x} {y} {z} {yaw} {uid:#06x} "
                f"{model}")
        note = (f"# ^ item type {r['type']:#04x}, model id "
                f"{r['param']:#04x} ({src}), take family {r['model']}, "
                f"spawn cond {cond} ({cdesc})")
        if cond in (0, 1, 2):
            block.append(line)
            block.append(note)
            n_active += 1
        else:
            block.append("# STORY-GATED (cond false at a fresh visit):")
            block.append("#" + line)
            block.append(note)
            n_gated += 1

    # Main-table kind-0xB display props (engine: func_001C4820, class 4
    # WITHOUT the interactive flag — render-only).
    n_props = 0
    pl = sys.modules.get("_placements") or _load("_placements",
                                                 "placements.py")
    tables = pl.KNOWN_TABLES.get(ov_path.name)
    if tables and sub < len(tables):
        ents = pl.parse_table(ov_path.read_bytes(), tables[sub])
        props = [e for e in ents
                 if e.kind == 0xB and e.behavior == FN_PICKUP_PROP]
        if props:
            block.append(f"# {len(props)} kind-0xB DISPLAY PROPS from the "
                         f"placement table @{tables[sub]:#x} (engine: "
                         f"class 4, fn 001C4820 — NOT collectible;")
            block.append("# the per-area model-table box/ammo stacks, no "
                         "longer baked into the level mesh):")
            for e in props:
                x, y, z = (f"{v:.6g}" for v in e.pos)
                yaw = f"{e.rot[1]:.6g}"
                block.append(f"pickup {e.param:#04x} {x} {y} {z} {yaw} "
                             f"0 props/box_{e.param:02x}.emdl prop")
                n_props += 1
    block.append(PICKUP_BLOCK_END)

    mf = scene_dir / "scene.txt"
    lines = mf.read_text().splitlines() if mf.exists() else []
    if PICKUP_BLOCK_BEGIN in lines:
        b = lines.index(PICKUP_BLOCK_BEGIN)
        e = lines.index(PICKUP_BLOCK_END) if PICKUP_BLOCK_END in lines \
            else len(lines) - 1
        lines = lines[:b] + lines[e + 1:]
    while lines and not lines[-1].strip():
        lines.pop()
    lines += block
    mf.write_text("\n".join(lines) + "\n")
    print(f"manifest: {mf}: pickups block — AREA{area:02d} sub {sub}: "
          f"{n_active} active item(s), {n_gated} story-gated, "
          f"{n_props} display prop(s)")
    return 0


# ---------------------------------------------------------------------------
# EXAMINE objects (scene.txt `examine` lines).
#
# Decoded 2026-06-11 (s66, FINDINGS "EXAMINE INTERACTION DECODED"):
# examine-able objects are MAIN PLACEMENT-TABLE records with the
# interactive class flag 0x80 whose behavior fn lives in the AREA
# OVERLAY. On the use scan's CROSS arm (+0x0B = 4) the behavior pumps
# an overlay-resident SCRIPT: op07 sub2 enter scripted mode, optional
# op00 camera cue / op02 wait, op0C MESSAGE (the mode-2 radio/examine
# machine — bit-31 line word = GLOBAL slot-0x16 bank, else the
# per-AREA line-record chain D_00264DD0[area+1] presented from the
# loaded slot-0x41 text bank), op07 sub4|STOP exit; then RE-ARM.
#
# This registry holds only ADDRESSES + structural facts from the
# decode; every value/coordinate/text is read from the user's own
# extracted files at export time (nothing disc-derived lives here).
#
# Per-entry keys:
#   overlay   the record's AREAxx.BIN (may differ from the scene's
#             --overlay: the snow world mesh spans AREA06 + AREA11)
#   fn        behavior fn (engine vaddr) — locates the table record
#   desc      ("elf"|"ov", vaddr) -> {dist, dy} floats; "ov+anchor"
#             reads {point[3], dist, dy} (the AREA11 archetype-1
#             desc-point — emitted AS the examine position)
#   gline     ("ov", op0C_rec_va) -> GLOBAL line = rec+0x14 low bits
#   chain     (line_area, op_rec_va, word_off, bank_relpath|None) ->
#             AREA-bank chain: start line = u32 rec[word_off], records
#             from D_00264DD0[line_area+1], text from the bank file
#   delay     int | ("ov", vaddr) u32 | ("ovf", vaddr) f32 frames
#   cam       ("ov", vaddr) -> 3 floats (the op00 cue eye) | None
#   cooldown  re-arm cooldown frames (AREA11 +0x2A = 300)
#   notes     provenance/flag comment lines

EXAMINE_BANK_LINES = 0x00264DD0   # D_00264DD0: [0]=global, [area+1]=area
                                  # line-record tables (8-byte records)

EXAMINE_DECODE = {
    (2, 0): [dict(
        overlay="AREA02.BIN", fn=0x824FA0,
        desc=("elf", 0x2758E0),            # the examine desc {20, 10}
        gline=None,
        chain=(2, 0x8276B0, 0x14, "chunk05/f00_id41.bin"),
        delay=("ov", 0x8276B0 + 0x18),     # op0C pre-delay (30)
        cam=("ov", 0x8276F0 + 0x20),       # op00 cue eye
        cooldown=0,
        notes=[
            "the examined office prop (per-area model 0x17, baked in"
            " 03_placed.emdl), placement [40] of the sub-0 table —",
            "script 0x827670: enter(fade) -> message (AREA02 chain,"
            " no voice) -> 2x op00 camera cue -> chase restore.",
            "STALE-BANK QUIRK (decoded s66): AREA 2 ships NO id-0x41"
            " text bank — the engine presents the LAST-LOADED bank;",
            "entered from AREA01 (the port's only route in) that is"
            " the drawbridge bank, exported here. FLAGGED.",
            "second camera record (op00 sub1 @0x827730, rate 180) not"
            " modeled — the port holds the first cue. FLAGGED.",
        ])],
    (1, 0): [dict(
        overlay="AREA01.BIN", fn=0x825350,
        desc=("ov", 0x82A7A0),             # {10, 20}
        gline=None,
        chain=(1, 0x829EA0, 0x08, "chunk05/f00_id41.bin"),
        delay=0, cam=None, cooldown=0,
        notes=[
            "the drawbridge crank (lib model 0x47), placement [0] —"
            " counter-0 script 0x829E60: op15 cutscene compound,",
            "AREA01 bank line-0 dialogue chain (voice cues 2/3)."
            " FLAGGED: the op15 player-anim arm and the VOICE.DAT",
            "streams are not ported; the counter-0x80/0x81 scripts"
            " (lines 0x0A bridge-lowering cutscene / 0x38) and the",
            "second crank record [2] (script 0x82A7B0, line 0x48) are"
            " story-state machines — not emitted.",
        ])],
    (6, 0): [dict(
        overlay="AREA06.BIN", fn=0x824340,
        desc=("elf", 0x275940),            # {10, 10}
        gline=None,
        chain=(6, 0x827100, 0x14, "chunk10/f00_id41.bin"),
        delay=("ovf", 0x8270C0 + 0x0C),    # op02 WAIT 30.0
        cam=("ov", 0x827080 + 0x20),
        cooldown=0,
        notes=[
            "the AREA06 wall switch (placement [6], model 2 param 7)"
            " — message script 0x827040: enter(fade) -> camera cue ->",
            "wait 30 -> AREA06 bank line 0 (voice cue 40, dur 208) ->"
            " exit. FLAGGED: in the engine this message is the",
            "POST-THROW reminder (D_00810845 bit 0x20 set); the"
            " initial state runs the throw cutscene 0x826D40 (player",
            "anim 0x29 + fades + native 0x8242C0), which is not"
            " ported. Voice stream omitted.",
        ]), dict(
        overlay="AREA11.BIN", fn=0x827B10,
        desc=("ov+anchor", 0x82AB10),      # {(222,230,250.4), 5, 20}
        gline=("ov", 0x82AA90),            # op0C line 0x8000001A
        chain=None, delay=0, cam=None,
        cooldown=300,                      # the +0x2A refusal cooldown
        notes=[
            "the AREA11 switch (placement [19], flags2 7 = unlock bit"
            " 7 of D_00810841[11]) — UNPOWERED REFUSAL script",
            "0x82A990: walk-to + face + chase cue (op0D sub5) +"
            " GLOBAL line 0x1A + 300-frame cooldown. The powered",
            "script 0x82A750 (anim 0x47 throw) is gated on the unlock"
            " bit — 0 at new game; not emitted. The op01 walk-to and",
            "op04 face are FLAGGED-omitted in the port (input pause"
            " only). Emitted into the snow scene: the AREA11 region",
            "shares the exported snow world mesh (the scene spawn"
            " sits 50 u from this switch).",
        ])],
}

EXAMINE_BLOCK_BEGIN = ("# --- examine (export_level.py --examine, "
                       "use-scan examine objects) ---")
EXAMINE_BLOCK_END = "# --- end examine ---"


def _examine_chain(elf: BootElf, extract_root: Path, line_area: int,
                   start_line: int, bank_rel):
    """The AREA-bank message chain: 8-byte records {u16 dur, s16 cue,
    u8 flag, u8 wait} from D_00264DD0[line_area+1], bank line text from
    the extracted slot-0x41 bank (export_ui.parse_outer). Returns
    ([(dur, gap, text)], [voice cues]) — empty-bank-line records fold
    into the previous record's gap (the engine's blank odd lines)."""
    tab = elf.u32(EXAMINE_BANK_LINES + 4 * (line_area + 1))
    if not tab:
        sys.exit(f"area {line_area} has no line-record table")
    lines = None
    if bank_rel:
        bank = extract_root / bank_rel
        if bank.exists():
            ui = sys.modules.get("_export_ui_lvl") or _load(
                "_export_ui_lvl", "export_ui.py")
            lines, _ = ui.parse_outer(bank.read_bytes(), 0, str(bank))
        else:
            print(f"  WARNING: {bank} missing — durations only")
    out, cues = [], []
    i = start_line
    while True:
        dur, cue, _flag, wait = struct.unpack(
            "<hhBB", elf.read(tab + 8 * i, 6))
        if dur == 0 and wait == 1:
            break                      # the terminal record
        if cue >= 0:
            cues.append(cue)
        text = b""
        if lines is not None and i < len(lines):
            text = lines[i].rstrip(b"\x00")
        if text.strip():
            t = text.decode("latin1").rstrip()
            out.append([dur, 0, t.replace("\n", "\\n")])
        elif out:
            out[-1][1] += dur          # blank line -> previous gap
        i += 1
        if i - start_line > 0x20:
            sys.exit("runaway examine chain (no terminal record)")
    return out, cues


def emit_examine_manifest(scene_dir: Path, elf: BootElf, ov_path: Path,
                          area: int, sub: int) -> int:
    """--examine DIR: rewrite DIR/scene.txt's marker-delimited examine
    block from the EXAMINE_DECODE registry (engine values read from the
    user's own ELF/overlay/extract files)."""
    entries = EXAMINE_DECODE.get((area, sub))
    extract_root = ov_path.parent.parent
    block = [EXAMINE_BLOCK_BEGIN]
    block.append("# examine <x> <y> <z> <yaw> <dist> <dy> [gline 0xNN]"
                 " [delay N] [cooldown N] [cam ex ey ez]")
    block.append("# examinetext <dur> <gap> <text...> — chained "
                 "AREA-bank record of the LAST examine line.")
    n = 0
    for e in (entries or []):
        ovp = ov_path.parent / e["overlay"]
        ov = ovp.read_bytes()

        def ov32(va):
            return struct.unpack_from("<I", ov, va - OVERLAY_VADDR)[0]

        def ovf(va, cnt=1):
            return struct.unpack_from(f"<{cnt}f", ov, va - OVERLAY_VADDR)

        # the placement record (pos/yaw) by behavior fn — engine
        # registry walk: D_0024D7C0[area] -> desc[0] -> 0x28 records
        # (every registry record lives in its area's SUB-0 table)
        ent_area = int(e["overlay"][4:6])
        desc_va = elf.u32(0x0024D7C0 + 4 * ent_area)
        tab_va = (struct.unpack_from(
            "<I", ov, desc_va - OVERLAY_VADDR)[0]
            if desc_va >= OVERLAY_VADDR else elf.u32(desc_va))
        rec = None
        r = tab_va
        while True:
            raw = ov[r - OVERLAY_VADDR:r - OVERLAY_VADDR + 0x28]
            cls, = struct.unpack_from("<H", raw, 0)
            if cls == 0xFF:
                break
            fn, = struct.unpack_from("<I", raw, 0x24)
            if fn == e["fn"]:
                rec = raw
                break
            r += 0x28
        if rec is None:
            print(f"  WARNING: fn {e['fn']:#x} not in {e['overlay']} "
                  f"sub-0 table — skipped")
            continue
        pos = struct.unpack_from("<3f", rec, 0x0C)
        yaw = struct.unpack_from("<3f", rec, 0x18)[1]

        # the use-scan desc (+ optional archetype-1 anchor point)
        kind, dva = e["desc"]
        if kind == "elf":
            dist, dy = struct.unpack("<2f", elf.read(dva, 8))
        elif kind == "ov":
            dist, dy = ovf(dva, 2)
        else:                          # "ov+anchor": {point[3], d, dy}
            vals = ovf(dva, 5)
            pos = vals[0:3]
            dist, dy = vals[3], vals[4]

        delay = e["delay"]
        if isinstance(delay, tuple):
            delay = (int(ovf(delay[1])[0]) if delay[0] == "ovf"
                     else ov32(delay[1]))

        line = (f"examine {pos[0]:.6g} {pos[1]:.6g} {pos[2]:.6g} "
                f"{yaw:.6g} {dist:.6g} {dy:.6g}")
        if e["gline"]:
            word = ov32(e["gline"][1] + 0x14)
            assert word >> 31, "gline record is not a GLOBAL line word"
            line += f" gline {word & 0x7FFFFFFF:#x}"
        if delay:
            line += f" delay {delay}"
        if e["cooldown"]:
            line += f" cooldown {e['cooldown']}"
        if e["cam"]:
            cx, cy, cz = ovf(e["cam"][1], 3)
            line += f" cam {cx:.6g} {cy:.6g} {cz:.6g}"
        for note in e["notes"]:
            block.append("# " + note)
        block.append(line)
        if e["chain"]:
            line_area, recva, woff, bank_rel = e["chain"]
            start = ov32(recva + woff)
            assert start >> 31 == 0, "chain start is a GLOBAL word"
            chain, cues = _examine_chain(elf, extract_root, line_area,
                                         start, bank_rel)
            for dur, gap, text in chain:
                block.append(f"examinetext {dur} {gap} {text}")
            if cues:
                block.append(f"# ^ engine voice cue(s) {cues} "
                             "(VOICE.DAT) omitted — FLAGGED")
        n += 1
    if not entries:
        block.append(f"# DECODE VERDICT (s66): no examine records in "
                     f"AREA{area:02d} sub {sub}'s placement table.")
    block.append(EXAMINE_BLOCK_END)

    mf = scene_dir / "scene.txt"
    lines = mf.read_text().splitlines() if mf.exists() else []
    if EXAMINE_BLOCK_BEGIN in lines:
        b = lines.index(EXAMINE_BLOCK_BEGIN)
        eend = lines.index(EXAMINE_BLOCK_END) if EXAMINE_BLOCK_END in \
            lines else len(lines) - 1
        lines = lines[:b] + lines[eend + 1:]
    while lines and not lines[-1].strip():
        lines.pop()
    lines += block
    mf.write_text("\n".join(lines) + "\n")
    print(f"manifest: {mf}: examine block — AREA{area:02d} sub {sub}: "
          f"{n} examine object(s)")
    return 0


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
AREA01 = 1
DEST_PTR_ARRAY = 0x0024E140      # D_0024E140: per-area dest-table base
SPAWN_DESC_ARRAY = 0x0024D650    # D_0024D650: per-area spawn desc
SPAWN_REC_SIZE = 0x30
GOTO_MARK = "# door-goto:"       # idempotency marker for our comments

# AREA01 authoring data (decoded 2026-06-11 for the scene_drawbridge
# export — FINDINGS "DRAWBRIDGE ROOM"): the area's spawn DESC
# (D_0024D650[1] = 0x275500) is BSS, overlay-filled, but the SUB-0 spawn
# TABLE itself is static boot-ELF .data at 0x24B1A0 (s22 live-verified:
# entry 5 = (39, 0, -225) yaw -pi/2). The dest table is generic
# (D_0024E140[1] = 0x24DFA0, 8 records); door 4's room-move record
# {09 08} cross-checks spawn entries 9/8 = (143, -609.7)/(115.5, -609.7)
# flanking the door at (128.6, -610). The PLACEMENT tables live in
# OVERLAY/AREA01.BIN: two tables serve the area's 7 sub-states —
# 0x82BD50 (54 records; door id 0 is an OVERLAY-BRAIN scripted door
# fn 0x823580 and the generators are link-0 inert = the first-visit
# story beat -> treated as SUB 0, FLAGGED presumption) and 0x82C5F0
# (32 records; plain door id 0, active generators = later sub-states).
AREA01_SUB0_SPAWN_VADDR = 0x24B1A0
AREA01_SPAWN_COUNT = 10          # max entry referenced by the dest table
AREA01_TABLE_A = 0x82BD50        # sub-0 (flagged presumption, see above)
AREA01_TABLE_B = 0x82C5F0


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


def area_dest_table(elf: BootElf, area: int) -> list[bytes]:
    """An area's destination records (door id -> 4 bytes). The table runs
    from D_0024E140[area] to the next area's base, giving the record
    count without a stored length."""
    base = elf.u32(DEST_PTR_ARRAY + 4 * area)
    nxt = min(p for a in range(16)
              if (p := elf.u32(DEST_PTR_ARRAY + 4 * a)) > base)
    return [elf.read(base + 4 * d, 4) for d in range((nxt - base) // 4)]


def area_spawn_tables(elf: BootElf, area: int) -> dict[int, list[tuple]]:
    """An area's per-sub-state spawn tables as {sub: [(x, y, z, yaw),
    ...]}. Record = {f32 pos[3], f32 yaw @+0xC, ...} (s22, byte-verified).
    AREA02's desc is static; AREA01's desc is overlay-filled BSS but the
    sub-0 TABLE is static at AREA01_SUB0_SPAWN_VADDR (other AREA01 subs
    are not reachable offline yet — open item)."""
    if area == AREA01:
        recs = [struct.unpack("<4f", elf.read(
            AREA01_SUB0_SPAWN_VADDR + e * SPAWN_REC_SIZE, 0x10))
            for e in range(AREA01_SPAWN_COUNT)]
        return {0: recs}
    desc = elf.u32(SPAWN_DESC_ARRAY + 4 * area)
    tbls = {}
    ptrs = [elf.u32(desc + 4 * s) for s in range(4)]
    for si, tbl in enumerate(ptrs):
        if not tbl:
            break
        end = min([p for p in ptrs if p > tbl] + [tbl + 7 * SPAWN_REC_SIZE])
        recs = []
        for e in range((end - tbl) // SPAWN_REC_SIZE):
            recs.append(struct.unpack(
                "<4f", elf.read(tbl + e * SPAWN_REC_SIZE, 0x10)))
        tbls[si] = recs
    return tbls


def area_placement_table_vaddr(area: int, sub: int) -> int:
    """The placement-table vaddr serving (area, sub) — AREA02 via the
    placements.py registry, AREA01 via the two decoded tables (sub 0 =
    table A, flagged presumption; see the AREA01 block above)."""
    pl = sys.modules.get("_placements") or _load("_placements",
                                                 "placements.py")
    if area == AREA02:
        return pl.KNOWN_TABLES[OFFICE_OVERLAY][sub]
    if area == AREA01:
        return AREA01_TABLE_A if sub == 0 else AREA01_TABLE_B
    sys.exit(f"no placement-table registry for area {area}")


def _sub_doors(ov_data: bytes, area: int, sub: int):
    """Door placement records of one area sub-state table."""
    pl = sys.modules.get("_placements") or _load("_placements",
                                                 "placements.py")
    vaddr = area_placement_table_vaddr(area, sub)
    return [e for e in pl.parse_table(ov_data, vaddr)
            if e.behavior in FN_DOORS]


VENT_FILE = "doors/vent.emdl"          # flagged synthetic stand-in mesh
VENT_LINE = (f"door {VENT_FILE} 62.5 0 -168.5 3.14159 10 "
             f"goto scene_office0 40 0 -146 -1.5708")


def annotate_door_goto(args) -> int:
    """--door-goto DIR: annotate DIR/scene.txt's `door` lines with the
    decoded transition destination:

        door <file> <x> <y> <z> <yaw> <r> goto <scene-dir> <sx> <sy> <sz> <syaw>

    <scene-dir> is the target scene's SIBLING DIRECTORY NAME (the port
    resolves it against the current scene dir's parent) and the spawn is
    the target (area, sub)'s real spawn-table record (pos + exit yaw).
    A goto is emitted only when the decoded destination is an EXPORTED
    scene (--exported area.sub=dirname,...; a bare sub=dirname key means
    area 2); unexported destinations and intra-area room moves (bit7
    clear — the port re-places in the same scene) keep the plain 6-field
    line. Lines are matched to placement records by position; existing
    goto fields and our marker comments are stripped first (idempotent).

    2026-06-11: multi-area (--area; AREA01 = scene_drawbridge), the
    SHIPPED LINKS ARE THE REAL DEST RECORDS (the old --synthetic-link
    west<->m15 wiring is gone), and --vent appends the office scene's
    flagged synthetic VENT line (see VENT_LINE: the engine reaches
    AREA02 sub 0 through an overlay-SCRIPTED vent crawl in the office
    suite — not a placement object, so no honest door record exists;
    the stand-in is a door line at the suite's north-corridor vent wall
    whose goto lands on office0's real spawn entry 5 = (40, 0, -146),
    beside the engine's own class-0x0B trigger record [0] at
    (43, 3.5, -147))."""
    scene_dir = Path(args.door_goto)
    mf = scene_dir / "scene.txt"
    if not mf.exists():
        sys.exit(f"{mf} not found")
    if args.sub is None:
        sys.exit("--door-goto needs --sub (the scene's sub-state)")
    area = args.area
    exported = {}
    for kv in (args.exported or "").split(","):
        if kv:
            k, _, v = kv.partition("=")
            a, dot, s = k.partition(".")
            exported[(int(a), int(s)) if dot else (AREA02, int(k))] = v
    elf = BootElf(Path(args.elf))
    ov_data = Path(args.overlay).read_bytes()
    dest = area_dest_table(elf, area)
    doors = _sub_doors(ov_data, area, args.sub)
    spawn_cache: dict[int, dict[int, list[tuple]]] = {}

    def spawns_of(a: int) -> dict[int, list[tuple]]:
        if a not in spawn_cache:
            spawn_cache[a] = area_spawn_tables(elf, a)
        return spawn_cache[a]

    out, n_goto = [], 0
    lines = mf.read_text().splitlines()
    lines = [ln for ln in lines if not ln.startswith(GOTO_MARK)
             and VENT_FILE not in ln]

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
        tgt = exported.get((n_area, n_sub))
        tgt_spawns = spawns_of(n_area).get(n_sub) if tgt else None
        if tgt and tgt_spawns and entry < len(tgt_spawns):
            sx, sy, sz, syaw = tgt_spawns[entry]
            out.append(f"{GOTO_MARK} door id {door_id}|0x80 -> AREA"
                       f"{n_area:02d} sub {n_sub} entry {entry} "
                       f"(dest table D_0024E140[{area}], REAL record)")
            out.append(" ".join(f) + f" goto {tgt} {sx:.6g} "
                       f"{sy:.6g} {sz:.6g} {syaw:.6g}")
            n_goto += 1
            continue
        out.append(f"{GOTO_MARK} door id {door_id}|0x80 -> AREA"
                   f"{n_area:02d} sub {n_sub} entry {entry} — target not "
                   f"exported, no goto")
        out.append(" ".join(f))

    if args.vent:
        if area != AREA02 or args.sub != 1:
            sys.exit("--vent is the office (area 2 sub 1) scene's line")
        out.append(f"{GOTO_MARK} VENT — FLAGGED SYNTHETIC STAND-IN: the "
                   f"engine reaches AREA02 sub 0 (the main floor) through "
                   f"a vent crawl SCRIPTED IN THE AREA OVERLAY, not a "
                   f"placement object (the sub-1 table's 14 records are "
                   f"all identified: 2 doors, 7 pickups, 5 fixtures — no "
                   f"vent). This door line is the port's stand-in at the "
                   f"suite's north-corridor wall; the ARRIVAL is real "
                   f"(office0 spawn entry 5, beside the engine's own "
                   f"class-0x0B trigger record [0] at (43, 3.5, -147)).")
        out.append(VENT_LINE)
        vent_dst = scene_dir / VENT_FILE
        vent_src = scene_dir / "doors" / "door_m03.emdl"
        if not vent_dst.exists() and vent_src.exists():
            vent_dst.write_bytes(vent_src.read_bytes())
            print(f"vent: {vent_dst} written (copy of door_m03.emdl — "
                  f"flagged visual stand-in; no vent mesh is decoded)")

    mf.write_text("\n".join(out) + "\n")
    print(f"manifest: {mf}: {n_goto} REAL goto link(s)"
          + (" + 1 flagged synthetic vent line" if args.vent else ""))
    return 0


# ---------------------------------------------------------------------------
# DOOR LOCK BITS (scene.txt `door ... locked` annotation) — decoded
# 2026-06-11/12 from the door brains' sub-state-0 gates:
#
#   hinged family func_001BC350, model 0x15 ("security door"): state 0
#     tests the per-area unlock bitmask `D_00810841[D_00810700] &
#     (1 << door_id)` (door_id = actor +0x34 = placement flags2 & 0x7F;
#     FINDINGS "FIRST INTERACTIVE OBJECTS" / "DOOR SCRIPTS DECODED" s23)
#     — bit CLEAR queues the LOCKED TRY script D_0024DEC0 (camera cut,
#     player try anim 0x46/0x44, door jiggle clip 3/1, rattle 0x3F2, VO).
#   slider brain func_001BB860: only placements whose flags2 byte is in
#     {0x16, 0x17, 0x3E} are lock-gated (FINDINGS s56 "SLIDER DOOR
#     BRAIN"); their locked script D_0024DA40 is camera + VO only.
#   every other door (m03 doubles, plain sliders) always opens.
#
# D_00810841 is BSS — all bits are ZERO at boot, so a lock-gated door is
# LOCKED until a game event (door panel, keycard script) sets its bit.
# The port mirrors the bit with em_door_unlock(). Emitted grammar:
#
#   door <file> <x> <y> <z> <yaw> <r> locked [goto ...]

LOCK_MARK = "# door-locked:"
SLIDER_LOCK_FLAGS2 = {0x16, 0x17, 0x3E}     # func_001BB860 state-0 set
FN_DOOR_HINGED = 0x001BC350                 # m03/m15 double-door brain
FN_DOOR_SLIDER = 0x001BB860                 # m17/m09 slider brain


def annotate_door_locked(args) -> int:
    """--door-locked DIR: annotate DIR/scene.txt's `door` lines with the
    decoded LOCK GATE (the `locked` token, see the block comment above).
    Lines are matched to placement records by position like --door-goto;
    previous `locked` tokens and our marker comments are stripped first
    (idempotent). Doors with no matching record (the flagged synthetic
    vent) are left untouched."""
    scene_dir = Path(args.door_locked)
    mf = scene_dir / "scene.txt"
    if not mf.exists():
        sys.exit(f"{mf} not found")
    if args.sub is None:
        sys.exit("--door-locked needs --sub (the scene's sub-state)")
    ov_data = Path(args.overlay).read_bytes()
    doors = _sub_doors(ov_data, args.area, args.sub)

    out, n_locked = [], 0
    lines = mf.read_text().splitlines()
    lines = [ln for ln in lines if not ln.startswith(LOCK_MARK)]
    for ln in lines:
        f = ln.split()
        if not (f and f[0] == "door" and len(f) >= 7):
            out.append(ln)
            continue
        f = [t for t in f if t != "locked"]    # strip previous token
        x, z = float(f[2]), float(f[4])
        rec = next((e for e in doors
                    if abs(e.pos[0] - x) < 0.5 and abs(e.pos[2] - z) < 0.5),
                   None)
        if rec is None:
            out.append(" ".join(f))
            continue
        gated = ((rec.behavior == FN_DOOR_HINGED and rec.model == 0x15) or
                 (rec.behavior == FN_DOOR_SLIDER and
                  rec.flags2 in SLIDER_LOCK_FLAGS2))
        if not gated:
            out.append(" ".join(f))
            continue
        door_id = rec.flags2 & 0x7F
        out.append(f"{LOCK_MARK} model {rec.model:#04x} door id {door_id} "
                   f"-> D_00810841[area {args.area}] bit {door_id} (BSS, "
                   f"0 at boot = LOCKED until a game event / "
                   f"em_door_unlock); link {rec.link:#06x} -> locked-VO "
                   f"selector {rec.link & 0x3F} (bits 0-5)")
        out.append(" ".join(f[:7] + ["locked"] + f[7:]))
        n_locked += 1
    mf.write_text("\n".join(out) + "\n")
    print(f"manifest: {mf}: {n_locked} lock-gated door(s) annotated")
    return 0


# ---------------------------------------------------------------------------
# CAMERA REGIONS (scene.txt `camregion` lines) — the mode-0 camera
# director's fixed room cameras, decoded 2026-06-11 (FINDINGS "MODE-0
# CAMERA DIRECTOR func_00195130 DECODED").
#
# The director (cut-table mode 0, func_00195130) keeps ALL of its fixed
# cameras in the MAIN ELF: a per-area switch on the area byte D_00810700
# with hardcoded cases for areas 0/4/6/8/0xB/0xD/0xE/0xF/0x11/0x13 only —
# every other area (including AREA01 and AREA02) runs the generic chase
# func_001921D0. A case pins the camera when func_00194D10(cam, player,
# REGION_IDX) fires: player XZ inside the 0x40-byte quad record at
# D_0024A5F0[REGION_IDX] (4 vec4 corners — axis-aligned rects in all
# shipped data; func_001B1EA0 mode-0 point-in-poly) AND |player.y -
# corner0.y| < 4.0. The fixed EYE coordinates are float immediates in the
# director's code (decoded below); the desired TARGET keeps tracking the
# player. Verdict for the exported scenes:
#
#   AREA02 (office, subs 0/1/2)  NO fixed cameras: no director case, and
#                                the AREA02 overlay never writes the
#                                camera struct/pool (0x8101E0/0x8105D0).
#   AREA01 (sub 0, drawbridge)   NO fixed cameras: no director case; the
#                                overlay's only camera touch is the
#                                drawbridge-cutscene target retarget
#                                (func_00102948 copy of object+0xB0 into
#                                D_008105E0 at 0x825C90/0x825D28).
#   AREA06 (snow)                ONE region: D_0024A5F0[2] -> fixed eye
#                                (-367.7, 90.0, -598.9) (.L001952F8,
#                                immediates 0xC3B7D99A / 0x42B40000 /
#                                0xC415B99A; engine approach rate 0.7
#                                u/frame via func_0018C6A0/func_0018C4B0).
#
# The other two table records bind to non-exported areas: [0] -> AREA13
# (entry>=8 dam path, eye (839.8, 198.0, 1217.3)), [1] -> AREA11 (an AIM
# target-height tweak in func_00230000, not a fixed eye). The lone
# `jal 0x823FE0` "overlay hook" is the area-13 path's gate and lands
# MID-FUNCTION in the shipped AREA13.BIN (dead/drifted code) — it is NOT
# a general per-room camera delegate.
#
# Region rects are read from the user's local boot ELF (nothing
# disc-derived is committed; the scene.txt output lives in the port's
# git-ignored assets tree). Port consumption: em_game.c camera_mode_
# dispatch (fixed placement, L1 ignored, R1 aim + INSTANT release snap).

CAMREGION_TABLE_VADDR = 0x0024A5F0      # D_0024A5F0, 0x40-byte records
CAMREGION_BLOCK_BEGIN = ("# --- camera regions (export_level.py, "
                         "func_00195130 director decode) ---")
CAMREGION_BLOCK_END = "# --- end camera regions ---"

# area -> [(region table index, fixed eye (x, y, z) from the director's
# float immediates)]
CAMREGION_BINDINGS = {
    6: [(2, (-367.7, 90.0, -598.9))],
}

# SPAWN-RECORD FIXED CAMERAS — the SECOND fixed-camera mechanism,
# decoded + live-verified 2026-06-11 (FINDINGS "SPAWN-RECORD FIXED
# CAMERAS"; this is the user-observed supply-room corner camera the
# director decode above could not find). Room-ENTRY spawn records
# (D_0024D650[area] -> per-room record tables, 0x30 stride — the same
# records the re-place reads) carry a CAMERA-INIT word at +0x10:
#
#   bit 7 of the low byte   FIXED-camera flag  -> cam+0x05 = 1
#   low 7 bits              camera mode byte   -> cam+0x06
#   word >> 8               index into the fixed-eye vec3 table
#                           D_0024A8D0 (stride 12)
#
# On EVERY room entry (door transition commit / re-place / op 0x0D
# sub 1) func_001B0460 re-initializes the camera from the entry record:
# a flagged record HARD-PLACES the eye at the table spec and PINS it
# while the room is occupied (desired target = player + 15, the normal
# chase target height; L1/auto-orient dead, R1 aim still runs — the
# same user-observed semantics as the director regions); an unflagged
# record re-seats the normal chase. The engine keys this on the ENTRY
# RECORD, not a trigger volume — the port's camregion machinery stands
# in with a rect spanning the room behind the entry's doorway plane
# (derived from the entry-record PAIR: the two records of a door sit
# ~14 u apart across the doorway; the plane is their midpoint), which
# is behavior-identical for an enclosed room.
CAMSPAWN_DIR_VADDR = 0x0024D650         # D_0024D650: area -> room tables
CAMSPAWN_EYE_VADDR = 0x0024A8D0         # D_0024A8D0: fixed-eye vec3[]
CAMSPAWN_REC_SIZE = 0x30
CAMSPAWN_RECT_HALFWIDTH = 40.0          # rect: +-40 u about the entry
CAMSPAWN_RECT_DEPTH = 80.0              # 80 u beyond the doorway plane
# (area: {room index: record count}) — rooms to scan; counts bounded by
# the next room pointer in the user's local ELF (area 2 room tables:
# 0x24B560 / 0x24B6B0 / 0x24B800, 7 records each).
CAMSPAWN_BINDINGS = {
    2: {1: 7},   # AREA02 room 1: entry 3 = the SUPPLY ROOM behind the
                 # office double doors (live-verified: +0x10 = 0x380 ->
                 # eye idx 3; entry 2 = the office side, chase)
}

# Decode-verdict notes for exported areas that define NO fixed cameras.
CAMREGION_NONE = {
    1: ("AREA01 has no func_00195130 case (generic chase only); the "
        "overlay's lone camera touch is the drawbridge-cutscene target "
        "retarget — not a room camera."),
    2: ("AREA02 has no func_00195130 case (generic chase only) and its "
        "overlay never writes the camera struct/pool — but see the "
        "SPAWN-RECORD camera above: room-1 entry 3 pins the supply-room "
        "corner camera."),
}


def camspawn_records(elf: "BootElf", area: int, room: int, count: int):
    """The room's 0x30-byte entry records: (pos, yaw, caminit word)."""
    dirp = struct.unpack("<I", elf.read(CAMSPAWN_DIR_VADDR + area * 4,
                                        4))[0]
    base = struct.unpack("<I", elf.read(dirp + room * 4, 4))[0]
    out = []
    for i in range(count):
        rec = elf.read(base + i * CAMSPAWN_REC_SIZE, CAMSPAWN_REC_SIZE)
        x, y, z, yaw = struct.unpack("<4f", rec[0:16])
        caminit = struct.unpack("<I", rec[0x10:0x14])[0]
        out.append((x, y, z, yaw, caminit))
    return out


def camspawn_lines(elf: "BootElf", area: int, sub=None) -> list:
    """`camregion` lines for the area's FLAGGED entry records (the
    spawn-record fixed-camera mechanism above). `sub` (the scene's
    sub-state == the ROOM index: the captured office scene is room 1,
    office0 is room 0) filters to that room when given."""
    lines = []
    for room, count in CAMSPAWN_BINDINGS.get(area, {}).items():
        if sub is not None and room != sub:
            continue
        recs = camspawn_records(elf, area, room, count)
        for i, (x, y, z, yaw, caminit) in enumerate(recs):
            if not (caminit & 0x80):
                continue
            eye_idx = caminit >> 8
            ex, ey, ez = struct.unpack(
                "<3f", elf.read(CAMSPAWN_EYE_VADDR + eye_idx * 12, 12))
            # Doorway plane: midpoint to the PAIR record (the nearest
            # other entry — the two sides of the same doorway).
            best, pair = None, None
            for j, (px, py, pz, pyaw, pinit) in enumerate(recs):
                if j == i:
                    continue
                d2 = (px - x) ** 2 + (pz - z) ** 2
                if best is None or d2 < best:
                    best, pair = d2, (px, py, pz)
            mx = (x + pair[0]) / 2 if pair else x
            mz = (z + pair[2]) / 2 if pair else z
            # Rect: from the plane, CAMSPAWN_RECT_DEPTH deep along the
            # record's walk-out yaw (facing = (sin, cos)(yaw)), +-
            # CAMSPAWN_RECT_HALFWIDTH lateral. Axis-aligned yaws only
            # (all decoded records are); else fall back to a box about
            # the record.
            sy, cy = math.sin(yaw), math.cos(yaw)
            if abs(sy) > 0.999 or abs(cy) > 0.999:
                sy, cy = round(sy), round(cy)
                x0 = min(mx, mx + sy * CAMSPAWN_RECT_DEPTH) \
                    - abs(cy) * CAMSPAWN_RECT_HALFWIDTH
                x1 = max(mx, mx + sy * CAMSPAWN_RECT_DEPTH) \
                    + abs(cy) * CAMSPAWN_RECT_HALFWIDTH
                z0 = min(mz, mz + cy * CAMSPAWN_RECT_DEPTH) \
                    - abs(sy) * CAMSPAWN_RECT_HALFWIDTH
                z1 = max(mz, mz + cy * CAMSPAWN_RECT_DEPTH) \
                    + abs(sy) * CAMSPAWN_RECT_HALFWIDTH
            else:
                x0, x1 = x - CAMSPAWN_RECT_HALFWIDTH, \
                    x + CAMSPAWN_RECT_HALFWIDTH
                z0, z1 = z - CAMSPAWN_RECT_HALFWIDTH, \
                    z + CAMSPAWN_RECT_HALFWIDTH
            lines.append(f"# SPAWN-RECORD camera: area {area} room "
                         f"{room} entry {i} (+0x10 = {caminit:#x}, "
                         f"mode {caminit & 0x7F}) -> D_0024A8D0"
                         f"[{eye_idx}]; rect = port stand-in for the "
                         f"entry-keyed pin (doorway plane + "
                         f"{CAMSPAWN_RECT_DEPTH:g} u deep).")
            lines.append(f"camregion {x0:g} {z0:g} {x1:g} {z1:g} "
                         f"{y:g} {ex:g} {ey:g} {ez:g}")
    return lines


def emit_camregion_manifest(scene_dir: Path, elf: "BootElf",
                            area: int, sub=None) -> int:
    """--camregions DIR: rewrite DIR/scene.txt's marker-delimited camera-
    region block from the director decode: real `camregion x0 z0 x1 z1
    ygate ex ey ez` lines for areas with a fixed-camera binding, a
    verdict comment for decoded-empty areas."""
    block = [CAMREGION_BLOCK_BEGIN]
    n_real = 0
    for idx, eye in CAMREGION_BINDINGS.get(area, []):
        rec = struct.unpack("<16f",
                            elf.read(CAMREGION_TABLE_VADDR + idx * 0x40,
                                     0x40))
        xs = [rec[c * 4 + 0] for c in range(4)]
        zs = [rec[c * 4 + 2] for c in range(4)]
        block.append(f"# REAL region: D_0024A5F0[{idx}] (mode-0 director "
                     f"area-{area} case), y gate {rec[1]:g} +-4.0;")
        block.append("# fixed EYE = director immediates; the camera "
                     "target keeps tracking the player.")
        block.append(f"camregion {min(xs):g} {min(zs):g} {max(xs):g} "
                     f"{max(zs):g} {rec[1]:g} "
                     f"{eye[0]:g} {eye[1]:g} {eye[2]:g}")
        n_real += 1
    sp_lines = camspawn_lines(elf, area, sub)
    if sp_lines:
        block += sp_lines
        n_real += sum(1 for l in sp_lines if l.startswith("camregion"))
    if not n_real:
        verdict = CAMREGION_NONE.get(
            area, f"area {area} has no func_00195130 case and no "
                  f"D_0024A5F0 binding")
        block.append(f"# DECODE VERDICT (2026-06-11): no fixed-camera "
                     f"regions in this scene.")
        cur = "#"
        for w in verdict.split():
            if len(cur) + 1 + len(w) > 70:
                block.append(cur)
                cur = "#"
            cur += " " + w
        if cur != "#":
            block.append(cur)
    block.append(CAMREGION_BLOCK_END)

    mf = scene_dir / "scene.txt"
    lines = mf.read_text().splitlines() if mf.exists() else []
    if CAMREGION_BLOCK_BEGIN in lines:
        b = lines.index(CAMREGION_BLOCK_BEGIN)
        e = lines.index(CAMREGION_BLOCK_END) \
            if CAMREGION_BLOCK_END in lines else len(lines) - 1
        lines = lines[:b] + lines[e + 1:]
    while lines and not lines[-1].strip():
        lines.pop()
    lines += block
    mf.write_text("\n".join(lines) + "\n")
    print(f"manifest: {mf}: camera-region block — area {area}: "
          f"{n_real} real region line(s)"
          + ("" if n_real else " (decoded verdict: all chase)"))
    return 0


# ---------------------------------------------------------------------------
# CHARACTER LIGHT RIGS — the per-room VU1 light-matrix source data
# (decoded 2026-06-11 s57; FINDINGS "PER-ROOM LIGHT RIGS DECODED").
#
# D_00251C50 = 45 records x 0x78, keyed (area<<8)|sub (D_00810700/701);
# func_001D7B30 linear-searches and FALLS BACK TO ENTRY 0 on a miss
# (func_001D2910(8) active -> key 0xF00). Record layout:
#   +0x00 u32   key
#   +0x04/+0x08 fog near/far, +0x0C/10/14 fog RGB (func_001D8FD0)
#   +0x18 f32   (not read by the light path; fog-family, kept raw)
#   +0x1C f32   -> working set +0xB4 (paired with 0; no reader found
#                 on the normal path — kept raw)
#   3 light slots, 0x18 apart from +0x20: { angle_x, angle_y,
#                 color r, g, b, w } — DIRECTIONS ARE STORED AS ANGLE
#                 PAIRS, not vectors: func_001D8340 builds
#                 M = RotX(ax)*RotZ(az=0)*RotY(ay) (memory-row form,
#                 sceVu0 helpers func_00102B08/A60/BB0) and applies
#                 M^T to the base vector (0,1,0):
#                   dir = (sin ay * sin ax, cos ax, cos ay * sin ax)
#                 colors are on the GS 0..128 modulate scale; w of
#                 slot 0 = the dir weight in the point-light fold.
#   +0x68/6C/70 ambient RGB (working-set row 3; +0x12C = 128.0).
#
# SLOT 0 = the CAMERA FILL (actor flag +0x2 bit 0x20, func_001D8BF0 —
# set once for the player at init, by the NPC spawners, never by the
# flashlight): its angle-pair vector is CAMERA-SPACE and is rotated
# into world by the TRANSPOSED view rotation (D_00810610) every frame;
# flag clear -> slot 0 dir AND color zeroed. Slots 1/2 are static
# world-space directionals. The VU1 kernel (0x23C780) computes
# I_i = max(dot(dir_i, N), 0), rgb = min(amb + sum I_i*col_i, 255),
# vertex color = rgb (GS modulate /128).
#
# DYNAMIC POINT LIGHTS (32 slots, func_001D7FA0/001D7C30/001D8340):
# per-room PLACED LAMP lists below (func_001F6760 key map; 0x28-byte
# records: +0x0 s16 >=0 marker, +0x4 s16 color type, +0xC/10/14 pos,
# +0x24 runtime handle), registered each frame (func_001F68B0, story-
# flag gated) with color/intensity = D_0026EB70[type] * 128
# (func_001D7C30 copy: color qword scaled x128 — the W lane IS the
# intensity slot +0x2C). Actors passing the func_001D8270 gate fold
# them into SLOT 0: k = 0.1*I/max(dist^2,1) (dist UNNORMALIZED, the
# func_00102738 dot of the un-normalized offset);
#   dir0 = normalize(dir0*w0 + sum toLamp*10k)  [+ a +-1.8 deg random
#          flicker rotation per lamp, slot+0x40 — omitted in the port]
#   col0 = col0 + sum lampcol*2k.

LIGHTRIG_VADDR = 0x251C50
LIGHTRIG_COUNT = 45
LIGHTRIG_SIZE = 0x78

# func_001F6760: (area<<8)|sub -> placed-lamp list vaddr (NULL = none).
LAMP_LISTS = {
    0x0000: 0x25CF10, 0x0001: 0x25CF90, 0x0002: 0x25CFE0,
    0x0100: 0x25D030, 0x0200: 0x25D080, 0x0E00: 0x25D1F0,
    0x1100: 0x25D340, 0x1301: 0x25D270,
}
LAMP_LIST_1301_B = 0x25D2C0      # key 0x1301's second list (func_001F6850)
LAMP_COLOR_VADDR = 0x26EB70      # D_0026EB70: vec4 (r,g,b,intensity) /128

# func_001F68B0 story-flag gates (BSS byte, sense) per key — the port
# registers lamps unconditionally; the gate is recorded for honesty.
LAMP_GATES = {
    0x0000: ("D_0081075D", "!= 0xFF"), 0x0001: ("D_0081075E", "== 0xFF"),
    0x0002: ("D_00810784", "== 0xFF"), 0x0100: ("D_0081075E", "!= 0xFF"),
    0x0200: ("D_00810761", "== 0xFF"), 0x0E00: ("D_00810784", "== 0xFF"),
    0x1100: ("D_00810785", "== 0xFF"), 0x1301: ("D_0081079E/78/7B", "mixed"),
}

LIGHTRIG_BLOCK_BEGIN = ("# --- character light rig (export_level.py "
                        "--lightrig, D_00251C50 decode) ---")
LIGHTRIG_BLOCK_END = "# --- end character light rig ---"


def lightrig_angles_to_dir(ax: float, ay: float):
    """func_001D8340's angle-pair -> unit direction (base (0,1,0),
    M^T = RotY^T*RotZ^T(0)*RotX^T applied — see the section comment)."""
    return (math.sin(ay) * math.sin(ax), math.cos(ax),
            math.cos(ay) * math.sin(ax))


def lightrig_read(elf: "BootElf", area: int, sub: int) -> tuple:
    """The rig record for (area, sub) with the engine's entry-0 fallback.
    Returns (index, key_matched, dict)."""
    want = (area << 8) | sub
    idx, rec = 0, elf.read(LIGHTRIG_VADDR, LIGHTRIG_SIZE)
    matched = False
    for i in range(LIGHTRIG_COUNT):
        r = elf.read(LIGHTRIG_VADDR + i * LIGHTRIG_SIZE, LIGHTRIG_SIZE)
        if struct.unpack_from("<I", r, 0)[0] == want:
            idx, rec, matched = i, r, True
            break
    def f(o):
        return struct.unpack_from("<f", rec, o)[0]
    lights = []
    for base in (0x20, 0x38, 0x50):
        lights.append({"ang": (f(base), f(base + 4)),
                       "dir": lightrig_angles_to_dir(f(base), f(base + 4)),
                       "col": (f(base + 8), f(base + 0xC), f(base + 0x10),
                               f(base + 0x14))})
    return idx, matched, {
        "key": struct.unpack_from("<I", rec, 0)[0],
        "fog": (f(4), f(8), f(0xC), f(0x10), f(0x14)),
        "p18": f(0x18), "p1c": f(0x1C),
        "lights": lights,
        "amb": (f(0x68), f(0x6C), f(0x70)),
    }


def lamp_list_read(elf: "BootElf", vaddr: int) -> list:
    """One placed-lamp list: [(pos3, color3*128, intensity)] — the
    registration-time values (func_001F6640 + func_001D7C30's x128)."""
    out = []
    i = 0
    while True:
        rec = elf.read(vaddr + i * 0x28, 0x28)
        if struct.unpack_from("<h", rec, 0)[0] < 0:
            break
        typ = struct.unpack_from("<h", rec, 4)[0]
        pos = struct.unpack_from("<3f", rec, 0xC)
        c = struct.unpack_from("<4f",
                               elf.read(LAMP_COLOR_VADDR + typ * 0x10, 0x10))
        out.append((pos, tuple(x * 128.0 for x in c[:3]), c[3] * 128.0, typ))
        i += 1
    return out


def emit_lightrig_manifest(scene_dir: Path, elf: "BootElf",
                           area: int, sub: int) -> int:
    """--lightrig DIR: rewrite DIR/scene.txt's marker-delimited character
    light-rig block from the (area, sub) rig + placed-lamp decode.
    Port lines (em_game.c scene_manifest_load):
      lightamb r g b                ambient row (0..128 scale)
      lightcam dx dy dz r g b w     slot 0: CAMERA-SPACE fill dir + color
                                    + point-light-fold dir weight
      lightdir dx dy dz r g b       slots 1/2: world-space directionals
      lamp x y z r g b i            placed lamp (color/intensity x128)"""
    key = (area << 8) | sub
    idx, matched, rig = lightrig_read(elf, area, sub)
    block = [LIGHTRIG_BLOCK_BEGIN,
             f"# D_00251C50[{idx}] key 0x{rig['key']:04X}"
             + ("" if matched else
                f" (ENGINE FALLBACK: no entry for key 0x{key:04X} — "
                f"func_001D7B30 returns entry 0)")
             + " — func_001D89D0 normal path.",
             "# Colors are on the engine 0..128 GS-modulate scale; the"
             " kernel computes",
             "# rgb = min(amb + sum max(dot(N, dir_i), 0)*col_i, 255),"
             " shade = tex*rgb/128.",
             f"lightamb {rig['amb'][0]:g} {rig['amb'][1]:g}"
             f" {rig['amb'][2]:g}"]
    l0 = rig["lights"][0]
    block.append("# slot 0 = the CAMERA FILL (player flag +0x2 bit 0x20):"
                 " dir is CAMERA-SPACE")
    block.append("# (x view-right, y view-down, z forward), rotated into"
                 " world by the inverse")
    block.append("# view rotation per frame; w = the dir weight in the"
                 " point-light fold.")
    block.append(f"lightcam {l0['dir'][0]:.4f} {l0['dir'][1]:.4f}"
                 f" {l0['dir'][2]:.4f} {l0['col'][0]:g} {l0['col'][1]:g}"
                 f" {l0['col'][2]:g} {l0['col'][3]:g}")
    for li in (1, 2):
        l = rig["lights"][li]
        block.append(f"lightdir {l['dir'][0]:.4f} {l['dir'][1]:.4f}"
                     f" {l['dir'][2]:.4f} {l['col'][0]:g} {l['col'][1]:g}"
                     f" {l['col'][2]:g}")
    lamps_va = LAMP_LISTS.get(key)
    if lamps_va:
        gate = LAMP_GATES.get(key)
        lamps = lamp_list_read(elf, lamps_va)
        block.append(f"# placed lamps: func_001F6760 key 0x{key:04X} ->"
                     f" list @0x{lamps_va:06X} ({len(lamps)} records);")
        block.append(f"# engine registration is story-flag gated"
                     f" ({gate[0]} {gate[1]}) — the port")
        block.append("# registers them unconditionally (flagged"
                     " stand-in); the engine's per-lamp")
        block.append("# +-1.8deg random flicker rotation (func_001D7C30"
                     " type-1 path) is omitted.")
        for pos, col, inten, typ in lamps:
            block.append(f"lamp {pos[0]:g} {pos[1]:g} {pos[2]:g}"
                         f" {col[0]:g} {col[1]:g} {col[2]:g} {inten:g}"
                         f"  # type {typ}")
    else:
        block.append(f"# placed lamps: none — func_001F6760 has no list"
                     f" for key 0x{key:04X}.")
    block.append(LIGHTRIG_BLOCK_END)

    mf = scene_dir / "scene.txt"
    lines = mf.read_text().splitlines() if mf.exists() else []
    if LIGHTRIG_BLOCK_BEGIN in lines:
        b = lines.index(LIGHTRIG_BLOCK_BEGIN)
        e = lines.index(LIGHTRIG_BLOCK_END) \
            if LIGHTRIG_BLOCK_END in lines else len(lines) - 1
        lines = lines[:b] + lines[e + 1:]
    while lines and not lines[-1].strip():
        lines.pop()
    lines += block
    mf.write_text("\n".join(lines) + "\n")
    n_lamps = len(lamp_list_read(elf, lamps_va)) if lamps_va else 0
    print(f"manifest: {mf}: light-rig block — key 0x{key:04X} -> "
          f"D_00251C50[{idx}]"
          + ("" if matched else " (entry-0 fallback)")
          + f", {n_lamps} placed lamp(s)")
    return 0


def dump_lightrigs(elf: "BootElf") -> int:
    """--lightrig-dump: print all 45 rigs + every placed-lamp list (the
    documentation pass; nothing is written)."""
    for i in range(LIGHTRIG_COUNT):
        r = elf.read(LIGHTRIG_VADDR + i * LIGHTRIG_SIZE, LIGHTRIG_SIZE)
        key = struct.unpack_from("<I", r, 0)[0]
        _, _, rig = lightrig_read(elf, key >> 8, key & 0xFF)
        fog = rig["fog"]
        print(f"[{i:2d}] key 0x{key:04X} fog({fog[0]:7.1f},{fog[1]:7.1f}"
              f" rgb {fog[2]:3.0f},{fog[3]:3.0f},{fog[4]:3.0f})"
              f" p18={rig['p18']:.2f} p1c={rig['p1c']:.3f}"
              f" amb({rig['amb'][0]:3.0f},{rig['amb'][1]:3.0f},"
              f"{rig['amb'][2]:3.0f})")
        for li, l in enumerate(rig["lights"]):
            d, c = l["dir"], l["col"]
            print(f"      L{li}{' (cam)' if li == 0 else '      '[:6]}:"
                  f" dir({d[0]:6.3f},{d[1]:6.3f},{d[2]:6.3f})"
                  f" col({c[0]:3.0f},{c[1]:3.0f},{c[2]:3.0f}"
                  f" w {c[3]:g})")
    print()
    for key, va in sorted(LAMP_LISTS.items()) + [(0x1301, LAMP_LIST_1301_B)]:
        lamps = lamp_list_read(elf, va)
        gate = LAMP_GATES.get(key, ("?", "?"))
        print(f"lamp list key 0x{key:04X} @0x{va:06X}"
              f" (gate {gate[0]} {gate[1]}): {len(lamps)} lamp(s)")
        for pos, col, inten, typ in lamps:
            print(f"    type {typ} pos({pos[0]:7.1f},{pos[1]:7.1f},"
                  f"{pos[2]:7.1f}) col({col[0]:6.1f},{col[1]:6.1f},"
                  f"{col[2]:6.1f}) I={inten:.1f}")
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


# ---------------------------------------------------------------------------
# AREA01 SUB-STATE 0 — the DRAWBRIDGE ROOM area (chunk leaf chunk05.n0).
#
# Leaf identity: the soundmap's area_scene_map pins (area 1, sub 0) ->
# chunk05.n0 — and (1, 7) -> chunk15, the intro SNOW level: the snow
# level is an AREA01 sub-state, so this leaf is "the first room after
# the first snow level" (the user-remembered drawbridge room). The
# room itself sits around X[-35, 65] Z[-150, -250]; the leaf's world
# files cover the whole area (a deep shaft complex down to z -1297).
#
# Geometry survey (2026-06-11, walk_records over every file):
#   f02_id43.bin  main static world        f03_id42.bin  world zone
#   f05_id4a.bin  world zone               f09_id52.bin  world zone
#   f10_id47.bin  world zone               f01_id45.bin  world zone
#   f04_id46.bin  world zone               f06_id4d.bin  world zone
#   f07_id4c.bin  world zone               f08_id99.bin  world zone
#   f11_id4b.bin  world zone [0, 0x56000) — the per-area MODEL TABLE
#                 lives at file +0x56000 (concat 0x3F2000, 22 entries,
#                 func_001C6120 directory; entries span into f12..f14),
#                 so the tail is OBJECT-SPACE and must not bake as world
#   f00_id44.bin  collision world (head) + static render TAIL
#                 [0x1523D0, end) — same two-section shape as the office
#   f12_id4e.bin / f14_id88.bin  object-space blobs (model-table entries)
#   f13_id72.bin  no records (table data)
#
# TEXTURES: no captured save state exists for this area — the s28
# --uploads replay path: the leaf's own GS upload packets
# (f00_id44.bin, blocks 0x2A00../0x3180..) + the global library pack
# (chunk27/f00_id35.bin).
#
# Doors/placements: OVERLAY/AREA01.BIN table A @0x82BD50 (see the
# AREA01 block above). Spawn = the arrival entry 5 (39, 0, -225) yaw
# -pi/2 (the office west door's dest record).

DRAWBRIDGE_DIRNAME = "chunk05.n0"
DRAWBRIDGE_TABLE_OFF = 0x3F2000        # model table (concat; f11+0x56000)
DRAWBRIDGE_SPAWN = (39.0, 0.0, -225.0, -1.5708)   # AREA01 sub-0 entry 5
DRAWBRIDGE_ZONES = [
    # (file, lo, hi-or-None)
    ("f02_id43.bin", 0x000010, None),
    ("f03_id42.bin", 0x000010, None),
    ("f05_id4a.bin", 0x000010, None),
    ("f09_id52.bin", 0x000010, None),
    ("f10_id47.bin", 0x000010, None),
    ("f11_id4b.bin", 0x000010, 0x56000),
    ("f01_id45.bin", 0x000010, None),
    ("f04_id46.bin", 0x000010, None),
    ("f06_id4d.bin", 0x000010, None),
    ("f07_id4c.bin", 0x000010, None),
    ("f08_id99.bin", 0x000010, None),
    ("f00_id44.bin", 0x1523D0, None),
]


def drawbridge_uploads(dirp: Path) -> list:
    ups = [dirp / "f00_id44.bin",
           dirp.parent / "chunk27" / "f00_id35.bin"]
    return [p for p in ups if p.exists()]


def export_drawbridge(args) -> int:
    """--drawbridge DIR: export the AREA01 sub-state-0 leaf (chunk05.n0)
    as a complete scene into --out (the SCENE DIRECTORY): one EMDL per
    world zone file (NN_<file>.emdl), the manifest spawn + doorsfx +
    enemies block. Doors come from export_props.py --doors-drawbridge;
    collision from export_collision.py (both documented in PROGRESS)."""
    dirp = Path(args.drawbridge)
    scene_dir = Path(args.out)
    scene_dir.mkdir(parents=True, exist_ok=True)
    ups = drawbridge_uploads(dirp)
    if not ups:
        sys.exit(f"{dirp}: no upload-packet source files found")

    for zi, (name, lo, hi) in enumerate(DRAWBRIDGE_ZONES):
        p = dirp / name
        if not p.exists():
            sys.exit(f"{p} missing")
        d = p.read_bytes()
        b = MeshBuilder()
        b.add_stream((r if r is None else r[1:] for r in
                      walk_records(d, lo, hi)), None)
        if not b.pos:
            print(f"  zone {name}: no records — skipped")
            continue
        sections = [(b.pos, b.col, b.tris, b.bone, b.uv, b.tex)]
        tex_entries, tex_blob = build_texture_blob(None, b.tex_table,
                                                   None, ups)
        out = scene_dir / f"{zi:02d}_{name.split('.')[0]}.emdl"
        en.write_emdl(out, sections, [], [-1], [[en.mat_identity()]],
                      30.0, tex_entries, tex_blob, flags=1)

    update_manifest(scene_dir, "spawn",
                    " ".join(f"{v:g}" for v in DRAWBRIDGE_SPAWN))
    # Door sound pair: every m03/m15 door link in table A is 0x02xx ->
    # D_0024DB80 selector 2, the same pair the office uses (the table is
    # BSS — ids are the engine rule; the per-scene sfx registry maps
    # them to this area's bank or stays silent).
    update_manifest(scene_dir, "doorsfx", "0x3FD 0x3FE")

    # Enemies block (AREA01 table A; see the AREA01 constants block —
    # sub-0 presumption flagged there).
    pl = sys.modules.get("_placements") or _load("_placements",
                                                 "placements.py")
    ov_path = dirp.parent / "OVERLAY" / "AREA01.BIN"
    if ov_path.exists():
        ents = pl.parse_table(ov_path.read_bytes(), AREA01_TABLE_A)
        cen = _placement_census(ents)
        block = [ENEMY_BLOCK_BEGIN]
        block.append(f"# scene table = AREA01 table A @{AREA01_TABLE_A:#x} "
                     f"(SUB-0 PRESUMPTION, flagged: the overlay-brain "
                     f"door id 0 + link-0 inert generators read as the "
                     f"first-visit story beat), {len(ents)} records:")
        block.append(f"#   {cen['crawler']} crawlers (fn 0x001551B0, "
                     f"param 0x0004 — a DIFFERENT disguise model than the "
                     f"office cardboard box; the port's crate enemy mesh "
                     f"is a flagged visual stand-in), "
                     f"{cen['generator']} generators (link 0 = inert), "
                     f"{cen['door']} doors, {cen['pickup']} pickups, "
                     f"{cen['prop']} fixtures/props, "
                     f"{cen['deferred']} deferred(0x0B)")
        block.append("# NOTE record [12] is an OVERLAY-BRAIN scripted "
                     "door (fn 0x823580, door id 0) at (-35.5, -35, "
                     "-1276.5) — likely the drawbridge cutscene; not a "
                     "portable door record, omitted from the door lines.")
        block += _enemy_lines(ents)
        block.append(ENEMY_BLOCK_END)
        mf = scene_dir / "scene.txt"
        lines = mf.read_text().splitlines() if mf.exists() else []
        if ENEMY_BLOCK_BEGIN in lines:
            bidx = lines.index(ENEMY_BLOCK_BEGIN)
            eidx = lines.index(ENEMY_BLOCK_END) if ENEMY_BLOCK_END in lines \
                else len(lines) - 1
            lines = lines[:bidx] + lines[eidx + 1:]
        while lines and not lines[-1].strip():
            lines.pop()
        lines += block
        mf.write_text("\n".join(lines) + "\n")
        print(f"manifest: {mf}: enemies block — {cen['crawler']} crawler "
              f"line(s), {cen['generator']} generator line(s)")
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
    ap.add_argument("--drawbridge", metavar="DIR",
                    help="export the AREA01 sub-state-0 leaf (chunk05.n0, "
                    "the drawbridge room area) as a complete scene into "
                    "--out: zone EMDLs + manifest spawn/doorsfx/enemies "
                    "(doors via export_props.py --doors-drawbridge, "
                    "collision via export_collision.py)")
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
    ap.add_argument("--pickups", metavar="DIR",
                    help="rewrite DIR/scene.txt's marker-delimited "
                    "pickups block from the deferred-spawn registry "
                    "D_0024D820 (+ the placement table's kind-0xB "
                    "display props); needs --area/--sub/--overlay")
    ap.add_argument("--examine", metavar="DIR",
                    help="rewrite DIR/scene.txt's marker-delimited "
                    "examine block from the EXAMINE_DECODE registry "
                    "(s66 use-scan examine objects: positions/descs/"
                    "lines read from the user's own ELF + overlay + "
                    "extracted text banks); needs --area/--sub/"
                    "--overlay")
    ap.add_argument("--camregions", metavar="DIR",
                    help="rewrite DIR/scene.txt's camera-region block "
                    "from the mode-0 director decode (use with --area; "
                    "real `camregion` lines where the area has a "
                    "D_0024A5F0 binding, the decoded all-chase verdict "
                    "comment otherwise)")
    ap.add_argument("--lightrig", metavar="DIR",
                    help="rewrite DIR/scene.txt's character light-rig "
                    "block from the D_00251C50 per-room rig + placed-"
                    "lamp decode (use with --area/--sub; see "
                    "emit_lightrig_manifest)")
    ap.add_argument("--lightrig-dump", action="store_true",
                    help="print all 45 D_00251C50 rigs + every placed-"
                    "lamp list (documentation; writes nothing)")
    ap.add_argument("--area", type=int, default=AREA02,
                    help="--door-goto: the scene's AREA (default 2; "
                    "scene_drawbridge = 1)")
    ap.add_argument("--sub", type=int, default=None,
                    help="--door-goto: the scene's sub-state")
    ap.add_argument("--exported", default=None,
                    help="--door-goto: area.sub=dirname,... map of "
                    "exported scenes (sibling scene dir names), e.g. "
                    "1.0=scene_drawbridge,2.0=scene_office0,2.1=scene; "
                    "a bare sub=dirname key means area 2")
    ap.add_argument("--elf", default="elf/SCUS_971.12.elf",
                    help="--door-goto: the user's local boot ELF (read "
                    "only; never committed)")
    ap.add_argument("--overlay", default="extract/OVERLAY/AREA02.BIN",
                    help="--door-goto: the user's extracted area overlay "
                    "(pass AREA01.BIN with --area 1)")
    ap.add_argument("--door-locked", metavar="DIR",
                    help="annotate DIR/scene.txt's door lines with the "
                    "decoded LOCK GATE (`locked` token: model-0x15 "
                    "security doors + flags2-{16,17,3E} sliders vs the "
                    "BSS unlock bitmask D_00810841; use with --area/"
                    "--sub/--overlay; see annotate_door_locked)")
    ap.add_argument("--vent", action="store_true",
                    help="--door-goto (office scene only): append the "
                    "FLAGGED synthetic VENT door line -> scene_office0 "
                    "(the engine's vent crawl is overlay-scripted, not a "
                    "placement object; arrival = the real office0 spawn "
                    "entry 5). Replaces the removed --synthetic-link.")
    ap.add_argument("--out", default=None)
    args = ap.parse_args(argv)

    if args.door_goto:
        return annotate_door_goto(args)
    if args.door_locked:
        return annotate_door_locked(args)
    if args.pickups:
        if args.sub is None:
            ap.error("--pickups needs --sub (the scene's sub-state)")
        return emit_pickup_manifest(Path(args.pickups),
                                    BootElf(Path(args.elf)),
                                    Path(args.overlay), args.area,
                                    args.sub)
    if args.examine:
        if args.sub is None:
            ap.error("--examine needs --sub (the scene's sub-state)")
        return emit_examine_manifest(Path(args.examine),
                                     BootElf(Path(args.elf)),
                                     Path(args.overlay), args.area,
                                     args.sub)
    if args.camregions:
        return emit_camregion_manifest(Path(args.camregions),
                                       BootElf(Path(args.elf)), args.area,
                                       args.sub)
    if args.lightrig_dump:
        return dump_lightrigs(BootElf(Path(args.elf)))
    if args.lightrig:
        if args.sub is None:
            ap.error("--lightrig needs --sub (the scene's sub-state; "
                     "the rig key is (area<<8)|sub)")
        return emit_lightrig_manifest(Path(args.lightrig),
                                      BootElf(Path(args.elf)), args.area,
                                      args.sub)
    if not args.out:
        ap.error("--out is required")
    if args.office0_placed:
        return export_office0_placed(args)
    if args.office0_doors:
        return export_office0_doors(args)
    if args.drawbridge:
        return export_drawbridge(args)

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
