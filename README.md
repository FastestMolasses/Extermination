# Extermination (PS2): matching decompilation

A matching decompilation of the PlayStation 2 game Extermination (USA, `SCUS-97112`),
with the native port in the sibling repo `../extermination-port`. You supply your own
legal copy of the game. This repository holds only original code, build scripts and
tooling; the disc, its code and its assets are never committed.

Start with `CLAUDE.md` (the rules and the toolkit) and `docs/HANDOFF.md` (current
state). `tools/verify_all.py` is the gate that proves the build is byte-identical.

## Recording your own gameplay

`tools/record_original_play.py` records you playing the original game. The take then
becomes a test for the port: the port replays your exact pad input and every tick is
compared with what the original did. Real play reaches paths, fights and menus that
scripted routes miss.

Everything below runs natively on the Mac. The game window is the debugging fork of
PCSX2 (`../pcsx2-fork`, x86_64 under Rosetta). Use this repository's `.venv` Python.

### Record

Start a run from the title screen:

```bash
.venv/bin/python tools/record_original_play.py play run1
```

Continue from where an earlier take ended (the port stitches them into one run):

```bash
.venv/bin/python tools/record_original_play.py play run2 --after run1
```

Play in the window that opens, with the keyboard or a controller (a controller is
not tested yet). **Press Enter in the terminal to end the take.** Do not close the
game window: that loses the end state. Keep each take under an hour or so; a
long playthrough goes in several takes.

If a check is using the emulator, `play` waits for it and says so.

### What happens after a take

The take is replayed hidden from the same start, at about 3.5 times normal speed, and
the whole machine is compared at the last vsync. `exact` means the pad input alone
reproduces your take, so it can be used as a test. Ctrl+C skips this check; you can
run it later:

```bash
.venv/bin/python tools/record_original_play.py verify run1
```

```bash
.venv/bin/python tools/record_original_play.py list
```

`passing difference, gone by the end` is normal. About once per replay a held key
reads as released for one tick, and it usually changes nothing. If it does change
play, the check retries, up to three times.

### Memory cards

Each take has its own formatted card, carried from take to take, so saving in a take
really saves. `run1` and `test1` predate this: they played on an unformatted card and
their saves failed in the original too. The port reproduces that failure when it
plays them. Your own memory cards and PCSX2 settings are never touched.

### Watch it in the port

From `../extermination-port`, after `make`:

```bash
python3 tools/watch_takes.py
```

See that repository's README, "Watching your own recorded takes".

### The files

A take is in `build/human_play/<take>/`, derived from your disc and never committed.
**Never delete these.** `input.log` is the pad per vsync, `take.rec` the take as a port
recording (one row per game tick), `start.json`, `end.json` and `end.p2s` the two ends,
`card_start.ps2` and `card_end.ps2` the card, and `verify.json` the check's result.
`trace <take>` rebuilds `take.rec`; `run1` and `test1` need it because they predate the
columns a take now records for itself.

Details: `docs/PCSX2_FORK.md`, "A person's play recorded on the original".
