# PCSX2 fork against v2.6.3: why the captured GS fields differ

Investigation of 2026-10-09 (GS-diff lane, fork side). It explains the
observation in [PCSX2_FORK.md](PCSX2_FORK.md) "Status": the fork replayed the
demo_hill route with per-tick game state equal to the v2.6.3 run, yet most of
its captured GS fields differed. This page describes behaviour and
measurements only. It quotes no emulator source and no disassembly; upstream
changes are named by commit id and title. Port-side agents may read it.

## The answer in short

1. **The difference was the game's frame and field phase.** The game draws
   each tick into one of two buffers, chosen by a frame index that flips
   every main-loop iteration. It draws with a vertical offset half a field
   row apart, chosen from the GS field bit it samples at every vsync. Which
   buffer and which half line a given tick gets depends on how many
   iterations and vsyncs ran before it, not on the tick's game state.
   In the compat stage's run:
   - The fork's AREA11 load ran 5 more iterations than v2.6.3's (a disc-timing
     difference), which flipped both after the load.
   - The comparison decoded v2.6.3's buffer number, which in the fork held
     the neighbouring tick's picture. The fork's own buffer held the same tick
     drawn half a line lower or higher.
2. **With the phase matched, the frames are equal bit for bit.** Fork runs C
   and D reproduce 1,152 of 1,173 v2.6.3 reference captures exactly: every
   cutscene, the snow and other random sprites, the HUD and the hill.
   - 13 of the other 21 come from the 4-tick longer load: aligned offsets show
     different moments of its fade-in.
   - 8 come from the one renderer difference below.
3. **The renderers are equal except for one upstream change.** For the same
   GS input, v2.6.3 and the fork's base (v2.9.114) write byte-identical GS
   local memory:
   - in 906 conformance tests;
   - in 100 dumped fields of the first level.

   One dumped stretch (the first play segment) differs by 5 or 6 pixels per
   field. The cause is upstream `4fa2b8e45` (2026-05-07), which widens the
   rectangle used to reject primitives lying entirely outside the scissor by
   half a pixel. Alpha-blended vertical line strips there have a segment that
   starts a quarter row below the last field row. v2.6.3 drops that segment;
   the fork draws its first pixel on row 223. Restoring the old rectangle
   makes all 112 dumped fields identical to v2.6.3.
4. **The capture point is the same moment.** In the fork, the direct GS read
   at loop top t+3 equals a save state's GS freeze at that loop top in 1,268
   of 1,268 captures. The game's random-number state is equal at all 4,674
   aligned loop tops in both versions.
5. **The fork's software renderer is an equal pixel reference.** The
   exception is the culling change, which on this route showed in 8 of 1,173
   captures, 5 pixels each. Comparisons must pair captures by tick and by
   field phase, and must decode each run's own drawing buffer (section 8).
6. **A v2.6.3 re-run did not reproduce the v2.6.3 references.** It ran the
   same tool with extra per-tick reads and saves. In 1,148 of 1,174 captures it caught the drawing
   buffer one tick ahead, with the scene drawn but the late 2D writes still
   missing (section 7). The fork's captures were stable across four runs.
   Legacy captures are therefore references of the run that made them, not
   reproducible facts about the game.

## 1. The observation

The compat stage (receipts in the ignored
`build/pcsx2-fork/acceptance/reports/`) cold-booted the fork, drove the title
the way `tools/video_compare/ps2.py` does and replayed `demo_hill.rec`.
- At every 4th tick offset of every segment it read GS local memory at loop
  top t+3.
- It decoded the 512x224 buffer at the frame-buffer pointer (FBP) that the
  v2.6.3 capture of the same (segment, offset) had used.
- It compared the result with that capture's PNG
  (`build/video_compare/demo_hill/ps2/frames/`).

Per-tick game state matched (4,673 aligned rows; only 5 rows of the loader
busy byte differed). The pictures did not:

| compat run | identical | different | identical in the fork's other buffer |
|---|---|---|---|
| run 3 (plain) | 82 | 1,091 | 1 |
| run 2 (a fork state saved on the title screen first) | 36 | 1,137 | 1,070 |

## 2. Method and runs

Everything ran on scratch data under the shared run lock, hidden. Nothing was
written to user data or to the emulator installs. The only user file read was
a byte copy of slot 01, the v2.6.3 title state. Scripts and compact receipts
are in the ignored `build/pcsx2-fork/gsdiff/` (section 10).

| Run | What |
|---|---|
| Fork A, B, C, D | `demo_hill` from a cold boot (details below). |
| v2.6.3 references | `build/video_compare/demo_hill/ps2` and `demo_level/ps2`. Both use `ps2.py` from slot 01, and New Game was committed at different counters (1,574 and 1,462). Their 1,155 shared captures are identical. |
| v2.6.3 re-run | A scratch copy of the legacy app in portable mode on scratch data. It runs the committed `ps2.py` driver, plus per-tick Pine reads of the game's random-number state, frame index and field. Every tick of the four windows is captured. 16 save states are kept (section 7). |
| GS dump runners | Upstream's dump replayer, built from `v2.6.3` and from the fork's base `aa7ab4306` with the fork's build flags (x86_64 Release, multi-ISA). A scratch-only change makes the replayer write all 4 MiB of GS local memory after each field. The runners run portable with no window, on the software renderer. |
| Bisect | Nine runner builds over the 432 upstream commits in `v2.6.3..aa7ab4306` that touch GS code, each with Metal shaders compiled from the same commit. One build of v2.9.114 with only the culling rectangle restored. |

The fork runs use the compat stage's driver. Every capture keeps:
- both 512x224 buffers (FBP 0 and 0x38);
- the GS privileged registers;
- the game's frame index (D_00810E80), field (D_00810E88) and random-number
  state word (0x002426C8) at every loop top.

Every tick is also captured in four windows: segment 2 offsets 180..200,
segment 4 offsets 0..64, segment 11 offsets 0..24 and segment 17 offsets
396..412.

| Fork run | Extra recording |
|---|---|
| A | A save state at every capture (no code-cache clearing), and EE RAM kept at 13 points. |
| B | 5 GS dumps, 100 fields. |
| C | none |
| D | Save states at every capture, and a GS dump in segment 5. |

All four used a real-path scratch base (section 9).

## 3. The renderers

### 3.1 Conformance batches: equal

The 30 designed batches of [GS_CONFORMANCE.md](GS_CONFORMANCE.md) were
replayed. They hold 906 tests: the B16 suite and lane RASTER's probes 3..8,
with 4.6 MB of GIF data. Each batch's packet became a GS dump. Its initial
state was the live v2.6.3 capture's GS freeze, with local memory filled with
0xA5 in one dump and 0x5A in the other. Bytes equal in both fills are the
bytes the batch wrote: 40.0 MB.

| Check | Result |
|---|---|
| v2.6.3 runner against the live v2.6.3 capture, written bytes | identical in 30 of 30 batches |
| v2.9.114 runner against the v2.6.3 runner, all 4 MiB, both fills | identical in 30 of 30 batches |

The first line validates the method and the build: the v2.6.3 runner
reproduces the legacy app's own captures.

### 3.2 The game's own GS stream

| Dumps (fork) | Fields | GIF data | v2.9.114 against v2.6.3 |
|---|---|---|---|
| Run B: load (segment 2), opening fade-in (segment 4, offsets 12..52), status pages (8), elevator console (11), hill (17) | 100 | 150.6 MB | **0 bytes differ** in every field |
| Run D: first play segment (segment 5, offsets 76..84) | 12 | 6.2 MB | **15 or 18 bytes** in every field |

- **Replay against live.** 93 of run B's 100 replayed fields contain a buffer
  that is byte for byte one of the fork's live captures, so the dump runner
  stands in for the live renderer.
- **Where the segment-5 bytes are.** Row 223 of buffer 0x38 (the field's last
  row), at x 428, 430, 432, 434 and 436, plus one pixel at (511, 160) in buffer
  0 differing by one in each colour channel.
- **The primitives there.** Five alpha-blended, Gouraud-shaded vertical line
  strips at x 427.94..435.94 (window coordinates after XYOFFSET). Each has a
  vertex at y 223.25, so its next segment runs from y 223.25 to 231.56,
  almost wholly below the scissor.
- **The two versions.**
  - v2.6.3 rejects that segment whole in its early culling, and row 223 keeps
    the background.
  - The fork lets it through, and its first pixel lights row 223: 5 pixels
    brighter by up to 53 per channel.

### 3.3 The cause, bisected

| Build | Segment-5 dump |
|---|---|
| Last GS commit before `4fa2b8e45` (`fc9097350`) | equal to v2.6.3 |
| `4fa2b8e45` "GS/HW: Fixes/improvements to early scissor/bbox test." (2026-05-07) and every later step | 15 / 18 bytes |
| v2.9.114 with only the culling rectangle restored to its earlier value | equal to v2.6.3 on all 6 dumps (112 fields). The fork's three segment-5 captures then match the reference: 0 px instead of 5. |

The behaviour change: the rectangle used to reject primitives that lie
entirely outside the scissor grew by half a pixel on every side. A primitive
whose bounding box starts less than half a pixel past the last row or column,
and whose rasterization still lights an in-scissor pixel, was dropped by
v2.6.3 and is drawn by the fork. On real hardware the GS has no such
primitive-level rejection, only the per-pixel scissor, so the fork is
probably the closer of the two. That is an inference, not a hardware
measurement.

The conformance suite did not catch this, because no designed test places a
line half a pixel outside the scissor.

### 3.4 What the game uses, and the other upstream changes

A census of the 112 dumped fields covered every GIF tag and A+D write, using
the public register layout.

| Feature | In the dumps |
|---|---|
| Primitive kinds | Triangle strips (2.4 million vertex kicks), triangles, sprites, lines, line strips (7,000), points, a few fans |
| PRIM flags | Gouraud, texture, fog, alpha blend, FST. Context 2 is used by 88 alpha sprites. **No AA1, no FIX.** |
| Textures | PSMCT32, PSMT8 and PSMT4 with CT32 CLUTs, **CSM1 only**. TFX 0/1, TCC 0/1. Bilinear (mostly) and nearest. **No mipmapping (MXL 0).** Clamp modes REPEAT, CLAMP and REGION_CLAMP. |
| Alpha test | ATST NEVER with AFAIL FB_ONLY, ZB_ONLY or RGB_ONLY; GREATER with AREF 0; ALWAYS. **DATE with DATM 1.** |
| Z | Z24 at ZBP 0x70 with GEQUAL or ALWAYS, writes masked in places. Z16 for two small off-screen targets. |
| Blending | Five ALPHA settings (A/B/C/D 0/2/2/1, 0/2/2/2, 0/1/0/1, 1/2/2/2, 1/0/2/2; FIX 0x80 or 0). COLCLAMP 1. DTHE 0, PABE 0, FBA 0, SCANMSK 0. |
| Transfers | **Host to local only** (TRXDIR 0). No local-to-local copies. |
| Targets | The two 512x224 fields (FBP 0 and 0x38) with XYOFFSET (1792, 1936.0 / 1936.5). Off-screen 256- and 128-wide targets at FBP 0x12C with XYOFFSET (1920, 1920) and (1984, 1984). |

The other upstream classes that touch the software renderer or the shared GS
state change no pixel here, as measured above.

| Change class (upstream commits) | Why it does not show here |
|---|---|
| AA1 coverage and bounding boxes (`84c0cd889`, `9afea55bb`, `433614ae8`) | The game never sets AA1. `9afea55bb`'s new draw bounding box was also tested alone: restoring the old one changes nothing. |
| C rasterizer colour packing (`3dd702830`) | Affects the C path. These x86_64 builds use the code generator. |
| Draw buffering and the vertex-queue rework (`76c401b0f`, `1fa3c6aa4`, `7887919e7`, `b2fa00844`, `99cfbb49c`, `3b2962fe1`, `20721b05f`, `051fdd55b`, `9945046a4`, `5c611f85e`, `d88510e3a`) | Draw buffering is an option, off unless manual user hacks or a GameDB entry turn it on; this game's entry does not. The always-on queue code gave identical output. |
| Local-to-local transfer fixes (`92adacf99`, `0244cde98`, `ca47a0888`) and the CSM2 CLUT case (`45fa6a8bb`) | The game uses neither. |
| Horizontal-line early exit (`5f3dc1a76`), code-generator constant layout (`a291936bf`, `9b343bae3`, `1160758a8`), dirty registers (`75187d69c`), texture min/max (`4e1975ec8`), overlap check (`bf94ee1c8`) | Identical output on every test and field. |
| Large-ST rewrite (`77bec5e0f`, `ac24959c4`) | A hardware-renderer option. |
| Presentation changes: anti-blur, PCRTC offsets and deinterlacing, BGCOLOR with both circuits off, CRTC checks (`b7fa45ee7`, `2f3006a57`, `16fde28c3`, `ed515782f`, `4e6b6904c`, `77236c594`, `daa0d2d3c`, `1b4af9dee`, `08d3cc32f`) | They change the picture PCSX2 presents, never GS local memory. The fork's field reads and every capture tool decode local memory directly. |

The GameDB entry for SCUS-97112 is the same in both versions.

## 4. The capture moment

- **Both tools read at the same loop top.** `ps2.py` saves a state at
  main-loop top t+3 and decodes the GS freeze's drawing FRAME buffer. The fork
  drivers read `gs_vram` at the same loop top. Both flush the GS first.
- **The fork's direct read equals a save state.** In run A a save state was
  taken right after each read. Its GS local memory equals the read in
  **1,268 of 1,268** captures.
- **The fork's frames are reproducible.** Run A has compat run 3's phase and
  reproduced its split (82 identical, 1,091 different) exactly. Runs C and D
  (same phase) agree in both buffers of all 1,267 captures. Runs B and C differ only in frame-index
  phase after the load, and their buffers hold the same pictures swapped
  (1,136 captures).
- **The registers at a capture loop top.** All 1,268 captures of run A show
  one of two states:

  | Drawing FRAME | OFY | Frame index D_00810E80 | Field D_00810E88 | CSR FIELD | DISPFB2 | VIF1 DMA (D1) |
  |---|---|---|---|---|---|---|
  | FBP 0x38 | 1936.5 | 1 | 0 | 0 | FBP 0 | active, TADR 0x28F710 |
  | FBP 0 | 1936.0 | 0 | 1 | 1 | FBP 0x38 | active, TADR 0x293710 |

  This is the AREA11 pairing of [CAPTURES_C7.md](CAPTURES_C7.md) 5b. The
  drawing FRAME is the buffer DISPFB2 does not show. Decoded, it is the field
  built from tick t. The DMA for the next field is active at the head of its
  chain.

## 5. The frame and field phase

### 5.1 The game's rule

This is decomp knowledge ([FINDINGS.md](FINDINGS.md) "Vsync ISR",
[CAPTURES_C7.md](CAPTURES_C7.md) 5b), confirmed by the table above:

- The vsync interrupt handler samples CSR FIELD into D_00810E88 at every
  vsync.
- The main loop flips the frame index D_00810E80 once per iteration.
- The next draw environment takes OFY 1936.5 for field 0 and 1936.0 for
  field 1, and its buffer from the frame index.

So the buffer follows the iteration count and the half line follows the vsync
count. An iteration that does not take exactly one vsync, or any extra
iteration before the compared point, shifts one phase or both. The game state
stays the same.

### 5.2 Measured

During the AREA11 load every iteration took exactly one vsync in both
emulators; the fork simply ran more of them.

| Run | New Game commit (counter, game vsync) | Frame index, field at commit | Load segment (ticks) | Loader busy (ticks) | Frame index vs reference after the load | Field vs reference after the load | Own buffer equal to the reference |
|---|---|---|---|---|---|---|---|
| v2.6.3 demo_hill (reference) | 1,574, 11,435 | 0, 1 (inferred from C) | 268 | 186 | | | |
| v2.6.3 demo_level | 1,462, 11,323 | | 268 | 186 | same | same | 1,155 of 1,155 |
| fork A | 1,474, 11,373 | 0, 1 | 273 | 191 | other | other | 83 of 1,173 (static frames) |
| fork B | 1,474, 11,372 | 0, 0 | 273 | 191 | other | same | 1,106 of 1,173 |
| fork C | 1,474, 11,367 | 0, 1 | 272 | 190 | same | same | **1,152 of 1,173** |
| fork D | 1,474, 11,367 | 0, 1 | 272 | 190 | same | same | **1,152 of 1,173** |
| v2.6.3 re-run | 1,467, 11,330 | 1, 0 | 269 | 187 | same | same | 26 of 1,174 (section 7) |

- **The commit phase comes from the New Game movie skip.** Both tools press
  START when a host-timed poll sees the movie flag, so the vsync of the
  commit, and with it the field phase, varies by run: 11,367 / 11,372 /
  11,373 / 11,377 in fork cold boots.
- **The load adds the rest.** An even number of extra load iterations keeps
  both phases. An odd number flips both.
- **The load length is emulator timing.** The fork's loader stays busy 190 or
  191 iterations against 186 or 187 in v2.6.3. Compat run 2 fits the same
  rule: its title-screen save moved the commit by 17 vsyncs (odd), and the
  odd load then flipped both phases.

What the two mismatches do to a comparison:
- **Frame index only.** The tick is in the other buffer, and decoding the
  other run's buffer number gives the neighbouring tick. The differences
  follow horizontal edges in panning shots. This is what the compat stage
  measured.
- **Field.** The same tick is drawn half a field row higher or lower. Every
  edge and vertical gradient changes: 1,161 to 100,812 pixels per frame. The
  difference correlates with the vertical gradient (0.58 to 0.69 in the
  opening and on the hill) and barely with the horizontal one.

### 5.3 The half-line test

Each of run B's dumps was copied with every drawing offset moved to the other
half line. That covers the A+D writes to XYOFFSET_1/_2 and the two contexts in
the dump's initial state (OFY 1936.0 <-> 1936.5); nothing else changed. Both
copies were replayed, and the buffer holding each captured tick was compared
with the v2.6.3 reference frame of the same (segment, offset). Black fade
frames are left out, because they match either way.

| Dump (run B) | Run B field vs reference there | As recorded = reference | Other half line = reference |
|---|---|---|---|
| Segment 2, offsets 100..108 | other | 0 of 3 (10,851..11,357 px differ) | **3 of 3** |
| Segment 4, offsets 32..52 | same | **6 of 6** | 0 of 6 (1,161..67,311 px) |
| Segment 8, offsets 4..12 | same | **3 of 3** | 0 of 3 |
| Segment 11, offsets 0..12 | same | **4 of 4** | 0 of 4 |
| Segment 17, offsets 400..412 | same | **4 of 4** | 0 of 4 |

The half-line choice is the whole difference between frames of equal ticks.

### 5.4 Random-seeded effects

The random-number state word was compared at every aligned loop top:

- fork runs A, B and C against each other: equal at every aligned loop top
  (4,677 to 4,678 per pair);
- fork run C against the v2.6.3 re-run: equal at 4,674 of 4,674.

It first changes at segment 3 offset 1, the area entry, in all runs. The
phase-matched fork frames reproduce the reference's snow and the other
random sprites exactly. No masking was needed.

## 6. Residuals at matched phase

In run C (and the same in run D), 21 of 1,173 captures differ.

- **Segment 2, offsets 188..236 (13 captures, 26..2,508 px).** The fork's
  loader is busy 4 iterations longer (offsets 1..190 against 1..186), so the
  post-load fade starts 4 ticks later. Aligned offsets show different moments
  of the fade. This is the 5-row loader-byte difference the compat stage
  reported. It is timing, not rendering.
- **Segment 5, offsets 68..96 (8 captures, 5 px each).** This is the culling
  change of section 3.3, on row 223 below the lamp's thin vertical cables.

## 7. The v2.6.3 re-run

A copy of the legacy app replayed demo_hill with the committed `ps2.py`
driver. It added a few Pine reads per tick (the random-number state, the
frame counter, frame index and field, and two GS register reads that return
CSR), and save states at every tick of the four windows.

- Its game state equals the reference in all 4,673 aligned rows; only one
  loader-byte row differs, from its 269-tick load.
- Its frame index and field after the load equal the reference's.
- Yet its frames match the reference in only 26 of 1,174 captures.

At its capture loop tops (PC 0x001AAF28 in all 16 kept states), the game's
VIF1 DMA for the next field had already finished: D1 CHCR 0x70000005, with
TADR at the end of the chain. The drawing buffer therefore already held the
**next** tick's scene in rows 32..191, with the late 2D writes (letterbox
rows, fade) still missing.

In the kept states of fork runs A and D, the same DMA is active at the head of
its chain at the loop top (CHCR 0x30010105, TADR 0x28F710 or 0x293710).
[CAPTURES_C7.md](CAPTURES_C7.md) 5b recorded the same in v2.6.3 route
snapshots, and the reference frames are complete.

The cause was not traced. The legacy build runs VU1 on its own thread, and
the re-run's extra per-tick host work is the only intended difference. The
measured consequence: a v2.6.3 capture depends on the session that made it.
The two references agree with each other because they were made the same
way. The fork's captures were stable across all four runs.

## 8. What this means

**For pixel references (the port's Original profile).**
- The fork's software renderer can replace v2.6.3 as the pixel reference.
  For identical GS input it is byte-identical except for the half-pixel
  culling change of section 3.3.
- That change decides lines or points that start within half a pixel
  outside the scissor. On this route it showed in 8 of 1,173 captures,
  5 pixels each.
- The port's documented rules (`docs/GS_EXACT.md` 3.6 line coverage, scissor
  bounds inclusive) predict the fork's pixel there, not v2.6.3's. A port-side
  agent should confirm this and record which reference the port follows for
  such primitives.
- Nothing has to stay on v2.6.3 for renderer reasons.

**Pairing captures.** Equal tick numbers are not enough. Two captures are
comparable only when:
1. They show the same tick (game state). The tools already align by segment
   and offset.
2. They have the same field phase: the same OFY, or the same D_00810E88 at
   the tick's drawing. Otherwise every edge moves by half a line, which is
   the port's "frame loop's phase" of `docs/GS_EXACT.md` 10.1.
3. Each run's own drawing buffer is decoded: the FRAME of the GS freeze, or
   the buffer DISPFB2 does not show. Never decode the other run's buffer
   number.
4. The field was complete at the capture point. The VIF1 DMA should be active
   at the head of its chain (0x28F710 or 0x293710), not finished; section 7
   shows a run where it was not.

Capture tools should record per capture: the frame index D_00810E80, the
field D_00810E88, the drawing FRAME and XYOFFSET, and D1 CHCR and TADR. The
fork's run A recorded them (one non-exact save per capture); v2.6.3 save
states hold them too.

**Timing references.** Load lengths and anything else measured in main-loop
iterations or vsyncs are emulator-version specific. The AREA11 loader runs
4 or 5 iterations longer in the fork. The movie skip in both tools moves the
field phase at random from run to run. A capture tool that needs a given
phase should press START at a fixed emulated vsync, which the fork's run
control allows, or record the phase and pair by it.

**v2.6.3 references already made** (C7, route captures, demo_hill and
demo_level) are valid frames of their own runs. demo_hill and demo_level
agree with each other, and phase-matched fork runs reproduce 1,152 of 1,173 of
them exactly. A fork re-capture of the same route gives the same frames as
long as it lands in the same phase.

**Re-recorded (2026-10-10).** The fork's pixel references are in
`build/fork_refs/pixels/`, and the results are in
[PCSX2_FORK.md](PCSX2_FORK.md), "Pixel references re-recorded on the fork".
The fb2 points match bit for bit at 14 of 19, and the conformance tests at
906 of 906. Matching the phase was not enough on its own: the chain also has
to start on the same game update. User slots saved at the vsync wait are one
update earlier than a loop-top state with the same bytes (PCSX2_FORK.md,
"Correction (2026-10-10)").

## 9. Also found: ELF-override boots fail on a symlinked scratch path

A fork launch with `elf=` and the default scratch base `$TMPDIR/...` failed to
boot the game: "Failed to read ELF being loaded: host:SCUS_971.12", after
which the EE ran at address 0. On macOS `$TMPDIR` lies under `/var`, a symlink
to `/private/var`. The emulator roots the `host:` device at the ELF folder's
real path and allows the ELF only when that path equals the `-elf` argument as
given. With HostFs off, the load was refused. The compat stage worked because
its scratch was under `/private/tmp`, which is already a real path.

Reported to the lead on 2026-10-09, and **fixed in the fork's launcher**:
fork commit `1e22fdc0c`, "launcher: pass real paths for the scratch and
-elf", resolves both paths before building the command line. A launcher older
than that commit needs a real-path scratch base, such as
`os.path.realpath(TMPDIR)`.

## 10. Reproduce and receipts

Receipts and scripts are in the ignored `build/pcsx2-fork/gsdiff/` (nothing
disc-derived is committed):

| Receipt | Contents |
|---|---|
| `conf_report.json` | the 30 conformance batches, both checks |
| `dump_report_B.json`, `dump_report_D.json` | per dump and field: bytes differing between the runners, matches to live captures |
| `flip_report.json` | the half-line test |
| `census.json` | the GS feature census |
| `bisect.log` | the bisect steps |
| `runs.json` | per run: commit, phases at commit, load length |
| `own_buffer_vs_reference.txt` | fork runs A..D, each segment: own buffer and other buffer against the reference |
| `legacy_rerun_vs_reference.txt`, `crossmatch_C_vs_legacy_rerun.json` | the v2.6.3 re-run against the reference, and against fork run C (phases and random-number state) |
| `scripts/` | the drivers and analyses (Python, our code) and the description of the scratch-only runner change |

To repeat:
- **Fork runs.** `scripts/fork_replay.py` (based on the compat stage's
  `t2_fresh.py`), with a real-path scratch base, under the run lock. Set
  `GSD_CAPSTATES=1` for per-capture states and `GSD_DUMPS` for dumps.
- **Legacy re-run.** `scripts/legacy_replay.py`, on a copy of the legacy app.
  The legacy app has no `-datapath`, so the copy runs portable from its own
  folder.
- **Dump runners.** Build `pcsx2-gsrunner` from `git archive v2.6.3` and
  `git archive aa7ab4306` of the fork with the fork's configure line,
  `-DENABLE_QT_UI=OFF`; v2.6.3 needs `-DCMAKE_DISABLE_FIND_PACKAGE_FFMPEG=ON`
  against the current FFmpeg. Apply the replayer change in `scripts/`, put
  `portable.txt` and the matching app's `Resources` next to the binary, and
  run `-renderer sw -surfaceless` with `GSR_VRAM_DIR` and `GSR_VRAM_MAX` set.
- **Bisect.** Compile `Metal23.metallib` from each commit's `.metal` sources.
