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
| MCP bridge | `pcsx2-fork/extermination/mcp-server/pcsx2_agent_mcp.py` (55 tools). **Not registered yet.** The registered `pcsx2` server is still the legacy Node bridge in `../PCSX2-MCP`. Switching is the user's decision (command in `EXTERMINATION.md` section 7). |
| Legacy emulator | `/Applications/PCSX2.app` and `build/startup-reference/PCSX2.app`: v2.6.3, x86_64, with the old DebugServer (TCP 21512) and PINE. They are still in place and are still the default for every decomp tool. The lead retires them only after the user confirms. |
| Save states | The fork writes version `0x9A59` and refuses v2.6.x states (`0x9A55`) with an explicit error. Those are the user's slots 01 to 15 and the `build/startup-reference` states, and they load only in the legacy app. The compat stage saved fork states (title menu, New Game, demo_hill tick 1600) to the ignored `build/pcsx2-fork/acceptance/states/`. |
| Compat with the game | Receipts are in the ignored `build/pcsx2-fork/acceptance/reports/`. The fork cold-boots the game, passes the intro and title, and replays the demo_hill route (4,678 ticks). Against the legacy run, per-tick game state matched once one load segment was aligned (273 against 268 ticks), except 5 rows of the loader busy byte. The displayed GS fields differ in most compared frames, and these receipts do not establish the cause. Until that is resolved, do not treat v2.6.3 field captures as frame-exact references for the fork. |
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

`tools/video_compare/ps2.py`, `tools/route_capture.py` and
`tools/route_census.py` still use the legacy app and its states. Moving them
needs fork-saved states for their starting points. Before that, it is worth
using the fork's own commands (frame store, watch tables, `run {until}`, audio
tap), which replace their save-state-per-frame and breakpoint-per-tick
methods (`EXTERMINATION.md` section 15).
