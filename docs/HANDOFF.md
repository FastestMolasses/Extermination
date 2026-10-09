# HANDOFF — Extermination (PS2) decomp + native port

**Current as of 2026-10-09 (Claude: the first-level ROUTE step).** This is the short cross-repo entry point.
Below the "MATCHING-WORKFLOW REFERENCE" line is the older byte-matching reference
(compiler, build loop, NEARMISS, idioms, next matching tasks).

Working directory: `/Users/abe/Documents/Extermination.nosync/Extermination` (port:
`../extermination-port`). **Never** the iCloud copy at `~/Documents/Extermination` —
that is a different, stale tree.

## Start here
1. Read both `CLAUDE.md` files (this repo and `../extermination-port`). They hold the
   legal hard rules (never commit or print disc-derived material), the fidelity rules
   ("a label is not evidence"; missing workers fault), the test policy (first-level
   scope, ~10 s default, `EM_TEST_FULL=1` exhaustive, headless) and the cleanup policy.
2. **Goal (user, 2026-09-22/23):** port the whole first snow level (AREA11) completely
   and faithfully, up to Roger, then continue into the next level. Nothing may be
   invented: every behavior is a translation of original code and is verified by an
   original-instruction oracle or an original capture. The decomp's C stays
   byte-identical (`tools/verify_all.py`).
3. **Two port profiles (user, 2026-09-23):** Original (the default, and the only thing
   fidelity work measures: the exact GS framebuffer at 4:3, no smoothing, no CRT) and
   Enhanced (switches over the same logic). See port `docs/PORT_PROFILES.md`.
4. The port roadmap lives in its docs:
   - `docs/FIRST_LEVEL_AUDIT.md`: H-items and WP-0..WP-18 with status.
   - `docs/SCENE_COORDINATOR_DESIGN.md`: S1..S13, lead decisions D2..D5.
   - `docs/ORIGINAL_FRAME_ORDER.md`.
   - `docs/FIRST_LEVEL_ROUTE.md`: the original route to Roger, captured in
     `build/s87/route/` (keep).
   - `docs/EE_FLOAT_MODEL.md`: measured EE/VU0 float rules.

## Verification toolkit
- Original-instruction oracles: port `tools/test_*_reference.py` run the ORIGINAL
  ELF/overlay code in a Python MIPS/VU oracle over captured RAM in
  `build/startup-reference/`. Floats follow port `tools/ee_float_model.py`; the
  recordings are in `build/startup-reference/ee_float/` and the recorders in
  `tools/ee_float/`.
- Original runtime: `tools/pcsx2_session.py` runs PCSX2 hidden, with exact one-frame
  steps, pad input, memory reads and snapshots. Save states are in
  `build/startup-reference/portable-data/sstates/` (01-15 are the user's; never
  overwrite them).
- Port live checks: `EM_STARTUP_TEST=newgame-control` (displacement 9.599849) and the
  level smoke `EM_STARTUP_TEST=newgame-level` + `tools/test_level_smoke.py`. Its phases
  are checked against the route captures.
- Decomp gate: run `tools/decomp/build.py build` fresh, then `tools/verify_all.py`.
  Latest (e627b00, 2026-09-29): 2152/2211 units; boot ELF byte-identical; 19/19 overlays.
- Lanes: each owns disjoint files, builds privately and gets an adversarial review. The
  lead commits after an isolated index build (`git checkout-index` into scratch, then
  `make all`) and a leak scan.

## Paused 2026-10-04 (the user's usage limit) — resume here
- **Level 2 (AREA01) on port main:** Codex's work taken over and merged (6a4ecfe, then
  e2e0d23..ad0ea07 by topic; worktree removed), then chain build/workflows/level2-playable.js
  committed FRAME (8b900b6), FRAME2, NEUTRAL (c377c5f: the AREA01 guard is open, the first
  60 frames match the recording), MOVE (578abc0: movement and collision) and CAMERA
  (8ce3067). Step DRAW (AREA01 drawn, branch level2-draw-wip d764eef) was reviewed
  (approved) and merged on 2026-10-07 (merge 6c9a688; follow-up 1563220: the smoke asserts
  the kind-6 near-fire program draws, first at a01_00 f405). Codex's AREA01 crash sweep
  (branch level2-crash) was merged on 2026-10-07 (merge bbc4a08; follow-ups 9deab55: make
  targets for nine suites, target-hit quick mode, stale counted-gap notes, STARTUP
  re-exports; 6b16556: SECOND_LEVEL_CENSUS §12, 56 live, a01_s3 / a01_s4 pass; 9d221f8);
  worktree and branch removed, receipts in port build/level2-crashes and build/crashmerge.
  **Level-2 check (2026-10-08, port 4207f43..f6922a3):** the full verification set ran
  once (every make test-*, test-level-smoke-full, -ps2-drive): all passed except
  test-area-load-reference (the model bank refused AREA11's relocated table; now seated at
  the loader's word) and four tests whose source lists the level-2 merges broke (repaired).
  Play-testing AREA01 (port docs/LEVEL2_CRASHES.md "Level-2 check") found and fixed five
  reachable faults: shots near a floor field (001A8CE0 translated), the shaft-landing
  stairs (surface 0x35: the EMCL grid axis now carried), Use at the locked shaft door
  (D_002821B0 served from the live message block), shimmying while hanging (001784E0 /
  0017E6E0 / 0017F1C0 / 0017F130 translated, the hang sound bound) and the level exit
  (AREA00 sub 0 in the module pack); also the camera seed's fourth lane (a01_s0's whole
  camera) and the harness's first-command latency (read per recording; so that one
  response's latency is no longer checked independently). Rows equal: a01_00, a01_01,
  a01_02 (stairs), a01_s0 (switch), a01_s3, a01_s4 (switch), a01_s6; a01_03 to f956 (a
  voice line then ends a frame early); census §13: 64 live. Fix round (port, after the
  review, c0f54d5): tools/rand_order.py names em_player_footstep_tick (00187350's wading ripple,
  first drawn at a01_02 f39); before it every run past that frame failed its RNG post-check.
  Exit 0 now: test_level_smoke_area01.py --until a01_00 / a01_01 / a01_02, --side a01_s3 /
  a01_s6, and --side a01_s0 / a01_s4 with EM_PS2_DISC_DRIVE_TIMING=1; --until a01_03 and
  later exit 1 at a01_03 row 957 (port docs/LEVEL_SMOKE.md "Which commands finish green"). The
  exploration fixture (31 cases incl. ledge, shaft, shimmy, crates) completes with no fault
  except a01_07, which stops at the AREA00 load's completion (level 3's assets). Still
  faulting: the save terminal's Yes (00225AC0, module 0x2A memory-card screens: needs a
  user decision on card storage). Next: the AREA00 arrival (level 3's EXIT-style step:
  world texture catalog, shadow receivers, binding), the voice-line end timing under the
  drive switch (a01_03 f957, a01_05 f3836), AREA01 page-load timings (a01_s1/s2/s5), a bug
  hit, then the chain's CHECK step (journal build/workflows/level2-playable.partial.json).
- **Disc repacker (Codex, finished 2026-10-08):** all rounds reviewed and merged
  (a7acd17, 25b390f, c930eeb, 5b6b392): lossless DATA.DAT/INDEX.IDX and ISO rebuild,
  size-changing edits, streams, models, shareable .emmod mod packs (deltas only; the
  builder refuses any 65+ byte run also found on the user's disc), logical title-texture
  upgrades; port integration design in docs/REPACK_PORT.md (not yet implemented). Run
  from this checkout (docs/REPACK.md, MODDING.md); the repack worktree is removed and
  its cited receipts are in build/repack (loose/ and streams/ regenerate via unpack-disc).
- **iOS:** merged into main (841fc99); build and run from the port root with
  tools/ios/build.sh run (docs/IOS.md); free-team signing expires every 7 days.
- **First level:** CAMERAS (c11-t3) merged on 2026-10-08 (port merge a1f7873, follow-up
  62255fa: the scratch-alias test calls the AREA01 forwarders by their merged names).
  Resolution: 0022EEF0's cue and event tracks are em_cinematic_playback_events, then the
  sampler and main's shared post-sample core (AREA11's script host keeps the timeline
  globals, now also D_00275C98); camera actions 9 / 10 / 11 / 14 are bound while the
  camera has no AREA01 worker, AREA01 keeps its forwarding of 001B0300 / 00198D90 /
  001D2830; census section 1.61 (the branch's 1.57) recounted from the rows: live 918 of
  956 (118,433 of 120,964 instructions), route through beat 15 740 of 766. Full
  verification once after it: all 336 other make test-* targets, test-level-smoke-full
  and -ps2-drive PASS (test-area01-scratch-alias after 62255fa); level-2 phases (a01_00..
  a01_02, a01_s3 / s6, a01_s0 / s4 with the drive switch) PASS; tools/ios/build.sh device
  builds. Worktree ../extermination-port-t3 and branch c11-t3 removed. GSFRAME merged
  next (below); then TAKEOVERS, GLUE, OPTIONS, AUDIO, ROUTE.

## State (2026-10-02)
- **First level:** after C9 (PIXELS, AREALOAD, PAGELOADS) and C10/C10b/C10c: EM_NEW_GAME=1
  (3e9af67), the Codex aim/fire branch audited and merged (c899c5c), the aim camera
  (fc46140) and aim/fire live with em_weapon's stand-ins retired (1e6c771). The parallel workflow C11
  (build/workflows/first-level-parallel-c11.js, generated by make_c11.py; journal copy
  first-level-parallel-c11.partial.json) was STOPPED on 2026-10-03 for the user's compute
  limit (tracks in ../extermination-port-t1..t4, branches c11-t1..t4).
  Integrated so far: T2 LIGHTING (d47991a); port c11-t4 77139f4 (merge d788997): no
  first-level asset reads a capture (interaction.emis / background.embg from the disc's
  executed first frame; player.emdl reproduced whole; light cone re-baked); T1 A11FIX
  (merge c1fe03f); T3 AIMCAP (merge 332b27d); T2 UNITS (merge e8031d7): FIRST_LEVEL_AUDIT
  1b item 5 done (the indicator children's 001CACB0 -> 001CABA0 units, the area-title
  node 001C5930 on its own record, the sky grid walked with the grid program's
  translation; re-export effect_tables.emet and area_title.emat); T4 EXIT (port merge
  5682b32, audit 1b item 17): the level exit is live through the AREA01 arrival (the fan
  crossing, Roger's departure 0x828A10 with the movie E001.PSS, 001B0C60(1, 0, 4), the
  AREA01 sub 0 load, 0x1AE040 state 0's rebuild, checked by the level smoke's `exit`
  phase); the first level ends on AREA01's arrival frame (exit_01 f306) and every later
  AREA01 frame fail-stops at 0x1AE040 (level 2). Re-export module_loader/modules.emml,
  sfx_registry.emsr, script_walk_clips.emwc, movies/e001.mov and the AREA01 exports.
  T1 DAMAGE (port c11-t1 64383b0, audit 1b item 13): damage, death, the game over and
  the title after a death are original and replayed against the DAMAGE lane (side runs
  dmg_flame / dmg_crevice_fall / dmg_pit_fall, make test-level-smoke-damage). Re-export
  collision_knockback.emrg (export_collision_contact.py), effect_tables.emet,
  page_textures.emot, module_loader/modules.emml (screen module 0x27) and
  startup/game_over.emui (export_game_over.py). The title's load screen (dmg_05) stays
  missing for the OPTIONS step; the fan's hit (dmg_08) is not replayed yet.
  T1 BRANCHES (merge 3d482f6, audit 1b item 16) is merged too.
- **GSFRAME (c11-t2) merged on 2026-10-08** (port merge be2f04d; follow-ups 5e8884b, eb7c548):
  the Original profile's world frames and the load veil draw through the CPU GS model into
  the 512x224 field (Metal = the Enhanced profile's GPU renderer, EM_GPU_RENDERER=1).
  Resolution: the area consumer also sends AREA01 sub 0's room upload (001FF590(0xAC, 1),
  state 8) to GS memory; fb2 harness keeps both the GS field and main's AREA01 point;
  census section 1.62 (the branch's second 1.58), no status change, totals recounted
  (live 918). Semantic conflict fixed in 5e8884b: the level exit's departure movie faulted
  ("a world frame was recorded but never kicked"); the original's 001D1C10 restarts the
  list after 00203350, so em_frame drops the recorded world draws (em_gfx_gs_world_drop).
  Proof: fb2 full at the merge, AREA11 points unchanged (14 87.18 %, 13 55.92 %, 10
  15.96 %), 15_level_exit 98.57 % (GPU 47.41 %; new GS floor); level-2 phases PASS with
  the GS frame (a01_00..a01_02, a01_s3 / s6, a01_s0 / s4 with the drive switch; an AREA01
  field captured at a01_arrival+420 shows the world). Frame cost (EM_FRAME_TIMING
  newgame-control, load 5..7): in-level wall mean 8.1 ms, p99 12.1, 4-6 of 2,997 steps
  over (start-up, area load, first ticks; GPU 5). AREA01 ticks: 13.6 ms main-thread CPU on
  both renderers, 65 of 840 over the period with the GS frame (GPU 0): level-2 work.
  Re-export: tools/export_gs_memory.py (gs_library.emgm, STARTUP row 69; the game does not
  start without it), export_effect_tables.py then export_area01_water_effects.py
  (D_002531D0). Full verification once after it (receipts port build/gsmerge): all 339 other make test-*
  targets PASS, test-level-smoke-full (with the side, AIM, DAMAGE and BRANCH runs) and
  -ps2-drive PASS, test-fb2-pixels(-area01) PASS, newgame-control 9.599849. tools/ios/build.sh device
  builds. Worktrees ../extermination-port-t1 / -t2 / -t4 and branches c11-t1 / t2 / t4
  removed (all merged). Open (review 2 minor): d3d12 / Vulkan need field presentation
  before the Original profile starts there (today only with EM_GPU_RENDERER=1).
- **The remaining first-level steps, on port main (2026-10-08):** TAKEOVERS (163b5e2:
  every takeover is the player stage's own), GLUE (62e3666: the pad read, 001AFCA0,
  001AB790, 0015CF90 / 001B1190 / 001FC280 on their originals), OPTIONS (903eba7: the
  options screen, the memory-card screen, the title's LOAD GAME; data/memcard/slotN),
  AUDIO (2e5fa30: the sound driver on the game's field clock, the level smoke's sound
  check against the decomp's audio beats). Then **ROUTE** (dd7d000): the full verification set at 2e5fa30 PASS (all 349 make
  test-* targets, test-level-smoke-full through a01_arrival with every side run,
  -ps2-drive, the default target, cutscene skip, fb2 full unchanged, newgame-control
  9.599849, the level-2 phases, tools/ios/build.sh device; receipts port
  build/route_step/full/); one test repaired (test-area11-interaction-host's module
  list lacked OPTIONS' modules since 903eba7); census 1.67 re-measured with the edge
  recorder over the whole route and every side run (44 instrumented runs: 946 of 948
  live rows confirmed by a run, 001755B0 / 0021E9C0 bound but entered by no run; no
  status change); port FIRST_LEVEL_AUDIT section 1b re-made (18 items; status update
  "chain step ROUTE"), LEVEL_SMOKE's relaxed table re-read (every row holds),
  FIDELITY_FEATURES and LAUNCHER_OPTIONS updated. **New finding:** 001FBF50 (with
  001B15D0), 001CB5B0 and 00199C50 run their translations live only in AREA01 after
  the arrival while AREA11 runs something else for the same original (audit 1b item 7:
  one owner each). **Next (no recording needed):** the GS frame's overlay pass, status
  frames, 001DDE10 and the field phase (1b item 2), 001CB480's one owner and the
  duplicate owners (6, 7), the pool's free list (5), the fan's hit dmg_08 as a side run
  (10). **User decisions (2026-10-09):** an audio recording from a visible PCSX2 session
  is allowed (1), and so is using a memory card in PCSX2 for the slot / load / save
  paths (9; use a separate card file, never the user's own cards). Still open: the
  field-presentation and screen-position choices (3; the user asked to see them).
- **2026-10-09 (workflow wf_7fa526cc-b06):** (1) demo video build/video_compare/demo_hill/
  (New Game to the hill slide, 4,734 ticks recorded from the level smoke with
  EM_NEW_GAME=1 + EM_PS2_DISC_DRIVE_TIMING=1; PCSX2 vs port 4,345 of 4,346 ticks
  bit-exact in position and heading; port sound only). The original's audio did not
  record: PCSX2's Media Capture fails "Failed to load FFmpeg" (it needs libavcodec 62 /
  libavformat 62 / libavutil 60 / libswscale 9 / libswresample 6 = Homebrew's ffmpeg 8,
  which it does not find), and its menu items report disabled from background control
  (the End hotkey reaches it); ps2.py --media-audio is ready (074d87b, 4b5ab49). The
  user stopped a full-screen takeover: background control only. (2) presentation
  preview port build/presentation_preview/ (README.txt): field choice (a) is NOT fully
  stable as LAUNCHER_OPTIONS.md says (edges flicker one line per field; only the whole-
  picture bob of (b) is gone), (c) weave is calm when still and combs on motion; SCREEN
  ADJUST is -20..+20 per axis (DX = 636 + 5x, DY = 50 + 2y; direction inferred, not seen).
  (3) 001755B0 / 0021E9C0 explained: 001755B0 is the gait-3 stick-vs-facing test at the
  fall start and the 14.5..104 landing (a 0 gives the running landing, no 5-point hit);
  the dmg_pit_fall side run releases the stick a tick before the recording's 3-row pad
  lag, so its fall start sees gait 0 and never asks it (holding the stick 1-2 more steps
  would enter it; the fall-start rows are in no compared window). 0021E9C0 is fan r2's
  hit reaction (sound 0x154, rumble, clip 0x20 knock-back, 60-frame protection); no run
  is hit by the fan (the exit beat avoids its fast arm): a dmg_fan side run over the
  existing dmg_08 capture would enter it (52 entries). Stale: port PLAYER_FALL.md:447
  and FIRST_LEVEL_AUDIT.md:77 still call test_001755B0 untranslated; the
  em_options_original.c:545 comment swaps the Left / Right pad bits. (4) Cleanup: port
  build/ 13.1 -> 4.6 GiB, decomp build/ 17.5 -> 17.0 GiB (the rest is cited PCSX2
  captures); branch level2-draw-wip deleted; backup-local-work-ac0487b kept (062683f,
  a port-vs-decomp cross-reference tool, is not on main).
- **Census (port FIRST_LEVEL_CENSUS.md, section 1.67, chain step ROUTE):** 948 live /
  19 verified-unbound / 1 unverified (001CB480) / 0 stand-in / 0 missing / 436 boundary
  of 1,404 functions (the route's 1,195 through beat 15, the AIM beats' 114, the DAMAGE
  recordings' 45, the BRANCH recordings' 36 and the OPTIONS recordings' 14); 99.2% of
  non-boundary instructions live (the route's own 766 non-boundary rows: 747 live,
  98.9%); AREA01's 62 functions after the arrival are level 2 (census 3.26).
- **Later levels: paused by the user (2026-10-02, "stop checking ahead").** Levels 2-15 are
  recorded through AREA03's arrival (port SECOND..FIFTEENTH_LEVEL_ROUTE.md, decomp
  WORLD_GRAPH.md), their new functions translated standalone (em_area0x_*, em_level8..14_*)
  and their assets exported; every overlay is in C apart from a few NEARMISS; nothing of
  them is bound. Each level needs its own binding chain later.
- **Decomp:** 2159/2214, boot ELF and 19/19 overlays byte-identical (58ff8de);
  docs/FIRST_LEVEL_DECOMP.md, docs/LEVELS_DECOMP.md.
- **Tools:** side-by-side video comparison (decomp tools/video_compare/, docs/VIDEO_COMPARE.md;
  the PS2 side has no sound: PCSX2 records audio only from its UI, and the visible-session
  attempt was declined at the macOS access dialog, docs/CAPTURES_AUDIO.md).
- **Policy and registries (user, 2026-09-27):** the original code is the oracle;
  hardware timing is not reproduced by default (disc at host speed, no slowdown,
  no CRT); the recorded disc-drive timing (C7 VOICELAT) becomes an optional
  switch, off by default (built: EmSettings.ps2_disc_drive_timing). Port docs/FIDELITY_FEATURES.md lists what the
  port reproduces (evidence + status); port docs/LAUNCHER_OPTIONS.md lists every
  launcher option and the decisions the user still has to review (field
  presentation). Future goals: a launcher; a PS2 compile target for the port's
  game code (test it in the ELF under PCSX2).
- **Decision (user, 2026-09-26): do not integrate ran-j/PS2Recomp** (GPL-3.0 static
  recompiler). Evaluated read-only: host-IEEE EE/VU0 floats, no runtime-overlay support,
  non-GS-exact rasterizer, disc-derived output with per-instruction disassembly comments, and
  linking or copying it would make the port GPL. Do not link, vendor, copy or run it, and do
  not read its runtime sources while writing port hardware code (clean-room).
- **Guards:** tools/check_no_disassembly.py in both repos. The asm bodies stay by the
  user's decision. Never stage while a workflow chain's committer is running (the index
  is shared).
- **Level 2 (AREA01) on the side** (user, 2026-09-25; rules in memory
  `level2-side-track`: new files, decomp overlay sources, ignored build/ only, never the
  port's live files while a first-level chain runs):
  - Phase 1 done: original route recorded (`tools/route_capture.py` group `a01`,
    `build/s87/route_a01/`), census delta (`route_census.py a01-delta`: 154 new
    functions, 89 main line), overview (`tools/area_overview.py`), overlay matching
    (bdd40fb: 33 of 41 byte-identical C). Docs: port `docs/SECOND_LEVEL_ROUTE.md`,
    `docs/AREA01_OVERVIEW.md`; FINDINGS "AREA01 route capture" section.
  - Phase 2 wave 1 done (2026-09-26): port e21bd95 has 90 standalone translations of the
    main-line new functions (overlay 14, math 30, render 21, sys 25) and the AREA01 asset
    exports (assets/area01/), each oracle-tested, not bound; decomp 07c4e32/837d548 corrected
    9 NEARMISS bodies the lanes found wrong. Mutation sweeps were closed out on named
    survivors (they did not converge). Next: the 65 side/exit-only new functions.
  - 2026-09-28: AREA01 overlay at 37 of 41 functions linked from C (3 NEARMISS
    jump-table dispatchers: text and tables byte-identical, blocked only on
    tools/overlay/link_overlay.py placing compiled .rodata at link + 0x40; details
    docs/AREA01_OVERLAY_C.md). New AREA01 side beats a01_s4 (east room, save
    terminal), a01_s5 (control-room duct, healing pickup); the north room is
    unreachable on the first visit (its bridge lowers only on a return).
- **Level 3 = AREA00** (AREA01's shaft door exits to AREA00 sub 0 entry 0), 2026-09-28:
  overlay fully linked from C (33 byte-identical + the entry pad; 0 NEARMISS),
  AREA01's too (41/41; the overlay link now pins compiled jump tables,
  tools/overlay/jt_pin.py). The AREA00 route is recorded to its progression exit
  (a00_00..a00_10, port docs/THIRD_LEVEL_ROUTE.md): raised ferry deck, cab roof,
  container stack, crate tops, a one-way duct into the north-east room, its switch
  (D_0081075D), crates blocking door [51], then back through the shaft door, which
  returns to AREA01 with D_0081075E = 0xFF (the AREA01 bridge lowers: the north
  room and AREA02 lie beyond). Game order so far: AREA11 -> AREA01 -> AREA00 ->
  AREA01 (revisit) -> AREA02. Census: AREA00 runs 141 functions the first two
  levels never ran (76,048 bytes: 128 main line, 13 in the arrival back in
  AREA01); the AREA01 room beats add 20 (AREA01 total new: 174).
- **Level 4 = AREA01 revisit + AREA02** (2026-09-28): recorded to AREA02's exit into
  AREA04 (port docs/FOURTH_LEVEL_ROUTE.md: a01r_00..03, a02_00..05, side beats). The
  revisit plays a new ~6,900-frame cinematic (script 0x82AD90), gives item 0x20 and
  spawns six creatures; the lowered bridge leads to door [16] and AREA02 sub 1.
  AREA02: switch [31] starts a rail car (lethal if you stay on the rails), a ladder
  escape, a ledge climb with bug grabs, battery panel [24], door [25] to AREA04.
  Census: 111 functions new (a02_delta.json). AREA02 overlay: 15 of 17 functions
  link from C (008254E0 NEARMISS only because fill_overlay cannot absorb its
  trailing pad piece), docs/AREA02_OVERLAY.md. AREA00 phase 2 committed (port
  037e8b6). Next: AREA02/revisit phase 2 translations, AREA04 (level 5).
- **Level 5 = AREA04** (2026-09-28): recorded to its exit (port docs/FIFTH_LEVEL_ROUTE.md);
  overlay in C (31 of 32 match; all 31 non-pad functions link from C since the
  fill_overlay multi-.text fix). Level-4 phase 2 committed (port
  em_area02_*, em_area01_revisit*); AREA02 overlay 16/16 non-pad functions link from C.
  Possible next: AREA22 (door [37] path; FIFTH_LEVEL_ROUTE.md open items).
- **Level 6 = AREA22** (2026-09-28): a short connector (overlay = entry pad + one
  init; everything else is boot code); recorded to door [8] -> AREA01 entry 6, the upper
  floor (port docs/SIXTH_LEVEL_ROUTE.md); 11 new functions. Next: AREA01 upper floor ->
  door [19] -> AREA06 (to be established). Item 0x23 (AREA22's card reader key) comes
  from AREA04's NPC behind locked door [45] (not played).
- **Level 7 = AREA01 upper floor + AREA06** (2026-09-28, port docs/SEVENTH_LEVEL_ROUTE.md):
  a running jump crosses the upper-floor gap; door [19] -> AREA06. AREA06: beam, crate,
  room, keypad (sets AREA04 door [45]'s lock bit D_00810845 bit 5), beam collapse into
  the pit; exit door [3] (to AREA16) needs D_00810847 bit 2, whose only writer found is
  AREA15's overlay. Story order after AREA06 is open (AREA04 door [45] -> NPC [2] -> item
  0x23 -> AREA22 reader/door [10], or AREA15 first); no way out of the pit was found.
  AREA06 overlay: 9 of 10 functions link from C (00823B10 NEARMISS, a 12-byte copy).
  Census: a01u 7 new, a06 32 new. AREA22 phase 2 committed.
- **Level 8 = back through AREA06 -> AREA01 -> AREA22 -> AREA04 -> the lift -> AREA13**
  (2026-09-29, port docs/EIGHTH_LEVEL_ROUTE.md; the code-derived door/lock/flag map is
  decomp docs/WORLD_GRAPH.md, each edge marked played or inferred). AREA04's lift button
  [54] (001BC960, BM) also accepts counter 0x12 = 0x10 with D_008106C0 null, so the
  creature fight is not needed; beat a04b_04_lift rides to AREA13 entry 0 (recorded to
  arrival only). Census: a06b 5, a01v 3, a22b 0, a04b 45 new functions. AREA15 and
  AREA16 overlays in C (26/30 and 27/32 byte-identical; 7 NEARMISS in docs/NEARMISS.md);
  the D_00810847 bit-2 writer is AREA15's function at runtime 0x824B40. AREA06 phase 2
  done (port em_area06_port*: 39 rows, 27 translated + 12 reused; AREA06 exports). Three
  decomp C bodies corrected (00219870, 00169250 NEARMISS; 0021A440's prototype), FINDINGS
  "NEARMISS body corrections from the round-7 AREA06 lanes".
- **Cutscene skip crash fixed (port 898a7c2, 2026-09-29):** op18's landing stores -1 in
  player +0x20C and 0015BA50 then reads D_00248C98 row -1 (0.0 in the ELF); the port
  refused row -1. EMCR v2 exports rows -1..458; tools/test_cutscene_skip.py compares the
  opening / beats 0..2 / Roger skips with PCSX2 captures (traces kept in
  build/startup-reference/cutscene_skip/). Users must re-run
  tools/export_player_tables.py. After chain C9 finishes, apply
  build/workflows/skipfix_c9_files_for_lead.patch (Makefile target test-cutscene-skip
  and a FIDELITY_FEATURES entry; reword its beat-0 note: position identical, facing
  one turn step off). Open: the fade substate's 3->2 step is one frame late in the port;
  beat 0's promotion lands 11 frames after arming vs the original's 13.
- **Codex aim/fire work (2026-09-29):** its three commits were audited and brought onto
  port main file by file (c899c5c), then superseded by AIMLIVE (1e6c771); the branch
  codex/aim-fire and its worktree were deleted on 2026-10-09 (user decision).
- **Not started (planned round 8, held for the user's review):** the first level's own
  AREA11 overlay is still mostly assembly in the decomp (23 of 26 functions; the flame,
  the security gun, its cable, the fans) — decompile it and cross-check each function
  against its port translation; AREA13/AREA19 overlays in C; AREA13 capture from the
  lift to its exits; the eighth level's 53 translations. Script:
  build/workflows/levels-8-9-round8.js (not run).
- **Systemic extraction label shift (A22ASSETS finding):** tools/extract_data.py and the
  area exporters label nested-block files without the resident offset (+0x14), so some
  level-zone labels/splits are wrong (bytes are right); to fix across exporters.
    It does not affect the exports the AREA01 arrival reads (port chain C11 EXIT: those
    are byte-checked against the captures).
  - Port flags for binding: the em_actor_cells bit-29 blocker is resolved (port C11
    EXIT: the uid 0 word's bit 29 is the EE uncached RAM mirror, and em_actor_cells now
    accepts area01_cells.bin); em_coll_segment_walkers still returns -1 on 0019D770's
    no-span path (FINDINGS), which the arrival does not reach.
  - Phase 3 (after the first level is done): an AREA01 binding chain.
- **Next:** the Original profile (exact 512x448 GS framebuffer, 4:3) and the
  framebuffer-compare harness, EE-float harmonization of the older oracles.

---
MATCHING-WORKFLOW REFERENCE (§1-§13). Pre-s87 text kept as the reference for the
byte-matching loop; counts refreshed 2026-09-27, other details may lag — re-check
against `tools/verify_all.py` and `docs/PROGRESS.md` before relying on them. The
pre-s87 session-log continuations that used to sit here were cut on 2026-09-27
(git history has them).
---

## 1. What this project is, in one paragraph

A matching decompilation of **Extermination** (PS2, 2001), target **SCUS-97112**. "Matching"
means the C we write must compile to **byte-identical** machine code. The original was built
with **Metrowerks CodeWarrior for PS2** (`MW MIPS C Compiler 2.3.1.01` per `.comment`), with
some Sony SDK objects built by **ee-gcc**. The boot ELF is stripped — no DWARF, no symbol
names — so this is blind matching. 2,953 functions total.

**Current standing (2026-09-27; source counts from `src/`, object counts from the latest
fresh gate in `docs/PROGRESS.md` — re-run `tools/decomp/build.py build` +
`tools/verify_all.py` for live numbers):**

| | count | of 2953 |
|---|---|---|
| compiled units (objdiff units; includes inline-assembly wrappers) | **2211** | 74.9% |
| ...of which objdiff 100% | 2150 (97.2% of units; matched_code 98.60%) | |
| `// NEARMISS` (readable C, original assembly linked) | 727 | 24.6% |
| `// INCLUDE_ASM` stubs (no readable C at all) | 15 | 0.5% |

Ordinary compiled C actually linked is a separate, smaller number (about 1516 slots at
the 2149/2210 checkpoint); `tools/decomp/audit_link_provenance.py` gives the current
figure. Two separate goals, do not conflate them: readable semantic C and compiled-C byte
matching. Object matching is not proof of readable C, and a 100% object is not proof of
a linked compiled-C slot.

---

## 2. THE COMPILER — read this section twice

The period-correct compiler is a **32-bit Windows PE** that cannot run natively on the user's
Apple Silicon Mac. The chain is:

```
Apple `container` (Linux VM, arm64)
  └─ qemu-i386            (x86 emulation)
       └─ wibo32          (minimal Win32 PE loader)
            └─ mwccps2.exe (CodeWarrior MIPS compiler)
```

The MIPS **assembler** and **linker** are arm64-native inside the same container; only the
compiler needs qemu+wibo. `objdiff-cli` runs natively on the host.

### 2.1 Start the container service first

```bash
container system start
```

**Known wart, do not be alarmed:** `container images list` fails with
`Error: Plugin 'container-images' not found` — the CLI (0.12.3) and the installed plugins
(`/usr/local/libexec/container/plugins/`, which provides `container-core-images`) disagree on
a plugin name. **`container run` works fine**, and that is all the build uses. Verify with:

```bash
container run --rm exterm-permuter sh -c 'echo CONTAINER_OK'
```

If the whole daemon is down, `verify_all.py` reports `boot-elf FAIL — link produced no verify
line`. That is a stopped daemon, **not** a broken build. Start the service and re-run.

### 2.2 The image

`IMAGE = "exterm-permuter"` (set in `tools/decomp/build.py`). It is a superset of the older
`exterm-toolchain` image: adds i386 libs for ee-gcc plus decomp-permuter dependencies.
Recipe at `docker/Dockerfile`. Every container call is:

```bash
container run --rm -v <REPO_ROOT>:/work -w /work exterm-permuter sh -c '<script>'
```

### 2.3 The actual compile commands

CodeWarrior (the game code, default):
```bash
qemu-i386 tools/bin/wibo32 tools/mwccps2-233/mwccps2.exe -c <FLAGS> -o build/obj/<name>.o src/<name>.c
```
ee-gcc (Sony SDK functions):
```bash
tools/eegcc/ee-compile.sh src/<name>.c build/obj/<name>.o <FLAGS>
```

### 2.4 Which CodeWarrior builds actually exist

`tools/mwccps2*/` are **user-supplied and gitignored**. Only **three** are installed:

| directive | path | notes |
|---|---|---|
| `mwcc` (default) | `tools/mwccps2/mwccmips.exe` | 2.3, build 991202 |
| `mwcc233` | `tools/mwccps2-233/mwccps2.exe` | 2.3.3 (000906) — **the workhorse**; cracks the idiom-13 delay-slot family |
| `mwcc24` | `tools/mwccps2-24/mwccps2.exe` | 2.4 |

`build.py` also knows `mwcc30` / `mwcc301`, but **those are NOT installed.** Any sweep that
reports them is reporting `n/a`. Do not plan around them.

Matching a function with a *later* CodeWarrior build than 2.3.1 is legitimate — byte-identity
of the loadable region is the only criterion, and `.comment` is not in that region.

### 2.5 Per-file compiler routing

A file selects its own compiler and flags via directives in the **leading comment block**:

```c
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
```

Defaults are `mwcc` and `-O4,p`. `-sdatathreshold` (0 / 4 / 8) matters a lot and is
per-file — getting it wrong silently costs ~0.5%; one function this session measured
99.44 / 99.96 / 99.57 across the three values.

**Historical bug worth knowing:** `file_cflags()` used to stop scanning at the first
non-directive comment line, which silently discarded the declared flags for all 628 near-miss
files (whose first line is the `// NEARMISS` banner). Fixed. If you touch that parser, re-measure.

---

## 3. The build loop

```bash
cd /Users/abe/Documents/Extermination.nosync/Extermination
source .venv/bin/activate          # splat, objdiff bindings, pygltflib live here

python3 tools/decomp/build.py setup   # splat split + asm_fixup + regenerate objdiff.json
python3 tools/decomp/build.py build   # assemble expected/*.o + compile obj/*.o  [container]
python3 tools/verify_all.py           # THE GATE
```

`setup` is only needed when the set of translation units changes (a file flips between
stub/NEARMISS/compiled). `build` alone suffices after editing an existing compiled unit.
A full `setup && build && verify_all` is roughly 4–6 minutes, dominated by the ~130 s link.

`verify_all.py` must print **all six PASS**:

```
boot-elf   PASS  boot ELF byte-identical (0x175b00 loadable bytes)
overlays   PASS  19/19 overlays passed
match      PASS  matched_code 98.60% (floor 95.0), functions 2150/2211
gltf / selftest / gs-offset  PASS
```

**Do not commit unless `boot-elf` says byte-identical.** That is the project's whole point.

### Beware the `matched_code` percentage

`matched_code 98.60%` counts matched over **compiled** units, so near-misses are excluded from
the denominator by construction. Promoting correct functions *lowers* it. It is not the
progress metric. The whole-game object-matching number is **2150 / 2953 = 72.8%**;
`docs/PROGRESS.md` also records the assembly-fallback qualification above.

---

## 4. The `// NEARMISS` mechanism

A `src/<f>.c` whose **first line** starts with `// NEARMISS` is excluded from the
ordinary matched-unit selection. Matching tools can compile and measure it;
the marker alone is not a claim that it never compiles. The linked filler uses
original assembly for these slots. Verify actual link provenance whenever
promoting a candidate; a passing boot ELF alone can hide assembly substitution.

This lets us commit *body-correct readable C* that is not byte-exact — valuable as ground
truth for the native port — without weakening the byte-identity guarantee. Every near-miss
header must record the real re-measured objdiff %, the compiler, and the precise divergence.
Registry: `docs/NEARMISS.md`. Promote one by making it byte-exact and deleting the
marker, so it becomes a normal compiled unit.

---

## 5. THE MOST IMPORTANT LESSON: objdiff 100% is NOT a sound promotion gate

Late in the last session, all 792 near-misses were re-measured through the **canonical**
pipeline (trial-promoted so `build.py`'s `expected` path, including `inject_relocs.py`,
applied — not the scratch harness the sweeps use). Five scored 100.0%. **Only one was real:**

```
KEPT     func_001152D8   boot ELF byte-identical
REVERTED func_0012CAA0   first diff 0x0012cccc  0xc9 vs 0x8e
REVERTED func_00169730   first diff 0x0016987a  0x24 vs 0x25
REVERTED func_0016AE40   first diff 0x0016af4e  0x24 vs 0x25
REVERTED func_001EA240   first diff 0x001ea954  0x80 vs 0xa0
```

**Four false positives out of five**, every one caught only by linking and comparing the ELF.
The failing bytes matched the parked wall descriptions exactly (`0x24` vs `0x25` is the
a0-vs-a1 register colouring; `0x80` vs `0xa0` is the `0x3f80`/`0x41a0` FP-constant ordering).

**Gate every promotion on the boot ELF, bisecting when a batch fails.** Commit `9ac388a` has
the original write-up. Also verify `fill_unmatched.py` actually selects the candidate's
compiled object: GPREL_FORCE_ASM, SIZE_DRIFT_FORCE_ASM, local data, and size drift can
silently replace it with assembly. A passing ELF then says nothing about that C.
This continuation removed a stale force-assembly entry for func_00180850 and kept
func_00135D00 parked on its local jump-table placement blocker. The new source fixes
for func_0012CAA0 and func_001EA240 now pass the actual compiled-C ELF gate.

Corollary: **an agent's reported percentage is not evidence.** Four agent 100%-claims failed
integration across three batches last session. Always re-measure with
`tools/match/integrate_nearmiss.py`, which does its own canonical measurement.

---

## 6. Tooling map

```
tools/decomp/build.py              setup / build / --single-file (objdiff's custom-make hook)
tools/decomp/asm_fixup.py          re-applies VU/endlabel fixups after splat regenerates .s
tools/decomp/inject_relocs.py      relocation injection on the expected-object path
tools/decomp/fill_unmatched.py     linker fill from splat .s; LOCALDATA_FORCED + drift guards
tools/verify_all.py                the six-check gate
tools/match/integrate_nearmiss.py  THE integrator: <wave.json> [MIN%]; re-measures, writes
                                   src/, updates docs/NEARMISS.md. Pass 0 to keep low-% C.
tools/match/clean_registry.py      drops stale/duplicate NEARMISS.md rows after promotions
tools/match/baseline.py            measure named functions through the canonical harness
tools/match/permute.py             drive decomp-permuter (tools/permuter/)
tools/match/*_wave.js              Workflow fan-out scripts (Claude-specific; see §8)
docs/fanout/MATCHING_GUIDE.md      THE compiler-idiom bible — read before any matching work
docs/NEARMISS.md                   near-miss registry (committed)
docs/PROGRESS.md                   standing + roadmap
```

Most `docs/*.md` are committed; a few working references (e.g. `docs/STARTUP.md`,
`docs/OPENING_ACTORS.md`, `docs/fanout/*_GUIDE.md` except MATCHING_GUIDE) are kept
untracked on purpose. Check `git status` and run the no-disassembly guard before staging.

---

## 7. Compiler idioms — read `docs/fanout/MATCHING_GUIDE.md`

31 documented idioms. The latest is **idiom-31**, nonzero integer-to-float staging:
20 staged through an int fixes func_001EA240; four degrees converted/scaled to the
identical float bits fixes func_0012CAA0. Correct callee prototypes also resolved
func_00180850 and the remaining object residual in func_00135D00.

Two useful earlier wins:

**idiom-29 (strength-reduced multiply: fresh vs in-place shift destination).** The original
allocates a **fresh** register for the final shift (`FRESH = src << k; FRESH >>= 15;
src = FRESH + B`); all three installed mwcc builds reuse the dying source register in place.
The fix is a **source spelling**, not a flag: make the multiply and the shift compound
assignments on the *same* variable.

```c
int x = func_00122BB8() >> 16;
x *= K;
x >>= 15;
... = x + B;              /* the + B must be a SEPARATE expression */
```

Folding the multiply into the initialiser (`int x = r * K; x >>= 15;`) does **not** work, nor
does splitting stages across variables. ~50 respellings were tried; do not re-derive them.
This took the whole 7-function family at once (`func_001549C0` went 64.14 → **100**).

**idiom-30.** Hoisting a call out of a float argument reproduces the target's delay-slot `nop`
in the `mtc1`/`mul.s` sequence.

**The generalizable rule from idiom-29:** when a wall has **no matched exemplar anywhere in
the corpus**, it is a *respelling* problem, not a *search* problem. The permuter did not crack
idiom-29 despite it being pure register colouring; a source-spelling sweep did, with corpus
mining supplying the model. Grep the matched corpus before recording any wall.

---

## 8. If you are Codex (or any non-Claude agent): the parallelism story

`tools/match/*_wave.js` are **Claude Code `Workflow` scripts** and will not run for you. They
are still worth reading as specifications — each encodes the prompt, the schema, and the
per-function attack plan that produced last session's results.

The underlying loop is plain and tool-agnostic:

1. Pick candidates (§9).
2. For each: read the splat `.s`, read the current near-miss C, diff against the expected
   object, form a hypothesis about the divergence, respell, re-measure.
3. Sweep the three installed mwcc builds and the `-sdatathreshold` values 0/4/8.
4. Collect results as JSON: `[{"func": ..., "c_source": ..., "wall": ...}, ...]`.
5. `python3 tools/match/integrate_nearmiss.py <that.json>` — it re-measures canonically and
   decides KEEP / NEARMISS / REVERT.
6. `build.py setup && build.py build && verify_all.py`, then commit.

**Run ONE container invocation at a time.** The Apple `container` daemon has wedged under
concurrent `container run` calls in past sessions. Parallelise your *reasoning*, serialise
your *builds*.

---

## 9. Concrete next tasks, highest value first

### (a) Sweep the ≥99% near-miss band — the reliable grind
Current source headers contain 4 functions in 99.9–100%, 14 in 99.5–99.9%, and
27 in 99.0–99.5%, plus func_00135D00 at 100% with the linker-table blocker.
Regenerate rankings from each near-miss header; percentages are object scores.
Historical yield varies. This continuation recovered four verified matches.

Remaining top targets:
```
func_0016AE40  99.98  mwcc233 -O4,p -sdatathreshold 8
func_001F3620  99.92  mwcc233 -O4,p -sdatathreshold 8
func_001796C0  99.91  mwcc233 -O4,p -sdatathreshold 0
func_0010E8A8  99.89  eegcc   -O2
func_001662D0  99.83  mwcc233 -O4,p -sdatathreshold 8
```

Do not repeat the just-tested modulo compound-assignment variants on func_001F3620
or float-local/union variants on func_001796C0: no new match. For func_0016AE40,
the missing actor argument after its pad-mask test is a real source concern, but
explicitly forwarding it adds a reload; same-TU leaf experiments did not solve it.
See docs/FINDINGS.md. Auditing the 212 stale/intentional explicit fallback entries
among objdiff-100% units is another useful lane; trial each through the ELF gate.

**Skip `func_00169730` and `func_002134C0`** — both are genuine walls confirmed by ~31.5k and
~25k permuter iterations, and both were re-confirmed by the ELF test in §5. Do not re-grind them.

### (b) Family hunting — the step-change lever
idiom-29 produced seven improvements from one insight. Look for other idiom classes where
**every** user is a near-miss and **no** matched exemplar exists. That signature is the tell.
Mine `docs/NEARMISS.md` for repeated wall descriptions, then check the matched corpus.

### (c) The `lui`+`sw` scratchpad symbolization gap — diagnosed, NOT yet fixed
splat symbolizes `lui`+`addiu` pairs for `0x7000xxxx` scratchpad addresses but leaves
`lui`+`sw` pairs as raw constants, so the expected object cannot carry the relocation and
`func_001BF6B0` shows a permanent 4-row residual that is **not** a compiler wall — its linked
bytes are already correct.

**Caution, this has bitten twice.** A direct attempt to symbolize those pairs measured
99.9577 → **99.7531**, i.e. worse. The negative result is recorded in `build.py` beside
`_SPAD_SYMS`. Read that note before trying again, and measure the control through the same
harness.

### (d) The 15 remaining `INCLUDE_ASM` stubs — mostly not real
- **8** (`func_0025F0D4`…`func_0025F144`) are **data, not code** — a 36-byte-stride table whose
  words happen to decode as `bltzal`/`bgezal`, with zero references anywhere. splat false
  positives. Suppressing them means editing the segment config; not worth risking byte-identity
  for a counter.
- **`sub_D2_TADR_08x`** is an alias of the already-decompiled `0x00100A60`.
- **`func_001000B0`** is a genuine 2-instruction syscall thunk (it loads syscall number 0x23 and traps).
- **5** (`func_001CE860`, `anim_eval_skeleton`, `func_001F0A60`, `func_001F6FB0`,
  `sub__0000000000000000Inf`) were decoded to readable C last session, but that C **failed to
  compile** and the wave output has since been lost to temp cleanup. **These need re-deriving.**
  They are large VU0/COP2 functions; the C will never byte-match (see §10) — the goal is
  compiling, body-correct, documented C.

So exactly **one** genuinely undecoded function remains, and it is a kernel-call stub.

---

## 10. What can never byte-match, and why

VU0 macro mode (COP2), MMI 128-bit SIMD, and COP0 have **no C spelling under mwcc 2.3**. Those
functions are permanently near-misses; their C is a faithful scalar equivalent with the
divergence documented per file. Do not burn cycles trying to match them.

Related: PS2 floats are **not IEEE-754** (flush-to-zero, saturation, mostly truncating
rounding; division rounds to nearest — measured rules in port `docs/EE_FLOAT_MODEL.md`).
Note the divergence; don't emulate it in the decomp.

One classification trap that cost a lot of time: **the EE byte-add of a register with zero is
mwcc's REGISTER MOVE idiom, not SIMD.** A classifier that reads it as SIMD will invent a "structural ceiling"
that does not exist. Stripping move idioms first collapsed "genuine SIMD" from 30 functions to 1.

---

## 11. Process rules that were learned the expensive way

1. **A label is not evidence — read the definition.** splat marks compiler output
   "handwritten"; thunk headers describe sound cues as "allocations"; twelve callers declare a
   no-argument function with arguments. Every significant win last session came from checking
   the definition against its description.
2. **Measure the control through the same harness before attributing anything.** One
   conversion read as "only 4 of 47 reached 100%" — a failure — until the baseline run showed 3
   of those 4 already matched *unconverted* and 12 others had improved. Same numbers, opposite
   conclusion.
3. **Validate a detector against a known positive before generalising.** A detector that finds
   nothing looks exactly like a clean codebase. One returned zero twice on the very function it
   was written for.
4. **Never size a class by raw pattern count** (20,019 bare `lui`s; 11 real sites).
5. **Run audits at least twice** — round 2 has caught regressions round 1 introduced.
6. **A wrong "fix" is worse than an honest unknown.** Mark port-side choices explicitly rather
   than dressing them as decoded.
7. **Watch for silent no-ops in your own tooling.** `integrate_nearmiss.py`'s REVERT path used
   to fail silently, leaving non-compiling C on disk — invisible downstream, because a
   near-miss is never compiled, so every check including the boot ELF still passed. It now
   verifies its own post-condition. Assume your other scripts have the same class of bug.

---

## 12. There is also a port repo

`/Users/abe/Documents/Extermination.nosync/extermination-port` — a native macOS/Windows/Linux
port (Cocoa+Metal / Win32+D3D12 / X11+Vulkan), clean-room, **zero third-party dependencies**,
no code from any emulator. It consumes this repo's readable C as ground truth.

**Current user priority:** continue PS2 byte matching and readable native C together,
prioritizing a faithful first-level opening and player interaction. The later explicit
checkpoint expands the earlier PS2-only selection. Do not trust existing port behavior
or decomp comments without checking original instructions/runtime evidence.

If the port: `CLAUDE.md` there has a "Verified backlog" section with per-item blockers, and
`tools/xref_decomp.py` joins port annotations to decomp status.


## 13. Latest integration safeguards (2026-09-22 UTC)

Commit d80e74d hardens integrate_nearmiss.py: remove old outputs before compiling,
require command success and fresh objects, reject duplicate function entries,
restore exact original source bytes (including NEARMISS) on build/validation
failure or interruption, and accept the documented direct-list JSON input.
Run `.venv/bin/python3 tools/match/test_integrate_nearmiss.py` (14 synthetic tests).
GNU as pads reference .text sections: known nonzero compiled sizes may be smaller
than the reference section. Exact section-size equality wrongly rejected four real
matches; oversize remains rejected. The linked-ELF/provenance gate is still required.
