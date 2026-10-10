# PCSX2 agent-debug fork: where it is and how to start

The project's advanced PCSX2 for AI agents is a **separate local repository**:

`/Users/abe/Documents/Extermination.nosync/pcsx2-fork` (branch `extermination-mcp`)

It is upstream PCSX2 `v2.9.114` (commit `aa7ab4306e26`, 2026-10-08) plus our
DebugServer and AgentDebug engine. It can:

- run the original game hidden to a condition in one request (N vsyncs, N
  main-loop ticks, the Nth hit of a PC);
- record every frame compactly and rewind to any frame, verified;
- diff runs and find the first divergent frame;
- log who writes an address;
- count function coverage with no observer effect;
- record the SPU2 output in emulated time;
- read the displayed GS field.

## Read first

1. `pcsx2-fork/EXTERMINATION.md`: the entry point. It covers the build,
   launch options, every MCP tool with an example, the file formats, the
   performance numbers, the tests, the known limits and recipes. MCP clients
   can read it as the resource `pcsx2://docs/overview`.
2. `pcsx2-fork/extermination/docs/AGENT_GUIDE.md`: the agent's quick guide
   (rules, first calls, pitfalls).

Both pages, and everything in `pcsx2-fork/extermination/docs/`, describe
behaviour only, so port-side agents may read them.

## Rules that come with it

- **GPL boundary and clean room.** The fork is GPL-3.0 and is only a debugging
  tool. Nothing from it is copied into this repository or the port.
- **Readers of emulator source stay out of our code.** Agents that read PCSX2
  source never write in `../extermination-port` or in this repository's
  `src/`.
- **Port-side agents use behaviour only.** They use the fork through its tools
  and its behaviour docs, never its source or `docs/agent-debug/IMPLEMENTATION.md`.
- **Running it.** The run lock `build/.pcsx2.lock` (mkdir, always rmdir),
  hidden runs, scratch data outside `~/Documents` and never
  `build/startup-reference/portable-data`. Use the software renderer for
  captures, and leave no emulator running. The fork's launcher and MCP bridge
  do all of this for you.
- **Output.** Everything it writes from the disc (captures, states, fields,
  audio) stays in scratch or ignored `build/` folders. No disassembly from
  `pcsx2_disassemble` goes into docs, comments or commits.
- **Pushing.** Never push the fork. Its push URL is disabled.

## Status (2026-10-09)

| Item | State |
|---|---|
| Fork build | `pcsx2-fork/build-x64/pcsx2-qt/PCSX2.app`: x86_64, runs under Rosetta 2, not installed. Acceptance results: 70/70 v1 smoke, 78/78 BIOS, 21/21 MCP, 37/37 game. |
| MCP bridge | `pcsx2-fork/extermination/mcp-server/pcsx2_agent_mcp.py` (55 tools), registered in Claude Code as `pcsx2-agent` (local scope, the user's decision of 2026-10-09). The legacy Node bridge in `../PCSX2-MCP` is no longer the registered server. |
| Legacy emulator | `build/startup-reference/PCSX2.app`: v2.6.3, x86_64, with the old DebugServer (TCP 21512) and PINE. It stays the default for every decomp tool until the user retires it (checklist in "Retiring the 2.6.3 app" below). The duplicate `/Applications/PCSX2.app` went to the Trash on 2026-10-09. |
| Save states | The fork writes version `0x9A59` and refuses v2.6.x states (`0x9A55`) with an explicit error. The user's slots 01 to 15 and the `build/s87/...` snapshots load only in the legacy app. The fork-saved replacements the tools need are in `build/startup-reference/fork-states/` (next section). |
| Compat with the game | Receipts are in the ignored `build/pcsx2-fork/acceptance/reports/`. The fork cold-boots the game, passes the intro and title, and replays the demo_hill route (4,678 ticks). Against the legacy run, per-tick game state matched once one load segment was aligned (273 against 268 ticks), except 5 rows of the loader busy byte. |
| GS fields against v2.6.3 | **Explained** in [PCSX2_FORK_GS_DIFF.md](PCSX2_FORK_GS_DIFF.md). Most compat-stage frames differed because the game's frame index and field phase differed after the longer load, not because of the renderer. With the phase matched, a fork run reproduces 1,152 of 1,173 v2.6.3 reference captures bit for bit. The fork's software renderer equals v2.6.3's on 906 conformance tests and 112 dumped game fields. The one exception is the half-pixel culling change of upstream `4fa2b8e45`: lines or points starting within half a pixel outside the scissor, 8 captures here. Pair captures by tick and field, and decode each run's own drawing buffer (section 8 there). |
| arm64 | No native build. Upstream has no arm64 recompilers, and the fork's probes and memwatches live in the x86 recompilers. Rosetta 2 is available through macOS 27. See `EXTERMINATION.md` section 14. |

## Using it from this repository's tools

The DebugServer's v1 protocol is byte-compatible with the legacy build, so
`tools/pcsx2_session.py` can drive the fork. This is opt-in; the default stays
the legacy `-portable` launch:

```sh
# macOS (arm64 host; the emulator runs under Rosetta), decomp .venv
cd /Users/abe/Documents/Extermination.nosync/Extermination
FORK_BIN=/Users/abe/Documents/Extermination.nosync/pcsx2-fork/build-x64/pcsx2-qt/PCSX2.app/Contents/MacOS/PCSX2
mkdir build/.pcsx2.lock && {
  .venv/bin/python tools/pcsx2_session.py "<path>/SCUS-97112 (0AE679AF).fork.p2s" \
      --emulator "$FORK_BIN" --data-dir "$TMPDIR/pcsx2-session-fork" --frames 30
  rmdir build/.pcsx2.lock; }
rm -rf "$TMPDIR/pcsx2-session-fork"      # BIOS copies; absolute path
```

From Python: `OriginalSession(state, emulator=FORK_BIN, data_dir=...)`. The
command line does not take the run lock itself.

- `--data-dir` must lie outside `~/Documents`. The session fills it with
  read-only copies: the BIOS, `build/startup-reference/inis/PCSX2.ini` with
  every folder pointed inside it, PINE on slot 28011 and the software
  renderer. It also copies the state and the ELF and APFS-clones the ISO,
  which is removed when the session closes.
- Delete the folder afterwards: it holds BIOS copies.
- For `snapshot`, the state's file name must start with the serial
  (`SCUS-97112 (...).*.p2s`): the snapshot's slot file is named after it.
- Checked on 2026-10-09 from the fork's demo_hill tick-1600 state: ready in
  2.6 s, 30 consecutive main-loop steps in 2.4 s, a snapshot with
  `eeMemory.bin`, `gs.bin`, `scratchpad.bin` and the screenshot, the ISO clone
  removed, the source state unchanged and no emulator left.

## Fork states: the tools' save states regenerated in the fork

`tools/fork_states.py` rebuilt, from a **cold boot in the fork**, the v2.6.3
states the tools use (2026-10-09, fork build `v2.9.114-11-gc105df140`). Each
state sits at the same **game point** as the old one. Points are matched by
game state, never by the main-loop counter, which differs between the builds:
the task slots, fade and letterbox blocks, scratchpad selectors, the player
actor, camera, UI, area/sub/entry, story, power, inventory and charge bytes,
and the AREA11 owner nodes (battery, panel, elevator, Roger). That is about
1,750 bytes, with the player's clock left out.

Everything is in the git-ignored `build/startup-reference/fork-states/`:

| Path | What |
|---|---|
| `manifest.json` | old name -> new file; how it was reached; the fork rev, hash and repo HEAD; counter; SHA-256; the game-state fingerprint (new, old, differences); the byte score against the old state |
| `<key>/` | `state.p2s` (0x9A59, saved with the fork's exact `state_save`), `eeMemory.bin`, `scratchpad.bin`, `gs.bin`, `original.png` (the displayed field, software renderer), `snapshot.json`, `fork_state.json` |
| `beats/s87/route/<beat>/` | route_capture beats 00..15 run in fork mode, laid out like `build/s87/route/` (`trace.json` and the snapshot files) |
| `compare.json`, `verify.json` | each fork route trace compared row by row with its v2.6.3 trace; the load check of every state |
| `logs/` | per-session logs |

| Key (aliases) | Old state | How it was reached | Match |
|---|---|---|---|
| `slot01_title` (`01`) | user slot 01 (counter 1306) | cold boot, fixed RTC, no input, to the first loop top equal to slot 01 (the title fading in) | exact, counter 1313 |
| `slot02_opening` (`02`) | slot 02 (2939) | from `slot01_title`: route_census's title inputs, NEW GAME, the movie, the opening without input; the best tick | exact, tick 843 after the title, counter 2156 |
| `slot03_fade_in` (`03`) | slot 03 (3963) | same run; the fade-in after the opening | exact, counter 3180 |
| `slot04_first_control` (`04`) | slot 04 (4083) | same run; first control, 120 ticks after slot 03 as in the old pair | exact, counter 3300 |
| `route/00_panel_no_battery` .. `route/15_level_exit` (`00_panel_no_battery`, ...) | `build/s87/route/<beat>/state.p2s` | `route_capture.py --emulator fork`, closed loop as the legacy chain, from `04` and then each previous fork snapshot | see below |
| `slot08_battery_prompt` (`08`) | slot 08 (8003) | from `route/02_elevator_refusal`: route_capture's `use_panel` with the battery; the BATTERY use prompt once its UI bytes equal slot 08's, +10 ticks | UI equal; 49 bytes differ (below) |
| `slot12_status_root` (`12`) | slot 12 (8043) | Circle, Circle, as `panel_root_probe.py` did | UI equal; same 49 bytes |
| `slot14_status_hub` (`14`) | slot 14 (8059) | Circle, as `status_hub_probe.py` did | UI equal; same 49 bytes |
| `slot15_roger_encounter` (`15`) | slot 15 (4029) | from `slot03_fade_in`: `roger_encounter_probe.py`'s teleport to (340, 290, 190), 50 ticks after the bank-96 clip starts | exact, counter 3246 |

"Exact" means every compared byte is equal. Outside the compared spans, slots
02 to 04 still differ in about 460,000 bytes of EE RAM. Those bytes are
counters, render and DMA packet buffers (`0x28F708`, `0x2A3802`), scratchpad
work areas, stacks near `0x241000`, and about 2,900 bytes at
`0x7A1D10..0x7A2600` at the start of the actor arena. Those last bytes are not
identified. Slot 01 differs in 1,021 bytes.

The slot 08/12/14 chain differs from the old one in the same 49 bytes:

- the battery pickup's owner node: the route picks the battery up in beat 01,
  while the old slots come from a hand-played run;
- fade byte +6 (`0x20` against `0x04`);
- the low bits of the player's height and of the camera;
- the player actor's blend area (`+0x244..+0x26B`).

The status UI bytes, area, position, power, charge and item counts are equal.

### Route beats in the fork against v2.6.3

`tools/fork_states.py compare` checks traced fields row by row: position,
yaw, action, clip, selectors, UI, request, power, charge, battery item, area,
fade and story.

| Beats | Result |
|---|---|
| 00, 01, 02, 03, 04, 08, 09, 11, 12 | identical in every traced field on every frame, and the same frame count |
| 05 | same 676 frames. Yaw first differs at frame 7, by 2e-5. The stick inputs then differ slightly (closed loop), and the end position differs in the last bits |
| 06 | 182 frames against 205. The legacy capture needed a retry with 23 extra idle frames before its snapshot resumed, and the fork's first capture resumed. Row 0 differs by 5e-4 in x (05's end) |
| 07 | same 557 frames; positions differ in the 4th decimal (06's end) |
| 10 | same 3,568 frames; one clip number differs from frame 3,381 (357 against 358) |
| 13 | 802 frames against 809. Position first differs at frame 133, by 0.1; the closed loop then takes a slightly different path |
| 14 | same 1,818 frames; positions differ by 2e-5 |
| 15 | 884 frames against 801. The beat waits for the fan's slow window, and the fork's run reached it one fan cycle later: the departure triggers at frame 420 against 345 |

**Why the starts differ.** A legacy session loads its state, runs free until
PINE answers, then pauses. Each v2.6.3 beat therefore started 1 to 12 frames
after its source snapshot: for example, counter 11,524 to 11,525, but 7,944 to
7,956 for beat 10. `ForkSession` starts paused on the saved loop top, so every
fork beat starts exactly at its source (0 frames lost). Beats 05 and 15 begin
on a different phase of the game's clocks, and that is consistent with the
small divergences above.

The receipts do not show whether the 2e-5 yaw step in beat 05 comes from that
offset alone or also from emulation differences between 2.6.3 and 2.9.114.
Re-running beat 05 from the legacy snapshot with a frame-exact legacy start
would settle it.

Re-running a beat from a fork snapshot (`fork_states.py rerun <beat>`) gives
the same trace as the fork chain: see "Verification" below.

### Verification (2026-10-09)

- **`fork_states.py verify`, 24 of 24.** Every manifest state loads in the
  fork, and the live machine at the load equals the file (the compared spans
  and the counter). Two steps then advance the counter by one each. The
  fingerprint matches the old capture's row for every state except 08, 12 and
  14, which differ in fade byte +6 only (`0x20` against `0x04`; see above).
  The receipt is `fork-states/verify.json`.
- **`fork_states.py rerun 05_boxes`.** Beat 05 was re-run from the regenerated
  `route/04_elevator_ride` snapshot, written to the ignored
  `build/pcsx2-fork/rerun/05_boxes/`. Its 677 rows are identical to the fork
  chain's trace, so fork replays are exact. Against the v2.6.3 capture, yaw
  first differs at frame 7 (0.11527 against 0.11529). The yaw differs on 99
  rows, and the position from frame 28 on 432 rows. The end state is equal to
  5e-5.

### Using the fork states (opt-in; the defaults are unchanged)

```sh
# macOS (arm64 host; the fork runs x86_64 under Rosetta), decomp .venv
cd /Users/abe/Documents/Extermination.nosync/Extermination
# step the original from fork slot 04 (first control) and snapshot it
.venv/bin/python tools/pcsx2_session.py 04 --emulator fork --frames 30 --snapshot build/<task>/walk
# run route beats in the fork (sources resolve through the manifest;
# output goes to fork-states/beats/..., never to build/s87/...)
.venv/bin/python tools/route_capture.py run --beats 07 --emulator fork
```

From Python: `ForkSession(fork_state("04"))` (or `fork_state("14_roger_encounter")`,
or the old path) with the `OriginalSession` API (`step`, `pad`, `read`,
`write`, `snapshot`). Notes:

- It launches through the fork's `pcsx2dbg` launcher: hidden, on a fresh
  scratch folder under `$TMPDIR/pcsx2-fork-session`, software renderer, no
  frame limiter.
- It takes the run lock itself, waiting for other runs, and never starts while
  another PCSX2 process exists.
- A state load starts paused (`[UI] StartPaused`), and one frame step is one
  `run {until: {ticks: 1}}` at the loop top. That is about 80 frames a second
  with route_capture's per-frame sample, against about 8 on the legacy app.
- `snapshot()` saves with `state_save` (exact) and takes `original.png` from
  `gs_field`.
- `ForkSession(None)` cold-boots the disc with a fixed RTC. It passes no
  `-elf`: an `-elf` override on a cold boot leaves the EE in the kernel.
  The cause is the scratch path. `$TMPDIR` lies under the symlink `/var`, and
  the emulator accepts the override only when `-elf` names the real path. The
  fork's launcher passes real paths since fork commit `1e22fdc0c`, so `-elf`
  now boots. See [PCSX2_FORK_GS_DIFF.md](PCSX2_FORK_GS_DIFF.md) section 9.
- The scratch folder, ISO clone and lock are removed on close.

### Still 2.6.3-only

- **Tools with no fork option yet:**
  - `tools/route_census.py`: its census sessions are `OriginalSession`
    subclasses with legacy breakpoints;
  - `tools/video_compare/ps2.py`: title slot 01, Media Capture;
  - `tools/gs_conformance.py`, `tools/ee_float/harness.py`,
    `tools/c7cap_capture.py`, `tools/c7cap_partb.py`, `tools/load_wait_probe.py`;
  - the disposable probes in `build/startup-reference/*.py`.
- **The other route groups:** `route_a01` .. `route_a03`, `c7cap`, `aimfire`,
  and `c10/exit|damage|branch|options`. They chain from the route snapshots
  above. `route_capture.py --emulator fork --beats <group>` regenerates them
  into `fork-states/beats/...`, and none has been run yet.
- **Port tests that read v2.6.3 state files offline** (no emulator), through
  `parse_pcsx2_state`:
  - slot 01: `test_sound_bank_reference.py`, which also reads route 00;
  - slot 02: `test_area11_sfx_reference.py`, which reads `SPU2.bin`;
  - slot 04: `test_truck_original_reference.py` and
    `test_player_slide_reference.py`;
  - slot 14: `test_vu1_object_kernel_reference.py`.

  These tests keep working while the files exist; retiring the app does not
  delete them.
- **User slots no tool uses**, which `fork_states.py` does not regenerate:
  - 06, panel powered (counter 8268): hand-played, the source of
    `elevator_probe.py` and `elevator_refusal_probe.py`;
  - 07, panel clip 0x15C (8118): written by `panel_probe.py`;
  - 11 and 13, elevator terminal (8317, 8280): written by the two elevator
    probes from 06.

  Slot 08 (hand-played) and slot 12 (written by `panel_root_probe.py`) are
  needed only as sources of 12 and 14, and have fork equivalents above.

### Retiring the 2.6.3 app: what is still needed

1. Port the remaining legacy-only tools above to `ForkSession`, or decide to
   drop them. route_census is the important one: the census measure in
   `FIRST_LEVEL_CENSUS.md` depends on it.
2. Regenerate the other route groups in fork mode where they are still used,
   and point their readers (c7cap_partb, load_wait_probe, the port's capture
   tests) at `fork-states/beats/...`.
3. Move the port tests that read slot files offline to the fork files
   (`fork_state()` or the manifest). That is a port-side change. If the old
   slot files go to the Trash before then, those tests lose their inputs.
4. Decide, per v2.6.3 field or video capture used as a frame-exact reference,
   whether to re-record it in the fork. The fork reproduces such captures
   bit for bit when the tick, the field phase and the drawing buffer are
   matched. Two exceptions remain: frames where the AREA11 load length differs
   (4 or 5 more loader iterations in the fork), and lines or points starting
   within half a pixel outside the scissor. See
   [PCSX2_FORK_GS_DIFF.md](PCSX2_FORK_GS_DIFF.md) sections 5 to 8.
5. Then the user can trash `build/startup-reference/PCSX2.app` and the slots,
   with their confirmation (`CLAUDE.md`).
