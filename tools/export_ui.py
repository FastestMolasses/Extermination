#!/usr/bin/env python3
"""export_ui.py - extract the status-screen DECOR textures to assets/ui.emui,
and (--page N) the per-page SUB-SCREEN texture sets to assets/ui_pageN.emui.

Original tooling for the Extermination decompilation project. Ingests only
locally produced data: a GS-VRAM dump from the user's own save state (hub
mode), or the user's own extract/ chunk files (page mode); nothing
disc-derived is redistributed. Output lands in git-ignored asset directories.

WHERE THE UI TEXTURES LIVE (docs/FINDINGS.md "STATUS SCREEN UI TEXTURES")
-------------------------------------------------------------------------
The status-screen hub's textured decor sprites are PSMT4 textures RESIDENT
in GS VRAM (TBW 8 = one 512-texel-wide UI buffer), each with a 16-entry
CSM1 CT32 CLUT, stored VERTICALLY FLIPPED (the engine's sprite UVs flip V
on draw). They are present in an ordinary gameplay save state (uploaded by
the boot-time asset chunk path, not re-streamed per frame), so a plain
in-game state dump is a sufficient source. TEX0 tokens come from the hub
master drawer func_00209DF0's inline constants:

  sprite      TEX0 (lo_hi)          TBP    CBP    size     canvas pos
  title       0x9D421E50_200453A5   0x1E50 0x229D 128x64   (16,0)    "MAIN"
  legend      0xDD421D40_20045505   0x1D40 0x22A8 128x128  (0,320)   pad legend
  icon TL     0x55422186_20045EC5   0x2186 0x22F6 32x32    (11,304)
  icon TR     0x554221F0_20045EC5   0x21F0 0x22F6 32x32    (484,304)
  icon BL     0x55422192_20045EC5   0x2192 0x22F6 32x32    (417,10)
  icon BR     0x554221F4_20045EC5   0x21F4 0x22F6 32x32    (417,406)
  bullet      0x55422196_20045185   0x2196 0x228C 32x32    (16,262) dw/dh 24
              (func_00209860's SPR4 ammo icon — GS-scaled to 24x24 on draw)
  backdrop    0x9D421E40_20045EE5   0x1E40 0x22F7 128x64   BACKDROP record
              (the ANIMATED UI BACKGROUND tile — see below)

("legend" is the 128x128 quad the session-25 audit provisionally called
the "portrait" - the texture is the button legend: triangle EXIT, circle
BACK, cross OK. The audit's "help panel" is a FLAT translucent rect, not a
texture; it needs no asset.)

THE ANIMATED UI BACKGROUND (FINDINGS.md "STATUS SCREEN BACKGROUND")
-------------------------------------------------------------------
Every status/UI screen draws an ANIMATED FULL-SCREEN BACKGROUND before its
panels: the universal drawer func_0020A7A0(tex0_token) composites THREE
layers of ONE 128x64 PSMT4 tile (per-screen token, passed by each caller)
over the black UI-camera frame: two full-screen 256x128-px tilings (2x
upscale; one scrolling left 0.5 px/frame, one scrolling up 1 px/frame,
sin-pulsed alpha) plus a periodic full-screen "zoom burst". The hub's tile
is boot-resident (TBP 0x1E40/CBP 0x22F7, dumped from gs.bin like the decor
set); each pager page passes a tile from its own chunk (the records below
previously exported sheet-only as title_hi/pan_b/title_b/t2200 - now
flagged as bg_anim BACKDROP records).

BACKDROP records use the x = y = -32767 sentinel (vs -32768 = plain
sheet-only): the port's em_hud finds the sentinel, never draws it as a
positioned sprite, and runs the animated background pass with it; dw/dh
carry the engine's 2x tile draw size (256x128). Assets exported before
this sentinel existed simply have no backdrop record - the port keeps its
translucent scene-dim fallback.

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
              s16 x, y               (draw position on the 512x448 status
                                      canvas - the audited anchors above)
              u16 dw, dh             (draw size on the canvas; 1:1 = w,h
                                      for all but the bullet icon, which
                                      GS-scales its 32x32 texture to 24x24)
  ...  sheet_w*sheet_h*4  RGBA8 sheet, rows top-down, SCREEN-oriented
              (the VRAM v-flip is undone at export)

The port draws every record as one textured overlay sprite at (x,y,dw,dh)
sampling (u,v)-(u+w,v+h), white modulate - so the layout data travels with
the asset and em_hud stays free of per-sprite position tables.

PAGE MODE (--page N / --page all) - the pager sub-screens (session 31)
----------------------------------------------------------------------
The hub's pager diamond launches disc-loaded sub-screens; each page is a
DATA.DAT chunk (INDEX.IDX sector = chunk index, FINDINGS.md "STATUS
SCREEN UI TEXTURES" loader decode) whose one transient GS packet is the
page's texture upload. Page identities and chunk ids (FINDINGS.md
"STATUS SUB-PAGES", session 31; hover->page remap in func_0020CDC0):

  page 0  ITEM SCREEN      chunk 0x1F  extract/chunk31  view func_0020EE50
  page 1  MAP SCREEN       chunk 0x1E  extract/chunk30  view func_0020F950
  page 2  SPR4 SCREEN      chunk 0x2C  extract/chunk44  view func_00211970
  page 3  DATABASE SCREEN  chunk 0x24  extract/chunk36  view func_00214020
  page 4  passcode keypad  chunk 0x25  extract/chunk37  view func_002072C0
          (textures are data-driven - NO statically reachable TEX0
          tokens; --page 4 reports that and writes nothing)

Recipe: replay the chunk's GS upload packets into synthetic VRAM
(export_level.read_uploads_localmem - every page chunk uploads one
PSMCT32 block at dbp 0x1D00, dbw 4), then decode the TEX0 tokens lifted
statically from each page's draw-function closure (PSMT4 + 16-entry
CLUT or PSMT8 + 256-entry CLUT, both CSM1 CT32; textures stored
v-flipped like the hub set). The token tables below are audited
constants from the local disassembly (the engine inlines raw TEX0
values via lui/ori//dsll32/or); the sourcing function is noted per
record. Canvas anchors: (8,0) titles are from the page-1 draw call
(GS 0x7080/0x7900); background-tile and legend anchors are composition-
verified guesses (STITCH check) and flagged ASSUMED; records with NO
known anchor are packed sheet-only with x = y = -32768 (the port skips
drawing them; they stay available in the sheet).

MESSAGES MODE (--messages) - the status-screen message bank (session 40)
------------------------------------------------------------------------
The status screen's text (hub help lines, ITEM categories, SPR4
components, action prompts, map area names, item names/descriptions)
lives in ONE bank file: boot chunk asset slot 2 = extract/chunk00/
f02_id02.bin (runtime pointer D_0028A498; FINDINGS.md "STATUS
SUB-PAGES" -> "The message bank"). Resolution path in the engine:
func_001FCB90(x, y, group, line) -> func_001FE070 walks the group's
string blob. On-disc layout (little-endian, offsets are relative to the
named header unless stated):

  bank header   {u32 dir_base (0xA0), u32 ngroups (9), u32 total_size,
                 u32 dir_off (0x10)}
  group dir     at dir_off: ngroups x 16-byte entries
                {u32 group_off (rel. dir_base), u32 group_off>>4,
                 u32 size, u32 padded_size}
  group OUTER   at bank + dir_base + group_off:
                {u32 text_off, u32 line_count, u32 records_size,
                 u32 0x10}, then line_count x 16-byte MARKUP entries
                {u32 records_off (rel. OUTER), u32 ?, u32 ?,
                 u32 nrecords<<4} (func_001FE4B0/func_001FE4D0 -
                 inline style/color control records, 16 B each,
                 between the entry table and the TEXT blob; groups
                 3/4 use them, the rest are zero)
  group TEXT    at OUTER + text_off + records_size (func_001FE460):
                {u32 strbase (= 0x10 + 16*count), u32 line_count,
                 u32 str_bytes, u32 1}, then line_count x 16-byte line
                entries {u32 off, u32 off, u32 len, u32 len+1}
                (func_001FE480: string N = TEXT + strbase + off), then
                the NUL-terminated ASCII strings ('\n' = in-entry line
                break; entries often end with a trailing '\n')

Since 2026-06-11 ("RADIO-MESSAGE MACHINE DECODED") the mode also
appends the GLOBAL examine/radio text bank as .emsg GROUP 9: asset
slot 0x16 = extract/chunk03/f14_id16.bin (runtime pointer D_0028A4E8),
a BARE group OUTER blob (no group dir, no markup records) holding the
54 bit-31 lines of the mode-2 message machine (func_001FD950) -
locked-door refusals ("It's locked and won't open." = line 6),
station descriptions and corpse examines; even lines carry text, odd
lines are the empty terminal-record lines. The port's em_hud_radio
resolves group 9 line N.

OUTPUT FORMAT .emsg v1 (little-endian) - keep in sync with the port
loader (extermination-port/src/game/em_hud.c, msg_parse):

  off  size
  0    4    magic "EMSG"
  4    4    u32 version = 1
  8    4    u32 group_count
  12   4    u32 line_count (total, all groups)
  16   4    u32 blob_size
  20   8*group_count   per group: u32 first (index into the line
                       offset table), u32 count
  ...  4*line_count    u32 line offsets into the blob
  ...  blob_size       NUL-terminated ASCII/UTF-8 strings; '\n' is an
                       in-entry line break (multi-line help text)

USAGE (macOS arm64, repo root)
    python3 tools/parse_pcsx2_state.py <state.p2s> --out scratch/state01
    python3 tools/export_ui.py --gs scratch/state01/gs.bin \
        --out ../extermination-port/assets/ui.emui
    python3 tools/export_ui.py --page all \
        --out-dir ../extermination-port/assets [--png scratch/ui_pages]
    python3 tools/export_ui.py --messages \
        --out-dir ../extermination-port/assets
"""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))

from clut import write_png_rgba          # noqa: E402
from clut_pair import read_clut16_rgba, read_psmt4  # noqa: E402

# PCSX2 GS freeze blob: header, 4 MB local memory, 84 trailing state bytes
# (see tools/gs_vram.py - the local-memory base is len - 0x400000 - 84).
GS_LM_SIZE = 0x400000
GS_TRAILER = 84

TBW = 8           # all UI sprites live in the 512-texel-wide UI buffer

# Backdrop sentinel: x = y = -32767 marks the screen's ANIMATED-BACKGROUND
# tile (func_0020A7A0's per-screen texture; never drawn as a positioned
# sprite — the port's background pass picks it up). Plain sheet-only
# records keep -32768.
BACKDROP_XY = -32767

# (name, TBP0, CBP, w, h, canvas_x, canvas_y, draw_w, draw_h) -
# func_00209DF0/func_00209860 TEX0 tokens + audited draw anchors
# (FINDINGS.md "STATUS SCREEN LAYOUT" items 3/5/6/10). The bullet icon's
# 32x32 texture GS-scales to a 24x24 sprite; the backdrop record carries
# the engine's 2x tile draw size; everything else draws 1:1.
SPRITES = [
    ("title",    0x1E50, 0x229D, 128,  64,  16,   0, 128,  64),
    ("legend",   0x1D40, 0x22A8, 128, 128,   0, 320, 128, 128),
    ("icon_tl",  0x2186, 0x22F6,  32,  32,  11, 304,  32,  32),
    ("icon_tr",  0x21F0, 0x22F6,  32,  32, 484, 304,  32,  32),
    ("icon_bl",  0x2192, 0x22F6,  32,  32, 417,  10,  32,  32),
    ("icon_br",  0x21F4, 0x22F6,  32,  32, 417, 406,  32,  32),
    ("bullet",   0x2196, 0x228C,  32,  32,  16, 262,  24,  24),
    # the hub's animated-background tile (func_0020CDC0 hub state ->
    # func_0020A7A0 token 0x9D421E40_20045EE5; FINDINGS "STATUS SCREEN
    # BACKGROUND") — BACKDROP record, not a positioned sprite
    ("backdrop", 0x1E40, 0x22F7, 128,  64, BACKDROP_XY, BACKDROP_XY,
     256, 128),
]

# Sheet packing (2-px transparent gutters so bilinear sampling never
# bleeds between neighbors): legend left, title top-right, icons in rows
# under the title, backdrop tile on its own bottom shelf.
SHEET_W, SHEET_H = 272, 212
PACK = {            # name -> (u, v)
    "title":    (136,  0),
    "legend":   (  0,  0),
    "icon_tl":  (136, 72),
    "icon_tr":  (170, 72),
    "icon_bl":  (204, 72),
    "icon_br":  (238, 72),
    "bullet":   (136, 108),
    "backdrop": (136, 142),
}


# --------------------------------------------------------------------------
# Page-mode tables. Record = (name, tex0_lo, tex0_hi, anchor) - anchor is
# the 512x448-canvas position, None = sheet-only (packed but not
# auto-drawn), or BACKDROP = the page's func_0020A7A0 animated-background
# tile (exported with the BACKDROP_XY sentinel + 2x draw size).
# TEX0 fields decode as: TBP = lo & 0x3FFF, TBW = (lo>>14) & 0x3F, PSM =
# (lo>>20) & 0x3F (0x14 PSMT4 / 0x13 PSMT8), TW/TH log2 sizes, CBP =
# (hi>>5) & 0x3FFF. Records are in DRAW ORDER for the port's page view
# (backgrounds first, title/legend last). Sourcing functions noted.
SHEET_ONLY = None
BACKDROP = "backdrop"

PAGE_SPECS = {
    0: ("item", "chunk31", [
        # func_0020F170 (page-frame drawer): background panels
        ("bg_a",     0xDD321D00, 0x2003C405, SHEET_ONLY),
        ("bg_b",     0xDD421D20, 0x2003CA25, SHEET_ONLY),
        ("bg_wide",  0xE1421D40, 0x2003CA85, SHEET_ONLY),
        # func_0020F2A0 (hub-of-categories drawer): banners + slot frames
        ("ban_a",    0x5D421DC0, 0x2003C685, SHEET_ONLY),
        ("ban_a2",   0x5D421DC4, 0x2003C6A5, SHEET_ONLY),
        ("ban_b",    0x5D421DD0, 0x2003C805, SHEET_ONLY),
        ("ban_b2",   0x5D421DD4, 0x2003C685, SHEET_ONLY),
        ("ban_c",    0x5D421DE0, 0x2003C685, SHEET_ONLY),
        ("ban_c2",   0x5D421DE4, 0x2003C685, SHEET_ONLY),
        ("slot_a",   0x99421DF0, 0x2003CA05, SHEET_ONLY),
        ("slot_b",   0x99421DF8, 0x2003CA05, SHEET_ONLY),
        ("slot_c",   0x99421E00, 0x2003CA05, SHEET_ONLY),
        ("slot_d",   0x99421E10, 0x2003CA05, SHEET_ONLY),
        ("icon_a",   0x55321E24, 0x2003C405, SHEET_ONLY),
        ("icon_b",   0x55421E30, 0x2003C825, SHEET_ONLY),
        # func_0020F170 head: func_0020A7A0 animated-background tile
        # (the record s39 provisionally called "title_hi")
        ("bg_anim",  0x9D421DB0, 0x2003C8A5, BACKDROP),
        ("title",    0x9D421DA0, 0x2003C885, (8, 0)),     # "ITEM"
    ]),
    1: ("map", "chunk30", [
        # func_00210A00 (map drawer): graph-paper map, 2x1 + lower bands
        # (ASSUMED anchors, stitch-verified composition; map top y=64)
        ("map_l",    0x21321D00, 0x20042E86, (0, 64)),
        ("map_r",    0x21321D40, 0x20042E86, (256, 64)),
        ("band_l",   0xA1321F40, 0x20042E85, (0, 320)),
        ("band_r",   0xA1321FC0, 0x20042E85, (256, 320)),
        ("panel",    0xE1321F00, 0x20043205, SHEET_ONLY),
        ("pan_a",    0x9D422040, 0x20043A85, SHEET_ONLY),
        # func_00210A00 head: func_0020A7A0 animated-background tile
        # (the record s39 provisionally called "pan_b")
        ("bg_anim",  0x9D422050, 0x20043C25, BACKDROP),
        # func_00211240 via func_00210C00: player blip + 8 map icons
        ("blip",     0x113221D0, 0x20042E85, SHEET_ONLY),
        ("icon_1",   0x55322158, 0x20042E85, SHEET_ONLY),
        ("icon_2",   0x5532215C, 0x20042E85, SHEET_ONLY),
        ("icon_3",   0x55322168, 0x20042E85, SHEET_ONLY),
        ("icon_4",   0x5532216C, 0x20042E85, SHEET_ONLY),
        ("icon_5",   0x55322178, 0x20042E85, SHEET_ONLY),
        ("icon_6",   0x5532217C, 0x20042E85, SHEET_ONLY),
        ("icon_7",   0x55322180, 0x20042E85, SHEET_ONLY),
        ("icon_8",   0x55322184, 0x20042E85, SHEET_ONLY),
        # func_00210A00: button legend (ASSUMED at the hub legend anchor)
        ("legend",   0xDD422020, 0x20043A25, (0, 320)),
        # func_0020F950 state 1 draw call: GS 0x7080/0x7900 -> (8,0)
        ("title",    0x9D322000, 0x20042985, (8, 0)),     # "MAP"
    ]),
    2: ("spr4", "chunk44", [
        # func_00212B60 / func_00212F30: weapon-attachment renders
        # (256x128 each - SPR4 carbine, shotgun, grenade launchers,
        # flamethrower, missile launcher; selection-dependent, so only
        # the base carbine is anchored - ASSUMED center placement)
        ("wpn_spr4",  0xE1321D00, 0x20046B05, (128, 128)),
        ("wpn_b",     0xE1321E00, 0x20046F85, SHEET_ONLY),
        ("wpn_c",     0xE1321E40, 0x20047605, SHEET_ONLY),
        ("wpn_d",     0xE1321F00, 0x20047685, SHEET_ONLY),
        ("wpn_e",     0xE1321F40, 0x20047805, SHEET_ONLY),
        ("wpn_f",     0xE1322000, 0x20047C85, SHEET_ONLY),
        ("wpn_g",     0xE1322040, 0x20047705, SHEET_ONLY),
        # func_002121A0 / func_00212B60 / func_00212F30: panels + widgets
        ("panel_a",   0xE1422100, 0x200479C5, SHEET_ONLY),
        ("panel_b",   0xE1422180, 0x200479E5, SHEET_ONLY),
        ("panel_c",   0xE1422200, 0x200479E5, SHEET_ONLY),
        ("q2260",     0x9D322260, 0x20046D05, SHEET_ONLY),
        ("q2280",     0x9D322280, 0x20047885, SHEET_ONLY),
        ("q22A0",     0x9D3222A0, 0x20047E05, SHEET_ONLY),
        ("d2140",     0xDD322140, 0x20047E85, SHEET_ONLY),
        ("d2160",     0xDD322160, 0x20046785, SHEET_ONLY),
        ("d22C0",     0xDD4222C0, 0x20047B65, SHEET_ONLY),
        ("d22E0",     0xDD4222E0, 0x20047D45, SHEET_ONLY),
        ("v2320",     0xD9422320, 0x20047B45, SHEET_ONLY),
        ("q2310",     0x99322310, 0x20046F05, SHEET_ONLY),
        ("w2328",     0x59322328, 0x20047C05, SHEET_ONLY),
        ("w2350",     0x59322350, 0x20046D85, SHEET_ONLY),
        ("w2360",     0x59322360, 0x20047A05, SHEET_ONLY),
        ("w2370",     0x59322370, 0x20047A85, SHEET_ONLY),
        ("b2380",     0x5D422380, 0x20047D65, SHEET_ONLY),
        ("b2384",     0x5D422384, 0x20047DC5, SHEET_ONLY),
        ("b2390",     0x5D422390, 0x20047DE5, SHEET_ONLY),
        ("b2394",     0x5D422394, 0x20047F45, SHEET_ONLY),
        ("b23A0",     0x5D4223A0, 0x20047F65, SHEET_ONLY),
        ("b23A4",     0x5D4223A4, 0x20047FC5, SHEET_ONLY),
        ("s2338",     0x55322338, 0x20046785, SHEET_ONLY),
        ("s23BC",     0x554223BC, 0x20047BC5, SHEET_ONLY),
        ("s23C8",     0x554223C8, 0x20047BE5, SHEET_ONLY),
        # func_002121A0 head: func_0020A7A0 animated-background tile
        # (the record s39 provisionally called "title_b")
        ("bg_anim",   0x9D422300, 0x20047945, BACKDROP),
        ("legend",    0xDD422240, 0x20047965, (0, 320)),  # ASSUMED
        ("title",     0x9D422340, 0x200480A5, (8, 0)),    # "SPR4"
    ]),
    3: ("database", "chunk36", [
        # func_002131B0: 8x 256x128 background tiles - stitch-verified
        # 2-col x 4-row composition (top rounded panel + divider bar +
        # lower panel); ASSUMED anchored at the canvas origin.
        ("bg_00",    0xE1321D00, 0x20044A05, (0, 0)),
        ("bg_10",    0xE1321D40, 0x20044A85, (256, 0)),
        ("bg_01",    0xE1321E00, 0x20044C05, (0, 128)),
        ("bg_11",    0xE1321E40, 0x20044C85, (256, 128)),
        ("bg_02",    0xE1321F00, 0x20044E85, (0, 256)),
        ("bg_12",    0xE1321F40, 0x20044305, (256, 256)),
        ("bg_03",    0xE1322000, 0x20044385, (0, 384)),
        ("bg_13",    0xE1322040, 0x20044505, (256, 384)),
        # func_002131B0 / func_00213A00: banners + "PAGE  /" counter
        ("ban_a",    0xA1322100, 0x20044585, SHEET_ONLY),
        ("ban_b",    0xA1322140, 0x20044705, SHEET_ONLY),
        ("ban_c",    0xA1322180, 0x20044785, SHEET_ONLY),
        ("pagectr",  0x9D3221C0, 0x20044E05, (368, 8)),   # ASSUMED
        # func_002134C0: chevrons + small icons
        ("chev_a",   0x59322210, 0x20044905, SHEET_ONLY),
        ("chev_b",   0x59322220, 0x20044985, SHEET_ONLY),
        ("chev_c",   0x59322230, 0x20044B05, SHEET_ONLY),
        ("chev_d",   0x59322240, 0x20044B85, SHEET_ONLY),
        ("ic_a",     0x55422268, 0x20045025, SHEET_ONLY),
        ("ic_b",     0x5542226A, 0x20045085, SHEET_ONLY),
        ("ic_c",     0x55422278, 0x20045025, SHEET_ONLY),
        ("ic_d",     0x5542227C, 0x20045025, SHEET_ONLY),
        # func_00214020 state 2/3 token - RESOLVED: the page's
        # func_0020A7A0 animated-background tile (s39 called it "t2200",
        # "identity unresolved"; no "DATABASE" art title exists in the
        # statically reachable set - the heading may still be data-driven
        # via func_00213F30)
        ("bg_anim",  0x9D422200, 0x200450A5, BACKDROP),
        ("legend",   0xDD4221E0, 0x20045005, (0, 320)),   # ASSUMED
    ]),
    # page 4 (passcode keypad, chunk37/extract): func_00207350 reads its
    # textures from data tables (D_00275860[page-4]) - zero statically
    # reachable TEX0 tokens; handled specially in build_page().
    4: ("keypad", "chunk37", []),
}


def tex0_fields(lo: int, hi: int) -> dict:
    """Decode the TEX0 fields the UI textures use (audited rule)."""
    return {
        "tbp": lo & 0x3FFF,
        "tbw": (lo >> 14) & 0x3F,
        "psm": (lo >> 20) & 0x3F,
        "w":   1 << ((lo >> 26) & 0xF),
        "h":   1 << (((lo >> 30) & 0x3) | ((hi & 0x3) << 2)),
        "cbp": (hi >> 5) & 0x3FFF,
    }


def decode_token_lm(lm: bytes, lo: int, hi: int) -> tuple[bytes, dict]:
    """Decode one inlined TEX0 token from (synthetic or dumped) GS local
    memory -> screen-oriented RGBA8 (v-flip undone)."""
    from clut_pair import read_psmt8, read_clut_rgba   # noqa: E402
    t = tex0_fields(lo, hi)
    if t["psm"] == 0x14:                       # PSMT4 + 16-entry CLUT
        idx = read_psmt4(lm, t["tbp"], t["tbw"], t["w"], t["h"])
        clut = read_clut16_rgba(lm, t["cbp"])
    elif t["psm"] == 0x13:                     # PSMT8 + 256-entry CLUT
        idx = read_psmt8(lm, t["tbp"], t["tbw"], t["w"], t["h"])
        clut = read_clut_rgba(lm, t["cbp"])
    else:
        raise ValueError(f"unsupported PSM {t['psm']:#x}")
    w, h = t["w"], t["h"]
    out = bytearray(w * h * 4)
    for y in range(h):
        src = (h - 1 - y) * w                  # undo the stored v-flip
        dst = y * w * 4
        for x in range(w):
            c = idx[src + x] * 4
            out[dst:dst + 4] = clut[c:c + 4]
            dst += 4
    return bytes(out), t


def pack_shelf(items: list[tuple[int, int]], sheet_w: int,
               gutter: int = 2) -> tuple[list[tuple[int, int]], int]:
    """Shelf-pack (w,h) items into a sheet_w-wide sheet; returns
    per-item (u,v) positions (input order preserved) and the sheet
    height. Items are placed tallest-first per shelf."""
    order = sorted(range(len(items)), key=lambda i: -items[i][1])
    pos: list[tuple[int, int] | None] = [None] * len(items)
    x = y = shelf_h = 0
    for i in order:
        w, h = items[i]
        if x + w > sheet_w:
            y += shelf_h + gutter
            x = shelf_h = 0
        pos[i] = (x, y)
        x += w + gutter
        shelf_h = max(shelf_h, h)
    return pos, y + shelf_h            # type: ignore[return-value]


def build_page(page: int, out_dir: Path, extract_dir: Path,
               png_dir: Path | None) -> int:
    """Decode one pager page's texture set from the user's extract/ chunk
    and write assets/ui_pageN.emui. Returns 0/1 like a main()."""
    import glob as _glob
    name, chunk, specs = PAGE_SPECS[page]
    if not specs:
        print(f"page {page} ({name}): 0 statically reachable TEX0 tokens "
              "- the keypad view (func_00207350) pulls its textures from "
              "data tables; nothing to export (the port renders its "
              "flagged placeholder fill). Honest coverage: 0/0.")
        return 0
    files = sorted(_glob.glob(str(extract_dir / chunk / "f*.bin")))
    if not files:
        sys.exit(f"error: {extract_dir / chunk} has no chunk files - run "
                 "tools/extract_data.py against your own disc first")
    sys.path.insert(0, str(_HERE))
    from export_level import read_uploads_localmem   # noqa: E402
    lm, covered = read_uploads_localmem(files)

    decoded = []                # (name, rgba, fields, anchor)
    skipped = []
    for spec in specs:
        nm, lo, hi, anchor = spec
        t = tex0_fields(lo, hi)
        blk = t["tbp"]
        if not covered[blk]:    # leading block unwritten -> not uploaded
            skipped.append((nm, "not covered by the chunk's upload"))
            continue
        rgba, t = decode_token_lm(lm, lo, hi)
        cov = ink_coverage(rgba)
        if cov == 0.0:
            skipped.append((nm, "decodes fully transparent"))
            continue
        decoded.append((nm, rgba, t, anchor))
        if png_dir:
            png_dir.mkdir(parents=True, exist_ok=True)
            write_png_rgba(png_dir / f"page{page}_{nm}.png",
                           t["w"], t["h"], rgba)

    pos, sheet_h = pack_shelf([(d[2]["w"], d[2]["h"]) for d in decoded],
                              512)
    sheet_h = max(sheet_h, 1)
    sheet = bytearray(512 * sheet_h * 4)
    records = []
    anchored = 0
    backdrops = 0
    for (nm, rgba, t, anchor), (u, v) in zip(decoded, pos):
        w, h = t["w"], t["h"]
        for y in range(h):
            o = ((v + y) * 512 + u) * 4
            sheet[o:o + w * 4] = rgba[y * w * 4:(y + 1) * w * 4]
        dw, dh = w, h
        if anchor == BACKDROP:          # animated-background tile
            cx = cy = BACKDROP_XY
            dw, dh = w * 2, h * 2       # the engine's 2x tile draw size
            backdrops += 1
        elif anchor is not None:
            cx, cy = anchor
            anchored += 1
        else:
            cx = cy = -32768            # sheet-only sentinel (not drawn)
        records.append((u, v, w, h, cx, cy, dw, dh))

    out = out_dir / f"ui_page{page}.emui"
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("wb") as f:
        f.write(b"EMUI")
        f.write(struct.pack("<4I", 1, 512, sheet_h, len(records)))
        for r in records:
            f.write(struct.pack("<4H2h2H", *r))
        f.write(sheet)
    print(f"page {page} ({name}, {chunk}): {len(decoded)}/{len(specs)} "
          f"tokens decoded ({anchored} anchored, {backdrops} backdrop, "
          f"{len(decoded) - anchored - backdrops} sheet-only) -> {out} "
          f"(sheet 512x{sheet_h})")
    for nm, why in skipped:
        print(f"  skipped {nm}: {why}")
    return 0


def parse_outer(data: bytes, outer: int, label: str) -> tuple[list[bytes], int]:
    """Parse one group OUTER blob (markup dir + TEXT blob) at `outer`.
    Returns (lines, markup_line_count). Layout per the docstring; the
    same shape serves the slot-2 groups AND the bare slot-0x16 bank."""
    text_off, count, rec_size = struct.unpack_from("<3I", data, outer)
    markup_lines = 0
    # markup directory (16 B/line at outer+0x10): +0xC = nrecords<<4
    for i in range(count):
        if struct.unpack_from("<I", data, outer + 0x10 + i * 16 + 12)[0]:
            markup_lines += 1
    text = outer + text_off + rec_size      # func_001FE460's walk
    strbase, count2, str_bytes, _ = struct.unpack_from("<4I", data, text)
    if count2 != count:
        sys.exit(f"error: {label}: outer/text line counts disagree "
                 f"({count} vs {count2})")
    lines = []
    walk = text + strbase            # NUL-walk cross-check cursor
    for i in range(count):
        off, off2, ln, ln_nul = struct.unpack_from(
            "<4I", data, text + 0x10 + i * 16)
        if off != off2 or ln_nul != ln + 1:
            sys.exit(f"error: {label} line {i}: unexpected line "
                     f"entry ({off:#x}/{off2:#x}, {ln}/{ln_nul})")
        s = data[text + strbase + off: text + strbase + off + ln]
        # the engine resolves line N by walking NUL terminators
        # (func_001FE070) - verify both views agree
        if walk != text + strbase + off or data[walk + ln] != 0:
            sys.exit(f"error: {label} line {i}: line table "
                     "disagrees with the NUL walk")
        walk += ln + 1
        lines.append(s)
    return lines, markup_lines


def build_messages(out_dir: Path, extract_dir: Path) -> int:
    """Decode the chunk00 message bank (asset slot 2) PLUS the global
    examine/radio text bank (asset slot 0x16) and write
    assets/messages.emsg. Returns 0/1 like a main()."""
    src = extract_dir / "chunk00" / "f02_id02.bin"
    if not src.is_file():
        sys.exit(f"error: {src} not found - run tools/extract_data.py "
                 "against your own disc first")
    data = src.read_bytes()
    dir_base, ngroups, total, dir_off = struct.unpack_from("<4I", data, 0)
    if dir_base >= len(data) or ngroups == 0 or ngroups > 64:
        sys.exit(f"error: {src}: implausible bank header "
                 f"(base {dir_base:#x}, {ngroups} groups)")

    groups: list[list[bytes]] = []
    markup_lines = 0
    for g in range(ngroups):
        goff = struct.unpack_from("<I", data, dir_off + g * 16)[0]
        lines, ml = parse_outer(data, dir_base + goff, f"group {g}")
        markup_lines += ml
        groups.append(lines)

    # ---- the GLOBAL examine/radio bank: asset slot 0x16 -----------------
    # chunk03/f14_id16.bin (runtime pointer D_0028A4E8) is a BARE group
    # OUTER blob (no group dir): the bit-31 line words of the mode-2
    # message machine (func_001FD950) resolve their text here — locked-
    # door refusals, station descriptions, corpse examines (FINDINGS.md
    # "RADIO-MESSAGE MACHINE DECODED", 2026-06-11). Appended as ONE
    # extra .emsg group (index = ngroups = 9) so the port's em_hud_radio
    # can address it as group 9 line N. Missing file (partial extract):
    # warn and write the slot-2 groups only (back-compatible bank).
    radio = extract_dir / "chunk03" / "f14_id16.bin"
    radio_group = -1
    if radio.is_file():
        rdata = radio.read_bytes()
        rlines, rml = parse_outer(rdata, 0, "slot-0x16 bank")
        markup_lines += rml
        radio_group = len(groups)
        groups.append(rlines)
    else:
        print(f"warning: {radio} not found - the radio/examine bank "
              "(group 9) is not exported; locked-door messages will "
              "draw nothing in the port")

    # ---- write .emsg v1 -------------------------------------------------
    firsts, offsets, blob = [], [], bytearray()
    for lines in groups:
        firsts.append(len(offsets))
        for s in lines:
            offsets.append(len(blob))
            blob += s + b"\0"
    out = out_dir / "messages.emsg"
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("wb") as f:
        f.write(b"EMSG")
        f.write(struct.pack("<4I", 1, len(groups), len(offsets), len(blob)))
        for g, lines in enumerate(groups):
            f.write(struct.pack("<2I", firsts[g], len(lines)))
        for o in offsets:
            f.write(struct.pack("<I", o))
        f.write(blob)

    print(f"message bank {src.name} (+ slot-0x16 examine bank): "
          f"{len(groups)} groups, "
          f"{len(offsets)} lines, {len(blob)} text bytes -> {out}")
    if markup_lines:
        print(f"  note: {markup_lines} lines carry inline markup records "
              "(style/color runs - not exported; plain text only)")
    for g, lines in enumerate(groups):
        first = lines[0].split(b"\n")[0].decode("ascii", "replace") \
                if lines else ""
        tag = "  (slot 0x16, radio/examine)" if g == radio_group else ""
        print(f"  group {g}: {len(lines):3d} lines  "
              f"(line 0: {first[:46]!r}){tag}")
    return 0


def decode_sprite(lm: bytes, tbp: int, cbp: int, w: int, h: int) -> bytes:
    """PSMT4 texture + 16-entry CSM1 CLUT -> RGBA8, v-flipped back to
    screen orientation (rows top-down as drawn)."""
    idx = read_psmt4(lm, tbp, TBW, w, h)
    clut = read_clut16_rgba(lm, cbp)        # GS alpha 0x80 -> 255 inside
    out = bytearray(w * h * 4)
    for y in range(h):
        src = (h - 1 - y) * w               # undo the stored v-flip
        dst = y * w * 4
        for x in range(w):
            c = idx[src + x] * 4
            out[dst:dst + 4] = clut[c:c + 4]
            dst += 4
    return bytes(out)


def ink_coverage(rgba: bytes) -> float:
    n = sum(1 for i in range(3, len(rgba), 4) if rgba[i] > 8)
    return n / (len(rgba) // 4)


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--gs", default="scratch/state01/gs.bin",
                    help="GS freeze blob from parse_pcsx2_state.py "
                         "(default: scratch/state01/gs.bin)")
    ap.add_argument("--out", default="assets/ui.emui",
                    help="output .emui path (default: assets/ui.emui)")
    ap.add_argument("--png", metavar="DIR",
                    help="also write per-sprite debug PNGs into DIR")
    ap.add_argument("--page", metavar="N",
                    help="page mode: export pager sub-screen N (0-4) or "
                         "'all' from extract/ chunks to ui_pageN.emui")
    ap.add_argument("--messages", action="store_true",
                    help="messages mode: export the chunk00 message bank "
                         "(asset slot 2) to messages.emsg in --out-dir")
    ap.add_argument("--extract", default="extract",
                    help="extract/ root with the chunk dirs "
                         "(default: extract)")
    ap.add_argument("--out-dir", default="../extermination-port/assets",
                    help="page-mode output dir "
                         "(default: ../extermination-port/assets)")
    args = ap.parse_args(argv)

    if args.messages:
        return build_messages(Path(args.out_dir), Path(args.extract))

    if args.page is not None:
        pages = list(PAGE_SPECS) if args.page == "all" else [int(args.page)]
        png_dir = Path(args.png) if args.png else None
        rc = 0
        for p in pages:
            rc |= build_page(p, Path(args.out_dir), Path(args.extract),
                             png_dir)
        return rc

    gs = Path(args.gs).read_bytes()
    base = len(gs) - GS_LM_SIZE - GS_TRAILER
    if base < 0:
        sys.exit(f"error: {args.gs} is smaller than a GS freeze blob")
    lm = gs[base:base + GS_LM_SIZE]

    sheet = bytearray(SHEET_W * SHEET_H * 4)
    records = []
    for name, tbp, cbp, w, h, cx, cy, dw, dh in SPRITES:
        rgba = decode_sprite(lm, tbp, cbp, w, h)
        cov = ink_coverage(rgba)
        print(f"{name:8s} TBP {tbp:#06x} CBP {cbp:#06x} {w}x{h} "
              f"-> ({cx},{cy}) {dw}x{dh}  ink {cov * 100:.0f}%")
        if cov == 0.0:
            sys.exit(f"error: {name} decodes fully transparent - is this "
                     "an in-game save state with the UI chunk loaded?")
        u, v = PACK[name]
        for y in range(h):
            o = ((v + y) * SHEET_W + u) * 4
            sheet[o:o + w * 4] = rgba[y * w * 4:(y + 1) * w * 4]
        records.append((u, v, w, h, cx, cy, dw, dh))
        if args.png:
            d = Path(args.png)
            d.mkdir(parents=True, exist_ok=True)
            write_png_rgba(d / f"{name}.png", w, h, rgba)

    out = Path(args.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("wb") as f:
        f.write(b"EMUI")
        f.write(struct.pack("<4I", 1, SHEET_W, SHEET_H, len(records)))
        for (u, v, w, h, cx, cy, dw, dh) in records:
            f.write(struct.pack("<4H2h2H", u, v, w, h, cx, cy, dw, dh))
        f.write(sheet)
    print(f"wrote {out} - sheet {SHEET_W}x{SHEET_H}, "
          f"{len(records)} sprites")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
