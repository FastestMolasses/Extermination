# AUDIO captures of the original (chain step AUDIO)

This capture was requested by the port's chain step AUDIO (port
`docs/FIRST_LEVEL_AUDIT.md` section 1b item 1; `docs/IOP_STREAM.md`;
STARTUP.md's audio notes): record the original's sound output for first-level
route beats, so the port's sound output can be compared with it.

This file cites addresses, frame numbers and settings. It holds no original
code, no disassembly and no disc bytes. All outputs are local and ignored, in
`build/s87/audio/`, whose README.md is the full description. The tools are
there too (`build/s87/audio/tools/`); no tracked tool was changed.

## Result

**The original's audio output was not recorded. No WAV exists.**

The user allowed PCSX2's audio recording during this capture job only
(2026-10-01). PCSX2 v2.6.3, the MCP-enabled build the project uses, offers no
recording that a hidden, automated session can start from its settings. The
evidence was measured and is kept under `build/s87/audio/probe/`:

- **SPU2 debug WAV log.** Its ini keys are `[SPU2/Debug] Global_Enable` and
  `Log_WAVE_Output`, documented for the old SPU2-X plugin.
  - They were switched on for one probe session: 40 stepped frames and 2 s
    free-running.
  - PCSX2 kept the values but wrote no file anywhere.
  - The live ini was then restored byte for byte.
- **Media Capture.** This is the UI's Graphics > "Media Capture" tab, with
  audio-only capture to WAV.
  - It is started only from the Tools > Video Capture menu or a hotkey, and it
    asks for a file location.
  - A hidden emulator cannot receive either. Pressing the menu through System
    Events is refused, because the agent has no Accessibility permission, which
    is a macOS security setting.
  - PCSX2 loads FFmpeg at run time, and its compatibility with the installed
    Homebrew FFmpeg 8 was not tested.
- **No other route.**
  - The command line (`-help`), the DebugServer command list and Pine have no
    capture command.
  - The DebugServer's IOP read of the SPU2 register file (0x1F900000..0x1F9007FF)
    returns only zeros.

**What would make a recording possible.** Each option is the user's decision:
1. A visible PCSX2 window for one capture job, driven by computer-use. This
   conflicts with the "PCSX2 runs hidden" rule.
   - The user approved this in chat on 2026-10-01. The follow-up job stopped
     at its first step: the computer-use grant for PCSX2 (net.pcsx2.pcsx2)
     was declined in the approval dialog. Nothing was run: no PCSX2 session,
     no lock, no setting changed. The FFmpeg 8 question, the sync
     measurement and the WAVs are still open.
2. The user grants Accessibility to the agent's host app.
3. The user starts the capture by hand.

The capture tool is ready to measure sync once a WAV exists: it reads the
file's sample count at every frame boundary, and the same stretch can be run
frame-stepped and free-running and compared.

## What was recorded instead

Ten beats, one hidden session each, driven closed-loop. Route beats re-run
route_capture's own beat functions. All frame boundaries are at the main-loop
top 0x1AAF28. Per frame, each beat records:
- the route row;
- the EE sound state, as fixed-size raw records of 9,412 bytes. These hold the
  tables the port models:
  - D_002817C0..D_002821AF (the IOP reply, handle volumes, D_00281B70 /
    D_00281C30, the voice ring, D_00281F30, the three stream lanes, the cues
    D_00282178);
  - the 48 voice records D_0027CCC0;
  - the bank handles D_0027C6C0;
  - D_0027F740;
  - D_00275B18..54;
  - D_008106C0..FF and D_00810D38;
  - the frame words 0x70003B64 / 0x70003B68;
  - the vsync counter D_00810E90.

Each beat also keeps the pad input script and an end snapshot, whose
`state.p2s` includes PCSX2's SPU2 state at that frame.

| Beat | Source | Frames | Counters | Item-1 coverage |
|---|---|---|---|---|
| opening | slot 01 | 1385 | 1852..3237 | the AREA11 opening from task +0xB == 1 to first control (3177) + 60: stream request at 1855 (cue 0x3F), fade-in 1881, cue 25 back at 3177; before it, the title and the intro movie run as the lead segment (rows kept, marked `lead`). Only D_00282178[0] carries a stream cue in this beat |
| battery_ui | slot 04 | 516 | 4085..4601 | UI cues: pickup, ITEM page, exit |
| panel_power | route 02 | 686 | 4990..5676 | UI cues of the BATTERY page; panel unit |
| elevator_ride | route 03 | 576 | 5676..6252 | elevator unit |
| fence_door | route 08 | 534 | 7945..8479 | door unit, room move |
| walk_outdoor | route 06 | 187 | 7142..7329 | quiet walk on the low ground |
| walk_room | route 09 | 196 | 8478..8674 | quiet walk behind the fence door (entry 2) |
| flame | route 11 | 570 | 12786..13356 | the flame's looped positional 0x413, standing 16.5 units from it (XZ) |
| cage_roof | route 08 | 3568 | 7945..11513 | director beat 0 and Roger's voiced conversation (stream cues 0x8F..0x94) |
| roger_encounter | route 13 | 1818 | 13941..15759 | Roger's encounter (stream cue 0x1D) |

The beats total 10,036 frames, about 167 s of game time.

**Measured properties:**
- **The per-frame reads change nothing.** One stretch was run twice from route
  beat 06's snapshot, with and without the extra IOP read (the runs started at
  counters 7142 and 7144). All 299 common counters have identical route rows
  (the run-relative frame index excluded, the only differing key) and
  identical EE sound records; `readcheck/compare.json` records both counts.
- **Three beats reproduce their route traces row for row.** battery_ui (517 /
  517 rows), elevator_ride (577 / 577) and roger_encounter (1819 / 1819).
  - panel_power and fence_door are closed-loop re-runs that diverge after row
    3, the re-capture shift that FIRST_LEVEL_ROUTE.md section 7 documents.
  - cage_roof is a separate run from the same snapshot, of equal length.
- **One frame is one field.** In every beat, each main-loop frame is one field,
  except fence_door's start-up frame f3, which spans 2 fields.
- **The 48 kHz column is nominal.** frames.csv carries a
  `nominal_sample_48k` column: round((vsync − the first beat vsync) · 800.8),
  an NTSC field clock at 48 kHz. It is **not measured**, because no audio was
  recorded.

**Facts in the recorded state that bear on item 1** (recorded, not
interpreted):
- In the flame beat, D_00281B78 and D_00281C38 hold 0x413 in every frame.
- Over 31 frames between f245 and f515, 34 D_0027CCC0 records hold 0x413 at
  +0x1C. Each such record holds it for exactly one frame. Some frames (f342,
  f381, f400) have two such records, and some consecutive frames have it in
  different records.
- In cage_roof, 0x413 first enters D_00281B78 / D_00281C38 at f885.

These records do not settle whether the hardware lets a sample of the
same-flush key-on through (SFX_SEQUENCER.md); only an output recording can.

## Emulator hygiene

- **The live ini.** It was hashed before every session and restored byte for
  byte after the emulator exited, also on failure. Each report says
  `ini_restored_identical`.
- **Settings changed.** Only the probe session switched settings on: the two
  SPU2 debug keys. The renderer was never changed.
- **Memory cards.** Both cards' hashes are unchanged.
- **Save slots.** No save slot 01..15 was written.
- **Emulator processes.** No emulator was left running.
- **The shared lock.** Every session held `build/.pcsx2.lock`.
- **`playtime.dat`.** PCSX2 itself rewrites it at exit.
