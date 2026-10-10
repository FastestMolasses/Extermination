# PCSX2 live-testing cheats — memory addresses (SCUS-97112)

Decoded + live-verified addresses for editing player state while comparing the
native port against PCSX2. All addresses are EE RAM; poke via the MCP
(`pcsx2_write_memory`) or raw TCP
(`printf '{"cmd":"write_memory","address":0x810CB4,"data":"7800"}\n' | nc 127.0.0.1 21512`
— data is little-endian hex bytes). Sources: FINDINGS s18 (inventory),
s28b (battery), s25 (status screen), s22/s23.

## Vital stats (player actor @ 0x008102B0)
| What | Address | Type | Notes |
|---|---|---|---|
| Health | `0x008104D0` | float | 75.0 = "075/100"; max is 100, or 60 when flag `0x008104E4` set |
| Infection | `0x008104D8` | float | 60.0 = "60%"; 100 shows INFECTED |
| Display copies | `0x00810858/5C` | — | status screen counts toward the real value ±1/frame; poke the floats, the display follows |

## Ammo (SPR4)
| What | Address | Type | Notes |
|---|---|---|---|
| Magazine | `0x00810C62` | u8 | max 30 |
| Mag-equivalent counter | `0x00810C63` | u8 | cap 98; folds to reserve 30:1 |
| Reserve | `0x00810CB4` | s16 | TOTAL pool incl. mag (reload sets mag=min(30,reserve), doesn't subtract; firing decrements both) |

## Items
- Count array: `u8[] @ 0x00810C64`, indexed by **item type id** (catalog in
  FINDINGS s42 message bank group 3): e.g. office ammo boxes 0x0B/0x0C/0x0D →
  `0x810C6F/70/71`; SPR4 mag pack 0x10 → `0x810C74`.
- Secondary-weapon rows: ids `0x810CA4/0x810CA6`, counts `0x810CA8/AA/AC/AE/B0`.

## Battery
| What | Address | Type | Notes |
|---|---|---|---|
| Current | `0x00810CB2` | u16 | HALF-units: displayed value = stored >> 1 ("04/06" = 8/12) |
| Max | `0x00810CB7` | u8 | half-units (0x0C = "06", 0x24 = "18") |
| Overlay gate | `0x00810C7F` | u8 | nonzero enables the battery readout |

## Position / world
- Teleport: write the player position **qwords at `0x00810350` and `0x00810360`**
  (x,y,z floats; both copies) — verified working for repositioning. The
  scratchpad `0x70003B40` mirror is authoritative-read but writes don't stick.
- Area/sub bytes: `0x00810700/701`. Door unlock bits: `0x00810841[area] >> door_id`.
- Transition request: write bytes `0x008106B5..B8` (area, sub|0xFF, entry, mode
  1=inter-area/2=room) to force a transition (s22 lifecycle).

## Frame-exact original sessions (s87; the fork by default since 2026-10-09)

`tools/pcsx2_session.py` steps the original exactly one main-loop frame at a
time. `step(n, buttons=..., lx=, ly=)` advances exactly `n` frames: the
counter `0x70003B64` is checked to advance by one per step. `snapshot(dir)`
writes `state.p2s`, `eeMemory.bin`, `gs.bin`, `scratchpad.bin`, the picture
`original.png` and `snapshot.json`, which records the main-loop counter, the
game's vsync counter, the frame index D_00810E80 and the field D_00810E88.
Injected pad state reaches `0x810E70` two frames after it is set. State 03
has movement locked; state 04 walks. All output belongs in gitignored
`build/`.

**Default: the agent-debug fork** (the user's decision of 2026-10-09 to
retire v2.6.3; `docs/PCSX2_FORK.md`). `ForkSession` does the following:

- It loads a fork-saved state, resolved through
  `build/startup-reference/fork-states/manifest.json`. A state argument can
  be `04`, `slot04`, `14_roger_encounter`, `phase/04`, or the old v2.6.3
  path.
- It runs hidden on scratch data, takes the run lock itself and starts
  exactly on the saved frame.
- One frame step is one `run {until: {ticks: 1}}`, about 80 frames a second
  with route_capture's per-frame sample.
- `original.png` is the displayed field (`gs_field`).

```sh
# macOS (arm64 host; the fork runs x86_64 under Rosetta), decomp .venv
.venv/bin/python tools/pcsx2_session.py 04 --frames 30 --snapshot build/<task>/walk
```

The command line prints the state it used and the phase after the steps.
`--generation auto` (the default) takes the phase-locked state when the
manifest has one, else the base state.

**Two generations of fork states.**

- `base`: slots 01 to 04, the 08 -> 12 -> 14 status chain, slot 15 and
  route beats 00..15. They sit at the same game points as the v2.6.3
  states, matched by game state, not by counter.
- `phase` (`phase/...` keys): the same game points with the v2.6.3 frame
  index and field, so captures land on the same drawing buffer and half
  line. A phase-locked capture also replays each v2.6.3 beat's lead-in, so
  its rows sit on the v2.6.3 rows' ticks.

Details: `docs/PCSX2_FORK.md`, "Fork states" and "Phase-locked states".

**The legacy app** (v2.6.3, until the lead retires it): `--emulator legacy`
(or `EXTERMINATION_PCSX2=legacy`) launches `build/startup-reference/PCSX2.app`
from the user's slots or a `build/s87/...` snapshot, never modified (hashed
before and after). It parks on a breakpoint at the main-loop top `0x001AAF28`.
It runs hidden (`open -g -j`, kept hidden through System Events); `--visible`
shows it. Snapshots go through a free slot >= 16 that is moved into the
output folder. A legacy session loses 1 to 12 frames after its load. v2.6.3
states load only there; fork states load only in the fork.

## Gotchas
- Pause the VM before multi-byte pokes (`pcsx2_pause` / resume).
- The status screen reads live values — open it (Triangle) to watch edits land.
- Exec breakpoints AND memchecks DO fire on the current fast x86_64 build
  (verified s66 — bp at a jal reads live args; write/read memchecks pause at the
  offending PC). The old "don't fire" note applied to an earlier build. Remove
  them promptly (perf decay) and restart PCSX2 if the IOP audio wedges;
  `watch_change` (raw TCP) remains the low-impact option.
- Two control channels: the DebugServer JSON socket above (TCP 21512) and PINE.
  On macOS PINE is a Unix socket, while the MCP companion defaults to TCP 28011;
  check which one a tool expects before assuming PINE is down. (Moved from the
  pre-s87 HANDOFF archive, 2026-09-27.)
- No savestates without Pine: snapshot/restore RAM regions instead (player
  0x8102B0+0x320, camera 0x8101D0+0xE0, globals 0x810600+0x300, inventory
  0x810C00+0x100, fade 0x28A9A0+0x10, task 0x28A750+0x10) — and only restore
  while paused at a NEUTRAL PC (main loop), never inside the chain under test.
- Force a transition without a door: write B5..B8, then hand-start the fade
  (func_001AEDE0(4,0) is pure data: u16 0x28A9A0=3, u8 0x28A9A2=0, u8
  0x28A9A3=3, u16 0x28A9A6=4); the consumer commits at hold-black.
- Pad injection: `{"cmd":"pad_press","buttons":16384,"frames":12}` (Cross=0x4000,
  Circle=0x2000, Square=0x8000, Triangle=0x1000, R1=0x0800, L3=0x0002,
  Start=0x0008; sticks via pad_set lx/ly 0..255, 128 center).
