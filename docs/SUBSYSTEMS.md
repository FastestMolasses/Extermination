# Extermination — Subsystem Map & Decomp Roadmap

The navigation map for all semantic-decomp work. Every game function in
`docs/FUNCTIONS.csv` (2,063 rows, vram `0x001305B0–0x00271DF8`) now carries a
`subsystem` label from the cluster inventory below. Built from the full
`jal`/`j` call graph + ELF string cross-references (`tools/callgraph.py`),
vram locality (CodeWarrior links translation units contiguously — adjacent
functions are almost always the same source file), and the named anchors in
FINDINGS/STRUCTS/OVERLAYS.

Project annotation derived from our own call-graph analysis and short
debug-string literals; no disc data is reproduced here.

Regenerate the underlying graph any time:

```
# macOS arm64, repo root
.venv/bin/python tools/callgraph.py build     # -> scratch/callgraph.json
.venv/bin/python tools/callgraph.py verify    # check FUNCTIONS.csv callers col
.venv/bin/python tools/callgraph.py strings   # -> scratch/func_strings.json
```

---

## Headline identifications

| What | Function | Evidence | Confidence |
|------|----------|----------|------------|
| **main()** | `func_001AAE40` (currently misnamed `gs_readback_queue_run`) | the ONLY game function called by `_start` (entry 0x100008); boots IOP modules via 0x001AB1E0 (IRX path strings), then loops; contains the frame-wait poll of `D_00810E98` at 0x1AAFF0 | **high** |
| **Top-level frame loop** | inside `func_001AAE40` | per-frame tail calls: pad read `func_001B5790`, anim tops `anim_frame_top_a/b`, VIF chain heads (0x1D1xxx), audio/IOP service ticks (0x1F9xxx, 0x203350), state machine `sub_EXTERMINATION` | **high** |
| **Game-state machine** | `sub_EXTERMINATION` (0x001D0F20, 682 insns) | called once per frame from main; switch-heavy (two jump tables `jtbl_0026E410/440`); fans out to area-state hub (0x21Bxxx), weapon/HUD (0x1DExxx), render init | **medium-high** |
| **Entity/object process dispatch** | `func_001AFC10` (+ `func_001AF800`, `func_001AFBC0`) | 106 distinct callers across all gameplay code; STRUCTS.md documents it as "process/update an object — the dispatch tail"; sits in the object-manager TU (0x1AFxxx) | **high** |
| **Per-frame entity orchestrators** | `func_0015B130` (43 callees), `func_001662D0` (41), `func_001647D0` (37), `func_00168050` (31) | the widest fan-out functions in the game region; FINDINGS traces `func_0015B130 → func_001612D0 → func_0017C030 → anim_matrix_player` | **high** |
| **Pad input read** | `func_001B5790` → `func_001B5860` (libpad) | calls scePad bank (0x110690+); called once per frame from main; state lands in `D_00810D40/D_00810E40/...`, repackaged by 0x1B5xxx/0x1B6xxx | **high** |
| **Heap / allocator** | `func_002041D0` + sbrk-style helpers (crt_heap cluster) | sole game-region caller of `EndOfHeap`; cluster calls kernel/clib only | **medium-high** |
| **Overlay / area dispatch** | `func_001E7780` (+ outlier call in `func_00195130`) | OVERLAYS.md: reads area id from `D_00810700/701`, 40+-arm beq chain, hardcoded `jal` into 0x00823500 arena | **high** (documented) |
| **Archive streaming** | `func_001E7310` region | references the `\DATA\DATA.DAT;1` filename entry; same TU as the overlay dispatcher | **medium** |
| **CD block read service** | 0x1FExxx cluster | calls the libcdvd BLK-read functions (0x111C28 bank); used by music streamer, savegame code, archive loader | **medium-high** |
| **Music streaming** | `sub_O_STREAM_MUSIC_DAT_1` (0x001FA6E0) | `STREAM/MUSIC.DAT` filename ref; calls CD read + IOP RPC | **high** |
| **Sound effect dispatch** | `func_001FBD50` (play_sound, 213 callers — most-called function in the game) → `func_001FB9F0` (sound-descriptor lookup: bins id, indexes the embedded tables at 0x25Fxxx+ with the area byte `D_00810700`) | readable-C call sites; .s structure | **high / medium** |
| **Savegame build/format** | `sub_BASCUS_97112_DS00_00_EX_DATA_*` (0x228xxx–0x229xxx) | BASCUS-97112 path + `EX_DATA` filename strings, location-name strings for save metadata | **high** |
| **Camera** | NOT yet isolated | no string/SDK anchor; most likely inside `frame_update` (0x15Bxxx–0x165xxx) or `entity_logic`. Find it by locating the writer of the viewport matrix the renderer folds into bone palettes (FINDINGS "runtime-built palette") — trace writers of the per-frame view matrix buffer fed to 0x1D1xxx chain heads | open |
| **Collision** | NOT pinned to one TU | per-bone hull files (id 0x73) imply a hull-vs-world/projectile system. Best candidate: the `level_world` cluster (0x19Axxx–0x1A7xxx): self-contained, near-leaf, scratchpad-batch geometry processors called from entity logic and the 0x1B2/0x1B3 vector code | open (candidate) |

**SDK boundary (confirmed):** everything below `0x00130000` (~951 functions,
not in FUNCTIONS.csv) is non-game library code: sce DMA/GS/VU0 libs
(0x100000–0x102FFF — `dma_kick`, `copy_qw4`, vector utils at 0x1031E0),
libmpeg (0x1033E0–0x10B3FF, error-string named `sub_*`), EE kernel syscall
stubs (0x10B400+), sif/kernel glue, libpad (0x110690+), libcdvd (0x110A00+),
libmc/sifrpc (0x112150+), and C runtime (to 0x12FFFF). Game code proper
starts at `func_001305B0`. Two pieces of Sony library code DO live inside the
game range and are labeled separately: `sdk_gs` (0x22Exxx, sceGs sync/display
— "sSyncPath…" strings) and the MPEG glue in `movie`.

---

## Cluster inventory

Labels appear in the `subsystem` column of FUNCTIONS.csv. "und" =
undecompiled count (the decomp frontier inside that cluster).

| Label | vram range(s) | fns | und | What it is / evidence | Conf |
|-------|---------------|----:|----:|-----------------------|------|
| entity_logic | 0x130–0x15A | 268 | 191 | per-entity-type behaviour state machines (switch on `obj+0x04`, STRUCTS.md); call anim-clip API (0x174), math (0x1B1), play_sound. **Enemy-AI core mapped 2026-06-10 s22** (FINDINGS "ENEMY AI ARCHITECTURE"): generator `0x15A2C0`→spawn helper `0x15A200`→leech brains `0x153F10`/`0x1546C0`; placed crawler `0x1551B0`; fixture `0x156620`; damage helper `0x153B50`; HP at actor`+0x34`, damage mailbox `+0x36` | high |
| frame_update | 0x15B–0x165 | 49 | 36 | per-frame game orchestrators; widest fan-out fns (`func_0015B130` 43 callees, `func_001647D0` 37); FINDINGS player-anim chain runs through here | med-high |
| entity_update | 0x166–0x173 | 43 | 39 | per-frame entity setup/update family (`func_001662D0` 41 callees, `func_00168050` 31) | med |
| actor_anim | 0x174–0x18A | 236 | 138 | entity↔anim glue: clip arbiter/commit (0x1749F0/0x1749A0, 109 callers), motion updaters (0x17D8D0/0x17DE20), `bone_matrix_publish`, `anim_matrix_dispatch/player` | high (core) |
| init_io | 0x18B–0x199 | 64 | 53 | boot/init + IOP module loading: IOPRP20.IMG + PADMAN.IRX (0x18BC20), SNDN2DRV.IRX (0x1916C0), GS/display init TUs (0x18C–0x18D, dma-heavy), area init outlier `func_00195130` (calls into overlay) | med |
| level_world | 0x19A–0x1A7 | 58 | 46 | self-contained world/spatial query library: near-leaf, s16-grid + scratchpad batch processing (e.g. 0x19F1A0 walks 0x70003xxx buffers); hub fns `func_0019A570` (41 callers), `func_0019AD00` (36). Prime **collision** candidate | low-med |
| unknown_01 | 0x1A8–0x1A9 | 20 | 15 | small clib-heavy helpers (string/buffer?) | — |
| frame_main | 0x1AA–0x1AE | 61 | 37 | **main()** + frame-wait loop, IRX RPC boot (0x1AB1E0), `anim_frame_top_a/b`, per-frame VIF chain heads | high |
| entity_sys | 0x1AF–0x1B0 | 42 | 19 | object/process manager: `func_001AFC10` dispatch tail (106 callers) + 0x1B0 actor instantiation (calls bone setup) | med-high |
| math_vector | 0x1B1–0x1B4, 0x1B9–0x1C5 | 162 | 96 | vector/matrix/quat/trig library (`func_001B1470` 157 callers, `func_001B12B0` 87, heading 0x1B1240); a few procedural bone-effect fns relabeled to anim_runtime | high |
| input_io | 0x1B5–0x1B6 | 31 | 17 | pad read wrappers (libpad callers 0x1B5790/0x1B5860), CD-service wrappers (0x1B5940–0x1B6250), config-record unpacker `func_001B5BC0` | high |
| unknown_02 | 0x1B7–0x1B8 | 14 | 12 | calls anim tops + render; possibly skeletal FX/debug-draw | — |
| anim_runtime | 0x1C6–0x1CB (+3 fns in 0x1BD/0x1C0) | 95 | 42 | the named animation core: evaluator, samplers, quat ops, TRS build, bone arrays (see FINDINGS) | high |
| gs_upload | 0x1CC–0x1CE | 23 | 15 | GS-VRAM uploaders (`func_001CCB10/BD0/CC0`, FINDINGS) + texture-upload management | med-high |
| obj_registry | 0x1CF–0x1D0 | 21 | 7 | display-object/asset instantiation: `func_001CFBE0` (62 callers) indexes the embedded directories at `D_00230800+`; `func_001CFA60` object initialiser; also hosts `sub_EXTERMINATION` (state machine) | med |
| render_vif | 0x1D1–0x1DA | 142 | 76 | VIF/GIF packet build + draw walkers: `vif_append_ref_tag`, `skin_arena_init`, `vif_build_unpack_const`, `gs_readback_queue_drain`, per-frame chain heads 0x1D1C50/0x1D2830/0x1D30A0/0x1D7C30 | high |
| weapon_equip | 0x1DB–0x1E6 | 86 | 54 | weapon/equipment system: weapon-name dispatchers (`sub_NIGHT_VISION_SYSTEM`, `sub_SPECIAL_PURPOSE_MISSILE_LAUNCHER`, `sub_DELTA_AUTO_SIGHT_SYS`, `sub_TACTICAL_ADVANCED`); calls draw2d for HUD text | med-high |
| stream_archive | 0x1E7 | 9 | 7 | DATA.DAT/INDEX.IDX archive access (`func_001E7310`) + overlay area dispatcher `func_001E7780` | high |
| hud_objects | 0x1E8–0x1EE | 48 | 46 | builders that mass-create display objects via obj_registry (0x1CF x10–26 per bin); likely HUD/screen furniture | low |
| fx_render | 0x1EF–0x1F8 | 87 | 52 | render-adjacent gameplay visuals (particles/effects?): dma/quat-heavy, `func_001EFD90` (51 callers from entity logic) | low-med |
| unknown_03 | 0x1F9 | 10 | 7 | clib-heavy + per-frame ticks from main (0x1F9780/0x1F9820 touch `D_00282xxx` state) | — |
| stream_music | 0x1FA | 15 | 9 | MUSIC.DAT streamer + IOP RPC | high |
| audio | 0x1FB–0x1FD | 38 | 25 | play_sound + sound-descriptor lookup + voice management | high |
| stream_cd | 0x1FE | 24 | 14 | CD block-read service used by music/save/archive | med-high |
| movie | 0x1FF, 0x206 | 37 | 12 | sceMpeg movie playback glue (decode-error/IOP-memory strings, `sub_sceMpegGetPicture_decode_error`) | high |
| save_memcard | 0x200, 0x228–0x22A | 31 | 19 | memcard service (libmc callers) + savegame build/format (BASCUS-97112 / EX_DATA strings, location names) | high |
| unknown_04 | 0x201–0x202 | 7 | 7 | calls ui_menu_lib sound thunks; sequencer-ish | — |
| iop_services | 0x203 | 21 | 7 | sif/IOP service management (libmc + kernel-heavy; ticked from main) | med |
| crt_heap | 0x204 | 18 | 1 | heap/sbrk (`EndOfHeap` caller `func_002041D0`) + runtime services | med-high |
| unknown_05 | 0x205 | 29 | 2 | small leaf accessors/thunks, mostly matched but unidentified | — |
| draw2d | 0x207–0x208 | 24 | 12 | 2D/UI draw layer: per-slot GIF/DMA descriptor writers (`func_00207D00`, readable), callback-installed entry points (0x207060+) ; used by weapon_equip HUD + all menus | med-high |
| ui_menu_lib | 0x209–0x20C | 18 | 9 | menu framework helpers + UI sound thunks (`func_0020CD40+`, 33/34/27 callers) | med |
| ui_screens | 0x20D–0x218, 0x225–0x227 | 71 | 51 | front-end screens: save/load UI (BASCUS path + EX.ico strings 0x211–0x215), area-name title screens, menus calling draw2d/ui_menu_lib | med |
| unknown_06 | 0x219–0x21A | 8 | 8 | dma/render-heavy; menu 3D background? | — |
| area_state | 0x21B | 23 | 2 | area/level state hub: called by the state machine and by entity logic game-wide; area-name table refs | med |
| area_logic | 0x21C–0x224 | 49 | 35 | per-area gameplay/event logic calling actor_anim + audio + io; references the area-name table | low-med |
| ui_credits | 0x22B | 5 | 4 | credits roll (staff-name strings) | high |
| unknown_07 | 0x22D | 3 | 2 | — | — |
| sdk_gs | 0x22E–0x22F | 6 | 5 | sceGs sync/display-mode library code in the game range | high |
| data | 0x230+, 0x25F–0x261, 0x271DF8 | 67 | 66* | not code: embedded data blobs misdetected as functions (asset/sound directories read by obj_registry and audio; `D_00230800`, `D_0025ECA0`, `D_00264A70`). *Excluded from the decomp frontier.* | high |

Total: 2,063 functions, 1,295 undecompiled. Unknown clusters hold 91
functions (4.4%) — kept numbered rather than guessed.

---

## Highest-leverage undecompiled functions (by distinct callers)

| Callers | Function | Cluster | Size |
|--------:|----------|---------|-----:|
| 106 | func_001AFC10 | entity_sys | 140 B |
| 67 | func_001B17A0 | math_vector | 328 B |
| 62 | func_001CFBE0 | obj_registry | 1024 B |
| 60 | func_00175900 | actor_anim | 1004 B |
| 48 | func_001CFB50 | obj_registry | 140 B |
| 46 | func_001FB9F0 | audio | 596 B |
| 42 | func_00207E40 | draw2d | 308 B |
| 42 | func_001B61C0 | input_io | 132 B |
| 41 | func_0019A570 | level_world | 376 B |
| 40 | anim_advance_time | anim_runtime | 752 B |
| 36 | func_0019AD00 | level_world | 736 B |

---

## Decomp roadmap for the native port (prioritized)

The port already replicates rendering/skinning/animation from the asset side.
What it lacks is the GAME — the loop, the objects, the rules. Decompile in
this order; each step unblocks the next:

1. **frame_main** (61 fns, ~37 und) — `func_001AAE40` main() and its frame
   sequence. This is the skeleton: it fixes the per-frame ORDER of every
   other subsystem and is small. Rename `gs_readback_queue_run` → `main`
   first (symbol_addrs + src rename; follow the 2026-06-01 rename protocol).
2. **entity_sys + obj_registry** (63 fns) — object lifecycle: spawn, link,
   per-frame process (`func_001AFC10`), and instantiation from the embedded
   directories (`func_001CFBE0`). Every entity behaviour hangs off these;
   they also decode how the BSS function-pointer dispatch is installed.
3. **frame_update + entity_update** (92 fns) — the orchestrators
   (`func_0015B130`, `func_001662D0`, `func_001647D0`, `func_00168050`).
   After (2) these read as "for each object: …" and reveal the game-state →
   world-update pipeline, and almost certainly the **camera** writer.
4. **input_io** (31 fns) — small, anchored by libpad; the port needs a pad
   abstraction anyway. Also contains the config-record unpacker family.
5. **actor_anim** core (the ~20 hub fns first: 0x1749A0/0x1749F0 commit
   path, func_00175900) — bridges entity state to the already-understood
   anim_runtime; required for gameplay-correct animation triggering.
6. **level_world** (58 fns) — the collision/world-query candidate. Gameplay
   correctness (walking, shots, triggers) lives or dies here. Verify the
   collision hypothesis early by decompiling `func_0019A570`/`func_0019AD00`
   (41/36 callers) and checking for plane/hull math against the id 0x73
   hull format in FINDINGS.
7. **area_state + stream_archive + the overlay dispatch** (32 fns) — level
   progression and loading; OVERLAYS.md already documents the mechanism.
8. **audio + stream_music + stream_cd** interfaces (top ~15 fns) — the port
   replaces the IOP backends, so only the call interfaces (ids, volumes,
   positions — e.g. `func_001FBD50`/`func_001FB9F0`) need semantics, not
   matching internals.
9. **weapon_equip + draw2d + ui_menu_lib + ui_screens** — the front end.
   draw2d first (24 fns; everything UI calls it).
10. **entity_logic + area_logic last, breadth-wise** (317 fns) — the long
    tail of per-enemy/per-area behaviour. Parallelizable once 1–6 fix the
    shared vocabulary (object struct, dispatch, world queries).

Defer indefinitely for the port: `movie` (replace with a video file player),
`sdk_gs`, `crt_heap` (native heap), `iop_services`, `save_memcard` internals
(replace with native save files; keep the EX_DATA layout for import),
`data` (not code).

### Open questions

- **Camera**: not isolated (see Headline table for the search method).
- **Collision**: CONFIRMED (s7 id-0x44 world + s22 weapon pass):
  `func_0019A570(from, to, mode, mask)` is the segment/ray query — the
  weapon system uses mode 7/mask 0x20 for bullets, mode 1/0x20 for
  actor-hit tests, mode 6 for world LOS; results in scratchpad
  (0x700031B0 point, *0x700031D0 record, *0x700031D4 hit actor).
- **Weapon system** (s22): fully characterized — see FINDINGS "WEAPON
  SYSTEM". The fire/aim state machines live in entity_update
  (0x16FCF0–0x1735C0, player modes 0x1D-0x20), the shot resolution and
  gun-actor behavior in actor_anim (0x185–0x18B: func_00188630
  dispatcher, func_001861C0 bullet, func_001B41F0 hit application),
  and the HUD/equipment-text layer in weapon_equip proper.
- **unknown_01..07** (91 fns): name them as their callers get decompiled.
- The `gs_readback_queue_run` name on 0x001AAE40 predates the main()
  identification and should be retired (the actual readback-queue runner is
  `gs_readback_queue_drain` at 0x001D7410 plus the 0x1AAxxx helpers).

---

### Method notes

Graph: 3,014 deduped nodes (40 stale renamed twins removed), 10,086 call
edges (`jal` + non-local `j` tail calls), built by `tools/callgraph.py` from
the local splat tree. FUNCTIONS.csv `callers` column refreshed from this
graph (the old column missed tail-call edges; 188 rows changed). String
evidence: `%hi/%lo` data refs resolved against the local boot ELF's loadable
range (0x100000–0x275B00); 69 game functions reference ASCII literals.
Cluster boundaries: vram-contiguous TU runs segmented by per-4KB callee-bank
affinity histograms, then labeled from anchors; per-function overrides only
where direct evidence exists (4 rows).
