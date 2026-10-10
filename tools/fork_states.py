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
  route     route_capture beats 00..15 (`--beats` to choose) in fork mode, from
            slot04_first_control, each beat from the previous fork snapshot.
  status    from route 02_elevator_refusal: the panel's BATTERY prompt
            (slot08_battery_prompt), Circle to the status root
            (slot12_status_root), Circle to the status hub (slot14_status_hub),
            as build/startup-reference/panel_root_probe.py and
            status_hub_probe.py did on the legacy app.
  roger     from slot03_fade_in: roger_encounter_probe.py's teleport into the
            Roger trigger, 50 ticks after the bank-96 player clip starts
            (slot15_roger_encounter).
  manifest  rebuild manifest.json from the folders (fingerprints old vs new).
  verify    load every manifest state in the fork: the live machine equals the
            file, two ticks advance, the fingerprint matches the old capture's
            (fork-states/verify.json).
  compare   offline: every fork route trace against its v2.6.3 trace, row by
            row (fork-states/compare.json).
  rerun B   re-run route beat B from its fork source snapshot into
            build/pcsx2-fork/rerun/<B>/ and compare its trace with the fork
            chain's trace and with the v2.6.3 trace (build/s87/route/<B>).

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
                           SSTATES, PAD, FRAME_COUNTER, fork_state)
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
]
# Inside the player actor, bytes that are clocks rather than state (excluded
# from the score; still visible in the full diff).
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
        o = spans_from_images(*images_of(old))
        n = spans_from_images(*images_of(out))
        total, per = score(o, n)
        meta.update(score=total, score_spans=per)
    (out / "fork_state.json").write_text(json.dumps(meta, indent=1) + "\n")
    print(f"saved {key}: counter {info['main_loop_counter']}"
          + (f", score {meta.get('score')} {meta.get('score_spans')}" if old else ""), flush=True)
    return meta


def slot_file(slot: str) -> Path:
    return SSTATES / f"{SERIAL}.{slot}.p2s"


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
    old = spans_from_images(*images_of(slot_file("01")))
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


def opening_pass(log: Path, save_at: dict[str, int] | None) -> dict:
    """From slot01_title: the title inputs, NEW GAME, the movie, the AREA11
    load and opening, to first control + 200 ticks.  save_at None = pass 1
    (scores only); else save each key at its tick."""
    olds = {key: spans_from_images(*images_of(slot_file(slot))) for key, slot in BOOT_TARGETS}
    best = {key: (1 << 30, -1, None) for key, _ in BOOT_TARGETS}
    marks: dict = {}
    with ForkSession(fork_state("slot01_title"), log_dir=log / ("save" if save_at else "scan"),
                     boundary_timeout=900.0) as s:   # the NEW GAME movie plays inside one tick
        sampler = rc.Sampler(s)
        presses: list[int] = []
        seen_cut = False
        control_at = None
        n = 0
        while True:
            row = rc.decode(sampler.raw())
            sp = live_spans(s)
            if marks.get("new_game") is None and sp["area"][0] != 0:
                marks["new_game"] = s.frames_stepped
                s.pad(0)
            if marks.get("new_game") is None:
                title_inputs(s, n, presses)
            else:
                for key, _slot in BOOT_TARGETS:
                    total, per = score(olds[key], sp)
                    if total < best[key][0]:
                        best[key] = (total, s.frames_stepped, per)
                if rc.cutscene(row):
                    seen_cut = True
                if seen_cut and control_at is None and rc.in_control(row):
                    control_at = s.frames_stepped
                    marks["first_control"] = control_at
            if save_at:
                for key, tick in save_at.items():
                    if tick == s.frames_stepped:
                        slot = dict(BOOT_TARGETS)[key]
                        save_point(s, key, slot_file(slot), HOW[key], "slot01_title",
                                   {"ticks_from_slot01": tick, "marks": dict(marks)})
                if s.frames_stepped >= max(save_at.values()):
                    break
            if control_at is not None and s.frames_stepped >= control_at + 200:
                break
            if s.frames_stepped > 12000:
                raise TimeoutError("the opening did not reach first control")
            s.step(1)
            n += 1
        marks["presses"] = presses
    return {"best": best, "marks": marks}


HOW = {
    "slot02_opening": "from slot01_title: route_census's title inputs (Cross once the fade is clear), "
                      "NEW GAME, the movie, the AREA11 load and opening with no input; the tick whose "
                      "game state best matches slot 02 (the opening cinematic)",
    "slot03_fade_in": "as slot02_opening; the tick that best matches slot 03 (the fade-in after the "
                      "opening, movement still locked)",
    "slot04_first_control": "as slot02_opening; the tick that best matches slot 04 (first control, "
                            "the fade done)",
}


def cmd_boot(a) -> None:
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
    rc.use_fork()
    wanted = None if a.beats == "all" else set(a.beats.split(","))
    for name, source, fn in ROUTE_BEATS:
        if wanted is None or name in wanted or name[:2] in wanted:
            rc.run_beat(name, source, fn)
    cmd_manifest(None)


# ---------------------------------------------------------------------------
# status: BATTERY prompt -> status root -> status hub (legacy slots 08, 12, 14)

def cmd_status(_a) -> None:
    rc.use_fork()
    src = rc.beat_dir("02_elevator_refusal") / "state.p2s"
    with ForkSession(src, log_dir=FORK_STATES / "logs/status") as s:
        r = rc.Route(s)
        r.begin()
        rc.use_panel(r)
        r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][8:10] == "05"
                and row["ui"][10:12] == "04", 900)
        old8 = fingerprint(spans_from_images(*images_of(slot_file("08"))))
        r.until(lambda row: row["ui"] == old8["ui"], 300)
        r.idle(10)
        save_point(s, "slot08_battery_prompt", slot_file("08"),
                   "from route 02_elevator_refusal: route_capture's use_panel (walk to the panel, "
                   "face it, Cross) with the battery item; the BATTERY page's use prompt, 10 ticks "
                   "after its UI bytes equal slot 08's", "route/02_elevator_refusal")
        # panel_root_probe.py: Circle -> BATTERY browse (+8), Circle -> status root (+10)
        r.press("CIRCLE", 2)
        r.until(lambda row: row["ui"][8:12] == "0501", 300)
        r.idle(8)
        r.press("CIRCLE", 2)
        r.until(lambda row: row["ui"][2:6] == "0302" and row["ui"][8:10] == "01", 300)
        r.idle(10)
        save_point(s, "slot12_status_root", slot_file("12"),
                   "from slot08_battery_prompt: Circle (BATTERY browse, +8 ticks), Circle (status "
                   "root), +10 ticks, as panel_root_probe.py did", "slot08_battery_prompt")
        # status_hub_probe.py: Circle -> the normal status hub (+10)
        r.press("CIRCLE", 2)
        r.until(lambda row: row["ui"][2:6] == "0101", 300)
        r.idle(10)
        save_point(s, "slot14_status_hub", slot_file("14"),
                   "from slot12_status_root: Circle (the normal status hub), +10 ticks, as "
                   "status_hub_probe.py did", "slot12_status_root")
        (FORK_STATES / "logs/status").mkdir(parents=True, exist_ok=True)
        (FORK_STATES / "logs/status/trace.json").write_text(json.dumps(
            {"inputs": r.inputs, "rows": r.rows}, separators=(",", ":")) + "\n")
    cmd_manifest(None)


# ---------------------------------------------------------------------------
# roger: roger_encounter_probe.py's teleport from slot 03 (legacy slot 15)

def cmd_roger(_a) -> None:
    with ForkSession(fork_state("slot03_fade_in"), log_dir=FORK_STATES / "logs/roger") as s:
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
        save_point(s, "slot15_roger_encounter", slot_file("15"),
                   "from slot03_fade_in: wait for player +4 == 1 and selector 3B8D == 0, write the "
                   "position qwords 0x810350/0x810360 = (340, 290, 190, 1) (the Roger trigger; "
                   "nothing else written), run until player +0x2F3 == 2 and +0x2C == 1, then 50 "
                   "ticks, as roger_encounter_probe.py did", "slot03_fade_in",
                   {"teleport_tick": t_teleport, "teleport_before": before, "clip_start_tick": first})
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
    if old is not None and old.exists():
        old_ee, old_sp_img = images_of(old)
        old_sp = spans_from_images(old_ee, old_sp_img)
        e["old_counter"] = counter_of(old_sp_img)
        e["old_fingerprint"] = fingerprint(old_sp)
        e["fingerprint_diff"] = fp_diff(e["old_fingerprint"], e["fingerprint"])
        total, per = score(old_sp, new_sp)
        e["score"], e["score_spans"] = total, per
    return e


def cmd_manifest(_a) -> None:
    states, aliases = {}, {}
    for slot, key in ALIASES.items():
        d = FORK_STATES / key
        if (d / "state.p2s").exists():
            meta = json.loads((d / "fork_state.json").read_text())
            states[key] = _entry(key, d / "state.p2s", slot_file(slot), meta)
            aliases[slot] = key
            aliases["slot" + slot] = key
    rc.use_fork()
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
    m = {"what": "fork-saved (0x9A59) replacements for the v2.6.3 states the project's tools use; "
                 "written by tools/fork_states.py (docs/PCSX2_FORK.md)",
         "generated": time.strftime("%Y-%m-%d %H:%M:%S"), "fork_repo": str(FORK_REPO),
         "fork_head": fork_head(), "states": states, "aliases": aliases}
    FORK_STATES.mkdir(parents=True, exist_ok=True)
    FORK_MANIFEST.write_text(json.dumps(m, indent=1) + "\n")
    print(f"manifest: {len(states)} states -> {FORK_MANIFEST}", flush=True)


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
    rc.use_fork()
    name = a.beat
    entry = next(b for b in rc.BEATS if b[0] == name)
    _n, source, fn = entry
    src = rc.slot_path(source) if source.isdigit() else rc.beat_dir(source) / "state.p2s"
    out = RERUN / name
    if out.exists():
        shutil.rmtree(out)                          # absolute path under build/pcsx2-fork/rerun
    out.mkdir(parents=True)
    with ForkSession(src, log_dir=out / "logs") as s:
        r = rc.Route(s)
        r.begin()
        meta = fn(r)
        rows = r.rows
    (out / "trace.json").write_text(json.dumps({"beat": name, "source": source, "meta": meta,
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
    rc.use_fork()
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
    p = sub.add_parser("route")
    p.add_argument("--beats", default="all")
    sub.add_parser("status")
    sub.add_parser("roger")
    sub.add_parser("manifest")
    p = sub.add_parser("verify")
    p.add_argument("--keys", default="all")
    sub.add_parser("compare")
    p = sub.add_parser("rerun")
    p.add_argument("beat")
    a = ap.parse_args()
    {"boot": cmd_boot, "route": cmd_route, "status": cmd_status, "roger": cmd_roger,
     "manifest": cmd_manifest, "verify": cmd_verify, "rerun": cmd_rerun, "compare": cmd_compare}[a.cmd](a)
