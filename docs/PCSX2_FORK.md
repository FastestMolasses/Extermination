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

## Status (2026-10-10)

| Item | State |
|---|---|
| Fork build | `pcsx2-fork/build-x64/pcsx2-qt/PCSX2.app`: x86_64, runs under Rosetta 2, not installed. DebugServer **0.2.2** (fork commit `5a3048e8e`). Acceptance results on it: 70/70 v1 smoke, 82/82 BIOS, 21/21 MCP, 38/38 game (`--probes 1200`). The app's version string still reads `v2.9.114-11-gc105df140` (it is fixed when the build is configured), so identify a build by `hello.server_version`. |
| Metal | The Xcode Metal Toolchain was installed on 2026-10-09 (the user's decision) and the fork rebuilt with its Metal renderer (fork `36f3e3a50`; `EXTERMINATION_BUILD.md` section 6). Captures still use the software renderer (13): only it keeps rendered pixels in GS memory. |
| Code signing | The build is **unsigned** (ad-hoc signing is optional, `EXTERMINATION_BUILD.md` section 5). A stable local signing identity, which would keep one `~/Documents` grant across rebuilds, is **not set up**: Claude Code's permission system refused the agent's keychain import and signing steps. The user decides (create it from the recipe in `EXTERMINATION_BUILD.md` section 5, or allow an agent to). Until then the launcher keeps scratch outside `~/Documents` and APFS-clones the disc image. |
| MCP bridge | `pcsx2-fork/extermination/mcp-server/pcsx2_agent_mcp.py` (55 tools), registered in Claude Code as `pcsx2-agent` (local scope, the user's decision of 2026-10-09). The legacy Node bridge in `../PCSX2-MCP` is no longer the registered server. |
| Legacy emulator | `build/startup-reference/PCSX2.app`: v2.6.3, x86_64, with the old DebugServer (TCP 21512) and PINE. **No longer the default** (the user's decision of 2026-10-09 to retire it): every decomp tool runs on the fork unless given `--emulator legacy` (or `EXTERMINATION_PCSX2=legacy`). The app stays until the lead retires it (checklist in "Retiring the 2.6.3 app" below). The duplicate `/Applications/PCSX2.app` went to the Trash on 2026-10-09. |
| Save states | The fork writes version `0x9A59` and refuses v2.6.x states (`0x9A55`) with an explicit error. The user's slots 01 to 15 and the `build/s87/...` snapshots load only in the legacy app. The fork-saved replacements are in `build/startup-reference/fork-states/`, in two generations: `base` (the first regeneration) and `phase` (the default since 2026-10-10: the same game points, `rand()` state and player clock included, with the frame index and field of the v2.6.3 session's first loop top after loading the slot; "Correction" below). Every v2.6.3 fact the tools need (slot phases, save PCs, compared spans, beat lead-ins) is cached in `build/fork_refs/legacy_refs.json`, so no tool needs the slot files any more. |
| Compat with the game | Receipts are in the ignored `build/pcsx2-fork/acceptance/reports/`. The fork cold-boots the game, passes the intro and title, and replays the demo_hill route (4,678 ticks). Against the legacy run, per-tick game state matched once one load segment was aligned (273 against 268 ticks), except 5 rows of the loader busy byte. |
| GS fields against v2.6.3 | **Explained** in [PCSX2_FORK_GS_DIFF.md](PCSX2_FORK_GS_DIFF.md). Most compat-stage frames differed because the game's frame index and field phase differed after the longer load, not because of the renderer. With the phase matched, a fork run reproduces 1,152 of 1,173 v2.6.3 reference captures bit for bit. The fork's software renderer equals v2.6.3's on 906 conformance tests and 112 dumped game fields. The one exception is the half-pixel culling change of upstream `4fa2b8e45`: lines or points starting within half a pixel outside the scissor, 8 captures here. Pair captures by tick and field, and decode each run's own drawing buffer (section 8 there). |
| Fixes of 2026-10-09 | **Presentation off** (fork 0.2.2): builds before it never submitted GPU work while presentation was off, leaked about 1 MB per vsync and aborted the GS thread after long free runs or movies; 0.2.2 submits every 4 vsyncs and the footprint stays flat (765 MB over 36,000 vsyncs). **Shutdown** (0.2.2): about 0.5 s, also after thousands of stops and on lease expiry (before: over 20 s after a long closed loop, then killed). **`-elf` boots** (fork `1e22fdc0c`): the launcher passes real paths, so an `-elf` override from the `$TMPDIR` scratch (under the `/var` symlink) loads and runs. **`ForkSession` steps** (decomp `184f6f1`): `step(n)` advances exactly n frames with `align` False or True (below). |
| arm64 | No native build. Upstream has no arm64 recompilers, and the fork's probes and memwatches live in the x86 recompilers. Rosetta 2 is available through macOS 27. See `EXTERMINATION.md` section 14. |

## The tools run on the fork by default (2026-10-09)

The user decided on 2026-10-09 to retire PCSX2 v2.6.3 and migrate everything.
Every decomp tool that drives the original now uses the fork unless told
otherwise. The legacy path stays available, behind an explicit switch, until
the lead trashes the app:

- the command-line option `--emulator legacy` (route_capture, route_census,
  sfx_request_probe, c7cap_capture, c7cap_partb, load_wait_probe,
  gs_conformance, pcsx2_session, video_compare/ps2.py);
- or the environment variable `EXTERMINATION_PCSX2=legacy` (it also covers
  `ee_float/harness.py` and `repack/proof.py`).

```sh
# macOS (arm64 host; the fork runs x86_64 under Rosetta), decomp .venv
cd /Users/abe/Documents/Extermination.nosync/Extermination
.venv/bin/python tools/pcsx2_session.py 04 --frames 30 --snapshot build/<task>/walk   # fork, phase/04
.venv/bin/python tools/route_capture.py run --beats 00                                # fork, phase-locked
.venv/bin/python tools/route_capture.py run --beats 00 --emulator legacy             # v2.6.3 (until retired)
```

How the fork path works:

- **Sessions.** `pcsx2_session.ForkSession` launches through the fork's
  `pcsx2dbg` launcher: hidden, scratch outside `~/Documents`, software
  renderer, no frame limiter. It takes the run lock itself (it waits) and
  leaves nothing running. `pcsx2_session.open_original(state, backend)` makes
  either session.
- **One session class for every tool.** `route_capture.RouteSession` derives
  from `ForkSession` and picks the emulator when an instance is made
  (`route_capture.FORK`). The tools' own subclasses (census, c7cap, load-wait,
  sfx, gs_conformance) therefore run on the fork unchanged.
- **Their v1 stepping loops.** Those tools step with `resume` and then poll
  `status` until the VM pauses at the loop top or at one of their v1
  breakpoints. On the fork, `self.debug` is `pcsx2_session.ForkV1Debug`:
  - `resume` becomes an async `run {until: {ticks: 1}}`, which also stops at
    their v1 breakpoints, stop probes and memchecks;
  - `status` is answered from the fork's `state`;
  - `pause` becomes `halt`;
  - a tick stop that executed nothing (after a load at the loop top) is run
    again.
- **States.** States resolve through
  `build/startup-reference/fork-states/manifest.json`
  (`fork_state(name, generation)`).
- **Outputs.** Fork outputs never mix with the v2.6.3 captures.
  - The phase generation (the default) mirrors each v2.6.3 path under
    `build/fork_refs/`: `build/s87/route_a01/...` becomes
    `build/fork_refs/s87/route_a01/...`, and so on. Each set gets a
    `manifest.json`.
  - `--generation base` keeps the first regeneration's layout,
    `fork-states/beats/...`.
- **VU1 thread.** Capture tools run the fork with VU1 on the EE thread
  (`--fork-mtvu off`, the default). With VU1 on its own thread (the capture
  ini's `vuThread = true`), two runs of route beat 00 with identical EE state,
  rows and displayed field differed in GS memory at the end snapshot (221,348
  bytes, in the letterbox rows of one buffer and two other areas). With it off,
  the two runs' GS freezes were identical, and EE state and rows were the
  same as with it on. `video_compare/ps2_fork.py` keeps its own
  `--fork-mtvu` (default: the ini's value).

### Inventory: what used v2.6.3, and where it stands

The tools and probes that launch the v2.6.3 app or read its slots and states
(found by searching for the app path, `portable-data/sstates`, slot numbers,
`-statefile`, `OriginalSession` and port 21512):

| Tool | What it used on v2.6.3 | Sets it writes (legacy path) | Fork status (2026-10-09) |
|---|---|---|---|
| `tools/pcsx2_session.py` | `OriginalSession`: the app with `-statefile` on a user slot or a beat snapshot | the caller's | command line defaults to the fork (`--generation auto`: phase, else base); library: `ForkSession`, `ForkV1Debug`, `open_original`, `fork_state(..., generation)`, `read_phase` |
| `tools/route_capture.py` | `RouteSession` from slot 04 and each beat's snapshot | `build/s87/route` (00..15); `route_a00` .. `route_a22b`, `route_a13*`, `route_a15*`, `route_a19*`, `route_a03` (23 groups); `s87/c7cap/<item>/` (c7); `aimfire/capture`; `c10/{exit,damage,branch,options}` | fork by default, phase-locked (lead-in and tail replayed); `fi`/`fld`/`vs` in every row; `build/fork_refs/<path>`; a manifest per set; states hard-linked into `fork-states/phase/beats/`; `legacy-refs` caches the v2.6.3 facts |
| `tools/route_census.py` | `CensusSession`: thousands of one-shot v1 breakpoints, from slot 01 and beat snapshots | `build/s87/census` | fork by default through `ForkV1Debug`; beats with the recorded lead-in; outputs to `build/fork_refs/s87/census`; smoke-tested on beat 00 (same 518 functions as v2.6.3) |
| `tools/sfx_request_probe.py` | census sessions with persistent breakpoints | `build/sfx_probe` | fork by default; `build/fork_refs/sfx_probe`; not run yet |
| `tools/c7cap_capture.py` | `C7Session`; `ColdSession` (cold boot); slot loads through PINE slots >= 40 | `s87/c7cap/{stream,lane3,rng}` | fork by default; stretches re-drive the fork chain; `build/fork_refs/s87/c7cap`; **`lane3 --from boot` stays v2.6.3-only**; not run yet |
| `tools/c7cap_partb.py` | fb, h7 and fb2 sessions (fb2 needed the ini switched to Renderer 13 by hand) | `s87/c7cap/{fb,h7,fb2}` | fork by default (always software renderer, no ini switch); points from the fork chain; GS register page found at the fork state's offset (1,290 bytes after the tag against 1,246); `build/fork_refs/s87/c7cap`; not run yet |
| `tools/load_wait_probe.py` | `ProbeSession` (breakpoints) from beats 01 and 03 | `build/s87/loadwait` | fork by default, with the recorded lead-in; `build/fork_refs/s87/loadwait`; beat 01 breakpoints mode run (same loader events as v2.6.3) |
| `tools/gs_conformance.py` | slot 04 session, GS kick through `set_pc` (Renderer 13 by hand) | `build/b16/gscap` | fork by default from fork slot 04 (always software); `build/fork_refs/b16/gscap`; not run yet |
| `tools/ee_float/harness.py` (`battery.py`, `blockcheck.py`, `vusig.py`) | `OriginalSession` on slot 04, v1 `step` | `build/startup-reference/ee_float` | `open_original("04")`: fork by default; not run yet |
| `tools/video_compare/ps2.py` (+ `ps2_fork.py`) | slot 01 and Media Capture | `build/video_compare/<rec>/ps2` | fork by default (`ps2_fork.py`, a cold boot); every row records the frame index and field; `--fork-title-delay`; run on 2026-10-09 (demo_level) |
| `tools/repack/proof.py` | a private copy of the app, cold boot of a rebuilt image | `build/repack/...` | fork by default (`ForkColdDisc`: no `-elf`, memory cards off); unit tests pass (15); not run live yet |
| `tools/fork_states.py` | reads slots 01..15 and `build/s87/route` offline (match targets) | `fork-states/` | fork only; the slots' phases are cached in `build/fork_refs/legacy_refs.json` |
| `tools/parse_pcsx2_state.py`, `export_gltf.py`, `export_native.py`, `clut_pair.py`, `verify_all.py` (glTF stage) | offline parsing; their examples and the glTF stage read `~/Library/Application Support/PCSX2/sstates` (another install, not the 2.6.3 app) | none | unaffected |
| `build/startup-reference/*.py` (18 disposable probes; 11 launch the app: `collision_run_poll`, `first_control_poll`, `first_control_trace`, `interrupted_run_poll`, `elevator_probe`, `elevator_refusal_probe`, `panel_probe`, `panel_root_probe`, `roger_encounter_probe`, `status_hub_probe`, `debug_probe`) | user slots 03, 04, 06, 07, 08, 11, 12, 13, 14, 15 | `panel/`, `status-hub/`, `elevator/`, `roger-encounter/`, the `*_poll.json` files, `playable_ee.bin`, `opening_ee.bin`, `*_samples.jsonl` | not ported (ignored, disposable). `fork_states.py status` and `roger` replace 08/12/14/15; the rest is re-recorded from route groups (plan below) |
| ignored ad-hoc scripts: `build/s87/frame_trace*/` (13), `build/s87/audio/tools/` (3), `build/startup-reference/cutscene_skip/skip_capture.py`, `build/s87/ee_float/harness.py` (old copy) | `OriginalSession` / `RouteSession` | `s87/frame_trace*`, `s87/audio`, `startup-reference/cutscene_skip` | not ported. Those built on `RouteSession` follow `route_capture.FORK` once they call `use_fork()` |

Port tests that read these sets (counted with `git grep` in the port, docs
excluded): `build/s87/route/` 38 files, `s87/census` 39, `s87/c7cap` 11,
`aimfire/capture` 5, `c10/exit` 5, `c10/damage` 5, `c10/branch` 2,
`c10/options` 3, `s87/loadwait` 2, `s87/audio` 4, `frame_trace` 12,
`startup-reference/panel` 14, `status-hub` 11, `playable_ee.bin` 72,
`opening_ee.bin` 43, the level groups `route_a00` .. `route_a22` 1 to 16
each, and 5 tests that read user slots 01, 02, 04 and 14 offline. They all
keep reading the v2.6.3 files until a port-side switch; nothing here moves or
deletes them.

## Fork states: the tools' save states regenerated in the fork

`tools/fork_states.py` rebuilt, from a **cold boot in the fork**, the v2.6.3
states the tools use (2026-10-09, decomp `640fac0`, fork build
`v2.9.114-11-gc105df140`). Each
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

### Using the fork states

```sh
# macOS (arm64 host; the fork runs x86_64 under Rosetta), decomp .venv
cd /Users/abe/Documents/Extermination.nosync/Extermination
# step the original from fork slot 04 (first control) and snapshot it
.venv/bin/python tools/pcsx2_session.py 04 --frames 30 --snapshot build/<task>/walk
# run route beats in the fork, phase-locked (sources resolve through the
# manifest; output goes to build/fork_refs/s87/route/..., never to build/s87/...)
.venv/bin/python tools/route_capture.py run --beats 07
# the first regeneration's layout instead (fork-states/beats/...)
.venv/bin/python tools/route_capture.py run --beats 07 --generation base
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
- **Stepping.** `align=True` (the default) makes one `run {ticks: 1}` to the
  loop top after the load; a state saved at the loop top stays where it is.
  `align=False` stays exactly on the loaded state. Either way `step(n)`
  advances exactly n frames: a state saved at the loop top loads with the PC
  on the tick PC, and the fork counts that tick without executing, so a tick
  stop that executed no EE cycle is run again once (two in a row raise an
  error). Before decomp `184f6f1`, `align=False` lost the first step
  (counter 3300 -> 3300 from state 04). Test:
  `tools/test_pcsx2_session_fork.py` (4 unit checks; `--live` adds 4 checks
  on state 04).
- `snapshot()` saves with `state_save` (exact) and takes `original.png` from
  `gs_field`.
- `ForkSession(None)` cold-boots the disc with a fixed RTC and passes no
  `-elf` (none is needed). An `-elf` override used to leave the EE in the
  kernel: `$TMPDIR` lies under the symlink `/var`, and the emulator accepts
  the override only when `-elf` names the real path. The fork's launcher
  passes real paths since fork commit `1e22fdc0c`; checked on 2026-10-09: a
  cold boot with `-elf` from the default scratch loaded the ELF and reached
  tick 120. See [PCSX2_FORK_GS_DIFF.md](PCSX2_FORK_GS_DIFF.md) section 9.
- The scratch folder, ISO clone and lock are removed on close.

### Phase-locked states and captures (2026-10-09)

> **Superseded in part on 2026-10-10.** This section's phase target was the
> slot's stored frame index and field, but every user slot was saved inside an
> iteration (at the vsync wait), so the right target is the loop-top phase
> that a v2.6.3 session from the slot started on: the stored one flipped.
> The skip-at-40 states below ran one game update ahead and have been
> replaced; the phase states now come from the d = 0, no-skip boot (the same
> machine state as the base boot). The lead-in, tail, per-row phase report
> and cache below still hold. See "Correction (2026-10-10)".

**Why.** A capture shows the same picture as its v2.6.3 reference only when
both draw the same game tick with the same phase
([PCSX2_FORK_GS_DIFF.md](PCSX2_FORK_GS_DIFF.md) section 8):

- the frame index D_00810E80 picks the drawing buffer. In every capture so
  far, of both builds, it equals the parity of the main-loop counter;
- the field D_00810E88 picks the half-line draw offset. In every capture
  so far it equals the parity of the game's vsync counter 0x00810E90.

On 2026-10-09 the base generation looked off-phase: its boot gave slots 02,
03 and 04 frame index 0 and field 1, against the stored 1 and 0 of the
v2.6.3 slots. That comparison was wrong (the stored values are
mid-iteration); the base states have the v2.6.3 loop-top phase.

**The lever** (`fork_states.py boot --phase-lock`). From `slot01_title`
every input lands on a fixed tick, so a boot is repeatable. The scan, with
receipts in `build/fork_refs/logs/phase_lock_scan_delays.json` and
`phase_lock_boot_skip*.log`, found the following:

- **Holding the title driver back d ticks** moves the New Game commit by d,
  but the AREA11 load absorbs most of it:

  | d | Points at | Frame index, field |
  |---|---|---|
  | 0 | counter 2156 | 0, 1 |
  | 1 | counter 2158 | 0, 1 |
  | 2 | counter 2158 | 0, 1 |
  | 3 | counter 2159 | 1, 1 |

  The field at the opening points stayed 1 in every case.
- **Skipping the New Game movie with START** at a fixed emulated vsync after
  a stop probe on the movie driver func_00203350 decides the field.
  - A skip 0, 1 or 2 vsyncs after the driver starts changes nothing: the
    commit stays at game vsync 11,443.
  - A skip **40 vsyncs** after it gives slots 02, 03 and 04 frame index 1
    and field 0, as in v2.6.3. The points are at counters 2155, 3179 and
    3299, and each matches its v2.6.3 slot in every compared byte (score 0).

  These were the first `phase/slot02_opening`, `phase/slot03_fade_in` and
  `phase/slot04_first_control` (`press_delay` 0, `skip_at` 40; the scan
  pass and the save pass landed on the same counters). They were replaced
  on 2026-10-10.

**Captures** (`route_capture.py --generation phase`, the default):

- **Lead-in and tail.** Each beat first runs its v2.6.3 capture's lead-in:
  the frames the legacy session ran between the load and row 0, which is
  the trace's first counter minus the source state's counter. The pad stays
  as the session left it. The v2.6.3 retry tail is replayed too. When an
  earlier beat already left the v2.6.3 timeline and the source lands on the
  other parity, one more frame restores the phase
  (`phase_correction_frames`).
- **Phase per row.** Every row records `fi` (D_00810E80), `fld`
  (D_00810E88) and `vs` (the game's vsync counter).
- **Report.** `trace.json` "phase" compares each row with the v2.6.3 row
  of the same index:
  - the v2.6.3 frame index is its counter's parity;
  - its field follows from the pairing measured at its source and end
    snapshots, and is left unknown when the two disagree.
- **Where things go.** Beat folders go to `build/fork_refs/<v2.6.3 path>/`,
  each set's `manifest.json` lists every beat (source, lead-in, tail,
  counters, phase report, state hash, fork build), and each beat's
  `state.p2s` is hard-linked into `fork-states/phase/beats/<path>/` and
  registered in the fork-states manifest as `phase/route/<beat>`.
- **Cache.** `route_capture.py legacy-refs` caches every v2.6.3 fact the
  lock needs in `build/fork_refs/legacy_refs.json`: lead-ins, tails, row
  counters, phases at the sources and ends, and the 12 user slots' phases.
  The lock therefore keeps working after the slots and the app are retired
  (filled on 2026-10-09: 225 beats, 12 slots).

**Proof on beat 00** (receipt `build/fork_refs/phase_proof/00_panel_no_battery/proof.json`;
the four run folders were deleted on 2026-10-10 as one-update-early outputs).
Beat 00 was run from the old `phase/04` with its lead-in of 11 frames, four
times. Repeatability and the VU1-thread finding stand; "identical in every
traced field" covered 13 player fields only and missed the one-update lead:

| Check | Result |
|---|---|
| Repeatability | Runs 1 and 2 (VU1 thread on) and runs 3 and 4 (off): all 261 rows identical in every field. The end EE memory and the displayed field are identical in all four runs. |
| Frame index and field against the v2.6.3 rows | Equal on 261 of 261 rows in every run (counters 3310..3570 against 4094..4354). |
| Phase at the end snapshot | The fork's (0, 1) equals the v2.6.3 snapshot's (0, 1). |
| Traced fields and inputs against v2.6.3 | Identical in every traced field on all 261 rows; the inputs are equal. |
| GS memory | The two VU1-thread-on runs differed in 221,348 bytes of GS memory: the letterbox rows of one buffer and two other areas. The two VU1-off runs were identical. Hence capture tools default to VU1 off. |

**The main route and the other phase-locked states (2026-10-09, replaced).**
Run on 2026-10-09 with the default command (`route_capture.py run --beats
00,...,14`) from the old `phase/04`, then `fork_states.py status --generation
phase` and `roger --generation phase`. That chain, its receipts and these
states were one game update ahead (next section) and were replaced on
2026-10-10; the table is kept as the record of what the 13-field comparison
showed.

| Beats | Against the v2.6.3 traces |
|---|---|
| 00..12 | Identical in every traced field on every row, the same frame count and the same inputs, with the frame index and field equal on every row: 261 + 517 + 388 + 686 + 577 + 677 + 206 + 558 + 240 + 533 + 3,569 + 1,261 + 337 rows. The end snapshots have the v2.6.3 phase. This includes beats 05 and 06, which drifted in the base chain, and 06 now has 205 frames as in v2.6.3 (its retry tail of 23 is replayed). |
| 13 | Phase equal on all 803 rows. The position first differs at frame 133, by 0.1, as in the base chain. The v2.6.3 run has a host-timed one-frame stop there (CAPTURES_C7.md: three replays all left the recording at f133). The fork beat takes 802 frames against 809, so its end lands on the other parity. |
| 14 | One phase-correction frame after the lead-in (`phase_correction_frames` 1) restores the v2.6.3 phase: equal on all 1,819 rows and at the end. The traced positions start from 13's slightly different end. |
| status 08 / 12 / 14, Roger 15 | Phase equal to the v2.6.3 slots. 08, 12 and 14 differ in the same 49 bytes as the base chain; 12 needed one extra tick (`phase_extra_ticks`). 15 matches in every compared byte. |

Beat 15 (the level exit, minutes of host time) was not run.

**Run to run.** The first of five chain runs of beat 01 left the v2.6.3 path
at frame 16, by 1e-4 in position. A re-run of that beat and three repeats
from `phase/04` were identical to v2.6.3. The cause was not found; the run
shared the host with a parallel `-j10` build. Compare a re-recorded set
with its v2.6.3 trace (`compare_v263.json` does it for the main route)
before relying on it.

**Tools with breakpoints on the fork.** Smoke tests on 2026-10-09:
- `load_wait_probe.py run --beats 01 --mode breakpoints`: 517 of 517 rows
  equal the fork chain's trace. The loader events are 24 dispatches, 17
  polls, 3 reads and 1 request, the same counts as the v2.6.3 probe.
- `route_census.py run --segments 00 --pass FORKSMOKE`, which arms every
  candidate: 518 functions, the same set as v2.6.3 pass A. The end digests
  equal the recorded snapshot, with no unexpected pauses. It took 56 s.

Both ran through `ForkV1Debug`, with persistent, one-shot and return-site
breakpoints. Both smokes were re-run on 2026-10-10 from the canonical chain
(the outputs above came from the one-update-early chain) with the same
results, and each set now has a `manifest.json` (`route_capture.note_fork_set`:
source, lead-in, rows equal to the recorded beat, fork build).

### Correction (2026-10-10): the phase chain was one game update early; one canonical chain

The phase-locked chain of 2026-10-09 started one game update ahead of v2.6.3.
The traced fields the comparison covered did not show it, but the player's
clock, the r9 attachment node (`attach_r9`), the snow and every other
`rand()`-driven effect did.

- **Why.** Every v2.6.3 user slot (01, 02, 03, 04, 06, 07, 08, 11, 12, 13, 14,
  15) was saved by hand inside an iteration: the EE PC stored in each is
  0x1AAFF0, the first instruction of the main loop's vsync wait. Route
  snapshots (v2.6.3 and fork) are at the loop top 0x1AAF28. At the vsync wait
  the iteration's game logic has run, but the end-of-iteration frame-index
  toggle, the vsync ISR and the counter increment have not.
  - The PC is `cpuRegs.pc`, 712 bytes after the `cpuRegs` tag in the state's
    internal structures (`route_capture._legacy_save_pc`). Slots 06..14 have
    a shorter header (their session did not boot with `-elf`), so the tag sits
    0x4F bytes earlier; the 2026-10-09 note that they "store PC 0" read a
    fixed offset. The word 80 bytes before the PC is CP0 EPC, which holds the
    vsync wait in every state, route snapshots included.
  - A v2.6.3 session that loads such a slot first finishes that iteration,
    with no new logic. Its first loop top (counter + 1) has the slot's game
    state and the other phase: slot 04 stores (1, 0); the loop top after the
    load has (0, 1).
  - The 2026-10-09 `phase/slot04` (counter 3299) had the slot's game state
    with the stored (1, 0) at a loop top; replaying the full v2.6.3 lead-in
    from it ran one update more than v2.6.3. The base `slot04_first_control`
    (counter 3300, (0, 1)) is the v2.6.3 loop top 4084.
- **Measured.** From the old `phase/04` the fork made the same 22 `rand()`
  calls per tick as v2.6.3, but one tick early: at the end of every route
  beat its random-number state was 20 to 56 calls ahead, and all 18 fb2
  points re-recorded from it differed (snow and the r9 attachment).
- **The fix, in the tools themselves.**
  - `route_capture.slot_start(slot)` gives the state and lead-in correction
    of any beat or point that starts from a user slot: the fork state whose
    frame index and field are the slot's loop-top phase
    (`legacy_slot_loop_top_phase`: the stored phase flipped when the cached
    save PC is the vsync wait), and -1 frame of lead-in.
    `route_capture.run_beat`, `slot_path` (phase generation),
    `c7cap_partb.py fb2` (first_control) and every re-driver that reads a
    recorded lead-in (`load_wait_probe`, `route_census`, `fork_states.py
    rerun`) use it by default. Route snapshots need no change.
  - `fork_states.py` locks the phase generation to the loop-top phase and
    scores the `rand()` state and the player's clock (two new spans), so a
    state one update away no longer scores 0.
  - The library default of `route_capture` is the fork, phase generation
    (`FORK` follows `EXTERMINATION_PCSX2`); the v2.6.3 app is used only
    when chosen (`use_legacy()`, `--emulator legacy`).
- **One canonical chain.** `build/fork_refs/s87/route` is the corrected main
  route, origin base/phase `slot04_first_control` (counter 3300) with the
  v2.6.3 lead-in minus 1 (`slot_lead_in_correction` -1 in beats 00 and 01).
  It was re-recorded with `route_lanes.py` on 2026-10-10 00:24..00:48 and
  every group was chained from it afterwards (each of the 112 group beats'
  source state predates the beat). The separate copy that
  `fork_pixel_refs.py chain` had written to `build/fork_refs/pixels/chain/`
  was identical to it (all 16 beats: the same rows and the same end EE
  memory) and was deleted; `fork_pixel_refs.py` and `c7cap_partb.py` read
  the canonical chain (`--corrected-chain` is now a no-op).
  - Beats 00..12 equal the v2.6.3 traces in **every** traced field (clock and
    `attach_r9` included) on every row, and their end random-number states
    are equal (`build/fork_refs/s87/route/compare_v263.json`,
    `build/fork_refs/compare/main.json`).
  - Beat 13 still leaves v2.6.3 at frame 133, at the v2.6.3 run's host-timed
    stop, and beats 14 and 15 inherit that (below).
- **The phase states, regenerated** (2026-10-10, `fork_states.py boot
  --phase-lock --no-skip --delays 0`, then `roger` and `status`, all phase
  generation by default now):

  | State | Counter | Against the v2.6.3 slot |
  |---|---|---|
  | `phase/slot02_opening`, `phase/slot03_fade_in`, `phase/slot04_first_control` | 2156, 3180, 3300 | loop-top phase (0, 1); every compared byte equal, `rand()` state and player clock included; EE memory and scratchpad identical to the base states (the boot is deterministic) |
  | `phase/slot15_roger_encounter` | 3246 | from the new `phase/slot03`: loop-top phase, every compared byte equal; EE memory identical to the base state |
  | `phase/slot08_battery_prompt`, `phase/slot12_status_root`, `phase/slot14_status_hub` | 4632, 4674, 4690 | from the canonical route 02: loop-top phase (08 and 12 needed one extra tick); 55 bytes differ: the same 49 as before (below) plus the `rand()` state (4) and the player clock (2), because the v2.6.3 slots come from a hand-played run |
  | `slot01_title` (`phase_free`) | 1313 | the base state already has the loop-top phase (1, 1) |

  `fork_states.py verify` on the 11 slot states (base 01..04, phase 02..04,
  08, 12, 14, 15): all load, the live machine equals the file, and two steps
  advance the counter by one each (`fork-states/verify.json`).
  The fork-states manifest's `default_generation` is now `phase`, and each
  slot entry's `phase` records the stored and the loop-top v2.6.3 phase.
  The base `slot08_battery_prompt` is the only slot state off the loop-top
  phase. The slots' compared spans are cached in `legacy_refs.json`
  (`slot_spans`), so `boot`, `status` and `roger` work without the slot
  files.

### Pixel references re-recorded on the fork (2026-10-10)

User decision of 2026-10-09: re-record the v2.6.3 pixel references in the
fork, with a deterministic field phase. Everything is in the ignored
`build/fork_refs/pixels/<set>/`, in the layout the port's tests read today,
with a `manifest.json` per set. The manifests record the tick, the field
phase (D_00810E88, CSR FIELD and OFY), the drawing buffer (D_00810E80 and
FRAME), the fork build, and the comparison with the v2.6.3 frame.

- Fork build: `v2.9.114-11-gc105df140`, server 0.2.2, repository HEAD
  `2138d41fa`.
- Settings: software renderer, VU1 on the EE thread.
- Tool: `tools/fork_pixel_refs.py` (`chain`, `gscap`, `route`, `compare`).

| Set | Port reader (switch) | Result against v2.6.3 |
|---|---|---|
| `fb2` (19 points, CAPTURES_C7.md 5b) | `test_fb2_pixels.py`: `FB2` -> `build/fork_refs/pixels/fb2`, `ROUTE` -> `build/fork_refs/s87/route`, `FIRST_CONTROL_COUNTER` 4085 -> 3301 | **14 of 19 displayed fields bit-exact**: 00..05, 07, 09..12, first_control, route03_end, route07_end. draw.bin is exact at 13 points, z.bin at 16. The 5 others are below. |
| `b16` (GS conformance: gscap, gscap3..8, gscap_repeat) | `test_gs_raster_reference.py`, `test_gs_fog_conformance.py`: `GSCAP_ROOT=build/fork_refs/pixels/b16` | **906 of 906 tests bit-exact** (and 252 of 252 in the repeat), colour and Z. All 37 batch packets are equal. 187 of 187 page-aligned CT32 uploads are equal, and 111 of 111 in the repeat. 15 T8, T4 or unaligned uploads were not compared directly (11 in the repeat). The local memory outside the tests' buffers holds the game's own frame at the kick, a different moment in each run. |
| `route` (main route end frames) | `test_shadow_original_reference.py --capture` reads `original.png` | Displayed and drawing buffers decoded from the canonical chain's snapshots (`build/fork_refs/s87/route`). **No v2.6.3 pixel reference exists**: those snapshots were saved with Metal, so their GS buffers hold the 0x80000000 fill. Their `original.png` is the 640x480 host presentation, which is not even tick-exact (route 03's shows an earlier letterboxed frame). The fork's `original.png` is the 512x224 field, so this switch needs a port-side change, not only a path. |

How fb2 is captured: `c7cap_partb.py fb2 --fb2-out build/fork_refs/pixels/fb2`
(re-run on 2026-10-10 from the canonical chain: the same 14 of 19, below).
- Each point loads the canonical chain's state for its point (first_control:
  `route_capture.slot_start("04")`, the loop-top state of slot 04).
- It replays the v2.6.3 session's own free frames after its load
  (`--lead legacy`, s0 minus the recorded counter, minus 1 on a vsync-wait
  slot), so s0, s1 and s2 are the v2.6.3 ticks.
- It then runs the same two steps with sync snapshots as in v2.6.3.

The five fb2 points that differ (`fb2/manifest.json` "cause"; difference
images in `pixels/_compare/fb2/`):

| Point | Displayed RGB pixels | Cause |
|---|---|---|
| 06_hill_slide | 86,371 | **Field phase.** The v2.6.3 session had a post-load hitch: one iteration spanned 17 vsyncs (none in the fork). The v2.6.3 field was drawn at OFY 1936.0, the fork's at 1936.5. Game state, rows and random state are equal. |
| 08_truck_crossing | 105,364 | **Field phase**, the same kind: a 21-vsync iteration in v2.6.3. OFY 1936.5 against the fork's 1936.0. Rows and random state are equal. |
| 13_east_tower | 80,495 | **Route divergence.** The v2.6.3 recording of beat 13 has a host-timed one-frame stop at f133 that no replay reproduces. The position and camera differ from there, the beat ends 7 frames earlier, and phase and random state follow. |
| 14_roger_encounter | 3,030 | **Random state** inherited from beat 13: only snow flakes differ. Rows, phase and z.bin are equal. draw.bin differs (99,275) because the v2.6.3 post-load hitch (23 vsyncs) put s0 on the other half line. |
| 15_level_exit | 93,145 | **Field phase** (OFY 1936.5 against 1936.0 at s1; v2.6.3 had 2 extra vsyncs after its load) and the random state inherited from beat 13. Rows are equal. draw.bin differs in 4 pixels by 1, and z.bin is equal. |

None of the differences is the upstream scissor change: no differing frame
is confined to row 223. None is AREA11 load timing either, because the fb2
points are not inside a load. The field-phase cases are v2.6.3 host-timing
artefacts, so the fork frame is the reproducible one.

### Route groups re-recorded in parallel lanes (2026-10-10)

**Tool.** `tools/route_lanes.py` re-records route_capture groups on the fork.

- **One session, several instances.** It takes the run lock once and starts
  up to `--lanes` worker processes. Each worker runs one beat on its own
  DebugServer port (21612 + lane) and PINE slot (28200 + lane): the
  `pcsx2_session.FORK_LANE` environment. The fork guide allows several
  instances per lock.
- **Scheduling.** A beat starts as soon as its source beat exists, critical
  path first. Every `--yield-every` seconds (900 by default) the scheduler
  lets the running beats finish and releases the lock for a few seconds, so
  other runs waiting on it get their turn; the depth-of-field job did.
- **The beat itself** is route_capture's `run_beat`, unchanged: the same
  driving, closed loop where it is, phase-locked, with a fork-saved snapshot
  that is checked to resume. Output, set manifests and fork-states
  registration are route_capture's.
- **Attempts.** A beat whose traced fields leave the v2.6.3 path is run
  again, up to `--attempts` times. An identical rerun stops it (the
  difference is the fork's own). Otherwise the attempt closest to v2.6.3 is
  kept. The set manifest lists every attempt (`attempts`, `kept_attempt`),
  and `_attempts/<beat>/<n>/trace.json` keeps the traces.
- **Comparison.** `compare` sets each beat against its v2.6.3 trace in
  `build/fork_refs/compare/<group>.json`:
  - every row field except the time keys (counter, row index, frame index,
    field, vsync, and the counters inside the pad block and scratchpad
    slices), at the same row index;
  - then aligned by game state: difflib over row fingerprints, where a "hold"
    is extra rows that repeat the previous row (a wait or a load);
  - the end `rand()` state and how many calls apart it is.
  `report` writes `compare/summary.json`.

```sh
# macOS arm64 host (the fork runs x86_64 under Rosetta); decomp .venv
.venv/bin/python tools/route_lanes.py run --groups main,c7,aim,exit,dmg,br,opt --lanes 4 --attempts 3
.venv/bin/python tools/route_lanes.py run --groups levels --lanes 4 --attempts 2
.venv/bin/python tools/route_lanes.py compare --groups main,c7,aim,exit,dmg,br,opt
.venv/bin/python tools/route_lanes.py report
```

**Repeat runs.** `run --out-root DIR` (determinism checks) writes the beats,
their set manifests, the scheduler log and the per-beat lane logs under
`DIR` (`DIR/logs/lanes/`); the official lane logs in
`build/fork_refs/logs/lanes/` are never overwritten by it, and every lane log
is opened for append with a header per run (2026-10-10).

**Fixes found on the way** (decomp `1bba254`):

- **The slot start.** The user-slot correction above is applied in
  route_capture itself.
- **EXIT_PIN.** exit_00, dmg_08 and br_14 waited for the v2.6.3 main-loop
  counter 15800. The fork chain's counters are 784 lower, so they timed out.
  `route_capture.exit_pin()` keeps the pin's distance from route 14's end
  snapshot.
- **a02_04.** A bug grabs the player during the turn at the panel on the
  fork chain. The beat now shakes it off (`a04_shake`) and turns again. On a
  run without a grab the policy is unchanged.

**Results, first level** (4 lanes; 64 beats in 25 min of wall time):

| Group | Beats | Rows fork / v2.6.3 | Equal to v2.6.3 in every field and phase | The rest |
|---|---|---|---|---|
| main 00..15 | 16 | 13,248 / 13,241 | 00..12 (also the end `rand()` state) | 13: the v2.6.3 run's host-timed stop at f133 (as before). 14, 15 inherit it; 15 also has the fan's slow window one cycle later, 816 rows against 802 |
| c7 | 1 | 548 / 545 | – | row 200: action 1 where v2.6.3 has 5, same inputs; 11 of 12 fork runs agree on 1 |
| aim | 12 | 7,273 / 7,272 | 00..04, 08, 10, 11 | 06, 07, 09: the processed pad's held/pressed edge one frame apart on 1 or 2 rows, nothing else. 05: the same at row 217, then a different shot sequence from row 375 |
| exit | 2 | 815 / 802 | – | inherit route 14 (fan timers from row 0) |
| dmg | 9 | 7,296 / 7,292 | 00, 06 | 02, 07: the loader read gate D_00282157 on 3 rows. 03: the game-over screen module's fade and task slots from row 261 (load timing); 04, 05 follow. 01: a pad edge at row 569, then a different flame path. 08 inherits route 14 |
| br | 15 | 11,270 / 11,273 | 00, 06, 07, 10 | 01..05, 08: the loader read gate on 3 to 53 rows, plus the pad block's state on rows 0..1. 09: a pad edge at row 125. 11: a pad edge at row 1031, then a different climb; 12, 13 follow. 14 inherits route 14 |
| opt | 9 | 3,094 / 3,094 | – | every beat: the memory-card record D_00810040 (+0x38, +0x40). The fork's states come from a boot without the user's memory card, and that record is zero, as in the port. Some beats also differ in the loader read gate and the sound handle (snd150) on a few rows |

**Every remaining first-level difference, with its cause** (2026-10-10
review of `build/fork_refs/compare/<group>.json`; every group was recorded
from the canonical, slot-corrected chain, so none of these is the one-update
lead). "Equal" means every row field at the same row index, the frame index
and field on every row, the inputs, and the end `rand()` state.

- **Equal (27 of 64):** main 00..12; aim 00..04, 08, 10, 11; dmg 00, 06;
  br 00, 06, 07, 10.
- **v2.6.3 host timing in the reference itself:**
  - main 13: the v2.6.3 recording's one-frame stop at f133 (no replay
    reproduces it). Rows 0..132 equal; then position and camera; 803 rows
    against 810; end `rand()` 157 calls apart.
  - aim 06 (row 93), 07 (row 331), 09 (row 14): the processed pad's
    held/pressed edge one frame apart on 1 or 2 rows (v2.6.3 pad latency,
    finding 2). Nothing else differs; the end `rand()` state is equal.
  - aim 05: the same kind of edge at row 217, then a different shot
    sequence from row 375 (end `rand()` 147 calls apart).
  - dmg 01: a pad edge at row 569, then a different flame path (end `rand()`
    equal).
  - br 09: the pad block's state on rows 0..1, the loader read gate, and a
    pad edge at rows 125..126.
  - br 11: a pad edge at row 1031, then a different climb; br 12 and 13
    start from 11's end and differ from row 0.
- **Inherited from main 13 (through route 14):** main 14 (`attach_r9`,
  camera and position within 2e-5 from row 0; one phase-correction frame),
  main 15 (fan timers from row 0, the departure one fan cycle later: 816 rows
  against 802, the AREA01 rebuild at row 755 against 741), exit 00 and 01,
  dmg 08, br 14.
- **The fork's disc timing (finding 4):** the loader read gate D_00282157
  (`cd157`) for 3 rows in dmg 02, dmg 07, br 01, 03, 04, 05 and 08, and for
  53 rows in br 02 (plus the pad block on rows 0..1); dmg 03: the game-over
  screen module's fade and task slots from row 261, which dmg 04 and 05
  follow (their inputs then differ).
- **The memory card:** every opt beat differs in the memory-card record
  D_00810040 (`mc040`). The fork's states come from a boot without the
  user's card, so that record is zero, as in the port; opt 07 is equal in it
  from row 89 on. opt 00, 02, 03, 04, 06, 07, 08 also differ in the loader
  read gate and the sound handle (`snd150`) on a few rows.
- **Not explained:** c7 (the fence-door side beat): at row 200 the player's
  action is 1 where v2.6.3 has 5, with equal inputs and traced state; 11 of
  12 fork runs give 1. It is the same kind of event as finding 1.

**Results, level groups** (outside the first level; 4 lanes, 2 attempts).
59 of 159 beats were recorded: `a01` 16, `a00` 12, `a01r` 6, `a02` 7,
`a04` 7 of 10. `a01_00`, `a01_s0`, `a01_s2`, `a01_s4` and `a01_s6` equal
v2.6.3 in every field. The AREA01 arrival resets the world, so beat 15's
difference does not carry over.

- **Where the chain leaves v2.6.3.** At `a01_01` row 3, the player answers
  the same stick input 2 frames earlier than in the v2.6.3 run. Everything
  after it is the fork's own playthrough. Most of those beats are
  repeatable: the second attempt was identical.
- **The fork playthrough is not the v2.6.3 one.** In it, a bug bites and
  infects the player at the `a02_04` panel. The player then enters `a04_03`
  with health 41 against 90 and loses 2 every 240 frames.
- **Blocked.** `a04_03_back_to_hall` fails, the same way in both attempts:
  the injured idle clip 10 never satisfies `settle`'s clip-0 test. The 100
  beats after it are not recorded (`a04_03` .. `a04_05`, then `a22` ..
  `a03`).
- **What it would take.** Continuing means adapting the policies to the fork
  playthrough. The alternative is first putting the chain back on the v2.6.3
  path (below). That is the lead's decision.
- **Do not use these as references.** `a02_04` onward (with `a04_00`..`a04_02`
  and the `a04_s*` side beats) is the infected playthrough.

**Findings.**

1. **The fork's closed loop is not always repeatable.**
   - Route 10 from one state, run 10 times: 8 were identical in every row,
     every machine region (hash store, 3,568 frames) and the end EE memory.
     The other 2 left the path at rows 168 and 724, with the same inputs up
     to there.
   - The C7 beat, run 15 times with the software renderer: 4 left the path
     (rows 94 and 124, and two of the three attempts in the lanes run).
   - The C7 beat, run 8 times with the Null renderer: none did. That does not
     settle it.
   - Route 14 in the lanes run: 3 attempts, 3 different traces, from row 15.
   - Every departure is the same kind of event: the player's action becomes
     5 instead of 1 (or the reverse) on a frame where all traced state and
     the inputs are equal.
   - Also, two runs with identical EE state differed in GS memory at the end
     snapshot (54,296 words of route 00), with VU1 on the EE thread.
   - Open-loop runs with an input timeline did not diverge (4 instances,
     2,884 frames). Neither did runs with a hash-only frame store, which are
     slower.
   - Not explained. It needs a fork-side look: diverge a failing pair with
     full GS VRAM and IOP capture. Until then, the attempts above, and a
     second run of any beat that matters, are the guard.
2. **The v2.6.3 recordings carry host-timed pad latency.** On some rows the
   v2.6.3 run's processed pad (held/pressed) changes one frame earlier or
   later than the fork's, for the same input frame. Examples: aim_06 row 93,
   br_09 row 125, a01_01 row 3 (2 frames). The fork applies v1 `pad_set` at
   the next vsync, every time. The beats whose path changes at such an edge
   are aim_05, dmg_01, br_11..13 and the whole level chain after a01_01.
   Replaying the v2.6.3 latency per input (search the delay that keeps the
   rows equal) would put those beats back on the v2.6.3 path.
3. **The fork chain makes the same `rand()` calls as v2.6.3** once the slot
   start is corrected (the end state is equal for 00..12). The earlier
   "identical in every traced field" verdict missed the one-update lead
   because the comparison covered only 13 player fields.
4. **Loader timing.** The loader read gate D_00282157 and some screen-module
   fades differ by a row or two around disc reads. The game-over screen's
   load (dmg_03) is the visible case. This is the fork's disc timing.
5. **Request timeouts under 4 lanes.** A client request timed out 4 times in
   the first four-lane run (about 150 sessions), once inside a 300 s
   `state_save`; none in the later runs. The attempts absorbed them. Watch
   for them if lanes go above 4.

**Could the port's level smoke and side-run checkers switch as-is?** They
read the v2.6.3 paths today. The fork sets are under `build/fork_refs/` with
the same layout. Fork counters are 784 lower than v2.6.3's from first control
on (relative counters are equal).

- **Main route 00..12, aim 00..04, 08, 10, 11, dmg 00, 06, br 00, 06, 07,
  10.** Yes. Only the path changes: every field and phase is equal.
- **aim 06, 07, 09; br 01..05, 08; dmg 02, 07.** Very likely, unless a check
  compares the pad edge or the loader read gate row for row (the BRANCH and
  OPTIONS checkers name the busy byte D_00275BD8, not the read gate).
- **opt.** Likely. The card record is zero here, which is the port's
  "known difference". The OPTIONS checker compares the card record only from
  the load row's clear on, and it is equal there in opt_07.
- **No:**
  - `level_smoke_area01` hard-codes beat 15's arrival row 741 and its last
    counter;
  - the C7 side checks (row 200 on);
  - aim_05, dmg_01, dmg_03..05, dmg_08, br_09 (one edge), br_11..14, the EXIT
    pair and route 13..15: their windows hold different values;
  - every level group.
- The port test runs themselves are the final word. A port-side job makes
  the switch.

### What still needs v2.6.3 (2026-10-10)

- **To run the 2.6.3 app: one optional case.** `c7cap_capture.py lane3
  --from boot` arms memchecks before the game code runs, which its
  `ColdSession` does on the v2.6.3 app only. Use `--from title` on the fork,
  or port the cold start (a fork cold boot with `[UI] StartPaused`) when the
  lane-3 writers are needed again. No other tool needs the app.
- **The slot files: nothing.** Every v2.6.3 fact a tool reads from them is
  cached in `build/fork_refs/legacy_refs.json`: the 12 slots' stored phase,
  counter and save PC (`route_capture.py legacy-refs`, refreshed on
  2026-10-10), and the 12 slots' compared spans (`fork_states.py manifest`,
  `slot_spans`). `fork_states.py boot`, `status` and `roger` and the phase
  lock run from the cache.
- **The v2.6.3 captures: data the port reads** until its switch (checklist
  below). They stay: `build/s87/...`, `build/c10/...`, `build/aimfire/...`,
  `build/b16/...`, `build/startup-reference/{panel,status-hub,elevator,roger-encounter,cutscene_skip}`,
  `playable_ee.bin`, `opening_ee.bin`, the `*_poll.json` files and the slot
  files the 5 port tests read.
- **Fork runs not made yet** (fork re-recordings, not v2.6.3 needs; plan
  below): `sfx_request_probe`, `c7cap_capture` (stream, rng, lane3 from the
  title), `c7cap_partb` fb and h7, the full census (`route_census.py run
  --segments all`), `load_wait_probe` beat 03 and the boundary mode, the EE
  float vectors, the repack proof live, and the level chain after `a04_02`.
  Run on the fork so far: `pcsx2_session`, `route_capture` / `route_lanes`
  (every first-level group, 59 level beats), `fork_states`, `ps2_fork`,
  `gs_conformance` and probes 3..8 (`fork_pixel_refs.py gscap`),
  `c7cap_partb fb2`, and the smokes of `load_wait_probe` (beat 01) and
  `route_census` (segment 00).
- **User slots no tool regenerates:** 06 (the panel powered), 07 (panel clip
  0x15C), 11 and 13 (the elevator terminal). They were the probes' inputs,
  and no port test reads them. Their game points are on the canonical route
  (beat 03 powers the panel, beat 04 uses the elevator); their phases, save
  PCs and spans are cached.

### Video comparison on the fork (`ps2.py --emulator fork`)

`tools/video_compare/ps2_fork.py` is the PCSX2 pass of the video comparison
on the fork (decomp `8f9362f`), selected with `ps2.py --emulator fork` (or
`video_compare.py ps2 ... --emulator fork`). Full description:
[VIDEO_COMPARE.md](VIDEO_COMPARE.md) "The fork path".

```sh
# macOS arm64 host (the fork runs x86_64 under Rosetta); decomp .venv via the dispatcher
python3 tools/video_compare/video_compare.py ps2 build/video_compare/my_run.rec \
    --out build/video_compare/my_run/ps2_fork --stride 4 --audio --emulator fork
```

- **Start:** a cold boot of the disc image with a fixed RTC, the intro movie,
  then ps2.py's title driver to NEW GAME (no v2.6.x title state).
- **Ticks:** `run {until: {ticks: 1}}` at the loop top, a stop probe at the
  return from the input step (the processed pad block written there), and a
  stop probe at the movie driver func_00203350 (START skips land on the same
  vsync every run, so the pass is repeatable).
- **Pictures:** DISPFB2 read at loop top t + 2, then that buffer read from GS
  memory at t + 3 (the drawing FRAME buffer, ps2.py's capture point without
  save states); software renderer only. `--fork-field-k K` reads `gs_field`
  at t + K instead (diagnostics).
- **Sound:** `--audio` records the original's SPU2 output with the fork's
  emulated-time tap; each row's `af` holds its sample position, and
  `ps2/audio.wav` covers exactly the logged span.
- **Options:** `--fork-renderer`, `--fork-mtvu`, `--fork-app`, `--max-ticks`,
  `--fork-no-present` (safe from fork 0.2.2 on; presentation stays on by
  default).
- **Result (demo_level, 2026-10-09):** the whole first level in about 6 min
  (44.7 min on v2.6.3); position and heading equal to the v2.6.3 run on all
  13,632 paired ticks; the original's sound in the demo videos
  `demo_level_sound.mp4` and `demo_level_original_sound.mp4` (ignored,
  `build/video_compare/demo_level/`).

### Retiring the 2.6.3 app: checklist (2026-10-10)

The user decided on 2026-10-09 to migrate everything ("Yes, migrate
everything"; "Yes, re-record in fork"). The lead then trashes
`build/startup-reference/PCSX2.app` and the 12 user slot files (01, 02, 03,
04, 06, 07, 08, 11, 12, 13, 14, 15).

**Done (decomp side):**

1. **Tools on the fork.** Every tool in the inventory runs on the fork by
   default (decomp `4c563cd`); the `route_capture` library default is the
   fork too (2026-10-10). `--emulator legacy` / `EXTERMINATION_PCSX2=legacy`
   stay until the app goes.
2. **One canonical chain, slot start corrected.** `build/fork_refs/s87/route`
   (origin `slot04_first_control`, the v2.6.3 lead-in minus 1): beats 00..12
   equal v2.6.3 in every field and the end `rand()` state. Every tool that
   chains from it (`route_capture`, `route_lanes`, `fork_states`,
   `fork_pixel_refs`, `c7cap_partb`, `load_wait_probe`, `route_census`)
   starts a user-slot source from `route_capture.slot_start` by default.
3. **Every first-level group re-recorded** from it (`route_lanes.py`,
   2026-10-10): main 00..15, c7, aim, exit, dmg, br, opt (64 beats); 27 equal
   v2.6.3 in every field, and every other difference has a cause ("Every
   remaining first-level difference" above). Level groups: 59 of 159 beats,
   blocked at `a04_03`.
4. **The phase states regenerated** on the loop-top phase (02, 03, 04, 08,
   12, 14, 15; "Correction" above).
5. **Pixel references re-recorded** (the user's decision): fb2 14 of 19
   displayed fields bit-exact (re-run on 2026-10-10 from the canonical
   chain: the same 14, draw.bin 13, z.bin 16); GS conformance 906 of 906 bit-exact; the route end frames decoded from the
   canonical chain. The differences that cannot be removed: v2.6.3 host
   timing in the references (post-load hitches of 3 to 23 vsyncs, the f133
   stop of beat 13), frames inside the AREA11 load (4 or 5 iterations longer
   on the fork), and upstream's half-pixel scissor culling (the user's
   decision: follow the fork; a port-side job).
6. **The v2.6.3 facts cached.** `route_capture.py legacy-refs` (225 beats,
   12 slots with their save PCs) and `fork_states.py manifest` (the 12 slots'
   compared spans) ran on 2026-10-10. Re-run both if v2.6.3 captures change
   before the slots go.

**What still needs the 2.6.3 app:** nothing to run, except the optional
`c7cap_capture.py lane3 --from boot` ("What still needs v2.6.3").

**What the port-side switch must do** (a port job; until it is done the
port keeps reading the v2.6.3 files, so they must not be deleted):

1. **The 5 tests that read user slots offline** move to the fork states in
   `build/startup-reference/fork-states/` (each folder has `state.p2s`,
   `eeMemory.bin` and `scratchpad.bin`):

   | Port test | Slot | Fork state |
   |---|---|---|
   | `test_sound_bank_reference.py` | 01 | `slot01_title` |
   | `test_area11_sfx_reference.py` | 02 | `phase/slot02_opening` |
   | `test_player_slide_reference.py`, `test_truck_original_reference.py` | 04 | `phase/slot04_first_control` |
   | `test_vu1_object_kernel_reference.py` | 14 | `phase/slot14_status_hub` |

   The fork states are loop-top states: the slot's game state at the next
   loop top (the rest of the saved iteration done, no new game logic). Outside the compared spans (counters, render and DMA
   packet buffers, stacks, scratchpad work areas, about 460,000 bytes) they
   differ from the slots, and slot 14's chain differs in 55 compared bytes.
   A test that reads packet buffers or VU1 data from a slot must be checked
   against the fork state, not just re-pointed.
2. **`test_fb2_pixels.py`:** `FB2` -> `build/fork_refs/pixels/fb2`, `ROUTE`
   -> `build/fork_refs/s87/route`, `FIRST_CONTROL_COUNTER` 4085 -> 3301.
3. **`test_gs_raster_reference.py`, `test_gs_fog_conformance.py`:**
   `GSCAP_ROOT=build/fork_refs/pixels/b16`.
4. **Route readers** (`build/s87/route` 38 files, `s87/c7cap`, `aimfire/capture`,
   `c10/...`, `route_a*`): the same layout under `build/fork_refs/`. Fork
   counters are 784 lower than v2.6.3's from first control (790 from route
   14 on); relative counters are equal. Which checkers switch as-is: "Could
   the port's level smoke and side-run checkers switch as-is?" above.
5. **`level_smoke_area01.py`:** `ARRIVAL_ROW` 741 -> 755 and its last counter
   16562 -> 15786 on the fork's beat 15 (which differs from v2.6.3 from row
   0, inherited from beat 13).
6. **`test_shadow_original_reference.py --capture`:** it compares with each
   beat's `original.png`, which on v2.6.3 is the 640x480 host presentation
   (not tick-exact). The fork's is the 512x224 field; the decoded displayed
   and drawing buffers are in `build/fork_refs/pixels/route/<beat>/`. This
   needs a code change, not only a path.
7. **`test_message_capture.py`:** it reads the v2.6.3 screenshot
   `build/startup-reference/elevator/refusal/original.png`, written by the
   disposable `elevator_refusal_probe.py` (host presentation). No fork
   capture of that message frame exists yet: record one on the fork (route
   beat 02 holds the refusal) and change the test to the 512x224 field.
8. **The startup-reference probes' outputs** (`panel/` 14 files,
   `status-hub/` 11, `elevator/` 3, `roger-encounter/` 3, `playable_ee.bin`
   72, `opening_ee.bin` 43, the `*_poll.json` files, `cutscene_skip/`): the
   probes stay v2.6.3-only. Equivalents: `phase/slot02..04` (opening and
   first control), `phase/slot14_status_hub`, `phase/slot15_roger_encounter`,
   and route beats 02, 03 and 04 (elevator refusal, panel, elevator). The
   port decides per file; a file with no equivalent needs a fork capture.

**Then** the user can trash `build/startup-reference/PCSX2.app` and the 12
slot files (confirm first). Never delete the v2.6.3 captures above while any
port test reads them.

### Re-recording plan, set by set

Order matters: each group starts from snapshots of an earlier one. Every
command runs on the fork, phase-locked, hidden, under the run lock. It writes
to `build/fork_refs/<v2.6.3 path>` and a manifest per set, and registers its
states in `fork-states/manifest.json`. Each set's report says on how many rows
the frame index and field equal the v2.6.3 rows (`trace.json` "phase").

| # | Set (v2.6.3 path) | Command (decomp .venv) | Source | Port readers to switch later | Notes |
|---|---|---|---|---|---|
| 1 | main route 00..14 (`s87/route`) | `route_lanes.py run --groups main` | base `04`, lead-in minus one | 38 files | redone on 2026-10-10 with the slot correction, `route_lanes.py` ("Route groups re-recorded in parallel lanes"): 00..12 equal to v2.6.3 in every row field, phase and end `rand()` state; 13..15 differ (v2.6.3's f133 stop) |
| 2 | beat 15, level exit | `route_capture.py run --beats 15` | route 14 | (in the 38) | done on 2026-10-10: 816 rows against 802 (inherits route 14; the fan's slow window one cycle later) |
| 3 | status chain 08/12/14, Roger 15 | `fork_states.py status`; `fork_states.py roger` (phase generation by default) | canonical route 02; `phase/slot03` | slot 14 test, `status-hub/`, `roger-encounter/` | redone on 2026-10-10 on the loop-top phase ("Correction"): 15 equal in every compared byte; 08/12/14 differ in 55 bytes (hand-played v2.6.3 chain) |
| 4 | C7 group (`s87/c7cap/<item>/c7_*`) | `route_capture.py run --beats c7` | route snapshots | 11 files (`s87/c7cap`) | done on 2026-10-10: differs from row 200 |
| 5 | C7 stream / rng / lane3 (title) | `c7cap_capture.py stream`, `rng`, `lane3 --from title` | route 01/10/11/13, slot 01 | `test_iop_stream_reference` and others | lane3 `--from boot` stays v2.6.3-only |
| 6 | C7 fb, h7, **fb2 pixel points** | `c7cap_partb.py fb`, `h7`, `fb2` | the 16 route snapshots + first control (`slot_start("04")`) | `test_fb2_pixels.py` and others | fb2 re-run on 2026-10-10 from the canonical chain: the same result, 14 of 19 displayed fields bit-exact (draw.bin 13, z.bin 16), the five others with the causes above; fb and h7 not run |
| 7 | AIM (`aimfire/capture`) | `route_capture.py run --beats aim` | route 08 | 5 files | done on 2026-10-10: 8 of 12 equal in every field; 06, 07, 09 differ in one pad edge; 05 diverges |
| 8 | C10 EXIT (`c10/exit`) | `--beats exit` | route 14 | 5 files | done on 2026-10-10: both inherit route 14 |
| 9 | C10 DAMAGE (`c10/damage`) | `--beats dmg` | routes 07, 11, 14 | 5 files | done on 2026-10-10: 00, 06 equal; the rest loader timing, a pad edge (01) or route 14 (08) |
| 10 | C10 BRANCH (`c10/branch`) | `--beats br` | routes 02..14 | 2 files | done on 2026-10-10: 00, 06, 07, 10 equal; 01..05, 08 the loader read gate only; 09, 11..14 differ |
| 11 | C10 OPTIONS (`c10/options`) | `--beats opt` | route 08 | 3 files | done on 2026-10-10: the memory-card record (zero on the fork) and a few loader rows |
| 12 | level groups `a01`, `a00`, `a01r`, `a02`, `a04`, `a22`, `a01u`, `a06`, `a06b`, `a01v`, `a22b`, `a04b`, `a13`, `a19`, `a13b`, `a13c`, `a13d`, `a19b`, `a19c`, `a19d`, `a15`, `a15b`, `a19e`, `a03` (`s87/route_a*`) | `--beats <group>`, in this order | beat 15, then each previous group | 1 to 16 files per group | partly done on 2026-10-10: 59 of 159 beats (`a01`, `a00`, `a01r`, `a02`, 7 of `a04`); blocked at `a04_03` on the fork's own playthrough ("Route groups re-recorded in parallel lanes") |
| 13 | census (`s87/census`) | `route_census.py run --segments all --pass F`, then `report --passes F` | slot 01, the route | 39 files, `FIRST_LEVEL_CENSUS.md` | thousands of v1 breakpoints through `ForkV1Debug`: smoke-test one beat first (`--segments 00`); the function sets should equal the v2.6.3 census. Smoke (`--segments 00 --pass FORKSMOKE`) re-run on 2026-10-10 from the canonical chain: 518 functions (the v2.6.3 pass A set), 261 of 261 rows equal the recorded beat, end digests equal; the first attempt lost its DebugServer connection at frame 76 and the tool retried (`runs/FORKSMOKE/_failed/`) |
| 14 | load wait (`s87/loadwait`) | `load_wait_probe.py run --beats 01,03 --mode boundary` (and `breakpoints`) | routes 01, 03 | 2 files | the fork's loader runs longer. Beat 01 breakpoints mode re-run on 2026-10-10 from the canonical chain: 517 of 517 rows equal the recorded beat; 24 dispatches, 17 polls, 3 reads, 1 request, as on v2.6.3 |
| 15 | sound requests (`sfx_probe`) | `sfx_request_probe.py run` | the census route | 1 file | |
| 16 | GS conformance (`b16/gscap`) | `gs_conformance.py capture` | `phase/04` (the 2026-10-09 state; the tests' packets do not depend on the game frame) | 2 files | done on 2026-10-10 for all eight sets into `build/fork_refs/pixels/b16` (`fork_pixel_refs.py gscap`): 906 of 906 tests bit-exact |
| 17 | EE float vectors (`startup-reference/ee_float`) | `ee_float/battery.py`, `blockcheck.py`, `vusig.py` | `phase/04` | 2 files | |
| 18 | video compare (`video_compare/<rec>/ps2`) | `video_compare.py ps2 <rec> --out build/fork_refs/video_compare/<rec> --stride 4 --audio [--fork-title-delay N]` | cold boot | 2 files | pick the title delay whose phase after the first load equals the v2.6.3 run's (rows record both) |
| 19 | startup probes' outputs (`startup-reference/panel`, `status-hub`, `elevator`, `roger-encounter`, `playable_ee.bin`, `opening_ee.bin`, `*_poll.json`, `cutscene_skip`) | not ported; equivalents: `phase/slot02..04` and the status / Roger states' `eeMemory.bin`, route beats 03 and 04 | the phase chain | 14 + 11 + 3 + 3 + 72 + 43 files | the port switch decides per file; the probes stay v2.6.3-only |
| 20 | repack proofs | `python -m tools.repack proof-title` / `proof-gameplay` | cold boot of a rebuilt image | none | fork by default (`ForkColdDisc`) |
