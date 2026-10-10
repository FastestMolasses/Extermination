#!/usr/bin/env python3
"""fork_states.py - regenerate the tools' save states under the agent-debug PCSX2 fork.

The v2.6.3 save states (version 0x9A55: the user's slots in
build/startup-reference/portable-data/sstates and the route snapshots in
build/s87/route) do not load in the agent-debug fork (0x9A59,
docs/PCSX2_FORK.md).  This tool rebuilds the ones the project's tools use,
from a COLD BOOT in the fork, at the same game point the old state marks.
Points are matched by game state (task records, fade and letterbox blocks,
scratchpad selectors, the player actor, camera, UI, area/sub/entry, story and
inventory bytes, the AREA11 owner nodes), never by the main-loop counter,
which differs between the builds.  Output, all git-ignored and disc-derived:

  build/startup-reference/fork-states/<key>/        state.p2s, eeMemory.bin,
        scratchpad.bin, gs.bin, original.png (gs_field), snapshot.json,
        fork_state.json (how it was reached, its score against the old state)
  build/startup-reference/fork-states/beats/s87/route/<beat>/   route_capture
        beats 00..15 run closed loop in fork mode (same layout as build/s87/route)
  build/startup-reference/fork-states/manifest.json  old name -> new file, how,
        fingerprints (old, new, differences), fork build

Commands (decomp .venv python, from the repo root; macOS arm64 host, the fork
runs x86_64 under Rosetta; every run is hidden, on scratch outside ~/Documents,
under the run lock build/.pcsx2.lock, and leaves no emulator running):

  boot      cold boot -> title (slot01_title) -> NEW GAME -> the AREA11 opening
            (slot02_opening) -> its fade-in (slot03_fade_in) -> first control
            (slot04_first_control).  Pass 1 records the per-tick scores, pass 2
            replays from slot01_title and saves the best-matching ticks.
  route --generation base
            route_capture beats 00..15 (`--beats` to choose) in fork mode, from
            slot04_first_control, each beat from the previous fork snapshot,
            no lead-in (the first regeneration's layout fork-states/beats/).
  status    from route 02_elevator_refusal: the panel's BATTERY prompt
            (slot08_battery_prompt), Circle to the status root
            (slot12_status_root), Circle to the status hub (slot14_status_hub),
            as build/startup-reference/panel_root_probe.py and
            status_hub_probe.py did on the legacy app.
  roger     from slot03_fade_in: roger_encounter_probe.py's teleport into the
            Roger trigger, 50 ticks after the bank-96 player clip starts
            (slot15_roger_encounter).
  boot --phase-lock
            the phase-locked generation (phase/slot02..04): from slot01_title,
            scan (title-driver delay, movie skip) combinations until the
            opening gives 02, 03 and 04 the frame index D_00810E80 and field
            D_00810E88 of the v2.6.3 session's FIRST LOOP TOP after loading
            the slot (route_capture.legacy_slot_loop_top_phase: every user
            slot was saved at the vsync wait, so that is the stored phase
            flipped) at the same game point, rand() state and player clock
            included; save them under fork-states/phase/.  Deterministic:
            inputs land on fixed ticks.  `--no-skip --delays 0` is the boot
            that gives it (the movie played, no delay).
  status, roger (phase generation by default; --generation base for the
            first regeneration's layout)
            the same chains from the phase-locked sources (route 02 of the
            canonical chain build/fork_refs/s87/route; phase/slot03), each
            save point one tick later when needed to land on the loop-top
            phase of the v2.6.3 slot.
  route     (phase generation by default) the main route in route_capture's
            default layout: build/fork_refs/s87/route/, states linked into
            fork-states/phase/beats/ (route_lanes.py records it in lanes).
  manifest  rebuild manifest.json from the folders (fingerprints old vs new;
            both generations; written atomically).
  verify    load every manifest state in the fork: the live machine equals the
            file, two ticks advance, the fingerprint matches the old capture's
            (fork-states/verify.json).
  compare   offline: every fork route trace (phase generation by default)
            against its v2.6.3 trace, row by row (fork-states/compare.json).
  rerun B   re-run route beat B from its fork source (phase generation by
            default: the canonical chain's source with its recorded lead-in;
            a user-slot source starts from route_capture.slot_start) into
            build/pcsx2-fork/rerun/<B>/ and compare its trace with the fork
            chain's trace and with the v2.6.3 trace (build/s87/route/<B>).

The v2.6.3 slots themselves are needed only while legacy_refs.json lacks their
spans: every target (phase, counter, save PC and the compared spans) is cached
in build/fork_refs/legacy_refs.json (route_capture.py legacy-refs fills the
phases, `fork_states.py manifest` the spans), so the tool keeps working after
the slots are retired.

No original code or data is embedded here; it names addresses only.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import math
import shutil
import struct
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from parse_pcsx2_state import extract_zstd_entry  # noqa: E402
from pcsx2_session import (ForkSession, FORK_REPO, FORK_STATES, FORK_MANIFEST, SERIAL,  # noqa: E402
                           SSTATES, PAD, FRAME_COUNTER, FRAME_INDEX, FIELD, fork_state, read_phase)
import route_capture as rc  # noqa: E402

PLAYER = 0x8102B0
SPAD = 0x70000000
RERUN = ROOT / "build/pcsx2-fork/rerun"

# Spans compared between an old (v2.6.3) state and a fork candidate.  Counters
# and clocks that only measure elapsed time are left out on purpose.
SPANS = [
    ("task", 0x28A750, 0x60),           # task slots 0..2 (+4 function, +8.. state bytes)
    ("fade", 0x28A9A0, 0x10),           # transition/fade machine
    ("screen", 0x28A8D0, 0x20),         # screen-fade / letterbox block
    ("spad", 0x70003B8C, 0x8),          # 3B8C..3B93 selectors
    ("player", PLAYER, 0x320),          # player actor
    ("cam", 0x8101E0, 0x30),            # camera mode, eye, target
    ("ui", 0x810130, 0x10),             # status UI object
    ("req", 0x8106B0, 0x10),            # transition request
    ("area", 0x810700, 0x4),            # area, sub, entry
    ("story", 0x810790, 0x4),
    ("d2", 0x8107D8, 0x40),             # Roger progress .. 810813
    ("prog", 0x810840, 0x10),           # area power bits (0x81084C)
    ("inv", 0x810C64, 0x20),            # item counts (0x810C7F battery)
    ("charge", 0x810CB0, 0x8),          # battery charge (half units), max
    ("battery_g0_0", 0x7A5640, 0x10),   # AREA11 owner nodes (route_capture OWNERS)
    ("panel_r18", 0x7AA590, 0x10),
    ("elevator_r19", 0x7AA880, 0x10),
    ("roger_r8", 0x7A8830, 0x220),
    # The update count (2026-10-10): the game's rand() state and the player's
    # clock move on every world update.  Without them a loop-top state one
    # update away from the target scored 0 (the phase generation of
    # 2026-10-09 did; docs/PCSX2_FORK.md "Correction").
    ("rand", 0x2426C8, 0x4),
    ("player_clock", PLAYER + 0x3C, 0x4),
]
# Inside the player actor, bytes that are clocks rather than state: masked in
# the "player" span and scored once, as the "player_clock" span above.
PLAYER_CLOCKS = [(0x3C, 4)]


# ---------------------------------------------------------------------------
# reading spans: live (one PINE request) or from a state / snapshot folder

def _pine_body(spans) -> bytes:
    return b"".join(struct.pack("<BI", 2, a + i) for _n, a, n in spans for i in range(0, n, 4))


_BODY = _pine_body(SPANS)


def live_spans(s) -> dict[str, bytes]:
    data = s.pine.request(_BODY)
    out, off = {}, 0
    for name, _a, n in SPANS:
        out[name] = data[off:off + n]
        off += n
    return out


def spans_from_images(ee: bytes, sp: bytes) -> dict[str, bytes]:
    out = {}
    for name, a, n in SPANS:
        out[name] = sp[a - SPAD:a - SPAD + n] if a >= SPAD else ee[a:a + n]
    return out


def images_of(path: Path) -> tuple[bytes, bytes]:
    """(eeMemory, scratchpad) of a .p2s, or of a snapshot folder's extracted bins."""
    path = Path(path)
    folder = path if path.is_dir() else path.parent
    if (folder / "eeMemory.bin").exists() and (folder / "scratchpad.bin").exists() and \
            (path.is_dir() or path.name == "state.p2s"):
        return (folder / "eeMemory.bin").read_bytes(), (folder / "scratchpad.bin").read_bytes()
    return extract_zstd_entry(path, "eeMemory.bin"), extract_zstd_entry(path, "Scratchpad.bin")


def counter_of(sp: bytes) -> int:
    return struct.unpack_from("<I", sp, 0x3B64)[0]


def _mask_clocks(name: str, b: bytes) -> bytes:
    if name != "player":
        return b
    b = bytearray(b)
    for off, n in PLAYER_CLOCKS:
        b[off:off + n] = bytes(n)
    return bytes(b)


def score(old: dict[str, bytes], new: dict[str, bytes], names=None) -> tuple[int, dict[str, int]]:
    """Differing bytes per span (clocks masked); total and per span."""
    per = {}
    for name, _a, _n in SPANS:
        if names is not None and name not in names:
            continue
        a, b = _mask_clocks(name, old[name]), _mask_clocks(name, new[name])
        d = sum(x != y for x, y in zip(a, b))
        if d:
            per[name] = d
    return sum(per.values()), per


def fingerprint(sp: dict[str, bytes]) -> dict:
    """Readable game-state fields (route_capture identify's set, plus the
    fade, letterbox and task records)."""
    p = sp["player"]
    f = lambda b, o: round(struct.unpack_from("<f", b, o)[0], 3)  # noqa: E731
    task = sp["task"]
    return {
        "area": sp["area"][:3].hex(),
        "pos": [f(p, 0xA0), f(p, 0xA4), f(p, 0xA8)],
        "yaw": f(p, 0xC4), "action": p[0x1F0], "b2F3": p[0x2F3],
        "clip": struct.unpack_from("<h", p, 0x20C)[0],
        "spad3B8C": sp["spad"].hex(), "ui": sp["ui"][:8].hex(), "req": sp["req"][:10].hex(),
        "fade": sp["fade"][:8].hex(), "screen": sp["screen"][:4].hex(),
        "task0": f"{struct.unpack_from('<I', task, 4)[0]:#x}:{task[8:12].hex()}",
        "power": hex(sp["prog"][0xC]), "story790": sp["story"].hex(),
        "8107D8": hex(sp["d2"][0]), "810813": hex(sp["d2"][0x3B]),
        "item1B": sp["inv"][0x1B], "charge": struct.unpack_from("<H", sp["charge"], 2)[0],
        "battery_owner_state": sp["battery_g0_0"][4], "panel": sp["panel_r18"][:12].hex(),
        "elevator": sp["elevator_r19"][:12].hex(),
        "roger_script": hex(struct.unpack_from("<I", sp["roger_r8"], 0x1F8)[0]),
    }


def fp_diff(old: dict, new: dict, tol: float = 0.5) -> list[str]:
    """Fields that differ.  Positions and yaw within `tol` (world units /
    radians/10) count as equal; closed-loop beats end near, not on, a point."""
    out = []
    for k, v in old.items():
        w = new.get(k)
        if k == "pos":
            if max(abs(a - b) for a, b in zip(v, w)) > tol:
                out.append(f"pos {v} -> {w}")
        elif k == "yaw":
            d = abs((v - w + math.pi) % (2 * math.pi) - math.pi)
            if d > tol / 10:
                out.append(f"yaw {v} -> {w}")
        elif v != w:
            out.append(f"{k} {v} -> {w}")
    return out


# ---------------------------------------------------------------------------
# saving

def fork_head() -> str:
    try:
        return subprocess.run(["git", "-C", str(FORK_REPO), "rev-parse", "HEAD"], capture_output=True,
                              text=True, check=True).stdout.strip()
    except (OSError, subprocess.CalledProcessError):
        return "unknown"


def save_point(s: ForkSession, key: str, old: Path | None, how: str, source: str,
               extra: dict | None = None) -> dict:
    out = FORK_STATES / key
    if out.exists():
        shutil.rmtree(out)                          # absolute path under fork-states
    info = s.snapshot(out)
    meta = {"key": key, "how": how, "source": source, "frames_stepped": s.frames_stepped,
            "old": str(old.relative_to(ROOT)) if old else None, **(extra or {})}
    if old is not None:
        o = old_spans(old)
        n = spans_from_images(*images_of(out))
        total, per = score(o, n)
        meta.update(score=total, score_spans=per)
    (out / "fork_state.json").write_text(json.dumps(meta, indent=1) + "\n")
    print(f"saved {key}: counter {info['main_loop_counter']}"
          + (f", score {meta.get('score')} {meta.get('score_spans')}" if old else ""), flush=True)
    return meta


def slot_file(slot: str) -> Path:
    return SSTATES / f"{SERIAL}.{slot}.p2s"


def slot_spans(slot: str) -> dict[str, bytes]:
    """The compared spans of v2.6.3 user slot `slot`: from the slot file while
    it exists (and cached in build/fork_refs/legacy_refs.json "slot_spans"),
    else from the cache, so the targets survive the slots' retirement."""
    f = slot_file(slot)
    if f.exists():
        ee, sp = images_of(f)
        out = spans_from_images(ee, sp)
        rc._legacy_cache_put("slot_spans", slot, {"counter": counter_of(sp),
                                                  **{k: v.hex() for k, v in out.items()}})
        return out
    c = rc._legacy_cache().get("slot_spans", {}).get(slot)
    if c is None or any(name not in c for name, _a, _n in SPANS):
        raise FileNotFoundError(f"{f} is gone and legacy_refs.json has no spans for slot {slot}")
    return {name: bytes.fromhex(c[name]) for name, _a, _n in SPANS}


def slot_counter(slot: str) -> int | None:
    f = slot_file(slot)
    if f.exists():
        return counter_of(images_of(f)[1])
    return (rc._legacy_cache().get("slot_spans", {}).get(slot) or {}).get("counter")


def _slot_of(old: Path | None) -> str | None:
    """The user slot id of a v2.6.3 slot path (present or retired), else None."""
    if old is not None and Path(old).parent == SSTATES and Path(old).name.startswith(SERIAL):
        return Path(old).name.split(".")[-2]
    return None


def old_spans(old: Path) -> dict[str, bytes]:
    slot = _slot_of(old)
    return slot_spans(slot) if slot else spans_from_images(*images_of(old))


def loop_top_phase(slot: str) -> dict:
    """The phase a v2.6.3 session from user slot `slot` had at its first loop
    top (route_capture.legacy_slot_loop_top_phase): the target of every
    phase-generation slot state."""
    return rc.legacy_slot_loop_top_phase(slot)


# ---------------------------------------------------------------------------
# boot: cold boot -> title -> NEW GAME -> opening -> first control

TITLE_SPANS = ("task", "fade", "screen", "spad", "area", "ui")
BOOT_TARGETS = [("slot02_opening", "02"), ("slot03_fade_in", "03"), ("slot04_first_control", "04")]


def title_inputs(s: ForkSession, n: int, presses: list[int]) -> None:
    """route_census's title driver: Cross at n in 5/125/245/365 once the fade is
    clear (at most 4 presses), released at n % 120 == 9."""
    if n in (5, 125, 245, 365) and len(presses) < 4 and s.read(0x28A9A0, 4) == b"\0\0\0\0":
        s.pad(PAD["CROSS"])
        presses.append(n)
    elif n % 120 == 9:
        s.pad(0)


def boot_title(log: Path) -> None:
    old = slot_spans("01")
    # BIOS + game start-up before the first main-loop top, and the intro movie
    # inside one tick, both need far more than the default 30 s
    with ForkSession(None, log_dir=log / "title", boundary_timeout=900.0) as s:
        best = (1 << 30, -1)
        for _ in range(20000):
            sp = live_spans(s)
            total, _per = score(old, sp, TITLE_SPANS)
            if total < best[0]:
                best = (total, s.frames_stepped)
            if total == 0:
                save_point(s, "slot01_title", slot_file("01"),
                           "cold boot (fixed RTC 2026-01-01 00:00:00, fastboot, no input) to the first "
                           "main-loop top whose task slots, fade, letterbox, selectors, area and UI "
                           "bytes equal slot 01's (the title fading in)", "cold boot",
                           {"ticks_from_boot": s.frames_stepped})
                return
            s.step(1)
        raise RuntimeError(f"title not matched; best {best}")


MOVIE_DRIVER = 0x00203350      # func_00203350, the blocking movie driver (docs/STARTUP.md)


def opening_pass(log: Path, save_at: dict[str, int] | None, press_delay: int = 0,
                 prefix: str = "", skip_at: int | None = None) -> dict:
    """From slot01_title: the title inputs, NEW GAME, the movie, the AREA11
    load and opening, to first control + 200 ticks.  save_at None = pass 1
    (scores only); else save each key at its tick.  press_delay d holds the
    title driver back d ticks (its Cross presses land d ticks later), which
    moves the NEW GAME commit, and with it the frame-index and field phase of
    everything after it; prefix "phase/" saves into the phase-locked
    generation.  Pass 1 also returns, per key, the phase (frame index, field)
    at the best tick and the commit's counter, game vsync and phase.

    skip_at k (None = play the New Game movie, as the base generation did):
    a stop probe at the movie driver's entry halts inside the tick that plays
    the movie; the run goes on k vsyncs, then START is held until the tick
    ends, so the skip lands on a fixed emulated vsync (movie start + k) and
    the movie's end, the AREA11 load and the phase after it are the same in
    every run.  This is the lever for the phase: the played movie ends on a
    fixed vsync grid, while k moves the end vsync by vsync."""
    olds = {key: slot_spans(slot) for key, slot in BOOT_TARGETS}
    best = {key: (1 << 30, -1, None) for key, _ in BOOT_TARGETS}
    best_phase: dict = {}
    marks: dict = {"press_delay": press_delay}
    sub = ("save" if save_at else "scan") + (f"_d{press_delay}" if press_delay or prefix else "")
    with ForkSession(fork_state("slot01_title", "base"), log_dir=log / sub,
                     boundary_timeout=900.0) as s:   # the NEW GAME movie plays inside one tick
        sampler = rc.Sampler(s)
        presses: list[int] = []
        seen_cut = False
        control_at = None
        n = 0
        if skip_at is not None:
            s._client.call("probe_add", pc=MOVIE_DRIVER, action="stop", label="fork_states movie skip")
            marks["skip_at"] = skip_at
            marks["movies"] = []

            def step_one() -> None:
                # one main-loop frame, servicing the movie probe on the way
                ran = False
                while True:
                    r = s._client.call("run", until={"ticks": 1}, timeout_s=900, timeout=960)
                    stop = r["stop"]
                    if stop.get("reason") == "probe" and int(stop["pc"], 16) == MOVIE_DRIVER:
                        mv = {"tick": s.frames_stepped, "vsync_at_driver": stop.get("vsync"),
                              "game": read_phase(s)}
                        if skip_at:
                            s._client.call("run", until={"vsyncs": skip_at}, timeout_s=120, timeout=180)
                        mv["skip_vsync"] = s._client.call("state").get("vsync")
                        s.debug.call({"cmd": "pad_set", "buttons": PAD["START"], "lx": 0x7F, "ly": 0x7F,
                                      "rx": 0x7F, "ry": 0x7F})
                        marks["movies"].append(mv)
                        ran = True
                        continue
                    if stop.get("reason") != "tick":
                        raise RuntimeError(f"unexpected stop {stop}")
                    if stop.get("ee_cycle") == s._ee_cycle and not ran:
                        s._ee_cycle = stop.get("ee_cycle")
                        ran = True               # the zero-advance stop after a load
                        continue
                    s._ee_cycle = stop.get("ee_cycle")
                    break
                if marks["movies"] and marks["movies"][-1].get("released") is None:
                    s.pad(0)                     # START off at the first loop top after the skip
                    marks["movies"][-1]["released"] = s.frames_stepped + 1
                s.frames_stepped += 1
        else:
            step_one = None
        while True:
            row = rc.decode(sampler.raw())
            sp = live_spans(s)
            if marks.get("new_game") is None and sp["area"][0] != 0:
                marks["new_game"] = s.frames_stepped
                marks["commit"] = read_phase(s)
                s.pad(0)
            if marks.get("new_game") is None:
                if n >= press_delay:
                    title_inputs(s, n - press_delay, presses)
            else:
                for key, _slot in BOOT_TARGETS:
                    total, per = score(olds[key], sp)
                    if total < best[key][0]:
                        best[key] = (total, s.frames_stepped, per)
                        best_phase[key] = read_phase(s)
                if rc.cutscene(row):
                    seen_cut = True
                if seen_cut and control_at is None and rc.in_control(row):
                    control_at = s.frames_stepped
                    marks["first_control"] = control_at
            if save_at:
                for key, tick in save_at.items():
                    if tick == s.frames_stepped:
                        slot = dict(BOOT_TARGETS)[key]
                        movie = ("the New Game movie played" if skip_at is None else
                                 f"the New Game movie skipped with START {skip_at} vsync(s) after its "
                                 "driver starts")
                        how = HOW[key] + (f"; title driver held back {press_delay} tick(s), {movie}, so the "
                                          "frame index and field equal the v2.6.3 session's first loop top "
                                          "after loading the slot (phase lock)" if prefix else "")
                        save_point(s, prefix + key, slot_file(slot), how, "slot01_title",
                                   {"ticks_from_slot01": tick, "marks": dict(marks),
                                    "press_delay": press_delay, "skip_at": skip_at,
                                    "phase": read_phase(s)})
                if s.frames_stepped >= max(save_at.values()):
                    break
            if control_at is not None and s.frames_stepped >= control_at + 200:
                break
            if s.frames_stepped > 12000:
                raise TimeoutError("the opening did not reach first control")
            if step_one is not None:
                step_one()
            else:
                s.step(1)
            n += 1
        marks["presses"] = presses
    return {"best": best, "marks": marks, "phase": best_phase}


HOW = {
    "slot02_opening": "from slot01_title: route_census's title inputs (Cross once the fade is clear), "
                      "NEW GAME, the movie, the AREA11 load and opening with no input; the tick whose "
                      "game state best matches slot 02 (the opening cinematic)",
    "slot03_fade_in": "as slot02_opening; the tick that best matches slot 03 (the fade-in after the "
                      "opening, movement still locked)",
    "slot04_first_control": "as slot02_opening; the tick that best matches slot 04 (first control, "
                            "the fade done)",
}


def legacy_phase(path: Path) -> dict:
    """Frame index, field, game vsync and main-loop counter of a v2.6.3 state
    (or a beat folder's eeMemory.bin + scratchpad.bin), read offline.  A user
    slot that is gone (retired with the v2.6.3 app) comes from route_capture's
    cache build/fork_refs/legacy_refs.json."""
    path = Path(path)
    if not path.exists() and path.parent == SSTATES:
        ph = rc.legacy_slot_phase(path.name.split(".")[-2])
        if ph is None:
            raise FileNotFoundError(f"{path} and no cached phase (route_capture.py legacy-refs)")
        return dict(ph, vsync=None)
    ee, sp = images_of(path)
    return {"frame_index": ee[FRAME_INDEX], "field": ee[FIELD],
            "vsync": struct.unpack_from("<I", ee, 0x810E90)[0], "counter": counter_of(sp)}


def same_phase(a: dict, b: dict) -> bool:
    return (a["frame_index"], a["field"]) == (b["frame_index"], b["field"])


def cmd_phase_lock(a) -> None:
    """Phase-locked boot: find the smallest title-driver delay d (0..max) whose
    opening gives slots 02, 03 and 04 the frame index and field of the v2.6.3
    states at the same game points, then save them as phase/slot0N_*.  The
    scan is deterministic: from slot01_title every input lands on a fixed
    tick, so the same d gives the same commit vsync and phase in every run."""
    log = FORK_STATES / "logs/boot_phase"
    targets = {key: loop_top_phase(slot) for key, slot in BOOT_TARGETS}
    tried = []
    delays = [a.press_delay] if a.press_delay is not None else list(range(0, a.max_delay + 1))
    skips = ([None] if a.no_skip else
             [a.skip_at] if a.skip_at is not None else
             [int(x) for x in a.skips.split(",")] if a.skips else list(range(0, a.max_skip + 1)))
    if a.delays:
        delays = [int(x) for x in a.delays.split(",")]
    combos = [(d, k) for k in skips for d in delays]
    chosen = None
    for d, k in combos:
        scan = opening_pass(log, None, press_delay=d, skip_at=k)
        res = {k: {"tick": scan["best"][k][1], "score": scan["best"][k][0],
                   "fork": scan["phase"].get(k), "legacy": targets[k],
                   "equal": bool(scan["phase"].get(k)) and same_phase(scan["phase"][k], targets[k])}
               for k, _ in BOOT_TARGETS}
        entry = {"press_delay": d, "skip_at": k, "marks": scan["marks"], "points": res,
                 "all_equal": all(v["equal"] and v["score"] == 0 for v in res.values())}
        tried.append(entry)
        print(json.dumps(entry), flush=True)
        if entry["all_equal"]:
            chosen = entry
            break
    log.mkdir(parents=True, exist_ok=True)
    (log / "scan.json").write_text(json.dumps({"targets": targets, "tried": tried}, indent=1) + "\n")
    if chosen is None:
        raise SystemExit("no (title delay, movie skip) gave the v2.6.3 loop-top phase at 02, 03 and 04: see "
                         + str(log / "scan.json"))
    d, k = chosen["press_delay"], chosen["skip_at"]
    opening_pass(log, {key: v["tick"] for key, v in chosen["points"].items()}, press_delay=d,
                 prefix="phase/", skip_at=k)
    cmd_manifest(None)


def cmd_boot(a) -> None:
    if getattr(a, "phase_lock", False):
        return cmd_phase_lock(a)
    log = FORK_STATES / "logs/boot"
    if a is None or a.redo or not (FORK_STATES / "slot01_title/state.p2s").exists():
        boot_title(log)
    cmd_manifest(None)
    scan = opening_pass(log, None)
    report = {k: {"score": v[0], "tick": v[1], "spans": v[2]} for k, v in scan["best"].items()}
    print("pass 1:", json.dumps(report), json.dumps(scan["marks"]), flush=True)
    (FORK_STATES / "logs/boot/scan.json").write_text(json.dumps({"best": report, "marks": scan["marks"]},
                                                                indent=1) + "\n")
    opening_pass(log, {k: v["tick"] for k, v in report.items()})
    cmd_manifest(None)


# ---------------------------------------------------------------------------
# route: route_capture beats in fork mode

ROUTE_BEATS = list(rc.BEATS)          # 00..15, beat 15 included (opt-in in route_capture)


def cmd_route(a) -> None:
    rc.use_fork(_gen(a)[0])
    wanted = None if a.beats == "all" else set(a.beats.split(","))
    for name, source, fn in ROUTE_BEATS:
        if wanted is None or name in wanted or name[:2] in wanted:
            rc.run_beat(name, source, fn)
    cmd_manifest(None)


# ---------------------------------------------------------------------------
# status: BATTERY prompt -> status root -> status hub (legacy slots 08, 12, 14)

def _gen(a) -> tuple[str, str]:
    """(generation, key prefix) of a command: base, or phase ("phase/")."""
    g = getattr(a, "generation", None) or "phase"
    return g, ("phase/" if g == "phase" else "")


def cmd_status(a) -> None:
    gen, prefix = _gen(a)
    rc.use_fork(gen)
    src = rc.beat_dir("02_elevator_refusal") / "state.p2s"
    with ForkSession(src, log_dir=FORK_STATES / "logs" / (prefix + "status")) as s:
        # The v2.6.3 slot 08 chain was hand-played (no recorded lead-in).  In the
        # phase generation each save point idles 10 ticks as before, plus one
        # more when that lands on the other frame index / field than the v2.6.3
        # slot (phase_extra_ticks in fork_state.json).
        def idle_to(r, n: int, slot: str) -> dict:
            r.idle(n)
            extra = 0
            if gen == "phase":
                want = loop_top_phase(slot)
                while not same_phase(read_phase(s), want):
                    if extra >= 2:
                        raise RuntimeError(f"slot {slot}: phase {read_phase(s)} never equals {want}")
                    r.idle(1)
                    extra += 1
            return {"phase_extra_ticks": extra}
        r = rc.Route(s)
        r.begin()
        rc.use_panel(r)
        r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][8:10] == "05"
                and row["ui"][10:12] == "04", 900)
        old8 = fingerprint(slot_spans("08"))
        r.until(lambda row: row["ui"] == old8["ui"], 300)
        x = idle_to(r, 10, "08")
        save_point(s, prefix + "slot08_battery_prompt", slot_file("08"),
                   "from route 02_elevator_refusal: route_capture's use_panel (walk to the panel, "
                   "face it, Cross) with the battery item; the BATTERY page's use prompt, 10 ticks "
                   "after its UI bytes equal slot 08's", prefix + "route/02_elevator_refusal",
                   dict(x, phase=read_phase(s)))
        # panel_root_probe.py: Circle -> BATTERY browse (+8), Circle -> status root (+10)
        r.press("CIRCLE", 2)
        r.until(lambda row: row["ui"][8:12] == "0501", 300)
        r.idle(8)
        r.press("CIRCLE", 2)
        r.until(lambda row: row["ui"][2:6] == "0302" and row["ui"][8:10] == "01", 300)
        x = idle_to(r, 10, "12")
        save_point(s, prefix + "slot12_status_root", slot_file("12"),
                   "from slot08_battery_prompt: Circle (BATTERY browse, +8 ticks), Circle (status "
                   "root), +10 ticks, as panel_root_probe.py did", prefix + "slot08_battery_prompt",
                   dict(x, phase=read_phase(s)))
        # status_hub_probe.py: Circle -> the normal status hub (+10)
        r.press("CIRCLE", 2)
        r.until(lambda row: row["ui"][2:6] == "0101", 300)
        x = idle_to(r, 10, "14")
        save_point(s, prefix + "slot14_status_hub", slot_file("14"),
                   "from slot12_status_root: Circle (the normal status hub), +10 ticks, as "
                   "status_hub_probe.py did", prefix + "slot12_status_root",
                   dict(x, phase=read_phase(s)))
        (FORK_STATES / "logs" / (prefix + "status")).mkdir(parents=True, exist_ok=True)
        (FORK_STATES / "logs" / (prefix + "status") / "trace.json").write_text(json.dumps(
            {"inputs": r.inputs, "rows": r.rows}, separators=(",", ":")) + "\n")
    cmd_manifest(None)


# ---------------------------------------------------------------------------
# roger: roger_encounter_probe.py's teleport from slot 03 (legacy slot 15)

def cmd_roger(a) -> None:
    gen, prefix = _gen(a)
    with ForkSession(fork_state("slot03_fade_in", gen), log_dir=FORK_STATES / "logs" / (prefix + "roger")) as s:
        # the probe waited for player +4 == 1 and selector 3B8D == 0 (free running)
        for _ in range(600):
            p = s.read(PLAYER, 8)
            if p[4] == 1 and s.read(0x70003B8C, 4)[1] == 0:
                break
            s.step(1)
        else:
            raise TimeoutError("first-control condition not reached")
        before = s.read(0x810350, 32).hex()
        value = struct.pack("<4f", 340, 290, 190, 1)
        s.write(0x810350, value)
        s.write(0x810360, value)
        t_teleport = s.frames_stepped
        first = None
        for _ in range(1200):
            s.step(1)
            p = s.read(PLAYER, 0x300)
            if p[0x2F3] == 2 and struct.unpack_from("<H", p, 0x2C)[0] == 1:
                if first is None:
                    first = s.frames_stepped
                if s.frames_stepped >= first + 50:
                    break
        else:
            raise TimeoutError("the encounter's bank-96 player clip did not start")
        extra = 0
        if gen == "phase":                  # one more tick when +50 lands on the other phase
            want = loop_top_phase("15")
            while not same_phase(read_phase(s), want):
                if extra >= 2:
                    raise RuntimeError(f"slot 15: phase {read_phase(s)} never equals {want}")
                s.step(1)
                extra += 1
        save_point(s, prefix + "slot15_roger_encounter", slot_file("15"),
                   "from slot03_fade_in: wait for player +4 == 1 and selector 3B8D == 0, write the "
                   "position qwords 0x810350/0x810360 = (340, 290, 190, 1) (the Roger trigger; "
                   "nothing else written), run until player +0x2F3 == 2 and +0x2C == 1, then 50 "
                   "ticks, as roger_encounter_probe.py did", prefix + "slot03_fade_in",
                   {"teleport_tick": t_teleport, "teleport_before": before, "clip_start_tick": first,
                    "phase_extra_ticks": extra, "phase": read_phase(s)})
    cmd_manifest(None)


# ---------------------------------------------------------------------------
# manifest

ALIASES = {"01": "slot01_title", "02": "slot02_opening", "03": "slot03_fade_in",
           "04": "slot04_first_control", "08": "slot08_battery_prompt", "12": "slot12_status_root",
           "14": "slot14_status_hub", "15": "slot15_roger_encounter"}


def _entry(key: str, state: Path, old: Path | None, meta: dict) -> dict:
    new_sp = spans_from_images(*images_of(state))
    snap = json.loads((state.parent / "snapshot.json").read_text())
    e = {"file": str(state.relative_to(FORK_STATES)),
         "old": str(old.relative_to(ROOT)) if old else None,
         "how": meta.get("how") or meta.get("what"), "source": meta.get("source"),
         "fork_rev": snap.get("fork_rev"), "fork_hash": snap.get("fork_hash"),
         "save_version": snap.get("save_version"), "counter": snap.get("main_loop_counter"),
         "sha256": hashlib.sha256(state.read_bytes()).hexdigest(),
         "fingerprint": fingerprint(new_sp)}
    slot = _slot_of(old)
    if old is not None and (slot or old.exists()):
        old_sp = old_spans(old)
        e["old_counter"] = slot_counter(slot) if slot else counter_of(images_of(old)[1])
        e["old_fingerprint"] = fingerprint(old_sp)
        e["fingerprint_diff"] = fp_diff(e["old_fingerprint"], e["fingerprint"])
        total, per = score(old_sp, new_sp)
        e["score"], e["score_spans"] = total, per
    return e


ALL_SLOTS = ("01", "02", "03", "04", "06", "07", "08", "11", "12", "13", "14", "15")


def cmd_manifest(_a) -> None:
    for slot in ALL_SLOTS:                  # cache every slot's targets while the files exist
        if slot_file(slot).exists():
            slot_spans(slot)
    states, aliases = {}, {}
    for slot, key in ALIASES.items():
        d = FORK_STATES / key
        if (d / "state.p2s").exists():
            meta = json.loads((d / "fork_state.json").read_text())
            states[key] = _entry(key, d / "state.p2s", slot_file(slot), meta)
            states[key]["phase"] = _slot_phase_pair(d / "state.p2s", slot)
            aliases[slot] = key
            aliases["slot" + slot] = key
    rc.use_fork("base")                       # the base generation's beats (fork-states/beats/)
    for name, source, _fn in rc.BEATS:
        d = rc.beat_dir(name)
        if (d / "state.p2s").exists():
            trace = json.loads((d / "trace.json").read_text())
            meta = {"what": trace.get("what"), "source": ("route/" + source) if not source.isdigit()
                    else ALIASES.get(source, source)}
            old = rc._legacy_beat_dir(name) / "state.p2s"
            e = _entry("route/" + name, d / "state.p2s", old, meta)
            e["how"] = (f"route_capture --emulator fork --beats {name}: closed loop from "
                        f"{meta['source']} ({trace.get('what')})")
            e["frames"] = trace.get("frames")
            old_trace = rc._legacy_beat_dir(name) / "trace.json"
            if old_trace.exists():
                e["old_frames"] = json.loads(old_trace.read_text()).get("frames")
            states["route/" + name] = e
            aliases[name] = "route/" + name
    # the phase-locked generation: phase/<slot key> folders, and the route
    # beats route_capture hard-linked into phase/beats/<path>/<beat>/
    for slot, key in ALIASES.items():
        d = FORK_STATES / "phase" / key
        if (d / "state.p2s").exists():
            meta = json.loads((d / "fork_state.json").read_text())
            e = _entry("phase/" + key, d / "state.p2s", slot_file(slot), meta)
            e["phase"] = _slot_phase_pair(d / "state.p2s", slot)
            states["phase/" + key] = e
    for st in sorted((FORK_STATES / "phase/beats").glob("**/state.p2s")):
        info = json.loads((st.parent / "beat.json").read_text())
        name = info["beat"]
        old = rc._legacy_beat_dir(name) / "state.p2s"
        src = info.get("source") or ""
        meta = {"what": info.get("what"),
                "source": ("phase/route/" + src) if not src.isdigit() else "phase/" + ALIASES.get(src, src)}
        e = _entry("phase/route/" + name, st, old if old.exists() else None, meta)
        e["how"] = (f"route_capture --emulator fork --generation phase --beats {name}: closed loop "
                    f"from {meta['source']} with the v2.6.3 lead-in and tail ({info.get('what')})")
        e["folder"] = info.get("folder")
        e["frames"] = info.get("frames")
        e["phase_rows"] = info.get("phase")
        if old.exists():
            e["phase"] = _phase_pair(st, old)
        states["phase/route/" + name] = e
        aliases.setdefault(name, "route/" + name)
    m = {"what": "fork-saved (0x9A59) replacements for the v2.6.3 states the project's tools use; "
                 "written by tools/fork_states.py (docs/PCSX2_FORK.md)",
         "generated": time.strftime("%Y-%m-%d %H:%M:%S"), "fork_repo": str(FORK_REPO),
         "fork_head": fork_head(),
         "generations": {"base": "keys without a prefix: same game points as the v2.6.3 states, phase "
                                 "as the fork's boot gave it (decomp 640fac0); route/<beat> = the first "
                                 "regeneration's route, no lead-in",
                         "phase": "keys 'phase/...': same game points (rand() state and player clock "
                                  "included) AND the frame index D_00810E80 and field D_00810E88 of the "
                                  "v2.6.3 session's first loop top after loading the slot (every user slot "
                                  "was saved at the vsync wait: the stored phase flipped); phase/route/<beat> "
                                  "= the canonical chain build/fork_refs/... (fork_states.py boot "
                                  "--phase-lock, status, roger; route_capture / route_lanes.py)"},
         "default_generation": "phase", "phase_free": ["slot01_title"],
         "states": states, "aliases": aliases}
    FORK_STATES.mkdir(parents=True, exist_ok=True)
    tmp = FORK_MANIFEST.with_suffix(".json.tmp")
    tmp.write_text(json.dumps(m, indent=1) + "\n")
    tmp.replace(FORK_MANIFEST)               # readers never see a half-written manifest
    print(f"manifest: {len(states)} states -> {FORK_MANIFEST}", flush=True)


def _phase_pair(new: Path, old: Path) -> dict:
    """Frame index / field of a fork state and of the v2.6.3 route snapshot it
    replaces (both saved at the loop top)."""
    n, o = legacy_phase(new), legacy_phase(old) if old.exists() else None
    return {"fork": n, "v263": o, "equal": bool(o) and same_phase(n, o)}


def _slot_phase_pair(new: Path, slot: str) -> dict:
    """Frame index / field of a fork slot state against the v2.6.3 slot: the
    stored values (mid-iteration, at the vsync wait) and the loop-top phase a
    session from the slot started on, which is what the fork state matches."""
    n = legacy_phase(new)
    lt = loop_top_phase(slot)
    return {"fork": n, "v263_stored": lt["stored"], "v263_saved_at": lt["saved_at"],
            "v263_loop_top": {"frame_index": lt["frame_index"], "field": lt["field"]},
            "equal": same_phase(n, lt)}


# ---------------------------------------------------------------------------
# verify

def cmd_verify(a) -> None:
    m = json.loads(FORK_MANIFEST.read_text())
    keys = list(m["states"]) if a.keys == "all" else [m["aliases"].get(k, k) for k in a.keys.split(",")]
    results = {}
    for key in keys:
        e = m["states"][key]
        state = FORK_STATES / e["file"]
        res = {"file": e["file"]}
        try:
            file_sp = spans_from_images(*images_of(state))
            with ForkSession(state, log_dir=FORK_STATES / "logs/verify" / key.replace("/", "_")) as s:
                live = live_spans(s)
                res["live_equals_file"] = score(file_sp, live)[0] == 0 and \
                    s.u32(FRAME_COUNTER) == counter_of(images_of(state)[1])
                res["counters"] = s.step(2)
                res["loads"] = True
        except Exception as exc:  # noqa: BLE001
            res["loads"] = False
            res["error"] = repr(exc)
        res["fingerprint_diff"] = e.get("fingerprint_diff")
        res["fingerprint_matches_old"] = not e.get("fingerprint_diff")
        res["score"] = e.get("score")
        results[key] = res
        print(key, json.dumps(res), flush=True)
    out = {"verified": time.strftime("%Y-%m-%d %H:%M:%S"), "fork_head": m.get("fork_head"),
           "results": results}
    (FORK_STATES / "verify.json").write_text(json.dumps(out, indent=1) + "\n")


# ---------------------------------------------------------------------------
# rerun: one beat from its fork source, compared with both chains

TRACE_KEYS = ("pos", "yaw", "m1F0", "clip", "spad", "ui", "req", "power", "charge", "battery_item",
              "area", "fade", "story790")


def compare_traces(a: list[dict], b: list[dict], tol: float = 0.0) -> dict:
    n = min(len(a), len(b))
    first, per_key = None, {}
    for i in range(n):
        for k in TRACE_KEYS:
            x, y = a[i].get(k), b[i].get(k)
            if k == "pos" and x and y:
                same = max(abs(p - q) for p, q in zip(x, y)) <= tol
            elif k == "yaw" and x is not None and y is not None:
                same = abs(x - y) <= tol
            else:
                same = x == y
            if not same:
                per_key.setdefault(k, {"first_frame": i, "frames": 0})
                per_key[k]["frames"] += 1
                if first is None:
                    first = {"frame": i, "key": k, "a": x, "b": y}
    return {"frames": [len(a), len(b)], "compared": n, "first_difference": first, "keys": per_key}


def cmd_rerun(a) -> None:
    """Phase generation (default): from the canonical chain's source with the
    recorded lead-in (a user slot through route_capture.slot_start, which
    already shortens it by the v2.6.3 finishing iteration), as run_beat did.
    Base: from the base source, no lead-in."""
    gen = _gen(a)[0]
    rc.use_fork(gen)
    name = a.beat
    entry = next(b for b in rc.BEATS if b[0] == name)
    _n, source, fn = entry
    lead = 0
    if gen == "phase":
        src = rc.slot_start(source)[0] if source.isdigit() else rc.beat_dir(source) / "state.p2s"
        lead = rc.recorded_lead_in(name)
    else:
        src = rc.slot_path(source) if source.isdigit() else rc.beat_dir(source) / "state.p2s"
    out = RERUN / name
    if out.exists():
        shutil.rmtree(out)                          # absolute path under build/pcsx2-fork/rerun
    out.mkdir(parents=True)
    with ForkSession(src, log_dir=out / "logs") as s:
        if lead:
            s.step(lead)
        r = rc.Route(s)
        r.begin()
        meta = fn(r)
        rows = r.rows
    (out / "trace.json").write_text(json.dumps({"beat": name, "source": source, "source_state": str(src),
                                                "generation": gen, "lead_in_frames": lead, "meta": meta,
                                                "frames": r.frame_index, "inputs": r.inputs,
                                                "rows": rows}, separators=(",", ":")) + "\n")
    fork_trace = json.loads((rc.beat_dir(name) / "trace.json").read_text())
    old_trace = json.loads((rc._legacy_beat_dir(name) / "trace.json").read_text())
    report = {"beat": name, "source": source,
              "rerun_vs_fork_chain": compare_traces(rows, fork_trace["rows"]),
              "rerun_vs_v263": compare_traces(rows, old_trace["rows"], tol=0.0),
              "rerun_vs_v263_tol": compare_traces(rows, old_trace["rows"], tol=0.5),
              "inputs": {"rerun": r.inputs, "fork_chain": fork_trace["inputs"], "v263": old_trace["inputs"]},
              "end": {"rerun": rc.summary(rows[-1]), "fork_chain": rc.summary(fork_trace["rows"][-1]),
                      "v263": rc.summary(old_trace["rows"][-1])}}
    (out / "compare.json").write_text(json.dumps(report, indent=1) + "\n")
    print(json.dumps({k: report[k] for k in ("rerun_vs_fork_chain", "rerun_vs_v263", "end")}, indent=1))


def cmd_compare(_a) -> None:
    """Offline: every fork route beat's trace against its v2.6.3 trace, row by
    row from the beat start (fork-states/compare.json)."""
    rc.use_fork(_gen(_a)[0])
    out = {}
    for name, source, _fn in rc.BEATS:
        f, o = rc.beat_dir(name) / "trace.json", rc._legacy_beat_dir(name) / "trace.json"
        if not (f.exists() and o.exists()):
            continue
        a, b = json.loads(f.read_text()), json.loads(o.read_text())
        out[name] = {"frames_fork": a["frames"], "frames_v263": b["frames"],
                     "tail_fork": a.get("tail_idle_frames"), "tail_v263": b.get("tail_idle_frames"),
                     "inputs_equal": a["inputs"] == b["inputs"],
                     "exact": compare_traces(a["rows"], b["rows"]),
                     "tol_0_5": compare_traces(a["rows"], b["rows"], tol=0.5)}
        e = out[name]
        print(f"{name}: frames {e['frames_fork']}/{e['frames_v263']} inputs_equal {e['inputs_equal']} "
              f"first exact diff {e['exact']['first_difference']} keys {sorted(e['exact']['keys'])}", flush=True)
    (FORK_STATES / "compare.json").write_text(json.dumps(out, indent=1) + "\n")


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("boot")
    p.add_argument("--redo", action="store_true", help="redo the cold boot to slot01_title too")
    p.add_argument("--phase-lock", action="store_true",
                   help="write the phase-locked generation (phase/slot02..04): scan title-driver "
                        "delays until 02, 03 and 04 have the v2.6.3 frame index and field")
    p.add_argument("--press-delay", type=int, help="--phase-lock: use this delay, no scan")
    p.add_argument("--max-delay", type=int, default=0, help="--phase-lock: largest title delay scanned")
    p.add_argument("--skip-at", type=int, help="--phase-lock: skip the New Game movie this many vsyncs "
                                                "after its driver starts (no scan)")
    p.add_argument("--max-skip", type=int, default=7, help="--phase-lock: largest skip vsync scanned")
    p.add_argument("--no-skip", action="store_true", help="--phase-lock: play the movie (title delays only)")
    p.add_argument("--skips", default="", help="--phase-lock: comma list of skip vsyncs to scan")
    p.add_argument("--delays", default="", help="--phase-lock: comma list of title delays to scan")
    p = sub.add_parser("route")
    p.add_argument("--beats", default="all")
    p.add_argument("--generation", choices=["base", "phase"], default="phase")
    p = sub.add_parser("status")
    p.add_argument("--generation", choices=["base", "phase"], default="phase")
    p = sub.add_parser("roger")
    p.add_argument("--generation", choices=["base", "phase"], default="phase")
    sub.add_parser("manifest")
    p = sub.add_parser("verify")
    p.add_argument("--keys", default="all")
    p = sub.add_parser("compare")
    p.add_argument("--generation", choices=["base", "phase"], default="phase")
    p = sub.add_parser("rerun")
    p.add_argument("beat")
    p.add_argument("--generation", choices=["base", "phase"], default="phase")
    a = ap.parse_args()
    {"boot": cmd_boot, "route": cmd_route, "status": cmd_status, "roger": cmd_roger,
     "manifest": cmd_manifest, "verify": cmd_verify, "rerun": cmd_rerun, "compare": cmd_compare}[a.cmd](a)
