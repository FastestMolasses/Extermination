# C7 original captures

Original-game captures requested by the C6 chain (VOICE, FXLIVE, SHADOW, ROUTE,
the rand() order audit, and in part B DOOR, OBJKERNEL and H7), recorded in the hidden, MCP-enabled PCSX2 from the user's
own disc and save states. This file cites addresses, frame numbers and what was
recorded. It holds no original code, no disassembly and no disc bytes. All
outputs are local and ignored (`build/s87/c7cap/<item>/`).

Tools (decomp `.venv` python, repo root). Both parts drive
`tools/pcsx2_session.py` (frame boundary = breakpoint at the main-loop top
0x1AAF28) and reuse route_capture's beat functions and sampler and
route_census's persistent DebugServer connection.
- **Part A** (sections 1, 2, 2b and 3): `tools/c7cap_capture.py`, new. Part A
  changed no existing tool.
- **Part B** (sections 4 to 6): `tools/c7cap_partb.py`, new, and an opt-in beat
  group `c7` in `tools/route_capture.py`. That is the only change to an existing
  tool (`git diff --stat tools/pcsx2_session.py tools/route_census.py` is
  empty). route_capture's default behaviour was shown to be unchanged: the old
  and the new module give the same beat selection and the same output folders
  for every existing beat name (details under Part B).

Common rules of every run:
- PCSX2 runs hidden (`open -g -j`, System Events hide) and is closed at the end
  of every run. Each run ends by printing `no emulator process left: True`
  (`pgrep -f PCSX2.app/Contents/MacOS/PCSX2` finds nothing).
- Save states are only read. pcsx2_session hashes each source before and after.
  For route beats, the beat's recorded source snapshot is reloaded through Pine
  from a temporary copy in a free slot 40 or higher. The copy is deleted right
  after the load, and slots 01..15 are never written. No tool edits the
  emulator install, its ini files or its backups.
- **What PCSX2 itself wrote.** When PCSX2 exits it saves its own settings.
  On exit (the mtimes are the job's last exit, 18:11:00) it rewrote the live
  `build/startup-reference/inis/PCSX2.ini` (the file PCSX2 loads, per
  `config-check.log`) and `inis/playtime.dat`, and updated the mtimes of
  `portable-data/memcards/Mcd001.ps2` and `Mcd002.ps2`. No copy from before the
  job exists, so the only reference is the older, unused
  `portable-data/inis/PCSX2.ini`. Against it the live ini differs in
  `EnablePINE` (true, needed for Pine), `[UI] MainWindowGeometry` and the
  on-screen-display keys `Osd*` of `[EmuCore/GS]`. The renderer and every GS
  emulation key (`Renderer`, `upscale_multiplier`, `UserHacks*`) are equal.
  That every value is unchanged cannot be proven. Future jobs should hash the
  live ini, `playtime.dat` and both memory cards before and after each run.
- Frame numbers: `counter` is the main-loop counter 0x70003B64 and `vsync` is
  the vsync-ISR field counter 0x00810E90. Route frames `f` / `rec_f` use the
  numbering of route_capture's `trace.json` (row f = the state after f steps).

## Reproducibility of the original runs (applies to all items)

- **Open-loop replay of recorded pad values does not reproduce a route beat.**
  Route 13 was replayed three times from its source snapshot with the recorded
  pad values at the recorded frame indices, twice through the Pine reload and
  once through `-statefile`. The three replays were identical to each other.
  All three matched the recording row for row through f132. At f133 the
  recording shows a one-frame stop (player action +0x1F0 = 5, position held)
  with no input change, and the replays do not. The recorded runs therefore
  contain host-timed pad-application effects. route_census had already found
  the same kind of difference in its closed-loop replays.
- **So route stretches re-run route_capture's own closed-loop beat function**
  (as route_census does). The beat's source snapshot is reloaded through Pine,
  and neutral frames lead up to the recording's first counter, so row 0 equals
  the recording. Each stretch is aligned on an event anchor: the first frame
  whose owner script pointer (+0x1F8) equals a given script. The alignment is
  proven by comparing the event-side row fields (`spad`, `msg`, `cam_mode`,
  `screen`, `fade`, `req`, `story790`, `d2`, the director's +0x1F0 block and
  Roger's script) at equal anchor-relative offsets.
- The startup from user slot 01 is repeatable at the level of the events. A
  C7 run and route_census pass B both put the opening's stream request at
  counter 1756. Pass A and pass B had different first-control counters (3063
  and 3078).

## Conclusions for the port

- **Lane-3 +0x40 bytes: leftovers of the intro movie's data buffer** (sections 2
  and 2b). Their value at the opening depends on whether and when the player
  skips the movie. In the recorded data nothing *changes* them from the end of
  the movie through first control + 30 (C7 detected value changes and pinpointed
  writes only inside the movie frames; it traced no reads). They are read every
  frame: the barrel 001F0720 copies them into its lane-3 DMA packets 1..3 (port
  EFFECT_MANAGER.md, LEVEL_SMOKE.md), and per the port census (001F0720 row) no
  lane-3 slot is active and nothing from lane 3 is drawn in the first level.
  Excluding them from comparison instead of modelling them rests on that census
  observation and is a lead decision, not something this data proves.
- **Stream timing is a drive-busy mechanism, not a fixed latency** (section 1).
  The sequencer serves one read at a time and issues a lane's read only when
  the drive reports ready, so a refill still in progress delays the voice read
  (r10: +4 frames instead of +2; the opening: 16 fields). Each of the four
  16-sector stream reads completed 7 fields after issue, and key-on followed 2
  frames after completion (4 in the opening, where the lane waits on
  D_008106F4). Other read sizes took other times: the module-0x21 load's
  1-sector header about 6 fields and its 161-sector chunk about 8 (section 6).
  Four stream reads and two load reads give no general timing law.
- **rand() order in the opening is run-dependent after about 689 calls**
  (AE+31, section 3). Two New Game runs agree call for call up to there and
  then differ in caller order. rand comparisons must use closed-loop windows
  aligned on markers, not one long open-loop sequence.
- **Open-loop pad replay does not reproduce a recording** (above). Port
  comparisons against route captures must drive the port closed-loop, as
  route_census and these captures do.

## 1. STREAM: stream drive latency

**What was recorded.** Four stretches. In each stretch the following fields
were read at every main-loop top, and inside the per-field window also at
every vsync-ISR entry (exec breakpoint 0x1AB140):
- D_00282154..D_0028215B;
- the three lane records D_00281FD0 + 0x60·lane (the whole 0x60 bytes are kept;
  +0x00..+0x03, +0x20 cue, +0x30 sector, +0x34 count, +0x38 address, +0x4C
  duration and +0x50 start are decoded);
- D_00810E90 and D_00810E98;
- D_008106F4 / D_008106F5;
- the voice ring D_00281CF0[16] and its indices D_00275B30 / D_00275B34;
- 0x70003B8C..93, the fade block 0x28A9A0 and the message block 0x2821B0.

Exec breakpoints (one pause each; frame, counter, vsync, ra, a0, a1) were set on:
001FD4C0 (stream request), 001FA790 (lane start), 001FABF0, 001FA5A0 (voice
push), 00112610 (disc read issue), 0011A6A0 (key-on packer), 001FAAC0 (lane
release) and 001FAD70.

**IOP side.** The DebugServer's r3000 interface reads IOP memory and registers.
At every full record it read:
- the CDVD registers 0x1F402004..0x1F40200F (N-command, N-ready, error, break,
  interrupt status, 0x..09, drive status 0x1F40200A, 0x..0B, the three
  current-position bytes and the disc type);
- IOP DMA channel 3 (CDVD) MADR/BCR/CHCR at 0x1F8010B0..0x1F8010BB;
- the IOP PC.

What it did not read:
- the S-command/result FIFOs from 0x1F402016 on (reading them pops data);
- cdvdman's internal state in IOP RAM. The DebugServer's `get_modules` returns
  no IOP module list, so its address is unknown;
- the SIF RPC state;
- PCSX2's internal drive timing model, which is not memory-mapped.

**How to reproduce.**
```
.venv/bin/python tools/c7cap_capture.py stream --stretch opening   # ~11 min (intro movie in one frame)
.venv/bin/python tools/c7cap_capture.py stream --stretch r10       # ~3 min
.venv/bin/python tools/c7cap_capture.py stream --stretch r11       # ~1 min
.venv/bin/python tools/c7cap_capture.py stream --stretch r13       # ~1 min
```
- **Opening.** Starts from user slot 01 (title, counter 1306) with route_census's
  startup inputs: Cross at title frame 5, released at 9, then plain frames until
  0x810700 = 0x0B and task +0xB = 1. Recording starts at that frame (counter
  1753, n = 0) and runs per field until the fade-in + 20, then per frame for 120
  more.
- **r10 / r11 / r13.** Start from the recorded sources 08_truck_crossing /
  10_cage_roof_roger / 12_crevice_jump. The anchor is the director r12
  (0x7A93F0) script start: 0x8294C0 / 0x829A40 / 0x829CC0.

**Where.** `build/s87/c7cap/stream/<opening|r10|r11|r13>/`:
- `fields.jsonl`: per-field full records, `phase` = `isr` or `top`;
- `frames.jsonl`: per-frame compact records from the beat start or the AREA11
  start to the end;
- `events.jsonl`;
- `meta.json`: run summary and alignment probes;
- `logs/`.

**Alignment.**
- **Opening.** `ng` = counter + 886 maps the run onto newgame_samples' numbering
  (`build/startup-reference/newgame_samples.jsonl`). The offset is defined so
  that the request frame (0x70003B8D becomes 2) is ng 2642, so that frame
  matches by construction and is not a check. The independent checks, each
  against the last newgame_samples sample of the frame:
  - 0x70003B8F becomes 1 at ng 2643 and 2 at ng 2644, as in newgame_samples;
  - the fade-in (0x70003B91 and 0x70003B92 become 1 together, fade 0x28A9A0 =
    01 00 00 02 ef) is at ng 2667, as in newgame_samples;
  - over ng 2639..2807, 164 of the 169 frames present in both have identical
    0x70003B8C..93 and fade bytes. All 169 have identical 0x70003B8C..93. The 5
    that differ are in the fade counter only (ng 2639, 2641, 2671..2673).

  newgame_samples was sampled free-running, mostly 2 or 3 samples per frame,
  so the sample choice matters. With the first sample of each frame, 155 of
  169 agree, because a frame's first sample can still show the previous
  frame's state.
- **r10 and r11.** Every row of the closed-loop run is identical to the recording
  (row 0 through the stop row: 3398 and 1059 rows), and the anchor frames match
  (1089, 705).
- **r13.** Rows are identical through f132. After that the player's walk differs
  (the f133 stop, above), and the anchor comes 7 frames earlier (mine 523 =
  recorded 530). Every aligned probe row is equal in all event-side fields,
  from anchor−1 through the window, the teardown row 747 and 748.
- **Fields per frame.** Every main-loop frame in all four stretches is exactly
  one field (vsync +1 per counter step: 168, 3398, 1059 and 755 frames). Per
  frame therefore equals per field in these stretches.

**What it shows** (measured; vsync = D_00810E90, f = recorded frame):

| Stretch | Lane start (001FA790) | First read issue (00112610) | Read done (+0x03 = 2) | Key-on (0011A6A0), +0x50 | Start → key-on | +0x4C | Timer release (001FAAC0 from 0x1F9DD8) |
|---|---|---|---|---|---|---|---|
| Opening, lane 0 cue 0x3F (001FD4C0 a0 = 0x66, ra 0x1B863C) | n3 (ng 2641 executing), v21213 | n19, v21229, 16 sectors | n26 | n30, v21240 (+0x50 = 21240) | 27 fields | 1475 | — |
| r10 Roger 0x7F: lane 1 cue 0x8F | f1163, v19055 | f1167, v19059 | f1174 | f1176, v19068 | 13 fields | 243 | f1419, v19311 |
| r11 0x97: lane 1 cue 0x96 | f873, v22334 | f875, v22336 | f882 | f884, v22345 | 11 fields | 158 | f1042, v22503 |
| r13 0x99: lane 1 cue 0x95 | f536, v23596 | f538, v23598 | f545 | f547, v23607 | 11 fields | 126 | f673, v23733 |

- **Opening sequence.** At n0 (counter 1753) the previous lane-0 music (cue 1)
  is mid-refill: phase D_00282157 = 1, then 2 at n1. At n2 lanes 0/1/2 are
  released (001FAAC0 from 001FAB50 / 001FAB80) and D_00282157 is 0.
  - n3: 001FD4C0 runs the same release again through 001FD470, sets
    D_008106F4 = 2, and 001FA790(0, 0x3F) sets the lane to 01 02 01 01.
  - n3..n18: the sequencer stays in phase 1, because 00113280(1) does not
    report the drive ready. The drive is still busy with the old refill:
    N-ready 0x8C, status byte 0x12 from n1 through n16, then 0x06 at n17..n18.
  - n19: the new read is issued (0x12 again), and it completes at n26 (0x06 at
    n25, then 0x0A).
  - n27: the lane enters state 2 with D_008106F4 = 1.
  - n28: the fade-in starts (ng 2667).
  - n30: D_008106F4 = 0, key-on, +0x50 = 21240.

  The 27 fields from lane start to key-on are therefore:
  - n3 → n19, 16 fields: waiting for the drive to finish the old music refill;
  - n19 → n26, 7 fields: the new read, the same 7 fields as a voice read;
  - n26 → n30, 4 fields: read done to key-on (the lane waits in state 2 while
    D_008106F4 = 1, n27..n29).

  It is not a fixed read latency.
- **Voice lines** (lanes 1 and 2).
  - 001FA5A0 pushes the cue, and 001FA5F0 → 001FA790 → 001FABF0 start the lane
    in the same frame.
  - The read is issued 2 frames later (4 for 0x8F).
  - The read completes 7 fields after issue: the drive status is 0x12 for about
    6 fields, then 0x06, then 0x0A.
  - Key-on (0011A6A0) and +0x50 = D_00810E90 follow 2 frames after completion.
  - The line's timer release comes exactly +0x4C fields after +0x50: 243, 158
    and 126.
- **Lane 0 in the level.** Lane 0's refills run every 18/54 frames, alternately
  (r13 f53, 71, 125, 143, ...). Each refill finishes in 2 frames when the drive
  is idle. The sequencer serves one read at a time, and the voice latency
  depends on it:
  - In r10, a lane-0 refill read was issued at f1164, one frame after 0x8F's
    lane start. The voice read followed at f1167 (+4 instead of +2), and
    key-on came 13 fields after the start instead of 11.
  - In r11, lane 0's read at f871 was issued before the voice start (f873) and
    did not delay the voice read (+2).
  - r13 had no lane-0 read nearby.

  In r10, lane 2's second line starts while lane 1 plays: 0x90 is pushed at
  f1177, but its key-on waits on the voice hold until f1396.
- The CDVD status byte (0x1F40200A) takes 0x0A (idle), 0x12 and 0x06 in these
  stretches. The meaning of these codes comes from PCSX2's CDVD emulation, not
  from the game. The measured durations are what the port needs.

**What it does not settle.**
- It does not give a drive timing law in sectors. The seek time depends on the
  head position and PCSX2's model, and only four starts were measured. A
  model has to be fitted from these (and more) captures. A constant does not
  fit (opening 27 fields versus 11..13 for voices).
- Whether PCSX2's CDVD timing equals a real PS2's is outside what any emulator
  capture can show.
- Why the port's teardown is 8/6/6 rows early is left to the VOICELAT step. The
  captures give the original's lane start, read, key-on, +0x50/+0x4C and timer
  release frames to compare with.
- **Event completeness** (checked, not a gap). In all four stretches every
  frame where D_00282157 becomes 2 has its 00112610 event, including the
  opening's first one: n1 (vsync 21211, a0 = 0xB001F, lane 0's sector 720927,
  16 sectors), after which the IOP drive status is 0x12 from the n1 ISR on.
  The match is one to one: 7, 143, 35 and 24 events for as many phase changes,
  and no 00112610 event without one.

## 2. LANE3: who writes the lane-3 ring's +0x40 quadwords before the opening

**What was recorded.** The 32 quadwords at 0x76D9C0 + 0x60·slot + 0x40
(D_0028F700 + 0x4DBEC0 + 3·0xC00, slots 0..31). These are the bytes that
FXLIVE left uncompared.

Two passes from user slot 01 (title) through New Game, the intro movie, the
AREA11 load and the opening, to first control + 30 frames:
- **bracket.** No memchecks. The 32 quadwords are sampled at every main-loop top
  and at every vsync-ISR entry (0x1AB140). The ISR keeps firing while the intro
  movie plays inside one main-loop frame, so every change is bracketed to one
  field.
- **pinpoint.** The same run again. EE write memchecks on the 32 ranges are armed
  only over the field before 9 chosen bracket changes: the first 3, 3 in the
  middle and the last 3. Every hit pauses and records the store PC (the pause
  PC), ra, a backtrace (entry/pc pairs only), counter and vsync.
- Samples are keyed (frames stepped, ISR count since the last top), which does
  not depend on the start counter.
- Memchecks armed for the whole run were tried first and abandoned. The region
  is rewritten continuously while the movie plays, at about 9 pauses per second
  of host time.

**How to reproduce.**
```
.venv/bin/python tools/c7cap_capture.py lane3 --from title --mode bracket    # ~14 min
.venv/bin/python tools/c7cap_capture.py lane3 --from title --mode pinpoint   # ~16 min, needs the bracket files
```
Inputs: route_census's startup (Cross at title frame 5, released at 9). The
intro movie is **not** skipped.

**Where.** `build/s87/c7cap/lane3/title/`:
- `bracket_changes.jsonl` / `pinpoint_changes.jsonl`: every sample whose 32
  quadwords changed, with the slots, the nonzero set and the new values (local
  only);
- `*_samples.json`: the sample sequence with keys;
- `writes.jsonl`: memcheck hits;
- `*_meta.json`;
- `logs/`.

**What it shows.**
- **Title.** In slot 01 (title, counter 1306) the block already has 4 nonzero
  slots (12..15). The run's first sample is byte-identical to the slot file.
- **Every change happens inside one main-loop frame, the intro movie's frame.**
  That is the 145th frame from the start (counter 1470 in the bracket run). The
  frame spans 9648 fields (vsync 11280..20928, about 161 s of movie). The
  changes run from movie field 33 to field 9500 (225 change samples). There is
  no change during the rest of the title, the NEW GAME commit, the AREA11 load
  (task +B 0 → 1, 0x1AE040 state 0) or the opening, up to first control + 30
  (counter 3099).
- **The writer is the movie player's data copy, not an effect routine.** All 4240
  memcheck hits (512 per armed field; 144 in one) have the same store PC,
  0x121908, inside block_copy 0x00121870. The call chain is identical for every
  hit (return sites):
  1. 00203F40 @0x204034;
  2. 00203D30 @0x203DD8;
  3. 00108AA0 @0x108BF4;
  4. 002036E0 @0x20382C, the movie player (STARTUP.md: it polls the held pad
     for the skip);
  5. 00203460 @0x2034A0;
  6. 00203350 @0x2033F0;
  7. the main loop 001AAE40 @0x1AAFDC, the step that runs only while
     D_00821058 == 1.

  The lane-3 ring's storage is reused as a movie data buffer.
- **The end state depends on where the movie stops.** When the movie plays to its
  end, the block passes through all-zero (field 9413) and ends with slots 12..15
  nonzero. Those 4 values equal the title-time values. So in a New Game without
  a skip, the opening starts with 4 nonzero slots, not 20.
- **Where the route captures' 20 slots come from.** Every route capture and the
  user's slots 02..15 have 20 nonzero slots (0..19), and lanes 0, 1 and 4..6 are
  zero. Slot 04's whole 32-quadword block is byte-equal to the bracket run's
  state after movie field 113, which stays unchanged until the next change at
  field 153. It equals no other state of the run. Those saves come from
  newgame_probe.py, which skips the movie with START about 2 s after it starts,
  and field 113 is about 1.9 s. So the captures' lane-3 parameters are the movie
  buffer's contents at the skip.
- **Repeatability.** The bracket and pinpoint runs had the same number of change
  samples (226) and the same final block. The movie's field timing moved by 1..2
  fields at 4 of the 225 changes.

**What it does not settle.**
- **Who wrote the title-time slots 12..15.** They are already in slot 01, and
  this pass starts at the title. Section 2b answers this from a cold boot.
- **The 4240 recorded hits are 9 of the 225 changed fields.** The other fields
  were not pinpointed. The identical call chain in the first, middle and last
  groups suggests the same writer throughout, but this is not proven for every
  field.
- **A memcheck sees EE CPU stores only.** A DMA write to the block would show in
  the bracket pass as a change without hits. In every pinpointed field the
  change was accompanied by block_copy hits.
- **What the port does with them.** The value of these quadwords at the
  opening depends on whether and when the player skips the intro movie. The
  port excludes them from comparison rather than modelling the movie buffer
  (see Conclusions for the port). Port docs/EFFECT_MANAGER.md records that
  001F03D0 leaves +0x40 as it finds it. C7 recorded writes only, so it adds
  no evidence about reads.

### 2b. LANE3 from a cold boot to the title

**What was recorded.** The same bracket and pinpoint passes on a cold boot of
the user's disc: `-fastboot -elf config/SCUS_971.12` with the rebuilt ISO and
no save state, so no slot is read or written. The DebugServer is paused as
soon as it reports a live VM:
- bracket run: at the BIOS reset vector 0xBFC00014, 11 cycles;
- pinpoint run: at 0x9FC42970, 4.76 M cycles, still in the BIOS.

The ISR breakpoint and, in the pinpoint pass, the 32 memchecks are armed
before the VM continues. Both passes run to the title (task 0 = 001AC070, no
movie) + 90 frames. The title is first reached at counter 1212 and the runs
end at 1302.

**How to reproduce.**
```
.venv/bin/python tools/c7cap_capture.py lane3 --from boot --mode bracket    # ~12 min
.venv/bin/python tools/c7cap_capture.py lane3 --from boot --mode pinpoint   # ~12 min
```

**Where.** `build/s87/c7cap/lane3/boot/` (the same file set as section 2).

**What it shows.**
- **At the first main-loop top** (counter 0) all 32 quadwords are nonzero.
- **Before the main loop**, the pinpoint pass's memchecks hit 600 times (the
  per-interval cap; the memchecks were then disarmed for the rest of that
  interval). All hits are at 0x8000EBE0, in BIOS kernel code, with a
  kernel-only backtrace (0x8000EBB8 ← 0x8000E0D0 ← 0x80001024).
- **After that, every change happens inside one main-loop frame**, counter 1214:
  the pre-title intro movie, 9651 fields (vsync 1375..11024). The changes run
  from movie field 32 to field 9500 (225 change samples, as in the New Game
  movie).
- **The pinpointed fields** (the first 3, 3 in the middle and the last 3 changes;
  512 hits in 7 of the fields, 368 and 144 in two middle ones) produced 4096
  memcheck hits. All of them are block_copy 0x00121870 (store PC
  0x121908) under the same movie-player chain as in section 2 (00203F40 ←
  00203D30 ← 00108AA0 ← 002036E0 ← 00203460 ← 00203350 ← main loop 0x1AAFDC).
- **The block after the movie** (slots 12..15 nonzero) is byte-equal to user
  slot 01's block and to section 2's title-start block.

**Answer to FXLIVE's open item.** Nothing writes lane-3 parameters as such
before the AREA11 opening. The ring's storage doubles as the movie player's
data buffer, and block_copy fills it under 002036E0:
- the pre-title intro movie leaves slots 12..15 (the title state);
- the New Game intro movie leaves slots 12..15 when it plays to the end;
- a skipped New Game movie leaves the buffer contents of the field where it
  was skipped (the route captures' 20 slots come from a skip at about movie
  field 113).

**What it does not settle.**
- **The pre-main-loop writers are not fully recorded.** The 600-hit cap stopped
  recording them, and the kernel-mode writes are not attributed further. Those
  contents are overwritten by the pre-title movie in any case.
- **The pre-title and New Game movies' change sequences are not identical
  state by state.** They have the same change count and the same final block,
  but the state sequences differ. Whether it is the same movie file was not
  checked.

## 3. RNG: every rand() call, with caller, frame and value

**The function.** rand is 0x00122BB8 (`src/func_00122BB8.c`: byte-matched C,
ee-gcc, objdiff-perfect, linked from the compiled object per the census
provenance). Its state word is +0x58 of the block that D_0024295C points to,
0x002426C8 at runtime. The step is `state = state * 1103515245 + 12345`, and it
returns `state & 0x7FFFFFFF`. srand is 0x00122BA8.

**What was recorded.**
- **Every rand call** (exec breakpoint at 0x122BB8) gives one JSON line with:
  - `f`: frame index (the frame being executed);
  - `c`: the main-loop counter during that frame;
  - `v`: D_00810E90;
  - `ra`: the caller's return address;
  - `s`: the state word before the call;
  - `r`: the value returned, computed from `s` by the byte-matched step.
- **Checks on the computed return:**
  - The first 25 calls of every stretch were also stopped at their return
    address, and v0 was compared with `r`: 25/25 equal in every stretch.
  - Every call's `s` must equal the previous call's new state. The chain starts
    at the state read when tracing is armed (`state_at_arm`), so a missed call
    shows as a chain break.
- **srand calls** (0x122BA8) are recorded with their seed. None occurred in any
  stretch.
- **Markers:**
  - 001AFCF0 (called from 0x1AE040 state 0 at 0x1AE094), marking the area entry;
  - 001FAE70 (music cue select), with ra and a0.

**How to reproduce.**
```
.venv/bin/python tools/c7cap_capture.py rng --stretch newgame   # 1-2 h (each call pauses the VM; host-load bound)
.venv/bin/python tools/c7cap_capture.py rng --stretch r01       # ~6 min
.venv/bin/python tools/c7cap_capture.py rng --stretch r10       # ~5 min
```
- **newgame.** Starts from user slot 01 with route_census's title inputs. Tracing
  is armed at the NEW GAME commit (0x810700 != 0, after the intro movie's
  frame). It continues through the AREA11 load, the area-entry frame, the
  opening and first control (route_capture's `in_control` after a cutscene),
  to first control + 300 frames. `f` counts frames stepped after the commit.
- **r01.** The whole battery beat (recorded f1..f516), closed loop from user
  slot 04. The anchor is the battery pickup's take script 0x266620 (recorded
  f125).
- **r10.** Recorded f1080..f1400, closed loop from 08_truck_crossing. This
  covers director beat 0 (script 0x8294C0 at f1089), Roger's conversation
  script 0x828990 (f1094), his first line (msg 0x7F at f1163) and the next
  lines. Tracing is armed 60 frames before the window (f1020) and cut to the
  window.

**Where.** `build/s87/c7cap/rng/<newgame|r01|r10>/rand.jsonl` (JSON lines;
route stretches also carry `rec_f`), `meta.json` (summary, alignment, stall
events), `logs/`. `rng/newgame_run1/` is the first New Game run, which lost
12 calls (below).

**Alignment.**
- **r01.** All 516 rows are identical to the recording, and the anchor is at
  f125 in both.
- **r10.** All 1400 rows are identical to the recording, and the anchor is at
  f1089 in both.
- So `rec_f` = `f`, and the traces are the recorded routes' own frames.
- **newgame.** No route recording exists for this stretch. It is aligned by its
  markers.

**What it shows.**

*Callers (return address → function):*

| ra | function |
|---|---|
| 0x1F4D74 | 001F4D40 |
| 0x1F54FC | 001F54E0 |
| 0x1D7D44, 0x1D7DD4 | 001D7C30 (point-light sway; exactly one call from each per frame, every frame) |
| 0x1D0A10, 0x1D0AAC, 0x1D0B80, 0x1D077C, 0x1D08A4, 0x1D08D4 | 001D0720 |
| 0x179BA0 | 00179B90 |
| 0x1E26C4, 0x1E2724 | 001E2560 (head sprite) |
| 0x1E5660, 0x1E5690, 0x1E57F8 | 001E55F0 (weather effect) |
| 0x1EA424, 0x1EA430 | 001EA240 (footstep effect) |
| 0x1F11F8, 0x1F14F0 | 001F1180 |
| 0x1FAF28 | 001FAE70 (music cue select) |
| 0x1E25BC | 001E2560 (first frame after its spawn) |
| 0x1E564C, 0x1E56D4, 0x1E5710, 0x1E5758 | 001E55F0 (first frame) |
| 0x1F112C | 001F1110 |
| 0x8236B4, 0x8259F0 | AREA11 overlay code (one draw each, at AE+1) |

*r01* (5438 calls over f1..f516; 2..26 per frame):
- by caller: 001F4D40 2420, 001F54E0 1949, 001D7C30 514 + 514, 00179B90 12,
  001EA240 6 + 6, 001D0720 4 + 2, 001E2560 3 + 3, 001E55F0 2, 001F1180 1 + 1,
  001FAE70 1;
- state at f1: 0xE70CE8B7;
- the one 001FAE70 call is 001FAE70(1) at f485, returning to 0x1AE50C, as the
  status page closes;
- chain intact, no stall events.

*r10* (6619 calls over f1080..f1400; 20..27 per frame):
- by caller: 001F4D40 3531, 001F54E0 2247, 001D7C30 321 + 321, 001D0720 185,
  00179B90 6, 001E2560 7;
- state at f1080: 0x334A57DA;
- chain intact.

*newgame* (run `rng/newgame/`: 36512 calls, chain intact, 4 stall kicks). The
kicks were all at breakpoint PCs, two at 0x122BB8 and two at 0x1AAF28, and
the two of each pair have the same cycle count. None carries `taken_as_pause`,
which the tool sets when a frozen VM's cycle count has moved past the last
pause. So each kick was the case of a resume that had not yet taken effect at
the pause just handled, and the tool re-issued the resume. The intact chain
shows that no call was lost. Findings:
- **rand is unseeded when New Game starts.** The state word is 0x00000001 when
  tracing is armed at the NEW GAME commit (counter 1476). No call happens from
  the commit through the AREA11 load until the area-entry frame. srand is
  never called.
- **Area-entry frame** (n270, counter 1745 during the frame; 001AFCF0 is called
  from 0x1AE094, then 001FAE70(1) from 0x1AE0CC). Exactly one call:
  001FAE70 at ra 0x1FAF28, state 0x00000001 → returns 0x41C67EA6. This is the
  draw C6 left unbound (UM_001FAE70).
- **Frame AE+1** (the first frame after the entry): 35 calls. Besides the per-frame
  callers, it holds the one-off initial draws:
  - overlay 0x8259F0 and 0x8236B4 (one each);
  - 001F1110 at 0x1F112C;
  - 001E55F0 at 0x1E564C, 0x1E56D4, 0x1E5710 and 0x1E5758;
  - 001E2560 at 0x1E25BC (three).
- **Opening, AE+1..first control** (first control at n1594 = AE+1324, counter
  3070): 22..35 calls per frame, 29893 in all (these counts include AE+1 and
  the first-control frame). By caller: 001F4D40 14564, 001F54E0 11907,
  001D7C30 1324 + 1324, 001D0720 712, 001E2560 35, 001E55F0 23, overlay 2,
  001F1110 1, 001FAE70 1.
- **First control.** The frame that returns control (AE+1324) calls
  001FAE70(0) from overlay 0x823F94, which draws once at 0x1FAF28.
- **First control + 1..+300:** 22..23 calls per frame, 6618 in all. By caller:
  001F4D40 3300, 001F54E0 2700, 001D7C30 300 + 300, 001E2560 7, 001D0720 5,
  001F1180 5, 001E55F0 1.
- **Run to run.** A first New Game run (`newgame_run1/`, commit counter 1577,
  area entry n271) agrees with this one call for call (relative frame, ra and
  state) for the first 689 calls, from the area entry to AE+31. At AE+31 the
  order of callers differs: run1 has 001F4D40 draw the state that this run's
  001D0720 and 001E2560 draw. From there on the sequences differ. So the
  original's call order during the opening is not fixed from run to run after
  the first 31 frames; it varies with the run's timing.
- **A second attempt** (log `rng/run_newgame2.log`) stopped because the intro
  movie's frame gave no pause within the 1200 s stall limit under host load
  (load average 20..120 during these runs). The limit is now 3000 s, and the
  third attempt is `rng/newgame/`.
- **The run1 trace has 12 calls missing** (f1462..1463 and f1743..1745). Each
  chain break is exactly one LCG step. Under heavy host load, the DebugServer
  reported "running" while the VM stood on the rand breakpoint, and the
  stall-recovery resume skipped that breakpoint. The tool now treats a frozen
  VM at one of its breakpoints with a new cycle count as the pause it is.
  This run (`newgame/`) has no break.

**What it does not settle.**
- **Inline copies of the LCG are not traced.** The decomp's C has no second copy
  of the multiplier (`grep 1103515245 src` finds only 00122BB8), but overlay
  code was not searched.
- **A different state block is not covered.** D_0024295C was read once per
  stretch (0x002426C8 every time). A call through a different block would draw
  from another state and not be seen by the chain check.
- **The newgame stretch's absolute counters depend on the run.** The free-running
  start from slot 01 moves the title start (1326..1336 in C7 runs). The markers
  (area entry, first control) are the alignment.
- **Why each caller draws is not interpreted.** Mapping these callers onto the
  port's modules is the RNGORDER step's job.

## Part B

Part B adds sections 4 to 6. Its tools:
- `tools/route_capture.py` gained an opt-in beat group `c7`. `--beats all`
  and every existing beat name select exactly what they selected before, and
  every existing beat writes to the same folder. This was checked by loading
  the old and the new module side by side and comparing the selection for
  `all`, `09`, `15`, `07,08`, `a01`, `a01_03,a01_s1` and a full beat name, and
  `beat_dir` for all 28 existing beats. The SPANS, OWNERS and BEATS tables are
  unchanged.
- `tools/c7cap_partb.py` is new. It has the `fb`, `fb-survey` and `h7` items.
  It reuses `tools/c7cap_capture.py`'s session, closed-loop driver and IOP
  reader unchanged.

The common rules above apply. Every run ended with no PCSX2 process left.

## 4. DOOR1: the fence door from side 1 (entry 1)

**What was recorded.** Route beat 09 ends behind the fence, at spawn entry 2
(424.2, 184.8, 274.5), facing yaw π. The new opt-in beat
`c7_door1_fence_door_side1` starts from beat 09's end snapshot
(`build/s87/route/09_fence_door/state.p2s`, counter 8477). It turns the
player toward the door r0 (001BC350, at (423, 184.8, 290.3)) and walks
against it from the south. It then presses Cross, which is accepted at the
first press. It waits for the room move (B8 = 2), then for control to return,
then idles 60 frames. Every frame records route_capture's usual row (the same
Sampler and decode as beat 09): the player record, spad 3B8C..93, the camera,
the request block 0x8106B0, the fade block 0x28A9A0, the screen and message
blocks, the UI bytes, the story and area bytes, and every OWNERS node,
including the door r0 at 0x7A70B0 (+0x00..+0x0F, +0xB0, +0x1F0..+0x1FF,
+0x2DC).

Frames f0..f544 cover counters 8478..9022. The run ends with a snapshot.

**How to reproduce.**
```
.venv/bin/python tools/route_capture.py run --beats c7                    # ~3 min
.venv/bin/python tools/route_capture.py events --beats c7_door1_fence_door_side1
```

**Where.** `build/s87/c7cap/door1/c7_door1_fence_door_side1/`: `trace.json`
(rows, inputs, the `presses` list), the snapshot (`state.p2s`, `eeMemory.bin`,
`gs.bin`, `scratchpad.bin`, `original.png`, `snapshot.json`). The snapshot
was checked to resume. Logs are in `build/s87/c7cap/door1/logs/`.

**What it shows.** Side 1 is compared here with beat 09's side 2. "Accept" is
the frame in which 3B8D becomes 2.

| | Side 2 (route 09) | Side 1 (this capture) |
|---|---|---|
| Cross held | f306..f307 | f225..f226 |
| Player at the press | (417.786, 293.837), yaw 2.4073 | (422.757, 284.633), yaw −0.1215 |
| Accept (3B8D = 2, player +5 = 0x25, camera byte 2, door script 0x24DE40) | f309 | f228 |
| Aligned player position and yaw (from accept) | (416.44382, 292.94885), 2.74017 | (420.3511, 283.7438), −0.4014 |
| Player +0x1F0 = 0x41 | f310 | f229 |
| Door script 0x24DC00 | f313 row | f231 |
| Player clip | 0x45 from f313 | **0x43** from f232 |
| Room-move request B7 / B8, fade machine to 3 | f407: B7 = 02, B8 = 02 | f306: **B7 = 01**, B8 = 02 |
| Fade to 2 (hold black) | f470 | f369 |
| Commit (request block cleared, 3B8D = 0, player re-placed) | f472 | f371 |
| Accept → B7 | 98 frames | 78 frames |
| B7 → commit | 65 frames | 65 frames |
| Arrival position and yaw | entry 2 (424.2, 184.8, 274.5), π | **entry 1 (413.7, 184.84021, 299.4), 0.0** |
| Player at the commit | +0x1F0 = 0, +5 = 0, clip 0 (in control at once) | +0x1F0 stays 0x41, **+5 = 1, clip 2** |
| Fade back to 0 | — | f434 |
| Control returns (+0x1F0 = 0, +5 = 0) | f472 | **f484** |

- **Side-1 arrival walk-out** (f371..f484). The player stands at entry 1 with
  clip 2 running from f371 through f422, 52 frames. Its clip clock is 1 at
  f371 and 45 at f372, counting down.
  - From f423 to f454, Z rises by 0.29998..0.29999 per frame. X stays 413.7
    and yaw stays 0.0. There is one frame with no change, f453.
  - From f455 to f480, the step shrinks by about 0.01136 per frame (0.28863,
    0.27725, ... 0.00452).
  - The walk ends at Z 312.51059, a total of 13.11059. Clip 0 starts at f480,
    and +0x1F0 / +5 return to 0 at f484.
- **Door record.** The door's +0x1F0 block runs 0x24DE40 → 0x24DE80 → 0x24DC00
  → 0x24DC40 → 0x24DC80 (f228..f233). It ends as ffffffff … 80dc2400 01000000.
  At the end of route 09 it is … 01000010.
- **UI bytes.** 0x810130 reads 00000000 0506 0000 through the whole capture.
  This was already the value in beat 09's last row.

**What it does not settle.**
- **One press stance.** The alignment point and yaw come from 001BBE40, and
  they depend on where the player stands. The port's oracle test
  (test_door_transit_reference) can check the f228 point against this
  capture. Other stances were not recorded.
- **The cause of the walk-out's shape is not identified.** The capture shows
  the 52-frame stand, the 0.3 per frame walk, the linear slow-down and the
  control return. Which original routine drives each part (001B07C0's 5/1/0
  per DOOR_ORIGINAL.md, the player's state 5/1 path) is not recorded. No
  breakpoints were set, so the run is unperturbed.
- **The closed-loop approach walk (f0..f224) is this tool's own navigation.**
  It is not a route the port must reproduce. The comparable part starts at
  the Cross press.

## 5. FB: framebuffer capture feasibility

**What was recorded.**
- **Live.** At three points, the GS privileged registers were read through
  Pine and through the DebugServer at 9 consecutive main-loop tops. The
  points are user slot 04 (first control, counters 4085..4093), route 03's
  end (5676..5684) and route 07's end (7704..7712).
- **Snapshot.** One save-state snapshot was taken at each point, into a free
  slot that is moved into the output folder at once.
- **Survey** (no emulator). For the three snapshots and all 16 route
  snapshots (00..15), the tool decodes:
  - the frozen GS privileged register page;
  - the GS freeze's drawing contexts (FRAME, ZBUF, SCISSOR, XYOFFSET);
  - the contents of GS local memory at the displayed and drawn buffers.

**How to reproduce.**
```
.venv/bin/python tools/c7cap_partb.py fb          # ~3 min: the three live points + the survey
.venv/bin/python tools/c7cap_partb.py fb-survey   # no emulator: the three points' freeze analysis again, then the survey
```

**Where.** `build/s87/c7cap/fb/`:
- `<first_control|route03_end|route07_end>/fb.json`: live rows (`rows`) and
  the freeze analysis (`freeze`, with the frozen register page under
  `freeze.frozen_registers`). The live rows' decoded registers are CSR
  mirrors (item 1 below), and the file's `note` says so;
- `<point>/snapshot/`: `state.p2s`, `gs.bin`, the save state's
  `original.png` and `snapshot.json`;
- `survey.json`;
- `meta.json`: per point, the live summary (`pmode`, `smode2`, `display*`,
  `dispfb*_fbp_seq`, decoded from the CSR mirrors, so not register values)
  and the frozen values (`frozen_csr_field`, `frozen_dispfb2_fbp`,
  `freeze_frame`, `freeze_scissor`, `freeze_buffers`).

`freeze.buffers_512x224` in fb.json and `freeze_buffers` in meta.json were
first keyed on FBP 0xC (bytes 0x18000..0x88000), a DISPFB decoded from the
live CSR mirror. They were recomputed without the emulator
(`c7cap_partb.py fb-survey`, which now takes the displayed buffer from the
frozen register page) and now name FBP 0 and FBP 0x38, as survey.json does.
The live rows, the contexts and survey.json were unchanged by the recompute.
Both buffers still hold the single value 0x80000000 at all three points.

**What it shows.**

1. **The GS registers cannot be read live.** Every EE read of 0x12000000..
   through Pine or the DebugServer returns the CSR value (0x551B400C or
   0x551B600C) for every register except SIGLBLID. PCSX2's gsRead8/16/32/64
   mirror CSR, as the hardware does. DISPFB, DISPLAY and PMODE can therefore
   only be read from a save state. The save state's internal structures
   contain the 0x2000-byte GS register page 1246 bytes after the
   `EE-Subsystems` freeze tag (PCSX2's gsFreeze). Its CSR there equals the
   live CSR.
2. **Display setup.** It is the same in all 19 snapshots:
   - PMODE = 0x66: circuit 2 only, fixed alpha 0.
   - SMODE2 = 3: interlaced, FFMD = 1.
   - DISPLAY2: DW = 2559 with MAGH = 4, so 512 pixels; DH = 447, so 448 lines;
     DX 636, DY 50.
   - DISPFB2: PSMCT32, 512 wide, DBX = DBY = 0.
   - Both drawing contexts (ctx 0 and ctx 1 are identical in every
     snapshot): FRAME PSMCT32 512 wide, SCISSOR x 0..511, y 0..223, ZBUF at
     0xE0000 (ZBP 0x70) PSMZ24.
   - With INT = 1 and FFMD = 1 the read-out is half height (PCSX2
     GSState.cpp: "Half height read if FFMD + INT enabled"). **Each displayed
     field is one 512×224 PSMCT32 buffer, shown as 448 display lines.**
3. **Two buffers alternate by field.** Buffer A is at FBP 0 (bytes
   0x0..0x70000) and buffer B at FBP 0x38 (0x70000..0xE0000). In every AREA11
   snapshot (routes 00..14 and the three fb points):

   | CSR FIELD | Displayed (DISPFB2) | Drawn (FRAME, both contexts) | XYOFFSET (OFX, OFY) |
   |---|---|---|---|
   | 0 | FBP 0 | FBP 0x38 | (1792.0, 1936.5) |
   | 1 | FBP 0x38 | FBP 0 | (1792.0, 1936.0) |

   (1792, 1936) is (2048 − 256, 2048 − 112), the centre of 512×224. The
   drawing offset moves by half a pixel in Y between the two buffers. Route 15
   (AREA01 arrival) shows FIELD 0 with display 0x38, draw 0 and OFY 1936.5,
   so the pairing above holds in AREA11 only.
4. **No rendered pixels are available.** In all 19 snapshots, both colour
   buffers and the Z buffer region of GS local memory hold one value in
   every word (0x80000000). The renderer is Metal
   (`build/startup-reference/inis/PCSX2.ini`, the file PCSX2 loads per
   `config-check.log`: `[EmuCore/GS] Renderer = 17`, `upscale_multiplier = 1`).
   The older copy `portable-data/inis/PCSX2.ini` is not loaded; it has the
   same values for these keys. With a hardware
   renderer the drawn frames stay in the renderer's texture cache. PCSX2's
   GSState::Freeze copies them back into local memory only when
   `UserHacks_ReadTCOnClose` is set, which is false here, and only honoured
   with `UserHacks = true`, also false.
5. **Other interfaces.**
   - **DebugServer and MCP server:** no GS or framebuffer command. Their
     memory reads reach EE and IOP address space only, and GS local memory
     is not mapped there.
   - **Pine:** no screenshot or GS-dump message.
   - **The save state's `Screenshot.png`:** 640×480 RGBA. It is the host
     presentation (display merge and scaling), not the 512×224 GS buffer. It
     is kept as `original.png`.
   - **PCSX2's GS dump:** reachable only through hotkeys (GSDumpSingleFrame
     = Shift+F8, GSDumpMultiFrame = Ctrl+Shift+F8) or the menu. Both need
     keyboard or mouse input to a visible, focused PCSX2 window, so it was
     not used.
   - **ToggleSoftwareRendering (F9):** a runtime renderer switch through
     the same keyboard path.

**Conclusion: not feasible with the current settings.** Recording frames at
first control and at the ends of routes 03 and 07 was therefore not done. The
register state and the host screenshots at those three points are recorded.

**Setting changes that would make it feasible.** None was made; this is for
the lead to put to the user. Every change goes into
`build/startup-reference/inis/PCSX2.ini`, the file PCSX2 loads. Editing
`portable-data/inis/PCSX2.ini` does nothing. Edit the file only while PCSX2 is
not running, because PCSX2 rewrites it on exit.
- **(a) Software renderer (recommended).** In
  `build/startup-reference/inis/PCSX2.ini`, section `[EmuCore/GS]`, change
  `Renderer = 17` to `Renderer = 13`.
  - The software renderer draws into GS local memory, so every save-state
    snapshot's `gs.bin` then holds both 512×224 buffers with the software
    rasteriser's values. The displayed one is DISPFB2's FBP; the other is the
    field being drawn.
  - `c7cap_partb.py fb` would then decode them. A small addition is needed to
    write the buffers out with tools/extract_textures.py's PSMCT32 swizzle.
  - **Reversible** by setting `Renderer = 17` again. No other key changes.
  - Emulation is slower (host time only). The EE-side captures are not
    expected to change, but that is untested.
- **(b) Hardware readback.** In the same file and section
  (`build/startup-reference/inis/PCSX2.ini`, `[EmuCore/GS]`), set
  `UserHacks = true` and `UserHacks_ReadTCOnClose = true`. The snapshot's
  local memory then holds
  Metal's rendered targets. They are the hardware renderer's approximation of
  the GS (shader blending and dithering), not the GS's own values.
  - Reversible by restoring both to `false`.
  - Not recommended for a pixel-exact harness.
- **(c) Runtime alternative.** Press F9 once in a visible PCSX2 window.
  This needs the window in front, which the capture rules exclude unless the
  user allows it.

**What it does not settle.**
- **Whether PCSX2's software renderer equals the real GS bit for bit.** It is
  the closest available reference. No emulator capture can prove hardware
  equality.
- **Which field the port's Original profile should present.** The measured
  facts are a 512×224 buffer per field, shown as 448 lines, with half-pixel
  Y offsets alternating per buffer. How to present that at 4:3 is a lead
  decision for PORT_PROFILES.md (the port doc currently says "512x448").
- **The display-merge path.** DX/DY, the CRT timing and the PCRTC merge were
  recorded as register values only.

## 5b. Framebuffers (software renderer)

**User decision (2026-09-26).** PCSX2's software renderer may be used, but
only during capture jobs. The switch is a manual step around the job; no tool
edits the emulator's ini.

**Ini handling (this job).**
1. Before PCSX2 started, sha256 and a comparison copy of the live
   `build/startup-reference/inis/PCSX2.ini`, `inis/playtime.dat`,
   `portable-data/memcards/Mcd001.ps2`, `Mcd002.ps2` (and the unused
   `portable-data/inis/PCSX2.ini`) went to `build/s87/c7cap/fb2/pre/`
   (`sha256.txt`). The copies are for comparison only; nothing is copied back.
2. `[EmuCore/GS] Renderer = 17` (line 220, the only `Renderer` line) was
   verified and changed to `Renderer = 13`. No other line was touched.
3. Captures (below). Every run ended with `no emulator process left: True`.
4. With PCSX2 stopped, the line was set back to `Renderer = 17`. The diff of
   the live ini against the pre copy is one line:
   `[GameListTableView] HeaderState` (a Qt table-header blob PCSX2 rewrites on
   exit). `Renderer` and every other key are equal. `playtime.dat` changed
   (PCSX2's play-time counter: 5774489 → 5774640 s). Both memory cards have
   their pre sha256 (757d6f77…, 47ebe237…); only their mtimes moved. The
   unused `portable-data/inis/PCSX2.ini` is unchanged. The save-state folder
   holds only the user slots afterwards.

`c7cap_partb.py fb2` refuses to start unless the live ini says
`Renderer = 13` and prints the restore instruction at the end of every run.

**How to reproduce.**
```
# manual: pre copies + hashes, then Renderer = 17 -> 13 (PCSX2 not running)
.venv/bin/python tools/c7cap_partb.py fb2            # ~3.5 min, 19 points
# manual: Renderer = 13 -> 17, diff the ini against build/s87/c7cap/fb2/pre/
.venv/bin/python tools/c7cap_partb.py fb2-decode     # no emulator: decode again
```

**Procedure per point.** The 19 points are the 16 route snapshots
(`build/s87/route/00..15`) and the three fb points of section 5
(`first_control` = user slot 04, `route03_end`, `route07_end`; the last two
load the same states as routes 03 and 07 and serve as a repeatability check).
1. Load the state (route_capture's session: `-statefile`, then pause and
   align to the next main-loop top 0x1AAF28). This loop top is `s0`.
2. Step two frames with a neutral pad. Inside each frame the tool takes two
   synchronisation snapshots: at the first instruction of the vsync wait
   0x1AAFF0 (`s<k>a_wait`) and at the first vsync ISR entry 0x1AB140
   (`s<k>b_isr`). It then continues to the next loop top (`s1`, `s2`). Why:
   this PCSX2 runs VU1 on its own host thread (`vuThread = true`). Without the
   syncs, the last GS writes of a frame can land after later EE breakpoints.
   In the free-running test (route 03, no syncs) both buffers changed in
   every step. That is a host-timing effect, not game behaviour. With the
   syncs, each frame's writes land before its vsync.
3. Decode the final loop top `s2` (outputs), and track which buffer each
   stage wrote.

Snapshots go to a free slot 40 or higher and are moved out at once. The
intermediate stages keep only `gs.bin`, the host screenshot and the frozen
register page (`priv.json`); `s2` keeps its `state.p2s`.

**Outputs** (`build/s87/c7cap/fb2/<point>/`, ignored; 787 MB in total):
- `displayed.bin`: the DISPFB2 buffer at `s2`. 512×224 PSMCT32 words in
  raster order, little-endian (bytes R, G, B, A), de-swizzled.
- `draw.bin`: the FRAME buffer at `s2` (both contexts name the same FBP).
- `z.bin`: ZBUF at ZBP 0x70, PSMZ24, 512×224 words. The low 24 bits are Z;
  the top byte stays 0x80.
- `displayed.png`: the displayed field at 512×224, RGB with alpha forced
  opaque, no scaling. `draw.png` is the same for the FRAME buffer.
- `PREVIEW_ONLY_displayed_448_line_doubled.png`: every line doubled to 448.
  It is a preview only, not the GS output.
- `registers.json`: the frozen PMODE, SMODE1/2, DISPFB1/2, DISPLAY1/2,
  BGCOLOR, CSR and CSR FIELD, and both contexts' FRAME, ZBUF, SCISSOR and
  XYOFFSET.
- `meta.json`:
  - the recorded snapshot and the beats that start from it;
  - the session rows compared with the recorded rows;
  - the counter and vsync of every stage, and the D1/D2 DMA registers;
  - per stage and buffer: rows and pixels changed, distinct words and
    0x80000000 count;
  - `last_written_at`, field parity and the decode proof.
- `capture.json` (raw) and the stage folders `s0`, `s1a_wait`, `s1b_isr`,
  `s1`, `s2a_wait`, `s2b_isr`, `s2`. `summary.json` covers all points;
  `run.log` and `decode.log` are the console output.

**Decode (GS local-memory layout).** These are public GS documentation facts;
PCSX2's GSTables.cpp carries the same tables.
- A page is 8 KiB (2048 words). A PSMCT32 or PSMZ32/24 page is 64×32 pixels.
  FBP and ZBP count pages, and a 512-wide buffer is 8 pages per page row.
  So 224 lines take 56 pages: buffer A is FBP 0, buffer B is FBP 0x38, and Z
  is at 0x70.
- A page holds 32 blocks of 8×8 pixels, 8 across and 4 down. Their order,
  row by row, is 0 1 4 5 16 17 20 21 / 2 3 6 7 18 19 22 23 /
  8 9 12 13 24 25 28 29 / 10 11 14 15 26 27 30 31. PSMZ32/24 uses the same
  order XOR 24.
- A block holds 4 columns of 8×2 pixels (16 words each). In a column, pixel
  row 0 holds words 0 1 4 5 8 9 12 13 and row 1 holds 2 3 6 7 10 11 14 15.

This is the table set tools/extract_textures.py already uses; it is
implemented again in `c7cap_partb.gs_word_map`.

**Proof of the decode** (all 19 points; `meta.json` `decode_proof`):
- **Block seams.** The ratio of the mean horizontal neighbour difference
  across 8-pixel block seams to the mean inside blocks is 0.93..1.15 for the
  decoded displayed buffer. Reading the same words in raster order (a
  deliberately wrong decode) gives 1.48..2.17.
- **Host screenshot, coarse.** On a 64×48 luma grid, the correlation with
  the save state's own 640×480 screenshot is 0.989..0.998 for the displayed
  buffer and 0.994..0.9999 for the draw buffer. The raster-order reading gives
  0.08..0.78. It is highest where letterbox bands dominate, because whole
  rows of pages are then black.
- **Known flat region.** Letterbox bands are runs of rows that are entirely
  0x80000000 (black, alpha 0x80). The host screenshot's mean RGB over the same
  rows is (0,0,0) to (0.3,0.3,0.3):
  - 00: rows 0..31 displayed, 0..17 draw;
  - 02: rows 192..208 and 192..221 (bottom band; edge max difference 23);
  - 03: rows 0..16 and 0..25;
  - 04: rows 1..31 and 0..31.
- **Rendered content.** The loaded states are the hardware-renderer route
  snapshots, whose buffers hold 0x80000000 in every word. Every output colour
  buffer now holds 4549..22429 distinct words; none is the uniform fill (for
  `z.bin` see below: at three points it holds only the post-frame clear). 0x80000000
  is also the game's own clear/letterbox word, so a uniform fill is not by
  itself a sign of "nothing rendered".

**What happens to the two buffers in one main-loop iteration** (counter
C → C+1). Measured in every second step (the steady one) at all 19 points:

| Stage | GS writes | DMA / registers |
|---|---|---|
| loop top C | none yet to X (FRAME names X, DISPFB2 names the other buffer Y) | VIF1 DMA active (D1_CHCR 0x30010105); D1_TADR 0x28F710 when frame_idx (0x810E80) = 1, 0x293710 when 0 |
| loop top → vsync-wait start | X: 175..224 rows (the scene); Z: 160..224 rows | VIF1 idle at the wait start (D1_CHCR 0x70000005) |
| wait start → first vsync ISR | X: 0..224 rows more (the last writes, e.g. letterbox bands, or a 50-row HUD region top-left at first control); Z: 0..224 rows | |
| ISR → loop top C+1 | nothing, in every point | S step: DISPFB2 := X; FRAME then names Y |

In the first step after the load, the other buffer (DISPFB2 at `s0`) also
received 0..224 rows before `s1`. That is the late tail of the free-running
alignment frame (the "late tail" above). It lands before `s1`, and step 2
writes only its FRAME buffer, so both buffers are final at `s2`. At 10 and 15
the first step's ISR came before the wait start (a long iteration).

Hence, at a loop top C (the point where route rows and snapshots are taken):
- **`displayed.bin` (DISPFB2)** was completed during iteration C−1 → C,
  before its vsync. PCSX2 presents it at the vsync that ends iteration C.
  Its list was built during iteration C−2's logic, so it shows the game state
  of **row C−1**. This is inferred from the VIF1 source slot (the packet slot
  of the previous iteration) and from func_001D2300 (V, after the vsync)
  dispatching the frame. It is not proven pixel for pixel.
- **`draw.bin` (FRAME)** was completed during iteration C−2 → C−1 and is
  untouched until C. It is the field on screen when the snapshot is taken,
  and it shows **row C−2**. The host screenshot is closer to it than to the
  displayed buffer in 18 of 19 points (mean absolute RGB error; 07 is the
  exception, 1.97 vs 2.19).
- **`z.bin`** was last written in step 2 at every point, but at 16 of 19
  points it holds the depth of the frame in `displayed.bin`. At 09_fence_door,
  14_roger_encounter and 15_level_exit it holds a single value, 0x80FFFFFF, in
  every word: there Z is drawn before the vsync-wait start (the s2a snapshots
  hold 21601, 35221 and 13710 distinct words) and then cleared completely
  between the wait start and the vsync ISR (s2b: one word); step 1 shows the
  same. At those points `z.bin` is a post-frame clear and carries none of the
  displayed frame's depth. The late full Z clear is itself a timing fact for
  the port's frame order.

**Frame alignment per point.** Columns:
- `rows at s0..s2` compares only the fields pos, yaw, spad, m1F0, camera,
  screen, fade, msg, ui and req. With every trace field compared, 07, 12 and
  route07_end also differ in hip, clip and clock (plausibly pad-driven: the
  recorded beats moved the stick at f0, these runs used a neutral pad).
- The state loaded for each route point was a scratch resume copy of
  `build/s87/route/<NN>/state.p2s` (deleted afterwards); exact row equality
  with the recorded next-beat rows supports that it was the same state, but
  no hash was recorded. Future runs should store the loaded file's sha256.
- `rec` is the recorded snapshot's counter.
- `s0..s2` are the session's loop tops; `s2` is the output.
- `+it` is the iterations from `rec` to `s2`.
- `rows` names the recorded rows at s0..s2 and whether the row fields
  (`pos`, `yaw`, `spad`, `m1F0`, camera, `screen`, `fade`, `msg`, `ui`,
  `req`) are equal.
- `x-vs` is the number of vsyncs beyond one per iteration between `rec` and
  `s2`.
- `FIELD` is CSR FIELD at `s2`.
- `disp/draw` are the FBPs of DISPFB2 and FRAME at `s2`.

| point | rec | s0..s2 | +it | rows at s0..s2 | x-vs | FIELD | disp/draw | draw OFY |
|---|---|---|---|---|---|---|---|---|
| 00_panel_no_battery | 4354 | 4355..4357 | 3 | none (next beat 01 is from slot 04) | 0 | 0 | 0 / 0x38 | 1936.5 |
| 01_battery | 4601 | 4602..4604 | 3 | 02 f0..f2 equal | 11 | 0 | 0x38 / 0 | 1936.5 |
| 02_elevator_refusal | 4989 | 4990..4992 | 3 | 03 f0..f2 equal | 0 | 1 | 0x38 / 0 | 1936.0 |
| 03_panel_power | 5675 | 5676..5678 | 3 | 04 f0..f2 equal | 0 | 1 | 0x38 / 0 | 1936.0 |
| 04_elevator_ride | 6252 | 6253..6255 | 3 | 05 f0..f2 equal | 17 | 0 | 0 / 0x38 | 1936.5 |
| 05_boxes | 6929 | 6930..6932 | 3 | none (06 starts at 6936) | 0 | 1 | 0x38 / 0 | 1936.0 |
| 06_hill_slide | 7141 | 7142..7144 | 3 | 07 f0..f2 equal | 16 | 0 | 0x38 / 0 | 1936.5 |
| 07_truck_preview | 7699 | 7709..7711 | 12 | 08 f4..f6, `yaw` differs | 0 | 0 | 0 / 0x38 | 1936.5 |
| 08_truck_crossing | 7944 | 7945..7947 | 3 | 09 f0..f2 equal | 20 | 1 | 0 / 0x38 | 1936.0 |
| 09_fence_door | 8477 | 8478..8480 | 3 | none (no beat starts from 09) | 0 | 1 | 0x38 / 0 | 1936.0 |
| 10_cage_roof_roger | 11524 | 11525..11527 | 3 | 11 f0..f2 equal | 14 | 0 | 0 / 0x38 | 1936.5 |
| 11_crevice_prompt | 12785 | 12786..12788 | 3 | none (12 starts at 12794) | 0 | 1 | 0x38 / 0 | 1936.0 |
| 12_crevice_jump | 13130 | 13139..13141 | 11 | 13 f8..f10, `yaw` differs | 22 | 1 | 0 / 0x38 | 1936.0 |
| 13_east_tower | 13940 | 13941..13943 | 3 | 14 f0..f2 equal | 0 | 0 | 0 / 0x38 | 1936.5 |
| 14_roger_encounter | 15759 | 15760..15762 | 3 | s1, s2 = 15 f0, f1 equal | 22 | 1 | 0x38 / 0 | 1936.0 |
| 15_level_exit (AREA01) | 16562 | 16563..16565 | 3 | none (no later beat) | 2 | 0 | 0 / 0x38 | 1936.5 |
| first_control (slot 04) | n/a | 4085..4087 | n/a | 01 f0..f2 equal | n/a | 0 | 0 / 0x38 | 1936.5 |
| route03_end | 5675 | 5676..5678 | 3 | 04 f0..f2 equal | 0 | 1 | 0x38 / 0 | 1936.0 |
| route07_end | 7699 | 7708..7710 | 11 | 08 f3 equal on the compared fields, f4..f5 `yaw` differs | 0 | 1 | 0x38 / 0 | 1936.0 |

Notes on the table:
- The alignment after the load takes 1..10 iterations. It happens while the
  emulator runs free before the pause, so it varies between runs.
- 07, 12 and route07_end run past the recorded next-beat's first input. The
  recorded beat moved the stick at f0; with the 2-frame pad latency the yaw
  differs from f4. Their outputs are therefore neutral-pad continuations of
  the original, not recorded route frames.
- The multi-vsync iterations (x-vs > 0) are load hitches. One iteration
  spanned 12..23 vsyncs right after the load. They vary between runs: the
  first full attempt at 00 took 8 alignment iterations and a 25-vsync step;
  the second had none.

**Field and interlace facts measured.**
- **Display setup.** Identical at all 19 outputs: PMODE 0x66, SMODE2 0x3
  (INT 1, FFMD 1), DISPLAY2 0x1BF9FF0203227C (DX 636, DY 50, MAGH 4, DW 2559,
  DH 447), DISPFB2 PSMCT32 512 wide, both contexts FRAME PSMCT32 512 wide,
  SCISSOR 0..511 × 0..223, ZBUF 0x31000070 (ZBP 0x70, PSMZ24), OFX 1792.0.
- **XYOFFSET mostly follows FIELD.** At the `s2` outputs of all 19 points,
  the FRAME buffer is drawn with OFY 1936.5 when CSR FIELD = 0 and with 1936.0
  when FIELD = 1, and the same holds at 52 of the 57 loop tops recorded. The 5
  exceptions are post-load or hitch loop tops: 04 s1 (6254: FIELD 0, OFY
  1936.0), 06 s0 (7142: FIELD 1, OFY 1936.5), 08 s0 (7945: FIELD 0, OFY
  1936.0), 10 s1 (11526: FIELD 0, OFY 1936.0) and 15 s1 (16564: FIELD 0, OFY
  1936.0); 06 and 08 are also reversed-pairing points (below), which may bear
  on the pairing rule. The steady case matches func_001D2300 building the next
  draw environment from (1 − field) (NEARMISS body).
- **Which FBP is shown per FIELD is not fixed.**
  - The section 5 table (FIELD 0 → DISPFB2 FBP 0) holds at 15 of 19 points.
  - 01, 06, 08 and 12 show the reverse: FIELD 0 → FBP 0x38, FIELD 1 →
    FBP 0.
  - The FBP alternates once per iteration (frame_idx) and FIELD once per
    vsync, so an iteration that spans several vsyncs can re-pair them. The
    four reversed points all had a load hitch, but 04, 10 and 14 had one too
    and kept the section-5 pairing. The rule is therefore not established.
  - The route snapshots of section 5 had no load between them and the
    recording.
- **CSR FIELD within one iteration.** In the steady second step, FIELD at the
  vsync-wait start equals FIELD at the ISR entry and at the next loop top at
  all 19 points. Whether it already differs from the iteration's own loop-top
  value varies: it does at most points, but not at 01 (0/0), 10 (0/0), 12
  (1/1) and 15 (0/0), nor in step 1 at 08. The vsync counter's parity is not
  FIELD (they differ at 04, 06, 08, 12 and 15).
- **Pixel alpha.** Alpha values over the 19 displayed buffers span
  0x00..0x80. Letterbox and cleared words are 0x80000000 (the displayed PNGs
  force alpha opaque; the .bin files keep it). PMODE has circuit 1 off, so the PCRTC outputs circuit 2's RGB; alpha
  plays no part in the displayed image.

**Repeatability.**
- **route03_end** (a second run of route 03's state) reproduced
  `displayed.bin`, `draw.bin` and `z.bin` byte for byte (sha256 prefixes
  67780e1eea360142, 5ca2a62ca01218e4 and 2e40ee210d0284c8). Only the
  mid-frame `s2a_wait` stage differed, because of the VU1 thread's progress
  at that sync.
- **route07_end** aligned one iteration earlier than 07 (7708 against 7709).
  At the common counter 7710, both buffers and Z were identical.

**Limits.**
- **Emulator, not hardware.** The software renderer is PCSX2's model of the
  GS, not real hardware. Bit equality with a real GS is not established by
  any of this. The data is the best available reference for the Original
  profile, not a hardware capture.
- **VU1 thread.** `vuThread = true` makes the timing of GS writes relative to
  EE breakpoints host-dependent; the syncs remove that from the outputs. The
  EE side matched the recorded rows wherever the pads agree.
- **Row mapping is inferred.** The link between a buffer and the row it shows
  (C−1 for displayed, C−2 for draw) rests on the VIF1 slot and the frame
  anatomy, not on a pixel comparison with an independent render.
- **Host screenshot.** It is PCSX2's 640×480 presentation (deinterlace mode
  0, scaled), so it supports only coarse comparisons.
- **Route 15 is AREA01**, not AREA11.
- **Not recorded.** No field was captured from real hardware. The CRT/PCRTC
  merge was not emulated beyond the registers.

## 6. H7: the panel prompt's module-0x21 load, per field, with the drive

The port's `docs/STATUS_LOAD_WAIT_PROBE.md` (tools/load_wait_probe.py,
2026-09-23) already measured the 24-dispatch wait of route 03 at the frame
boundaries. It left two things open, and this item adds them:
- the drive state per field;
- the writer of D_00275BD8 = 1, which appears one frame before the load
  request.

**What was recorded.** Route beat 03 (panel, BATTERY prompt) is re-driven by
route_capture's own closed-loop beat function. It starts from its recorded
source (`02_elevator_refusal/state.p2s`, reloaded through Pine) with one
neutral frame to the recorded first counter 4990. There are two passes:
- **fields.** At every main-loop top of the whole beat (f0..f685) the tool
  records:
  - loader slot 2's record 0x28A790 (0x20 bytes; +0 state, +9 step, +0xB
    sub-step, +0xE module, +0x14 count, +0x16 chunk);
  - D_00282154..5B (D_00282157 = the loader's read gate);
  - D_00275BD8;
  - the status UI bytes 0x810130;
  - the request block 0x8106B0;
  - spad 3B8C..93;
  - route_capture's row.

  Over f386..f418 it also records:
  - at each top, the IOP side: CDVD registers 0x1F402004..0F, IOP DMA
    channel 3 MADR/BCR/CHCR 0x1F8010B0..BB and the IOP PC;
  - one more full record (the same fields plus the IOP side) at every
    vsync-ISR entry 0x1AB140.
- **writer.** The same run, with an EE write memcheck on D_00275BD8 armed
  over f385..f392 only (before and at the request). Each hit records the
  store PC, ra and a backtrace (entry/pc pairs only). The pass stops at f392.

**How to reproduce.**
```
.venv/bin/python tools/c7cap_partb.py h7 --mode fields   # ~3 min
.venv/bin/python tools/c7cap_partb.py h7 --mode writer   # ~2 min
```

**Where.** `build/s87/c7cap/h7/<fields|writer>/`:
- `frames.jsonl` (per top);
- `fields.jsonl` (per ISR);
- `writes.jsonl`;
- `meta.json` (run summary; `changes` = every frame where the slot-2 state,
  step, sub-step, chunk, gate, BD8 or UI bytes change);
- `logs/`.

**Alignment.** In the fields pass, all 686 rows (f0..f685) are identical to
the recorded route 03 trace, with row 0 equal. The ISR pauses did not change
the route rows. They also did not change the load timing: the per-frame
slot-2 sequence equals the load-wait probe's unperturbed boundary table
exactly (request f391, step 2 at f398, sub 2 at f399, sub 3 at f408, steps
3/4/5/7 at f409..f412, idle and BD8 = 0 at f414). The writer pass was
identical through f392, where it stops.

**What it shows.** "In fN" is the ISR sample taken during frame N, after that
frame's loader dispatch. "Top N" is the state after frame N.

- **Who raises D_00275BD8.** There is exactly one memcheck hit in f385..f392.
  It happens during f390 (counter 5379 at the hit), store PC 0x20F06C in the
  ITEM root 0020EE50. The chain is 0020CDC0 @0x20D810 ← 0x1AE040 @0x1AE47C
  ← 001AD250 @0x1AD2DC ← 001ACEC0 @0x1ACFFC ← 001AB6A0 @0x1AB70C ← main loop
  @0x1AAF54. BD8 read 0 at the pause, so the pause comes before the store.
  So 0020EE50 sets BD8 one frame before its own 001FF080(0, 0x21) call in
  f391. At the same top (f390) the UI bytes become 0300 0500, and at f391
  0301 0500. This answers STATUS_LOAD_WAIT_PROBE.md's open "writer of that
  first 1".
- **Header read (0x800 bytes).**
  - The kick is in f391.
  - The drive status (0x1F40200A) is 0x12, with N-ready 0x8C and DMA3 CHCR
    bit 24 set, in the ISR samples of f391..f396: 6 fields.
  - It is 0x0A (N-ready 0x4C, CHCR bit 24 clear) from the ISR of f397 on.
  - The game's polls in f392..f397 return busy, and the poll in f398
    completes. The drive finishes inside f397, after that frame's dispatch
    and before its vsync.
- **Chunk read (0x50800 bytes, 161 sectors).**
  - The kick is in f399.
  - The status is 0x12 in the ISR of f399, then 0x06 in the ISRs of
    f400..f406 (7 fields), then 0x0A from the ISR of f407 on.
  - The polls in f400..f407 are busy, and the poll in f408 completes. Again
    the drive finishes inside the last busy frame, after its dispatch.
- **Payload read (0 bytes).** The kick is in f410, and the drive status does
  not leave 0x0A.
- **D_00282157** is 0 at every top and every ISR sample of the window.
- Every frame of the window is exactly one field. The ISR vsync count equals
  the top's, and there is one ISR per frame.
- **The 24 dispatches** (f391..f414) therefore consist of:
  - 6 + 8 frames in which the drive is busy past the frame's dispatch;
  - 3 completing polls;
  - 7 frames of state work.

  The busy frames are the fields between each kick and the drive's
  completion.

**What it does not settle.**
- **A timing law.** Only these two reads (1 and 161 sectors, one head
  position) are measured. Section 1's limits apply: a law for other modules
  needs more reads.
- **What the status codes 0x12 and 0x06 mean.** They are PCSX2's CDVD
  emulation states, not game state. The port needs the frame counts.
- **Real hardware.** Whether the same counts hold on a real PS2 cannot be
  shown by an emulator capture.
