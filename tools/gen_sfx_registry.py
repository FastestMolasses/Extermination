#!/usr/bin/env python3
"""gen_sfx_registry.py — emit the native port's SFX registry (sfx.txt).

Resolves engine SOUND IDS to the locally-decoded WAV files through
extract/audio_decoded/soundmap.json (the `audio_export.py soundmap`
output) and writes the port's `assets/sfx/sfx.txt` registry: one
`<id-hex> <abs-wav-path>` line per sound (em_sfx.c parses exactly that;
'#' lines are comments).

Soundmap schema (commit 99cf57d):
  - `sounds` keys are UPPERCASE zero-padded hex: '0x162', '0x3FB',
    '0x7D8' (a naive lowercase/`%x` lookup finds nothing).
  - GLOBAL ids carry the trigger-script decode inline:
        {"record": [g,b,sg,si], "bank": ..., "events": [...]}
  - AREA-TABLED / region-dependent ids instead carry "variants", each
    with the regions ("chunk04.n0") and the area list ("2.1") it is
    loaded for. Resolution order here: a variant listing the requested
    area wins; otherwise fall back to the area's region from
    `area_scene_map` (its best-coverage container bank).
  - Every event lists the WAV (relative to the soundmap's directory)
    and the integer SPU pitch word (audio_export.a0_pitch). The registry
    keeps the FIRST event's WAV.

RETIRED FOR PLAYBACK (2026-09, WP-14/H19): the native port no longer reads
sfx.txt. Its em_sfx loads assets/sfx/sfx_registry.emsr, produced by the
port's tools/export_sfx_registry.py, which carries every A0 event with its
integer pitch, Q14 volume inputs and sequencer tick. The legacy WAV rates
this file used to cite were x1.531 sharp (the retired tone_rate formula).
This tool remains the source of the per-scene id presets (SCENES) and the
door-pair / locked-door census, which the port exporter imports.

DOOR PAIR IDS (FINDINGS.md "DOOR SCRIPTS DECODED" s23): a door's open
sound is NOT a constant — `func_001BBD60` patches the op-0x0B sub-6
script record with `D_0024DB80[link >> 8][side]`, the halfword
[front_id, back_id] pair table indexed by the placement LINK halfword's
high byte (ids 0x3FB..0x40E). `--scene office` recomputes this from the
user's own local data: the AREA02 state-1 placement table (door records,
tools/placements.py) for the link bytes + the boot ELF for the pair
table. It also prints the matching `doorsfx <front> <back>` scene.txt
line the port's em_door consumes.

Usage (macOS arm64, plain python3 — no deps):
  tools/gen_sfx_registry.py --scene office \
      -o ../extermination-port/assets/sfx/sfx.txt
  tools/gen_sfx_registry.py 0x162 0x7D8 --area 2.1   # ad-hoc id list

Everything read here is the user's own locally-extracted material; the
output registry is likewise local-only (the port gitignores assets/).
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SOUNDMAP = ROOT / "extract" / "audio_decoded" / "soundmap.json"
BOOT_ELF = ROOT / "elf" / "SCUS_971.12.elf"

# D_0024DB80 — door sound [front_id, back_id] halfword pair table
# (FINDINGS s23), 13 used entries (selectors 0..12).
DOOR_PAIR_VADDR = 0x24DB80
DOOR_PAIR_COUNT = 16

# LOCKED-DOOR "VO" — func_001BBAE0 (the locked script's op-0x09 native,
# decoded 2026-06-11/12 from the .s + local ELF data): the door LINK
# halfword's low 6 bits select through jtbl_0026E1A0 (6 cases) a RADIO
# MESSAGE LINE word 0x8000000X posted to the message machine
# D_002821B0 (mode 2, line D_002821B8, duck B0=2/B4=1). Bit 31 = the
# GLOBAL message table D_00264DD0[0] (per-area tables start at [1],
# func_001FD790); the 8-byte line records are {u16 duration/steps,
# s16 voice_cue, u8, u8 wait_stream}. Every locked-door line's
# voice_cue is -1: the "VO" is a TEXT-ONLY radio message (typewriter
# subtitle, bank slot 0x16 string[line]) — there is NO audio id to
# register. The decode below recomputes this from the user's local
# data per lock-gated door and emits the verdict as comments; if a
# future line ever carries a real cue, a `lockedvo` scene.txt line is
# emitted for the port's em_door (which plays it through em_sfx).
LOCKED_VO_JTBL_VADDR = 0x26E1A0
LOCKED_VO_CASE_LINE = {          # case target vaddr -> line index
    0x1BBB60: 0x6, 0x1BBB6C: 0x0, 0x1BBB74: 0x2,
    0x1BBB80: 0x8, 0x1BBB8C: 0xA, 0x1BBB98: 0x4,
}
GLOBAL_MSG_PTR_VADDR = 0x264DD0  # D_00264DD0[0] = global message table

# Lock-gated door placements in the EXPORTED scenes (FINDINGS s56 lock
# gate: model 0x15 hinged + flags2-{0x16,0x17,0x3E} sliders): the two
# m15 security doors. (area, sub, overlay, table_vaddr) per scene.
LOCKED_DOOR_TABLES = [
    ("scene_office0 (AREA02 sub 0)", "AREA02.BIN", 0x827830),
    ("scene_drawbridge (AREA01 sub 0)", "AREA01.BIN", 0x82BD50),
]
SLIDER_LOCK_FLAGS2 = {0x16, 0x17, 0x3E}
FN_DOOR_HINGED = 0x001BC350
FN_DOOR_SLIDER = 0x001BB860

# The locked rattle — the locked script's op-0x17 sub-0 positional
# sound at its 60-frame mark (FINDINGS s23 D_0024DEC0 @24DD80).
LOCKED_RATTLE_ID = 0x3F2

# Office scene = AREA02 sub-state 1 (placement table 0x828170), engine
# area key 2.1 in the soundmap's area tables.
#
# Snow scene = AREA11 save-state 01 (placement table 0x82A3C0, 21 entries;
# FINDINGS s9 overlay-table census), engine area key 11.0 in the
# soundmap's area tables -> region chunk15 (the opening snow level's
# per-area sound bank; area_scene_map 11.0 -> chunk15, exact coverage
# 1.0). This is the FIRST PLAYABLE level (MEMORY new-game-first-level).
# Its chunk15 bank is a SMALL opening-area bank: it carries the
# elevator/door/ambient set but NONE of the crawler/tendril combat-enemy
# tones (0x42D/0x42F/0x430/0x431/0x434) — those have no chunk15 variant
# in the soundmap, i.e. the engine never loads them in AREA-11 (faithful:
# no combat enemies in the opening), so they are intentionally omitted.
# The ids below are exactly those the port's snow scene fires (em_game.c
# ELEV_SFX_DOWN 0x453 + the up 0x452, em_enemy.c EGG_SFX_BREAK 0x1A1 +
# the burst pair 0x1A0, and the scene.txt `doorsfx 0x401 0x402`) AND
# resolve to a real chunk15 (or GLOBAL) sample.
SCENES = {
    "office": {
        "area": "2.1",
        "overlay": "AREA02.BIN",
        "table_vaddr": 0x828170,
        # Gameplay ids already wired in the port (em_sfx.h):
        # - weapon draw 0x162 / handling-foley 0x163 (holster + reload
        #   start) / fire 0x164/0x165 / reload MAG ACTION 0x168 / dry
        #   0x169 — the reload pair 0x163+0x168 was live-pinned s29
        #   (FINDINGS "GAMEPLAY SOUND IDS PINNED LIVE"), retiring the
        #   old 0xF002 -> 0x166 placeholder alias.
        # - FOOTSTEPS (s37 "FOOTSTEP SURFACE TABLE" — replaces the s29
        #   fixed-pair reading): per step, surface_id = BLOCK(attr) +
        #   GAIT_SUB(gait) + rand5() and gear_id = 0x138 + rand5()
        #   (independent draw; rand5 = (rand() & 7), 5..7 folded to
        #   0..2). The office floor is attr 0 -> block 0x10, so the
        #   FULL variant sets ship: walk (gait 2, +5) 0x15..0x19, run
        #   (gait 3, +0xA) 0x1A..0x1E, gear/cloth 0x138..0x13C.
        # - canonical enemy death 0x7D8.
        # - KNIFE/MELEE (s36 decode, FINDINGS "KNIFE/MELEE DECODED"):
        #   swing/impact sounds 0x17D/0x17E/0x17F (light hits 1-3; the
        #   heavy stab shares 0x17F), fired at each attack's impact
        #   gate, vol 300; 0x179 = the armed-stance SQUARE attachment-0
        #   toggle-ON sound (func_0017A970 — the s29 "unidentified
        #   action", snd_0436; user-attested 2026-06-11 as the
        #   FLASHLIGHT toggle — the D_00810D3C flag).
        # - FLASHLIGHT auto-off 0x15D (s28b light decode: the 300-frame
        #   burst's turn-off anim/event id doubles as the pinned 920 ms
        #   switch sound, snd_0361) — the port's em_weapon plays it at
        #   the burst expiry (2026-06-11 weapon-fidelity pass).
        # - FIRE-CHAIN TAIL (s29 live, FINDINGS "GAMEPLAY SOUND IDS
        #   PINNED LIVE"): wall impact/ricochet 0x189 ~2 frames after
        #   the shot (snd_0421) + shell casing 0x16A ~0.7 s after each
        #   shot (2-event, snd_0347). SURFACE-VARIANT FLAG: the
        #   soundmap's 0x188/0x18A/0x18B neighbors (snd_0423/0422/0420,
        #   consecutive tones of the same program) look like the
        #   per-surface impact family, but no surface->id mapping is
        #   pinned in FINDINGS — only the observed 0x189 ships; the
        #   port plays it for every wall (em_sfx.h flag).
        # - LOCKED-DOOR RATTLE 0x3F2 (s23 locked script D_0024DEC0's
        #   op-0x17 sub-0 at its 60-frame mark; em_door's locked
        #   sequence plays it — same snd_0533 WAV in every exported
        #   area's bank). The locked "VO" itself is a TEXT-ONLY radio
        #   message (see LOCKED-DOOR "VO" above) — verdict comments per
        #   lock-gated door are appended by locked_door_census().
        # - PLAYER DAMAGE / DEATH voice set (2026-06-11 damage-pipeline
        #   decode, FINDINGS "PLAYER DAMAGE & DEATH"; all GLOBAL ids,
        #   vol 300 in the engine):
        #     0x152 hurt grunt (flinch func_0021D800; 0x153 = the
        #            infection-hit variant, +0x1F1 == 1)
        #     0x146 death voice + 0x151 death body foley (death-seq
        #            phase 0, func_0021E240/func_0021E830)
        #     0x156 mid-fall body cue (death clip T-80)
        #     0x14E body-hits-ground thud (T-16; 0x14F = infected)
        #     0x149 infection-hits-100 sting (func_0021C270)
        #     0x147 flinch-while-falling extra (func_0021D800 settle)
        #     0x14D infected-death extra (func_0021C200)
        #     0x159 type-7 latched kill voice (func_0021C440)
        "ids": [0x162, 0x163, 0x164, 0x165, 0x168, 0x169, 0x7D8,
                0x015, 0x016, 0x017, 0x018, 0x019,   # walk (gait 2)
                0x01A, 0x01B, 0x01C, 0x01D, 0x01E,   # run (gait 3)
                0x138, 0x139, 0x13A, 0x13B, 0x13C,   # gear/cloth
                0x17D, 0x17E, 0x17F, 0x179, 0x15D,
                0x189, 0x16A, LOCKED_RATTLE_ID,
                0x152, 0x153, 0x146, 0x151, 0x156,   # player damage
                0x14E, 0x14F, 0x149, 0x147, 0x14D, 0x159],
    },
    "snow": {
        # AREA-11 opening snow level (area 11.0 -> chunk15). The first
        # PLAYABLE level; sounds the port's snow scene actually triggers.
        "area": "11.0",
        "overlay": "AREA11.BIN",
        "table_vaddr": 0x82A3C0,   # AREA11 save-state-01 placement table
        # - ELEVATOR descent 0x453 (em_game.c ELEV_SFX_DOWN) + ascent 0x452
        #   (chunk15: snd_0937 / snd_0942). The elevator-control terminal
        #   gates the descent; both directions ship so the cab is not
        #   silent in either travel.
        # - DRUM/egg burst pair 0x1A0 (GLOBAL "fixture death", snd_0469) +
        #   0x1A1 (GLOBAL, snd_0464; em_enemy.c EGG_SFX_BREAK). The
        #   explodable snow drums + the egg-pod break reuse this global
        #   fixture-death pair — global ids, area-independent.
        # - DOOR front/back 0x401/0x402 (chunk15: snd_0520 / snd_0519).
        #   AREA11's lone door (placement uid 0x00, link 0x0400) selects
        #   D_0024DB80[4] = [0x401, 0x402]; the snow scene.txt's
        #   `doorsfx 0x401 0x402` line consumes exactly this pair, and
        #   office's 0x3FD/0x3FE stay registered alongside (no collision).
        # - WEDGED-TRUCK fall crash 0x454 (em_truck.c TRUCK_SFX_FALL, the
        #   §11.3 groan/crash cue fired with em_sfx_play_at on the fall).
        #   AREA-TABLED: its chunk15 variant covers area 11.0 (snd_0938,
        #   2-event). Loaded by the
        #   engine in AREA-11, so this ships (the s78 em_truck FLAG that
        #   it "may be absent until the AREA-11 bank is exported" is now
        #   resolved: present in chunk15).
        # - STEAM/FX EMITTER LOOP 0x413 (the §11 steam-pipe emitter,
        #   placement record 7 near (452,279,278); fired positionally by
        #   the FX emitter when its em_sfx_play_at is wired). AREA-TABLED:
        #   its chunk15 variant's area list explicitly carries area 11.0
        #   (alongside 1.7/4.5/6.3/10.1), resolving exact-area to snd_0733
        #   (2-event), the same chunk15
        #   resolution path as 0x452/0x401/0x454. The engine loads this id
        #   into AREA-11's bank, so the steam emitter is audible once wired.
        # - 0x19A (GLOBAL one-shot, snd_0405; area-independent like
        #   the 0x1A0/0x1A1 fixture pair and the 0x97/0x99 stings). A global
        #   id is always resident, so it is loaded in AREA-11 and registered
        #   here for the §11 emitter set (re-audit follow-up).
        # - CINEMATIC opening-beat MUSIC STINGS 0x97 / 0x99 (em_game.c
        #   director CINE_MUSIC_BEAT1 / BEAT2, played non-positionally via
        #   em_sfx_play((unsigned)b->music) on beat ENTER). These are NOT
        #   streamed MUSIC.DAT/BGM cues (those are the small 1..28 cue
        #   indices of the D_0025DD30 cue table -> em_bgm; AREA-11's
        #   looping BGM is cue 25/track_16). 0x97 (151) and 0x99 (153) are
        #   GLOBAL SShd BANK trigger ids in chunk00/f05_id05.bin#bank0 —
        #   the same global container as the weapon/UI set (0x162.. live in
        #   #bank1; the egg/fixture pair 0x1A0/0x1A1 also there) — i.e.
        #   short one-shot musical stings the bank fires by id, fully
        #   addressable through em_sfx_play. No bgm/code change needed; they
        #   just had to be in the registry (0x97 = snd_0117,
        #   4-event; 0x99 = snd_0192). Below any shipped bank id so
        #   no collision with the office/AREA-11 sets.
        # OMITTED (no chunk15 variant -> not loaded by the engine in
        # AREA-11, i.e. faithful silence, NOT a bug):
        # - the locked-door rattle 0x3F2 and the crawler/tendril combat
        #   tones 0x42D/0x42F/0x430/0x431/0x434 (no combat enemies in the
        #   opening snow area);
        # - the AREA POWER-ON cue 0x3EE (func_001580C0 master-unlock /
        #   func_001FB9F0 0x3EE, fired when the powered flag sets). 0x3EE is
        #   the near-universal power-on sting (snd_0536) loaded in 19 other
        #   regions across 86 area sub-states, but it has NO chunk15 variant
        #   and 11.0 is in none of its area lists — the engine never loads
        #   it into AREA-11's bank, so the powered-flag cue is FAITHFULLY
        #   SILENT in the opening area. Not registered (cannot fabricate a
        #   sample the engine never loads there). FLAGGED for a later code
        #   pass: if a live capture ever proves AREA-11 plays a power-on
        #   sound, it would be a different chunk15 id, not 0x3EE.
        # - PLAYER FOOTSTEPS on the snow ground (s82 decode, FINDINGS
        #   "FOOTSTEP SURFACE TABLE"). The per-frame footstep slice
        #   func_00187350 fires at the locomotion clip's property-table
        #   frameA/frameB (the foot-plant frames: walk clip id 1 -> frames
        #   72/21, run clip id 2 -> 26/3) — i.e. TWO steps per clip cycle,
        #   cadence is clip-phase driven (the run clip is shorter, so steps
        #   come faster), NOT a fixed interval / distance accumulator. Each
        #   step submits TWO positional func_001FBD50(actor, id, 300.0, 0)
        #   calls back-to-back: a SURFACE id and the GEAR/cloth id.
        #     surface_id = BLOCK(attr) + GAIT_SUB(gait) + rand5()
        #     gear_id    = 0x138 + rand5()   (independent 2nd rand5)
        #   func_00182430 reads attr from actor +0x23A (the collision hit
        #   record's surface byte +0x1A, copied by func_00175900). AREA-11's
        #   snow GROUND is surface ATTR 4 (LIVE-CONFIRMED s82: player
        #   +0x23A == 0x04, and the live floor-probe result record
        #   *0x700031D0 +0x1A == 0x04 under the player) -> BLOCK base 0x54.
        #   So the snow step block is 0x54..0x62: creep (gait 0/1) 0x54..58,
        #   WALK (gait 2, +5) 0x59..5D, RUN (gait 3, +0xA) 0x5E..62; GAIT_SUB
        #   from actor +0x25C (walk->+5, run->+0xA). rand5 = (rand()&7),
        #   5..7 folded to 0..2. ALL 15 ids resolve to GLOBAL-bank WAVs
        #   (snd_0077..0090), which are always resident -> loaded in AREA-11.
        #   The GEAR layer 0x138..0x13C is the same global cloth set already
        #   shipped via the office preset (so not re-added here; merge keeps
        #   it). NOTE the office floor is attr 0 -> block 0x10 (0x15..0x1E):
        #   the snow block 0x54..0x62 is a DISTINCT, non-colliding material.
        "ids": [0x452, 0x453,            # elevator ascent / descent
                0x1A0, 0x1A1,            # drum/egg burst pair (global)
                0x401, 0x402,            # AREA11 door front / back
                0x454,                   # wedged-truck fall crash (chunk15)
                0x97, 0x99,              # opening-beat music stings (global)
                0x413,                   # steam/FX emitter loop (chunk15)
                0x19A,                   # §11 emitter set one-shot (global)
                # SNOW FOOTSTEPS — surface attr 4, block base 0x54 (global):
                0x54, 0x55, 0x56, 0x57, 0x58,   # creep (gait 0/1)
                0x59, 0x5A, 0x5B, 0x5C, 0x5D,   # WALK  (gait 2, +5)
                0x5E, 0x5F, 0x60, 0x61, 0x62],  # RUN   (gait 3, +0xA)
    },
}


# ---------------------------------------------------------------------------
# FIRST-LEVEL SOUND CENSUS (WP-14, 2026-09-25; the port's
# docs/SFX_REGISTRY_FIRST_LEVEL.md). Every sound id the first level (New
# Game -> the AREA11 opening -> all of AREA11 -> its exit) can request,
# with the evidence for each group. The port's tools/export_sfx_registry.py
# appends these ids to the AREA11 (11.0) scope after the "snow" preset
# above, so the entries that preset already exported keep their bytes.
#
# Evidence kinds:
#   ROUTE   the original requested the id on the recorded first-level route
#           (tools/sfx_request_probe.py: persistent breakpoints on 001FB9F0,
#           001FBD50 and 001FC580 over the startup segment and route beats
#           00..14 in the hidden PCSX2; build/sfx_probe/report_A.json);
#   CONST   a constant id argument at a sound call or tail jump (001FB9F0
#           a0, 001FBD50 / 001FC580 a1, 001FC3C0 a2) of an original function
#           that runs on the route (the route census, build/s87/census), that
#           a first-level port translation binds, or that those reach
#           (calls, installed behaviors, function tables, forwarding thunks:
#           tools/sfx_request_probe.py scan, which fails on a reachable id
#           missing here);
#   DATA    an id computed by original code from data the first level can
#           present (the families below).
#
# 00179B90 is rand5: (rand() & 7), 5..7 folded to 0..2, so a "+ 00179B90"
# id is base..base+4.
#
# Surface families. 00182430 (footsteps) plays block(+23A) + tier sub
# (tier 2: +5, tier 3: +0xA) + rand5, then the gear 0x138 + rand5; 00182870
# (landing, second argument 0 / 1) plays one id per surface. The AREA11
# surfaces that can reach the player's +23A: the grid node attributes below
# 0x1E (0, 3, 4, 5) and 0x5A..0x77 (0x5A, 0x5D) of the captured AREA11 grid
# (3,099 nodes), the static cell kinds below 0x1E of D_0024D7C0[11][0]
# (0, 3, 4, 8, 0xB, 0xD) and the published class-4 owners' +0x54 kinds in
# route beats 00..14 (0, 3, 4, 0xB, 0xD; 0x46 passes neither gate).
# Surfaces 0xB and 0x5D take the default block (surface 0's).
FIRST_LEVEL_AREA = "11.0"
FIRST_LEVEL_SURFACE_BLOCKS = {       # 00182430 block base, 00182870 ids
    0x00: (0x10, (0x1F, 0x20)),      # also 0xB, 0x5D and every unlisted kind
    0x03: (0x43, (0x52, 0x53)),
    0x04: (0x54, (0x63, 0x64)),      # snow (the route's ground)
    0x05: (0x65, (0x74, 0x75)),
    0x08: (0x87, (0x96, 0x97)),
    0x0D: (0xDC, (0xEB, 0xEC)),      # the crates
    0x5A: (0x76, (0x85,)),           # 00182870 plays nothing for arg 1
}


def _run(base: int, count: int = 5) -> tuple[int, ...]:
    return tuple(range(base, base + count))


def _surface_ids() -> tuple[int, ...]:
    out: list[int] = []
    for block, landing in FIRST_LEVEL_SURFACE_BLOCKS.values():
        out += list(_run(block, 15)) + list(landing)
    return tuple(out)


FIRST_LEVEL_GROUPS = (
    ("surface footsteps (00182430) and landings (00182870)", _surface_ids(),
     "DATA: the blocks above; ROUTE: surface 0 (0x1A..0x1F, beat 11), 3 "
     "(0x43..0x4F, beats 02..05), 4 (0x54..0x64), 5 (0x65..0x75), 0xD "
     "(0xDC..0xEC, the crates, beat 05) and 0x5A (0x76..0x84, beat 10)"),
    ("gear layer 0x138 + rand5 (00182430)", _run(0x138),
     "DATA; ROUTE: every walking beat"),
    ("player rand5 families", _run(0x100) + _run(0x109) + _run(0x112) +
     _run(0x11B) + _run(0x124) + _run(0x13F),
     "DATA: 00182AF0 0x100+, 00182A70 0x109+, 0016AE40 0x112+, 00182AB0 "
     "0x11B+, 00169730 0x124+, 0016D130 / 0016DE40 0x13F+ (each id + "
     "00179B90); bound by the port's player closures"),
    ("equipment hit 0018A180", (0x17F, 0x180, 0x181),
     "DATA: 0x180 + (rand() & 1, -1 for a negative odd value)"),
    ("holster table D_00248680[+0x275] (0016F600)",
     (0x168, 0x5DE, 0x5DF, 0x5E0),
     "DATA: the six sub-unit entries (0x168, 0x5DF, 0x5DF, 0x5DE, 0x5DF, "
     "0x5E0)"),
    # CONST, route-executed player code (build/s87/census).
    ("player: climb, drop, ladder, slide, reversal, landing",
     (0x12B, 0x12C, 0xFE, 0xFF, 0x107, 0x10E, 0x10F, 0x123, 0x187, 0x12E,
      0x137, 0x13D, 0x151, 0x152, 0x153, 0x146, 0x159, 0x86, 0xA8),
     "CONST: 00187DC0 (0x86, a tail jump into 001FBD50 from the floor "
     "service 00175900; ROUTE 08, 10) and 00187EA0 (0xA8, a tail jump into "
     "001FB9F0), 00161790 (0x12B/0x12C/0xFE), 00162DB0 (0xFF), 00165B60 / "
     "001662D0 (0x107, 0x10E, 0x10F, 0x123, 0x152/0x153, 0x187, 0xFF), "
     "0016C6A0 / 0016CD70 (the slide loop 0x12E), 0017C030 (0x137), "
     "0017C580 (0x13D, 0x151), 0021C440 (0x152, 0x159), 00224290 / "
     "002243F0 / 00224B80 (0x152, 0x153, 0x146, 0x151)"),
    # CONST, player code bound by the port's first-level closures.
    ("player: hang, ledge, fall, weapon states, damage and death",
     (0x119, 0x120, 0x121, 0x122, 0x134, 0x105, 0x106, 0x14E, 0x14F, 0x186,
      0xCA, 0xDB, 0x162, 0x163, 0x164, 0x165, 0x169, 0x179, 0x17A, 0x17D,
      0x17E, 0x5DC, 0x5DD, 0x12F, 0x147, 0x148, 0x149, 0x14D, 0x150, 0x154,
      0x156, 0x15A),
     "CONST: 001647D0 (0x119), 00168050 (0x120/0x121), 0016D130 (0x122), "
     "0016BC40 (0x134), 0017DFB0 (0x105/0x106), 00163E90 / 0021D490 "
     "(0x14E/0x14F), 001747F0 (0x186), 00187DE0 (0xCA/0xDB), 0016F530 / "
     "0016F600 / 0016FCF0 / 001703E0 / 00170A60 / 001729A0 / 00173000 / "
     "001735C0 / 00173E60 / 0017A970 (0x162..0x17F), the stance workers "
     "00171320..001723D0 (0x17A, 0x5DC, 0x5DD), the damage and death "
     "states 0021C120..002255C0 (0x12F, 0x146..0x15A)"),
    # CONST, route-executed owners, pickups and UI.
    ("AREA11 owners, pickups, panel", (0x19C, 0x19D, 0x19E, 0x19F, 0x1A0,
                                      0x1A1, 0xDB, 0x3EE, 0x3EF, 0x194),
     "CONST: 001551B0 (0x19C, queued 0x19D/0x19E through 001FC580), "
     "00156620 (0xDB, queued 0x19F..0x1A1), 001580C0 / 00157360 (0x3EE), "
     "001575B0 (0x3EF), 00219550 (0x194)"),
    ("AREA11 overlay owners", (0x413, 0x454, 0x455, 0x423, 0x424, 0x425,
                               0x428, 0x426, 0x427, 0x451, 0x19A, 0x452,
                               0x453),
     "CONST: 008235F0 (the flame loop 0x413 through 001FC3C0), 00823FF0 "
     "(the truck 0x454/0x455), 00825940 (0x423..0x425, 0x428), 00827490 "
     "(0x426/0x427), 00827630 (the fan 0x451), 00827B10 (0x19A), 00828050 "
     "(the elevator 0x452/0x453)"),
    ("flame loop service 001E3D90", (0x411, 0x412, 0x413),
     "CONST: 001E3D90 through 001FC3C0 (em_sfx.h)"),
    ("effects with a sound (001EF940)", (0x14A, 0x14B),
     "DATA: the only D_00259C70 records with +0x24 != -1 (0x27: 0x14A, "
     "0x44: 0x14B); the AREA11 table 0x2595B0 has none"),
    ("frame machine, status pages, message presenter",
     (0xC, 0xD, 0xA, 0xB, 0x6, 0x5, 0xE, 0xF, 0x182, 0x0, 0x1, 0x2, 0x4,
      0x8C9),
     "CONST: 001AE040 (0xC/0xD), 0020CDC0 (0xA/0xB/0xD), 0020E0C0 (0xD), "
     "002149F0 (0x6), 0020D930 (0x5), 002160B0 (0xE), 00214020 (0xF), "
     "00211970 (0x182), the cue thunks 0020CD40 / 0020CD60 / 0020CD80 / "
     "0020CDA0 (0x0/0x1/0x2/0x4, tail jumps into 001FB9F0), "
     "001FDDB0 (0x8C9)"),
    # DATA, the room-ambient selector (route: S1, S2, beats 01, 03, 09).
    ("room ambient 001FC280", (0x44F,),
     "DATA: the high half (sra 16; -1 = none) of word +0x20 of record "
     "D_00810702 of D_0024D650[11][0]. AREA11's own table is 4 records "
     "(0..3, all none; AREA13's room table starts 0xC0 after it) and the "
     "route captures hold entries 0 and 2; the probe logged no 001FC280 "
     "request in 11.0. The scan (sfx_request_probe.py) also takes every "
     "constant a reachable function stores to D_00810702 (2..5, 7..10; "
     "their area gating is not proven): those records hold 0x44F, which the "
     "11.0 remap refuses (ABSENT). The forced 0x44E (area 0xB with event "
     "flag 0x30 == 0xFF) is excluded: flag 0x30 is 0 in every capture "
     "through beat 15 (revisit-only)"),
    # CONST, found by the reachability scan (sfx_request_probe.py scan):
    # direct successors and siblings of the states and owners above.
    ("player states 17 and 23 (0015B130 calls every state handler)",
     (0x110, 0x135, 0x136),
     "CONST: 0016AC50, state 17 (0x110; entered from 0015D4C0 surface 0x1E "
     "and from the route's 0021C440, hands off to state 18 0016AE40), and "
     "0016BF80, state 23 (0x136 in its cases 1 and 3, 0x135 in 2 and 3; "
     "entered from state 22 0016BC40 case 3, from 0015B130's special-mode "
     "arm and from 0021C440)"),
    ("gun impact marker 0018ABA0", (0x188, 0x189, 0x18A, 0x18B),
     "CONST: 0018ABA0, the behavior every gun shot's 00186A60 / 001861C0 "
     "installs (the SPR4 tick 00188630): +0x2E bit 0x10 (or the 0x300 flag "
     "arms) selects the pair 0x18A / 0x18B, otherwise 0x188 / 0x189; the "
     "member is (00122BB8() >> 12) & 1 (decomp C)"),
    ("hit application 001B41F0", (0x15B, 0x15D, 0x1B1),
     "CONST: 001B41F0 (0x15D and 0x1B1 through 001FBD50, 0x15B queued "
     "through 001FC580), called by the shot paths 00186A60 / 001861C0 / "
     "00189FE0 and the projectiles 0018AF50 / 0018B3E0"),
    ("debris clatter 001F3620", (0x16A,),
     "CONST through the forwarding thunk 001F02C0 (id = its a1): 001F3620, "
     "the debris piece update 001F40C0 (route) runs per piece: 0x16A for "
     "type 3 (0x5E0 for 7 / 8, 0x5DE for 9 and 0xDB for the splash are "
     "above)"),
    ("SPR4 screen sub-modules", (0x17B,),
     "CONST: 00217090 / 002177B0 / 00217FA0 / 00218640 / 00218D90, the "
     "state 4..8 modules of the status page 00211970"),
    ("passcode keypads 002072C0", (0x8C6, 0x8C7, 0x8C8, 0x8CA, 0x8CB, 0x8CD),
     "CONST: the keypad callback 00207350 that 002072C0 installs (status "
     "pages 4 / 5 of 0020CDC0, opened on the request D_008106C5) and its "
     "helper 002072A0 (0x8CD)"),
)


def first_level_ids() -> tuple[int, ...]:
    """Every census id once, in group order (the exporter's order)."""
    out: list[int] = []
    for _name, ids, _evidence in FIRST_LEVEL_GROUPS:
        out += [i for i in ids if i not in out]
    return tuple(out)


def load_soundmap() -> dict:
    with open(SOUNDMAP) as f:
        return json.load(f)


def sound_key(sid: int) -> str:
    """soundmap `sounds` key format: uppercase, >= 3 hex digits."""
    return f"0x{sid:03X}"


def resolve(sm: dict, sid: int, area: str):
    """id -> (wav_rel, spu_pitch, label, n_events, how) or None."""
    entry = sm["sounds"].get(sound_key(sid))
    if entry is None:
        return None
    label = entry.get("label", "")
    if "events" in entry:                       # global (fixed) id
        ev = entry["events"]
        return ev[0]["wav"], ev[0]["pitch"], label, len(ev), "global"
    # area-tabled / region-dependent: prefer an exact area listing
    for var in entry.get("variants", []):
        if area in var.get("areas", []):
            ev = var["events"]
            return (ev[0]["wav"], ev[0]["pitch"], label, len(ev),
                    f"area {area} ({var['regions'][0]})")
    # fall back to the area's best-coverage region bank
    region = sm["area_scene_map"].get(area, {}).get("region")
    if region:
        for var in entry.get("variants", []):
            if region in var.get("regions", []):
                ev = var["events"]
                return (ev[0]["wav"], ev[0]["pitch"], label, len(ev),
                        f"region {region} (area-map fallback)")
    return None


def elf_read(vaddr: int, n: int) -> bytes:
    """`n` bytes at `vaddr` from the user's local boot ELF."""
    data = BOOT_ELF.read_bytes()
    e_phoff, = struct.unpack_from("<I", data, 0x1C)
    e_phnum, = struct.unpack_from("<H", data, 0x2C)
    for i in range(e_phnum):
        (p_type, p_offset, p_vaddr, _pa, p_filesz, _ms, _fl,
         _al) = struct.unpack_from("<8I", data, e_phoff + i * 32)
        if p_type == 1 and p_vaddr <= vaddr and \
                vaddr + n <= p_vaddr + p_filesz:
            off = vaddr - p_vaddr + p_offset
            return data[off:off + n]
    raise RuntimeError(f"vaddr {vaddr:#x} not covered by any LOAD segment")


def read_door_pairs() -> list[tuple[int, int]]:
    """D_0024DB80 [front, back] pairs from the user's local boot ELF."""
    hw = struct.unpack(f"<{DOOR_PAIR_COUNT * 2}H",
                       elf_read(DOOR_PAIR_VADDR, DOOR_PAIR_COUNT * 4))
    return [(hw[2 * k], hw[2 * k + 1]) for k in range(DOOR_PAIR_COUNT)]


def locked_vo_decode(link: int):
    """func_001BBAE0's locked-door VO resolution for one door LINK
    halfword: (selector, line, duration, voice_cue) — voice_cue is -1
    when the radio line is text-only (every shipped locked-door line;
    see the LOCKED-DOOR \"VO\" block above), or None when the selector
    is out of the 6-case jump-table range (no VO at all)."""
    sel = link & 0x3F
    if sel >= 6:
        return sel, None, None, None
    tgt, = struct.unpack("<I", elf_read(LOCKED_VO_JTBL_VADDR + 4 * sel, 4))
    line = LOCKED_VO_CASE_LINE[tgt]
    table, = struct.unpack("<I", elf_read(GLOBAL_MSG_PTR_VADDR, 4))
    dur, cue = struct.unpack("<Hh", elf_read(table + 8 * line, 4))
    return sel, line, dur, cue


def locked_door_census():
    """Comment lines (+ any lockedvo registry ids) for every lock-gated
    door of the exported scenes: the lock-gate decode + the VO verdict."""
    sys.path.insert(0, str(ROOT / "tools"))
    import placements  # noqa: E402

    notes, vo_ids = [], []
    for label, overlay, vaddr in LOCKED_DOOR_TABLES:
        table = placements.parse_table(
            (ROOT / "extract" / "OVERLAY" / overlay).read_bytes(), vaddr)
        for e in table:
            if (e.spawn_class & 0xFF) != 0x85:
                continue
            gated = ((e.behavior == FN_DOOR_HINGED and e.model == 0x15) or
                     (e.behavior == FN_DOOR_SLIDER and
                      e.flags2 in SLIDER_LOCK_FLAGS2))
            if not gated:
                continue
            sel, line, dur, cue = locked_vo_decode(e.link)
            head = (f"LOCKED door {label}: model {e.model:#04x} id "
                    f"{e.flags2 & 0x7F} link {e.link:#06x} -> VO selector "
                    f"{sel}")
            if line is None:
                notes.append(f"{head} -> out of jtbl range, NO VO")
            elif cue == -1:
                notes.append(f"{head} -> radio line {line} ({dur} frames), "
                             f"voice cue -1 = TEXT-ONLY radio message; no "
                             f"audio id -> no lockedvo scene.txt line")
            else:
                notes.append(f"{head} -> radio line {line} ({dur} frames), "
                             f"VOICE cue {cue} — scene.txt line: lockedvo "
                             f"0x{cue:03X}")
                vo_ids.append(cue)
    return notes, vo_ids


def office_door_pairs(scene: dict):
    """Door placements' link bytes -> their D_0024DB80 pairs.

    Returns (pair_ids_in_order, per_door_info, doorsfx_pair)."""
    sys.path.insert(0, str(ROOT / "tools"))
    import placements  # noqa: E402  (repo tool, same conventions)

    overlay = ROOT / "extract" / "OVERLAY" / scene["overlay"]
    table = placements.parse_table(overlay.read_bytes(),
                                   scene["table_vaddr"])
    pairs = read_door_pairs()
    door_ids: list[int] = []
    info: list[str] = []
    selectors: list[int] = []
    for e in table:
        # class-5 door actors (behavior func_001BC350) with a live link
        if (e.spawn_class & 0xFF) != 0x85 or e.link == 0xFFFF:
            continue
        sel = (e.link >> 8) & 0xFF
        front, back = pairs[sel]
        selectors.append(sel)
        info.append(f"door uid {e.uid_byte:#04x} link {e.link:#06x} -> "
                    f"selector {sel} -> pair [front 0x{front:03X}, "
                    f"back 0x{back:03X}]")
        for sid in (front, back):
            if sid and sid not in door_ids:
                door_ids.append(sid)
    if not door_ids:
        raise RuntimeError("no door placements with links found")
    if len(set(selectors)) > 1:
        info.append("WARNING: doors use DIFFERENT sound-set selectors; "
                    "the port's single global doorsfx line carries only "
                    "the first pair")
    doorsfx = pairs[selectors[0]]
    return door_ids, info, doorsfx


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("ids", nargs="*",
                    help="sound ids (hex like 0x162, or decimal)")
    ap.add_argument("--scene", choices=sorted(SCENES),
                    help="scene preset: its id set, area, and the "
                         "door-pair computation")
    ap.add_argument("--area", default=None,
                    help="area key A.S for area-tabled ids "
                         "(default: the scene preset's, e.g. 2.1)")
    ap.add_argument("-o", "--out", default=None,
                    help="write the registry here (default: stdout)")
    ap.add_argument("--merge", default=None, metavar="REGISTRY",
                    help="append this scene's ids to an EXISTING shared "
                         "registry, preserving its current ids (used to "
                         "fold a second area's bank into the port's single "
                         "assets/sfx/sfx.txt without dropping the first). "
                         "An id already present is left untouched.")
    args = ap.parse_args(argv)

    ids = [int(t, 16) if t.lower().startswith("0x") else int(t)
           for t in args.ids]
    door_info: list[str] = []
    doorsfx = None
    area = args.area

    if args.scene:
        scene = SCENES[args.scene]
        area = area or scene["area"]
        ids = scene["ids"] + [i for i in ids if i not in scene["ids"]]
        door_ids, door_info, doorsfx = office_door_pairs(scene)
        ids += [i for i in door_ids if i not in ids]
        # The locked-door VO census is keyed to the m15 security doors of
        # the office/drawbridge scenes (LOCKED_DOOR_TABLES); only run it
        # for the office preset — AREA-11's opening has no lock-gated door.
        if args.scene == "office":
            vo_notes, vo_ids = locked_door_census()
            door_info += vo_notes
            ids += [i for i in vo_ids if i not in ids]
    if not ids:
        ap.error("no ids (pass ids and/or --scene)")
    if not area:
        ap.error("--area required for area-tabled ids (or use --scene)")

    sm = load_soundmap()
    wav_root = SOUNDMAP.parent

    # MERGE: keep every id already in the target registry (e.g. the office
    # bank) and only add the ones this scene introduces. The shared
    # assets/sfx/sfx.txt is a flat id->wav map, so two areas' banks
    # coexist as long as their ids do not collide; em_sfx looks up by id.
    merge_existing: list[str] = []
    have: set[int] = set()
    if args.merge:
        import re as _re
        for ln in Path(args.merge).read_text().splitlines():
            merge_existing.append(ln)
            m = _re.match(r"\s*0x([0-9A-Fa-f]+)\b", ln)
            if m:
                have.add(int(m.group(1), 16))
        # drop ids the target already carries: faithful no-op for those
        ids = [i for i in ids if i not in have]

    lines = [f"# sfx.txt — generated by tools/gen_sfx_registry.py "
             f"(scene {args.scene or '-'}, area {area})",
             "# <id-hex> <wav-path>; WAVs are this user's own local "
             "decodes (extract/audio_decoded/)."]
    if args.merge:
        lines = [f"# === appended by gen_sfx_registry.py --scene "
                 f"{args.scene or '-'} --area {area} "
                 f"--merge (AREA-11 bank folded into the shared "
                 f"registry; {len(have)} pre-existing id(s) kept) ==="]
    for note in door_info:
        lines.append(f"# {note}")
    if doorsfx:
        lines.append(f"# scene.txt line for the port's em_door: "
                     f"doorsfx 0x{doorsfx[0]:03X} 0x{doorsfx[1]:03X}")

    missing = 0
    for sid in ids:
        r = resolve(sm, sid, area)
        if r is None:
            lines.append(f"# 0x{sid:03X} UNRESOLVED for area {area}")
            print(f"UNRESOLVED: 0x{sid:03X} (area {area})",
                  file=sys.stderr)
            missing += 1
            continue
        wav_rel, pitch, label, n_ev, how = r
        wav = (wav_root / wav_rel).resolve()
        if not wav.is_file():
            lines.append(f"# 0x{sid:03X} WAV MISSING: {wav}")
            print(f"WAV MISSING: 0x{sid:03X} -> {wav}", file=sys.stderr)
            missing += 1
            continue
        lines.append(f"# 0x{sid:03X} {label or '(unlabeled)'} — "
                     f"SPU pitch {pitch}, {n_ev} event(s), {how}")
        lines.append(f"0x{sid:03X} {wav}")

    added = sum(1 for ln in lines if _re_id_line(ln))
    if args.merge:
        # the kept existing registry first, then the freshly-resolved block
        out_lines = list(merge_existing)
        if out_lines and out_lines[-1].strip() != "":
            out_lines.append("")
        out_lines += lines
        lines = out_lines

    text = "\n".join(lines) + "\n"
    if args.out:
        Path(args.out).parent.mkdir(parents=True, exist_ok=True)
        Path(args.out).write_text(text)
        scope = (f"{added} new id(s) appended, {len(have)} kept"
                 if args.merge else f"{len(ids)} id(s)")
        print(f"wrote {args.out} ({scope}, {missing} unresolved)",
              file=sys.stderr)
    else:
        sys.stdout.write(text)
    return 1 if missing else 0


def _re_id_line(ln: str) -> bool:
    import re
    return re.match(r"\s*0x[0-9A-Fa-f]+\s", ln) is not None


if __name__ == "__main__":
    raise SystemExit(main())
