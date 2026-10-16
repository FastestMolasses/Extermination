# Side-by-side video: original (PCSX2) vs native port

`tools/video_compare/` makes a side-by-side video of the original game in
PCSX2 and the native port, both driven by the same recorded inputs and synced
by game tick, with labels, a speed-up and an H.264 MP4 that plays inline in
Discord. It also writes a drift report: the first tick where the player's
position or heading differs between the two runs, which is where the port
diverges. The videos are silent by default. The legacy v2.6.3 PCSX2 cannot
record its audio while it is frame-stepped (see "Audio"); the project's
agent-debug fork can (`ps2.py --emulator fork --audio`, see "The fork
path"), and then `--audio both` puts the original's sound on the left and
the port's on the right. `--audio native` adds only the port's sound,
labelled as such.

Everything it writes (recordings, frames, audio, videos) is derived from your
own disc and stays in ignored `build/video_compare/`. Never commit any of it.
Posting a finished clip somewhere is your call; the repository only holds the
tool and this document.

## What runs where (macOS arm64)

| Piece | Where it runs |
|---|---|
| Native port (records and replays) | native arm64 macOS, `../extermination-port` |
| PCSX2 playback | default since 2026-10-09 (the user's decision to retire v2.6.3): the agent-debug fork (`../pcsx2-fork`, x86_64, Rosetta), hidden, through the fork's own `pcsx2dbg` launcher (`--emulator fork`); until the app is retired, `--emulator legacy`: the MCP-enabled PCSX2 v2.6.3 in `build/startup-reference/PCSX2.app` (x86_64, Rosetta), hidden, through `tools/pcsx2_session.py` |
| `tools/video_compare/*.py` | native arm64 Python 3; the PCSX2 stage uses the decomp `.venv` (it needs `zstandard`), the compose stage a `python3` with Pillow and numpy. `video_compare.py` picks the interpreter for each stage itself |
| ffmpeg / ffprobe | Homebrew arm64 (`/opt/homebrew/bin`) |

Nothing else needs Rosetta, and nothing needs the Linux container.

## Setup (once)

1. The port's input recording and playback (`src/game/em_replay.c`, the
   input filter in `src/em_input.c`, the offline audio output in
   `src/game/em_bgm.c`) are on the port's main since 5e68edf / b676245.
   Build it: `make -C ../extermination-port all`.
2. The decomp `.venv` (already used by `pcsx2_session.py`) and a `python3`
   with Pillow and numpy (the python.org 3.13 install has both). Check with
   `python3 -c "import PIL, numpy"`.
3. ffmpeg with libx264 and aac (`brew install ffmpeg`).

## 1. Record a session (you play the native port)

```sh
cd ../extermination-port
EM_INPUT_RECORD=../Extermination/build/video_compare/my_run.rec \
EM_PS2_DISC_DRIVE_TIMING=1 \
build/extermination
```

- Play normally, keyboard or gamepad. The recording starts at boot: let the
  logos run, start a **New Game** on the title menu, then play. Quit with Esc.
- Press START to skip the movies (the intro FMV and the New Game FMV). The
  playback skips every movie the recording skipped; an FMV you watch to the
  end plays in full on both sides (in real time) and is cut from the video
  anyway.
- `EM_PS2_DISC_DRIVE_TIMING=1` (recommended) makes the port's disc reads take
  the PS2 drive's measured time, so loads and voice lines take the same time
  as on the PS2 and segments line up by themselves (the New Game load veil:
  257 frames vs the PS2's 258, against 55 at host speed). Without it the
  tool still syncs, at every load and cutscene boundary.
- Keep it short: the PCSX2 side is slow (below), and shared PCSX2 time is
  limited.
- The PCSX2 side starts from your title save state (slot 01) and chooses
  NEW GAME itself; your title-menu inputs are not replayed there.

`python3 tools/video_compare/video_compare.py info build/video_compare/my_run.rec`
lists the recording's segments.

## 2. Make the video (one command)

```sh
cd ../Extermination
python3 tools/video_compare/video_compare.py all build/video_compare/my_run.rec \
    --name my_run --speed 2 \
    --title "Extermination (PS2) - original vs native port" \
    --subtitle "same inputs, synced by game tick" \
    --target-mb 10
```

Output: `build/video_compare/my_run/my_run.mp4`, plus `my_run_drift.txt`,
`my_run_drift.json` and `my_run_summary.json` next to it.

The command runs three stages, each also available on its own:

```sh
python3 tools/video_compare/video_compare.py native  build/video_compare/my_run.rec --out build/video_compare/my_run/native [--stride 4] [--audio] [--disc-timing recorded|0|1] [--sampling gs|area] [--binary PATH]
python3 tools/video_compare/video_compare.py ps2     build/video_compare/my_run.rec --out build/video_compare/my_run/ps2 [--stride 4] [--audio] [--emulator legacy [--frames gs|screenshot]] [--fork-title-delay N|auto [--fork-title-delay-runs 3]]
python3 tools/video_compare/video_compare.py compose --native build/video_compare/my_run/native --ps2 build/video_compare/my_run/ps2 --out my_run.mp4 [options]
python3 tools/video_compare/video_compare.py drift   --native build/video_compare/my_run/native --ps2 build/video_compare/my_run/ps2 [--out build/video_compare/my_run/drift]
python3 tools/video_compare/video_compare.py clean   my_run      # deletes build/video_compare/my_run/
python3 tools/video_compare/video_compare.py synth   build/video_compare/demo.rec [--spec script.json]
```

`synth` writes a recording from a small input script (the built-in demo, or
a JSON file with the same keys as `DEMO` in `video_compare.py`: the title
Cross tick, whether movies are skipped, and per segment its phase, length
and inputs). It is for tests: the segment lengths must match what the game
does, so play it on the port once (`native` stage) and use the playback log
(`<run>/native/native.rec`) as the real recording.

`all --skip-native` / `--skip-ps2` reuse a stage's output (e.g. to try other
compose options without running PCSX2 again).

### Options

| Option | Default | Meaning |
|---|---|---|
| `--speed S` | 2 | playback speed; frames are dropped, never blended |
| `--fps F` | 30 | output frame rate (29.97 at the default stride) |
| `--stride K` | from speed and fps | capture every K-th tick (both sides); `--speed 2 --fps 30` gives 4. Speed 1 at 30 fps needs 2, at 60 fps 1 (slow on PCSX2) |
| `--loads trim\|hold` | trim | loads: drop the longer side's middle load frames, or hold the shorter side's last load frame (tagged "held: the other side is still loading") |
| `--audio none\|native\|original\|both` | none | `native` = the port's sound only (the header says so); `original` / `both` need a PS2 `audio.wav`, which only the fork pass produces (`ps2.py --emulator fork --audio`; the legacy app writes none, see "Audio"); `both` = original LEFT, port RIGHT (each downmixed to mono) |
| `--disc-timing recorded\|0\|1` | recorded | `EM_PS2_DISC_DRIVE_TIMING` for the native playback |
| `--sampling gs\|area` | gs | native frame: the port's exact 512x224 GS field, with the presented frame averaged over each field pixel's footprint where the port's overlay pass drew (see "Limits"), like with like with the PS2 field; or the presented 4:3 frame box-averaged to 640x480 |
| `--ps2-frames gs\|screenshot` | gs | PS2 frame: the 512x224 GS field (needs the software renderer, switched for the session) or the save state's 640x480 host screenshot (hardware renderer, scaled and filtered by PCSX2) |
| `--title`, `--subtitle` | none | header text |
| `--height H` | 480 | height of each side (4:3, so 640x480 per side) |
| `--target-mb M` | none | two-pass encode to M x 10^6 bytes (Discord's free limit is 10 MB) |
| `--fork-title-delay N\|auto` | 0 | the PS2 stage's title delay (fork), as in `ps2.py`: `auto` picks the delay that enters the New Game movie on the port's frame index (see "The field phase"; which criterion `auto` should use is an open lead decision) |
| `--info` | off | footer with the segment, both counters and each side's drawn half line (OFY .0 / .5) |
| `--summary-seconds T` | 0 | end card with the drift summary |

## The recording format (`EMREC 1`)

A text file. Header lines start with `#`: `# EMREC 1`, then `# key value`
lines (`source`, `mode` = recording or playback-log,
`ps2_disc_drive_timing`, `tick_hz`, `env VAR=value` for launch switches that
change the game's path, which the playback re-applies), then the column
line `#cols ...`. One row per main-loop step:

| Column | Meaning |
|---|---|
| `step` | step index (the port's `em_frame_step` calls; on the PS2 side the tick index) |
| `counter` | main-loop counter 0x70003B64 |
| `mv` | 1 = a movie step (the blocking movie suspends the main iteration; the counter does not advance) |
| `ph` | sync phase: `T` title, `L` load, `C` cutscene/menu, `P` play, `M` movie step |
| `btn` | buttons, canonical DualShock bits (SELECT 0x0001 .. SQUARE 0x8000), hex |
| `lx ly rx ry` | stick bytes as the game unpacks them (0x80 = centre) |
| `area sub entry` | D_00810700..702 |
| `bd8` | loader busy byte D_00275BD8 |
| `s8d` | world-frame selector 0x70003B8D |
| `t9 tb` | slot-0 task record +9 (001AD250 state, 5 = load) and +B (0x1AE040 state) |
| `x y z yaw` | player position (0x00810350) and heading (0x00810374), float32 printed round-trip exact |
| `seg off` | the recorded segment the tick played and its offset in it (-1 outside playback) |
| `cap` | frame capture index (-1 = none) |
| `af` | native playback: audio frames written before this step (offline mixer) |

Phase of a tick, from the state the tick starts from (the original's
main-loop top 0x001AAF28): `T` if D_00810700 == 0; `L` if D_00275BD8 != 0 or
the slot-0 task's +9 == 5; `C` if 0x70003B8D != 0; else `P`. A segment is a
run of ticks with one phase.

## How the sync works

- **Tick.** One main-loop iteration (counter 0x70003B64) on both sides. The
  port's playback is deterministic: replaying a playback log reproduces every
  tick bit for bit, paced or uncapped, whatever the number of movie steps.
- **Segments.** Inside a segment the recorded pad at offset k is applied at
  offset k. At a phase change each side takes the next recorded segment on its
  *own* first tick of that phase: each side waits for its own load, cutscene
  or menu to end. A phase change the recording does not have at that point
  (a flicker of one or two ticks) continues the current segment's timeline;
  recorded segments of at most 8 ticks may be skipped to reach a matching
  one. Past a recorded segment's end the last pad is held; after 1800 ticks
  of overrun the run stops (desync).
- **Movies.** On the PS2 the movie runs inside one main-loop iteration, so
  the tool sets START as the raw pad when it sees D_00821058 == 1 there; the
  port holds START on its movie steps. Movies are not in the video.
- **Title.** The PS2 side starts from the user's title save state (slot 01)
  and confirms NEW GAME itself (Cross while the fade is idle, as
  `route_census.py` does); the video starts at the first tick after the
  title phase (D_00810700 != 0).
- **PS2 input, same tick.** `pad_set` alone reaches the game's pad block two
  iterations late. The tool therefore also stops at the main loop's return
  from the input step (the call to 0x001B57E0, found in the user's ELF) and
  writes the processed pad block the port computes from the same pad
  (0x00810E57 gait, 0x00810E64..67 sticks, 0x00810E70..7B held / previous /
  pressed / previous pressed / repeat / repeat timer, the rules of the
  port's `em_pad_unpack`, i.e. original 001B5940 on an analog pad). The raw
  pad is still set, for raw readers such as the movie skip. Each PS2 row
  also logs the block the original computed itself, for checking the model.
- **Frames.** The port captures its frame at the end of step t (it presents
  the frame its tick built). On the PS2 the field built from tick t is drawn
  in iteration t + 1; at the main-loop top of tick t + 3 it is the drawing
  context's FRAME buffer, the field on screen, untouched until that
  iteration draws (docs/CAPTURES_C7.md 5b). A save state is taken there and
  its GS freeze decoded (PSMCT32 de-swizzle; checked byte for byte against
  the C7 `draw.bin` captures). The displayed buffer (DISPFB2) one loop top
  earlier is the same field, but in the test its letterbox bands were
  sometimes still missing (written late in the frame), so the tool uses the
  later point. Frames are captured at offsets that are multiples of the
  stride in every segment, so both sides capture the same offsets.
- **Video.** Play and cutscene segments are paired by offset; a side whose
  segment is shorter holds its last frame ("held"). Loads follow
  `--loads`. Every output frame stands for `stride` ticks (or a multiple
  when `--speed` asks for more); at `--fps` that is the speed-up.
- **Audio.** Each output frame takes its own side's audio for the ticks it
  stands for; held frames are silent. The port renders its mixer offline
  (no audio device; 800.8 frames per step at 48 kHz, pulled through the
  current field since port a345d9a, which removed a one-field lag of every
  music and voice sample; column `af`). With `--speed` > 1 the
  audio is sped up with ffmpeg's `atempo` (pitch kept), so sound and picture
  stay in sync.

## The PCSX2 session

This section describes the legacy v2.6.3 app (`--emulator legacy`, until the
app is retired). The agent-debug fork is the default since 2026-10-09 (next
section).

- Takes `build/.pcsx2.lock` (mkdir; polled every 30 s) for the whole
  session and always removes it, also on failure.
- Switches, only while it holds the lock and only while PCSX2 is stopped:
  `[EmuCore/GS] Renderer = 17 -> 13` (software renderer, for GS pixels;
  user decision 2026-09-26) and, with `--audio`,
  `[SPU2/Debug] Global_Enable` and `Log_WAVE_Output` (user decision
  2026-10-01), and with `--media-audio` (below) `[EmuCore/GS]
  EnableVideoCapture = false`, `EnableAudioCapture = true`,
  `CaptureContainer = wav` and the added hotkey `[Hotkeys]
  ToggleVideoCapture = Keyboard/End` (removed again afterwards; user
  decision 2026-10-09). It keeps a copy of the ini and restores exactly these keys
  after PCSX2 has exited (PCSX2 rewrites its ini on exit), then writes the
  remaining difference from the pre-session copy to `ps2/ini/`.
- Save states go to a free slot >= 40 and are moved out at once; slots
  01..15 are never written; the source state is hashed before and after.
- PCSX2 runs hidden (visible only with `--media-audio`) and is closed at
  the end (`no emulator process left`).

## The fork path (`ps2.py --emulator fork`, the default since 2026-10-09)

The project's agent-debug PCSX2 fork (`docs/PCSX2_FORK.md`; its guide
`../pcsx2-fork/EXTERMINATION.md`) replaces the save-state-per-frame and
breakpoint-per-tick machinery. Every row of `ps2_extra.json` records the
game's frame index D_00810E80 (`frame_idx`) and field D_00810E88 (`field`),
and `info.title` the commit's; every captured row's `frame.drawn` the half
line its field was drawn on (see "The field phase"). `--fork-title-delay N`
holds the title driver back N ticks, which moves the New Game commit, to land
a run on a reference's phase; `--fork-title-delay auto` chooses N itself so
that the New Game movie is entered on the port's frame index (see "The field
phase"). `tools/video_compare/ps2_fork.py` drives it
through the fork's DebugServer and its MIT Python package `pcsx2dbg` (no
emulator source is used). The game-side rules are ps2.py's: the same
scheduler, pad block model, phases, title driver and movie handling.

```sh
# macOS arm64 host (the fork is x86_64 and runs under Rosetta); decomp .venv via the dispatcher
python3 tools/video_compare/video_compare.py ps2 build/video_compare/my_run.rec \
    --out build/video_compare/my_run/ps2_fork --stride 4 --audio --emulator fork
python3 tools/video_compare/video_compare.py compose --native build/video_compare/my_run/native \
    --ps2 build/video_compare/my_run/ps2_fork --out my_run.mp4 --audio both [options]
```

- **Start: a cold boot.** The fork does not load v2.6.x states, so it boots
  the user's disc image (APFS-cloned into scratch) with a fixed RTC
  (2026-01-01 00:00:00), plays the intro movie, waits for the idle title and
  confirms NEW GAME with ps2.py's title driver (one Cross; the New Game
  movie skipped with START when the recording skipped it). It passes no
  `-elf` override. (An `-elf` boot failed on 2026-10-09 because the scratch
  path ran through the `/var` symlink; the fork's launcher passes real
  paths since fork commit `1e22fdc0c`, and a cold boot with `-elf` now
  reaches the game.) Boot to title takes about 78 s.
- **Ticks.** `run {until: {ticks: 1}}` with the tick PC at the loop top
  0x001AAF28 and a persistent stop probe at the return from the input step
  (`ps2.post_input_pc()`), where the processed pad block is written with
  `mem_write`. The raw pad goes through v1 `pad_set`.
- **Movies are caught at an emulated point.** A second stop probe sits at
  the entry of the blocking movie driver func_00203350 (`docs/STARTUP.md`);
  START is set there. ps2.py's host-polled hook sets START whenever the
  0.5 s poll notices the movie: two fork runs with that hook committed New
  Game 3 vsyncs apart, and every later field differed between them (the
  game state did not). With the probe the pass is repeatable. A second
  run of the first 2,000 demo_level ticks equalled the full run in every
  row, every EE and IOP cycle and vsync, all 503 frames (byte for byte)
  and every audio sample (2026-10-09).
- **Fields.** This is ps2.py's capture point without save states. At the
  loop top of tick t + 2 the tool reads DISPFB2 as the EE wrote it
  (0x12000090, fork region `gs_priv`); that buffer holds the field built from
  tick t. At the loop top of t + 3 that buffer is the drawing context's
  FRAME buffer, untouched until that iteration draws, and the tool reads its
  56 pages from GS local memory (`mem_read` space `gs`) and de-swizzles them
  (`ps2.word_map`). The software renderer (13) is required. In the
  demo_level run the result equalled the fork's displayed field (`gs_field`)
  at loop top t + 2 on all 3,578 captures, so no write lands late.
  `--fork-field-k K` reads `gs_field` at loop top t + K instead, and
  `--fork-extra-k` adds such reads to `frames_k<K>/` (diagnostics only).
  **Do not use `gs_field` at t + 3:** it shows the field of tick t + 1.
- **Audio (`--audio`).** The fork's emulated-time SPU2 tap (`audio_tap`):
  48 kHz s16 stereo, as the SPU2 mixed it, before the host's filters. It is
  started at the title. At every logged loop top the tool reads the tap's
  sample count (`audio_tap status`) and stores it in the row's `af` column,
  relative to the first recorded tick, as the port does. `ps2/audio.wav` is
  cut from the tap to exactly the logged span: sample 0 is the first
  recorded tick's loop top, and the WAV ends at the last logged loop top.
  `ps2/audio_sync.json` checks it. Each loop top's count is within -1.2 to
  +0.8 samples of the SPU2 clock (one sample per 768 IOP cycles from the
  tap's first mark). compose.py places the PS2 audio by `af` whenever a
  run has it; legacy runs keep the vsync-counter placement.
- **Presentation stays on by default** (frames are copied to the hidden
  window; it only paces the run). `--fork-no-present` turns it off. Fork
  builds before server 0.2.2 leaked GPU memory with presentation off and
  aborted after long free runs or movies (twice on 2026-10-09: after about
  9 min, and about 10 s into the level-exit movie E001). Fork 0.2.2
  (`5a3048e8e`) fixed it: the footprint stays flat over 36,000 vsyncs, so
  the switch is safe from 0.2.2 on (`hello.server_version`).
- **Lock and safety.** pcsx2dbg's `Session` takes `build/.pcsx2.lock` (polled
  every 3 s: other lanes run short sessions back to back). It launches
  hidden on scratch under `$TMPDIR/pcsx2-fork-vc` (BIOS and ini copies),
  shuts the instance down and deletes the scratch. It never touches
  `portable-data`, the memory cards or slots. SIGTERM closes the session
  too. The emulator log is kept in `ps2/pcsx2_logs/`. Since fork 0.2.2 the
  shutdown ends in about 0.5 s; before it, the process did not end within
  20 s after a long closed loop and the launcher terminated it.
- **Output.** The same as ps2.py (`ps2.rec`, `ps2_extra.json`, `frames/`),
  plus `audio.wav` and `audio_sync.json`. `ps2_extra.json` adds `info`
  (boot, title, movies with their vsync, play, end, shutdown) and, per row,
  the agent vsync, EE and IOP cycles, the tap count and the game's frame
  index 0x00810E80.
- **Speed.** About 60 to 75 ticks/s closed loop, about 6 min per session for
  the whole first level (78 s of it the cold boot), against 44.7 min on
  v2.6.3.

## Audio

- **Native.** During playback the port has no audio device
  (`EM_REPLAY_AUDIO`): its mixer (SFX voices, startup sequencer, the stream
  lanes' SPU output) is pulled on the game thread, 800.8 frames per
  main-loop step at 48 kHz, into `native/audio.wav`. It is tied to the game
  tick, not to a host clock.
- **PCSX2 fork.** The original's sound in emulated time, sample-exact per
  loop top: see "The fork path". The rest of this section is about the
  legacy v2.6.3 app.
- **PCSX2 v2.6.3.** Not available in this build. The SPU2 debug WAV log
  (`[SPU2/Debug] Global_Enable` + `Log_WAVE_Output`, switched on for the
  session with `--audio original|both`, then restored) wrote no file in two
  sessions on 2026-10-01 (frame-stepped and free-running). PCSX2's own
  video capture (its ini has `EnableVideoCapture` / `EnableAudioCapture`)
  is started from the UI or a hotkey only (none is bound), which needs a
  visible, focused window; the DebugServer's command list and Pine have no
  capture command. The live output (`[SPU2/Output] Backend = Cubeb`,
  `SyncMode = TimeStretch`) runs on host time and stops and stretches while
  the emulator is frame-stepped, so recording it would not stay in sync.
  The PS2 side is therefore silent and the default video has no sound
  track.
- **PCSX2 Media Capture (`ps2.py --media-audio`, opt-in).** The user
  allowed a visible PCSX2 session for recording audio (2026-10-09). With
  `--media-audio` the session runs visible with the Media Capture keys
  above (audio only, WAV) and stops twice with the VM held at a main-loop
  top: at the first recorded tick it writes `ps2/audio_start.ready` and
  waits for `ps2/audio_start.go`; after the last tick it writes
  `audio_stop.ready` and waits for `audio_stop.go` (`--media-wait`,
  default 1800 s; without a go the side stays silent). Between `.ready`
  and `.go` the capture is started / stopped: the End key sent to the
  PCSX2 window (computer-use `app_key`, background) toggles it. Afterwards
  the WAV is moved to `ps2/audio_media.wav`, `ps2/audio_sync.json` compares
  its sample count with the gates' vsync span (48000 / 59.94 samples per
  field), and `ps2/audio.wav` is written only if they agree within one
  field (the WAV then ends at the last logged loop top, which is where
  compose.py anchors it). `ps2_extra.json` keeps the gates (`audio_gates`)
  and the DebugServer `cycles` value at every loop top.
  - **Result 2026-10-09 (demo_hill): no WAV.** PCSX2's menu items (System >
    Video Capture, Screenshot, every Settings item) report disabled when
    pressed through the menu bar while PCSX2 is in the background, VM paused
    or running alike; taking over the screen to use the menu was stopped by
    the user. The End hotkey does reach PCSX2 in the background, also while
    the VM is held at a breakpoint, but the capture then fails with "Failed
    to load FFmpeg" (this build asks for libavcodec 62, libavformat 62,
    libavutil 60, libswscale 9, libswresample 6). Homebrew's FFmpeg 8.1.2
    (`/opt/homebrew/opt/ffmpeg/lib`) has exactly those versions, and
    `DYLD_FALLBACK_LIBRARY_PATH` pointing there reached the emulator
    process, but the load still failed. The reason (found the same day,
    `lipo -archs`): this PCSX2 build is **x86_64 only** and runs under
    Rosetta, while Homebrew's FFmpeg 8 in `/opt/homebrew` is **arm64 only**
    (all five libraries); dyld never loads a library of another
    architecture into a process. Media Capture therefore needs an **x86_64
    FFmpeg 8** (libavcodec 62 / libavformat 62 / libavutil 60 / libswscale 9
    / libswresample 6), e.g. an x86_64 Homebrew under `/usr/local` or a
    universal build; an arm64 PCSX2 would take the existing one. The user
    chose Homebrew FFmpeg (2026-10-09); which x86_64 build to use is still
    the user's decision (nothing was installed, upgraded or copied into the
    app). Whether the capture follows emulated time while frame-stepped is
    therefore still unmeasured.
  - **`ps2.py --emulator-lib-path DIR` (opt-in).** Passes DIR to the
    emulator process only, as `DYLD_FALLBACK_LIBRARY_PATH`
    (`pcsx2_session.OriginalSession(env=...)`: `Popen(env=...)` for a
    visible session, `open --env` for a hidden one); dyld looks there only
    for libraries PCSX2 does not find itself. Before the session it checks
    that DIR holds `lib<name>.<major>.dylib` for the five libraries above
    and that they share the emulator's architecture (`lipo -archs`), and
    refuses otherwise: `/opt/homebrew/opt/ffmpeg/lib` is refused with
    "the emulator is ['x86_64'], these libraries are not". Without the
    option the launch is unchanged. Nothing in the emulator install is
    written (it only reads the emulator binary's architecture).
  - The DebugServer's `cycles` advanced about 325,600 per field in that
    session, not the EE's 294.912 MHz / 59.94 = 4.92 million, so it is not
    a plain EE cycle count; the sync check uses the vsync counter.
- `--audio native` puts the port's sound in the video and writes
  "sound: native port only" in the header, so nobody mistakes it for the
  original. `both` (original LEFT, port RIGHT, one track that Discord's
  inline player can play) and `original` need `ps2/audio.wav`, which only
  the fork pass writes; without it they fall back to `native` / silent and
  say so in the summary.
- **Speed-up.** The audio covers the same game ticks as the frames and is
  sped up with ffmpeg's `atempo` (pitch kept) by the same factor, so it
  stays in sync at any `--speed`; held frames are silent.

## The drift report

For every tick both runs played at the same (segment, offset) it compares the
player position and heading: how many are bit-identical, the first tick past
the tolerance (default 0.01 units, 0.001 rad) and the largest difference. It
also lists every segment's length on both sides; different lengths show where
loads, cutscenes or behaviour differ. Since 2026-10-16 it also reports the
field phase per segment (next section).

## The field phase

The same tick drawn on the other half line differs in every edge and
vertical gradient (docs/PCSX2_FORK_GS_DIFF.md 5.2 and 5.3): about 29 % of
the old demo_level's paired frames were half a line apart, which the pairing
by tick could not see. The tool now logs, for every captured row, the half
line each side drew its field on, and reports per segment whether they
agree.

- **What is compared.** The drawn XYOFFSET_1 of the field: OFY on a whole
  pixel (line 0, e.g. 1936.0) or on a half pixel (line 1, 1936.5). Every
  draw environment of the frame list carries it; it is not inferred from a
  rule.
- **Port side** (`native/frames.json`, per capture: `line`, `ofy`,
  `xyoffset`, plus `counter`, `seg`, `off` and `uniform`): the XYOFFSET_1
  the port's GS field was drawn with, from its `.present` (checked against
  the port log, as before). A capture without a GS field has none (`line`
  null): the status / ITEM / BATTERY pages, the load veil's ends, and the
  black frame the port presents on an iteration that draws nothing.
  `uniform` names the colour of a frame of one colour.
- **The port's mapping, checked on every run.** On the port the frame
  index D_00810E80 of an iteration is its counter's low bit (em_frame.c
  starts both at 0, step W flips the one and increments the other, nothing
  else writes either; the recorder logs `em_frame_counter()`), step P sets
  the field from it, and step V (em_frame_kick.c, as 001D2300) draws with
  the half pixel when 1 - field is set: a tick's field sits on line
  1 - (counter & 1). `native.py` checks this on every field capture and
  prints and stores the result (`frames.json` `port_phase`).
- **PS2 side** (`ps2/ps2_extra.json`, per captured row: `frame.drawn` with
  `line`, `ofy`, `xyoffset1`, `slot`; per row as before `field` =
  D_00810E88 and `frame_idx` = D_00810E80 at the loop top). Step V
  (func_001D2300, NEARMISS C at 80.44 %; the linker fills it from the asm)
  sets the two contexts' draw environments of the frame list's packet slot
  with the SDK setters func_001015A8 / func_00101810 (both link from their
  asm bodies; readable NEARMISS companions in `src/readable/`, 49.26 %):
  D_00275674 + slot * 0x190 + 0x40 and + 0xC0,
  slot = D_00275670 +0x9C, XYOFFSET at +0x20 with the half pixel when
  1 - D_00810E88 is set. Each block is (value, A+D register address) pairs,
  and the tool checks the addresses on every read (FRAME_1 0x4C /
  XYOFFSET_1 0x18, FRAME_2 0x4D / XYOFFSET_2 0x19). At the loop top of
  t + 2, where it reads DISPFB2 (the buffer that holds tick t's field), the
  two slots hold the lists of ticks t and t + 1, which draw into the two
  buffers, so the slot whose FRAME_1 FBP is that buffer is tick t's; its
  XYOFFSET_1 is the drawn one. The layout is cross-checked at run time on
  every captured row: the register-address words must be the ones above,
  exactly one slot may draw into the shown buffer, that slot must not be
  +0x9C's (the list built last, tick t + 1's), and context 2's FRAME and
  XYOFFSET must equal context 1's; a row that fails gets no `line` (with
  the reason). The pass prints the totals and stores them as
  `ps2_extra.json` `info.drawn_check` (captured, resolved, not_slot_9c,
  ctx2_equal, fallback). On the 2026-10-16 test recording (the review
  re-run on port 92786d2, `build/video_compare/review_phase_1016/ps2/ps2_extra.json`)
  all 419 captured rows resolved to one slot, none was +0x9C's, context 2
  agreed on all 419, no fallback. `uniform` marks a buffer of one colour.
- **The report** (`<name>_drift.txt` / `.json`, key `field_phase`; also
  `drift --out`): per segment the frame pairs at the same offset, how many
  carry a field on both sides, how many of those are on the same and on the
  other half line (with the offsets), and the offsets where either side
  has no field or a frame of one colour. Runs made before these records
  count as unknown. `compose --info` prints both halves in the footer.
- **`--fork-title-delay auto`.** Both phases come from timing, not game
  state: the frame index follows the iteration count, the half line the
  vsync count. At the New Game movie (00203350 plays it inside one
  main-loop iteration) the original reads field 1 at step P after every
  movie at host speed (002061B0 opens the display only after a field-0
  vblank, and 00203350 ends with two vblank waits that return field 0, then
  field 1; fork probes of 2026-10-16, receipts in the ignored
  `build/fork_refs/probes_1016/movie_end/`), so its relation afterwards follows D_00810E80 at the movie's
  entry; the port hard-codes field = frame index. With `auto` the PS2 pass
  takes the port's frame index at the New Game movie's entry from the
  recording (`emrec.new_game_movie`: the counter's low bit of the movie's
  iteration), runs delay 0, and stops at the movie driver's entry probe
  (an emulated point) if the fork's D_00810E80 there differs: the session
  is closed and the next delay tried, up to `--fork-title-delay-runs`
  (default 3) sessions (the cost of a wrong delay is one cold boot and the
  title, about 1.5 to 2 min). The attempts and the chosen delay are written
  to `ps2/title_delay_scan.json` and `ps2_extra.json`
  `info.title_delay_auto`, the delay also to the `ps2.rec` header
  (`title_delay`); each failed attempt's emulator log is kept as
  `pcsx2_logs/emulator_delay<N>.log`. When no delay matches the pass fails
  and says so. `video_compare.py all` keeps delay 0 by default, as
  `ps2.py` does (pass `--fork-title-delay auto` to scan) until the lead
  decides which criterion `auto` should use (see the test below).
- **What a match does not fix.** Equal frame index at the entry gives the
  same relation after the New Game movie only if both sides then run the
  same number of iterations: a load of another length by an odd count, the
  level-exit movie (E001 runs on the fork's vsync count, docs/PCSX2_FORK_GS_DIFF.md
  5.3a) and an iteration that draws nothing (the PS2 repeats an older
  field, the port presents black) move one side's phase; the report shows
  where.

## Limits and known differences

- **Random effects differ.** The port's rand() stream position differs from
  the original's (the order of rand calls diverges after a few hundred calls,
  docs/CAPTURES_C7.md section 3), so snow flakes, flames and other random
  sprites are not the same.
- **Areas the port has not reached fault.** A missing original worker stops
  the port (fail-stop); its stream then ends and the video holds its last
  frame ("held") while the PS2 side goes on.
- **The port side is the port's GS field, plus its GPU overlay pass.**
  Since the port's chain step GSFRAME every world frame of the Original
  profile is a 512x224 GS field (port `docs/GS_EXACT.md` section 9). A
  capture of such a frame also writes the field itself
  (`<capture>.gsfield`, 512x224 RGBA) and, since the port's presentation
  merge (3d403ac: field choice (a), SCREEN ADJUST (a); GS_EXACT.md section
  11), `<capture>.present`, a short text file with the field's XYOFFSET_1
  and the constants its f_gsfield shader placed it with (game rectangle
  origin, 512 / width and 448 / height, the shift in pixels and lines with
  the field's line added; since port 0eea8b5 also the
  overlay pass's viewport, `overlay x y w h`, before the last line,
  BGCOLOR). Port `em_gfx_metal.m` writes and closes
  the BMP first, then the field, then the `.present`, on the same thread.
  `native.py` makes `frames.fifo.gsfield` and `frames.fifo.present`
  symlinks to the capture FIFO, so each capture arrives as BMP, field,
  `.present` in one ordered stream (no side file to overwrite, no race).
  After the run it checks the parsed sequence against the port's
  `capture: wrote ...` log lines and every `.present`'s XYOFFSET_1
  against the one logged with its field.
  - **The mapping** (port `src/gs/em_gs_display.h`, the shader's float32
    expression, as the port's `tools/check_present_capture.py` uses it):
    each field row covers two of the 448 display lines, a field drawn with
    the half-line draw offset sits one line lower, and SCREEN ADJUST moves
    the whole picture. The frame pixels that show a field pixel are its
    footprint.
  - **`--sampling gs` (default).** A frame is the exact field (RGB; alpha is
    not displayed, PMODE EN1 0), except the field pixels whose footprint
    the port's overlay pass drew over (it still draws after the field: port
    FIRST_LEVEL_AUDIT 1b item 2, GS_EXACT.md 10.1): those are the presented
    frame averaged over the footprint (a box filter). Frames without a
    field (the status / ITEM / BATTERY pages, the load veil's first and last
    frames, the loads between areas, single frames at some phase changes)
    are the presented frame box-averaged over the footprints of the default
    placement (no `.present` exists for them; a run whose `.present` files
    show another placement is reported). A footprint pixel within 1e-3 of a
    texel edge may show either neighbour (GPU rounding), which alone is not
    counted as overlay. The SCREEN ADJUST shift moves the picture in the
    window, not the field, so the comparison frame stays the field, like
    the PS2 side's GS field. `native/frames.json` lists per capture whether
    it had a field, its line, its placement and the overlay's share.
  - **Text.** Overlay glyphs are drawn at host resolution. Until the box
    filter (2026-10-09) they were sampled once per field pixel (its
    centre), and thin strokes dropped out ("Found: BATTERY PACK" read as
    "l ound: BAI IEKY PACK", the "F" of "First"); averaged over the
    footprint every stroke keeps its weight (`check_text.png`, below). The
    PS2 draws them into the field itself, so they stay slightly softer on
    the port side.
  - **Port finding (fixed): the overlay pass ignored the field's half
    line.** At port 3d403ac / f1e589c the port drew the overlay pass in the
    placed picture without the field's line (`gsw_viewport` used the
    SCREEN ADJUST shift only), while a field drawn with the half-line
    offset is shown one display line lower. The PS2 draws its letterbox
    bands and subtitles into the field, at whole field rows. So on
    half-line fields (355 of the demo's 1,048) the bands and text sat one
    display line (half a field row) above the picture: the overlay covered
    65 field rows there instead of 64 (overlay share 29.0 % instead of
    28.6 %), and the field rows at the band edges (31 and 191) were half
    band, half picture. Fixed by port 0eea8b5 (the overlay
    pass is placed with the line of the field it is drawn over, from that
    field's XYOFFSET_1, port GS_EXACT.md section 11): the re-run below
    shows 28.6 % on both field lines. Drawing the overlay pass through the
    GS model itself stays port FIRST_LEVEL_AUDIT 1b item 2.
  - Lighting stand-ins and the other known differences (port
    `docs/FIDELITY_FEATURES.md`) remain visible.
- **Port audio** is the dry SPU2 model (no reverb).
- **PCSX2 is not hardware.** The software renderer is PCSX2's GS model.
- **Loads.** At host speed the port loads in a fraction of the PS2's time;
  `--loads trim` hides that, `--loads hold` shows it,
  `EM_PS2_DISC_DRIVE_TIMING=1` removes most of it.
- **First two ticks after a long segment boundary** are exact on both sides
  (the pad block is written per tick); only raw-pad readers (the movie skip)
  see the two-iteration libpad latency.
- **Cost.** The PS2 side takes a save state per captured frame (software
  renderer); see the test numbers below for the rate.

## Test (2026-10-01)

Short footage only, deleted afterwards (frames, recordings, videos). The
input came from `video_compare.py synth` (the built-in demo script: New Game,
both movies skipped, the AREA11 opening skipped with START at offset 300, a
600-tick walk with stick turns), played once on the port to get a full
recording with anchors; that playback log was then used as the recording.

- **Port determinism.** Replaying the playback log reproduced all 2384 ticks
  bit for bit (phase, pads, area, loader byte, selector, task bytes,
  position, heading, segment/offset), uncapped (13,082 movie steps) and at
  real-time pace (65 movie steps). With `--disc-timing 1` the same
  recording re-synced on its own: title 1440 ticks instead of 1380, the load
  266 instead of 64, the cutscene and play segments unchanged, the same end
  position.
- **Pad model.** emrec.PadBlock equals the port's `em_pad_unpack` on 20,000
  random pad steps. In PCSX2 the original's own unpack (read at the
  post-input breakpoint before the write) equalled the model on 1194 of 1209
  ticks; the 15 others are the two ticks after each pad change (the libpad
  latency the write removes) and one tick where PCSX2 reported the sticks as
  0x7F.
- **test1** (recording made at host speed): PCSX2 session 157 s for 1209
  ticks and 304 captured fields (about 0.3 s per save state; the lock wait
  is extra). Segments native / PS2: P 1/1, L 64/269, P 4/4, C 335/335,
  P 600/600. Position and heading bit-exact on all 940 play/cutscene ticks
  (no drift). Video: 1284x604, 29.97 fps, 342 frames (11.4 s at 2x, 3 s end
  card), H.264 + AAC (`--audio native`), 2.9 MB; the PS2 load shows the tag
  "load trimmed: 205 ticks longer". Mean absolute pixel difference per
  channel between the two fields: 3.2 in the walk, 9.3 in the opening
  (test1 and test2 used the port's Metal world and the old edge sampling,
  see "Demo run 2026-10-09").
- **What test1 shows about the port.** At host speed the port's opening
  fades in about 20 ticks (0.33 s) before the original's (image from C
  offset 16 vs 36) and its first subtitle ("Dennis here.") appears 20 ticks
  earlier (offset 36 vs 56); the camera shots themselves match, and later
  in the opening ("Where are you?") the subtitles line up too. The walk is
  identical in position and heading.
- **test2** (the same inputs recorded with `EM_PS2_DISC_DRIVE_TIMING=1`,
  `--loads hold`): PCSX2 session 212 s for 1210 ticks and 304 fields.
  Segments native / PS2: P 1/1, L 266/270, P 4/4, C 335/335, P 600/600;
  940 play/cutscene ticks bit-exact. Video: 304 frames, 10.1 s, 2.8 MB.
  Mean absolute pixel difference per channel: 0.17 in the load (the veil's
  particles line up), 6.6 in the opening, 0.95 in the walk. Shifting the
  native frames by -4 / +4 ticks raises the walk's difference from 1.0 to
  6.8 / 6.8, so the pairing is on the exact tick. With the drive timing the
  opening's lead shrinks from 20 to 12 ticks (fade-in at offset 24 vs 36,
  first subtitle at 48 vs 60): the remaining lead is a port difference
  inside the opening, not load or voice-read time.
- PCSX2's SPU2 WAV log wrote no file with the switches on (here and in the
  audio lane's probe of the same day), so `--audio original|both` falls back.
- Every session ended with `no emulator process left`, the ini keys
  restored (remaining difference from the pre-session copy: none, or PCSX2's
  own `[GameListTableView] HeaderState` blob), the lock released.
- One session lost its emulator about 5 s in (DebugServer connection
  refused, no crash report, the log just stops); the tool restored the ini,
  released the lock and exited with an error. A rerun is the remedy.

## Demo run 2026-10-09 (demo_hill)

New Game to the hill below the elevator: 4,734 ticks recorded from the
level smoke (`EM_NEW_GAME=1`, `EM_PS2_DISC_DRIVE_TIMING=1`), stride 4,
`--audio native`, `--speed 2 --fps 30 --height 480 --summary-seconds 3`,
the title and subtitle in the header, `--target-mb 10` for the Discord
copy. PCSX2 vs port: 4,345 of 4,346 play/cutscene ticks bit-exact in
position and heading. Output in `build/video_compare/demo_hill/`.

- **Blocky port frames: a sampling bug in this tool, fixed the same day.**
  The port's image was sharp; `native.py` sampled the presented 1920x1440
  frame at each field pixel's left / top edge (`floor(vx + x * vw / 512)`,
  the rule `test_fb2_pixels.py` uses for a Metal-drawn world, not for a
  field spread over the window). With a field pixel 3.75 x 6.43 host pixels
  large that point falls in the previous field pixel for most columns and
  rows, so about half of them were duplicated and others dropped. Since the
  fix the port side is the exact GS field (above). The field
  presentation choice (LAUNCHER_OPTIONS.md) was not involved: all its
  options show the same 512x224 field.
- **Measured over the 1,171 paired frames** (identical neighbours, a
  measure of blockiness; held frames excluded): vertical 0.587 -> 0.361
  (PS2 0.366), horizontal 0.529 -> 0.409 (PS2 0.411). In the walk
  segments vertical 0.52 -> 0.19..0.24 with the PS2 at 0.19..0.24. Mean
  absolute difference per channel to the PS2 frame, all frames 4.62 ->
  3.70; per segment (old -> new): opening cutscene (4) 6.23 -> 5.70, first
  play (5) 4.95 -> 3.92, cutscene 6 10.10 -> 8.59, status pages (8) 6.04 ->
  3.77, play 9 4.85 -> 2.96, 10 3.35 -> 2.27, 11 3.65 -> 2.23, 12 4.71 ->
  4.30, 14 4.31 -> 2.68, 15 3.80 -> 2.95, 16 4.26 -> 2.72, the hill (17)
  3.81 -> 3.26; loads 0.05 -> 0.00. 1,048 of 1,171 captures carry the
  field; the overlay pass drew over 597 of them. The playback reproduced
  the recording's 4,734 ticks bit for bit (and the earlier playback's
  segment / offset / capture columns).
- Check images (ignored): `check_closeup_before.png` /
  `check_closeup_after.png` (segment 4 offset 288, field box x 180..340,
  y 40..150, PS2 left, port right, 4x nearest).

### Re-run 2026-10-09 after the port's presentation merge

The native stage was run again on port main f1e589c (field choice (a) and
SCREEN ADJUST (a)) with the box-filtered overlay (`--stride 4 --audio`,
drive timing as recorded), and both videos recomposed from it and the
same `ps2/` (`--skip-ps2`; `demo_hill.mp4` 13.9 MB, `demo_hill_discord.mp4`
9.28 MB with `--target-mb 10`; 42.2 s each). That run found the overlay
finding above; the numbers below are this run's, then the second re-run
after the fix follows.

- **Playback.** All 4,734 ticks equal the recording in every game column
  (counter, movie flag, phase, pads, area, loader byte, selector, task
  bytes, position, heading), and the earlier playback in segment, offset
  and capture; only the uncapped movie-step count (`step`) and the
  offline audio position (`af`) differ, as they may. Drift unchanged:
  4,345 of 4,346 play/cutscene ticks bit-exact, no drift beyond the
  tolerance.
- **Fields.** 1,048 of 1,171 captures carry the field (693 drawn on whole
  lines, 355 with the half-line offset), all at the default position. The
  overlay pass drew over 597: share 28.6 % (450 frames, line 0), 29.0 %
  (110, the half-line frames, see the port finding above), 100 % (37
  fades); 451 frames none.
- **Identical neighbours** (vertical / horizontal, 1,171 paired frames):
  0.359 / 0.407 (0fba64a: 0.361 / 0.409; PS2 0.366 / 0.411). **Mean
  absolute difference per channel** to the PS2 frame, all frames 3.75
  (0fba64a 3.70); per segment (0fba64a -> now): opening cutscene (4) 5.70
  -> 5.68, first play (5) 3.92 -> 3.89, status pages (8) 3.77 -> 3.75,
  cutscene 10 2.27 -> 2.70, cutscene 12 4.30 -> 5.11, play 15 2.95 ->
  3.04, the hill (17) 3.26 -> 3.28, others unchanged (6 8.59, 9 2.96, 11
  2.23, 14 2.68 -> 2.70, 16 2.72, loads 0.00). The rises are all on
  half-line fields under the letterbox (the overlay half a field row off,
  above): leaving out rows 31 and 191 alone takes segment 15 back to its
  old value and segment 10 from 2.69 to 2.42.
- Check images (ignored): `check_text.png` (the BATTERY page text and
  three subtitles, one on a half-line field: PS2, port at f1e589c, port at
  0fba64a, 3x; not redrawn after the fix).

### Re-run 2026-10-09 after port 0eea8b5

The native stage was run a third time on port main with the overlay
fix (uncommitted at the time of the run; committed as port 0eea8b5), same command (`native ... --stride 4 --audio`, drive timing as
recorded, 111.5 s), and both videos recomposed from it and the same
`ps2/` (`all --skip-native --skip-ps2`, the title, subtitle, `--speed 2
--fps 30 --height 480 --audio native --summary-seconds 3`, plus
`--target-mb 10` for the Discord copy): `demo_hill.mp4` 13.86 MB,
`demo_hill_discord.mp4` 9.28 MB, 1284x582, 42.175 s each.

- **Frame count.** Each video decodes to 1,263 frames
  (`ffprobe -count_frames`). The summary's `frames` (1,264) counts the
  frames compose writes to its lossless intermediate: the 1,174 aligned
  frames plus the 3-second end card (90 frames at 29.97 fps); the
  intermediate has all 1,264. The final encode muxes with `-shortest`, and
  the port audio, sped up by atempo, ends a fraction of a frame before the
  video, so the last end-card frame is cut (without `-shortest` the same
  encode keeps 1,264). The earlier runs' videos were the same (their
  ffprobe `nb_frames` was 1,263 too).
- **Playback.** All 4,734 ticks equal the recording in every game column
  (counter, movie flag, phase, pads, area, loader byte, selector, task
  bytes, position, heading, segment, offset); only the uncapped
  movie-step count (`step`), the offline audio position (`af`) and the
  capture index (`cap`, -1 in the recording; 0..1170 in order here)
  differ, as they may. Drift unchanged: 4,345 of 4,346 play/cutscene
  ticks bit-exact, no drift beyond the tolerance.
- **Fields.** 1,048 of 1,171 captures carry the field (693 on whole
  lines, 355 with the half-line offset), all at the default position. The
  overlay pass drew over 597: share 28.6 % on both lines now (450 frames
  line 0, 110 frames line 1: 64 of 224 field rows, the bands), 100 % (37
  fades); 451 frames none.
- **Identical neighbours** (vertical / horizontal, 1,171 paired frames):
  0.360 / 0.407 (0fba64a 0.361 / 0.409; PS2 0.366 / 0.411). **Mean
  absolute difference per channel** to the PS2 frame, all frames 3.69
  (f1e589c 3.75, 0fba64a 3.70); per segment (0fba64a / f1e589c -> now):
  cutscene 10 2.27 / 2.70 -> 2.26, cutscene 12 4.30 / 5.11 -> 4.24, play
  15 2.95 / 3.04 -> 2.95, the hill (17) 3.26 / 3.28 -> 3.26, 14 2.68 /
  2.70 -> 2.70, others as at f1e589c (4 5.68, 5 3.89, 6 8.59, 8 3.75, 9
  2.96, 11 2.23, 16 2.72, loads 0.00). Segments 10, 12 and 15 are back at
  (or just under) their 0fba64a values.
- Port captures of the fix (ignored, port `build/overlay_line/`): a
  letterbox frame and a subtitle frame of the opening on each field line
  (level smoke ticks 406 / 429, 544 / 567); `check_present_capture.py
  --overlay-ok --bands=32` passes on all four (overlay viewport with the
  field's line, 32 + 32 band rows, no split row); the subtitle text sits on
  field rows 194..216 on both lines.
- Check images (ignored): `demo_hill_contact.png` (16 frames of the
  video), `contact/` (segment 17 pairs, the Discord copy at 34 s, the end
  card), `sharpness_check.json` (the measures above per segment; `new` =
  this run, `old` = the 0fba64a frames, `mad_f1e589c` = the run before
  the fix).


## Demo run 2026-10-09 (demo_level: the whole first level)

The user asked for the demo over the entire first level. Same method as
demo_hill: the port level smoke's main line recorded with the recorder
(`EM_UNCAPPED=1 EM_STARTUP_TEST=newgame-level EM_NEW_GAME=1
EM_PS2_DISC_DRIVE_TIMING=1 EM_LEVEL_SMOKE_UNTIL=a01_arrival`), START added on
the New Game movie's steps; the level-exit movie (E001, 76.9 s) was watched in
the checked smoke run, so it is left unskipped and plays in real time on both
sides (movies are cut from the video). Receipts: `build/video_compare/demo_level/`
(logs/recording_notes.txt, the stage logs, the drift report).

- Recording: 14,102 ticks in 32 segments, New Game to the AREA01 arrival's 60
  neutral ticks; the smoke and its checker passed all 20 main-line phases with
  the recorder on (266 s and 348 s).
- Port side: 750 s wall (incl. the exit movie in real time); 3,521 captures,
  3,394 with the exact GS field; all 14,102 ticks equal the recording.
- PCSX2 2.6.3 side: one session, 44.7 min, no desync; 14,244 ticks, 3,574 GS
  fields; position and heading bit-exact on 13,619 of 13,620 paired play and
  cutscene ticks, no drift past the tolerance.
- Video: `demo_level.mp4`, 2:00.6 (117.6 s at 2x plus the 3 s end card),
  2568x1164 (each side 1280x960, nearest), H.264 `--crf` high quality, 339 MB,
  sound the port's only. Nearest scaling to 1280x960 is not an integer factor
  (2.5 across, 4.29 rows per field line); 1024x896 per side would be even.
- Seen in the review: the opening cutscene runs about 0.3 s ahead on the port
  (the known opening lead); in the fan room the blades' angle differs between
  the sides on some frames (not traced); the AREA01 load takes 294 ticks on the
  PS2 against the port's 94 (six of the module loader's 19 reads have no
  recorded drive time and answer at host speed).
- `native/audio.wav` is 2.38 GB (the offline mixer also writes the uncapped
  exit-movie steps); compose cuts audio by tick, so only the file is large.
  The original's sound came from the fork pass (next section).

### The original's sound: the fork pass (2026-10-09)

The PCSX2 pass was re-run on the agent-debug fork (v2.9.114-11-gc105df140,
x86_64, Metal build, software renderer, MTVU on as in the capture ini) into
`demo_level/ps2_fork/`:
`video_compare.py ps2 build/video_compare/demo_level.rec --out
build/video_compare/demo_level/ps2_fork --stride 4 --audio --emulator fork`.

- **Session.** 349 s under the lock (78 s boot to the title, 147 title ticks
  with one Cross, 237 s for the 14,258 played ticks including the exit movie)
  plus 634 s waiting for the lock. 3,578 fields, all read from the buffer
  (no fallback). Three movies were caught at the driver probe: the intro
  (played), New Game (skipped, at vsync 11,427) and E001 (played).
- **Audio.** `ps2/audio.wav` holds 15,223,222 samples (317.2 s) over 19,010
  vsyncs (14 samples more than 800.8 per vsync; the tap follows the SPU2
  clock, not the vsync).
- **Drift against the port (compose).** Position and heading are bit-exact on
  13,619 of 13,620 paired play and cutscene ticks. The one difference is
  segment 4 offset 1,309, the same tick as against v2.6.3. No drift passes
  the tolerance.
- **Against the v2.6.3 run (`ps2/`).** Position and heading are bit-exact on
  all 13,632 paired ticks, and every other column is equal except 14 rows of
  the loader byte in the first load. Segment lengths, fork / v2.6.3 / port:
  - first load: 274 / 268 / 266. That is 6 ticks longer than v2.6.3,
    against 5 in the compat stage, which booted with `-elf` and skipped
    New Game by host polling.
  - AREA01 load: 302 / 294 / 94.
  - segment 24: 219 / 219 / 218.
  - opening cutscene (segment 4): 1,321 / 1,321 / 1,310: the port's known
    opening lead.
  - title (segment 0): different by construction. The port logs the
    recording's 74 title ticks; both PS2 passes drive the title with
    ps2.py's title driver and log no rows there (the fork pass took 147
    title ticks with one Cross after its cold boot).
  - Every other segment is equal on all three sides.
- **Pictures** (mean absolute difference per channel, 3,521 paired frames):
  - fork vs port: 2.08 overall. That is closer than v2.6.3 vs port, 3.53.
  - In play segments the fork is within 0.16 to 0.6 of the port, against
    2.2 to 6.6 for v2.6.3.
  - fork vs v2.6.3: 2.78. Only loads, black frames and a few cutscene
    frames are identical (78).
  - The v2.6.3 frames came from a run whose New Game skip landed on a
    host-timed vsync. Two fork runs that differ only in that vsync (3
    vsyncs apart) differ in every field while their game state is
    identical. Exact pixel comparisons therefore need the same emulated
    timeline. The vsync-dependent state is the game's frame and field
    phase: the frame index D_00810E80 (it picks the drawing buffer and
    flips every main-loop iteration) and the field D_00810E88 (sampled at
    every vsync; it picks the half-line draw offset). See
    [PCSX2_FORK_GS_DIFF.md](PCSX2_FORK_GS_DIFF.md) sections 3 to 5: the
    renderers are equal, the capture moment is the same, and with the phase
    matched a fork run reproduces 1,152 of 1,173 v2.6.3 captures bit for
    bit.
- **Sound against the picture.** These checks were made on the per-tick
  audio of both sides (`audio_rms_by_tick.json`) and again in the final
  video's left and right channels (`sound_check/`):
  - *Voice line* ("Dennis here.", opening). The voice starts 2 to 5 ticks
    after its subtitle appears, on both sides. The subtitle appears at
    offset 53..56 and the voice at 58 on the original; on the port they are
    45..48 and 48. The port's whole opening, picture and sound, runs 10
    ticks ahead (the known opening lead).
  - *Footsteps* (segment 9, the first walk). The onsets are at offsets 52,
    74, 95 and 112 on both sides (the port's first one tick earlier), while
    the player moves, bit-exact on both sides.
  - *Elevator* (segment 16; the player's Y falls from 230 to 190 between
    offsets 211 and 360). The ride's sound rises at 211, swells at 227, 237
    and 247 and falls off at 360, on the same ticks on both sides.
  - The two loudest effects (RMS about 13,000, segments 5 and 6 at offset 67)
    start on the same tick on both sides.
- **Port finding (explained by port `a345d9a`, below): a recurring loud
  sound starts 14 ticks early.** In the play segments a loud sound returns every 130 to 170 ticks
  (RMS about 8,000; e.g. segment 17 offsets 51, 350, 519, 650, 818, 949).
  It starts 14 ticks (0.23 s) later on the original than on the port, at
  about 30 places, while every effect above is on the same tick. At segment
  17 the original's sound starts with the camera cut at offset 64; the
  port's starts at 51. It looks like a stream lane starting early. That is
  a lead for a port-side agent, not a diagnosis.
  **Explained (port `a345d9a`, 2026-10-09):** the sound is the area music's
  own percussive hit (lane 0, cue 25, every 131 / 169 ticks). After every
  status page the original's lane-0 read seeks for about 17 fields after
  the page's module load; the port answers at host speed and resumes the
  music 17 fields early (15 with the disc-drive timing switch). That is
  disc timing, which the port does not reproduce by policy (an open launcher
  decision, port `LAUNCHER_OPTIONS.md`). The same commit fixed a port bug:
  the replay's offline audio pull left music and voices one field late in
  the comparison WAVs.
- **Videos** (ignored, `demo_level/`; `--speed 2 --fps 30 --height 960 --crf
  12 --summary-seconds 3`, the same title, and the subtitle ending in the
  sound note): `demo_level_sound.mp4` (`--audio both`: original LEFT, port
  RIGHT; 336 MB) and `demo_level_original_sound.mp4` (`--audio original`;
  335 MB). Each is 2568x1164, 2:00.6, H.264 + AAC, 3,613 decoded frames.

## Test 2026-10-16 (the field phase)

A fresh recording on port main 92786d2 (a private build of HEAD): the level
smoke's main line to first control (`EM_UNCAPPED=1
EM_STARTUP_TEST=newgame-level EM_NEW_GAME=1 EM_PS2_DISC_DRIVE_TIMING=1
EM_LEVEL_SMOKE_UNTIL=first_control EM_INPUT_RECORD=...`), with START added on
the New Game movie's steps as for demo_level: 1,732 ticks in 6 segments (the
title 74, P 1, the load 267, P 4, the opening 1,321, P 65). Native pass
`--stride 4 --audio`, fork pass `--stride 4 --audio --fork-title-delay auto`,
`drift`, and a 13.9 s clip (`compose --speed 2 --audio both --info`). Kept in
`build/video_compare/phase_test/` (ignored): the recording, the drift reports,
the stage logs, `title_delay_scan.json` and the clip; the frames were deleted.

- **Port mapping.** 413 of 417 captures carry the GS field; all 413 sit on
  line 1 - (counter & 1).
- **Auto delay.** The port's New Game movie iteration is counter 71 (frame
  index 1). Delay 0 entered it on frame index 0 (vsync 11,427) and ended at
  the probe (101.5 s, boot included); delay 1 entered it on frame index 1
  (vsync 11,428) and was used. Every captured PS2 field's draw environment
  was found by its buffer (419 of 419, all on the slot other than +0x9C, both
  contexts equal, no fallback).
- **Segments, port / PS2:** P 1 / 1, load 267 / 274, P 4 / 4, **opening
  1,321 / 1,321**, P 65 / 65. Position and heading bit-exact on all 1,391
  paired play and cutscene ticks.
- **Field phase with the chosen delay 1:** the load 65 of 65 pairs on the
  same half line; the opening 0 of 331 and the first play 0 of 17 (all on the
  other half line). Mean absolute difference per channel to the port: load
  0.01, opening 1.81 (49.9 % of the pixels differ), first play 0.79 (26.0 %).
- **Why.** The PS2's load is 7 iterations longer (274 against 267). Six of
  them are the fork against v2.6.3 (268 in demo_level; the fork's late IOP
  acknowledgement D_00275B18, port `docs/IOP_STREAM.md`), one is the port
  against v2.6.3, whose drive timing the port's model follows (not traced
  here). An odd difference flips the phase relative to the
  port for everything after the load. Equal frame index at the New Game
  movie is therefore the wrong criterion for this recording's opening.
- **Control run, delay 0** (`phase_test_delay0_drift.txt`): the load 0 of
  65 on the same half line, the opening **331 of 331**, the first play
  **17 of 17**; mean absolute difference: opening 0.25 (0.8 % of the pixels),
  first play 0.00 (bit-exact frames), load 0.14. The buffer index differs
  there (the other frame index at the movie, flipped again by the load);
  the buffer alone changes no pixel (docs/PCSX2_FORK_GS_DIFF.md 5.3a).
- **Open (a lead decision).** Which criterion the auto delay should use for
  the video: the frame index at the New Game movie (as built, the step's
  contract) or the drawn half line after the first load (what the pixels
  follow; it would need each try to run past the load, about 350 more
  ticks). Until then `--fork-title-delay 0` gives the matching half line
  for recordings with this load.
