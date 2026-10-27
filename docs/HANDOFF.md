# HANDOFF — Extermination (PS2) decomp + native port

**Current as of 2026-10-27 (Claude: matching wave, one subagent per NEARMISS function: 39 byte-matched in two batches, MATCHING_GUIDE "Matching wave 2026-10-27", gate below; decomp2 lane round 4, call arities on the first-level set: func_0020E460 byte-matched through a TU-static callee with a jump table (tu_static.py extended), anim_matrix_dispatch / func_0016C6A0 / func_00195130 bodies corrected, gate below; decomp2 lane round 3, argument registers compared per call on the first-level NEARMISS set: 8 byte-matched including the camera-aim actions and the sight switch func_001999C0, three wrong bodies corrected, gate below; decomp2 lane round 1, callee prototypes fixed from the original: func_00214020 and func_001E7570 byte-matched, func_002044F0's body corrected, gate below; decomp lane round 8, declaration reconciliation and call-argument fixes: 5 byte-matched, 3 NEARMISS bodies corrected, gate below; round 7: same-TU static callees via `// TU_STATIC:`, 5 byte-matched; round 6: idiom-32 swept per address on the on-route NEARMISS set, 10 byte-matched; round 5: call-site checks, 3 byte-matched, 16 NEARMISS improved; round 4: 12 DMA-list functions byte-matched; the cut-content and character census branch and the func_001BA8E0 / func_001BA580 matches reviewed and merged; 2026-10-19: Codex's `levels` branch reviewed and merged into port main 789b4b4, and the levels 2-4 state toward the drawbridge, below; earlier: the lossless comparison master demo_level_1019 and the comparison-only IOP tick phase, below; the Discord video re-run demo_level_1018; the timing follow-ups of demo_level_1017; the Discord comparison video demo_level_1017 on port main f8616f9; earlier: the frame-timing findings and the port's fixes, the comparison video's field phase, the PCSX2 fork migration, census 1.69).** This is the short cross-repo entry point.
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
- Original runtime: the agent-debug PCSX2 fork, **the default of every decomp tool
  since 4c563cd** (2026-10-09, the user's decision to retire v2.6.3):
  `tools/pcsx2_session.py` (`ForkSession`) runs it hidden, with exact one-frame
  steps, pad input, memory reads and snapshots, under the shared run lock
  `build/.pcsx2.lock`. States: `build/startup-reference/fork-states/`
  (`manifest.json`; `fork_state("04")`; phase generation by default). The v2.6.3
  app and its 12 slots are retired (macOS Trash, 2026-10-10); `--emulator legacy` no
  longer works, and the v2.6.3 captured data stays on disk.
- The fork itself: `../pcsx2-fork`, entry point `../pcsx2-fork/EXTERMINATION.md`;
  status, rules, the canonical capture chain and the retirement checklist are in
  `docs/PCSX2_FORK.md`. It offers run-to-condition, a per-frame store with
  verified rewind, memwatch logs, inline coverage probes and an emulated-time
  audio tap. Its MCP bridge is the registered server `pcsx2-agent`.
- Port live checks: `EM_STARTUP_TEST=newgame-control` (displacement 9.599849) and the
  level smoke `EM_STARTUP_TEST=newgame-level` + `tools/test_level_smoke.py`. Its phases
  are checked against the route captures.
- Decomp gate: run `tools/decomp/build.py build` fresh, then `tools/verify_all.py`.
  Latest (2026-10-27, fresh build after the matching wave's second batch,
  20 more NEARMISS byte-matched, all compiled_object_ordinary_c): 2295/2299
  units, matched_code 99.38%; boot ELF byte-identical; 19/19 overlays.
  FUNCTIONS.csv: 1963 readable, 643 nearmiss. A NEARMISS listed in
  fill_unmatched.py's GPREL_FORCE_ASM / SIZE_DRIFT_FORCE_ASM keeps linking
  from the .s after its C reaches 100% until its entry is removed (check
  audit_link_provenance after every promotion). Next: the wave's levers
  (declaration order, idiom-32 on scratchpad delay-slot residuals, callee
  definitions, twins) on the rest of the NEARMISS list by %.
  Before it (2026-10-21, fresh build after decomp2 lane round 4, MATCHING_GUIDE
  "Decomp2 lane round 4": func_0020E460 byte-matched, compiled_object_ordinary_c;
  anim_matrix_dispatch 99.55, func_0016C6A0 99.95, func_00195130 88.48 with
  their wrong bodies fixed): 2256/2260 units, matched_code 99.33%; boot ELF
  byte-identical; 19/19 overlays. FUNCTIONS.csv: 1924 readable, 682
  nearmiss. tu_static.py now takes callees with a jump table, so round 8's
  TU-static sweep should be re-run with jump-table callees included. Next
  also: func_001DFF70 (whether func_001DFE70 / func_001D2830 get the switch
  constant and the state byte; the registers hold them, the callees read
  neither), the rest of bulk.json's first-level files from func_0015DF10
  down, the func_001B12B0 argument pair shared by func_0016C6A0 /
  func_00162DB0, and anim_matrix_dispatch's loop back-edge delay slots.
  Before it (2026-10-21, fresh build after decomp2 lane round 3, MATCHING_GUIDE
  "Decomp2 lane round 3": func_001999C0, func_00197490, func_00198AF0,
  func_00198650, func_00198D90, func_001D1AE0, func_001B6BF0, func_00183910
  byte-matched; func_001999C0 / func_00197490 / func_00198AF0 bodies were
  wrong and are fixed): 2255/2259 units, matched_code 99.33%; boot ELF
  byte-identical; 19/19 overlays; all eight compiled_object_ordinary_c.
  FUNCTIONS.csv: 1923 readable, 683 nearmiss. Next: the remaining call
  differences of that comparison (build/decomp2-r3-diff/bulk.json,
  108 files; most are temporaries, check each in the target), the camera
  family's func_00198440 / func_00197870 / func_00198050 / func_00199C50 /
  func_00199DB0 / func_00199FA0 (the `break` / `+=` pack gave nothing).
  Before it (2026-10-20, fresh build after decomp2 lane round 1: callee
  prototypes fixed from the original, MATCHING_GUIDE "Decomp2 lane round 1";
  func_00214020 and func_001E7570 byte-matched, func_002044F0 body
  corrected): 2247/2251 units, matched_code 99.33% (99.3266); boot ELF
  byte-identical; 19/19 overlays; audit_link_provenance shows both as
  compiled_object_ordinary_c. Round 8 before it: 2245/2249, 99.3248.
  `// TU_STATIC:` (round 7) compiles same-TU callees as `static` copies that
  the guarded `tools/decomp/tu_static.py` removes again; the callee must be
  compiled at the caller's `-sdatathreshold` or the guard rejects the copy.
  `docs/FUNCTIONS.csv`'s status
  column is derived by `tools/decomp/reconcile_functions_csv.py` (rerun it
  with `--write` after a fresh build + match stage): 1915 readable, 691
  nearmiss, 334 word, 2 c_partial (func_00140F80 / func_0014AFA0: the
  D_700038A0 literal-vs-relocation expected-object case, NEARMISS.md), no
  c_asm_fallback left. Before diagnosing a NEARMISS, rebuild its expected
  object from the normalized `.s` (build.py
  refreshes only compiled units). Next matching targets: decomp2 round 1 fixed the six
  named callee prototypes (func_001DF9D0, func_00204080 / func_00204090,
  func_001D8130, func_00206B00 / func_00206B10, func_00213F30) and matched
  func_00214020 / func_001E7570; the rest of round 8's failing TU-static
  variants are unchecked (func_00154120, func_00159970, func_001764E0,
  func_0017C860, func_001809B0, func_00187780, func_00193EB0, func_001B3F10,
  func_001B6BF0, func_001C2770, func_001D1AE0, func_001E7D20, func_001F88C0,
  func_001FAFD0, func_002217C0, func_0022A460, func_0022BBC0 and others in
  build/decomp-lane-r8-diff/tusweep.json); new lever: a NEARMISS with a
  byte-matched structural twin matches in the twin's spelling (func_001E7570),
  so sweep twins first; the round-6 scratchpad partial gains
  (func_0019B6C0 / func_0019A910 / func_0019A570 / func_0019AA80,
  func_0015FDF0, func_00177030, func_001916C0). A declaration-order sweep over
  the 11 on-route register-choice-only residuals and permuter runs of up to 15
  minutes on nine of them found nothing.
- Lanes: each owns disjoint files, builds privately and gets an adversarial review. The
  lead commits after an isolated index build (`git checkout-index` into scratch, then
  `make all`) and a leak scan.

## 2026-10-19: toward the drawbridge (levels 2-4), current state
User goal (2026-10-19): before level 2 continues, finish AREA01 and AREA00
up to the AREA01 revisit's cutscene (Roger attacked by the bugs) and the
lowered bridge, every phase tick-exact against the fork recordings
(`build/fork_refs/s87/route_a01`, `route_a00`, `route_a01r`). **Not
reached.** On port main 789b4b4 (Codex's `levels` branch, reviewed
"merge_with_fixes", merged), headless:
- First level unchanged: newgame-control 9.599849, `make
  test-level-smoke-full` and `make test-level-smoke-ps2-drive` PASS.
- AREA01: a01_arrival, a01_00 (781 rows), a01_01 (306), a01_02 (591) exact
  at host speed and with the drive switch; a01_03 exact for rows f0..f956
  of 992 with the switch.
- a01_03 row 957 is the **disc drive's per-read seek time**, not the game
  code (the earlier "voice line ends a frame early" reading is
  superseded): the fork's far seeks to the AREA01 voice data take 6 or 7
  fields read by read (a01_03: 7 reads at 6, 9 at 7), the switch's model
  always serves 6. A private diagnostic build serving the fork's per-read
  values passes all 992 rows (never committed; receipts in the port's
  ignored `build/frontier_i1_20261019_063524/`). a01_04..a01_07 inherit
  the shift; a01_05 row 3836 very likely has the same cause.
- AREA00: arrival aligns, then fault-stops at frame 0 in `00156620`: the
  first-level file `em_area11_boxes.c` holds 32 owner views, the original
  AREA00 arrival needs at least 39 (proposed change in the port's
  LEVEL3_BINDING.md "Requests for main"). Zero AREA00 rows certified.
- The AREA01 revisit (a01r_00..a01r_02): not reached; its 17 owners are
  translated and oracle-checked but unwired (port AREA01_REVISIT.md); no
  revisit harness exists yet.

Left before the bridge, in order: the user's decision on the drive's seek
spread (port LAUNCHER_OPTIONS.md "Decisions for the user to review":
(a) a comparison-only per-read drive replay from fork tables, recommended;
(b) event alignment from the first voiced line; (c) another rule), then
a01_03..a01_07; the owner-view capacity (39+) with its first-level
regression proof; AREA00 arrival, neutral frames, a00_00..a00_10 and
a00_s0 (four unsampled ticks need fork boundary receipts first); a revisit
harness and the binding of the revisit owners; the one continuous run from
New Game to the bridge in both drive modes. Docs: port LEVEL2_BINDING.md /
LEVEL3_BINDING.md / AREA01_REVISIT.md "State (2026-10-19)",
LEVEL_READINESS.md "Native play state", LEVEL_SMOKE.md "Levels 2-4 toward
the drawbridge" (port 479f387). The stale empty run-lock file was renamed
to `build/.pcsx2.lock.stale-file-20261018` (no process held it; the user
is asked to confirm).

## 2026-10-17: the lossless master (demo_level_1019) and the IOP tick phase
User decisions 2026-10-17: a comparison-only port switch
`EM_IOP_TICK_PHASE=N` (port 17c5bf9; off by default, not a player or
launcher option; unset leaves the level smokes' tick logs byte-identical
and newgame-control 9.599849), and the video as **one lossless master**
(`compose --lossless`, decomp 0253d6a: libx264rgb `-qp 0` .mkv, PCM s16le),
the original's sound only, no compressed / Discord copies, no compose
`--subtitle` line (every other text and the game's own subtitles stay).
Made into the ignored `build/video_compare/demo_level_1019/`
(VIDEO_COMPARE.md "Demo run 2026-10-17 (demo_level_1019)"; demo_level_1018
is superseded): a new smoke recording at phase 104 (equal to
`demo_level_p104.rec`), the native playback at 68 (the playback's
equivalent), a new fork pass. **Segment 24 is now 219 / 219 and segments
4-27, the fan room included, are on the same half line** (MAD 0.258, was
0.692; the blades match). The master `demo_level_1019_master.mkv`:
2394x1040, 3,566 frames, 237.971 s, 1.65 GB, lossless check verified.
Open: the AREA01 arrival (segment 28's last frame, segments 29 and 31)
is still one half line apart: the E001 movie-entry field relation, not
the tick phase. A caption must say the port side ran with the disc-drive
switch and tick phase 68 (both off in the Original profile).

## 2026-10-16: the Discord video re-run (demo_level_1018)
Re-made exactly as demo_level_1017 on port main df0720c / decomp 85b1c46
into the ignored `build/video_compare/demo_level_1018/` (VIDEO_COMPARE.md
"Demo run 2026-10-16 (demo_level_1018)"). The smoke checker now PASSES
under the drive switch through a01_arrival (item B fixed). Everything else
is byte-identical to demo_level_1017: recording, both passes' frames and
sound, the full videos and the full-length Discord copy. **Segment 24 is
still 220 / 219 and segments 25-31 (the fan room) still on the other half
line**: item A waits on the user's sound-driver tick-phase decision (port
LAUNCHER_OPTIONS.md); nothing committed changes it. So the Discord excerpt's
third cut is segments 21-23 (160-182 s: the pipe flame, the crevice
cutscene, the jump), not the fan room. If the full-length copy is posted,
its caption must mention the fan-room half-line difference and that the
port side ran with the optional disc-drive switch (`EM_PS2_DISC_DRIVE_TIMING=1`,
off by default). The user's "fix the frame timing first" is therefore only
half met: tell them plainly before posting. demo_level_1017's frames and
audio were deleted; its reports stay (its videos were removed on 2026-10-20,
superseded by demo_level_1019).

## 2026-10-16: the timing follow-ups of demo_level_1017 (committed df0720c, 85b1c46)
Details: VIDEO_COMPARE.md "Timing follow-ups 2026-10-16 (after
demo_level_1017)". Port tools / Makefile / docs and the decomp's
`tools/video_compare/native.py`; the port binary is unchanged.
- **Lead (1), the E001 entry under the drive switch: fixed (a checker
  bug).** The port's `field_rule_port` gave the movie iteration the new
  relation; the tick log samples +0x98 / +0x9C in step E, before the movie
  and W, so that iteration keeps its loop top's (the original's, fork
  probes_1016 movie_end/canon counter 15418). The check now also asserts
  pair == loop top on every tick. `make test-level-smoke-ps2-drive` plays
  through a01_arrival (PASS).
- **Lead (3), native.py's phase check: movie-aware** (3,397 / 3,397 on the
  demo's native pass).
- **Lead (2), segment 24 (220 / 219): open, not a logic bug.** A lane-0
  music refill lands in the voice line's first field on the port and one
  field later on the fork, so the sequencer 001FA0D0 serves the refill
  first on the port and the cutscene ends a field later. The field follows
  the IOP sound driver's tick phase, which no capture records (port
  IOP_STREAM.md item 3). A user decision is listed in the port's
  LAUNCHER_OPTIONS.md (keep it, or a switch-on phase knob fitted to a
  recording). For the Discord re-run, segments 25-31 stay on the other
  half line at delay 0 unless the user decides; the third-cut alternative
  (segments 9-24) stands.

## 2026-10-16: the Discord comparison video (demo_level_1017)
The user's request (2026-10-15): the side-by-side video for the PS2Recomp
Discord after the frame-timing fixes. Made on port main f8616f9 and the
fork, delay 0, into the ignored `build/video_compare/demo_level_1017/`
(details, numbers and the file list: VIDEO_COMPARE.md "Demo run 2026-10-16
(demo_level_1017)").
- Recording from the level smoke's title route (menu tick 15 CROSS, drive
  timing on); the fixture's title inputs and movie START written into the
  pad column (`logs/make_demo_rec_1017.py`); playback bit-identical on all
  15,361 ticks; position and heading bit-exact on all 13,619 paired ticks;
  opening 1,321 / 1,321.
- Videos: two full-quality 1x videos (sound both / original only, 274 MB
  each, 237.9 s) and two Discord copies under 10 MB (46-s excerpt 9.46 MB,
  full length 9.42 MB).
- **Open leads (not fixed here):** (1) the smoke checker fails
  `check_render_context` at the E001 entry tick when the drive-timing
  switch makes the run enter E001 on D_00810E80 = 0 (passes with the
  switch off): the check or the port's field at that iteration; (2)
  segment 24 (east tower) is 220 port ticks against the fork's 219, which
  puts segments 25 to 31 (the fan room, Roger, AREA01) on the other half
  line and likely the fan blades' angle; on port 0eea8b5 (demo_level,
  2026-10-09) it was 218 against 219, so look at 0eea8b5..f8616f9
  (fc986dc's seek, SIFDMA, FIELD); the Discord excerpt's third cut (fan
  room) shows this difference, and a cut from segments 9-24 is offered to
  the user as an alternative; (3) native.py's phase check
  assumes one field relation per run (40 false reports after E001 entered
  on 0); (4) the port's sound is within one tick of the original's either
  way (mostly one tick later).

## 2026-10-16: frame timing in the comparison video (fork probes, port fixes)
The user's goal (2026-10-15) is to stop L1 optimisation and make the
side-by-side video for the PS2Recomp Discord, once the frame timing is
fixed; then L2. "Frame timing" covers every place where the port and the
original fall out of step in time in the video. Probes of the original on
the fork are in the ignored `build/fork_refs/probes_1016/` (PCSX2_FORK.md
"Field timing probes"). The code behaviour is in FINDINGS.md "The field
D_00810E88 across the main loop, movies and empty lists" and "The
sound-bank SIF DMA's first status query".

- **The field.** Outside movies the relation between the field and the
  frame index never changes. Every movie ends on field 1 at host speed, so
  the relation after it follows the frame index at its entry. The old rule
  "the departure FMV leaves frame index == field from every entry phase"
  was the emulator's late teardown RPC in the d4 chain, and is withdrawn
  (PCSX2_FORK.md, GS_DIFF 5.3a corrected).
- **Port FIELD (movies on the vblank clock): implemented in the port tree,
  re-applied on port ded1bcb (MINORS); committed as port f8616f9 (2026-10-16, step
  "Finish the code-exact field model and field-paced movies").** User
  decisions of 2026-10-16: keep the code-exact model and press New Game at
  title menu tick 15 (the New Game movie entered on D_00810E80 = 1 as in
  run d0); first_control's, 11's and 14_roger_encounter's fb2 floors reset
  to their measured exact counts (1,096, 11,225 and 105,947 pixels; 14
  "Yes, reset it": its 26 lost pixels are the port's snow, its rand()
  stream, on the other half line, port GS_EXACT.md 10.4). Done: the live field check from
  both entries (`tools/test_movie_field_live.py`), the d4 chain's 08 / 13 /
  14 on the new phase (`build/fork_refs/pixels/fb2_phase_d4b_1016`, PCSX2_FORK.md),
  the AREA01 title fixed (the message glyph strips drawn as the GS draws
  them: fb2 15 at 99.99 %), the START skip measured on the fork
  (`probes_1016/skip/d0_start_1016c`; FINDINGS.md) and matched, the offline
  audio carrying the movie's PCM, the iOS copies' picture counts. The
  New Game switch test's DWELL allowance for `_s_frame` is 2 words (was 1)
  for the new vblank count em_frame_vblanks (port STARTUP.md). The saved
  diff (on port ded1bcb / decomp a0e50dd): port
  `build/field_final_saved_20261016_184958/`; it supersedes the first
  implementation's `build/field3_saved_20261016_153328/` (on 92786d2), the
  set-aside copy `build/setaside_FIELD3_20261016_164438/` and
  `build/movie_fields_r2/final_054754/`. The decomp's part still to commit
  with it: `tools/fb2_phase_refs.py` (`manifest --root / --port-phase
  press15`) and this file; its FINDINGS / PCSX2_FORK / VIDEO_COMPARE text
  went in with a0e50dd.
- **Port NODRAW (72fee14), committed.** Iterations that draw nothing (the
  status-screen entry pair, the options close) now show what the original
  shows: the buffer's older picture again, or the background, where the
  port used to present black. Open: the old demo's other black frames
  (segment 6 offset 64, segment 12 offset 156) are not verified to be
  empty-list iterations; port GS_EXACT 11 lists the render-context
  follow-ups.
- **Port SIFDMA (92786d2), committed.** With the disc-drive switch on
  only (user decision Q2), the sound bank's first sceSifDmaStat answers
  pending: 9 001FB370 calls in the New Game load, as on the PS2. Open (a
  lead decision): whether to probe the 4 unmeasured 001FB370 calls of the
  level exit's AREA01 bank load on the fork.
- **The video tool** (ccb65ff, next section) reports the half line per
  segment. The 2026-10-09 demo predates port fc986dc and all three fixes
  (VIDEO_COMPARE.md). Segment 24's 218 port ticks against 219 is not
  traced.
- **Done:** FIELD committed (port f8616f9) and the Discord video recorded
  (the section above).

## 2026-10-16: the comparison video reports the field phase
`tools/video_compare/` now logs the half line each side drew every captured frame on
(port: `native/frames.json` `line`/`ofy`, checked against the rule line = 1 - (counter & 1);
fork: `ps2_extra.json` `frame.drawn` from the frame list's draw environments, cross-checked
at run time, `info.drawn_check`), and the drift report gives per-segment half-line agreement
and the frames where either side has no field or one colour. `--fork-title-delay auto` scans
title delays until the New Game movie is entered on the port's frame index; `ps2.py` and
`video_compare.py all` keep delay 0 by default. **Lead decision open:** on the 2026-10-16
test recording `auto` picks delay 1, which puts the whole opening on the other half line
(the fork's first load is 274 iterations, the port's 267); delay 0 agrees 331/331 in the
opening. Pass `--fork-title-delay 0` (the default) for the Discord video until the criterion
is decided. Details and receipts: `docs/VIDEO_COMPARE.md` "The field phase";
`build/video_compare/phase_test/` (keep: the drift reports and one clip).

## 2026-10-16: cut-content census (side task)
A fan-blog claim of a cut teenage-boy subplot and a reporter boss was
checked against the disc: neither was found (static census; limits in
FINDINGS "Cut-content census" sections 3, 8 and 9). The shipped plot does
speak of a son of Falken (AREA16, AREA21; FINDINGS section 2), not tied to
the blog's teenager. The disc does keep Japanese-only
cut audio, including an unused conversation with the journalist
(CURIOSITIES 29, 30, withdrawn-claims note; FINDINGS "Cut-content census";
`tools/cut_content_census.py`). Reviewed and merged into main 2026-10-20. Evidence lives in ignored
`build/cut_content/` of the main checkout.
Follow-up the same day: `tools/character_census.py` names every scripted
actor from the code (001BA8E0 face table, 001BA580 speaker-slot table) and
ties it to its mesh through the load sets; the static one- to four-node
meshes are adult corpses only (FINDINGS "Cut-content census" sections 7-9,
CURIOSITIES 31). The NEARMISS C of 001BA8E0 / 001BA580 was corrected,
and both now byte-match from C as sparse switches (PROGRESS.md 2026-10-16);
001BA8E0's head call order was fixed too (the port already had it right).

## 2026-10-16: func_002061B0 corrected (movie display gate)
The C now has the original's field-wait loop (repeat func_001009C8(0) while it equals
the argument; then the three flag stores) and byte-matches under mwcc 2.3.3 with the
two gp-relative flags volatile; it links from C (PROGRESS.md 2026-10-16). The port's
FIELD step translates it (the gate opens after a field-0 vblank); implemented in the
port tree, not committed yet (frame-timing section above).

## 2026-10-10: the fork migration (PCSX2 v2.6.3 retired)
User decisions of 2026-10-09: migrate everything to the fork, re-record the pixel
references in it with a deterministic field phase, use every slot. Details, receipts and
the retirement checklist: `docs/PCSX2_FORK.md`.
- **The fork is the default** of every decomp tool since 4c563cd (`--emulator legacy` /
  `EXTERMINATION_PCSX2=legacy` only when chosen; since 2026-10-10 also the
  `route_capture` library default).
- **The one-update correction.** All 12 v2.6.3 user slots were saved at the vsync wait
  (PC 0x1AAFF0), after the iteration's game logic; fork states sit at the loop top. The
  2026-10-09 phase states matched the slots' stored (mid-iteration) phase, so chains from
  them ran one game update ahead (`rand()` 20..56 calls ahead at route ends).
  `route_capture.slot_start` now starts every slot-sourced beat or point from the
  loop-top-equivalent state with the v2.6.3 lead-in minus 1, and `fork_states.py` locks
  the phase states to the loop-top phase and scores `rand()` and the player clock. The
  phase states 02/03/04/08/12/14/15 were regenerated (02/03/04/15 equal their slots in
  every compared byte).
- **One canonical chain:** `build/fork_refs/s87/route` (origin `slot04_first_control`,
  lead-in minus 1). Beats 00..14 equal v2.6.3 in every traced field and the end `rand()`
  state, 15 in everything but disc timing (AREA01 load 8 frames longer, arrival row 749).
- **Pad read dropouts, not host-timed stops** (2026-10-10 03:00..05:50). The game's pad
  read sometimes returns nothing for one frame, in both emulators on different frames;
  the fork's are not repeatable. Beat 13's "host-timed stop at f133" was a v2.6.3
  dropout at f130; the "v2.6.3 pad latency" of aim 05/06/07/09, dmg 01 and br 09 was
  the fork's own dropouts (gone on a re-run); level beats' v2.6.3 first inputs often
  came 1..5 frames late. `route_capture.V263_PAD_CORRECTIONS` replays the 12 cases a
  v2.6.3 recording proves (13, c7, br_11, a00_04 dropouts; 8 late first inputs);
  `route_lanes.py` retries a beat with a fork dropout. The phase-correction frame is
  now opt-in (it put beats a tick off after an odd-length load).
- **Route groups** (`tools/route_lanes.py`): first level (64 beats) 36 equal v2.6.3 in
  every field, the end `rand()` state equal in all 64; the rest is disc timing (loader
  read gate, load lengths), the pad driver's bytes after a load, the memory-card record
  (zero on the fork); nothing unexplained (dmg 04's flag is the NEW GAME load's 5-row shift). Level chain: 78 of 161 beats; 37 on
  the v2.6.3 path (`a01`, `a00`, `a01r`, part of `a02`); disc timing (a sub-area's
  objects 7 frames late) takes it off at `a02_02` row 552; the fork's own playthrough
  then runs to `a22b_01` and is blocked at `a04b_00` (Use at door 45 not taken).
- **Pixel references** (`build/fork_refs/pixels/`): fb2 16 of 19 displayed fields
  bit-exact (13..15 re-captured from the corrected chain; the three others are v2.6.3
  post-load hitches), GS conformance 906 of 906 bit-exact plus the new half-pixel
  scissor set `b16/gscap9` (45 tests; v2.6.3 differs exactly where the fork's rule
  says), the elevator refusal message frame (`messages/elevator_refusal`, for
  test_message_capture), the route end frames decoded. Same-phase fb2 points for the
  port (`pixels/fb2_phase`, `tools/fb2_phase_refs.py`; PCSX2_FORK.md "fb2 points on the
  port's field phase"): first_control, 10, 11, 12 on the port's buffer and half line,
  rows and `rand()` equal to the primary points; 15_level_exit on the port's half line
  in the other buffer. The port's exact phase never occurred on the fork. The 2026-10-16
  probes show why: the departure movie ends on field 1 at host speed, and the d4 chain
  left it one field late through the emulator's teardown RPC (section above).
- **2.6.3 retired (2026-10-10).** The port switch is done (port 0faaee4, d11045a, the
  review fixes 750b40f, the fb2 phase references 8b68485): every port test reads the
  fork's recordings by default (`tools/reference_root.py`, `EM_REFERENCE=fork`;
  `EM_REFERENCE=v263` still reads the old data). After a review found no default-mode
  test broken, the lead moved `build/startup-reference/PCSX2.app` and the 12 user slots
  to the macOS Trash. The legacy paths (`--emulator legacy`, `EXTERMINATION_PCSX2=legacy`,
  `c7cap_capture.py lane3 --from boot`, the legacy `ps2.py` options) no longer work; all
  v2.6.3 captured data stays on disk. Checklist: `docs/PCSX2_FORK.md` "Retiring the 2.6.3
  app".
- **What remained:** port side, 15_level_exit's field phase: closed by the FIELD step
  (section above; port GS_EXACT.md 10.4). Fork runs still to make: the full census, sfx_request_probe,
  c7cap stream/rng/lane3 (from the title), fb/h7, load_wait beat 03, EE float vectors,
  the level chain from `a04b_00` (needs policy work on the fork's own playthrough, or a
  way past `a02_02`'s disc timing). The fork's closed loop is not always repeatable (its
  pad read dropouts, finding 1): compare a re-recorded set with its v2.6.3 trace before
  relying on it.

## 2026-10-09: the PCSX2 agent-debug fork, the demos, presentation, coverage
- **The fork** (`../pcsx2-fork`, branch `extermination-mcp`, local only, never pushed):
  upstream PCSX2 `v2.9.114` (`aa7ab4306e26`) plus our DebugServer 0.2.2 and AgentDebug
  engine: run to N vsyncs / ticks / the Nth hit of a PC in one request (about 200
  closed-loop ticks/s, 280 free vsyncs/s), a per-frame store with verified rewind and a
  divergence finder, memwatch logs, inline count/log probes (coverage with no observer
  effect), watch tables, an emulated-time SPU2 tap, GS field / VRAM reads, exact save
  states. x86_64 under Rosetta; Metal Toolchain installed and the fork rebuilt with Metal
  (`36f3e3a50`); unsigned. MCP bridge `pcsx2-agent` registered (local scope, both repos;
  the old Node bridge is no longer registered). Entry point `../pcsx2-fork/EXTERMINATION.md`; decomp
  side `docs/PCSX2_FORK.md`. Clean room: readers of emulator source never write in the
  port or `src/`.
- **Fork fixes today:** `1e22fdc0c` (the launcher passes real paths, so `-elf` boots from
  `$TMPDIR` scratch); `5a3048e8e` = 0.2.2 (presentation off no longer leaks GPU memory and
  aborts; shutdown about 0.5 s instead of 20 s + kill); decomp `184f6f1` (`ForkSession`
  `step(n)` is exactly n frames with `align` False or True). Suites on 0.2.2: 70/70,
  82/82, 21/21, 38/38.
- **Fork states** (decomp `640fac0`): `tools/fork_states.py` regenerated the tools' 24
  states from a cold boot in the fork, matched by game state, in ignored
  `build/startup-reference/fork-states/` (`manifest.json`). Use
  `ForkSession(fork_state("04"))`, `pcsx2_session.py 04`, `route_capture.py run`
  (the fork is the default since 4c563cd; 2026-10-10 section above for the
  corrected phase states). The checklist for retiring the 2.6.3 app is in
  `docs/PCSX2_FORK.md`.
- **GS diff explained** (`docs/PCSX2_FORK_GS_DIFF.md`, 0d0ad2d / bacc01e): fork fields
  differed from v2.6.3 because of the game's frame index D_00810E80 and field phase
  D_00810E88 after a longer load, not the renderer; with the phase matched 1,152 of
  1,173 v2.6.3 captures reproduce bit for bit (the rest: the load-length fade, and
  upstream `4fa2b8e45`'s half-pixel scissor culling, 8 captures). The port roadmap now
  treats the fork as the frame pixel reference (port 94c02a9).
- **Demos** (`docs/VIDEO_COMPARE.md`; ignored `build/video_compare/`): demo_hill (New
  Game to the hill, 4,345 of 4,346 ticks bit-exact; re-rendered after the presentation
  merge and the overlay fix) and demo_level (the whole first level, 14,102 ticks, 13,619
  of 13,620 bit-exact). With the original's sound (decomp `8f9362f`: `ps2.py --emulator
  fork` / `ps2_fork.py`, about 6 min per pass against 44.7 min on v2.6.3):
  `demo_level_sound.mp4` (original left, port right) and `demo_level_original_sound.mp4`.
  The loud sound 14 ticks early in the port is the area music resuming after status
  pages (disc seek timing, port a345d9a, which also fixed the replay WAV's one-field
  stream lag). (The demo_hill, demo_level, demo_level_1017 and demo_level_1018 MP4s
  and the superseded runs' frames were removed on 2026-10-20; demo_level_1019_master.mkv
  is the current video.)
- **Port presentation** (user, "(a) for both"): fields line-doubled to 448 lines with the
  half-line field one line lower, the picture moved by SCREEN ADJUST (port merge 3d403ac);
  the overlay pass now follows the field's line on half-line fields (0eea8b5); the movie
  driver's display position is known (f1e589c).
- **Coverage:** 001755B0 / 0021E9C0 have byte-matched C (decomp bc49270) and are entered
  by side runs (port merge de2b1d4: dmg_fan, dmg_pit_fall; a fan-damage bug fixed);
  census 1.69, all 951 live rows entered by a run (details under "State" below).
- **Open user decisions:** (1) DONE 2026-10-10: the fork is signed with the stable local
  identity "Extermination PCSX2 Fork Local" after every build (fork faeded218); (2) DONE 2026-10-10: `build/startup-reference/PCSX2.app` and the 12 slots
  are in the macOS Trash, after the port switch (`docs/PCSX2_FORK.md` "Retiring the
  2.6.3 app"); (3) DECIDED 2026-10-09 and built (port fc986dc): the port's disc-drive timing
  switch models the first stream read after a module-loader read as a 17-field seek (one
  value fits every captured case; with the switch, first control lands on the original's
  frame, newgame-control locked_ticks 1322).

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
  paths (9; use a separate card file, never the user's own cards). The
  field-presentation and screen-position choices (3) were decided later that day: (a)
  and (a), port merge 3d403ac.
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
- **2026-10-09, the coverage step (001755B0 / 0021E9C0 entered by a run):** decomp
  bc49270 gives both readable C (byte-matched) and fixes their callers' C (func_00175900
  now byte-matched; 0017C580 / 00162DB0 / 001756E0 at 99.94 / 99.95 / 99.74). Port merge
  de2b1d4 (branch cov-2fn): a new side run dmg_fan replays dmg_08 row for row (0021E9C0
  52 times, as recorded) and found a port bug: the fan binding stored the hit's pending
  damage +0x224 in the player record's image instead of g.pd_pend_hp, so the hit dealt
  no damage (fixed); dmg_pit_fall holds the stick through dmg_07's pad lag and compares
  the fall start on rows 309..315 (001755B0 asked at row 313, result 0). Port f8f8019:
  the review's minors (the six history words; the old policy's fall start measured
  with a scratch hook, no longer inferred). Census 1.69: all 951 live rows entered by
  a run, no status change. Still open: 001755B0's two landing sites in 0017C580 need a
  new PCSX2 recording of a full-stick running landing (drop -104..-14.5); the 18
  verified-unbound rows; the unverified 001CB480.
- **Census (port FIRST_LEVEL_CENSUS.md, section 1.69; counts from 1.68):** 951 live /
  18 verified-unbound / 1 unverified (001CB480) / 0 stand-in / 0 missing / 434 boundary
  of 1,404 functions (the route's 1,195 through beat 15, the AIM beats' 114, the DAMAGE
  recordings' 45, the BRANCH recordings' 36 and the OPTIONS recordings' 14); 99.2% of
  non-boundary instructions live (the route's own 768 non-boundary rows: 750 live,
  98.9%); AREA01's 62 functions after the arrival are level 2 (census 3.26).
- **Later levels: paused by the user (2026-10-02, "stop checking ahead").** Levels 2-15 are
  recorded through AREA03's arrival (port SECOND..FIFTEENTH_LEVEL_ROUTE.md, decomp
  WORLD_GRAPH.md), their new functions translated standalone (em_area0x_*, em_level8..14_*)
  and their assets exported; every overlay is in C apart from a few NEARMISS; nothing of
  them is bound. Each level needs its own binding chain later.
- **Decomp:** 2159/2214, boot ELF and 19/19 overlays byte-identical (58ff8de);
  docs/FIRST_LEVEL_DECOMP.md, docs/LEVELS_DECOMP.md.
- **Tools:** side-by-side video comparison (decomp tools/video_compare/, docs/VIDEO_COMPARE.md).
  The PS2 side has the original's sound only on the fork pass (`--emulator fork`); the
  v2.6.3 pass stays silent (PCSX2 2.6.3 records audio only from its UI,
  docs/CAPTURES_AUDIO.md).
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

The `docs/*.md` are committed (`docs/STARTUP.md` and `docs/OPENING_ACTORS.md`
since 2026-10-09, reviewed first, by user decision). The `docs/fanout/*_GUIDE.md`
working references except MATCHING_GUIDE stay untracked on purpose. Check
`git status` and run the no-disassembly guard before staging.

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
or float-local/union variants on func_001796C0: no new match (func_001796C0 was
byte-matched on 2026-10-20 with a `// TU_STATIC:` callee, MATCHING_GUIDE round 7). For func_0016AE40,
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
