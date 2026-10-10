#!/usr/bin/env python3
"""route_capture.py - original ground truth for the AREA11 first-level route.

Drives the ORIGINAL game through tools/pcsx2_session.py (hidden PCSX2, exact
one-frame steps) from the user's own save state 04 (first control), with pad
input only, and records a per-frame trace of the fields the port needs for
each beat of the route (battery pickup, elevator refusal, panel power,
elevator ride, boxes, hill slide, truck preview, the Roger encounter, and
beat 15: the level exit through Roger's departure to the AREA01 arrival).
Every beat ends with pcsx2_session.snapshot() into build/s87/route/<nn_beat>/
so a later session can resume from that beat's state.p2s.

Everything written here is derived from the user's own disc and stays in the
gitignored build/ tree. No original code or data is embedded in this file;
it names addresses only.

Emulator (2026-10-09, the user's decision to retire PCSX2 v2.6.3): the default
is the agent-debug fork (docs/PCSX2_FORK.md) with the phase-locked fork states
of build/startup-reference/fork-states/manifest.json.  Each beat then replays
its v2.6.3 capture's lead-in and tail, so its rows land on the same game tick,
frame index D_00810E80 and field D_00810E88 as the v2.6.3 rows; every row
records fi / fld / vs (game vsync); beat folders go to build/fork_refs/<path>
(the v2.6.3 path under build/ mirrored) with a manifest.json per set, and the
beat states are hard-linked into fork-states/phase/beats/.  `--generation base`
writes the first regeneration's layout (fork-states/beats/), `--emulator
legacy` the v2.6.3 app and the build/<path> folders below (until the app is
retired).  The paths below are the legacy ones.

Usage (decomp .venv python, from the repo root):
    .venv/bin/python tools/route_capture.py identify            # user slot table
    .venv/bin/python tools/route_capture.py run --beats all     # beats 00..14, in order
    .venv/bin/python tools/route_capture.py run --beats 07,08   # some beats (each resumes
                                                                # from its source beat)
    .venv/bin/python tools/route_capture.py run --beats 15      # the level exit (opt-in)
Beat 15 (15_level_exit) is opt-in: `--beats all` leaves it out and only an
explicit `--beats 15` (or its full name) runs it.  Its departure movie plays
inside ONE emulated frame, which costs roughly 80..260 s of host time (the
frame timeout is raised to 900 s for that beat), so the beat takes several
minutes on its own.
    .venv/bin/python tools/route_capture.py events --beats 03   # change log of a trace
    .venv/bin/python tools/route_capture.py run --beats a01     # AREA01 group (opt-in), in order
    .venv/bin/python tools/route_capture.py run --beats a01_03,a01_s1   # some AREA01 beats
AREA01 beats (a01_*) start from the beat-15 snapshot and write to
build/s87/route_a01/<beat>/; they are described in the port's
docs/SECOND_LEVEL_ROUTE.md.
    .venv/bin/python tools/route_capture.py run --beats a00     # AREA00 group (opt-in), in order
AREA00 beats (a00_*) start from the a01_07 snapshot or an earlier a00 beat and write to
build/s87/route_a00/<beat>/; they are described in the port's
docs/THIRD_LEVEL_ROUTE.md.
    .venv/bin/python tools/route_capture.py run --beats a01r    # AREA01 revisit group (opt-in)
    .venv/bin/python tools/route_capture.py run --beats a02     # AREA02 group (opt-in)
The AREA01 revisit (a01r_*, from the a00_10 snapshot) and AREA02 (a02_*) beats
write to build/s87/route_a01r/<beat>/ and build/s87/route_a02/<beat>/; they
are described in the port's docs/FOURTH_LEVEL_ROUTE.md.
    .venv/bin/python tools/route_capture.py run --beats a04     # AREA04 group (opt-in)
The AREA04 beats (a04_*, from the a02_05 snapshot) write to
build/s87/route_a04/<beat>/; they are described in the port's
docs/FIFTH_LEVEL_ROUTE.md.
    .venv/bin/python tools/route_capture.py run --beats a22     # AREA22 group (opt-in)
The AREA22 beats (a22_*, from the a04_05 snapshot) write to
build/s87/route_a22/<beat>/; they are described in the port's
docs/SIXTH_LEVEL_ROUTE.md.
    .venv/bin/python tools/route_capture.py run --beats a01u    # AREA01 upper-floor group (opt-in)
    .venv/bin/python tools/route_capture.py run --beats a06     # AREA06 group (opt-in)
The AREA01 upper-floor beats (a01u_*, from the a22_02 snapshot) and the AREA06
beats (a06_*, from the a01u_02 snapshot) write to build/s87/route_a01u/<beat>/
and build/s87/route_a06/<beat>/; they are described in the port's
docs/SEVENTH_LEVEL_ROUTE.md.
    .venv/bin/python tools/route_capture.py run --beats c7       # C7 capture group (opt-in)
C7 beats (c7_*) are original captures the C6 chain requested; each writes to
build/s87/c7cap/<item>/<beat>/ and is described in docs/CAPTURES_C7.md.
None of them runs under `--beats all`.
    .venv/bin/python tools/route_capture.py run --beats a13     # AREA13 group (opt-in)
    .venv/bin/python tools/route_capture.py run --beats a19,a13b  # tenth level (opt-in)
    .venv/bin/python tools/route_capture.py run --beats a13c       # eleventh level (opt-in)
    .venv/bin/python tools/route_capture.py run --beats a13d,a19b  # twelfth level (opt-in)
    .venv/bin/python tools/route_capture.py run --beats a19c       # thirteenth level (opt-in)
    .venv/bin/python tools/route_capture.py run --beats a19d,a15   # fourteenth level (opt-in)
    .venv/bin/python tools/route_capture.py run --beats a15b,a19e,a03  # fifteenth level (opt-in)
The AREA13 beats (a13_*, from the a04b_04_lift snapshot) write to
build/s87/route_a13/<beat>/; they are described in the port's
docs/NINTH_LEVEL_ROUTE.md.
    .venv/bin/python tools/route_capture.py run --beats aim      # AIM capture group (opt-in)
The AIM beats (aim_*: aiming, firing, reloads, the gun light, melee and the
security gun's cable in AREA11, from the 08_truck_crossing snapshot) write to
build/aimfire/capture/<beat>/; they are described in docs/CAPTURES_C10.md
(section AIM).
    .venv/bin/python tools/route_capture.py run --beats exit     # EXIT capture group (opt-in)
The EXIT beats (exit_*: beat 15's level exit re-recorded closed loop with a
wider trace, from the 14_roger_encounter snapshot: the fan crossing and Roger's
departure, then the movie, the area change, the AREA01 load and the arrival up
to control) write to build/c10/exit/<beat>/; they are described in
docs/CAPTURES_C10.md (section EXIT).  Like beat 15, exit_01 costs minutes of
host time (the movie plays inside one frame).
    .venv/bin/python tools/route_capture.py run --beats dmg      # DAMAGE capture group (opt-in)
The DAMAGE beats (dmg_*: the flame's contacts, low health, death, the game-over
screen, the title menu after a death, NEW GAME, the LOAD GAME screen, the
crevice fall, the truck pit's 0x5D floor and the fan's hit, from route
snapshots 07, 11 and 14 or an earlier dmg beat) write to build/c10/damage/<beat>/;
they are described in docs/CAPTURES_C10.md (section DAMAGE).
    .venv/bin/python tools/route_capture.py run --beats br       # BRANCH capture group (opt-in)
The BRANCH beats (br_*: the AREA11 branches the route skips: the optional
pickups g0.1..g0.6, the west-yard and plateau ladders up and down, the boxes
broken by melee, the terminal's ride back up, the panel's BATTERY
prompt declined, Roger's talk after the encounter; from route snapshots 02..14 or an
earlier br beat) write to build/c10/branch/<beat>/; they are described in
docs/CAPTURES_C10.md (section BRANCH).
    .venv/bin/python tools/route_capture.py run --beats opt      # OPTIONS capture group (opt-in)
The OPTIONS beats (opt_*: the in-game options screen SELECT opens in AREA11,
every row changed and changed back, its load screen up to the memory-card slot
choice and its quit prompt declined, from the 08_truck_crossing snapshot)
write to build/c10/options/<beat>/; they are described in docs/CAPTURES_C10.md
(section OPTIONS).  No beat writes a memory card.
The route and every beat are described in the port's docs/FIRST_LEVEL_ROUTE.md.
"""
from __future__ import annotations

import argparse
import json
import math
import shutil
import struct
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from pcsx2_session import (OriginalSession, ForkSession, PAD, SSTATES, FORK_STATES, fork_state,  # noqa: E402
                           DEFAULT_BACKEND, FRAME_INDEX)

SERIAL = "SCUS-97112 (0AE679AF)"
OUT = ROOT / "build/s87/route"
PLAYER = 0x8102B0

# Owner nodes (AREA11 placement records, list order; see the port's
# docs/ORIGINAL_FRAME_ORDER.md section 4).  Stable across every state
# captured from the same New Game load.
OWNERS = {
    "battery_g0_0": 0x7A5640,   # deferred g0.0, 00219550, item type 0x1B
    "crate_r3": 0x7A7980, "crate_r4": 0x7A7C70, "crate_r5": 0x7A7F60,
    "crate_r6": 0x7A8250,       # 001551B0
    "roger_r8": 0x7A8830,       # overlay 0x8237E0
    "truck_r16": 0x7A9FB0,      # overlay 0x823FF0
    "trigger_r17": 0x7AA2A0,    # overlay 0x8251E0
    "panel_r18": 0x7AA590,      # 00159210
    "elevator_r19": 0x7AA880,   # overlay 0x827B10
    "door_r0": 0x7A70B0,        # 001BC350 fence room-move door
    "attach_r9": 0x7A8B20,      # 001C5C90 attachment near Roger
    "manager_r11": 0x7A9100,    # overlay 0x823CE0 (class 9 manager)
    "director_r12": 0x7A93F0,   # overlay 0x8253F0 (class 9 director, 810813 beats)
}

# (name, address, byte length) read once per frame in ONE Pine request.
SPANS = [
    ("counter", 0x70003B64, 4),
    ("spad", 0x70003B8C, 8),            # 3B8C..3B93
    ("player", PLAYER, 0x300),
    ("cam", 0x8101E0, 0x30),            # +4 top/cinematic byte, +10 eye, +20 target
    ("cam_actual", 0x8105D0, 0x40),     # actual eye/target, up, forward
    ("req", 0x8106B0, 0x10),            # D_008106B0..BF
    ("fade", 0x28A9A0, 0x10),           # transition/fade machine
    ("screen", 0x28A8D0, 0x20),         # 001AEBE0 screen-fade/letterbox block
    ("msg", 0x2821B0, 0x10),            # message machine mode/active/token
    ("ui", 0x810130, 0x10),             # status UI object
    ("prog", 0x810840, 0x10),           # 0x81084C area power bits
    ("story", 0x810790, 0x4),           # D_00810790..93 (792 truck, 793 Roger alternate)
    ("d2", 0x8107D8, 0x40),             # 8107D8 Roger progress .. 810813 Roger auxiliary
    ("inv", 0x810C64, 0x20),            # item counts (0x810C7F = battery 0x1B)
    ("charge", 0x810CB0, 0x8),          # 0x810CB2 charge (half units), CB7 max
    ("area", 0x810700, 0x4),
    ("phase", FRAME_INDEX, 0x14),       # D_00810E80 frame index, +8 field D_00810E88, +0x10 game vsync
    ("counter2", 0x70003B64, 4),
]
for _name, _base in OWNERS.items():
    SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
              (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14)]


class Sampler:
    def __init__(self, session: OriginalSession):
        self.s = session
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in SPANS for i in range(0, n, 4))

    def raw(self) -> dict[str, bytes]:
        for _ in range(5):
            data = self.s.pine.request(self.body)
            out, off = {}, 0
            for name, _a, n in SPANS:
                out[name] = data[off:off + n]
                off += n
            if out["counter"] == out["counter2"]:
                return out
        raise RuntimeError("inconsistent sample")


def f32(b: bytes, o: int) -> float:
    return struct.unpack_from("<f", b, o)[0]


def vec(b: bytes, o: int, n: int = 3) -> list[float]:
    return [round(v, 5) for v in struct.unpack_from(f"<{n}f", b, o)]


def decode(r: dict[str, bytes], owners=None) -> dict:
    """One trace row.  `owners` (name -> node) defaults to the AREA11 OWNERS;
    the AREA01 beats pass their own table (A01_OWNERS)."""
    owners = OWNERS if owners is None else owners
    p = r["player"]
    row = {
        "counter": struct.unpack("<I", r["counter"])[0],
        "pos": vec(p, 0xA0), "hip": vec(p, 0xB0), "yaw": round(f32(p, 0xC4), 5),
        "p5": p[5], "m1F0": p[0x1F0], "m1F1": p[0x1F1], "req1F2": struct.unpack_from("<h", p, 0x1F2)[0],
        "b2F3": p[0x2F3], "clip": struct.unpack_from("<h", p, 0x20C)[0],
        "clock": round(f32(p, 0x3C), 3), "ground": hex(struct.unpack_from("<I", p, 0x214)[0]),
        "spad": r["spad"].hex(),      # 3B8C..3B93; [1]=3B8D selector, [5]=3B91, [6]=3B92
        "cam_mode": r["cam"][4:8].hex(), "cam_eye": vec(r["cam"], 0x10), "cam_tgt": vec(r["cam"], 0x20),
        "eye": vec(r["cam_actual"], 0), "tgt": vec(r["cam_actual"], 0x10),
        "fwd": vec(r["cam_actual"], 0x30),
        "req": r["req"][:10].hex(), "fade": r["fade"].hex(), "screen": r["screen"].hex(),
        "msg": r["msg"].hex(), "ui": r["ui"][:8].hex(),
        "power": r["prog"][0xC], "story792": r["story"][2], "story790": r["story"].hex(),
        "d2": r["d2"].hex(),
        "battery_item": r["inv"][0x1B], "charge": struct.unpack_from("<H", r["charge"], 2)[0],
        "area": r["area"][:2].hex(),
        # the frame and field phase (docs/PCSX2_FORK_GS_DIFF.md 5): the drawing buffer and
        # the half-line offset this tick's field gets; "vs" is the game's vsync counter
        "fi": r["phase"][0], "fld": r["phase"][8], "vs": struct.unpack_from("<I", r["phase"], 0x10)[0],
    }
    for name in owners:
        h, pos, s, t = (r[name + k] for k in (":h", ":p", ":s", ":t"))
        row[name] = {"h": h.hex(), "pos": vec(pos, 0), "s1F0": s.hex(), "t2DC": t.hex()}
    return row


class Route:
    """One beat at a time: step frames with pad input and record every frame."""

    def __init__(self, session: OriginalSession):
        self.s = session
        self.sampler = Sampler(session)
        self.rows: list[dict] = []
        self.inputs: list[dict] = []
        self.teleports: list[dict] = []
        self.pad_state = (0, 0x7F, 0x7F)
        self.frame_index = 0

    # -- state ------------------------------------------------------------
    def now(self) -> dict:
        return decode(self.sampler.raw())

    def begin(self) -> None:
        self.rows, self.inputs, self.teleports = [], [], []
        self.frame_index = 0
        self.rows.append(dict(self.now(), f=0))

    def set_pad(self, buttons: int = 0, lx: int = 0x7F, ly: int = 0x7F) -> None:
        state = (buttons, lx, ly)
        if state != self.pad_state:
            self.s.pad(buttons, lx=lx, ly=ly)
            self.pad_state = state
            self.inputs.append({"f": self.frame_index, "buttons": buttons, "lx": lx, "ly": ly})

    def step(self, n: int = 1) -> dict:
        row = None
        for _ in range(n):
            self.s.step(1)
            self.frame_index += 1
            row = dict(self.now(), f=self.frame_index)
            self.rows.append(row)
        return row

    def teleport(self, x: float, y: float, z: float, reason: str) -> None:
        before = self.s.read(0x810350, 32).hex()
        value = struct.pack("<4f", x, y, z, 1.0)
        self.s.write(0x810350, value)
        self.s.write(0x810360, value)
        self.teleports.append({"f": self.frame_index, "position": [x, y, z],
                               "before": before, "reason": reason})

    # -- input helpers -------------------------------------------------------
    def press(self, name: str, frames: int = 2, after: int = 0) -> None:
        self.set_pad(PAD[name])
        self.step(frames)
        self.set_pad(0)
        if after:
            self.step(after)

    def idle(self, frames: int) -> dict:
        self.set_pad(0)
        return self.step(frames)

    def until(self, pred, limit: int, buttons: int = 0, lx: int = 0x7F, ly: int = 0x7F) -> dict:
        self.set_pad(buttons, lx, ly)
        row = self.rows[-1]
        for _ in range(limit):
            if pred(row):
                return row
            row = self.step(1)
        if pred(row):
            return row
        raise TimeoutError(f"condition not reached in {limit} frames; last {summary(row)}")

    def stick_toward(self, x: float, z: float, magnitude: float = 1.0) -> float:
        """Point the left stick at world (x, z) relative to the current camera.
        Stick up follows the camera forward (x, z) (docs/FIRST_CONTROL.md)."""
        row = self.rows[-1]
        px, _py, pz = row["pos"]
        fx, _fy, fz = row["fwd"]
        dx, dz = x - px, z - pz
        dist = math.hypot(dx, dz)
        norm = math.hypot(fx, fz) or 1.0
        fx, fz = fx / norm, fz / norm
        up = (dx * fx + dz * fz) / (dist or 1.0)
        right = (dx * RIGHT_SIGN * fz - dz * RIGHT_SIGN * fx) / (dist or 1.0)
        lx = int(round(0x80 + 0x7F * magnitude * right))
        ly = int(round(0x80 - 0x7F * magnitude * up))
        self.set_pad(0, max(0, min(255, lx)), max(0, min(255, ly)))
        return dist

    def goto(self, x: float, z: float, tol: float = 1.0, limit: int = 900,
             magnitude: float = 1.0, stop: bool = True, stuck_ok: bool = False) -> dict:
        history: list[tuple[float, float]] = []
        for _ in range(limit):
            if self.stick_toward(x, z, magnitude) <= tol:
                break
            self.step(1)
            px, _py, pz = self.rows[-1]["pos"]
            history.append((px, pz))
            if len(history) > 30 and math.hypot(px - history[-30][0], pz - history[-30][1]) < 0.05:
                if stuck_ok:
                    break
                raise TimeoutError(f"goto({x},{z}) stuck; last {summary(self.rows[-1])}")
        else:
            raise TimeoutError(f"goto({x},{z}) not reached; last {summary(self.rows[-1])}")
        if stop:
            self.set_pad(0)
        return self.rows[-1]

    # -- output --------------------------------------------------------------
    def save(self, name: str, meta: dict, snapshot: bool = True) -> Path:
        out = beat_dir(name)
        out.mkdir(parents=True, exist_ok=True)
        self.set_pad(0)
        info = self.s.snapshot(out) if snapshot else None
        doc = dict(meta, beat=name, frames=self.frame_index,
                   first_counter=self.rows[0]["counter"], last_counter=self.rows[-1]["counter"],
                   inputs=self.inputs, teleports=self.teleports, snapshot=info, rows=self.rows)
        (out / "trace.json").write_text(json.dumps(doc, separators=(",", ":")) + "\n")
        return out


# Stick-right world direction relative to the camera forward (fx, fz):
# right = RIGHT_SIGN * (fz, -fx).  Measured by `calibrate` below.
RIGHT_SIGN = -1.0


def summary(row: dict) -> str:
    return (f"c={row['counter']} pos={row['pos']} yaw={row['yaw']} m={row['m1F0']}/{row['m1F1']} "
            f"clip={row['clip']} spad={row['spad']} ui={row['ui']} req={row['req']}")


# Fork mode (the command line's default since 2026-10-09; `--emulator legacy`
# keeps the v2.6.3 app until it is retired; from Python: use_fork() /
# use_legacy()).  Every session is then a pcsx2_session.ForkSession on the
# agent-debug fork, a 2-digit source slot resolves through
# build/startup-reference/fork-states/manifest.json (fork_state), and beat
# folders never mix with the v2.6.3 captures in build/<path>:
#   generation "phase" (the command line's default): the phase-locked chain.
#       Sources are the phase/... states, each beat first runs the v2.6.3
#       capture's lead-in (the frames its legacy session ran between the load
#       and row 0, the pad as the session left it) and its tail, so every row
#       lands on the same game tick, frame index and field as the v2.6.3 row.
#       Beat folders go
#       to build/fork_refs/<path>, each set gets a manifest.json there, and
#       each beat's state is hard-linked into fork-states/phase/beats/<path>
#       and registered in the fork-states manifest (fork_states.py manifest).
#   generation "base": the first regeneration (decomp 640fac0), beat folders
#       in fork-states/beats/<path>, no lead-in (what use_fork() gives by
#       default, for fork_states.py).
# The module default (FORK False) stays the legacy app for importers that
# have not chosen; every command line chooses.
FORK = False
GENERATION = "base"
PHASE_LOCK = True                 # phase generation: replay the v2.6.3 lead-in and tail
FORK_REFS = ROOT / "build/fork_refs"
FORK_PHASE_BEATS = FORK_STATES / "phase/beats"


def use_fork(generation: str = "base", phase_lock: bool = True) -> None:
    global FORK, GENERATION, PHASE_LOCK
    FORK, GENERATION, PHASE_LOCK = True, generation, phase_lock


def use_legacy() -> None:
    global FORK
    FORK = False


def add_emulator_args(ap: argparse.ArgumentParser) -> None:
    """--emulator / --generation / --no-phase-lock, shared by the capture tools."""
    ap.add_argument("--emulator", choices=["fork", "legacy"], default=DEFAULT_BACKEND,
                    help="fork (default; env EXTERMINATION_PCSX2) = the agent-debug fork with the "
                         "fork-states manifest's states; legacy = the v2.6.3 app (until it is retired)")
    ap.add_argument("--generation", choices=["phase", "base"], default="phase",
                    help="fork: phase (default) = the phase-locked states, outputs in build/fork_refs/ "
                         "with a manifest per set; base = the first regeneration (fork-states/beats/)")
    ap.add_argument("--no-phase-lock", action="store_true",
                    help="fork, phase generation: start on the saved frame instead of replaying the "
                         "v2.6.3 capture's lead-in and tail")
    ap.add_argument("--fork-mtvu", choices=["off", "on", "ini"], default="off",
                    help="fork: VU1 on its own thread.  off (default) = VU1 on the EE thread: GS memory "
                         "at a loop top is then the same in every run (with it on, two runs with "
                         "identical EE state differed in GS memory; EE state is the same either way)")


def apply_emulator_args(a) -> None:
    import pcsx2_session
    if a.emulator == "fork":
        use_fork(a.generation, not a.no_phase_lock)
        mt = getattr(a, "fork_mtvu", "off")
        pcsx2_session.FORK_MTVU = None if mt == "ini" else (mt == "on")
    else:
        use_legacy()


def resumable(state: Path) -> Path:
    """pcsx2_session.snapshot() derives the slot file name from the source
    state's name, so a beat's state.p2s is resumed through a correctly named
    copy next to it (never inside the sstates directory).  Fork snapshots are
    saved by path, so fork mode resumes the state itself."""
    if FORK or state.name.startswith(SERIAL):
        return state
    folder = OUT / "_resume" / state.parent.name
    folder.mkdir(parents=True, exist_ok=True)
    copy = folder / f"{SERIAL}.resume.p2s"
    shutil.copyfile(state, copy)
    return copy


def pcsx2_session_lane() -> int | None:
    """The parallel lane this process runs in (tools/route_lanes.py), or None."""
    import pcsx2_session
    return pcsx2_session.FORK_LANE


def wait_for_free_emulator(timeout: float = 600.0) -> None:
    """The Pine socket and DebugServer port are global: never start a second
    emulator while another session (any lane) is running one."""
    import subprocess
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        # Anchored to the start of the command line: a shell whose command
        # text merely mentions the emulator path (another lane waiting for the
        # lock, a pgrep in a script) is not an emulator.
        if subprocess.run(["pgrep", "-f", "^[^ ]*PCSX2.app/Contents/MacOS/PCSX2"],
                          capture_output=True).returncode != 0:
            return
        time.sleep(2)
    raise RuntimeError("another PCSX2 session is still running")


class RouteSession(ForkSession):
    """The session the capture tools subclass, on either emulator (FORK).

    Legacy (v2.6.3): pcsx2_session.OriginalSession with a longer
    frame-boundary timeout.  Right after -statefile some states take longer
    than the default 5 s to reach the main-loop top the first time (observed
    on several beat snapshots); the per-frame timeout is only a fault
    detector.

    Fork: pcsx2_session.ForkSession.  The tools' own subclasses (route_census,
    c7cap_*, load_wait_probe, sfx_request_probe, gs_conformance) keep their
    v1 stepping loops (resume, poll status, service their breakpoints): on
    the fork, self.debug is pcsx2_session.ForkV1Debug, which turns resume into
    a run to the next loop top (stopping earlier at their v1 breakpoints) and
    answers status from the fork's state.

    The class derives from ForkSession so one subclass serves both; the
    emulator is chosen when the instance is made (route_capture.FORK)."""

    boundary_timeout = 30.0     # beat 15 raises it: Roger's departure plays an FMV inside one frame

    def __init__(self, state, log_dir=None, **kw):
        self._fork = FORK
        if self._fork:
            ForkSession.__init__(self, state, log_dir=log_dir, **kw)
        else:
            OriginalSession.__init__(self, state, log_dir=log_dir, **kw)

    def _start(self):
        return ForkSession._start(self) if self._fork else OriginalSession._start(self)

    def _paused(self) -> bool:
        return ForkSession._paused(self) if self._fork else OriginalSession._paused(self)

    def write(self, address: int, data: bytes) -> None:
        return ForkSession.write(self, address, data) if self._fork else OriginalSession.write(self, address, data)

    def snapshot(self, out_dir, slot=None) -> dict:
        return (ForkSession.snapshot(self, out_dir, slot) if self._fork
                else OriginalSession.snapshot(self, out_dir, slot))

    def close(self) -> None:
        return ForkSession.close(self) if self._fork else OriginalSession.close(self)

    def _resume_to_boundary(self, timeout: float | None = None) -> None:
        if self._fork:
            ForkSession._resume_to_boundary(self, timeout)
        else:
            OriginalSession._resume_to_boundary(self, timeout=timeout or self.boundary_timeout)


class RetrySession:
    """Context manager: start the hidden emulator, retrying start-up races
    (DebugServer not yet listening, first boundary missed)."""

    def __init__(self, state: Path, log_dir: Path | None = None, attempts: int = 6):
        self.state, self.log_dir, self.attempts = state, log_dir or OUT / "logs", attempts
        self.session: OriginalSession | None = None

    def __enter__(self) -> OriginalSession:
        for attempt in range(self.attempts):
            if not (FORK and pcsx2_session_lane() is not None):   # a route_lanes.py lane: the parent's
                wait_for_free_emulator(3600.0 if FORK else 600.0)  # other lanes are running emulators
            session = RouteSession(self.state, log_dir=self.log_dir)
            try:
                self.session = session.__enter__()
                return self.session
            except (RuntimeError, TimeoutError, OSError, EOFError) as exc:
                print(f"start attempt {attempt + 1} failed: {exc}", flush=True)
                time.sleep(2)
        raise RuntimeError(f"emulator did not start from {self.state}")

    def __exit__(self, *exc) -> None:
        if self.session is not None:
            self.session.close()


def open_session(state: Path, log_dir: Path | None = None, attempts: int = 6) -> RetrySession:
    return RetrySession(state, log_dir, attempts)


def slot_path(slot: str) -> Path:
    if FORK:
        return fork_state(slot, GENERATION)
    return SSTATES / f"{SERIAL}.{slot}.p2s"


# ---------------------------------------------------------------------------
# Beats.  Each beat starts from its source (a user slot or an earlier beat's
# snapshot), drives the route with pad input and ends idle with control back.

def in_control(row: dict) -> bool:
    """Ordinary gameplay: selector 3B8D == 0, player action mode 0, no status."""
    return row["spad"][2:4] == "00" and row["m1F0"] == 0 and row["ui"][2:4] == "00"


def settle(r: Route, frames: int = 30) -> None:
    r.idle(frames)
    r.until(lambda row: in_control(row) and row["clip"] == 0, 600)


def face(r: Route, yaw: float, tol: float = 0.12, limit: int = 40) -> None:
    """Turn the player toward body yaw `yaw` (X = sin, Z = cos, FIRST_CONTROL.md)
    with a short stick tap; the original turns in place before the walk blend
    ends, so this translates the player very little."""
    for _ in range(limit):
        row = r.rows[-1]
        diff = (yaw - row["yaw"] + math.pi) % (2 * math.pi) - math.pi
        if abs(diff) <= tol:
            break
        px, _py, pz = row["pos"]
        r.stick_toward(px + 100 * math.sin(yaw), pz + 100 * math.cos(yaw), 0.6)
        r.step(1)
    settle(r, 10)


def beat_panel_no_battery(r: Route) -> dict:
    # Panel owner r18 at (240,245,232.8), yaw -pi; 001B6F00 aligns the player
    # to about (239.7, y, 223.8) facing +z.  Approach from the south.
    r.goto(239.7, 216.0, tol=1.0)
    r.goto(239.7, 222.0, tol=0.8, magnitude=0.5, stuck_ok=True)
    settle(r, 20)
    r.press("CROSS", 2)
    r.until(lambda row: not in_control(row), 60)
    r.until(in_control, 1500)
    settle(r)
    return {"what": "Cross at the power panel with no battery item (0x810C7F == 0)"}


def beat_battery(r: Route) -> dict:
    # Battery pickup g0.0 (00219550, item 0x1B) at (211.6, 229.9, 227.2).
    r.goto(211.6, 227.2, tol=5.0, stuck_ok=True)
    settle(r, 20)
    r.press("CROSS", 2)
    r.until(lambda row: row["battery_item"] == 1 and row["ui"][2:4] == "03", 600)
    # ITEM page message runs out, then the page browses (ui[4..5] == 05 01).
    r.until(lambda row: row["ui"][8:12] == "0501", 900)
    r.idle(20)
    r.press("TRIANGLE", 2)
    r.until(in_control, 300)
    settle(r)
    return {"what": "Cross at battery pickup g0.0 (take clip, status ITEM auto-open, Triangle exit)"}


def use_elevator_terminal(r: Route) -> None:
    # Elevator owner r19 (overlay 0x827B10) at (224,230,250.7); its script
    # aligns the player to (222, y, 250) facing yaw -1.3037 (-x).  Approach
    # along -x so the use scan's facing gate passes.
    r.goto(229.0, 250.4, tol=1.0, stuck_ok=True)
    r.goto(223.5, 250.4, tol=0.6, magnitude=0.5, stuck_ok=True)
    settle(r, 20)
    face(r, -1.3037)
    r.press("CROSS", 2)
    r.until(lambda row: not in_control(row), 60)


def beat_elevator_refusal(r: Route) -> dict:
    use_elevator_terminal(r)
    r.until(in_control, 1500)
    settle(r)
    return {"what": "Cross at the elevator terminal with area power bit 0x81084C&0x80 clear (refusal)"}


def use_panel(r: Route) -> None:
    r.goto(239.7, 216.0, tol=1.0, stuck_ok=True)
    r.goto(239.7, 222.0, tol=0.8, magnitude=0.5, stuck_ok=True)
    settle(r, 20)
    face(r, 0.0)
    r.press("CROSS", 2)
    r.until(lambda row: not in_control(row), 60)


def beat_panel_power(r: Route) -> dict:
    use_panel(r)
    # Script 2477A0 -> 00157F60 posts B0=1/B1=0x82: status BATTERY page, which
    # opens on its "consume 2 units ... Proceed?" prompt with No selected.
    r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][8:10] == "05"
            and row["ui"][10:12] == "04", 900)
    settle_frames = 30
    r.idle(settle_frames)
    r.press("LEFT", 2, after=10)      # cursor to Yes
    r.press("CROSS", 2)
    r.until(lambda row: row["power"] & 0x80, 900)
    r.until(in_control, 900)
    settle(r)
    return {"what": "Cross at the power panel with the battery: BATTERY prompt, LEFT to Yes, Cross; power bit set"}


def beat_elevator_ride(r: Route) -> dict:
    use_elevator_terminal(r)
    r.until(in_control, 1500)
    settle(r)
    return {"what": "Cross at the powered elevator terminal: script 82A750, clip 0x47, 150-call carry to the lower floor"}


def climb(r: Route, yaw: float) -> None:
    """Use (Cross) against a ledge: no interaction candidate wins, so the
    player's ledge fallback runs (action +0x1F0 = 8, clips 0x70/0x78/0x8C)."""
    settle(r, 10)
    face(r, yaw)
    r.press("CROSS", 2)
    r.until(lambda row: row["m1F0"] == 8, 30)
    r.until(in_control, 300)
    settle(r, 10)


def beat_boxes(r: Route) -> dict:
    # Crates 001551B0: r4 alone at (228.8, 189.8, 292.8); r3 on r5 at
    # (214.7, 203.8 / 189.8, 292.8).  The upper ledge (floor ~218-220) starts
    # at z ~ 297 behind the stack.
    r.goto(228.8, 283.0, tol=1.0, stuck_ok=True)
    r.goto(228.8, 292.0, tol=0.5, magnitude=0.5, stuck_ok=True)
    climb(r, 0.0)                      # onto r4, y 203.8
    r.goto(222.0, 288.3, tol=0.5, magnitude=0.5, stuck_ok=True)
    climb(r, -math.pi / 2)             # onto r3, y 217.8
    r.goto(219.2, 303.0, tol=1.0, magnitude=0.5, stuck_ok=True)
    settle(r)
    return {"what": "walk to the crates below the elevator, Cross-climb r4 then r3, step onto the upper ledge"}


def beat_hill_slide(r: Route) -> dict:
    # Down-slope from the ledge (floor ~218 at (240,315)) to the low ground
    # (~185 at (260,355)).  Hold the stick down the hill; the player enters
    # action +0x1F0 = 0x30 (slide, clips 94/97/100) and skids out on landing.
    r.goto(240.0, 312.0, tol=1.5, stop=False)
    for _ in range(300):
        r.stick_toward(262.0, 356.0)
        row = r.step(1)
        if row["m1F0"] == 0x30:
            break
    else:
        raise TimeoutError("no slide: " + summary(r.rows[-1]))
    for _ in range(400):
        r.stick_toward(262.0, 356.0)
        row = r.step(1)
        if row["m1F0"] != 0x30:
            break
    r.set_pad(0)
    settle(r)
    return {"what": "walk off the upper ledge down the short hill: slide action 0x30 to the low ground"}


def walk_path(r: Route, points, tol: float = 2.0, until=None, limit: int = 600) -> bool:
    """Follow waypoints with the stick; stop early (stick released) when
    `until(row)` holds.  Returns True when `until` fired."""
    for x, z in points:
        history: list[tuple[float, float]] = []
        for _ in range(limit):
            if until is not None and until(r.rows[-1]):
                r.set_pad(0)
                return True
            if r.stick_toward(x, z) <= tol:
                break
            row = r.step(1)
            history.append((row["pos"][0], row["pos"][2]))
            if len(history) > 45 and math.hypot(history[-1][0] - history[-45][0],
                                                history[-1][1] - history[-45][1]) < 0.3:
                break      # blocked: continue with the next waypoint
    return until is not None and until(r.rows[-1])


def trigger_started(row: dict) -> bool:
    return row["trigger_r17"]["h"][8:10] == "01"


def beat_truck_preview(r: Route) -> dict:
    # Trigger 0x8251E0 (r17): state 4 tests player X/Z against
    # 312<x<336 x 413<z<427 or 319<x<336 x 390<z<427, then starts script
    # 0x8292C0 through 001BA1A0 and polls it (001BA1F0) until it returns.
    if not walk_path(r, [(280, 392), (300, 400), (328, 412)], until=trigger_started):
        raise TimeoutError("trigger did not start: " + summary(r.rows[-1]))
    r.until(lambda row: row["story792"] == 1, 900)
    r.until(in_control, 300)
    settle(r)
    return {"what": "walk into the truck trigger band: camera preview script 0x8292C0 (letterbox), D_00810792 = 1"}


def beat_truck_crossing(r: Route) -> dict:
    # The truck (r16, overlay 0x823FF0) bridges the pit east of the trigger
    # area.  Standing on it (player +0x214 = truck, truck +0x0D = 9) arms it:
    # 47-tick shake, 119-beat fall, D_00810792 = 0xFF.  Cross it and step off
    # its north side onto the low ground (~185) at about (362, 370).
    walk_path(r, [(345, 390), (365, 368)], tol=2.0)
    r.set_pad(0)
    r.until(lambda row: row["story792"] == 0xFF, 600)
    settle(r)
    return {"what": "cross the truck bridge and step off its north side; the truck arms, shakes and falls into the pit"}


def beat_fence_door(r: Route) -> dict:
    # Distant door r0 (001BC350, class 5/subtype 3) in the fence at
    # (423, 184.8, 290.3): Use starts the door script (clip 0x45), posts the
    # same-area room move B7=2/B8=2 and fades; 001BC150 re-places the player
    # at spawn entry 2 on the far side.
    walk_path(r, [(395, 340), (412, 300), (416, 291)], tol=1.5)
    r.set_pad(0)
    settle(r, 10)
    r.goto(421.0, 290.3, tol=0.6, magnitude=0.5, stuck_ok=True)
    settle(r, 10)
    r.press("CROSS", 2)
    r.until(lambda row: not in_control(row), 60)
    r.until(lambda row: row["req"][14:16] == "02", 400)
    r.until(in_control, 600)
    settle(r, 60)
    return {"what": "Cross at the fence door: door script, room move to entry 2 behind the fence, area title card"}


def ladder(r: Route) -> None:
    """Use (Cross) at an attr-0x32 ladder face: grab (action 0x15, clip 0xE3),
    climb with stick up (0x17, clips 0xE8/0xEA), top out (0x18, clip 0xF0)."""
    r.press("CROSS", 2)
    r.until(lambda row: row["m1F0"] == 0x17, 90)
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["m1F0"] not in (0x15, 0x17, 0x18), 900, 0, 0x7F, 0x00)
    r.set_pad(0)


def cutscene(row: dict) -> bool:
    return row["spad"][2:4] != "00"


def beat_cage_roof_roger(r: Route) -> dict:
    # Cage ladders (attr 0x32 faces): A at z 287.5 (x 356-363, 185 -> 225),
    # B at z 257.5 (225 -> 265).  On the roof, director r12 (0x8253F0, D_00810813
    # = 0) sees Y in (260, 280] inside quad 0x82ABE0 (x 335..385, z 228..255) and
    # starts script 0x8294C0; that sets D_00810793 = 1, so Roger r8 (0x8237E0)
    # takes its alternate branch and runs script 0x828990 (the conversation).
    walk_path(r, [(360, 320), (360, 296)], tol=1.0)
    r.set_pad(0)
    settle(r, 5)
    r.goto(360.0, 293.5, tol=0.5, magnitude=0.4, stuck_ok=True)
    settle(r, 5)
    face(r, math.pi)
    ladder(r)                                   # A: up to the cage floor (225)
    r.goto(359.8, 262.0, tol=0.6, magnitude=0.5, stuck_ok=True)
    settle(r, 5)
    face(r, math.pi)
    r.press("CROSS", 2)                         # B: up to the roof (265)
    r.until(lambda row: row["m1F0"] == 0x17, 90)
    r.set_pad(0, 0x7F, 0x00)
    r.until(cutscene, 900, 0, 0x7F, 0x00)
    r.set_pad(0)
    r.until(in_control, 5000)
    settle(r, 60)
    return {"what": "cage ladders A and B to the roof: director beat 0 (script 0x8294C0) and Roger's conversation (0x828990)"}


def beat_crevice_prompt(r: Route) -> dict:
    # Bridge east to the tank, climb it (clip 0x79), leave by the junction onto
    # the south-west pipe, follow it to the 270 plateau, and climb the east pipe
    # end.  Director beat 1 (D_00810813 = 0x10/0x11): Y >= 275 inside quad
    # 0x82AC20 starts script 0x829A40 ("I have to jump that crevice.").
    walk_path(r, [(385, 238), (407, 240)], tol=1.0)
    r.set_pad(0)
    settle(r, 5)
    climb(r, math.pi / 2)
    walk_path(r, [(420, 262), (416, 270), (412, 276), (405, 285), (401, 300), (410, 312),
                  (430, 330), (450, 347), (462, 355), (470, 340), (470, 292)], tol=1.5, until=cutscene)
    if not cutscene(r.rows[-1]):
        r.set_pad(0)
        settle(r, 5)
        face(r, math.pi)
        r.press("CROSS", 2)
        r.until(lambda row: row["m1F0"] == 8 or cutscene(row), 30)
        r.until(lambda row: cutscene(row) or in_control(row), 400)
    if not cutscene(r.rows[-1]):
        walk_path(r, [(475, 284), (490, 286)], tol=1.5, until=cutscene)
    r.until(cutscene, 60)
    r.until(in_control, 5000)
    settle(r, 60)
    return {"what": "tank climb, pipes to the east plateau, pipe-end climb: director beat 1 (script 0x829A40)"}


def beat_crevice_jump(r: Route) -> dict:
    # Running jump (Cross at the edge while running: action 0x0C, clips
    # 0x69/0x6B) across the crevice from the plateau (z ~ 248) to the north
    # block (270 at z <= 205).
    walk_path(r, [(485, 275), (477, 262)], tol=1.0)
    r.set_pad(0)
    settle(r, 5)
    face(r, math.pi)
    for _ in range(200):
        if r.rows[-1]["pos"][2] <= 249.5:
            break
        r.stick_toward(477, 150)
        r.step(1)
    r.set_pad(PAD["CROSS"], r.pad_state[1], r.pad_state[2])
    r.step(2)
    r.set_pad(0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] == 0x0C, 10, 0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] not in (0x0C, 0x0F), 200, 0, r.pad_state[1], r.pad_state[2])
    r.set_pad(0)
    settle(r)
    if r.rows[-1]["pos"][2] > 210:
        raise RuntimeError("jump fell short: " + summary(r.rows[-1]))
    return {"what": "running jump across the crevice onto the north block (270)"}


def beat_east_tower(r: Route) -> dict:
    # From the north block, Cross facing -x at the east tower's north end
    # (x 444.2, z 179.8) is a high ledge climb (clips 0x70/0x79/0x8C) onto the
    # tower top (290).  Director beat 2 (D_00810813 = 0x20): Y >= 285 inside
    # quad 0x82AC60 (x 410..439, z 175..203) starts script 0x829CC0.
    r.goto(445.0, 178.0, tol=0.7, magnitude=0.6, stuck_ok=True)
    r.set_pad(0)
    settle(r, 5)
    face(r, -math.pi / 2)
    r.press("CROSS", 2)
    r.until(lambda row: row["m1F0"] == 8, 30)
    r.until(cutscene, 400)
    r.until(in_control, 3000)
    settle(r, 60)
    return {"what": "high ledge climb onto the east tower top: director beat 2 (script 0x829CC0)"}


def beat_roger_encounter(r: Route) -> dict:
    # Running jump west from the east tower top (x ~ 410) across the gap to
    # the west tower top.  In mid-air the player crosses x < 358 inside Roger's
    # XZ quad 0x82AB80 (x 330..358, z 160..204); Roger r8's ordinary branch
    # (D_00810793 != 1) starts script 0x8283D0 (bank 96 encounter).
    r.goto(436.0, 190.0, tol=0.8, magnitude=0.6, stuck_ok=True)
    r.set_pad(0)
    settle(r, 5)
    face(r, -math.pi / 2)
    for _ in range(200):
        if r.rows[-1]["pos"][0] <= 411.5:
            break
        r.stick_toward(300.0, 190.0)
        r.step(1)
    r.set_pad(PAD["CROSS"], r.pad_state[1], r.pad_state[2])
    r.step(2)
    r.set_pad(0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] == 0x0C, 10, 0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["roger_r8"]["s1F0"][16:24] == "d0838200", 120, 0,
            r.pad_state[1], r.pad_state[2])
    r.set_pad(0)
    r.until(cutscene, 60)
    r.until(in_control, 6000)
    settle(r, 60)
    return {"what": "running jump from the east tower top to the west tower top: Roger's automatic encounter (script 0x8283D0, bank 96)"}


# -- beat 15: the level exit ---------------------------------------------------
# Beat 15 samples extra fields on top of SPANS (so the traces of beats 00..14
# keep their exact row format): the fan pair, the flag bytes D_00810758.., the
# full area/sub/entry bytes, the three task-slot records, the loader flags and
# the resident overlay header.
FAN_R1, FAN_R2 = 0x7A73A0, 0x7A7690     # overlay 0x827630, records [1] and [2]
EXIT_SPANS = [
    ("fan_r1:a", FAN_R1, 0x10), ("fan_r1:b", FAN_R1 + 0x20, 0x20), ("fan_r1:c", FAN_R1 + 0xC0, 0x10),
    ("fan_r2:a", FAN_R2, 0x10), ("fan_r2:b", FAN_R2 + 0x20, 0x20), ("fan_r2:c", FAN_R2 + 0xC0, 0x10),
    ("flags758", 0x810758, 0x8),        # D_00810758[0] (0xFF = Roger's departure done)
    ("area4", 0x810700, 0x4),           # 700 area, 701 sub, 702 entry
    ("slots", 0x28A750, 0x60),          # task slots 0..2 (slot 2 = module loader 001FF0D0)
    ("bd8", 0x275BD8, 0x4),             # D_00275BD8 (module load pending)
    ("cd157", 0x282154, 0x4),           # byte 3 = D_00282157 (loader read gate)
    ("ovl", 0x823500, 0x8),             # overlay magic + id (9 = AREA11)
    ("movie", 0x275C78, 0x4),           # D_00275C78 movie select
    ("movie_req", 0x821058, 0x4),       # D_00821058 movie request
]


class ExitSampler(Sampler):
    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = SPANS + EXIT_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))

    def raw(self) -> dict[str, bytes]:
        for _ in range(5):
            data = self.s.pine.request(self.body)
            out, off = {}, 0
            for name, _a, n in self.spans:
                out[name] = data[off:off + n]
                off += n
            if out["counter"] == out["counter2"]:
                return out
        raise RuntimeError("inconsistent sample")


def decode_exit(r: dict[str, bytes]) -> dict:
    row = decode(r)
    for name in ("fan_r1", "fan_r2"):
        a, b, c = r[name + ":a"], r[name + ":b"], r[name + ":c"]
        row[name] = {"h": a.hex(), "phase": a[5], "timer": struct.unpack_from("<h", b, 8)[0],
                     "flags2": struct.unpack_from("<H", b, 0xE)[0], "spin": round(f32(b, 0x18), 7),
                     "rotz": round(f32(c, 8), 6)}
    row["flags758"] = r["flags758"].hex()
    row["area4"] = r["area4"].hex()
    row["slots"] = r["slots"].hex()
    row["bd8"] = r["bd8"][0]
    row["cd157"] = r["cd157"][3]
    row["ovl"] = r["ovl"].hex()
    row["movie"] = r["movie"].hex()
    row["movie_req"] = r["movie_req"].hex()
    return row


def use_exit_sampler(r: Route) -> None:
    """Switch a Route to the beat-15 sampler; row 0 is re-read (no frame has
    run yet, so it is the same machine state)."""
    sampler = ExitSampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_exit(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)
    # Wall-clock time per frame is kept out of the rows (they must stay
    # reproducible); frames longer than 2 s (the FMV frame) go into the meta.
    r.slow_frames = []
    plain_step = r.s.step

    def timed_step(n: int = 1, **pad) -> list[int]:
        t0 = time.monotonic()
        out = plain_step(n, **pad)
        dt = time.monotonic() - t0
        if dt > 2.0:
            r.slow_frames.append({"f": r.frame_index + 1, "seconds": round(dt, 1)})
        return out
    r.s.step = timed_step


def fan_slow_window(row: dict) -> bool:
    """Fan r2 at the start of its 60-tick wait (phase 1, spin 0): the
    slow arm has no hit, so the player can pass under it."""
    fan = row["fan_r2"]
    return fan["phase"] == 1 and fan["timer"] >= 55


def beat_level_exit(r: Route) -> dict:
    # From the beat-14 release point (338, 289.75, 192) on the west tower top:
    # fan record [2] (0x827630, +0x2E == 1) tests the player box Y (280, 320),
    # X (318, 340); Z < 156 is the exit-or-bit.  With D_00810758[0] != 0xFF
    # (Roger's departure not yet done) it sets D_008107D8 |= 0x80, Roger r8's
    # controller starts departure script 0x828A10 (movie selector 1 plays inside
    # one frame), and on its completion the controller (runtime 0x823C80)
    # requests 001B0C60(1, 0, 4): AREA01 sub 0, spawn entry 4.  Z < 166.5 while
    # the fan spins fast is the hit, so wait outside that band for the slow window.
    use_exit_sampler(r)
    r.s.boundary_timeout = 900.0
    r.goto(331.0, 177.0, tol=1.5, stuck_ok=True)
    r.goto(329.5, 172.0, tol=0.8, magnitude=0.5, stuck_ok=True)
    settle(r, 5)
    r.until(fan_slow_window, 400)
    walk_path(r, [(329.5, 150.0)], tol=1.0,
              until=lambda row: row["d2"][:2] not in ("01", "00") or row["req"][16:18] != "00")
    r.set_pad(0)
    r.until(lambda row: row["req"][16:18] != "00", 2000)      # B8 set: area change posted
    r.until(lambda row: row["area4"][:2] == "01", 3000)
    r.until(lambda row: row["slots"][22:24] == "01" and in_control(row), 6000)
    settle(r, 60)
    return {"what": "walk under fan r2 (slow window) past z 156: D_008107D8 |= 0x80, Roger's departure "
                    "script 0x828A10, 001B0C60(1, 0, 4), AREA01 sub 0 entry 4 arrival",
            "slow_frames": r.slow_frames}


BEATS = [
    ("00_panel_no_battery", "04", beat_panel_no_battery),
    ("01_battery", "04", beat_battery),
    ("02_elevator_refusal", "01_battery", beat_elevator_refusal),
    ("03_panel_power", "02_elevator_refusal", beat_panel_power),
    ("04_elevator_ride", "03_panel_power", beat_elevator_ride),
    ("05_boxes", "04_elevator_ride", beat_boxes),
    ("06_hill_slide", "05_boxes", beat_hill_slide),
    ("07_truck_preview", "06_hill_slide", beat_truck_preview),
    ("08_truck_crossing", "07_truck_preview", beat_truck_crossing),
    ("09_fence_door", "08_truck_crossing", beat_fence_door),
    ("10_cage_roof_roger", "08_truck_crossing", beat_cage_roof_roger),
    ("11_crevice_prompt", "10_cage_roof_roger", beat_crevice_prompt),
    ("12_crevice_jump", "11_crevice_prompt", beat_crevice_jump),
    ("13_east_tower", "12_crevice_jump", beat_east_tower),
    ("14_roger_encounter", "13_east_tower", beat_roger_encounter),
    ("15_level_exit", "14_roger_encounter", beat_level_exit),
]
# Beats that `--beats all` leaves out: they run only when named explicitly.
# 15_level_exit costs minutes of host time (an FMV inside one frame) and ends
# in AREA01, outside the first level proper.
OPT_IN_BEATS = {"15_level_exit"}


# ---------------------------------------------------------------------------
# AREA01 (the second level, an underground-tunnel area), opt-in beat group `a01`.
# The second level: from the beat-15 end snapshot (AREA01 sub 0 spawn entry 4,
# placement table 0x82BD50) through the locked shaft door, the control-room
# NPC and back to the shaft door, whose opening is the area change to AREA00
# sub 0 entry 0.  Outputs go to build/s87/route_a01/<beat>/ (ignored).  The
# route, every beat and the census delta are described in the port's
# docs/SECOND_LEVEL_ROUTE.md.  None of these beats run under `--beats all`;
# `--beats a01` runs the whole group in order, or name beats one by one.
OUT_A01 = ROOT / "build/s87/route_a01"

# Owner nodes of the AREA01 sub-0 load (pool order is stable across every
# state captured from the beat-15 arrival; the deferred g0.1 node is freed by
# its take and reused).  Names give the placement record [n] of 0x82BD50 or
# the node address, plus what the capture showed.
A01_OWNERS = {
    "npc_r36": 0x7B0390,          # overlay 0x825350 (class 10): the control-room NPC
    "r37_826CF0": 0x7B0680,       # overlay 0x826CF0 (class 8) at the console
    "shaft_door_r12": 0x7ABD10,   # overlay 0x823580 (class 5): shaft-bottom door, id 0|0x80
    "r13_158D30": 0x7AC000,       # 00158D30 (class 8) beside the shaft door
    "door_r15": 0x7AC5E0,         # 001BC350 room-move door id 2 (control room)
    "doc_g0_1": 0x7A5930,         # 00219550 deferred g0.1, item 0x48 (DATA BASE page)
    "n7A70B0_826D40": 0x7A70B0,   # overlay 0x826D40 at (-45, -3, -1140)
    "n7A7690_826D40": 0x7A7690,   # overlay 0x826D40 at (-45, 37, -900)
    "n7A7C70_826D40": 0x7A7C70,   # overlay 0x826D40 at (30, 17, -1020)
    "r41_8261A0": 0x7B1240,       # overlay 0x8261A0 at (0, 3, -525)
    "r42_8261A0": 0x7B1530,       # overlay 0x8261A0 at (0, 3, -315)
}
A01_BASE_SPANS = [sp for sp in SPANS if ":" not in sp[0]]
A01_SPANS = A01_BASE_SPANS + [
    ("story758", 0x810758, 0x8),        # D_00810758..5F (759 = 0xFF after the NPC's second talk)
    ("area4", 0x810700, 0x4),           # area, sub, entry, previous area
    ("slots", 0x28A750, 0x60),          # task slots 0..2
    ("bd8", 0x275BD8, 0x4),
    ("ovl", 0x823500, 0x8),             # overlay magic + id (2 = AREA01, 1 = AREA00)
    ("taken", 0x810860, 0x40),          # taken-bit bytes
    ("docs", 0x810D00, 0x20),
    ("msgrec", 0x282210, 0x14),         # message service record index / countdown
]
for _name, _base in A01_OWNERS.items():
    A01_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                  (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                  (_name + ":c", _base + 0x10, 0x4)]
A01_EVENT_KEYS = ("hp", "d9", "f759", "area4", "ovl", "taken", "docs")


class A01Sampler(ExitSampler):
    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = A01_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))


def decode_a01(r: dict[str, bytes]) -> dict:
    row = decode(r, owners=A01_OWNERS)
    row["hp"] = round(f32(r["player"], 0x220), 3)       # player +0x220 health
    row["d9"] = r["d2"][1:2].hex()                     # D_008107D9 (shaft-door / NPC stage)
    row["f759"] = r["story758"][1:2].hex()
    row["story758"] = r["story758"].hex()
    row["area4"] = r["area4"].hex()
    row["slots"] = r["slots"].hex()
    row["bd8"] = r["bd8"][0]
    row["ovl"] = r["ovl"].hex()
    row["taken"] = r["taken"].hex()
    row["docs"] = r["docs"].hex()
    row["msgrec"] = r["msgrec"].hex()
    for name in A01_OWNERS:
        row[name]["cb"] = hex(struct.unpack("<I", r[name + ":c"])[0])
    return row


def use_a01_sampler(r: Route) -> None:
    """Switch a Route to the AREA01 sampler (row 0 re-read, same frame)."""
    sampler = A01Sampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_a01(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


def use_press(r: Route, pred, tries: int = 4, wait: int = 40, button: str = "CROSS") -> dict:
    """Press Use until `pred` holds.  The original does not take every press
    (a press right after the player settles can be missed); each retry is
    recorded in the inputs like any other press."""
    for _ in range(tries):
        r.press(button, 2)
        try:
            return r.until(pred, wait)
        except TimeoutError:
            settle(r, 10)
    raise TimeoutError(f"{button} not taken; last {summary(r.rows[-1])}")


def approach(r: Route, x: float, z: float, tol: float = 0.6, magnitude: float = 0.5,
             limit: int = 200) -> dict:
    """Half-stick walk to (x, z); stops when within tol, when blocked, or at limit."""
    for _ in range(limit):
        if r.stick_toward(x, z, magnitude) <= tol:
            break
        r.step(1)
    r.set_pad(0)
    settle(r, 10)
    return r.rows[-1]


def a01_hp(r: Route) -> float:
    return r.rows[-1]["hp"]


def a01_near(r: Route, x: float, z: float, tol: float, what: str) -> None:
    """Fail the beat when the closed loop did not arrive (a blocked walk)."""
    px, _py, pz = r.rows[-1]["pos"]
    if math.hypot(px - x, pz - z) > tol:
        raise RuntimeError(f"{what}: not reached; last {summary(r.rows[-1])}")


# Paths (world x, z).  The main line passes west of the DATA BASE pickup box
# at (12.5, -984.5) (taken only in side beat a01_s1).  The fire effects of the
# train room and the tunnel (boot 001E3D90 nodes) burn the player (action
# 0x3E; health -5 in a01_s3, kept at 6.5 units; the burn distance is open,
# since main-line passes at 11.8 and 12.4 units did not burn, see the port's
# docs/SECOND_LEVEL_ROUTE.md section 5); these waypoints keep clear of them, and the train room is crossed
# over the crate stack at x 8..23, z -709..-730.
A01_TO_CRATE_NORTH = [(33, -571), (26, -594), (20, -617), (30, -650), (33, -668), (33, -692), (18, -697)]
A01_MOUTH_TO_TUNNEL = [(-5, -785), (-12, -800), (-12, -812), (-26, -822), (-26, -848), (-14, -856),
                       (-14, -885), (14, -905), (15, -970)]
A01_TUNNEL_TO_LANDING = [(7, -975), (3, -988), (5, -1060), (5, -1160), (-12, -1175), (-26, -1185), (-30, -1212),
                         (-41, -1220), (-40.5, -1255), (-40.5, -1271.5)]
A01_LANDING_TO_MOUTH = [(-40.5, -1250), (-41, -1220), (-30, -1212), (-26, -1185), (-12, -1175), (5, -1160),
                        (5, -1060), (3, -990), (7, -975), (14, -960), (15, -905), (-14, -885), (-14, -856),
                        (-26, -848), (-26, -822), (-12, -812), (-12, -800), (-5, -785)]
A01_CRATE_NORTH_TO_DOOR = [(33, -692), (32, -668), (22, -632), (19, -612), (30, -585), (52, -563.5)]


def a01_crate_crossing(r: Route) -> None:
    """From north of the crate stack: Use facing -z at (15.4, -706.4) grabs
    the ledge (action 8, hang 0x10), stick forward pulls up (0x11) onto the
    top (y 28); walking on off the south edge falls (0x0B, 0x0F) to the floor."""
    walk_path(r, A01_TO_CRATE_NORTH, tol=1.2)
    approach(r, 15.4, -706.4)
    face(r, math.pi)
    use_press(r, lambda row: row["m1F0"] == 8)
    r.until(lambda row: row["m1F0"] == 0x10, 120)
    settle_frames = 0
    for _ in range(200):
        r.stick_toward(15.4, -740.0)
        row = r.step(1)
        settle_frames += 1
        if row["m1F0"] not in (8, 0x10, 0x11) and row["pos"][1] > 20:
            break
    else:
        raise TimeoutError("crate pull-up: " + summary(r.rows[-1]))
    for _ in range(300):
        r.stick_toward(16.5, -745.0)
        row = r.step(1)
        if row["pos"][1] < 1.5 and row["m1F0"] not in (0x0B, 0x0F) and row["pos"][2] < -735:
            break
    else:
        raise TimeoutError("crate drop: " + summary(r.rows[-1]))
    r.set_pad(0)
    settle(r, 10)
    walk_path(r, [(16, -770)], tol=1.5)


def a01_door_into_control(r: Route) -> None:
    """Room-move door r15 (001BC350, id 2) from the tunnel side: Use facing
    +x at (55, -563.5); the door script aligns the player to (55.5, -564)
    and re-places it at spawn entry 1 (64, -563) inside."""
    approach(r, 55.0, -563.5)
    face(r, math.pi / 2)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][4:6] == "01" and in_control(row), 900)
    settle(r, 20)


def a01_door_out_of_control(r: Route) -> None:
    r.goto(70.0, -563.0, tol=1.0, stuck_ok=True)
    approach(r, 66.0, -564.0)
    face(r, -math.pi / 2)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][4:6] == "02" and in_control(row), 900)
    settle(r, 20)


def a01_talk_npc(r: Route, limit: int = 6000) -> None:
    """NPC r36 (overlay 0x825350) at (81, 0, -521): Use from (76.1, -527.9)."""
    walk_path(r, [(76.5, -535), (76.1, -528)], tol=0.8)
    r.set_pad(0)
    settle(r, 10)
    px, _py, pz = r.rows[-1]["pos"]
    face(r, math.atan2(81.0 - px, -521.0 - pz))
    use_press(r, lambda row: not in_control(row))
    r.until(in_control, limit)
    settle(r, 30)


def a01_beat_train_room(r: Route) -> dict:
    use_a01_sampler(r)
    a01_crate_crossing(r)
    walk_path(r, [(-5, -785), (-12, -800), (-12, -812)], tol=1.2)
    r.set_pad(0)
    settle(r, 10)
    a01_near(r, -12, -812, 12, "tunnel mouth")
    return {"what": "AREA01 arrival -> train room: around the crates to the stack, ledge grab and pull-up, "
                    "drop off the south side, on to the tunnel mouth", "hp_end": a01_hp(r)}


def a01_beat_tunnel(r: Route) -> dict:
    use_a01_sampler(r)
    walk_path(r, A01_MOUTH_TO_TUNNEL[3:], tol=1.2)
    approach(r, 13.2, -977.2)
    a01_near(r, 13.2, -977.2, 6, "tunnel middle")
    return {"what": "tunnel mouth -> middle of the tunnel beside the DATA BASE pickup g0.1", "hp_end": a01_hp(r)}


def a01_beat_shaft_landing(r: Route) -> dict:
    use_a01_sampler(r)
    walk_path(r, A01_TUNNEL_TO_LANDING, tol=1.2)
    r.set_pad(0)
    settle(r, 10)
    a01_near(r, -40.5, -1271.5, 2.5, "shaft landing")
    return {"what": "lower tunnel (y -60) -> west stairs -> shaft landing (y -35) at the shaft door",
            "hp_end": a01_hp(r)}


def a01_beat_shaft_locked(r: Route) -> dict:
    use_a01_sampler(r)
    face(r, math.pi)
    use_press(r, lambda row: not in_control(row))
    r.until(in_control, 3000)
    settle(r, 30)
    if r.rows[-1]["d9"] != "80":
        raise RuntimeError("D_008107D9 not 0x80: " + summary(r.rows[-1]))
    return {"what": "Use at the shaft door r12 with D_008107D9 == 0: locked try, script 0x8298E0, D_008107D9 = 0x80"}


def a01_beat_return_north(r: Route) -> dict:
    use_a01_sampler(r)
    walk_path(r, A01_LANDING_TO_MOUTH, tol=1.2)
    walk_path(r, [(16, -770), (16.5, -738)], tol=1.0)
    approach(r, 16.5, -733.5)
    face(r, 0.0)
    climbed = False
    try:
        use_press(r, lambda row: row["m1F0"] in (8, 0x10), tries=2, wait=30)
        climbed = True
    except TimeoutError:
        pass
    if climbed:
        r.until(lambda row: row["m1F0"] not in (8,), 120)
        for _ in range(200):
            r.stick_toward(16.0, -690.0)
            row = r.step(1)
            if row["m1F0"] not in (8, 0x10, 0x11) and row["pos"][1] > 20:
                break
        for _ in range(300):
            r.stick_toward(16.0, -697.0)
            row = r.step(1)
            if row["pos"][1] < 1.5 and row["m1F0"] not in (0x0B, 0x0F) and row["pos"][2] > -706:
                break
        r.set_pad(0)
        settle(r, 10)
    else:                               # the middle lane between the fires
        walk_path(r, [(-12, -752), (-6, -738), (-6, -702), (15, -697)], tol=1.2)
    walk_path(r, A01_CRATE_NORTH_TO_DOOR, tol=1.2)
    a01_door_into_control(r)
    return {"what": "shaft landing -> tunnel -> train room -> control-room door (room move to entry 1)",
            "crate_climb_north": climbed, "hp_end": a01_hp(r)}


def a01_beat_npc_bridge_talk(r: Route) -> dict:
    use_a01_sampler(r)
    a01_talk_npc(r)
    if r.rows[-1]["d9"] != "81":
        raise RuntimeError("D_008107D9 not 0x81: " + summary(r.rows[-1]))
    return {"what": "Use at NPC r36 with D_008107D9 == 0x80: script 0x829FA0, D_00810759 = 0xFF, "
                    "D_008107D9 = 0x81"}


def a01_beat_return_south(r: Route) -> dict:
    use_a01_sampler(r)
    a01_door_out_of_control(r)
    a01_crate_crossing(r)
    walk_path(r, A01_MOUTH_TO_TUNNEL + A01_TUNNEL_TO_LANDING, tol=1.2)
    r.set_pad(0)
    settle(r, 10)
    a01_near(r, -40.5, -1271.5, 2.5, "shaft landing")
    return {"what": "control room -> tunnel side -> train room crates -> tunnel -> shaft landing",
            "hp_end": a01_hp(r)}


def a01_beat_level_exit(r: Route) -> dict:
    use_a01_sampler(r)
    face(r, math.pi)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][:2] == "00", 1500)
    # Control flickers back for one frame at the arrival rebuild before the
    # AREA00 arrival script takes it again; wait for 30 frames of control.
    for _ in range(4):
        r.until(lambda row: row["slots"][22:24] == "01" and in_control(row), 8000)
        r.idle(30)
        if all(in_control(row) for row in r.rows[-30:]):
            break
    else:
        raise TimeoutError("AREA00 arrival: control not kept; " + summary(r.rows[-1]))
    settle(r, 30)
    return {"what": "Use at the shaft door with D_008107D9 == 0x81: door opens, area change to AREA00 sub 0 "
                    "entry 0, AREA00 arrival script, control"}


def a01_beat_npc_first_talk(r: Route) -> dict:
    use_a01_sampler(r)
    walk_path(r, [(52, -563.5)], tol=1.0)
    a01_door_into_control(r)
    a01_talk_npc(r)
    return {"what": "arrival -> control-room door -> Use at NPC r36 with D_008107D9 == 0: script 0x829E60"}


def a01_beat_sentry_doc(r: Route) -> dict:
    use_a01_sampler(r)
    approach(r, 13.2, -978.5)
    px, _py, pz = r.rows[-1]["pos"]
    face(r, math.atan2(12.5 - px, -984.5 - pz))
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][4:6] == "02" and row["ui"][6:8] == "04", 600)
    r.idle(30)
    r.press("TRIANGLE", 2)
    r.until(in_control, 300)
    settle(r)
    return {"what": "Use at the DATA BASE pickup g0.1 (item 0x48): take, DATA BASE page, Triangle exit"}


def a01_take_item(r: Route, x: float, z: float, stand_x: float, yaw: float) -> dict:
    approach(r, stand_x, z)
    face(r, yaw)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: in_control(row) or row["ui"][2:4] == "03", 900)
    if not in_control(r.rows[-1]):
        r.idle(90)
        r.press("TRIANGLE", 2)
        r.until(in_control, 600)
    settle(r)
    return {"at": [x, z], "row": summary(r.rows[-1])}


def a01_beat_control_room_items(r: Route) -> dict:
    use_a01_sampler(r)
    walk_path(r, [(70, -545), (66, -549.6)], tol=1.0)
    first = a01_take_item(r, 61.8, -549.6, 66.0, -math.pi / 2)
    walk_path(r, [(68, -565), (66, -574.4)], tol=1.0)
    second = a01_take_item(r, 62.1, -574.4, 66.0, -math.pi / 2)
    return {"what": "control-room pickups g0.8 (61.8, 15, -549.6) and g0.7 (62.1, 16, -574.4)",
            "takes": [first, second]}


def a01_beat_fire_contact(r: Route) -> dict:
    use_a01_sampler(r)
    walk_path(r, A01_TO_CRATE_NORTH[:6], tol=1.2)
    hp0 = a01_hp(r)
    r.until(lambda row: row["m1F0"] == 0x3E, 300, 0, *stick_values(r, 34.0, -708.0))
    r.set_pad(0)
    settle(r, 30)
    return {"what": "walk toward the fire at (31, -710) east of the crates: burn reaction (action 0x3E)",
            "hp_before": hp0, "hp_end": a01_hp(r)}


def stick_values(r: Route, x: float, z: float) -> tuple[int, int]:
    r.stick_toward(x, z)
    return r.pad_state[1], r.pad_state[2]


# -- AREA01 completeness beats (s88, lane capture) ----------------------------
# The rooms the main route left out (port docs/SECOND_LEVEL_ROUTE.md section 9):
# the east room behind the slider door [17] (001BB860, flags2 0x04: not one of
# the lock-gated models 0x16/0x17/0x3E) with the save terminal [20] (00159B90),
# the duct entered at the control room's attribute-0x37 floor square, the raised
# bridge [41] that closes the north room on the first visit, and the NPC's third
# conversation (D_008107D9 == 0x81, script 0x82A660).

def a01_turn_crawl(r: Route, yaw: float, limit: int = 40) -> None:
    """In the duct (player action 0x2D) the stick's x turns the view in 90
    degree steps (stick right lowers +0xC4) and stick up crawls."""
    for _ in range(limit):
        diff = (yaw - r.rows[-1]["yaw"] + math.pi) % (2 * math.pi) - math.pi
        if abs(diff) < 0.05:
            break
        r.set_pad(0, 0xFF if diff < 0 else 0x00, 0x7F)
        r.step(5)
    r.set_pad(0)
    r.step(5)


def a01_crawl(r: Route, limit: int = 60) -> None:
    """Hold stick up in the duct until the player stops moving or leaves the
    crawl (action 0x2E is the exit)."""
    last, still = None, 0
    for _ in range(limit):
        r.set_pad(0, 0x7F, 0x00)
        r.step(20)
        row = r.rows[-1]
        if row["m1F0"] != 0x2D:
            break
        px, _py, pz = row["pos"]
        if last is not None and abs(px - last[0]) < 0.6 and abs(pz - last[1]) < 0.6:
            still += 1
            if still >= 3:
                break
        else:
            still = 0
        last = (px, pz)
    r.set_pad(0)
    r.step(5)


def a01_beat_east_room(r: Route) -> dict:
    use_a01_sampler(r)
    walk_path(r, [(50, -590), (80, -605), (110, -610)], tol=1.2)
    approach(r, 123.0, -610.0)
    face(r, math.pi / 2)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][4:6] == "08" and in_control(row), 900)
    settle(r, 20)
    # save terminal [20] (00159B90): description, then the battery page's
    # prompt with the cursor on No; Cross keeps No, so nothing is saved.
    walk_path(r, [(150, -612), (160, -615)], tol=1.0)
    approach(r, 166.0, -617.5)
    face(r, math.pi)
    charge0 = r.rows[-1]["charge"]
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["msg"][:2] != "00", 400)
    r.idle(60)
    r.press("CROSS", 2)
    r.until(lambda row: row["ui"][2:4] == "03", 400)
    r.idle(60)
    r.press("CROSS", 2)
    r.until(in_control, 600)
    settle(r, 20)
    if r.rows[-1]["charge"] != charge0:
        raise RuntimeError("battery charge changed at the save prompt: " + summary(r.rows[-1]))
    walk_path(r, [(150, -612), (140, -610)], tol=1.0)
    approach(r, 134.5, -610.0)
    face(r, -math.pi / 2)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][4:6] == "09" and in_control(row), 900)
    settle(r, 20)
    return {"what": "slider door [17] to the east room (spawn entry 8), the save terminal [20] declined "
                    "(battery prompt, No), back through [17] (entry 9)", "charge": charge0}


def a01_beat_duct(r: Route) -> dict:
    use_a01_sampler(r)
    walk_path(r, [(90, -540), (120, -535), (130, -530)], tol=1.2)
    approach(r, 132.5, -529.5)
    face(r, math.pi / 2)
    use_press(r, lambda row: row["m1F0"] in (0x2C, 0x2D))
    r.until(lambda row: row["m1F0"] == 0x2D, 300)
    r.idle(90)                                    # the entry clip ignores the stick
    a01_crawl(r)                                  # east to (166.5, -529.5)
    a01_turn_crawl(r, 0.0)
    a01_crawl(r)                                  # north to (166.5, -416.5)
    a01_turn_crawl(r, -math.pi / 2)
    a01_crawl(r)                                  # west to the dead end at x 143
    taken0 = r.rows[-1]["taken"]
    use_press(r, lambda row: row["ui"][2:4] == "03", wait=60)
    r.idle(60)
    r.press("TRIANGLE", 2)
    r.until(lambda row: row["ui"][2:4] == "00" and row["m1F0"] == 0x2D, 300)
    r.step(20)
    a01_turn_crawl(r, math.pi / 2)
    a01_crawl(r)
    a01_turn_crawl(r, math.pi)
    a01_crawl(r)
    a01_turn_crawl(r, -math.pi / 2)
    a01_crawl(r)                                  # out through the entry (action 0x2E)
    r.until(in_control, 300)
    settle(r, 20)
    if r.rows[-1]["taken"] == taken0:
        raise RuntimeError("duct item not taken: " + summary(r.rows[-1]))
    return {"what": "control-room duct (attribute-0x37 entry square): crawl to the dead end, take the "
                    "item there, crawl back out"}


def a01_beat_bridge_blocked(r: Route) -> dict:
    use_a01_sampler(r)
    walk_path(r, [(20, -540), (0, -530)], tol=1.2)
    for _ in range(40):
        z0 = r.rows[-1]["pos"][2]
        r.stick_toward(0.0, -470.0)
        r.step(10)
        if abs(r.rows[-1]["pos"][2] - z0) < 0.05:
            break
    r.set_pad(0)
    settle(r, 10)
    face(r, 0.0)
    r.press("CROSS", 2)
    r.idle(60)
    settle(r, 10)
    if r.rows[-1]["pos"][2] > -520.0:
        raise RuntimeError("the north room was entered: " + summary(r.rows[-1]))
    return {"what": "walk north at the raised bridge [41] (overlay 0x826200 +0xC0 = pi/3): stopped at "
                    "z -525.4; Use there is not taken", "stop": r.rows[-1]["pos"]}


def a01_beat_npc_third_talk(r: Route) -> dict:
    use_a01_sampler(r)
    a01_talk_npc(r)
    if r.rows[-1]["d9"] != "81":
        raise RuntimeError("D_008107D9 changed: " + summary(r.rows[-1]))
    return {"what": "Use at NPC r36 with D_008107D9 == 0x81: script 0x82A660"}


A01_BEATS = [
    ("a01_00_train_room", "15_level_exit", a01_beat_train_room),
    ("a01_01_tunnel", "a01_00_train_room", a01_beat_tunnel),
    ("a01_02_shaft_landing", "a01_01_tunnel", a01_beat_shaft_landing),
    ("a01_03_shaft_locked", "a01_02_shaft_landing", a01_beat_shaft_locked),
    ("a01_04_return_north", "a01_03_shaft_locked", a01_beat_return_north),
    ("a01_05_npc_bridge_talk", "a01_04_return_north", a01_beat_npc_bridge_talk),
    ("a01_06_return_south", "a01_05_npc_bridge_talk", a01_beat_return_south),
    ("a01_07_level_exit", "a01_06_return_south", a01_beat_level_exit),
    # side beats
    ("a01_s0_npc_first_talk", "15_level_exit", a01_beat_npc_first_talk),
    ("a01_s1_sentry_doc", "a01_01_tunnel", a01_beat_sentry_doc),
    ("a01_s2_control_room_items", "a01_s0_npc_first_talk", a01_beat_control_room_items),
    ("a01_s3_fire_contact", "15_level_exit", a01_beat_fire_contact),
    # completeness side beats (s88)
    ("a01_s4_east_room", "15_level_exit", a01_beat_east_room),
    ("a01_s5_duct", "a01_s0_npc_first_talk", a01_beat_duct),
    ("a01_s6_bridge_blocked", "15_level_exit", a01_beat_bridge_blocked),
    ("a01_s7_npc_third_talk", "a01_05_npc_bridge_talk", a01_beat_npc_third_talk),
]
A01_SIDE_BEATS = {"a01_s0_npc_first_talk", "a01_s1_sentry_doc", "a01_s2_control_room_items",
                  "a01_s3_fire_contact", "a01_s4_east_room", "a01_s5_duct", "a01_s6_bridge_blocked",
                  "a01_s7_npc_third_talk"}


def a01_selected(spec: str) -> list[tuple]:
    """`a01` = every AREA01 beat in order; otherwise a comma list of names or
    name prefixes (a01_03, a01_s1, ...)."""
    wanted = spec.split(",")
    if "a01" in wanted:
        return list(A01_BEATS)
    return [b for b in A01_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# ---------------------------------------------------------------------------
# C7 capture group (opt-in, `--beats c7` or a beat's name): original captures
# the C6 chain requested (docs/CAPTURES_C7.md).  Same Route, sampler and row
# format as the AREA11 beats; outputs go to build/s87/c7cap/<item>/<beat>/.
OUT_C7 = ROOT / "build/s87/c7cap"


def beat_c7_door1(r: Route) -> dict:
    # Side 1 of the fence door r0 (001BC350): beat 09 ends behind the fence at
    # spawn entry 2 (424.2, 184.8, 274.5), facing yaw pi.  Turn back to the
    # door (423, 184.8, 290.3), walk against it from the south and Use it: the
    # door's side decision now picks destination entry 1, and the arrival's
    # walk-out runs before control returns.
    settle(r, 10)
    face(r, 0.0)
    r.goto(422.0, 290.3, tol=0.6, magnitude=0.5, stuck_ok=True)
    settle(r, 10)
    pressed = []
    for attempt in range(4):
        if attempt == 1:
            face(r, 0.0)
        elif attempt == 2:
            r.goto(419.0, 288.0, tol=0.6, magnitude=0.5, stuck_ok=True)
            settle(r, 10)
            face(r, 0.3)
        elif attempt == 3:
            r.goto(426.0, 288.0, tol=0.6, magnitude=0.5, stuck_ok=True)
            settle(r, 10)
            face(r, -0.3)
        pressed.append({"f": r.frame_index, "pos": r.rows[-1]["pos"], "yaw": r.rows[-1]["yaw"]})
        r.press("CROSS", 2)
        try:
            r.until(lambda row: not in_control(row), 40)
            break
        except TimeoutError:
            settle(r, 10)
    else:
        raise TimeoutError("the fence door did not take the Use from side 1")
    r.until(lambda row: row["req"][16:18] == "02", 400)
    r.until(in_control, 900)
    settle(r, 60)
    return {"what": "Cross at the fence door from behind the fence (side 1): door script, room move to "
                    "entry 1, arrival walk-out until control returns", "presses": pressed}


C7_BEATS = [
    ("c7_door1_fence_door_side1", "09_fence_door", beat_c7_door1),
]
C7_DIRS = {"c7_door1_fence_door_side1": "door1"}


def c7_selected(spec: str) -> list[tuple]:
    """`c7` = every C7 beat; otherwise a comma list of names or name prefixes."""
    wanted = spec.split(",")
    if "c7" in wanted:
        return list(C7_BEATS)
    return [b for b in C7_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# ---------------------------------------------------------------------------
# AREA00 (the level after AREA01), opt-in beat group `a00` (s88, lane capture).
# From the a01_07 end snapshot (AREA00 sub 0, spawn entry 0, control after the
# arrival script) over the route as far as it was found: down to the floor,
# the locked door [51], the south route to the cage door [55], its padlock
# (001581A0, broken by a melee hit), the cage room's terminal [40] (which sends
# the ferry 0x825600 east); round 2 adds a00_05..a00_10, the progression
# route to AREA00's exit (see the block above their beat functions).
# Outputs go to build/s87/route_a00/<beat>/
# (ignored); described in the port's docs/THIRD_LEVEL_ROUTE.md.  None of these
# beats runs under `--beats all`.
OUT_A00 = ROOT / "build/s87/route_a00"

# Owner nodes of the AREA00 sub-0 load (the a01_07 arrival; placement table
# 0x82BB50 records [n]).  Measured in the a01_07 end snapshot.
A00_OWNERS = {
    "shaft_door_r52": 0x7B7C00,   # overlay 0x823580 (class 5): door id 0|0x80 back to AREA01
    "door_r56": 0x7B87C0,         # 001BC350 room move id 1 (west room)
    "door_r51": 0x7B7910,         # overlay 0x825170 room move id 3 (north-east room)
    "lock_r54": 0x7B81E0,         # 001581A0 (class 0x44): the padlock of door [55]
    "door_r55": 0x7B84D0,         # 001BC350 model 0x15, id 2 (cage room), lock-gated
    "terminal_r40": 0x7B58C0,     # overlay 0x825480 (cage-room terminal)
    "ferry_cab_r47": 0x7B6D50,    # overlay 0x8263C0 (examine)
    "ferry_r48": 0x7B7040,        # overlay 0x825600 (+0x0D == 2)
    "r43_825920": 0x7B6190,       # overlay 0x825920 (the class-0x88 member of [41]..[43])
    "beam_r60": 0x7B9380,         # overlay 0x8266A0
}
A00_SPANS = A01_BASE_SPANS + [
    ("story758", 0x810758, 0x8),        # D_00810758..5F (75A arrival, 75B door [51], 75C terminal, 75D, 75E)
    ("area4", 0x810700, 0x4),
    ("slots", 0x28A750, 0x60),
    ("bd8", 0x275BD8, 0x4),
    ("ovl", 0x823500, 0x8),             # overlay magic + id (1 = AREA00, 2 = AREA01)
    ("taken", 0x810860, 0x40),
    ("docs", 0x810D00, 0x20),
    ("msgrec", 0x282210, 0x14),
    ("locks", 0x810840, 0x8),           # D_00810841[area] door-lock bits (+1 = AREA00)
    ("wpn", 0x810C60, 0x8),             # D_00810C60.. (C62 = rounds in the weapon)
]
for _name, _base in A00_OWNERS.items():
    A00_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                  (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                  (_name + ":c", _base + 0x10, 0x4)]
A00_EVENT_KEYS = ("locks", "d7dc")


class A00Sampler(ExitSampler):
    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = A00_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))


def decode_a00(r: dict[str, bytes]) -> dict:
    row = decode(r, owners=A00_OWNERS)
    row["hp"] = round(f32(r["player"], 0x220), 3)
    row["d9"] = r["d2"][1:2].hex()
    row["f759"] = r["story758"][1:2].hex()
    row["story758"] = r["story758"].hex()
    row["d7dc"] = r["d2"][4:5].hex()                    # D_008107DC (ferry position bits)
    row["area4"] = r["area4"].hex()
    row["slots"] = r["slots"].hex()
    row["bd8"] = r["bd8"][0]
    row["ovl"] = r["ovl"].hex()
    row["taken"] = r["taken"].hex()
    row["docs"] = r["docs"].hex()
    row["msgrec"] = r["msgrec"].hex()
    row["locks"] = r["locks"].hex()
    row["wpn"] = r["wpn"].hex()
    for name in A00_OWNERS:
        row[name]["cb"] = hex(struct.unpack("<I", r[name + ":c"])[0])
    return row


def use_a00_sampler(r: Route) -> None:
    sampler = A00Sampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_a00(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


def a00_beat_descend(r: Route) -> dict:
    # From the arrival platform (y -35) west and south to the top of the
    # attribute-0x35 stairs (x -105..-55, z -1460..-1477), down to the floor.
    use_a00_sampler(r)
    walk_path(r, [(-45, -1380), (-45, -1430), (-100, -1440), (-110, -1455), (-105, -1468),
                  (-80, -1468), (-56, -1468), (-45, -1478)], tol=1.2)
    r.set_pad(0)
    settle(r, 10)
    a01_near(r, -45, -1478, 12, "stairs foot")
    return {"what": "arrival platform -> west stairs -> floor (y -60) beside door [56]"}


def a00_beat_door51_locked(r: Route) -> dict:
    # East along z ~ -1460, clear of the fire row (001E3D90 nodes at
    # z -1473..-1487, x -20..52), to the south side of door [51] (overlay
    # 0x825170): with D_0081075B != 0xFF its Use plays script 0x8294E0 and
    # stores D_0081075B = 1.
    use_a00_sampler(r)
    walk_path(r, [(-30, -1462), (0, -1459), (40, -1459), (80, -1462), (110, -1466)], tol=1.2)
    approach(r, 121.0, -1463.5)
    face(r, 0.0)
    use_press(r, lambda row: not in_control(row))
    r.until(in_control, 1500)
    settle(r, 30)
    if r.rows[-1]["story758"][6:8] != "01":
        raise RuntimeError("D_0081075B not 1: " + summary(r.rows[-1]))
    return {"what": "door [51] from the south with D_0081075B == 0: locked script 0x8294E0, "
                    "D_0081075B = 1", "hp_end": a01_hp(r)}


def a00_beat_south_route(r: Route) -> dict:
    # Onto the ferry (0x825600, at its west position) and down the pit's west
    # walkway, along the pit's south edge and the south strip to the cage door
    # [55]; the waypoints stay south of the creatures' nest at (200, -1654).
    use_a00_sampler(r)
    walk_path(r, [(100, -1500), (75, -1508), (55, -1512), (45, -1518), (40, -1524), (37, -1529.5),
                  (27, -1560), (27, -1600), (28, -1640), (29, -1656), (45, -1660), (70, -1660),
                  (95, -1662), (100, -1675), (128, -1668), (150, -1667), (170, -1670), (185, -1685),
                  (200, -1690)], tol=1.2, limit=300)
    approach(r, 209.0, -1690.0)
    a01_near(r, 209.0, -1690.0, 3, "cage door")
    return {"what": "door [51] -> ferry deck -> pit west walkway -> south edge -> cage door [55]",
            "hp_end": a01_hp(r)}


def a00_beat_padlock(r: Route) -> dict:
    # The padlock [54] (001581A0, +0x2E = door id 2): a hit (+0x36) sets
    # D_00810841[D_00810700] |= 1 << 2, which unlocks door [55] (001BC350
    # model 0x15).  A light melee (Circle, action 0x36) facing it lands the hit.
    use_a00_sampler(r)
    px, _py, pz = r.rows[-1]["pos"]
    face(r, math.atan2(214.8 - px, -1688.6 - pz), tol=0.08)
    for _ in range(4):
        r.press("CIRCLE", 2)
        try:
            r.until(lambda row: int(row["locks"][2:4], 16) & 4, 60)
            break
        except TimeoutError:
            settle(r, 10)
    else:
        raise TimeoutError("padlock not broken: " + summary(r.rows[-1]))
    settle(r, 30)
    return {"what": "light melee at the padlock [54]: D_00810841[0] |= 4 (door [55] unlocked)"}


def a00_beat_cage_terminal(r: Route) -> dict:
    # Door [55] (room move id 2) to spawn entry 3 inside the cage room, up its
    # attribute-0x35 stairs (x 254..265) to the terminal [40] (overlay
    # 0x825480): Use runs script 0x8299E0 (D_0081075C = 0xFF), then
    # D_008107DC |= 1 and the ferry 0x825600 runs script 0x829BE0 east
    # (D_008107DC = 2).
    use_a00_sampler(r)
    approach(r, 209.5, -1681.5)
    face(r, math.pi / 2)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][4:6] == "03" and in_control(row), 900)
    settle(r, 20)
    walk_path(r, [(240, -1692), (259, -1690), (259, -1660), (259, -1632), (245, -1630), (232, -1640),
                  (224, -1650)], tol=1.2, limit=300)
    r.set_pad(0)
    settle(r, 10)
    px, _py, pz = r.rows[-1]["pos"]
    face(r, math.atan2(219.0 - px, -1655.0 - pz))
    use_press(r, lambda row: not in_control(row))
    r.until(in_control, 3000)
    settle(r, 30)
    row = r.rows[-1]
    if row["story758"][8:10] != "ff" or row["d7dc"] != "02":
        raise RuntimeError("terminal effects not seen: " + summary(row))
    return {"what": "cage door [55] -> entry 3 -> stairs -> terminal [40]: D_0081075C = 0xFF, "
                    "ferry east (D_008107DC = 2)"}


def a00_beat_shaft_door_back(r: Route) -> dict:
    # The AREA00 shaft door [52] (overlay 0x823580) with D_0081075D and
    # D_0081075E != 0xFF takes its plain door path (001BBE40, 001BC240): door
    # id 0, record 01 00 00 00 = AREA01 entry 0 sub 0.
    use_a00_sampler(r)
    walk_path(r, [(-40.5, -1300), (-40.5, -1292)], tol=1.0)
    approach(r, -40.5, -1288.0)
    face(r, 0.0)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][:2] == "01", 1500)
    for _ in range(4):
        r.until(lambda row: row["slots"][22:24] == "01" and in_control(row), 8000)
        r.idle(30)
        if all(in_control(row) for row in r.rows[-30:]):
            break
    else:
        raise TimeoutError("AREA01 arrival: control not kept; " + summary(r.rows[-1]))
    settle(r, 30)
    return {"what": "AREA00 shaft door [52] before D_0081075D: plain door to AREA01 entry 0"}


# -- AREA00 progression (s88 round 2, lane capture) --------------------------
# The way into the north-east room, found from the original code and then
# played: the ferry (0x825600) at its raised east position carries the cab
# [47] (0x8263C0); a ledge climb onto the cab's roof (y -19.5), a running
# jump (0015EC50) east onto the container stack (y -10), the crate tops (the
# "mezzanine", y -20) and the attribute-0x37 duct square of spawn entry 10.
# Its duct entry (0016DE40 state 0: AREA00, z > -1470) puts the player at
# (185.8, -1450) in the north-east room with D_00810702 = 5; door [51] then
# stores D_0081075B = 0xFF.  [43] (0x825920) takes Use at entry 5: script
# 0x82A540 (D_0081075D = 0xFF, D_00810701 = 1).  The room's barricade of
# 001551B0 crates (deferred records 0x826F80[39..46]) blocks door [51]; the
# second melee (Square, action 0x37) breaks the floor crates.  Back at the
# shaft door [52] in sub-state 6, script 0x8286E0 (D_0081075E = 0xFF) and
# 001B0C60(1, 0xFF, 0): the area change to AREA01.

def a00_go(r: Route, points, tol: float = 1.2, limit: int = 500, magnitude: float = 1.0) -> None:
    """walk_path that fails the beat when a waypoint is not reached."""
    for x, z in points:
        hist: list[tuple[float, float]] = []
        for _ in range(limit):
            if r.stick_toward(x, z, magnitude) <= tol:
                break
            row = r.step(1)
            hist.append((row["pos"][0], row["pos"][2]))
            if len(hist) > 45 and math.hypot(hist[-1][0] - hist[-45][0], hist[-1][1] - hist[-45][1]) < 0.3:
                r.set_pad(0)
                raise RuntimeError(f"blocked before {(x, z)}; last {summary(r.rows[-1])}")
        else:
            r.set_pad(0)
            raise RuntimeError(f"{(x, z)} not reached; last {summary(r.rows[-1])}")
    r.set_pad(0)


def a00_push(r: Route, x: float, z: float, frames: int, magnitude: float = 0.5) -> None:
    """Hold the stick toward (x, z) for a fixed number of frames (walks until blocked)."""
    for _ in range(frames):
        r.stick_toward(x, z, magnitude)
        r.step(1)
    r.set_pad(0)
    settle(r, 5)


def a00_long_frames(r: Route) -> None:
    """The shaft door's script 0x8286E0 plays a movie inside one emulated frame
    (about 110 s of host time), during which the emulator answers neither the
    5 s Pine reads nor the 5 s DebugServer status polls: raise both."""
    import socket as _socket
    r.s.boundary_timeout = 900.0
    pine = getattr(r.s, "pine", None)
    if pine is not None:
        pine.s.settimeout(300)
    if type(r.s.debug).__name__ != "DebugServer":
        return                  # route_census's persistent connection has its own timeout
    port = r.s.debug.port

    def call(cmd: dict) -> dict:
        for _ in range(30):
            try:
                with _socket.create_connection(("127.0.0.1", port), timeout=30) as sock:
                    sock.sendall((json.dumps(cmd) + "\n").encode())
                    data = b""
                    while b"\n" not in data:
                        chunk = sock.recv(65536)
                        if not chunk:
                            raise EOFError("DebugServer closed the connection")
                        data += chunk
                response = json.loads(data.split(b"\n")[0])
                if not response.get("ok"):
                    raise RuntimeError(response)
                return response
            except (TimeoutError, _socket.timeout):
                time.sleep(2)
        raise TimeoutError("DebugServer did not answer")
    r.s.debug.call = call


def a00_crate_alive(r: Route, node: int) -> bool:
    head = r.s.read(node, 0x14)
    return head[4] == 4 and struct.unpack_from("<I", head, 0x10)[0] == 0x1551B0


def a00_kick(r: Route, node: int, x: float, z: float, tries: int = 4) -> None:
    """Second melee (Square, player action 0x37) at a floor crate 001551B0."""
    for _ in range(tries):
        if not a00_crate_alive(r, node):
            return
        px, _py, pz = r.rows[-1]["pos"]
        if math.hypot(x - px, z - pz) > 9.0:     # the kick reaches only the crate in front
            d = math.hypot(x - px, z - pz)
            approach(r, x + 7.0 * (px - x) / d, z + 7.0 * (pz - z) / d, tol=0.8)
            px, _py, pz = r.rows[-1]["pos"]
        face(r, math.atan2(x - px, z - pz), tol=0.06)
        r.press("SQUARE", 2, after=40)
    if a00_crate_alive(r, node):
        raise RuntimeError(f"crate {node:#x} not broken; last {summary(r.rows[-1])}")
    for _ in range(300):                         # the broken crate keeps its collision until freed
        head = r.s.read(node, 0x14)
        if head[0] == 0 or struct.unpack_from("<I", head, 0x10)[0] != 0x1551B0:
            break
        r.step(1)


def a00_beat_ferry_deck(r: Route) -> dict:
    # From the terminal platform down the cage room's stairs, out through door
    # [55] (room move to entry 4), west round the fence end at x 127 and east
    # along the pit's south ledge (y -70) to the raised ferry's west side;
    # a ledge climb (Use facing +x) onto its deck at y -48.
    use_a00_sampler(r)
    a00_go(r, [(227.5, -1630), (245, -1628), (259, -1628), (259, -1660), (259, -1690), (240, -1689),
               (225, -1687)])
    settle(r, 10)
    approach(r, 222.0, -1686.0)
    face(r, -math.pi / 2)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][4:6] == "04" and in_control(row), 900)
    settle(r, 20)
    a00_go(r, [(185, -1685), (170, -1670), (150, -1667), (128, -1668), (100, -1675), (95, -1662), (95, -1655)])
    settle(r, 10)
    a00_push(r, 140.0, -1655.0, 110, 1.0)       # east until the ferry's side stops the walk
    face(r, math.pi / 2)
    use_press(r, lambda row: row["m1F0"] == 8)
    r.until(in_control, 300)
    settle(r, 10)
    row = r.rows[-1]
    if abs(row["pos"][1] + 48.0) > 0.1 or row["ground"] != hex(A00_OWNERS["ferry_r48"]):
        raise RuntimeError("not on the ferry deck: " + summary(row))
    return {"what": "cage room -> door [55] (entry 4) -> pit south ledge -> ledge climb onto the raised "
                    "ferry's deck (y -48)", "hp_end": a01_hp(r)}


def a00_beat_cab_roof(r: Route) -> dict:
    # Onto the step [49] (0x825600 kind 0x15, y -40) with a ledge climb facing
    # +z, off its north end, north along the deck's west walkway until the cab
    # [47] stops the walk (z -1558.8), then Use facing +x: a ledge grab (action
    # 0x10, +5 = 9) and stick up pulls the player onto the cab's roof (y -19.5).
    use_a00_sampler(r)
    a00_go(r, [(139.7, -1648)], tol=0.8, magnitude=0.6)
    settle(r, 5)
    approach(r, 139.75, -1641.0, tol=0.4, magnitude=0.4)   # pressed against the step, Use is not taken
    face(r, 0.0, tol=0.05)
    use_press(r, lambda row: row["m1F0"] == 8, tries=8)
    r.until(in_control, 300)
    settle(r, 10)
    if abs(r.rows[-1]["pos"][1] + 40.04) > 0.1:
        raise RuntimeError("not on the step: " + summary(r.rows[-1]))
    for _ in range(300):                         # north off the step's end (z -1619)
        r.stick_toward(139.7, -1400.0, 0.5)
        r.step(1)
        if abs(r.rows[-1]["pos"][1] + 48.0) < 0.1:
            break
    r.set_pad(0)
    settle(r, 10)
    a00_go(r, [(138.2, -1575.0)], tol=1.0, magnitude=0.6)
    approach(r, 138.2, -1558.8, tol=0.3, magnitude=0.4)     # beside the cab's ladder (e9 of the s88 exploration)
    face(r, math.pi / 2)
    use_press(r, lambda row: row["m1F0"] in (8, 0x10), tries=8)
    r.until(lambda row: row["m1F0"] == 0x10 and row["p5"] == 9, 300)
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["m1F0"] not in (0x10, 0x11), 300, 0, 0x7F, 0x00)
    r.set_pad(0)
    r.until(in_control, 300)
    settle(r, 20)
    row = r.rows[-1]
    if abs(row["pos"][1] + 19.5) > 0.1 or row["ground"] != hex(A00_OWNERS["ferry_cab_r47"]):
        raise RuntimeError("not on the cab roof: " + summary(row))
    return {"what": "ferry deck -> step [49] -> walkway -> ledge grab at the cab [47] -> its roof (y -19.5)",
            "hp_end": a01_hp(r)}


def a00_beat_duct_to_ne_room(r: Route) -> dict:
    # A running jump (Cross while running, 0015EC50) east off the cab roof onto
    # the container stack (y -10), north onto the crate tops (y -20), to the
    # attribute-0x37 square at (182.5..189.5, -1467..-1460) facing +z; the duct
    # entry teleports into the north-east room (entry 5), control after a drop.
    use_a00_sampler(r)
    face(r, -math.pi / 2)
    a00_go(r, [(146.0, -1558.8)], tol=0.5, magnitude=0.5)
    face(r, math.pi / 2)
    for _ in range(200):
        r.stick_toward(260.0, -1558.8, 1.0)
        r.step(1)
        if r.rows[-1]["pos"][0] >= 160.0:
            break
    r.set_pad(PAD["CROSS"], r.pad_state[1], r.pad_state[2])
    r.step(2)
    r.stick_toward(260.0, -1558.8, 1.0)
    r.until(lambda row: row["p5"] == 6, 30, 0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["p5"] not in (6, 8) and abs(row["pos"][1] + 10.0) < 0.1, 200,
            0, r.pad_state[1], r.pad_state[2])
    r.set_pad(0)
    settle(r, 10)
    a00_go(r, [(200, -1540), (203, -1525)], tol=1.0, magnitude=0.8)
    for _ in range(200):
        r.stick_toward(203.0, -1505.0, 0.8)
        r.step(1)
        if r.rows[-1]["pos"][2] > -1508:
            break
    r.set_pad(0)
    settle(r, 10)
    a00_go(r, [(203, -1485), (203, -1472), (192, -1471)], tol=1.0, magnitude=0.7)
    settle(r, 10)
    approach(r, 186.0, -1465.0)
    face(r, 0.0)
    use_press(r, lambda row: row["p5"] in (0x18, 0x19, 0x1A))
    r.until(lambda row: row["area4"][4:6] == "05" and in_control(row), 900)
    settle(r, 30)
    if r.rows[-1]["story758"][6:8] != "ff":
        raise RuntimeError("D_0081075B not 0xFF: " + summary(r.rows[-1]))
    return {"what": "running jump cab roof -> container (y -10) -> crate tops (y -20) -> duct square of "
                    "entry 10 -> north-east room at entry 5 (D_0081075B = 0xFF)", "hp_end": a01_hp(r)}


def a00_beat_switch(r: Route) -> dict:
    # [43] (0x825920, class 0x88) publishes itself only at entry 5/6; Use from
    # 5 units south of it runs script 0x82A540: D_0081075D = 0xFF and
    # D_00810701 = 1 (the screen fades out and back in around the script).
    use_a00_sampler(r)
    a00_long_frames(r)          # one frame of the script ran past the 30 s default in a capture
    a00_go(r, [(180, -1425), (160, -1419)], tol=1.0, magnitude=0.8)
    settle(r, 10)
    approach(r, 150.0, -1417.0)
    face(r, 0.0)
    use_press(r, lambda row: not in_control(row))
    r.until(in_control, 3000)
    settle(r, 30)
    row = r.rows[-1]
    if row["story758"][10:12] != "ff" or row["area4"][2:4] != "01":
        raise RuntimeError("[43] effects not seen: " + summary(row))
    return {"what": "Use at [43] (entry 5): script 0x82A540, D_0081075D = 0xFF, D_00810701 = 1",
            "hp_end": a01_hp(r)}


def a00_beat_ne_room_out(r: Route) -> dict:
    # The crate barricade in front of door [51] (001551B0, model 0x50): the
    # floor crates at (124.3, -1446.5), (118.6, -1453.9) and (113.1, -1446.4)
    # are broken with the second melee (Square), then door [51] from the north
    # (D_0081075B == 0xFF: the ordinary door) moves the player to entry 7.
    use_a00_sampler(r)
    walk_path(r, [(140, -1422), (134, -1446)], tol=1.2, limit=300)
    r.set_pad(0)
    settle(r, 10)
    # the three upper crates (y -53, on the floor crates) first: the light
    # melee (Circle) reaches them; breaking one wakes the others (+0x52 set)
    uppers = (0x7AAB70, 0x7AAE60, 0x7AB150)
    for _ in range(6):
        if not any(a00_crate_alive(r, n) for n in uppers):
            break
        face(r, -math.pi / 2, tol=0.06)
        r.press("CIRCLE", 2, after=60)
    for _ in range(600):
        if not any(a00_crate_alive(r, n) for n in uppers):
            break
        r.step(1)
    if any(a00_crate_alive(r, n) for n in uppers):
        raise RuntimeError("upper crates not cleared: " + summary(r.rows[-1]))
    settle(r, 10)
    a00_kick(r, 0x7ABA20, 124.3, -1446.5)
    a00_push(r, 126.5, -1449.0, 60, 0.6)
    a00_kick(r, 0x7AB730, 118.6, -1453.9)
    a00_kick(r, 0x7AB440, 113.1, -1446.4)
    a00_push(r, 118.5, -1454.0, 120, 0.5)
    face(r, math.pi, tol=0.06)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][4:6] == "07" and in_control(row), 1500)
    settle(r, 20)
    return {"what": "break three barricade crates (Square) and leave the north-east room by door [51] "
                    "(entry 7)", "hp_end": a01_hp(r)}


def a00_beat_progression_exit(r: Route) -> dict:
    # Back west and up to the shaft door [52] (overlay 0x823580), now in
    # sub-state 6 (D_0081075D == 0xFF): Use starts script 0x8286E0
    # (D_0081075E = 0xFF, a movie inside one frame) and 001B0C60(1, 0xFF, 0):
    # AREA01, sub D_00810730[1] & 0x7F, entry 0.
    use_a00_sampler(r)
    a00_long_frames(r)
    a00_go(r, [(110, -1466), (80, -1462), (40, -1459), (0, -1459), (-30, -1462), (-56, -1468), (-80, -1468),
               (-105, -1468), (-110, -1455), (-100, -1440), (-45, -1430), (-45, -1380), (-40.5, -1300),
               (-40.5, -1292)])
    settle(r, 10)
    approach(r, -40.5, -1288.0)
    face(r, 0.0)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["area4"][:2] == "01", 1500)
    r.until(lambda row: row["slots"][22:24] == "01" and in_control(row), 8000)
    settle(r, 30)
    if r.rows[-1]["story758"][12:14] != "ff":
        raise RuntimeError("D_0081075E not 0xFF: " + summary(r.rows[-1]))
    return {"what": "shaft door [52] in sub-state 6: script 0x8286E0 (D_0081075E = 0xFF), "
                    "001B0C60(1, 0xFF, 0), AREA01 arrival at entry 0", "hp_end": a01_hp(r)}


A00_BEATS = [
    ("a00_00_descend", "a01_07_level_exit", a00_beat_descend),
    ("a00_01_door51_locked", "a00_00_descend", a00_beat_door51_locked),
    ("a00_02_south_route", "a00_01_door51_locked", a00_beat_south_route),
    ("a00_03_padlock", "a00_02_south_route", a00_beat_padlock),
    ("a00_04_cage_terminal", "a00_03_padlock", a00_beat_cage_terminal),
    ("a00_05_ferry_deck", "a00_04_cage_terminal", a00_beat_ferry_deck),
    ("a00_06_cab_roof", "a00_05_ferry_deck", a00_beat_cab_roof),
    ("a00_07_duct_to_ne_room", "a00_06_cab_roof", a00_beat_duct_to_ne_room),
    ("a00_08_switch", "a00_07_duct_to_ne_room", a00_beat_switch),
    ("a00_09_ne_room_out", "a00_08_switch", a00_beat_ne_room_out),
    ("a00_10_progression_exit", "a00_09_ne_room_out", a00_beat_progression_exit),
    # side beats
    ("a00_s0_shaft_door_back", "a01_07_level_exit", a00_beat_shaft_door_back),
]


def a00_selected(spec: str) -> list[tuple]:
    """`a00` = every AREA00 beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "a00" in wanted:
        return list(A00_BEATS)
    return [b for b in A00_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# ---------------------------------------------------------------------------
# AREA01 revisit (opt-in group `a01r`) and AREA02 (opt-in group `a02`), s88
# lane NEXT.  The revisit starts from the a00_10 end snapshot: AREA01 sub 0
# loaded again at spawn entry 0 with D_0081075E = 0xFF, which lowers the
# bridge halves [41]/[42] (0x8261A0 state 0) and spawns the condition-3
# records of the deferred group 0x828A00 (0x823CD0 x5, 0x825950 x2, 0x826CF0).
# Outputs go to build/s87/route_a01r/<beat>/ and build/s87/route_a02/<beat>/
# (ignored); described in the port's docs/FOURTH_LEVEL_ROUTE.md.  None of
# these beats runs under `--beats all`.
OUT_A01R = ROOT / "build/s87/route_a01r"
OUT_A02 = ROOT / "build/s87/route_a02"

# Owner nodes of the revisit's AREA01 sub-0 load, measured in the a00_10 end
# snapshot (the pool differs from the first visit's: the group records come
# first).  Record numbers are placement table 0x82BD50 [n] or group 0x828A00 g[n].
A01R_OWNERS = {
    "shaft_door_r12": 0x7ADD60,   # overlay 0x823580
    "r13_158D30": 0x7AE050,       # 00158D30
    "door_r14": 0x7AE340,         # 001BC350 model 0x15, door id 1|0x80: AREA02 entry 0 sub 0
    "door_r15": 0x7AE630,         # 001BC350 room move id 2 (control room)
    "door_r16": 0x7AE920,         # 001BC350 model 0x03, door id 3|0x80: AREA02 entry 1 sub 1
    "bridge_r41": 0x7B3290,       # overlay 0x8261A0 (+0x0D 2): the south bridge half, z -525
    "bridge_r42": 0x7B3580,       # overlay 0x8261A0 (+0x0D 3): the north bridge half, z -315
    "r45_8267C0": 0x7B3E50,       # overlay 0x8267C0 at (0, 156.9, -337.9)
    "g5_825950": 0x7A64F0,        # overlay 0x825950 (class 0x2A), group g[5] at (-9, 0, -576)
    "g7_825950": 0x7A6AD0,        # overlay 0x825950 (class 0x2A), group g[7] at (-5, 0, -543)
    "g6_826CF0": 0x7A67E0,        # overlay 0x826CF0 (class 8)
    "g0_823CD0": 0x7A5640, "g1_823CD0": 0x7A5930, "g2_823CD0": 0x7A5C20,
    "g3_823CD0": 0x7A5F10, "g4_823CD0": 0x7A6200,   # overlay 0x823CD0, group g[0..4]
}
A01R_SPANS = A00_SPANS[:len(A01_BASE_SPANS) + 10] + [
    ("story760", 0x810760, 0x4),        # D_00810760..63 (760 = the [42] script's flag)
    ("s784", 0x810784, 0x4),            # D_00810784..87
]
for _name, _base in A01R_OWNERS.items():
    A01R_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]


class A01RSampler(ExitSampler):
    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = A01R_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))


def decode_a01r(r: dict[str, bytes], owners=None) -> dict:
    owners = A01R_OWNERS if owners is None else owners
    row = decode(r, owners=owners)
    row["hp"] = round(f32(r["player"], 0x220), 3)
    row["d9"] = r["d2"][1:2].hex()
    row["f759"] = r["story758"][1:2].hex()
    row["story758"] = r["story758"].hex()
    row["story760"] = r["story760"].hex()
    row["s784"] = r["s784"].hex()
    row["d7dc"] = r["d2"][4:5].hex()
    row["e0"] = r["d2"][8:9].hex()                      # D_008107E0 (the [42] script's counter)
    row["df"] = r["d2"][7:8].hex()                      # D_008107DF (the revisit event stage, 0x825950)
    row["e1"] = r["d2"][9:10].hex()                     # D_008107E1 (AREA02: the switch / car / gate bits)
    if "inv2" in r:
        row["item20"] = r["inv2"][0]                    # D_00810C84 (item 0x20 count)
    row["inf"] = round(f32(r["player"], 0x228), 3)      # player +0x228 infection (FINDINGS: 0x8104D8)
    row["infected"] = r["player"][0x234]                # player +0x234 infected latch
    row["area4"] = r["area4"].hex()
    row["slots"] = r["slots"].hex()
    row["bd8"] = r["bd8"][0]
    row["ovl"] = r["ovl"].hex()
    row["taken"] = r["taken"].hex()
    row["docs"] = r["docs"].hex()
    row["msgrec"] = r["msgrec"].hex()
    row["locks"] = r["locks"].hex()
    row["wpn"] = r["wpn"].hex()
    for name in owners:
        row[name]["cb"] = hex(struct.unpack("<I", r[name + ":c"])[0])
        row[name]["rot"] = vec(r[name + ":r"], 0, 4)
    return row


def next_long_frames(r: Route, budget: float = 1800.0) -> None:
    """a00_long_frames for the revisit and AREA02: some frames play a movie
    inside one emulated frame (minutes of host time on a loaded host), and
    meanwhile the DebugServer may also reset or refuse connections.  Every
    DebugServer call retries for up to `budget` seconds."""
    import socket as _socket
    r.s.boundary_timeout = budget
    pine = getattr(r.s, "pine", None)
    if pine is not None:
        pine.s.settimeout(600)
    if type(r.s.debug).__name__ != "DebugServer":
        return                  # route_census's persistent connection has its own timeout
    port = r.s.debug.port

    def call(cmd: dict) -> dict:
        deadline = time.monotonic() + budget
        while True:
            try:
                with _socket.create_connection(("127.0.0.1", port), timeout=60) as sock:
                    sock.sendall((json.dumps(cmd) + "\n").encode())
                    data = b""
                    while b"\n" not in data:
                        chunk = sock.recv(65536)
                        if not chunk:
                            raise EOFError("DebugServer closed the connection")
                        data += chunk
                response = json.loads(data.split(b"\n")[0])
                if not response.get("ok"):
                    raise RuntimeError(response)
                return response
            except (OSError, EOFError):
                if time.monotonic() > deadline:
                    raise TimeoutError("DebugServer did not answer")
                if not r.s._alive():
                    raise RuntimeError("the emulator process exited")
                time.sleep(2)
    r.s.debug.call = call


def use_a01r_sampler(r: Route) -> None:
    sampler = A01RSampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_a01r(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


NEXT_EVENT_KEYS = ("df", "e0", "e1", "story760", "s784", "inf")

# AREA02 owners.  Sub 1 (the room behind AREA01 door [16]; placement table
# 0x828170) measured at the a01r_03 arrival, sub 0 (placement table 0x827830)
# after the duct's sub change (a02_00).  The two loads reuse the same pool
# addresses, so each row carries both tables: only the one of the resident sub
# names real nodes.
A02S1_OWNERS = {
    "s1_pick_g0": 0x7A5640,       # 0015AFA0 deferred 0x825B50[0] at (62.2, 14, -216.2)
    "s1_pick_g1": 0x7A5930,       # 0015AFA0 deferred 0x825B50[1] at (88.4, 9.7, -269.9)
    "s1_pick_g2": 0x7A5C20,       # 0015AFA0 deferred 0x825B50[2] at (88.3, 9.5, -272.6)
    "s1_bed_r0": 0x7A5F10,        # 00159620 model 0x36 at (75, 0, -188.2): its Use opens the vaccine prompt
    "s1_r1_1C4AF0": 0x7A6200,     # 001C4AF0 at the same position
    "s1_r2_159970": 0x7A64F0,     # 00159970 model 0x37 at (57.5, 15, -292.6)
    "s1_save_r3": 0x7A67E0,       # 00159B90 at (116.2, 8, -184)
    "s1_panel_r4": 0x7A6AD0,      # 00159210 model 0x2C at (80.1, 8.2, -244)
    "s1_door_r5": 0x7A6DC0,       # 001BC350 door id 1|0x80: AREA01 entry 5
    "s1_door_r6": 0x7A70B0,       # 001BC350 room move id 2 (entries 3/2)
}
A02_OWNERS = dict(A02S1_OWNERS, **{
    "s0_lock_r21": 0x7AD780,      # 001582E0 at (-38.6, 7.9, -187.2), beside door [22]
    "s0_door_r22": 0x7ADA70,      # 001BC350 model 0x15, door id 0|0x80: AREA01 entry 3
    "s0_panel_r24": 0x7AE050,     # 00158EC0 model 0x14 at (424.5, 30, 100): the battery panel
    "s0_door_r25": 0x7AE340,      # 001BB860 model 0x17, door id 3|0x80: AREA04 entry 0
    "s0_lamp_r26": 0x7AE630,      # 00158BD0 at (440, 38.4, 100)
    "s0_r27_825100": 0x7AE920,    # overlay 0x825100 kind 0x15 at (-78.3, 0, -55.8)
    "s0_turntable_r28": 0x7AEC10, # overlay 0x825100 kind 9 at (-0.2, -4, 10)
    "s0_r31_823980": 0x7AF4E0,    # overlay 0x823930 -> 0x823980 (class 0x89) at (140, 0, -25): the switch
    "s0_k7_r32": 0x7AF7D0,        # overlay 0x823930 -> 0x824020 kind 7 (model 0x54): the runaway car
    "s0_k8_r33": 0x7AFAC0,        # kind 8 at (-425, 0, 10)
    "s0_k11_r34": 0x7AFDB0,       # kind 11 at (-80.2, 0, 5.2)
    "s0_k10_r35": 0x7B00A0,       # kind 10 at (210.9, 0, 9.1): the wreck climbed in a02_03
    "s0_gate_r36": 0x7B0390,      # overlay 0x823D70 kind 16 at x -280.9
    "s0_gate_r37": 0x7B0680,      # overlay 0x823D70 kind 15 at x 185
    "s0_gate_r38": 0x7B0970,      # overlay 0x823D70 kind 14 at x 185 (freed when D_008107E1 bit 3 is set)
})


class A02Sampler(ExitSampler):
    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = A02_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))


A02_SPANS = A01R_SPANS[:len(A00_SPANS[:len(A01_BASE_SPANS) + 10]) + 2] + [
    ("inv2", 0x810C84, 0x4),            # D_00810C84 (item 0x20 count) ..
]
for _name, _base in A02_OWNERS.items():
    A02_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                  (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                  (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]


def use_a02_sampler(r: Route) -> None:
    sampler = A02Sampler(r.s)
    r.sampler = sampler
    r.now = lambda: dict(decode_a01r(sampler.raw(), owners=A02_OWNERS))
    r.rows[0] = dict(r.now(), f=0)


def next_control_kept(r: Route, limit: int, need: int = 60) -> dict:
    """Step until control has held for `need` consecutive frames."""
    for _ in range(limit):
        row = r.step(1)
        if in_control(row) and len(r.rows) > need and all(in_control(x) for x in r.rows[-need:]):
            return row
    raise TimeoutError("control not kept; last " + summary(r.rows[-1]))


def next_climb(r: Route, x: float, z: float, yaw: float, tries: int = 4, rounds: int = 5) -> None:
    """Push toward (x, z) until blocked, face `yaw`, Use: a ledge climb.  The
    AREA02 bugs grab the player (action 0x3E) and knock the walk off its line,
    so the push is repeated for up to `rounds` rounds."""
    for attempt in range(rounds):
        a00_push(r, x, z, 40, 0.5)
        face(r, yaw)
        try:
            use_press(r, lambda row: row["m1F0"] in (8, 0x10), tries=tries if attempt == rounds - 1 else 2, wait=30)
            break
        except TimeoutError:
            if attempt == rounds - 1:
                raise
    r.until(lambda row: row["m1F0"] not in (8, 0x10, 0x11), 300)
    settle(r, 10)


def next_prompt_yes(r: Route) -> None:
    """A status-page yes/no prompt (ui byte 5 = 4) with the cursor on No:
    d-pad left moves it to Yes (ui byte 6 = 0), Cross confirms."""
    r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][10:12] == "04", 1500)
    r.idle(30)
    r.press("LEFT", 2, after=20)
    if r.rows[-1]["ui"][12:14] != "00":
        raise RuntimeError("prompt cursor not on Yes: " + summary(r.rows[-1]))
    r.press("CROSS", 2)


# -- the AREA01 revisit (a01r) -------------------------------------------------

def a01r_beat_to_train_room(r: Route) -> dict:
    # Arrival (entry 0) -> the tunnel -> the train room's crate stack (ledge
    # grab from the south, drop off its north side) -> x 30, z -655, just south
    # of the 0x825950 trigger quad 0x82B090 (x -75..100, z -640..-600).
    use_a01r_sampler(r)
    next_long_frames(r)
    walk_path(r, A01_LANDING_TO_MOUTH, tol=1.2)
    walk_path(r, [(16, -770), (16.5, -738)], tol=1.0)
    approach(r, 16.5, -733.5)
    face(r, 0.0)
    use_press(r, lambda row: row["m1F0"] in (8, 0x10))
    r.until(lambda row: row["m1F0"] not in (8,), 120)
    for _ in range(200):
        r.stick_toward(16.0, -690.0)
        row = r.step(1)
        if row["m1F0"] not in (8, 0x10, 0x11) and row["pos"][1] > 20:
            break
    for _ in range(300):
        r.stick_toward(16.0, -697.0)
        row = r.step(1)
        if row["pos"][1] < 1.5 and row["m1F0"] not in (0x0B, 0x0F) and row["pos"][2] > -706:
            break
    r.set_pad(0)
    settle(r, 10)
    walk_path(r, [(33, -692), (32, -668), (30, -655)], tol=1.2)
    r.set_pad(0)
    settle(r, 10)
    a01_near(r, 30, -655, 14, "train room north of the crates")
    if r.rows[-1]["df"] != "00" or r.rows[-1]["pos"][2] > -641.0:
        raise RuntimeError("the event started early: " + summary(r.rows[-1]))
    return {"what": "revisit arrival (entry 0) -> tunnel -> crate stack -> train room south of the trigger quad",
            "hp_end": a01_hp(r)}


def a01r_beat_event(r: Route) -> dict:
    # Into the quad 0x82B090: 0x825950 (g[5], model 0x47) starts script
    # 0x82AA90 (D_008107DF = 1); the event runs D_008107DF 1 -> 2 -> 0x10 ->
    # 0x40 -> 0x80 -> 0xFF (script 0x82AD90, a long camera cinematic); at its
    # end 001C47A0(0x20) gives item 0x20 and opens its status page.
    use_a01r_sampler(r)
    next_long_frames(r)
    walk_path(r, [(24, -645), (20, -625)], tol=1.2, until=lambda row: row["df"] != "00")
    r.set_pad(0)
    r.until(lambda row: row["ui"][2:4] == "03" and row["df"] == "ff", 12000)
    r.idle(30)
    r.press("TRIANGLE", 2)
    r.until(in_control, 600)
    settle(r, 20)
    row = r.rows[-1]
    item20 = r.s.read(0x810C84, 4)[0]
    if row["df"] != "ff" or row["story758"][14:16] != "ff" or item20 != 1:
        raise RuntimeError("revisit event effects not seen: " + summary(row))
    return {"what": "the train-room event: script 0x82AA90 .. 0x82AD90, D_008107DF -> 0xFF, D_0081075F = 0xFF, "
                    "item 0x20 given (status page closed with Triangle)", "item20": item20, "hp_end": a01_hp(r)}


def a01r_beat_bridge(r: Route) -> dict:
    # North up the lowered south half [41] (tilt pi/18), off its end onto the
    # north half [42] (tilt 0): inside quad 0x82CC60 at y <= 2 [42] starts
    # script 0x82B0D0 (D_008107E0 = 1, 0xE0, 2, then 0xFF; D_00810760 = 0xFF;
    # [41] rises to 50 degrees behind the player).
    use_a01r_sampler(r)
    next_long_frames(r)
    walk_path(r, [(0, -580), (0, -550), (0, -530), (0, -500), (0, -450), (0, -420), (0, -400)], tol=1.5,
              until=lambda row: row["e0"] != "00")
    r.set_pad(0)
    r.until(lambda row: row["e0"] == "ff", 1500)
    next_control_kept(r, 900, 30)
    settle(r, 10)
    row = r.rows[-1]
    if row["story760"][:2] != "ff":
        raise RuntimeError("D_00810760 not 0xFF: " + summary(row))
    return {"what": "over the lowered bridge [41] onto [42]: script 0x82B0D0, D_008107E0 -> 0xFF, D_00810760 = 0xFF",
            "hp_end": a01_hp(r)}


def a01r_beat_door16(r: Route) -> dict:
    # The north room: north along x 0, east into door [16]'s recess (x 40..50,
    # z -235..-215); Use facing +x: area change to AREA02 entry 1 sub 1.
    use_a01r_sampler(r)
    next_long_frames(r)
    walk_path(r, [(0, -330), (0, -300), (0, -260), (0, -232), (20, -225), (35, -224), (43, -221)], tol=1.5,
              limit=300)
    r.set_pad(0)
    settle(r, 5)
    approach(r, 45.5, -220.5)
    face(r, math.pi / 2)
    use_press(r, lambda row: row["spad"][2:4] != "00" or row["m1F0"] == 0x41)
    r.until(lambda row: row["area4"][:2] == "02", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "020101":
        raise RuntimeError("not at AREA02 sub 1 entry 1: " + summary(r.rows[-1]))
    return {"what": "north room -> door [16] (id 3|0x80, record 02 01 01 01): AREA02 sub 1 entry 1 arrival",
            "hp_end": a01_hp(r)}


def a01r_beat_pickup(r: Route) -> dict:
    # The pickup group 0x8291C0 (0015AFA0, param 0x6C) spawned at the event's
    # end at (-11.4, 0.2, -607.4): take it, close its page.
    use_a01r_sampler(r)
    next_long_frames(r)
    walk_path(r, [(-5, -610)], tol=1.0)
    approach(r, -8.0, -607.4)
    face(r, -math.pi / 2)
    taken0 = r.rows[-1]["taken"]
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["ui"][2:4] == "03", 900)
    r.idle(90)
    r.press("TRIANGLE", 2)
    r.until(in_control, 900)
    settle(r, 20)
    return {"what": "take the pickup 0x8291C0[0] spawned by the event (item page, Triangle)",
            "taken_changed": r.rows[-1]["taken"] != taken0, "hp_end": a01_hp(r)}


def a01r_beat_door14_locked(r: Route) -> dict:
    # Door [14] (001BC350 model 0x15, id 1|0x80, lock bit D_00810841[1] bit 1)
    # from the north room: the locked-door program.
    use_a01r_sampler(r)
    next_long_frames(r)
    settle(r, 10)
    walk_path(r, [(0, -330), (0, -260), (-20.5, -210)], tol=1.2)
    approach(r, -20.5, -197.5)
    face(r, 0.0)
    use_press(r, lambda row: not in_control(row))
    next_control_kept(r, 3000, 60)
    settle(r, 10)
    if r.rows[-1]["area4"][:2] != "01":
        raise RuntimeError("door [14] opened: " + summary(r.rows[-1]))
    return {"what": "Use at the lock-gated door [14] (D_00810841[1] bit 1 clear): locked-door program",
            "hp_end": a01_hp(r)}


A01R_BEATS = [
    ("a01r_00_to_train_room", "a00_10_progression_exit", a01r_beat_to_train_room),
    ("a01r_01_event", "a01r_00_to_train_room", a01r_beat_event),
    ("a01r_02_bridge", "a01r_01_event", a01r_beat_bridge),
    ("a01r_03_door16", "a01r_02_bridge", a01r_beat_door16),
    # side beats
    ("a01r_s0_pickup", "a01r_01_event", a01r_beat_pickup),
    ("a01r_s1_door14_locked", "a01r_02_bridge", a01r_beat_door14_locked),
]
A01R_SIDE_BEATS = {"a01r_s0_pickup", "a01r_s1_door14_locked"}
A01R_CHANGE_BEATS = {"a01r_03_door16"}      # the beat that leaves AREA01 for AREA02


# -- AREA02 (a02) ----------------------------------------------------------------

def a02_beat_duct(r: Route) -> dict:
    # Sub 1: the attribute-0x37 square (x 85.4..95.4, z -180..-170, axis +z)
    # east of the bed; the duct runs north to z -146.5 and west.  Mid-crawl the
    # load switches to sub 0 (D_00810701 = 0, D_00810702 = 5) and the crawl
    # exits at (35, 0, -146) in the south tunnel.
    use_a02_sampler(r)
    next_long_frames(r)
    walk_path(r, [(100, -215), (103, -190), (100, -178)], tol=1.0)
    approach(r, 93.0, -175.5)
    face(r, 0.0, tol=0.05)
    use_press(r, lambda row: row["m1F0"] in (0x2C, 0x2D) or row["p5"] in (0x18, 0x19, 0x1A))
    r.until(lambda row: row["m1F0"] == 0x2D, 400)
    r.idle(90)
    a01_crawl(r)
    a01_turn_crawl(r, -math.pi / 2)
    for _ in range(6):
        a01_crawl(r)
        if r.rows[-1]["m1F0"] != 0x2D:
            break
    next_control_kept(r, 3000, 60)
    settle(r, 10)
    if r.rows[-1]["area4"][:6] != "020005":
        raise RuntimeError("not at AREA02 sub 0 entry 5: " + summary(r.rows[-1]))
    return {"what": "sub-1 duct square -> crawl north and west -> sub change to AREA02 sub 0 entry 5",
            "hp_end": a01_hp(r)}


def a02_beat_switch(r: Route) -> dict:
    # North through the south tunnel and the junction (the turntable [28]) to
    # [31] (0x823980, class 0x89) at (140, 0, -25): its Use starts script
    # 0x826780 (D_00810761 = 1, D_008107E1 = 1, then |= 2 at the end) and
    # leaves the player on the rails at (121, 0, 12) facing -x.
    use_a02_sampler(r)
    next_long_frames(r)
    walk_path(r, [(33, -120), (30, -75), (38, -40), (60, -10), (100, 5), (145, -5), (140, -15)], tol=1.5,
              limit=400)
    approach(r, 140.0, -19.0)
    face(r, math.pi)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: int(row["e1"], 16) & 2 and in_control(row), 1500)
    r.idle(20)
    row = r.rows[-1]
    if row["story760"][2:4] != "01":
        raise RuntimeError("D_00810761 not 1: " + summary(row))
    return {"what": "south tunnel -> junction -> Use at [31]: script 0x826780, D_00810761 = 1, "
                    "D_008107E1 = 1 then 3", "hp_end": a01_hp(r)}


def a02_beat_ladder_escape(r: Route) -> dict:
    # The car (kind 7, [32]) runs east along the rails; standing on them it
    # kills the player (health 0 in an exploration run).  The ladder at
    # (122..136, z 38.6..49, attribute 0x32) north of the rails leads to a
    # ledge at y 50; the car passes, stops at x 210.9 and D_008107E1 = 0xFF
    # (D_00810761 = 0xFF).  Back down the ladder.
    use_a02_sampler(r)
    next_long_frames(r)
    walk_path(r, [(128, 35)], tol=1.2)
    approach(r, 129.0, 44.0)
    face(r, 0.0)
    ladder(r)
    settle(r, 10)
    r.until(lambda row: row["e1"] == "ff", 1500)
    settle(r, 20)
    approach(r, 129.5, 56.0)
    face(r, math.pi)
    r.press("CROSS", 2)
    r.until(lambda row: row["m1F0"] != 0, 120)
    r.set_pad(0, 0x7F, 0xFF)
    r.until(lambda row: row["pos"][1] < 1 and row["m1F0"] in (0, 1), 600, 0, 0x7F, 0xFF)
    r.set_pad(0)
    settle(r, 10)
    row = r.rows[-1]
    if row["story760"][2:4] != "ff" or row["hp"] <= 0:
        raise RuntimeError("the car's run not survived: " + summary(row))
    return {"what": "up the ladder (y 50) while the car runs east; D_008107E1 = D_00810761 = 0xFF; back down",
            "hp_end": a01_hp(r)}


def a02_beat_over_wreck(r: Route) -> dict:
    # East along the rails: the stopped car and [35] (kind 10, 0x823930) block
    # the tunnel at x ~194; a climb (Use facing +z) onto it (y ~21), along its
    # top, a drop at x ~268, then a ledge climb onto the east platform (y 15)
    # at its south edge (z 35).
    use_a02_sampler(r)
    next_long_frames(r)
    walk_path(r, [(160, 10), (200, 8)], tol=2.0, limit=300)
    next_climb(r, 193.5, 40.0, 0.0)
    if r.rows[-1]["pos"][1] < 15:
        raise RuntimeError("not on the wreck: " + summary(r.rows[-1]))
    walk_path(r, [(210, 8), (225, 8), (245, 8), (270, 20), (295, 30), (310, 30)], tol=2.0, limit=300)
    next_climb(r, 310.0, 60.0, 0.0)
    if abs(r.rows[-1]["pos"][1] - 15.0) > 0.2:
        raise RuntimeError("not on the east platform: " + summary(r.rows[-1]))
    return {"what": "over the stopped car / [35] and onto the east platform (y 15)", "hp_end": a01_hp(r)}


def a02_beat_panel(r: Route) -> dict:
    # The panel [24] (00158EC0 model 0x14) south of door [25]: Use opens the
    # battery page's prompt (4 units); Yes spends them and sets
    # D_00810841[2] |= 8 (door [25]'s lock bit, its door id 3).
    use_a02_sampler(r)
    next_long_frames(r)
    walk_path(r, [(330, 45), (380, 55), (425, 58), (425, 88)], tol=1.5, limit=300)
    r.set_pad(0)
    settle(r, 5)
    approach(r, 424.5, 94.0)
    face(r, 0.0)
    charge0 = r.rows[-1]["charge"]
    use_press(r, lambda row: not in_control(row))
    next_prompt_yes(r)
    next_control_kept(r, 3000, 60)
    settle(r, 10)
    row = r.rows[-1]
    if row["locks"][6:8] != "08":
        raise RuntimeError("D_00810841[2] bit 3 not set: " + summary(row))
    return {"what": "panel [24]: battery prompt, Yes: charge spent, D_00810841[2] |= 8",
            "charge_before": charge0, "charge_after": row["charge"], "hp_end": a01_hp(r)}


def a02_beat_progression_exit(r: Route) -> dict:
    # Door [25] (001BB860 model 0x17, door id 3|0x80, record 04 00 00 00):
    # area change to AREA04 entry 0, its arrival script, control.
    use_a02_sampler(r)
    next_long_frames(r)
    walk_path(r, [(440.2, 96.0)], tol=0.8)
    door = lambda row: row["spad"][2:4] != "00" or row["m1F0"] == 0x41
    for attempt in range(8):                    # a bug's grab (action 0x3E) can take the press
        approach(r, 440.2, 103.5, tol=0.5, magnitude=0.4)
        face(r, 0.0, tol=0.05)
        r.press("CROSS", 2)
        try:
            r.until(door, 40)
            break
        except TimeoutError:
            r.until(in_control, 1200)
    else:
        raise TimeoutError("door [25] not opened: " + summary(r.rows[-1]))
    r.until(lambda row: row["area4"][:2] == "04", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    return {"what": "door [25] (unlocked by the panel): area change to AREA04 entry 0, arrival, control",
            "hp_end": a01_hp(r)}


def a02_beat_mts_bed(r: Route) -> dict:
    # Sub 1: Use at the bed 00159620 (model 0x36) opens the healing page's
    # use-item prompt for item 0x20; Yes.
    use_a02_sampler(r)
    next_long_frames(r)
    walk_path(r, [(78, -205), (75, -197)], tol=1.0)
    approach(r, 75.0, -195.0)
    face(r, 0.0)
    inf0 = r.rows[-1]["inf"]
    use_press(r, lambda row: not in_control(row))
    next_prompt_yes(r)
    next_control_kept(r, 6000, 60)
    settle(r, 10)
    return {"what": "Use at the bed [0] with item 0x20: use-item prompt, Yes",
            "inf_before": inf0, "inf_after": r.rows[-1]["inf"], "hp_end": a01_hp(r)}


A02_BEATS = [
    ("a02_00_duct", "a01r_03_door16", a02_beat_duct),
    ("a02_01_switch", "a02_00_duct", a02_beat_switch),
    ("a02_02_ladder_escape", "a02_01_switch", a02_beat_ladder_escape),
    ("a02_03_over_wreck", "a02_02_ladder_escape", a02_beat_over_wreck),
    ("a02_04_panel", "a02_03_over_wreck", a02_beat_panel),
    ("a02_05_progression_exit", "a02_04_panel", a02_beat_progression_exit),
    # side beats
    ("a02_s0_mts_bed", "a01r_03_door16", a02_beat_mts_bed),
]
A02_SIDE_BEATS = {"a02_s0_mts_bed"}
A02_CHANGE_BEATS = {"a02_05_progression_exit"}         # beats that leave AREA02


def a01r_selected(spec: str) -> list[tuple]:
    """`a01r` = every revisit beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "a01r" in wanted:
        return list(A01R_BEATS)
    return [b for b in A01R_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def a02_selected(spec: str) -> list[tuple]:
    """`a02` = every AREA02 beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "a02" in wanted:
        return list(A02_BEATS)
    return [b for b in A02_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# ---------------------------------------------------------------------------
# AREA04 (opt-in group `a04`), s88 lane NEXT.  The beats start from the a02_05
# end snapshot: AREA04 (overlay id 5) sub 0 at spawn entry 0, control after
# the arrival script, D_008107E4 = 1.  Outputs go to build/s87/route_a04/<beat>/
# (ignored); described in the port's docs/FIFTH_LEVEL_ROUTE.md.  None of these
# beats runs under `--beats all`.
OUT_A04 = ROOT / "build/s87/route_a04"

# Owner nodes of the AREA04 sub-0 load, measured in the a02_05 end snapshot.
# Record numbers are placement table 0x82A130 [n] or group 0x826930 g[n].
A04_OWNERS = {
    "director_r1": 0x7A9FB0,      # overlay 0x823B90 (class 8): D_008107E4, the door-[45] event
    "npc_r2": 0x7AA2A0,           # overlay 0x824320 (class 0xAA) at (400, 15, 100), behind door [45]
    "r3_8246B0": 0x7AA590,        # overlay 0x8246B0 at (322, 15, 224)
    "r4_824DC0": 0x7AA880,        # overlay 0x824DC0 at (560, 69.9, 284)
    "door35_r35": 0x7B0390,       # 001BB860 model 0x09, door id 8|0x80: back to AREA02 entry 4
    "door37_r37": 0x7B0970,       # 001BC350 model 0x03, door id 1|0x80: AREA22 entry 0
    "door38_r38": 0x7B0C60,       # 001BC350 model 0x15, door id 2|0x80: AREA03 (lock bit 2)
    "door40_r40": 0x7B1240,       # 001BC350 model 0x15, room move id 3 (lock bit 3)
    "door42_r42": 0x7B1820,       # 001BC350 model 0x15, door id 4|0x80: AREA20 (lock bit 4)
    "door45_r45": 0x7B20F0,       # overlay 0x823700 model 0x16, room move id 5 (lock bit 5)
    "r47_823EE0": 0x7B26D0,       # overlay 0x823EE0 at door [45]
    "lock_r48": 0x7B29C0,         # 001581A0 (class 0x44) beside door [49]
    "door49_r49": 0x7B2CB0,       # 001BC350 model 0x15, room move id 6 (lock bit 6)
    "lift_r51": 0x7B3290,         # 001BD560 variant 0x0B at (561.2, 54.9, 260)
    "lift_r62": 0x7B52E0,         # overlay 0x825510 subtype 0x11 at (317, 14.9, 260)
    "console_r69": 0x7B6770,      # overlay 0x825DF0 (class 0x84) at (556.7, 29.9, 193)
    "reel_r70": 0x7B6A60,         # overlay 0x8260C0 at (509, 54.9, 260): the top of the conveyor
    "pick_g8": 0x7A6DC0,          # 0015AFA0 deferred 0x826930[8] at (542.3, 20.6, 156.2)
    "pick_g12": 0x7A7690,         # 0015AFA0 deferred 0x826930[12] at (514.3, 30, 192.9)
}
A04_SPANS = A01R_SPANS[:len(A00_SPANS[:len(A01_BASE_SPANS) + 10]) + 2] + [
    ("inv2", 0x810C84, 0x4),            # D_00810C84 (item 0x20 count) ..
    ("story764", 0x810764, 0x4),        # D_00810764..67 (764 = 0xFF after the door-[45] event)
    ("s830", 0x810830, 0x10),           # D_00810830..3F (83B = console [69], 834 / 83D / 83E switches)
]
for _name, _base in A04_OWNERS.items():
    A04_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                  (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                  (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]
A04_EVENT_KEYS = ("e4", "e5", "e9", "ea", "story764", "s830", "infected")


class A04Sampler(ExitSampler):
    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = A04_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))


def decode_a04(r: dict[str, bytes]) -> dict:
    row = decode_a01r(r, owners=A04_OWNERS)
    row["story764"] = r["story764"].hex()
    row["s830"] = r["s830"].hex()
    row["e4"] = r["d2"][0x0C:0x0D].hex()                # D_008107E4 (the director's counter)
    row["e5"] = r["d2"][0x0D:0x0E].hex()                # D_008107E5 (0x8241F0's bug group)
    row["e9"] = r["d2"][0x11:0x12].hex()                # D_008107E9 (the NPC [2] behind door [45])
    row["ea"] = r["d2"][0x12:0x13].hex()                # D_008107EA ([3] 0x8246B0)
    return row


def use_a04_sampler(r: Route) -> None:
    sampler = A04Sampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_a04(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


A04_GRABS = (0x3B, 0x3E)        # a bug on the player's back / a bite


def a04_shake(r: Route, limit: int = 300) -> None:
    """A bug on the player's back (action 0x3B) lets go when the left stick is
    rocked left and right; a bite (0x3E) plays out on its own."""
    for i in range(limit):
        action = r.rows[-1]["m1F0"]
        if action not in A04_GRABS:
            break
        if action == 0x3B:
            r.set_pad(0, 0x00 if (i // 2) % 2 else 0xFF, 0x7F)
        else:
            r.set_pad(0)
        r.step(1)
    r.set_pad(0)


def a04_go(r: Route, points, tol: float = 1.5, limit: int = 500, until=None) -> str:
    """walk_path for AREA04: a bug's grab interrupts the walk (a04_shake) and
    the walk resumes toward the same waypoint.  Returns 'until', 'ok' or
    'blocked@<index>' (the walk is then left where it stopped)."""
    for i, (x, z) in enumerate(points):
        history: list[tuple[float, float]] = []
        for _ in range(limit):
            row = r.rows[-1]
            if until is not None and until(row):
                r.set_pad(0)
                return "until"
            if row["m1F0"] in A04_GRABS:
                a04_shake(r)
                history = []
                continue
            if r.stick_toward(x, z) <= tol:
                break
            row = r.step(1)
            history.append((row["pos"][0], row["pos"][2]))
            if len(history) > 45 and math.hypot(history[-1][0] - history[-45][0],
                                                history[-1][1] - history[-45][1]) < 0.3:
                r.set_pad(0)
                return f"blocked@{i}"
    r.set_pad(0)
    return "ok"


def a04_use(r: Route, x: float, z: float, yaw: float, pred, tries: int = 5) -> None:
    """Approach (x, z), face `yaw`, Cross until `pred`; a bug's grab can take
    the press, so the approach is repeated."""
    for attempt in range(tries):
        a04_shake(r)
        approach(r, x, z)
        face(r, yaw)
        r.press("CROSS", 2)
        try:
            r.until(lambda row: pred(row) and row["m1F0"] not in A04_GRABS, 60)
            return
        except TimeoutError:
            a04_shake(r)
            r.until(lambda row: row["m1F0"] not in A04_GRABS, 600)
            settle(r, 10)
    raise TimeoutError("Use not taken: " + summary(r.rows[-1]))


def a04_program(row: dict) -> bool:
    """A door or talk program took the press (3B8D != 0 or action 0x41)."""
    return row["spad"][2:4] != "00" or row["m1F0"] == 0x41


A04_HALL_WEST = [(420, 350), (385, 330), (380, 290), (390, 250), (390, 220), (420, 180), (440, 150)]
A04_HALL_TO_CONVEYOR = [(450, 160), (420, 180), (390, 220), (392, 250), (394, 260)]
A04_CONVEYOR_UP = [(420, 260), (450, 260), (480, 260), (495, 260), (515, 262)]
A04_BALCONY_NORTH = [(543, 290), (545, 350), (542, 395)]
# The store room: from spawn entry 4 (516.5, 157) north of the pillar block
# (x 523..533, z 145..173) toward the east wall.
A04_STORE_NORTH = [(518, 165), (518, 178), (536, 178)]


def a04_beat_door45_event(r: Route) -> dict:
    # From the arrival (440.1, 14.9, 356.4) west round the tower (x 400..460,
    # z 238..300) and south to the director's quad 0x827CD0 (x 430..450,
    # z 115..137) in front of door [45]: D_008107E4 = 2, script 0x8278D0, at
    # its end D_00810845 |= 8 (door [40]) and the player placed at
    # (440.4, 14.9, 114.4) facing 0.
    use_a04_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A04_HALL_WEST + [(440, 132)], until=lambda row: row["e4"] != "01")
    if how != "until":
        raise RuntimeError("the director's quad not reached (" + how + "): " + summary(r.rows[-1]))
    r.set_pad(0)
    next_control_kept(r, 9000, 60)
    settle(r, 10)
    row = r.rows[-1]
    if row["e4"] != "02" or not int(row["locks"][10:12], 16) & 0x08:
        raise RuntimeError("door-[45] event effects not seen: " + summary(row))
    return {"what": "arrival -> west round the tower -> the director's quad at door [45]: script 0x8278D0, "
                    "D_008107E4 = 2, D_00810845 |= 8 (door [40])", "hp_end": a01_hp(r)}


def a04_beat_door40(r: Route) -> dict:
    # Door [40] (001BC350 model 0x15, room move id 3, unlocked by the event):
    # the hall side (501.5, 162) facing +x -> spawn entry 4 (516.5, 14.9, 157).
    use_a04_sampler(r)
    next_long_frames(r)
    a04_go(r, [(445, 135), (470, 158), (492, 162)])
    a04_use(r, 496.0, 162.0, math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:6] == "040004", 1500)
    next_control_kept(r, 3000, 40)
    settle(r, 10)
    return {"what": "door [40] (room move id 3): the store room behind it, spawn entry 4", "hp_end": a01_hp(r)}


def a04_beat_console(r: Route) -> dict:
    # The console [69] (overlay 0x825DF0, class 0x84) at (556.7, 29.9, 193):
    # its Use starts script 0x82C3B0; D_0081083B = 0xFF and the reel [70]
    # leaves the top of the conveyor for (380, 14.9, 260).
    use_a04_sampler(r)
    next_long_frames(r)
    a04_go(r, A04_STORE_NORTH + [(553, 176), (555, 186)])
    a04_use(r, 556.0, 189.0, 0.0, a04_program)
    next_control_kept(r, 4000, 40)
    settle(r, 10)
    row = r.rows[-1]
    if row["s830"][22:24] != "ff":
        raise RuntimeError("D_0081083B not 0xFF: " + summary(row))
    return {"what": "console [69]: script 0x82C3B0, D_0081083B = 0xFF, the reel [70] rolls down the conveyor",
            "reel_pos": row["reel_r70"]["pos"], "hp_end": a01_hp(r)}


def a04_beat_back_to_hall(r: Route) -> dict:
    # Back through door [40] from the store room (the pillar block x 523..533,
    # z 145..173 is passed on its north side): spawn entry 3 in the hall.
    use_a04_sampler(r)
    next_long_frames(r)
    a04_go(r, [(553, 176), (536, 177), (518, 177), (516, 162), (511, 161.5)])
    a04_use(r, 506.0, 161.5, -math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:6] == "040003", 1500)
    next_control_kept(r, 3000, 40)
    settle(r, 10)
    return {"what": "door [40] from the store room: back in the hall at spawn entry 3", "hp_end": a01_hp(r)}


def a04_beat_conveyor(r: Route) -> dict:
    # West round the pit to the foot of the conveyor (x 400, z 260), up it
    # (y 19 -> 54.9) now that the reel is gone, onto the east balcony and
    # north along it to door [37]'s corridor.
    use_a04_sampler(r)
    next_long_frames(r)
    a04_go(r, A04_HALL_TO_CONVEYOR)
    for _ in range(4):
        how = a04_go(r, A04_CONVEYOR_UP + A04_BALCONY_NORTH, limit=400)
        if how == "ok":
            break
    row = r.rows[-1]
    if abs(row["pos"][1] - 54.9) > 0.3 or math.hypot(row["pos"][0] - 542, row["pos"][2] - 395) > 4:
        raise RuntimeError("the balcony corridor not reached: " + summary(row))
    return {"what": "up the conveyor (the reel gone) onto the east balcony, north to door [37]",
            "hp_end": a01_hp(r)}


def a04_beat_progression_exit(r: Route) -> dict:
    # Door [37] (001BC350 model 0x03, door id 1|0x80, record 16 00 00 00):
    # area change to AREA22 entry 0, its arrival, control.
    use_a04_sampler(r)
    next_long_frames(r)
    a04_use(r, 540.5, 404.5, 0.0, a04_program)
    r.until(lambda row: row["area4"][:2] == "16", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "160000":
        raise RuntimeError("not at AREA22 sub 0 entry 0: " + summary(r.rows[-1]))
    return {"what": "door [37]: area change to AREA22 entry 0, arrival, control", "hp_end": a01_hp(r)}


def a04_beat_door45_locked(r: Route) -> dict:
    # Door [45] (overlay 0x823700) behind the player after the event: with
    # D_00810845 bit 5 clear its Use runs the talk turn 0x823580 (script
    # 0x8272A0, one message).
    use_a04_sampler(r)
    next_long_frames(r)
    face(r, math.pi)
    a04_use(r, r.rows[-1]["pos"][0], r.rows[-1]["pos"][2], math.pi, a04_program)
    next_control_kept(r, 3000, 60)
    settle(r, 10)
    if r.rows[-1]["area4"][:4] != "0400":
        raise RuntimeError("door [45] opened: " + summary(r.rows[-1]))
    return {"what": "Use at door [45] with D_00810845 bit 5 clear: the talk turn and one message",
            "hp_end": a01_hp(r)}


def a04_beat_reel_blocks(r: Route) -> dict:
    # Before the console: up the conveyor with the reel [70] still at its top
    # (509, 54.9, 260): the walk stops against it.
    use_a04_sampler(r)
    next_long_frames(r)
    a04_go(r, A04_HALL_TO_CONVEYOR)
    how = a04_go(r, A04_CONVEYOR_UP, limit=200)
    settle(r, 10)
    row = r.rows[-1]
    if how == "ok" or row["pos"][0] > 500:
        raise RuntimeError("the reel did not stop the walk: " + summary(row))
    return {"what": "up the conveyor before the console: stopped by the reel [70] at its top",
            "walk": how, "hp_end": a01_hp(r)}


def a04_beat_pickup(r: Route) -> dict:
    # The pickup 0x826930[12] (0015AFA0, class 0x87) on the store room's north
    # shelf at (514.3, 30, 192.9): Use from (514.3, 188.5) facing +z.
    use_a04_sampler(r)
    next_long_frames(r)
    a04_go(r, [(518, 165), (516, 187)])
    taken0, inv0 = r.rows[-1]["taken"], r.s.read(0x810C60, 0x60).hex()
    a04_use(r, 514.3, 188.0, 0.0, lambda row: not in_control(row))
    r.until(lambda row: row["ui"][2:4] == "03", 900)
    r.idle(90)
    r.press("TRIANGLE", 2)
    next_control_kept(r, 3000, 40)
    settle(r, 10)
    return {"what": "the pickup 0x826930[12] on the store room's shelf (item page, Triangle)",
            "taken_changed": r.rows[-1]["taken"] != taken0,
            "inventory_before": inv0, "inventory_after": r.s.read(0x810C60, 0x60).hex(), "hp_end": a01_hp(r)}


def a04_beat_door42_locked(r: Route) -> dict:
    # Door [42] (001BC350 model 0x15, door id 4|0x80: AREA20) in the store
    # room, D_00810845 bit 4 clear: the locked-door program.
    use_a04_sampler(r)
    next_long_frames(r)
    a04_go(r, A04_STORE_NORTH + [(553, 174), (555, 160), (555, 154)])
    a04_use(r, 555.5, 153.0, math.pi / 2, a04_program)
    next_control_kept(r, 3000, 60)
    settle(r, 10)
    if r.rows[-1]["area4"][:2] != "04":
        raise RuntimeError("door [42] opened: " + summary(r.rows[-1]))
    return {"what": "Use at the lock-gated door [42] (to AREA20): locked-door program", "hp_end": a01_hp(r)}


A04_BEATS = [
    ("a04_00_door45_event", "a02_05_progression_exit", a04_beat_door45_event),
    ("a04_01_door40", "a04_00_door45_event", a04_beat_door40),
    ("a04_02_console", "a04_01_door40", a04_beat_console),
    ("a04_03_back_to_hall", "a04_02_console", a04_beat_back_to_hall),
    ("a04_04_conveyor", "a04_03_back_to_hall", a04_beat_conveyor),
    ("a04_05_progression_exit", "a04_04_conveyor", a04_beat_progression_exit),
    # side beats
    ("a04_s0_door45_locked", "a04_00_door45_event", a04_beat_door45_locked),
    ("a04_s1_reel_blocks", "a04_00_door45_event", a04_beat_reel_blocks),
    ("a04_s2_pickup", "a04_01_door40", a04_beat_pickup),
    ("a04_s3_door42_locked", "a04_01_door40", a04_beat_door42_locked),
]
A04_SIDE_BEATS = {"a04_s0_door45_locked", "a04_s1_reel_blocks", "a04_s2_pickup", "a04_s3_door42_locked"}
A04_CHANGE_BEATS = {"a04_05_progression_exit"}         # the beat that leaves AREA04


def a04_selected(spec: str) -> list[tuple]:
    """`a04` = every AREA04 beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "a04" in wanted:
        return list(A04_BEATS)
    return [b for b in A04_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# ---------------------------------------------------------------------------
# AREA22 (opt-in group `a22`), s88 lane NEXT.  The beats start from the a04_05
# end snapshot: AREA22 (overlay id 0x13, whose text is the 0x40 entry pad plus
# the area init at runtime 0x823580, which boot 001E7780 calls for key 0x1600
# at the area load, i.e. before these beats start; docs/AREA22_OVERLAY.md)
# sub 0 at spawn entry 0 (335.7, 155, 483.1), control.  Outputs go to
# build/s87/route_a22/<beat>/ (ignored); described in the port's
# docs/SIXTH_LEVEL_ROUTE.md.  None of these beats runs under `--beats all`.
OUT_A22 = ROOT / "build/s87/route_a22"

# Owner nodes of the AREA22 sub-0 load, measured in the a04_05 end snapshot.
# Record numbers are placement table 0x823A20 [n] or deferred group 0x8236B0 g[n].
A22_OWNERS = {
    "pick_g0": 0x7A5640,          # 00219550 g[0] at (132.7, 239.1, 286.6), behind door [10]
    "pick_g1": 0x7A5930,          # 00219550 g[1] at (348.2, 155, 206.3)
    "pick_g2": 0x7A5C20,          # 00219550 g[2] at (317.9, 155, 339.4)
    "pick_g3": 0x7A5F10,          # 00219550 g[3] at (348, 155, 400.1)
    "pick_g4": 0x7A6200,          # 0015AFA0 g[4] at (99.8, 247.9, 301.4), behind door [10]
    "bug_g6": 0x7A67E0,           # 00128C10 g[6] at (276.2, 153.4, 227.1)
    "bug_g10": 0x7A73A0,          # 00128C10 g[10] at (138.2, 161.1, 231.5)
    "door6_r6": 0x7A9FB0,         # 001BC350 model 0x03, door id 0|0x80: AREA04 entry 1
    "door7_r7": 0x7AA2A0,         # 001BB860 model 0x09, room move id 1 (entries 1 / 2)
    "door8_r8": 0x7AA590,         # 001BB860 model 0x09, door id 2|0x80: AREA01 entry 6
    "reader_r9": 0x7AA880,        # 00158810 model 0x12 at (105.5, 254, 280): with item 0x23, D_00810857 bit 3
    "door10_r10": 0x7AAB70,       # 001BB860 model 0x16, room move id 3 (entries 3 / 4), lock bit 3
    "lamp_r11": 0x7AAE60,         # 00158BD0 at (115, 262, 280)
    "r13_158D30": 0x7AB440,       # 00158D30 at (115, 262, 283)
    "r17_156F30": 0x7AC000,       # 00156F30 model 0x2B at (347.7, 155, 293)
    "r18_156F30": 0x7AC2F0,       # 00156F30 model 0x2B at (324, 155, 279.1)
    "r19_1C1A80": 0x7AC5E0,       # 001C1A80 model 0x52 at (111.2, 155, 165.7)
    "r20_1C1A80": 0x7AC8D0,       # 001C1A80 model 0x52 at (247.9, 155, 230.2)
}
A22_SPANS = A04_SPANS[:len(A01R_SPANS[:len(A00_SPANS[:len(A01_BASE_SPANS) + 10]) + 2]) + 3] + [
    ("s768", 0x810768, 0x4),            # D_00810768..6B (76A = 00158810's model-0x2F latch)
    ("locks22", 0x810850, 0x8),         # D_00810850..57 (857 = D_00810841[22], AREA22's lock bits)
    ("inv3", 0x810C88, 0x4),            # D_00810C88.. (item 0x24 count ..)
]
for _name, _base in A22_OWNERS.items():
    A22_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                  (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                  (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]
A22_EVENT_KEYS = ("l857", "c87", "c88", "s768")


class A22Sampler(ExitSampler):
    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = A22_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))


def decode_a22(r: dict[str, bytes]) -> dict:
    row = decode_a01r(r, owners=A22_OWNERS)
    row["story764"] = r["story764"].hex()
    row["s830"] = r["s830"].hex()
    row["e4"] = r["d2"][0x0C:0x0D].hex()
    row["e5"] = r["d2"][0x0D:0x0E].hex()
    row["e9"] = r["d2"][0x11:0x12].hex()
    row["ea"] = r["d2"][0x12:0x13].hex()
    row["s768"] = r["s768"].hex()
    row["l857"] = r["locks22"][7:8].hex()               # D_00810857 (AREA22 door-lock bits)
    row["c87"] = r["inv2"][3]                           # D_00810C87 (item 0x23: 00158810 model 0x12's key)
    row["c88"] = r["inv3"][0]                           # D_00810C88 (item 0x24)
    return row


def use_a22_sampler(r: Route) -> None:
    sampler = A22Sampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_a22(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


# Paths (world x, z).  The north arm (x 313..353) is crossed between the drums
# 00156620 at (335.1, 427.5) / (351, 442) / (320, 396) and the crate 001551B0
# at (347.5, 398.4); the east-west corridor (z 200..240) holds boxes at
# x 280..290 / 190..209 (north half) and 240..250 (south half); the south arm
# (x 100..137.8) runs from z 240 to door [8] at z 78.5.
A22_NORTH_ARM = [(341, 470), (341, 435), (334, 415), (334, 372)]
A22_CORRIDOR = [(336, 300), (336, 232), (318, 213), (270, 213), (255, 224), (238, 224),
                (220, 212), (180, 212), (150, 213), (125, 208)]
A22_SOUTH_ARM = [(125, 190), (118, 176), (118, 150), (128, 125), (122, 100), (120, 88)]


def a22_beat_door7(r: Route) -> dict:
    # From the arrival (335.7, 155, 483.1) south along the north arm to door
    # [7] (001BB860 model 0x09, room move id 1, not lock-gated) at
    # (335.1, 155, 361): spawn entry 1 (335.1, 155, 349.2) facing pi.
    use_a22_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A22_NORTH_ARM)
    if how != "ok":
        raise RuntimeError("the north arm not crossed (" + how + "): " + summary(r.rows[-1]))
    a04_use(r, 335.1, 366.5, math.pi, a04_program)
    r.until(lambda row: row["area4"][:6] == "160001", 1500)
    next_control_kept(r, 3000, 40)
    settle(r, 10)
    return {"what": "the north arm, door [7] (room move id 1): spawn entry 1", "hp_end": a01_hp(r)}


def a22_beat_corridor(r: Route) -> dict:
    # From entry 1 south, west along the corridor round its boxes, and south
    # along the south arm to the front of door [8].
    use_a22_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A22_CORRIDOR + A22_SOUTH_ARM)
    if how != "ok":
        raise RuntimeError("the corridor not crossed (" + how + "): " + summary(r.rows[-1]))
    settle(r, 10)
    a01_near(r, 120, 88, 4, "door [8]'s front")
    return {"what": "south and west along the corridor, south along the south arm to door [8]",
            "hp_end": a01_hp(r)}


def a22_beat_progression_exit(r: Route) -> dict:
    # Door [8] (001BB860 model 0x09, door id 2|0x80, record 01 06 00 00; the
    # model is not lock-gated): area change to AREA01 entry 6 sub 0, arrival,
    # control.
    use_a22_sampler(r)
    next_long_frames(r)
    a04_use(r, 120.0, 84.0, math.pi, a04_program)
    r.until(lambda row: row["area4"][:2] == "01", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "010006":
        raise RuntimeError("not at AREA01 sub 0 entry 6: " + summary(r.rows[-1]))
    return {"what": "door [8]: area change to AREA01 entry 6, arrival, control", "hp_end": a01_hp(r)}


def a22_beat_pickup(r: Route) -> dict:
    # The pickup g[2] (00219550, param 0x72) at (317.9, 155, 339.4), west of
    # spawn entry 1: Use from (322, 339.4) facing -x, its page, Triangle.
    use_a22_sampler(r)
    next_long_frames(r)
    a04_go(r, [(324, 339.4)])
    approach(r, 322.0, 339.4)
    face(r, -math.pi / 2)
    taken0, docs0 = r.rows[-1]["taken"], r.rows[-1]["docs"]
    inv0 = r.s.read(0x810C60, 0x60).hex()
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: in_control(row) or row["ui"][2:4] == "03", 900)
    if not in_control(r.rows[-1]):
        r.idle(90)
        r.press("TRIANGLE", 2)
        r.until(in_control, 600)
    settle(r, 20)
    row = r.rows[-1]
    if row["pick_g2"]["cb"] == "0x219550":
        raise RuntimeError("the pickup g[2] is still there: " + summary(row))
    return {"what": "the pickup g[2] west of entry 1 (its page, Triangle)",
            "taken_changed": row["taken"] != taken0, "docs_before": docs0, "docs_after": row["docs"],
            "inventory_before": inv0, "inventory_after": r.s.read(0x810C60, 0x60).hex(), "hp_end": a01_hp(r)}


# From the front of door [8] back north to the ladder (attribute 0x32, the
# wall z 239 at x 111.5..118.5, y 155 -> 239) at the corner of the south arm.
A22_TO_LADDER = [(120, 100), (128, 125), (118, 150), (118, 176), (125, 190), (125, 215), (115, 230)]


def a22_beat_ladder_reader(r: Route) -> dict:
    # Up the ladder to the landing (y 239, x 100..130, z 240..280) and Use at
    # the reader [9] (00158810 model 0x12, facing -z at (105.5, 254, 280))
    # without item 0x23 (D_00810C87 = 0): 001576E0's refusal, D_00810857
    # unchanged.
    use_a22_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A22_TO_LADDER)
    if how != "ok":
        raise RuntimeError("the ladder foot not reached (" + how + "): " + summary(r.rows[-1]))
    approach(r, 115.0, 234.5)
    face(r, 0.0)
    ladder(r)
    settle(r, 10)
    if abs(r.rows[-1]["pos"][1] - 239.0) > 0.5:
        raise RuntimeError("not on the landing: " + summary(r.rows[-1]))
    a04_go(r, [(106, 262), (105.5, 272)])
    approach(r, 105.5, 274.5)
    face(r, 0.0)
    use_press(r, lambda row: not in_control(row))
    next_control_kept(r, 3000, 60)
    settle(r, 10)
    row = r.rows[-1]
    if row["l857"] != "00":
        raise RuntimeError("D_00810857 changed: " + summary(row))
    return {"what": "up the ladder to the landing; Use at the reader [9] without item 0x23: refused",
            "hp_end": a01_hp(r)}


def a22_beat_door10_locked(r: Route) -> dict:
    # Door [10] (001BB860 model 0x16, room move id 3) on the landing, with
    # D_00810857 bit 3 clear: the locked branch (001BB560 with 1).
    use_a22_sampler(r)
    next_long_frames(r)
    approach(r, 115.0, 276.0)
    a04_use(r, 115.0, 276.0, 0.0, a04_program)
    next_control_kept(r, 3000, 60)
    settle(r, 10)
    if abs(r.rows[-1]["pos"][2] - 276.0) > 3 or r.rows[-1]["area4"][:6] != "160003":
        raise RuntimeError("door [10] opened: " + summary(r.rows[-1]))
    return {"what": "Use at door [10] with D_00810857 bit 3 clear: one message, the door stays shut",
            "hp_end": a01_hp(r)}


def a22_beat_door6_back(r: Route) -> dict:
    # Door [6] (001BC350 model 0x03, door id 0|0x80, record 04 01 00 00),
    # behind the arrival: area change back to AREA04 entry 1, arrival, control.
    use_a22_sampler(r)
    next_long_frames(r)
    a04_go(r, [(338, 498)])
    a04_use(r, 339.7, 505.0, 0.0, a04_program)
    r.until(lambda row: row["area4"][:2] == "04", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "040001":
        raise RuntimeError("not at AREA04 sub 0 entry 1: " + summary(r.rows[-1]))
    return {"what": "door [6]: area change back to AREA04 entry 1, arrival, control", "hp_end": a01_hp(r)}


A22_BEATS = [
    ("a22_00_door7", "a04_05_progression_exit", a22_beat_door7),
    ("a22_01_corridor", "a22_00_door7", a22_beat_corridor),
    ("a22_02_progression_exit", "a22_01_corridor", a22_beat_progression_exit),
    # side beats
    ("a22_s0_pickup", "a22_00_door7", a22_beat_pickup),
    ("a22_s1_ladder_reader", "a22_01_corridor", a22_beat_ladder_reader),
    ("a22_s2_door10_locked", "a22_s1_ladder_reader", a22_beat_door10_locked),
    ("a22_s3_door6_back", "a04_05_progression_exit", a22_beat_door6_back),
]
A22_SIDE_BEATS = {"a22_s0_pickup", "a22_s1_ladder_reader", "a22_s2_door10_locked", "a22_s3_door6_back"}
A22_CHANGE_BEATS = {"a22_02_progression_exit", "a22_s3_door6_back"}   # the beats that leave AREA22


def a22_selected(spec: str) -> list[tuple]:
    """`a22` = every AREA22 beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "a22" in wanted:
        return list(A22_BEATS)
    return [b for b in A22_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# ---------------------------------------------------------------------------
# AREA01 upper floor (opt-in group `a01u`), s88 lane NEXT.  The beats start
# from the a22_02 end snapshot: AREA01 (overlay id 2) sub 0 at spawn entry 6
# (119, 60, -336), the platform at door [18], control at (119, 60, -349.1).
# Outputs go to build/s87/route_a01u/<beat>/ (ignored); described in the
# port's docs/SEVENTH_LEVEL_ROUTE.md.  None of these beats runs under
# `--beats all`.
OUT_A01U = ROOT / "build/s87/route_a01u"

# Owner nodes of the AREA01 sub-0 load made by the a22_02 arrival (placement
# table 0x82BD50 [n], deferred group 0x828A00 g[n]), measured in the a22_02
# end snapshot.  This load's pool addresses differ from the first visit's
# (A01_OWNERS) and the revisit's (A01R_OWNERS).
A01U_OWNERS = {
    "door18_r18": 0x7AFAC0,       # 001BB860 model 0x09, door id 5|0x80: AREA22 entry 5
    "door19_r19": 0x7AFDB0,       # 001BC350 model 0x03, door id 6|0x80: AREA06 entry 0
    "pick_g11": 0x7A7690,         # 0015AFA0 (class 0xC7) g[11] at (136.5, 60.1, -473.1)
    "pick_g13": 0x7A7C70,         # 0015AFA0 (class 0xC7) g[13] at (142.8, 2.1, -416.2), below the gap
    "n_g36_1C02E0": 0x7ABA20,     # 001C02E0 (kind 0x12) g[36] at (140.2, 66.5, -450.1)
    "n_1BFFD0": 0x7B8DA0,         # 001BFFD0, the companion 001C02E0 spawns
    "bug_g30": 0x7AA880,          # 00128C10 g[30] at (128.9, 82.6, -671.5)
    "bug_g31": 0x7AAB70,          # 00128C10 g[31] at (82.8, 75.9, -662.4)
    "bug_g32": 0x7AAE60,          # 00128C10 g[32] at (129.7, 60.4, -392.4)
    "bug_g33": 0x7AB150,          # 00128C10 g[33] at (133.9, 60.4, -383.8)
    "bug_g34": 0x7AB440,          # 00128C10 g[34] at (132.6, 60.4, -397.5)
    "bug_g35": 0x7AB730,          # 00128C10 g[35] at (125.3, 60.4, -395.4)
}
A01U_SPANS = A22_SPANS[:len(A04_SPANS[:len(A01R_SPANS[:len(A00_SPANS[:len(A01_BASE_SPANS) + 10]) + 2]) + 3]) + 3]
for _name, _base in A01U_OWNERS.items():
    A01U_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]
A01U_EVENT_KEYS = ("s766", "l845", "l847")


class A01USampler(ExitSampler):
    def __init__(self, session: OriginalSession, owners=None, spans=None):
        self.s = session
        self.spans = A01U_SPANS if spans is None else spans
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))


def decode_a01u(r: dict[str, bytes], owners=None) -> dict:
    row = decode_a01r(r, owners=A01U_OWNERS if owners is None else owners)
    row["story764"] = r["story764"].hex()
    row["s830"] = r["s830"].hex()
    row["e4"] = r["d2"][0x0C:0x0D].hex()
    row["e5"] = r["d2"][0x0D:0x0E].hex()
    row["e9"] = r["d2"][0x11:0x12].hex()
    row["ea"] = r["d2"][0x12:0x13].hex()
    row["s768"] = r["s768"].hex()
    row["l857"] = r["locks22"][7:8].hex()
    row["c87"] = r["inv2"][3]
    row["c88"] = r["inv3"][0]
    row["s766"] = r["story764"][2:3].hex()              # D_00810766 (001C02E0's 0xFF latch)
    row["l845"] = r["locks"][5:6].hex()                 # D_00810845 (AREA04 lock bits; 001C02E0 tests bit 5)
    row["l847"] = r["locks"][7:8].hex()                 # D_00810847 = D_00810841[6] (AREA06 lock bits)
    return row


def use_a01u_sampler(r: Route) -> None:
    sampler = A01USampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_a01u(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


# Paths (world x, z) on AREA01's upper floor (y 60).  The platform at door
# [18] is x 101..140, z -320..-400 (x 121..140 south of z -379); a gap
# (z -400..-440) separates it from the catwalk, which runs south along
# x 120..140 to z -640 and west along z -662..-678 to door [19] at x -109.5.
A01U_TO_EDGE = [(130, -360), (130, -385)]
A01U_CATWALK = [(130, -500), (130, -600), (129, -650), (110, -670), (40, -670)]
A01U_TO_DOOR19 = [(0, -670), (-75, -670), (-100, -670)]


def a01u_running_jump(r: Route, x: float, z_edge: float, z_far: float) -> None:
    """Run toward -z along x and press Cross once z <= z_edge: the running
    jump (0015EC50; action 0x0C, state 6, clips 0x69/0x6B), then the landing."""
    for _ in range(200):
        if r.rows[-1]["pos"][2] <= z_edge:
            break
        r.stick_toward(x, z_far)
        r.step(1)
    r.set_pad(PAD["CROSS"], r.pad_state[1], r.pad_state[2])
    r.step(2)
    r.set_pad(0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] == 0x0C, 10, 0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] not in (0x0C, 0x0F), 200, 0, r.pad_state[1], r.pad_state[2])
    r.set_pad(0)


def a01u_beat_gap_jump(r: Route) -> dict:
    # From entry 6's walk-in end (119, 60, -349.1) south along the platform to
    # its edge (z -400) and a running jump over the gap (z -400..-440) onto
    # the catwalk.  AREA01 is area 1: no 0015EC50 area box applies.
    use_a01u_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A01U_TO_EDGE)
    if how != "ok":
        raise RuntimeError("the platform edge not reached (" + how + "): " + summary(r.rows[-1]))
    face(r, math.pi)
    a01u_running_jump(r, 130.0, -397.5, -500.0)
    settle(r)
    row = r.rows[-1]
    if row["pos"][2] > -440 or abs(row["pos"][1] - 60.0) > 0.5:
        raise RuntimeError("the jump did not reach the catwalk: " + summary(row))
    return {"what": "south along the platform, running jump over the gap onto the catwalk", "hp_end": a01_hp(r)}


def a01u_beat_catwalk_event(r: Route) -> dict:
    # South along the catwalk and west along it into the quad 0x82CCA0
    # (x 21..35, z -680..-660): placement [45] (overlay 0x8267C0) starts
    # script 0x82B590 while story flag 15 (D_00810767) is clear and flag 7
    # (D_0081075F) is set; the script ends with D_00810767 = 0xFF.
    use_a01u_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A01U_CATWALK + [(20, -670)], until=lambda row: row["spad"][2:4] != "00")
    if how != "until":
        raise RuntimeError("the catwalk event did not start (" + how + "): " + summary(r.rows[-1]))
    r.set_pad(0)
    next_control_kept(r, 6000, 60)
    settle(r, 10)
    row = r.rows[-1]
    if row["story764"][6:8] != "ff":
        raise RuntimeError("D_00810767 not 0xFF: " + summary(row))
    return {"what": "south and west along the catwalk: [45]'s script 0x82B590 (a cutscene), D_00810767 = 0xFF",
            "hp_end": a01_hp(r)}


def a01u_beat_progression_exit(r: Route) -> dict:
    # West along the catwalk to door [19] (001BC350 model 0x03, door id 6|0x80,
    # record 06 00 00 00; the model is not lock-gated): area change to AREA06
    # entry 0 sub 0, arrival, control.
    use_a01u_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A01U_TO_DOOR19)
    if how != "ok":
        raise RuntimeError("door [19] not reached (" + how + "): " + summary(r.rows[-1]))
    a04_use(r, -103.5, -674.5, -math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:2] == "06", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "060000":
        raise RuntimeError("not at AREA06 sub 0 entry 0: " + summary(r.rows[-1]))
    return {"what": "door [19]: area change to AREA06 entry 0, arrival, control", "hp_end": a01_hp(r)}


def a01u_beat_pickup(r: Route) -> dict:
    # The pickup g[11] (0015AFA0, class 0xC7) on the catwalk at
    # (136.5, 60.1, -473.1): Use from (132, -473.1) facing +x.
    use_a01u_sampler(r)
    next_long_frames(r)
    a04_go(r, [(130, -465), (131, -473.1)])
    taken0, docs0 = r.rows[-1]["taken"], r.rows[-1]["docs"]
    inv0 = r.s.read(0x810C60, 0x60).hex()
    a04_use(r, 132.0, -473.1, math.pi / 2, lambda row: not in_control(row))
    r.until(lambda row: in_control(row) or row["ui"][2:4] == "03", 900)
    if not in_control(r.rows[-1]):
        r.idle(90)
        r.press("TRIANGLE", 2)
        r.until(in_control, 600)
    settle(r, 20)
    row = r.rows[-1]
    if row["pick_g11"]["cb"] == "0x15afa0":
        raise RuntimeError("the pickup g[11] is still there: " + summary(row))
    return {"what": "the pickup g[11] on the catwalk",
            "taken_changed": row["taken"] != taken0, "docs_before": docs0, "docs_after": row["docs"],
            "inventory_before": inv0, "inventory_after": r.s.read(0x810C60, 0x60).hex(), "hp_end": a01_hp(r)}


def a01u_beat_door18_back(r: Route) -> dict:
    # Door [18] (001BB860 model 0x09, door id 5|0x80, record 16 05 00 00),
    # behind the entry-6 arrival: area change back to AREA22 entry 5.
    use_a01u_sampler(r)
    next_long_frames(r)
    a04_use(r, 120.0, -324.0, 0.0, a04_program)
    r.until(lambda row: row["area4"][:2] == "16", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "160005":
        raise RuntimeError("not at AREA22 sub 0 entry 5: " + summary(r.rows[-1]))
    return {"what": "door [18]: area change back to AREA22 entry 5, arrival, control", "hp_end": a01_hp(r)}


A01U_BEATS = [
    ("a01u_00_gap_jump", "a22_02_progression_exit", a01u_beat_gap_jump),
    ("a01u_01_catwalk_event", "a01u_00_gap_jump", a01u_beat_catwalk_event),
    ("a01u_02_progression_exit", "a01u_01_catwalk_event", a01u_beat_progression_exit),
    # side beats
    ("a01u_s0_pickup", "a01u_00_gap_jump", a01u_beat_pickup),
    ("a01u_s1_door18_back", "a22_02_progression_exit", a01u_beat_door18_back),
]
A01U_SIDE_BEATS = {"a01u_s0_pickup", "a01u_s1_door18_back"}
A01U_CHANGE_BEATS = {"a01u_02_progression_exit", "a01u_s1_door18_back"}   # the beats that leave AREA01


def a01u_selected(spec: str) -> list[tuple]:
    """`a01u` = every AREA01 upper-floor beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "a01u" in wanted:
        return list(A01U_BEATS)
    return [b for b in A01U_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# ---------------------------------------------------------------------------
# AREA06 (opt-in group `a06`), s88 lane NEXT.  The beats start from the
# a01u_02 end snapshot: AREA06 (overlay id 6) sub 0 at spawn entry 0
# (-117, 60, -670.9), control at (-130.1, 60, -670.9).  Outputs go to
# build/s87/route_a06/<beat>/ (ignored); described in the port's
# docs/SEVENTH_LEVEL_ROUTE.md.  None of these beats runs under `--beats all`.
OUT_A06 = ROOT / "build/s87/route_a06"

# Owner nodes of the AREA06 sub-0 load (placement table 0x827AC0 [n],
# deferred group 0x826000 g[n]), measured in the a01u_02 end snapshot.
A06_OWNERS = {
    "door1_r1": 0x7AB730,         # 001BC350 model 0x03, door id 0|0x80: AREA01 entry 7
    "door2_r2": 0x7ABA20,         # 001BC350 model 0x03, room move id 1 (entries 2 / 1)
    "door3_r3": 0x7ABD10,         # 001BB860 model 0x3E, door id 2|0x80: AREA16 entry 0 (lock bit 2)
    "lamp_r4": 0x7AC000,          # 00158BD0 at (-240, 51, -799), above door [3]
    "keypad_r6": 0x7AC5E0,        # overlay 0x824340 (class 0x86) at (-306, 68, -650)
    "r7_1C50B0": 0x7AC8D0,        # 001C50B0 at (-306.5, 70, -650)
    "r8_823B50": 0x7ACBC0,        # overlay 0x823B50 at (-352.9, 75, -611)
    "r10_22DCD0": 0x7AD1A0,       # 0022DCD0 at (-340, 64.2, -591.5)
    "beam_r11": 0x7AD490,         # overlay 0x824560 at (-239.9, 50, -584)
    "crate_r31": 0x7B0C60,        # 001551B0 model 0x1E at (-339.6, 60, -609.6)
    "save_r43": 0x7B2FA0,         # 00159B90 at (-333.2, 68, -627.3)
    "r47_1E7D20": 0x7B3B60,       # 001E7D20 at (-260, 18.5, -781)
    "r54_15A2C0": 0x7B4FF0,       # 0015A2C0 at (-330.7, 60, -601.5)
    "r56_1C1A80": 0x7B55D0,       # 001C1A80 model 0x52 at (-250, 18.3, -778.5)
    "doc_g2": 0x7A5C20,           # 00219550 g[2] at (-343.7, 60, -674.5), in the room
    "pick_g8": 0x7A6DC0,          # 0015AFA0 g[8] at (-366.3, 60.4, -652)
    "g11_219870": 0x7A7690,       # 00219870 g[11] at (-350.5, 70, -670)
    "bug_g27": 0x7AA590,          # 00128C10 g[27] at (-345.6, 62.9, -573.2)
    "bug_g28": 0x7AA880,          # 00128C10 g[28] at (-335.6, 68.6, -570.4)
    "bug_g29": 0x7AAB70,          # 0012A5D0 g[29] at (-327.7, 60.4, -572.9)
}
A06_SPANS = A01U_SPANS[:len(A22_SPANS[:len(A04_SPANS[:len(A01R_SPANS[:len(A00_SPANS[:len(A01_BASE_SPANS) + 10]) + 2]) + 3]) + 3])] + [
    ("inv4", 0x810CC8, 0x4),            # D_00810CC8..CB (CCA = D_00810CC3[7])
    ("kpad", 0x28B020, 0x10),           # the status-UI object the keypad callback 00207350 runs on
    ("kpad2", 0x28B048, 0x4),           # its +0x28 cursor and +0x2A timer
]
for _name, _base in A06_OWNERS.items():
    A06_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                  (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                  (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]
A06_EVENT_KEYS = ("l847", "cca", "kpad")


class A06Sampler(ExitSampler):
    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = A06_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))


def decode_a06(r: dict[str, bytes]) -> dict:
    row = decode_a01u(r, owners=A06_OWNERS)
    row["cca"] = r["inv4"][2]                           # D_00810CCA (001C4760(7, 1) adds 1)
    k = r["kpad"]
    row["kpad"] = (k[4:7].hex() + "/" + k[0xA:0xC].hex()
                   + "/" + r["kpad2"][:2].hex())        # +4..6 state, +0xA count, +0xB slot, cursor
    return row


def use_a06_sampler(r: Route) -> None:
    sampler = A06Sampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_a06(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


def a06_settle(r: Route, frames: int = 10, limit: int = 1500) -> None:
    """settle() that shakes off a bug's grab (a04_shake) instead of waiting."""
    r.idle(frames)
    for _ in range(limit):
        row = r.rows[-1]
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        if in_control(row) and row["clip"] == 0:
            return
        r.step(1)
    raise TimeoutError("not settled: " + summary(r.rows[-1]))


def a06_face(r: Route, yaw: float, tol: float = 0.12, limit: int = 40) -> None:
    for _ in range(limit):
        row = r.rows[-1]
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        diff = (yaw - row["yaw"] + math.pi) % (2 * math.pi) - math.pi
        if abs(diff) <= tol:
            break
        px, _py, pz = row["pos"]
        r.stick_toward(px + 100 * math.sin(yaw), pz + 100 * math.cos(yaw), 0.6)
        r.step(1)
    a06_settle(r, 10)


def a06_approach(r: Route, x: float, z: float, tol: float = 0.6, limit: int = 200) -> None:
    for _ in range(limit):
        if r.rows[-1]["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        if r.stick_toward(x, z, 0.5) <= tol:
            break
        r.step(1)
    r.set_pad(0)
    a06_settle(r, 10)


def a06_use(r: Route, x: float, z: float, yaw: float, pred, tries: int = 5) -> None:
    """a04_use with the grab handling of a06_settle in every step."""
    for _ in range(tries):
        a04_go(r, [(x, z)], tol=1.5)
        a06_approach(r, x, z)
        a06_face(r, yaw)
        r.press("CROSS", 2)
        try:
            r.until(lambda row: pred(row) and row["m1F0"] not in A04_GRABS, 60)
            return
        except TimeoutError:
            a06_settle(r, 5)
    raise TimeoutError("Use not taken: " + summary(r.rows[-1]))


def a06_control(r: Route, limit: int, need: int = 60) -> None:
    """next_control_kept that shakes off a grab."""
    for _ in range(limit):
        row = r.rows[-1]
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        row = r.step(1)
        if in_control(row) and len(r.rows) > need and all(in_control(x) for x in r.rows[-need:]):
            return
    raise TimeoutError("control not kept; last " + summary(r.rows[-1]))


def a06_keypad_entry(r: Route, limit: int = 1500) -> int:
    """Enter, on the keypad page (00207350's 3x4 pad; cell digits from
    D_00265010, the cursor starting on cell 9), the string the page compares
    against (D_00275858[slot], read from RAM at run time; the tool holds no
    game data), then OK (cell 10).  Closed loop on the page object's cursor
    (+0x28) and character count (+0xA).  Returns the number of characters."""
    p = 0x28B020
    for _ in range(limit):
        head = r.s.read(p, 0x14)
        if struct.unpack_from("<I", head, 0x10)[0] == 0x207350 and head[4:7] == b"\x01\x00\x02":
            break
        r.step(1)
    else:
        raise TimeoutError("keypad not live: " + summary(r.rows[-1]))
    slot = r.s.read(p + 8, 4)[3]
    ptr = struct.unpack("<I", r.s.read(0x275858 + 4 * slot, 4))[0]
    raw = r.s.read(ptr, 16)
    code = raw[:raw.index(0)]
    table = list(r.s.read(0x265010, 12))[:11]
    cell_of = {d: c for c, d in enumerate(table[:10])}

    def state() -> tuple[int, int]:
        return struct.unpack("<h", r.s.read(p + 0x28, 4)[:2])[0], r.s.read(p + 8, 4)[2]

    def press_until(btn: str, pred, tries: int = 6) -> None:
        for _ in range(tries):
            r.press(btn, 2)
            for _ in range(8):
                if pred(state()):
                    r.idle(2)
                    return
                r.step(1)
        raise TimeoutError(f"keypad {btn} not taken: " + summary(r.rows[-1]))

    def row_col(c: int) -> tuple[int, int]:
        return (3, c - 9) if c >= 9 else (c // 3, c % 3)

    def go(target: int) -> None:
        cur = state()[0]
        while cur != target:
            (rc_, cc), (rt, ct) = row_col(cur), row_col(target)
            btn = "UP" if rt < rc_ else "DOWN" if rt > rc_ else "RIGHT" if ct > cc else "LEFT"
            before = cur
            press_until(btn, lambda st: st[0] != before)
            cur = state()[0]
    for ch in code:
        go(cell_of[ch - 0x30])
        n0 = state()[1]
        press_until("CROSS", lambda st: st[1] == n0 + 1)
    go(10)
    r.press("CROSS", 2)
    return len(code)


# Paths (world x, z).  East floor (y 60, x -180..-110): round the cable
# bundle (x -158..-149, down to the floor at z -675..-639) and the block
# x -159.5..-110, z -620..-610, to the north-east pad (x -200..-180,
# z -600..-570); the beam [11] crosses the pit at z ~ -586 to the west pad
# (x -300..-280).  West floor: the corridor between the machine block
# (x -370..-340, z -600..-583) and the room (x -350.5..-299.5,
# z -680.5..-619.5) is closed by the crate [31] at its east mouth and runs
# under the cable bundle (x -357.6..-348.5, bottom y 74.1) to x -362.
A06_TO_NE_PAD = [(-140, -665), (-140, -632), (-160, -630), (-176, -625), (-176, -600), (-185, -585), (-195, -585)]
# The beam is walked along its middle (z -584); a line nearer its south
# edge (z -586..-588) stepped down onto a lower part at x -251 and, one
# frame later in a replay, off the beam into the pit.
A06_BEAM_WEST = [(-215, -584), (-230, -584), (-245, -584), (-260, -584), (-275, -584), (-290, -585)]
A06_PAD_TO_CRATE = [(-310, -586), (-316, -611), (-326, -611)]
A06_CORRIDOR_WEST = [(-345, -611), (-362, -612), (-362, -630), (-362, -654.5)]
A06_CORRIDOR_EAST = [(-362, -630), (-362, -611), (-345, -611), (-316, -611), (-310, -586), (-298, -586), (-285, -586)]
A06_PIT_SOUTH = [(-250, -590), (-240, -620), (-240, -700), (-240, -770), (-240, -786)]


def a06_beat_beam(r: Route) -> dict:
    # From the arrival east to the north-east pad and west over the beam [11]
    # (overlay 0x824560, state 4 while D_00810845 bit 5 is clear) to the west
    # pad (y 60 at x -300..-280).
    use_a06_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A06_TO_NE_PAD, limit=400)
    if how == "ok":
        how = a04_go(r, A06_BEAM_WEST, tol=1.0, limit=400)
    if how != "ok":
        raise RuntimeError("the beam not crossed (" + how + "): " + summary(r.rows[-1]))
    a06_settle(r, 10)
    if abs(r.rows[-1]["pos"][1] - 60.0) > 0.5 or r.rows[-1]["pos"][0] > -285:
        raise RuntimeError("not on the west pad: " + summary(r.rows[-1]))
    return {"what": "east floor to the north-east pad, west over the beam [11] to the west pad", "hp_end": a01_hp(r)}


def a06_beat_crate_door2(r: Route) -> dict:
    # The crate [31] (001551B0 model 0x1E) closes the corridor to door [2]:
    # one second melee (Square, action 0x37) breaks it; west under the cable
    # bundle and door [2] (001BC350 model 0x03, room move id 1): entry 1
    # inside the room.
    use_a06_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A06_PAD_TO_CRATE, limit=300)
    if how != "ok":
        raise RuntimeError("the crate not reached (" + how + "): " + summary(r.rows[-1]))
    a06_face(r, -math.pi / 2)
    for _ in range(4):
        if not a00_crate_alive(r, A06_OWNERS["crate_r31"]):
            break
        r.press("SQUARE", 2, after=40)
    if a00_crate_alive(r, A06_OWNERS["crate_r31"]):
        raise RuntimeError("the crate [31] not broken: " + summary(r.rows[-1]))
    r.idle(60)
    how = a04_go(r, A06_CORRIDOR_WEST, limit=300)
    if how != "ok":
        raise RuntimeError("the corridor not crossed (" + how + "): " + summary(r.rows[-1]))
    a06_settle(r, 10)
    a06_use(r, -355.0, -654.5, math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:6] == "060001", 1500)
    a06_control(r, 3000, 40)
    a06_settle(r, 10)
    return {"what": "Square breaks the crate [31]; the corridor under the cable bundle; door [2] (room move id 1): "
                    "entry 1 inside the room", "hp_end": a01_hp(r)}


def a06_beat_keypad(r: Route) -> dict:
    # The keypad [6] (overlay 0x824340, class 0x86) on the room's east desk:
    # its Use starts script 0x826D40, whose op09 callback 0x8242C0 requests
    # the keypad page (D_008106C5 = 1); the matching string sets
    # D_00810845 |= 0x20 (00207350); at the script's end 001C4760(7, 1).
    use_a06_sampler(r)
    next_long_frames(r)
    a04_go(r, [(-320, -650)])
    a06_use(r, -315.0, -650.0, math.pi / 2, lambda row: not in_control(row))
    n = a06_keypad_entry(r)
    a06_control(r, 3000, 60)
    a06_settle(r, 10)
    row = r.rows[-1]
    if not int(row["locks"][10:12], 16) & 0x20:
        raise RuntimeError("D_00810845 bit 5 not set: " + summary(row))
    return {"what": "the keypad [6]: the page's string entered, D_00810845 |= 0x20", "characters": n,
            "hp_end": a01_hp(r)}


def a06_beat_room_out(r: Route) -> dict:
    # Door [2] from inside the room: entry 2 outside its west wall.
    use_a06_sampler(r)
    next_long_frames(r)
    a06_use(r, -342.5, -654.5, -math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:6] == "060002", 1500)
    a06_control(r, 3000, 40)
    a06_settle(r, 10)
    return {"what": "door [2] from inside the room: entry 2", "hp_end": a01_hp(r)}


def a06_beat_beam_collapse(r: Route) -> dict:
    # Back east through the corridor onto the beam: with D_00810845 bit 5
    # set the beam [11] runs its state 1; in its quad it starts script
    # 0x827180 (D_00810768 1 -> 0xFF) and the player ends in the pit.
    use_a06_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A06_CORRIDOR_EAST, limit=300)
    if how != "ok":
        raise RuntimeError("the west pad not reached (" + how + "): " + summary(r.rows[-1]))
    how = a04_go(r, [(-260, -584), (-230, -584)], tol=1.0, limit=300, until=lambda row: row["spad"][2:4] != "00")
    if how != "until":
        raise RuntimeError("the beam event did not start (" + how + "): " + summary(r.rows[-1]))
    r.set_pad(0)
    a06_control(r, 6000, 60)
    a06_settle(r, 10)
    row = r.rows[-1]
    if row["s768"][:2] != "ff":
        raise RuntimeError("D_00810768 not 0xFF: " + summary(row))
    return {"what": "back over the beam: [11]'s script 0x827180, D_00810768 = 0xFF, the fall toward the pit",
            "hp_end": a01_hp(r)}


def a06_beat_door3_locked(r: Route) -> dict:
    # Down into the pit and south to door [3] (001BB860 model 0x3E, door id
    # 2|0x80 -> AREA16 entry 0), with D_00810847 bit 2 clear: the locked branch.
    use_a06_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A06_PIT_SOUTH, limit=400)
    if how != "ok":
        raise RuntimeError("door [3] not reached (" + how + "): " + summary(r.rows[-1]))
    a06_settle(r, 10)
    a06_use(r, -240.0, -792.0, math.pi, a04_program)
    a06_control(r, 3000, 60)
    a06_settle(r, 10)
    if r.rows[-1]["area4"][:2] != "06":
        raise RuntimeError("door [3] opened: " + summary(r.rows[-1]))
    return {"what": "the pit, Use at door [3] with D_00810847 bit 2 clear: one message, the door stays shut",
            "hp_end": a01_hp(r)}


def a06_beat_door1_back(r: Route) -> dict:
    # Door [1] (001BC350 model 0x03, door id 0|0x80, record 01 07 00 00),
    # behind the arrival: area change back to AREA01 entry 7 (the catwalk).
    use_a06_sampler(r)
    next_long_frames(r)
    a06_use(r, -115.0, -674.5, math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:2] == "01", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "010007":
        raise RuntimeError("not at AREA01 sub 0 entry 7: " + summary(r.rows[-1]))
    return {"what": "door [1]: area change back to AREA01 entry 7, arrival, control", "hp_end": a01_hp(r)}


def a06_beat_bar(r: Route) -> dict:
    # After the keypad, outside the room: the raised block (attribute 0x3A
    # top at y 63) under the overhead bar (attribute 0x34, y 95.5, x -350..
    # -201.3): Use hangs from it (pole entry), the stick moves the player
    # east to its end; Cross lets go over the pit's east side.
    use_a06_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(-362, -665), (-362, -700), (-352, -722), (-338, -742)], limit=300)
    if how != "ok":
        raise RuntimeError("the block not reached (" + how + "): " + summary(r.rows[-1]))
    a06_settle(r, 10)
    a06_approach(r, -338.0, -755.0)
    r.press("CROSS", 2)
    r.until(lambda row: row["m1F0"] == 0x21, 90)
    for i in range(900):
        row = r.rows[-1]
        if row["pos"][0] >= -201.5 and i > 30:
            break
        r.stick_toward(row["pos"][0] + 100, row["pos"][2], 1.0)
        r.step(1)
    r.set_pad(0)
    r.idle(30)
    r.press("CROSS", 2)
    r.until(lambda row: row["pos"][1] < 60, 120)
    a06_settle(r, 30)
    return {"what": "the overhead bar west to east, let go over the pit's east side", "end": r.rows[-1]["pos"],
            "hp_end": a01_hp(r)}


A06_BEATS = [
    ("a06_00_beam", "a01u_02_progression_exit", a06_beat_beam),
    ("a06_01_crate_door2", "a06_00_beam", a06_beat_crate_door2),
    ("a06_02_keypad", "a06_01_crate_door2", a06_beat_keypad),
    ("a06_03_room_out", "a06_02_keypad", a06_beat_room_out),
    ("a06_04_beam_collapse", "a06_03_room_out", a06_beat_beam_collapse),
    ("a06_05_door3_locked", "a06_04_beam_collapse", a06_beat_door3_locked),
    # side beats
    ("a06_s0_door1_back", "a01u_02_progression_exit", a06_beat_door1_back),
    ("a06_s1_bar", "a06_03_room_out", a06_beat_bar),
]
A06_SIDE_BEATS = {"a06_s0_door1_back", "a06_s1_bar"}
A06_CHANGE_BEATS = {"a06_s0_door1_back"}          # the one beat that leaves AREA06


def a06_selected(spec: str) -> list[tuple]:
    """`a06` = every AREA06 beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "a06" in wanted:
        return list(A06_BEATS)
    return [b for b in A06_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# ---------------------------------------------------------------------------
# Eighth level (lane NEXT, s88; port docs/EIGHTH_LEVEL_ROUTE.md): the story
# continued from a06_03 (the keypad's bit set, outside the room) with the
# order the world graph gives (decomp docs/WORLD_GRAPH.md): back east over
# the beam without its collapse, AREA01's upper floor to door [18], AREA22
# to door [6], AREA04's door [45] (the NPC [2], item 0x23), the reader [50]
# and the lift [51] to AREA13 (its arrival; AREA13 itself is not yet a
# group).  Opt-in groups `a06b`, `a01v`, `a22b`, `a04b`.
#
# The beam [11] (overlay 0x824560, state 1 once D_00810845 bit 5 is set)
# starts its collapse script 0x827180 only while the player's +5
# (D_008102B5) is below 2 or in 29..34 inside its quad 0x827680 (x -273..
# -269, z -600..-570) at y >= 55; a running jump (0015EC50, +5 = 6) from the
# west pad's south part carries the player over the quad onto the beam's
# east part, and the beam stays in state 1.

OUT_A06B = ROOT / "build/s87/route_a06b"
A06B_TO_JUMP = A06_CORRIDOR_EAST[:-2] + [(-300, -592), (-297, -597)]
A06B_TO_EAST_FLOOR = [(-205, -584), (-195, -585), (-185, -590), (-176, -600), (-176, -625), (-160, -630),
                      (-140, -632), (-140, -665)]


def a06b_running_jump(r: Route, x_edge: float, tx: float, tz: float) -> None:
    """Run toward (tx, tz) and press Cross once x >= x_edge (the pad's lip):
    0015EC50's running jump (action 0x0C, +5 = 6), then the landing."""
    for _ in range(200):
        if r.rows[-1]["pos"][0] >= x_edge:
            break
        r.stick_toward(tx, tz)
        r.step(1)
    r.set_pad(PAD["CROSS"], r.pad_state[1], r.pad_state[2])
    r.step(2)
    r.set_pad(0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] == 0x0C, 10, 0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] != 0x0C, 200, 0, r.pad_state[1], r.pad_state[2])
    r.set_pad(0)                # the stick is released at the landing (the beam is narrow)
    r.until(lambda row: row["m1F0"] != 0x0F, 200)


def a06b_beat_trigger_jump(r: Route) -> dict:
    # From outside the room (entry 2) east through the corridor to the west
    # pad's south part (z -592..-597; the beam lies at z about -584), face
    # east-north-east and jump from the lip (x -281) onto the beam's east
    # part: the quad 0x827680 is crossed in the jump (+5 = 6), so the beam
    # keeps state 1 and D_00810768 stays 0.  Then east onto the north-east
    # pad and round the block to the east floor.
    use_a06_sampler(r)
    next_long_frames(r)
    a04_go(r, A06B_TO_JUMP, limit=300)            # ends against the pad's south railing
    a06_settle(r, 10)
    a06_face(r, math.atan2(57.0, 13.0))
    a06b_running_jump(r, -281.0, -240.0, -584.0)
    a06_settle(r, 10)
    row = r.rows[-1]
    if row["s768"][:2] != "00" or row["pos"][0] < -240 or row["pos"][1] < 50:
        raise RuntimeError("the jump did not land past the beam's quad: " + summary(row))
    landed = [round(v, 1) for v in row["pos"]]
    how = a04_go(r, A06B_TO_EAST_FLOOR, tol=1.0, limit=400)
    if how != "ok":
        raise RuntimeError("the east floor not reached (" + how + "): " + summary(r.rows[-1]))
    a06_settle(r, 10)
    if r.rows[-1]["s768"][:2] != "00":
        raise RuntimeError("the beam collapsed: " + summary(r.rows[-1]))
    return {"what": "running jump from the west pad over the beam's trigger quad onto its east part; "
                    "east pad and east floor, D_00810768 still 0", "landed": landed, "hp_end": a01_hp(r)}


def a06b_beat_door1(r: Route) -> dict:
    # Door [1] (001BC350 model 0x03, door id 0|0x80, record 01 07 00 00):
    # area change to AREA01 entry 7 (the catwalk west of door [19]).
    use_a06_sampler(r)
    next_long_frames(r)
    a06_use(r, -115.0, -674.5, math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:2] == "01", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "010007":
        raise RuntimeError("not at AREA01 sub 0 entry 7: " + summary(r.rows[-1]))
    return {"what": "door [1]: area change to AREA01 entry 7 with D_00810845 bit 5 set, arrival, control",
            "hp_end": a01_hp(r)}


A06B_BEATS = [
    ("a06b_00_trigger_jump", "a06_03_room_out", a06b_beat_trigger_jump),
    ("a06b_01_door1", "a06b_00_trigger_jump", a06b_beat_door1),
]
A06B_SIDE_BEATS: set[str] = set()
A06B_CHANGE_BEATS = {"a06b_01_door1"}


def a06b_selected(spec: str) -> list[tuple]:
    wanted = spec.split(",")
    if "a06b" in wanted:
        return list(A06B_BEATS)
    return [b for b in A06B_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# AREA01's upper floor again, from entry 7 (the catwalk west of door [19])
# with D_00810845 bit 5 set.  Pool nodes of this AREA01 sub-0 load, measured
# in the a06b_01 end snapshot (area_overview --ram): the gap node g[36]
# (001C02E0) has already taken its bit-5 branch (behaviour 001BF6B0) and
# spawned group 0x829110 (001B6660).
OUT_A01V = ROOT / "build/s87/route_a01v"
A01V_OWNERS = {
    "door18_r18": 0x7AEC10,       # 001BB860 model 0x09, door id 5|0x80: AREA22 entry 5
    "door19_r19": 0x7AEF00,       # 001BC350 model 0x03, door id 6|0x80: AREA06 entry 0
    "r45_8267C0": 0x7B3B60,       # overlay 0x8267C0 [45] (the catwalk event, flag 0xF already set)
    "pick_g11": 0x7A7690,         # 0015AFA0 g[11] at (136.5, 60.1, -473.1)
    "gap_g36_1BF6B0": 0x7AAB70,   # g[36] after 001C02E0's bit-5 branch: 001BF6B0 at (140.2, 66.5, -450.1)
    "n_1BFFD0": 0x7A6200,         # 001BFFD0 at (140.2, 66.5, -450.1)
    "n_1BE6C0": 0x7A6AD0,         # 001BE6C0 at (138.2, 66.5, -445.1)
    "bug_a": 0x7B7EF0,            # 0012A5D0 at (138.5, 60.4, -449.1) (group 0x829110)
    "bug_b": 0x7B81E0,            # 0012A5D0 at (129.9, 60.4, -391.3)
    "bug_c": 0x7B84D0,            # 0012A5D0 at (134.5, 60.4, -370.3)
    "bug_g30": 0x7AA880,          # 00128C10 g[30] at (128.9, 82.6, -671.5)
}
A01V_SPANS = list(A01U_SPANS[:len(A22_SPANS[:len(A04_SPANS[:len(A01R_SPANS[:len(A00_SPANS[:len(A01_BASE_SPANS) + 10]) + 2]) + 3]) + 3])])
for _name, _base in A01V_OWNERS.items():
    A01V_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]


def use_a01v_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A01V_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a01u(sampler.raw(), owners=A01V_OWNERS)
    r.rows[0] = dict(r.now(), f=0)


A01V_CATWALK_EAST = [(-40, -670), (40, -670), (110, -670), (129, -650), (130, -600), (130, -520), (130, -470)]


def a01v_beat_catwalk_east(r: Route) -> dict:
    # From entry 7's walk-in end east along the catwalk and north along x 130
    # to z -470, short of the gap (z -400..-440).
    use_a01v_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A01V_CATWALK_EAST, limit=600)
    if how != "ok":
        raise RuntimeError("the catwalk not walked (" + how + "): " + summary(r.rows[-1]))
    a06_settle(r, 10)
    return {"what": "east along the catwalk and north to z -470", "hp_end": a01_hp(r)}


def a01v_beat_gap_jump(r: Route) -> dict:
    # North to the catwalk's lip (z -440) and a running jump over the gap
    # onto the platform (z -400; the reverse of a01u_00).
    use_a01v_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(130, -455)], limit=300)
    if how != "ok":
        raise RuntimeError("the lip not reached (" + how + "): " + summary(r.rows[-1]))
    a06_settle(r, 10)
    a06_face(r, 0.0)
    for _ in range(200):
        if r.rows[-1]["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        if r.rows[-1]["pos"][2] >= -441.5:
            break
        r.stick_toward(130.0, -350.0)
        r.step(1)
    r.set_pad(PAD["CROSS"], r.pad_state[1], r.pad_state[2])
    r.step(2)
    r.set_pad(0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] == 0x0C, 10, 0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] != 0x0C, 200, 0, r.pad_state[1], r.pad_state[2])
    r.set_pad(0)
    r.until(lambda row: row["m1F0"] != 0x0F, 200)
    a06_settle(r, 10)
    row = r.rows[-1]
    if row["pos"][2] < -400 or abs(row["pos"][1] - 60.0) > 0.6:
        raise RuntimeError("the jump did not reach the platform: " + summary(row))
    return {"what": "running jump north over the gap onto the platform", "hp_end": a01_hp(r)}


def a01v_beat_door18(r: Route) -> dict:
    # North along the platform to door [18] (001BB860 model 0x09, door id
    # 5|0x80, record 16 05 00 00): area change to AREA22 entry 5.
    use_a01v_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(130, -380), (125, -345), (120, -330)], limit=400)
    if how != "ok":
        raise RuntimeError("door [18] not reached (" + how + "): " + summary(r.rows[-1]))
    a06_use(r, 120.0, -324.0, 0.0, a04_program)
    r.until(lambda row: row["area4"][:2] == "16", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "160005":
        raise RuntimeError("not at AREA22 sub 0 entry 5: " + summary(r.rows[-1]))
    return {"what": "door [18]: area change to AREA22 entry 5, arrival, control", "hp_end": a01_hp(r)}


A01V_BEATS = [
    ("a01v_00_catwalk_east", "a06b_01_door1", a01v_beat_catwalk_east),
    ("a01v_01_gap_jump", "a01v_00_catwalk_east", a01v_beat_gap_jump),
    ("a01v_02_door18", "a01v_01_gap_jump", a01v_beat_door18),
]
A01V_SIDE_BEATS: set[str] = set()
A01V_CHANGE_BEATS = {"a01v_02_door18"}


def a01v_selected(spec: str) -> list[tuple]:
    wanted = spec.split(",")
    if "a01v" in wanted:
        return list(A01V_BEATS)
    return [b for b in A01V_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# AREA22 again, from entry 5 (door [8]'s landing) to door [6]: the reverse of
# a22_00..a22_02.  Pool nodes of this AREA22 load (the a01v_02 end snapshot).
OUT_A22B = ROOT / "build/s87/route_a22b"
A22B_OWNERS = {
    "pick_g0": 0x7A5640,          # 00219550 g[0] at (132.7, 239.1, 286.6), behind door [10]
    "pick_g4": 0x7A6200,          # 0015AFA0 g[4] at (99.8, 247.9, 301.4), behind door [10]
    "door6_r6": 0x7A9CC0,         # 001BC350 model 0x03, door id 0|0x80: AREA04 entry 1
    "door7_r7": 0x7A9FB0,         # 001BB860 model 0x09, room move id 1 (entries 1 / 2)
    "door8_r8": 0x7AA2A0,         # 001BB860 model 0x09, door id 2|0x80: AREA01 entry 6
    "reader_r9": 0x7AA590,        # 00158810 model 0x12 (item 0x23 -> D_00810857 bit 3)
    "door10_r10": 0x7AA880,       # 001BB860 model 0x16, room move id 3, lock bit 3
    "lamp_r11": 0x7AAB70,         # 00158BD0 above door [10]
}
A22B_SPANS = list(A22_SPANS)
for _name, _base in A22B_OWNERS.items():
    A22B_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]


def use_a22b_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A22B_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a01u(sampler.raw(), owners=A22B_OWNERS)
    r.rows[0] = dict(r.now(), f=0)


A22B_SOUTH_ARM_NORTH = [(128, 125), (118, 150), (118, 176), (125, 190), (125, 208)]
A22B_CORRIDOR_EAST = [(150, 213), (180, 212), (220, 212), (238, 224), (255, 224), (270, 213), (318, 213),
                      (336, 232), (336, 300), (335, 345)]


def a22b_beat_to_door7(r: Route) -> dict:
    # From entry 5 north up the south arm, east along the corridor and north
    # to door [7] (001BB860 model 0x09, room move id 1) from its south side:
    # spawn entry 2 (335.1, 155, 373.2).
    use_a22b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A22B_SOUTH_ARM_NORTH + A22B_CORRIDOR_EAST, limit=600)
    if how != "ok":
        raise RuntimeError("the corridor not crossed (" + how + "): " + summary(r.rows[-1]))
    a06_use(r, 335.1, 355.5, 0.0, a04_program)
    r.until(lambda row: row["area4"][:6] == "160002", 1500)
    a06_control(r, 3000, 40)
    a06_settle(r, 10)
    return {"what": "south arm, corridor, door [7] from the south: entry 2", "hp_end": a01_hp(r)}


def a22b_beat_door6(r: Route) -> dict:
    # North along the north arm to door [6] (001BC350 model 0x03, door id
    # 0|0x80, record 04 01 00 00): area change to AREA04 entry 1.
    use_a22b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(334, 415), (341, 435), (341, 470), (338, 498)], limit=400)
    if how != "ok":
        raise RuntimeError("door [6] not reached (" + how + "): " + summary(r.rows[-1]))
    a06_use(r, 339.7, 505.0, 0.0, a04_program)
    r.until(lambda row: row["area4"][:2] == "04", 2500)
    next_control_kept(r, 12000, 60)
    settle(r, 30)
    if r.rows[-1]["area4"][:6] != "040001":
        raise RuntimeError("not at AREA04 sub 0 entry 1: " + summary(r.rows[-1]))
    return {"what": "door [6]: area change to AREA04 entry 1, arrival, control", "hp_end": a01_hp(r)}


A22B_BEATS = [
    ("a22b_00_to_door7", "a01v_02_door18", a22b_beat_to_door7),
    ("a22b_01_door6", "a22b_00_to_door7", a22b_beat_door6),
]
A22B_SIDE_BEATS: set[str] = set()
A22B_CHANGE_BEATS = {"a22b_01_door6"}


def a22b_selected(spec: str) -> list[tuple]:
    wanted = spec.split(",")
    if "a22b" in wanted:
        return list(A22B_BEATS)
    return [b for b in A22B_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


# AREA04 again, from entry 1 (the east balcony's north end) with D_00810845
# bit 5 set.  Pool nodes of this AREA04 sub-0 load (the a22b_01 end
# snapshot); the director [1] freed itself at the load (bit 5 set).
OUT_A04B = ROOT / "build/s87/route_a04b"
A04B_OWNERS = {
    "npc_r2": 0x7AA2A0,           # overlay 0x824320 (class 0xAA) at (400, 15, 100): gives item 0x23
    "r3_8246B0": 0x7AA590,        # overlay 0x8246B0 at (322, 15, 224): runs while flag 0x12 (D_0081076A)
    "r4_824DC0": 0x7AA880,        # overlay 0x824DC0 at (560, 69.9, 284): runs while flag 0x14 == 1
    "door37_r37": 0x7B0970,       # 001BC350 model 0x03, door id 1|0x80: AREA22 entry 0
    "door45_r45": 0x7B20F0,       # overlay 0x823700 model 0x16, room move id 5 (lock bit 5)
    "r47_823EE0": 0x7B26D0,       # overlay 0x823EE0 at door [45]
    "reader_r50": 0x7B2FA0,       # 00158810 model 0x2F at (560, 69.9, 284): item 0x23 -> lock bit 7, flag 0x12 = 1
    "lift_r51": 0x7B3290,         # 001BD560 model 0x0B (keyed on flag 0x14), door id 7|0x80: AREA13 entry 0
    "button_r53": 0x7B3870,       # 001BC960 model 0 at (560, 69.9, 281.2), bit 7
    "button_r54": 0x7B3B60,       # 001BC960 model 1 at (561.2, 69.9, 242), bit 7
    "lift_r56": 0x7B4140,         # 001BD560 model 0x0D, door id 0|0x80: AREA07 entry 0
    "socket_r55": 0x7B3E50,       # 00159E70 model 0x3C at (414, 14.9, 24.8): item 0x29 -> lock bit 0
    "button_r58": 0x7B4720,       # 001BDFC0 model 0 at (444, 29.9, 26), bit 0
    "reel_r70": 0x7B6A60,         # overlay 0x8260C0 at (380, 14.9, 260) (already rolled down)
}
A04B_SPANS = list(A01U_SPANS[:len(A22_SPANS[:len(A04_SPANS[:len(A01R_SPANS[:len(A00_SPANS[:len(A01_BASE_SPANS) + 10]) + 2]) + 3]) + 3])]) + [
    ("s76c", 0x81076C, 0x4),            # D_0081076C (flag 0x14, the lift key) ..
    ("s7ef", 0x8107EC, 0x4),            # D_008107EC..EF (counters 0x14..0x17)
]
for _name, _base in A04B_OWNERS.items():
    A04B_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]


def decode_a04b(r: dict[str, bytes], owners=None) -> dict:
    row = decode_a01u(r, owners=A04B_OWNERS if owners is None else owners)
    row["s76c"] = r["s76c"].hex()                       # D_0081076C..6F
    row["l84c"] = r["locks"][0xC:0xD].hex()             # D_0081084C
    return row


def use_a04b_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A04B_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a04b(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


A04B_BALCONY_TO_CONVEYOR = [(545, 350), (543, 290), (525, 262), (515, 262)]
A04B_CONVEYOR_DOWN = [(495, 260), (480, 260), (450, 260), (420, 260), (400, 260)]
A04B_HALL_TO_DOOR45 = [(395, 250), (391, 220), (420, 180), (440, 150), (440, 125)]


def a04b_beat_to_door45(r: Route) -> dict:
    # From entry 1 south along the east balcony, down the conveyor (the reel
    # rolled down in a04_02), west round the tower to door [45] (overlay
    # 0x823700; with D_00810845 bit 5 set it takes the open branch): the room
    # move to entry 9 behind it.
    use_a04b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A04B_BALCONY_TO_CONVEYOR + A04B_CONVEYOR_DOWN + A04B_HALL_TO_DOOR45, limit=600)
    if how != "ok":
        raise RuntimeError("door [45] not reached (" + how + "): " + summary(r.rows[-1]))
    a06_use(r, 440.0, 116.0, math.pi, a04_program)
    r.until(lambda row: row["area4"][:6] == "040009", 1500)
    a06_control(r, 3000, 40)
    a06_settle(r, 10)
    return {"what": "the balcony, the conveyor down, the hall, door [45] (bit 5 set): entry 9", "hp_end": a01_hp(r)}


def a04b_control(r: Route, limit: int, need: int = 60) -> None:
    """a06_control that also closes a status page an item take opens (ui byte
    1 = 3): 90 frames, then Triangle (as a01u_s0)."""
    for _ in range(limit):
        row = r.rows[-1]
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        if row["ui"][2:4] == "03":
            r.idle(90)
            r.press("TRIANGLE", 2)
            r.until(lambda x: x["ui"][2:4] != "03", 900)
            continue
        row = r.step(1)
        if in_control(row) and len(r.rows) > need and all(in_control(x) for x in r.rows[-need:]):
            return
    raise TimeoutError("control not kept; last " + summary(r.rows[-1]))


def a04b_beat_npc(r: Route) -> dict:
    # West into the NPC [2]'s quad 0x828220 (x 390..420, z 78..105): its
    # 0x824490 starts script 0x827D90 and, at the script's end, gives item
    # 0x23 (001C47A0(0x23, 1)), 001C4760(8, 1) and D_008107E9 = 1.
    use_a04b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(460, 95), (462, 75), (430, 70), (410, 70), (408, 86)], limit=300,
                 until=lambda row: row["spad"][2:4] != "00")
    if how != "until":
        raise RuntimeError("the NPC's script did not start (" + how + "): " + summary(r.rows[-1]))
    r.set_pad(0)
    a04b_control(r, 12000, 60)          # the cinematic's camera track runs 4141 units at 0.5 a frame
    a06_settle(r, 10)
    row = r.rows[-1]
    if row["c87"] != 1:
        raise RuntimeError("item 0x23 not given: " + summary(row))
    return {"what": "the NPC [2]'s quad: script 0x827D90, item 0x23", "hp_end": a01_hp(r)}


def a04b_beat_door45_out(r: Route) -> dict:
    # Back round the room's blocks to door [45] from its south side: the
    # room move to entry 8 in the hall.
    use_a04b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(430, 70), (462, 75), (460, 95), (441, 100)], limit=300)
    if how != "ok":
        raise RuntimeError("door [45] not reached (" + how + "): " + summary(r.rows[-1]))
    a06_use(r, 440.0, 102.5, 0.0, a04_program)
    r.until(lambda row: row["area4"][:6] == "040008", 1500)
    a04b_control(r, 3000, 40)
    a06_settle(r, 10)
    return {"what": "door [45] from the room: entry 8", "hp_end": a01_hp(r)}


A04B_HALL_TO_BALCONY = [(440, 150), (420, 180), (391, 220), (395, 250), (400, 260), (420, 260), (450, 260),
                        (480, 260), (495, 260), (515, 262), (535, 270), (548, 284)]


def a04b_beat_reader(r: Route) -> dict:
    # Up the conveyor to the east balcony and the reader [50] (00158810 model
    # 0x2F at (560, 69.9, 284)): with item 0x23 its 001576E0 starts script
    # 0x246C20, whose op09 001580C0 sets D_00810845 bit 7; 00158810 stores
    # D_0081076A (flag 0x12) = 1.
    use_a04b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A04B_HALL_TO_BALCONY, limit=600)
    if how != "ok":
        raise RuntimeError("the reader not reached (" + how + "): " + summary(r.rows[-1]))
    a06_use(r, 553.0, 284.0, math.pi / 2, lambda row: not in_control(row))
    a04b_control(r, 12000, 60)
    a06_settle(r, 10)
    row = r.rows[-1]
    if not int(row["l845"], 16) & 0x80:
        raise RuntimeError("D_00810845 bit 7 not set: " + summary(row))
    return {"what": "the conveyor, the balcony, the reader [50] with item 0x23: D_00810845 bit 7",
            "hp_end": a01_hp(r)}


def a04b_beat_lift(r: Route) -> dict:
    # Into the lift [51]'s car to its inner call button [54] (001BC960 model
    # 1 at (561.2, 69.9, 242), yaw +pi/2).  After the reader, [54] waits in
    # its state 2 sub-state 1; with D_00810845 bit 7 set, flag 0x12 not 0xFF,
    # counter 0x12 (D_008107EA) = 0x10 and D_008106C0 = 0 it offers Use every
    # frame (001BC740).  Use facing -x (the button's yaw + pi): state 1
    # sub-state 1 (script), then sub-state 6 moves the lift's +0x0B from 3 to
    # 4; the keyed lift's first ride sets flag 0x14 (D_0081076C) to 1 and
    # waits in its sub-state 7 for [4] 0x824DC0's script 0x828BE0 to store
    # 0xFF; then the request 0D FF 00 01: AREA13 entry 0.
    use_a04b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(552, 272), (561, 262), (566, 250), (566, 244)], limit=300)
    if how != "ok":
        raise RuntimeError("the lift's button [54] not reached (" + how + "): " + summary(r.rows[-1]))
    a06_use(r, 566.0, 243.0, -math.pi / 2, lambda row: row["button_r54"]["h"][8:10] == "01")
    r.until(lambda row: row["area4"][:2] == "0d", 3000)
    a04b_control(r, 3000, 60)
    a06_settle(r, 10)
    return {"what": "the lift [51]'s inner button [54] facing -x: flag 0x14, [4]'s script, AREA13 entry 0",
            "hp_end": a01_hp(r)}


A04B_BEATS = [
    ("a04b_00_door45", "a22b_01_door6", a04b_beat_to_door45),
    ("a04b_01_npc", "a04b_00_door45", a04b_beat_npc),
    ("a04b_02_door45_out", "a04b_01_npc", a04b_beat_door45_out),
    ("a04b_03_reader", "a04b_02_door45_out", a04b_beat_reader),
    ("a04b_04_lift", "a04b_03_reader", a04b_beat_lift),
]
A04B_SIDE_BEATS: set[str] = set()
A04B_CHANGE_BEATS: set[str] = {"a04b_04_lift"}


def a04b_selected(spec: str) -> list[tuple]:
    wanted = spec.split(",")
    if "a04b" in wanted:
        return list(A04B_BEATS)
    return [b for b in A04B_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def eighth_selected(spec: str) -> list[tuple]:
    """Every eighth-level group, in story order (each group opt-in by name)."""
    return a06b_selected(spec) + a01v_selected(spec) + a22b_selected(spec) + a04b_selected(spec)


def eighth_owners(name: str):
    if name.startswith("a06b_"):
        return A06_OWNERS
    if name.startswith("a01v_"):
        return A01V_OWNERS
    if name.startswith("a22b_"):
        return A22B_OWNERS
    if name.startswith("a04b_"):
        return A04B_OWNERS
    return None


# ---------------------------------------------------------------------------
# Ninth level (lane A13CAP, s89; port docs/NINTH_LEVEL_ROUTE.md): AREA13 from
# the lift's arrival (a04b_04_lift's end, entry 0) on, with the order the
# world graph gives (decomp docs/WORLD_GRAPH.md section 7 step 4).  Opt-in
# group `a13`.  Pool nodes of this AREA13 sub-0 load (placement table
# 0x82D570 [n], deferred group 0x829D00 g[n]), measured in the a04b_04 end
# snapshot with tools/area_overview.py --area 13 --ram.
OUT_A13 = ROOT / "build/s87/route_a13"
A13_OWNERS = {
    "r3_823700": 0x7AAB70,        # overlay 0x823700 (class 0xAA) at (677.3, 160, 1202): flag 0x19
    "r4_823A40": 0x7AAE60,        # overlay 0x823A40 at (760.5, 174, 1277): flag 0x1A, item 0x1A
    "hole_r5": 0x7AB150,          # overlay 0x823BC0 +0x0D 0 at (720.1, 160, 1262): off with item 0x27 or flag 0x1B
    "hole_r6": 0x7AB440,          # overlay 0x823BC0 +0x0D 1 at (1081, 160, 845)
    "r7_824A80": 0x7AB730,        # overlay 0x824A80 at (688, 163, 1041): counter 0x42
    "door8_r8": 0x7ABA20,         # 001BC350 model 0x03, room move id 1 (entries 2 / 1)
    "lift_r10": 0x7AC000,         # 001BD560 model 0x0B, door id 0|0x80: AREA04 entry 7
    "button_r13": 0x7AC8D0,       # 001BC960 model 1 (inner), id 0xFF
    "door14_r14": 0x7ACBC0,       # 001BDE60 model 5 at (688.7, 160, 1161.1)
    "button_r15": 0x7ACEB0,       # 001BD9F0 model 3 (south side; locked while flag 0x1C == 1), room move id 2
    "button_r16": 0x7AD1A0,       # 001BD9F0 model 4 (north side), room move id 2 (entries 8 / 3)
    "door17_r17": 0x7AD490,       # overlay 0x823580 model 0x03, room move id 3 (entries 9 / 4; locked while flag 0x1C == 1)
    "door20_r20": 0x7ADD60,       # 001BC350 model 0x03, room move id 4 (entries 5 / 10)
    "r44_823E90": 0x7B23E0,       # overlay 0x823E90 at (798.4, 215, 1149.5): counter / flag 0x1C
    "hatch_r62": 0x7B52E0,        # overlay 0x826850 model 0x35 at (720.1, 160, 1262): item 0x27, counter 0x61
    "hatch_r63": 0x7B55D0,        # overlay 0x826850 model 0x35 at (1081, 160, 845)
    "pick_g5": 0x7A64F0,          # 00219550 g[5] at (773.6, 160.2, 1274.3): item 0x27
    "pick_g8": 0x7A6DC0,          # 00219550 g[8] at (655.5, 160.2, 1283.1): item 0x1E
    "bug_g25": 0x7A9FB0,          # 0012A5D0 g[25]
    "bug_g26": 0x7AA2A0,          # 0012A5D0 g[26]
    "bug_g27": 0x7AA590,          # 00128C10 g[27]
    "bug_g28": 0x7AA880,          # 00128C10 g[28]
    "c_g21": 0x7A93F0,            # overlay 0x824BB0 g[21] at (724.9, 205, 1280)
}
A13_SPANS = [sp for sp in A04B_SPANS if ":" not in sp[0]] + [
    ("s770", 0x810770, 0x8),            # D_00810770..77 (flags 0x18..0x1F; 774 = flag 0x1C)
    ("s7f0", 0x8107F0, 0x8),            # D_008107F0..F7 (counters 0x18..0x1F; 7F4 = counter 0x1C)
    ("s798", 0x810798, 0x4),            # D_00810798..9B (flag 0x42 = 79A)
    ("s818", 0x810818, 0x4),            # D_00810818..1B (counter 0x42 = 81A)
    ("l848", 0x810848, 0x8),            # D_00810848..4F (84E = AREA13's lock byte)
    ("cc0", 0x810CC0, 0x10),            # D_00810CC0..CF (the CC3 table from +3)
]
for _name, _base in A13_OWNERS.items():
    A13_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                  (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                  (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]
A13_EVENT_KEYS = ("s770", "s7f0", "s798", "s818", "l84e", "c8b", "c82", "c7e", "c86")


def decode_a13(r: dict[str, bytes], owners=None) -> dict:
    row = decode_a04b(r, owners=A13_OWNERS if owners is None else owners)
    row["s770"] = r["s770"].hex()                       # flags 0x18..0x1F
    row["s7f0"] = r["s7f0"].hex()                       # counters 0x18..0x1F
    row["s798"] = r["s798"].hex()
    row["s818"] = r["s818"].hex()
    row["l84e"] = r["l848"][6:7].hex()                  # D_0081084E (AREA13 lock bits)
    row["c8b"] = r["inv3"][3]                           # D_00810C8B (item 0x27)
    row["c82"] = r["inv"][0x1E]                         # D_00810C82 (item 0x1E)
    row["c7e"] = r["inv"][0x1A]                         # D_00810C7E (item 0x1A)
    row["c86"] = r["inv2"][2]                           # D_00810C86 (item 0x22)
    row["cc0"] = r["cc0"].hex()
    row["c61"] = r["s830"][9:10].hex()                  # D_00810839 (counter 0x61)
    row["event230"] = struct.unpack_from("<i", r["player"], 0x230)[0]
    return row


def use_a13_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A13_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a13(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


def a13_settle(r: Route, frames: int = 10, limit: int = 1500) -> None:
    """a06_settle for AREA13: outdoors the idle player plays the cold clips
    (20 / 349) instead of clip 0, so control is action 0 with 3B8D == 0."""
    r.idle(frames)
    for _ in range(limit):
        row = r.rows[-1]
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        if in_control(row) and row["m1F0"] == 0:
            return
        r.step(1)
    raise TimeoutError("not settled: " + summary(r.rows[-1]))


def a13_face(r: Route, yaw: float, tol: float = 0.12, limit: int = 40) -> None:
    for _ in range(limit):
        row = r.rows[-1]
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        diff = (yaw - row["yaw"] + math.pi) % (2 * math.pi) - math.pi
        if abs(diff) <= tol:
            break
        px, _py, pz = row["pos"]
        r.stick_toward(px + 100 * math.sin(yaw), pz + 100 * math.cos(yaw), 0.6)
        r.step(1)
    r.set_pad(0)
    a13_settle(r, 10)


def a13_use(r: Route, x: float, z: float, yaw: float, pred, tries: int = 5) -> None:
    """a06_use with a13_settle: walk to (x, z), face `yaw`, Cross until `pred`
    (a press right after the player settles can be missed, so it retries)."""
    for _ in range(tries):
        a04_go(r, [(x, z)], tol=1.5)
        for _ in range(200):
            if r.rows[-1]["m1F0"] in A04_GRABS:
                a04_shake(r)
                continue
            if r.stick_toward(x, z, 0.5) <= 0.6:
                break
            r.step(1)
        r.set_pad(0)
        a13_settle(r, 10)
        a13_face(r, yaw)
        r.idle(3)
        r.press("CROSS", 2)
        try:
            r.until(lambda row: pred(row) and row["m1F0"] not in A04_GRABS, 60)
            return
        except TimeoutError:
            a13_settle(r, 5)
    raise TimeoutError("Use not taken: " + summary(r.rows[-1]))


def a13_control(r: Route, limit: int, need: int = 60) -> None:
    """a04b_control (grabs shaken off, an item page closed with Triangle after
    90 frames) that accepts the outdoor cold clips."""
    for _ in range(limit):
        row = r.rows[-1]
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        if row["ui"][2:4] == "03":
            r.idle(90)
            r.press("TRIANGLE", 2)
            r.until(lambda x: x["ui"][2:4] != "03", 900)
            continue
        row = r.step(1)
        if in_control(row) and len(r.rows) > need and all(in_control(x) for x in r.rows[-need:]):
            return
    raise TimeoutError("control not kept; last " + summary(r.rows[-1]))


A13_CAR_TO_DOOR8 = [(634, 1258), (650, 1258), (662, 1260), (680, 1250), (680, 1228), (670, 1220)]


def a13_beat_door8(r: Route) -> dict:
    # Out of the lift [10]'s car through its opening (z about 1258; the car's
    # east wall stops a walk at x 640.2 further north), across the lobby to
    # door [8] (001BC350 model 0x03, room move id 1) from its north side:
    # entry 2.  [3] (overlay 0x823700) then starts its scene at once: flag
    # 0x19 = 1, script 0x82A360, flag 0x19 = 0xFF and counter 0x19 = 1 at its
    # end, and an item page (Triangle).
    use_a13_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A13_CAR_TO_DOOR8, limit=300)
    if how != "ok":
        raise RuntimeError("door [8] not reached (" + how + "): " + summary(r.rows[-1]))
    a13_use(r, 669.7, 1215.0, math.pi, a04_program)
    r.until(lambda row: row["area4"][:6] == "0d0002", 1500)
    a13_control(r, 12000, 60)
    a13_settle(r, 10)
    row = r.rows[-1]
    if row["s770"][2:4] != "ff":
        raise RuntimeError("[3]'s scene did not end (flag 0x19): " + summary(row))
    return {"what": "out of the lift car, the lobby, door [8] (entry 2); [3]'s scene: flag 0x19 1 -> 0xFF",
            "hp_end": a01_hp(r)}


def a13_beat_door14(r: Route) -> dict:
    # The button [16] (001BD9F0 model 4 at (665.7, 175, 1164.6), yaw 0) of the
    # door [14] (001BDE60): Use facing -z; the room move id 2 (record 08 03)
    # with the side latch 0: entry 8 outside, south of the building.
    use_a13_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(675, 1180), (666, 1172)], limit=300)
    if how != "ok":
        raise RuntimeError("button [16] not reached (" + how + "): " + summary(r.rows[-1]))
    a13_use(r, 665.7, 1170.0, math.pi, lambda row: row["button_r16"]["h"][10:12] != "00")
    r.until(lambda row: row["area4"][:6] == "0d0008", 1500)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    return {"what": "button [16] facing -z: door [14], room move to entry 8 (outside)", "hp_end": a01_hp(r)}


A13_OUT_TO_DOOR17 = [(740, 1150), (770, 1180), (795, 1222), (790, 1250), (787, 1262)]


def a13_beat_door17(r: Route) -> dict:
    # Outdoors (the cold lowers the health about 1 per 360 frames while
    # outside, measured) east along z 1150 and north past the building's
    # south-east corner to door [17] (overlay 0x823580, room move id 3,
    # record 09 04; locked only while flag 0x1C == 1) from the east: entry 4.
    # [4] (overlay 0x823A40) starts its scene at the arrival: item 0x1A
    # (D_00810C7E = 1) with its equipment page (Triangle), flag and counter
    # 0x1A = 0xFF.
    use_a13_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A13_OUT_TO_DOOR17, limit=400)
    if how != "ok":
        raise RuntimeError("door [17] not reached (" + how + "): " + summary(r.rows[-1]))
    a13_use(r, 785.5, 1263.0, -math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:6] == "0d0004", 1500)
    a13_control(r, 12000, 60)
    a13_settle(r, 10)
    row = r.rows[-1]
    if row["c7e"] != 1 or row["s770"][4:6] != "ff":
        raise RuntimeError("[4]'s scene did not end (item 0x1A / flag 0x1A): " + summary(row))
    return {"what": "outdoors to door [17] from the east (entry 4); [4]'s scene: item 0x1A, flag 0x1A = 0xFF",
            "hp_end": a01_hp(r)}


def a13_beat_item27(r: Route) -> dict:
    # The pickup g[5] (00219550, item 0x27) at (773.6, 160.2, 1274.3): Use
    # from (769.5, 1274.3) facing +x.  With item 0x27 held both holes [5] / [6]
    # (0x823BC0) end (state 3) and the hatches [62] / [63] (0x826850) take
    # class 0x84 (offered for Use).
    use_a13_sampler(r)
    next_long_frames(r)
    a04_go(r, [(768, 1274)], limit=200)
    a13_use(r, 769.5, 1274.3, math.pi / 2, lambda row: not in_control(row))
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    row = r.rows[-1]
    if row["c8b"] != 1:
        raise RuntimeError("item 0x27 not taken: " + summary(row))
    return {"what": "the pickup g[5]: item 0x27", "hp_end": a01_hp(r)}


def a13_beat_hatch(r: Route) -> dict:
    # West over the grating step (x 742.4..751, z 1237.6..1277, top y 168.4;
    # a box x 713..751.3, z 1213..1238 closes the room's south part): a ledge
    # climb facing -x (action 8), walk off its west side, then the hatch
    # [62]'s Use point (its descriptor 0x82CDD0: (720, 160.5, 1253.7), radius
    # 10, yaw -3.072; the player faces +z): script 0x82CA50, the hatch opens
    # (state 2) and counter 0x61 (D_00810839) |= 1.
    use_a13_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(756, 1260), (745, 1260)], limit=120)
    if not how.startswith("blocked") and how != "ok":
        raise RuntimeError("the step not reached (" + how + "): " + summary(r.rows[-1]))
    a13_face(r, -math.pi / 2)
    use_press(r, lambda row: row["m1F0"] == 8, tries=4, wait=30)
    r.until(lambda row: row["m1F0"] != 8, 300)
    a13_settle(r, 10)
    if r.rows[-1]["pos"][1] < 168.0:
        raise RuntimeError("not on the step: " + summary(r.rows[-1]))
    how = a04_go(r, [(738, 1260)], limit=120)
    a13_settle(r, 20)
    a04_go(r, [(720, 1254)], limit=150, tol=0.8)
    a13_use(r, 720.0, 1253.7, 0.07, lambda row: row["hatch_r62"]["h"][10:12] != "00")
    r.until(lambda row: row["hatch_r62"]["h"][8:10] == "02", 1500)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    row = r.rows[-1]
    if not int(row["c61"], 16) & 1:
        raise RuntimeError("counter 0x61 bit 0 not set: " + summary(row))
    return {"what": "over the grating step (ledge climb), the hatch [62] with item 0x27: open, counter 0x61 |= 1",
            "hp_end": a01_hp(r)}


def a13_beat_shaft(r: Route) -> dict:
    # Use facing +z in the open hatch: the ladder (action 0x16, then 0x17);
    # at y 143.1 (entry 6's point) 00193EB0 sees the player at y <= 159 with
    # spawn entry 4 and requests AREA19 sub 0 entry 9 (13 00 09 01).  AREA19
    # entry 9 starts on a ladder (action 0x44, then 0x17); the stick held
    # down climbs to its foot (action 0x18) and control.
    use_a13_sampler(r)
    next_long_frames(r)
    a13_face(r, 0.0)
    use_press(r, lambda row: row["m1F0"] == 0x16, tries=4, wait=30)
    r.until(lambda row: row["area4"][:2] == "13", 1500)
    r.until(lambda row: row["m1F0"] == 0x17 and row["spad"][2:4] == "00", 3000)
    r.set_pad(0, 0x7F, 0xFF)
    r.until(lambda row: row["m1F0"] == 0 and row["p5"] == 0, 1500, 0, 0x7F, 0xFF)
    r.set_pad(0)
    a13_control(r, 1500, 60)
    a13_settle(r, 10)
    if r.rows[-1]["area4"][:6] != "130009":
        raise RuntimeError("not at AREA19 sub 0 entry 9: " + summary(r.rows[-1]))
    return {"what": "the hatch's ladder: 00193EB0's request to AREA19 entry 9, the AREA19 ladder down, control",
            "hp_end": a01_hp(r)}


A13_BEATS: list[tuple] = [
    ("a13_00_door8", "a04b_04_lift", a13_beat_door8),
    ("a13_01_door14", "a13_00_door8", a13_beat_door14),
    ("a13_02_door17", "a13_01_door14", a13_beat_door17),
    ("a13_03_item27", "a13_02_door17", a13_beat_item27),
    ("a13_04_hatch", "a13_03_item27", a13_beat_hatch),
    ("a13_05_shaft", "a13_04_hatch", a13_beat_shaft),
]
A13_SIDE_BEATS: set[str] = set()
A13_CHANGE_BEATS: set[str] = {"a13_05_shaft"}


def a13_selected(spec: str) -> list[tuple]:
    wanted = spec.split(",")
    if "a13" in wanted:
        return [b for b in A13_BEATS if b[0] not in A13_SIDE_BEATS]
    return [b for b in A13_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def ninth_owners(name: str):
    if name.startswith("a13_"):
        return A13_OWNERS
    return None


# ---------------------------------------------------------------------------
# Tenth level (lane A19CAP, s90; port docs/TENTH_LEVEL_ROUTE.md): AREA19 from
# the foot of its entry-9 ladder (a13_05_shaft's end) on, with the order the
# world graph gives (decomp docs/WORLD_GRAPH.md section 7 step 5), then back
# up into AREA13 and down its lift [10] to AREA04.  Opt-in groups `a19`
# (AREA19, overlay id 16) and `a13b` (AREA13 again, overlay id 10).  Pool
# nodes measured with tools/area_overview.py --area 19 --ram
# build/s87/route_a13/a13_05_shaft/eeMemory.bin (placement table 0x82E3D0
# [n], deferred group 0x829E00 g[n]).
OUT_A19 = ROOT / "build/s87/route_a19"
A19_OWNERS = {
    "r6_8250F0": 0x7AD490,        # overlay 0x8250F0 at (956, 210, 1006.5): D_008107F5 (flag / counter 0x1D)
    "r7_8257C0": 0x7AD780,        # overlay 0x8257C0 at (913.7, 198.2, 919.6): spawn entry 0xA, D_008107F6
    "r10_827790": 0x7AE050,       # overlay 0x827790 (class 9): spawn entry 0xD
    "reader_r21": 0x7B00A0,       # 00158810 model 0x13 at (770, 235, 1044): item 0x24 -> lock bit 0
    "door22_r22": 0x7B0390,       # 001BB860 model 0x16, room move id 0 (entries 6 / 5; lock bit 0)
    "panel_r24": 0x7B0970,        # 00158EC0 model 0x22 at (1013, 280, 1126.9): item 0x1B -> lock bit 1
    "door25_r25": 0x7B0C60,       # 001BB860 model 0x16, door id 1|0x80: AREA03 entry 1 (lock bit 1)
    "door27_r27": 0x7B1240,       # overlay 0x823580 model 0x15, room move id 2 (entries 1 / 2; lock bit 2)
    "door32_r32": 0x7B20F0,       # 001BC350 model 0x03, room move id 5 (entries 8 / 7)
    "r33_159970": 0x7B23E0,       # 00159970 model 0x37 at (717.5, 212.8, 1055.5)
    "pick_g2": 0x7A5C20,          # 00219550 g[2] at (750.3, 200, 1051.8)
    "pick_g12": 0x7A7980,         # 0015AFA0 g[12] at (649.4, 231.2, 1143.4)
    "g41_1C1030": 0x7ACEB0,       # 001C1030 g[41] (model 0x51) at (735, 260, 1209.3)
}
A19_SPANS = [sp for sp in A13_SPANS if ":" not in sp[0]] + [
    ("s778", 0x810778, 0x4),            # D_00810778..7B (flags 0x20..0x23)
    ("s7f8", 0x8107F8, 0x4),            # D_008107F8..FB (counters 0x20..0x23)
    ("s79c", 0x81079C, 0x4),            # D_0081079C..9F (flags 0x44..0x47)
    ("s81c", 0x81081C, 0x4),            # D_0081081C..1F (counters 0x44..0x47)
    ("ca8", 0x810CA8, 0x4),             # D_00810CA8 (halfword)
]
for _name, _base in A19_OWNERS.items():
    A19_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                  (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                  (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]
A19_EVENT_KEYS = ("s778", "s7f8", "s79c", "s81c", "l854", "ca8", "c88")


def decode_a19(r: dict[str, bytes], owners=None) -> dict:
    row = decode_a13(r, owners=A19_OWNERS if owners is None else owners)
    row["s778"] = r["s778"].hex()
    row["s7f8"] = r["s7f8"].hex()
    row["s79c"] = r["s79c"].hex()
    row["s81c"] = r["s81c"].hex()
    row["l854"] = r["locks22"][4:5].hex()               # D_00810854 (AREA19 lock bits)
    row["ca8"] = struct.unpack_from("<H", r["ca8"], 0)[0]
    return row


def use_a19_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A19_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a19(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


def a19_press_until(r: Route, pred, tries: int = 4, wait: int = 60) -> None:
    """Cross after 3 idle frames until `pred` (a press right after the player
    settles is not always taken, NINTH_LEVEL_ROUTE.md section 1)."""
    for _ in range(tries):
        r.idle(3)
        r.press("CROSS", 2)
        for _ in range(wait):
            if pred(r.rows[-1]):
                return
            r.step(1)
        if pred(r.rows[-1]):
            return
        a13_settle(r, 5)
    raise TimeoutError("Cross not taken: " + summary(r.rows[-1]))


def a19_page(r: Route, limit: int = 300) -> None:
    """A status page (ui byte 1 = 3): 90 frames, then Triangle."""
    r.until(lambda row: row["ui"][2:4] == "03", limit)
    r.idle(90)
    r.press("TRIANGLE", 2)
    r.until(lambda x: x["ui"][2:4] != "03", 900)


A19_E9_TO_DUCT = [(715, 1245), (715, 1232), (701, 1232), (700.5, 1226)]


def a19_beat_duct(r: Route) -> dict:
    # From the foot of the entry-9 ladder (710, 240, 1256.8) west to the duct
    # square (attribute 0x37, x 697..705, z 1220..1230, the a01_s5 duct's
    # entry kind): Cross facing -z enters the crawl (actions 0x2C, 0x2D); the
    # stick up crawls south to (703, 220, 1068) and, turned east, out through
    # the exit (action 0x2E) into the room south of door [32] (y 200), where
    # the spawn entry byte D_00810702 becomes 8.  An exploration walk south on
    # the ramp east of the duct stopped at (731.7, 242, 1217.4); a collision
    # scan (a lead) still reaches door [32]'s north side that way, so the
    # stop does not prove the corridor closed.
    use_a19_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A19_E9_TO_DUCT, tol=1.0, limit=200)
    if how != "ok":
        raise RuntimeError("duct square not reached (" + how + "): " + summary(r.rows[-1]))
    a13_settle(r, 5)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["m1F0"] in (0x2C, 0x2D))
    r.until(lambda row: row["m1F0"] == 0x2D, 300)
    r.idle(90)                                    # the entry clip ignores the stick
    a01_crawl(r, 80)                              # south to (703, 1068)
    a01_turn_crawl(r, math.pi / 2)
    a01_crawl(r, 40)                              # east, out through the exit
    r.until(in_control, 600)
    a13_settle(r, 20)
    row = r.rows[-1]
    if row["area4"][:6] != "130008" or row["pos"][1] > 201:
        raise RuntimeError("not out of the duct in the entry-8 room: " + summary(row))
    return {"what": "the duct from the ladder room south to the room behind door [32] (D_00810702 = 8)",
            "hp_end": a01_hp(r)}


def a19_beat_pickup_g2(r: Route) -> dict:
    # The pickup g[2] (00219550 at (750.3, 200, 1051.8)): Cross from
    # (744.7, 1057.7) facing it (yaw 2.37): its page (status request 01 12)
    # and D_00810CA8 0 -> 12.
    use_a19_sampler(r)
    next_long_frames(r)
    a04_go(r, [(740, 1062), (746, 1056)], tol=1.0, limit=200)
    a13_settle(r, 5)
    a13_face(r, 2.37)
    a19_press_until(r, lambda row: not in_control(row))
    a19_page(r)
    a13_control(r, 1500, 40)
    a13_settle(r, 10)
    if r.rows[-1]["ca8"] != 12:
        raise RuntimeError("g[2] not taken: " + summary(r.rows[-1]))
    return {"what": "the pickup g[2]: D_00810CA8 0 -> 12", "hp_end": a01_hp(r)}


def a19_beat_duct_back(r: Route) -> dict:
    # Back into the duct from its south exit square (x 715..725, z 1064..1072,
    # entered facing -x) and north to the ladder room, then to the ladder's
    # foot (710, 240, 1256.8) facing +z.
    use_a19_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(735, 1066), (726, 1068)], tol=1.0, limit=200)
    if how != "ok":
        raise RuntimeError("duct exit not reached (" + how + "): " + summary(r.rows[-1]))
    a13_settle(r, 5)
    a13_face(r, -math.pi / 2)
    a19_press_until(r, lambda row: row["m1F0"] in (0x2C, 0x2D))
    r.until(lambda row: row["m1F0"] == 0x2D, 300)
    r.idle(90)
    a01_crawl(r, 40)                              # west to (703, 1068)
    a01_turn_crawl(r, 0.0)
    a01_crawl(r, 80)                              # north, out into the ladder room
    r.until(in_control, 600)
    a13_settle(r, 20)
    how = a04_go(r, [(705, 1240), (710, 1252), (710, 1256.5)], tol=0.8, limit=200)
    a13_settle(r, 10)
    a13_face(r, 0.0)
    row = r.rows[-1]
    if row["pos"][1] < 239 or abs(row["pos"][0] - 710) > 2 or row["pos"][2] < 1252:
        raise RuntimeError("not at the ladder's foot: " + summary(row))
    return {"what": "the duct back north to the ladder room; the ladder's foot", "hp_end": a01_hp(r)}


A19_BEATS: list[tuple] = [
    ("a19_00_duct", "a13_05_shaft", a19_beat_duct),
    ("a19_01_pickup_g2", "a19_00_duct", a19_beat_pickup_g2),
    ("a19_02_duct_back", "a19_01_pickup_g2", a19_beat_duct_back),
]
A19_SIDE_BEATS: set[str] = set()
A19_CHANGE_BEATS: set[str] = set()


# AREA13 again, from the AREA19 ladder (a19_02's end).  Pool nodes of this
# AREA13 sub-0 load (entry 6; [3] and [4] freed themselves at the load, so
# every later record sits one or two nodes lower than in the a13 load),
# measured with tools/area_overview.py --area 13 --ram on its first snapshot.
OUT_A13B = ROOT / "build/s87/route_a13b"
A13B_OWNERS = {
    "door8_r8": 0x7AB730,         # 001BC350 model 0x03, room move id 1 (entries 2 / 1)
    "lift_r10": 0x7ABD10,         # 001BD560 model 0x0B, door id 0|0x80: AREA04 entry 7
    "button_r12": 0x7AC2F0,       # 001BC960 model 0 (outer), id 0xFF, at (647.7, 175, 1238.9)
    "button_r13": 0x7AC5E0,       # 001BC960 model 1 (inner), id 0xFF, at (644.7, 175, 1276.1)
    "door14_r14": 0x7AC8D0,       # 001BDE60 model 5 at (688.7, 160, 1161.1)
    "button_r15": 0x7ACBC0,       # 001BD9F0 model 3 (south side), room move id 2
    "button_r16": 0x7ACEB0,       # 001BD9F0 model 4 (north side), room move id 2
    "door17_r17": 0x7AD1A0,       # overlay 0x823580 model 0x03, room move id 3 (entries 9 / 4)
    "door20_r20": 0x7ADA70,       # 001BC350 model 0x03, room move id 4 (entries 5 / 10)
    "r44_823E90": 0x7B20F0,       # overlay 0x823E90 at (798.4, 215, 1149.5)
    "hatch_r62": 0x7B4FF0,        # overlay 0x826850 at (720.1, 160, 1262) (open: state 2)
    "hatch_r63": 0x7B52E0,        # overlay 0x826850 at (1081, 160, 845)
}
A13B_SPANS = [sp for sp in A13_SPANS if ":" not in sp[0]]
for _name, _base in A13B_OWNERS.items():
    A13B_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]


def use_a13b_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A13B_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a13(sampler.raw(), owners=A13B_OWNERS)
    r.rows[0] = dict(r.now(), f=0)


def a13b_beat_ladder_up(r: Route) -> dict:
    # Cross facing +z at the AREA19 ladder's foot: the ladder (actions 0x15,
    # 0x17); the stick up climbs; inside 00196970's circle (710, 1266.1) at y
    # >= 284.5 the request 0D 00 06 01: AREA13 entry 6 (720, 143.1, 1259.6),
    # the shaft under the open hatch [62]; the player climbs out on his own
    # (actions 0x43, 0x18) to (720, 160, 1252.3) facing -z.
    use_a19_sampler(r)
    next_long_frames(r)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x17))
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["area4"][:2] == "0d", 1500, 0, 0x7F, 0x00)
    r.until(lambda row: row["area4"][:8] == "0d00060d", 1500, 0, 0x7F, 0x00)
    r.until(lambda row: row["m1F0"] == 0 and row["spad"][2:4] == "00" and row["pos"][1] > 155, 1500, 0, 0x7F, 0x00)
    r.set_pad(0)
    a13_control(r, 1500, 60)
    a13_settle(r, 10)
    if r.rows[-1]["area4"][:6] != "0d0006":
        raise RuntimeError("not in AREA13 entry 6: " + summary(r.rows[-1]))
    return {"what": "the AREA19 ladder up: 00196970's request to AREA13 entry 6, out of the hatch [62]",
            "hp_end": a01_hp(r)}


def a13b_beat_door17(r: Route) -> dict:
    # East round the hatch's frame, the grating step (ledge climb facing +x,
    # action 8), off its east side, door [17] from inside facing +x: entry 9
    # outside (785, 160, 1262).
    use_a13b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(735, 1250), (738, 1258)], limit=200)
    if how != "ok":
        raise RuntimeError("the step not reached (" + how + "): " + summary(r.rows[-1]))
    a13_face(r, math.pi / 2)
    use_press(r, lambda row: row["m1F0"] == 8, tries=4, wait=30)
    r.until(lambda row: row["m1F0"] != 8, 300)
    a13_settle(r, 10)
    a04_go(r, [(760, 1262)], limit=150)
    a13_settle(r, 10)
    a13_use(r, 772.5, 1263.0, math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:6] == "0d0009", 1500)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    return {"what": "the grating step east, door [17] from inside: entry 9 (outside)", "hp_end": a01_hp(r)}


A13B_OUT_TO_DOOR14 = [(792, 1240), (795, 1222), (790, 1180), (770, 1160), (740, 1152), (700, 1152), (670, 1150)]


def a13b_beat_button15(r: Route) -> dict:
    # Outside, south and west round the building to door [14]; its button
    # [15] (001BD9F0 model 3, south side, latch 1) facing +z at (665.7,
    # 1151.5): room move id 2 to entry 3 (688, 160, 1174.1).  With item 0x1A
    # held the health stays (0015D100 returns while D_00810C7E != 0).
    use_a13b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, A13B_OUT_TO_DOOR14, limit=300)
    if how != "ok":
        raise RuntimeError("door [14] not reached (" + how + "): " + summary(r.rows[-1]))
    a13_use(r, 665.7, 1151.5, 0.0, lambda row: row["button_r15"]["h"][10:12] != "00")
    r.until(lambda row: row["area4"][:6] == "0d0003", 1500)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    return {"what": "outside to door [14], its button [15] facing +z: entry 3", "hp_end": a01_hp(r)}


def a13b_beat_door8(r: Route) -> dict:
    # Door [8] (001BC350, room move id 1) from its south side facing +z:
    # entry 1 (664.7, 160, 1221.1), the lift lobby.
    use_a13b_sampler(r)
    next_long_frames(r)
    a04_go(r, [(675, 1185), (670, 1198)], limit=200)
    a13_use(r, 669.7, 1203.0, 0.0, a04_program)
    r.until(lambda row: row["area4"][:6] == "0d0001", 1500)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    return {"what": "door [8] facing +z: entry 1 (the lift lobby)", "hp_end": a01_hp(r)}


def a13b_beat_lift_call(r: Route) -> dict:
    # The outer button [12] (001BC960 model 0, id 0xFF: no lock bit) facing
    # -x at (652.7, 1238.6): its script, then the lift [10]'s +0x0B 0 -> 2
    # and, when the car has come, 3 (doors open, waiting for the inner one).
    use_a13b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(672, 1224), (680, 1228), (680, 1250), (660, 1250), (653, 1243)], limit=200)
    if how != "ok":
        raise RuntimeError("button [12] not reached (" + how + "): " + summary(r.rows[-1]))
    a13_settle(r, 5)
    a13_face(r, -math.pi / 2)
    a19_press_until(r, lambda row: row["button_r12"]["h"][10:12] != "00" or not in_control(row))
    r.until(lambda row: row["lift_r10"]["h"][22:24] == "03" and in_control(row), 1500)
    a13_settle(r, 10)
    return {"what": "the outer button [12]: the lift [10] comes, +0x0B 0 -> 2 -> 3", "hp_end": a01_hp(r)}


def a13b_beat_lift_ride(r: Route) -> dict:
    # Into the car through its opening (z about 1258), the inner button [13]
    # (001BC960 model 1, id 0xFF) facing +x: a13_use aims at (639.7, 1276.1),
    # the walk stops at (639.5, 1271.6) and the frame the Use is taken puts
    # the player at (639.7, 160, 1276.4); its script,
    # the lift's +0x0B 3 -> 4, the request 04 FF 07 01: AREA04 entry 7
    # (D_00810730[4] = sub 0), control at (570.7, 54.9, 244.6).
    use_a13b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(656, 1252), (650, 1258), (638, 1260), (638, 1272)], limit=200)
    if how != "ok":
        raise RuntimeError("the car not reached (" + how + "): " + summary(r.rows[-1]))
    a13_use(r, 639.7, 1276.1, math.pi / 2,
            lambda row: row["button_r13"]["h"][10:12] != "00" or not in_control(row))
    r.until(lambda row: row["area4"][:2] == "04", 6000)
    r.until(lambda row: row["area4"][:8] == "04000704", 3000)
    a13_control(r, 6000, 60)
    a13_settle(r, 10)
    if r.rows[-1]["area4"][:6] != "040007":
        raise RuntimeError("not at AREA04 entry 7: " + summary(r.rows[-1]))
    return {"what": "the inner button [13]: the lift [10] to AREA04 entry 7", "hp_end": a01_hp(r)}


def a13b_beat_roof_ladder(r: Route) -> dict:
    # Side beat from a13b_01's end (outside, entry 9): the ladder at the
    # building's south-east corner (grid wall attribute 0x32 at x 720..727.4,
    # z 1208.6, y 160..215, facing -z): Cross facing +z at (723.7, 1199),
    # actions 0x15 / 0x17, the stick up climbs to the roof (y 215); east
    # along it until a railing stops the walk (the kept beat ends at (773.2,
    # 215, 1226.4); an earlier exploration run stopped at (773.7, 215, 1224.6)).
    use_a13b_sampler(r)
    next_long_frames(r)
    how = a04_go(r, [(795, 1222), (770, 1190), (740, 1180), (728, 1195), (723.7, 1199)], limit=300)
    if how != "ok":
        raise RuntimeError("the ladder not reached (" + how + "): " + summary(r.rows[-1]))
    a13_settle(r, 5)
    a13_face(r, 0.0)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x17))
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["pos"][1] > 214 and row["m1F0"] in (0, 1), 1500, 0, 0x7F, 0x00)
    r.set_pad(0)
    a13_settle(r, 10)
    how = a04_go(r, [(724, 1218), (748.5, 1217), (764.5, 1217), (771.5, 1222), (777.5, 1228)], limit=200)
    a13_settle(r, 10)
    row = r.rows[-1]
    if row["pos"][1] < 214:
        raise RuntimeError("not on the roof: " + summary(row))
    return {"what": "the roof ladder at the building's south-east corner; the roof east to the railing (" + how + ")",
            "hp_end": a01_hp(r)}


A13B_BEATS: list[tuple] = [
    ("a13b_00_ladder_up", "a19_02_duct_back", a13b_beat_ladder_up),
    ("a13b_01_door17", "a13b_00_ladder_up", a13b_beat_door17),
    ("a13b_02_button15", "a13b_01_door17", a13b_beat_button15),
    ("a13b_03_door8", "a13b_02_button15", a13b_beat_door8),
    ("a13b_04_lift_call", "a13b_03_door8", a13b_beat_lift_call),
    ("a13b_05_lift_ride", "a13b_04_lift_call", a13b_beat_lift_ride),
    ("a13b_s0_roof_ladder", "a13b_01_door17", a13b_beat_roof_ladder),
]
A13B_SIDE_BEATS: set[str] = {"a13b_s0_roof_ladder"}
A13B_CHANGE_BEATS: set[str] = {"a13b_05_lift_ride"}


def tenth_selected(spec: str) -> list[tuple]:
    """The tenth-level groups `a19` and `a13b` (opt-in, story order)."""
    wanted = spec.split(",")
    named = [w for w in wanted if w not in ("a19", "a13b")]
    out = []
    for tag, beats, side in (("a19", A19_BEATS, A19_SIDE_BEATS), ("a13b", A13B_BEATS, A13B_SIDE_BEATS)):
        out += [b for b in beats if (tag in wanted and b[0] not in side)
                or any(b[0] == w or b[0].startswith(w + "_") for w in named)]
    return out


def tenth_owners(name: str):
    if name.startswith("a19_"):
        return A19_OWNERS
    if name.startswith("a13b_"):
        return A13B_OWNERS
    return None



# ---------------------------------------------------------------------------
# Eleventh level (lane NEXTCAP, s91; port docs/ELEVENTH_LEVEL_ROUTE.md):
# AREA13's battery machine [44] and what its event opens.  Opt-in group
# `a13c`, from a13b_01_door17's end (outside door [17], entry 9, infection 80:
# the a13b trip down to AREA04 entry 7 is a detour, see the doc), with the
# order the code gives (decomp docs/WORLD_GRAPH.md section 8d).  Every beat
# stays in this AREA13 load, so the pool nodes measured on a13b_01's snapshot
# (tools/area_overview.py --area 13 --ram) hold throughout; a node that frees
# itself ([44], [7]) is reused by later spawns, so its row is garbage after.
OUT_A13C = ROOT / "build/s87/route_a13c"
A13C_OWNERS = {
    "pick_g3": 0x7A5F10,          # 00219550 g[3] at (703.6, 158.9, 1334.8): item 0x22
    # The two 0x824BB0 keys are those of the kept a13c traces and census
    # replays (a key only: whether these nodes hit the player is not known).
    "sentry_g19": 0x7A8B20,       # overlay 0x824BB0 g[19] at (759.7, 205, 974.9)
    "sentry_g21": 0x7A9100,       # overlay 0x824BB0 g[21] at (724.9, 205, 1280)
    "r7_824A80": 0x7AB440,        # overlay 0x824A80 at (688, 163, 1041): area 0x82E220, counter / flag 0x42
    "door17_r17": 0x7AD1A0,       # overlay 0x823580 model 0x03, room move id 3 (entries 9 / 4)
    "door20_r20": 0x7ADA70,       # 001BC350 model 0x03, room move id 4 (entries 5 / 10)
    "r44_823E90": 0x7B20F0,       # overlay 0x823E90 model 0x25 at (798.4, 215, 1149.5): the battery machine
    "r45_827150": 0x7B23E0,       # overlay 0x827150 at (827, 156.9, 1130.4): the step controller
    "r47_8293A0": 0x7B26D0,       # overlay 0x8293A0 at (706.2, 160, 1047.7): cell entries 0x1F / 0x20
    "r49_8292A0": 0x7B29C0,       # overlay 0x8292A0 at (766.5, 235, 1267.9): ends with D_008107F4 bit 5
    "r51_8292A0": 0x7B2FA0,       # overlay 0x8292A0 at (718, 215, 1246.9)
    "recharger_r60": 0x7B4A10,    # 00159210 model 0x2C at (774.8, 160, 1237.9): the recharger
    "hatch_r63": 0x7B52E0,        # overlay 0x826850 at (1081, 160, 845)
}
A13C_SPANS = [sp for sp in A13_SPANS if ":" not in sp[0]]
for _name, _base in A13C_OWNERS.items():
    A13C_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]
A13C_EVENT_KEYS = ("s770", "s7f0", "s798", "s818", "c61", "c86", "charge", "infected")


def use_a13c_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A13C_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a13(sampler.raw(), owners=A13C_OWNERS)
    r.rows[0] = dict(r.now(), f=0)


def a13c_control(r: Route, limit: int = 3000) -> None:
    """Control kept for 30 frames with no program running (3B8D == 0)."""
    for i in range(limit):
        row = r.step(1)
        if (in_control(row) and row["spad"][2:4] == "00" and i > 30
                and all(in_control(x) and x["spad"][2:4] == "00" for x in r.rows[-30:])):
            return
    raise TimeoutError("control not kept; last " + summary(r.rows[-1]))


def a13c_walk(r: Route, points, what: str, limit: int = 150, tol: float = 1.5, slack: float = 4.0) -> None:
    """a04_go point by point.  a04_go can return 'ok' short of a waypoint, and a
    railing or a step can hold the walk a little short of it: every stop is
    checked against `slack`.  Waypoints on the stair landing (z 1300..1310)
    use the tighter tolerance 1.0 (a wider one lets the walk climb the upper
    flight beside it)."""
    for i, p in enumerate(points):
        how = a04_go(r, [p], limit=limit, tol=1.0 if 1300 < p[1] < 1310 else tol)
        px, _py, pz = r.rows[-1]["pos"]
        if math.hypot(px - p[0], pz - p[1]) > slack:
            raise RuntimeError(f"{what}: waypoint {i} {p} not reached ({how}): " + summary(r.rows[-1]))


def a13c_take(r: Route, x: float, z: float, yaw: float, tries: int = 8) -> None:
    """Walk to (x, z), face `yaw` and press Cross until a pickup's program
    takes the player out of control (on the ground north of the building the
    player is hit about every 110 frames by an attacker that was not
    identified, so the press is retried at once, without a13_use's settling)."""
    for _ in range(tries):
        if r.rows[-1]["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        a04_go(r, [(x, z)], limit=120, tol=1.0)
        a13_face(r, yaw, tol=0.15)
        r.press("CROSS", 2)
        for _ in range(30):
            if not in_control(r.rows[-1]) and r.rows[-1]["m1F0"] not in A04_GRABS:
                return
            r.step(1)
    raise TimeoutError("pickup not taken: " + summary(r.rows[-1]))


def a13c_roof_ladder(r: Route, approach) -> None:
    """The ladder at the building's south-east corner (a13b_s0): Cross facing
    +z at (723.7, 1199), the stick up to the roof (y 215)."""
    how = a04_go(r, approach, limit=300)
    if how != "ok":
        raise RuntimeError("the roof ladder not reached (" + how + "): " + summary(r.rows[-1]))
    a13_settle(r, 5)
    a13_face(r, 0.0)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x17))
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["pos"][1] > 214 and row["m1F0"] in (0, 1), 1500, 0, 0x7F, 0x00)
    r.set_pad(0)
    a13_settle(r, 10)


A13C_ROOF_TO_MACHINE = [(745, 1222), (764, 1217), (772, 1218), (778, 1212), (784, 1203), (790, 1190),
                        (797, 1175), (800, 1162)]


def a13c_beat_recharger(r: Route) -> dict:
    # Door [17] from outside facing -x (entry 4, the east room); the
    # recharger [60] (00159210 model 0x2C at (774.8, 160, 1237.9), yaw -pi/2):
    # Use facing +x from (769, 1238): 00157860's model-0x2C path (charge 0 is
    # below the capacity 12) plays its script and posts the battery page's
    # recharge request (D_008106B1 bit 6); the page's prompt, Yes: the charge
    # D_00810CB2 rises 2 per 20 frames to 12 (a13c_00 f711..f811).
    use_a13c_sampler(r)
    next_long_frames(r)
    a13_use(r, 785.5, 1263.0, -math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:6] == "0d0004", 1500)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    a13c_walk(r, [(772, 1250), (769, 1240)], "the recharger")
    a13_use(r, 769.0, 1238.0, math.pi / 2, lambda row: not in_control(row) or row["spad"][2:4] != "00")
    next_prompt_yes(r)
    r.until(lambda row: row["charge"] >= 12, 1200)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    return {"what": "door [17] in (entry 4); the recharger [60]: the battery charge 0 -> 12", "hp_end": a01_hp(r)}


def a13c_beat_to_machine(r: Route) -> dict:
    # Door [17] from inside facing +x (entry 9); the roof ladder; east on the
    # roof to its south-east corner and down the walkway (the area 0x82E140,
    # x 765..815, z 1156..1221, y 213..215) to [44]'s platform; the Use point
    # (805.3, 1153.2) facing -z.
    use_a13c_sampler(r)
    next_long_frames(r)
    a04_go(r, [(766, 1255), (762, 1262)], limit=200)
    a13_use(r, 772.5, 1263.0, math.pi / 2, a04_program)
    r.until(lambda row: row["area4"][:6] == "0d0009", 1500)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    a13c_roof_ladder(r, [(795, 1222), (770, 1190), (740, 1180), (728, 1195), (723.7, 1199)])
    a13c_walk(r, A13C_ROOF_TO_MACHINE, "the walkway")
    a04_go(r, [(802.4, 1157)], limit=100, tol=0.8)
    a13_settle(r, 5)
    a13_face(r, math.pi)
    return {"what": "door [17] out, the roof ladder, the walkway to [44]", "hp_end": a01_hp(r)}


def a13c_beat_battery(r: Route) -> dict:
    # [44] (overlay 0x823E90 model 0x25, step 1 = 0x824180): Use facing -z
    # (+0xB bit 2) without bit 0: script 0x82B090, then D_008106B1 = 0x84
    # (+0x34 4 + 0x80) and D_008106B0 = 1 open the battery page's confirmation
    # for this owner; Yes: the charge falls 2 per 30 frames by 2 * 4 (12 -> 4)
    # and the page marks the owner +0xA = 1, +0xB = 5; step 1 then plays
    # script 0x82B2D0, sets flag 0x1C (D_00810774) = 1 and counter 0x1C
    # (D_008107F4) 1 -> 2; [45] (0x827150) sees bit 1 and ORs 0x10, then
    # 0x40 (its steps start).
    use_a13c_sampler(r)
    next_long_frames(r)
    a19_press_until(r, lambda row: row["r44_823E90"]["h"][10:12] != "00" or not in_control(row))
    r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][10:12] == "04", 1500)
    r.idle(30)
    r.press("LEFT", 2, after=20)
    if r.rows[-1]["ui"][12:14] != "00":
        raise RuntimeError("prompt cursor not on Yes: " + summary(r.rows[-1]))
    r.press("CROSS", 2)
    r.until(lambda row: row["charge"] <= 4, 900)
    a13c_control(r, 1500)
    row = r.rows[-1]
    if row["s770"][8:10] != "01":
        raise RuntimeError("flag 0x1C not 1: " + summary(row))
    return {"what": "[44]: the battery prompt, Yes: charge 12 -> 4, flag 0x1C = 1", "hp_end": a01_hp(r)}


def a13c_beat_blast(r: Route) -> dict:
    # Back north onto the walkway (the area 0x82E140) and wait.  At [45]'s
    # step-0 count 0x1DF an effect goes off at 0x82D1B0[0] (815.6, 242.8,
    # 1145.4), next to [44]'s platform (exploration: a player left standing
    # at the Use point died there); at [44]'s 490-frame count (0x824390) the
    # player is above y 210 inside 0x82E140, so script 0x82B3D0 runs (its
    # callback 0x8246D0 moves D_00810350 by -1 and the height on a sine arc)
    # and the scene puts the player in the field; at 0x1F3 [45] ORs 0x20 into
    # D_008107F4 (the roof's [49]..[54] end) and its later steps run; [44]
    # steps 3 and 4 follow ([45]'s step 4 sets D_00810833 = 0xFF, script
    # 0x82C110, flag 0x1C = 0xFF); control at (696, 160, 1102).  The player
    # walks to (720, 1175) after the blast scene (the exploration that
    # survived did so).
    use_a13c_sampler(r)
    next_long_frames(r)
    a13c_walk(r, [(800, 1162), (797, 1175), (790, 1190), (785, 1200)], "back onto the walkway")
    r.set_pad(0)
    r.until(lambda row: row["spad"][2:4] != "00", 900)
    r.until(lambda row: in_control(row) and row["spad"][2:4] == "00", 3000)
    a04_go(r, [(735, 1165), (720, 1175)], limit=200)
    r.set_pad(0)
    for _ in range(6000):
        row = r.step(1)
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
        if row["s770"][8:10] == "ff":
            break
    a13c_control(r, 3000)
    row = r.rows[-1]
    if row["s770"][8:10] != "ff":
        raise RuntimeError("flag 0x1C not 0xFF: " + summary(row))
    return {"what": "the walkway blast (script 0x82B3D0), [45]'s steps, flag 0x1C = 0xFF", "hp_end": a01_hp(r)}


A13C_ROOF_TO_STAIR = [(735, 1232), (738, 1250), (730, 1258), (715, 1254), (706, 1252), (700, 1258), (700, 1268),
                      (693, 1270), (693, 1272), (699.5, 1284), (698, 1293)]
A13C_STAIR_DOWN = [(693, 1297), (685, 1297), (675, 1297), (665, 1297), (655, 1297), (648, 1298), (648, 1306.5),
                   (653, 1306.8), (660, 1306.8), (675, 1306.8), (690, 1306.5), (699, 1302), (698, 1297),
                   (690, 1296.5), (680, 1297), (670, 1297), (660, 1297), (652, 1297), (647, 1298), (644.7, 1300),
                   (660, 1322), (682, 1332), (692, 1335)]


def a13c_beat_cure(r: Route) -> dict:
    # The roof ladder again; the roof's north part is open now (the [49]..[54]
    # objects ended with D_008107F4 bit 5); down a ramp west onto the y-210
    # part, then the switchback stair north of the lobby along z 1297 (upper
    # flight y 210 -> 184 westward, the landing x 646..698, z 1297..1307 at
    # y 184, lower flight y 184 -> 158 westward) to the ground north of the
    # building (y 157); the pickup g[3] (00219550, item 0x22) facing +x: its page is
    # the HEALING page with the item selected; Cross, Left (Yes), Cross: the
    # health D_008104D0 -> 100 and the infection +0x228 -> 0 (002160B0 kind
    # 4); Triangle closes the status screen.
    use_a13c_sampler(r)
    next_long_frames(r)
    a13c_roof_ladder(r, [(725, 1150), (745, 1165), (740, 1180), (728, 1195), (723.7, 1199)])
    a13c_walk(r, A13C_ROOF_TO_STAIR, "the roof's north-west")
    a13c_walk(r, A13C_STAIR_DOWN, "the stair")
    a13c_take(r, 692.0, 1334.8, math.pi / 2)
    r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][8:12] == "0701", 900)
    r.idle(60)
    r.press("CROSS", 2)
    r.until(lambda row: row["ui"][10:12] == "04", 120)
    r.idle(10)
    r.press("LEFT", 2, after=20)
    if r.rows[-1]["ui"][12:14] != "00":
        raise RuntimeError("prompt cursor not on Yes: " + summary(r.rows[-1]))
    r.press("CROSS", 2)
    r.until(lambda row: row["hp"] >= 99.0, 600)
    for _ in range(6):
        r.press("TRIANGLE", 2)
        r.step(90)
        if in_control(r.rows[-1]) and r.rows[-1]["spad"][2:4] == "00":
            break
    a13c_control(r, 1500)
    row = r.rows[-1]
    if row["inf"] > 0.5:
        raise RuntimeError("infection not cured: " + summary(row))
    return {"what": "the roof's north part, the stair, the ground north: item 0x22 used: health 100, infection 0",
            "hp_end": a01_hp(r)}


A13C_STAIR_UP = [(660, 1322), (644.7, 1300), (648, 1297), (652, 1297), (660, 1297), (670, 1297), (680, 1297),
                 (690, 1296.5), (698, 1297), (699, 1302), (690, 1306.5), (675, 1306.8), (660, 1306.8),
                 (653, 1306.8), (648, 1306.5), (648, 1298), (655, 1297), (665, 1297), (675, 1297), (685, 1297),
                 (693, 1296.5), (698, 1293),
                 (699.5, 1284), (693, 1272), (693, 1268), (700, 1268), (700, 1258), (706, 1252), (715, 1254),
                 (730, 1258), (738, 1250), (745, 1222)]
A13C_BOOM = [(814, 1115), (800, 1105), (785, 1093.5), (770, 1082), (755, 1070.6), (740, 1059.2),
             (725, 1047.8), (710, 1036.4), (695, 1025)]


def a13c_beat_boom(r: Route) -> dict:
    # Back up the switchback stair to the roof; the walkway to [44]'s platform, its south end
    # (809.5, 208.5, 1129.6), and down the fallen boom (the cell-44 top from
    # (816.7, 1117.2) to (633, 977.3), y 205.7 -> 179.3; its north-east end
    # was closed by cell entry 1's walls until [47] cleared them) to [7]'s
    # area 0x82E220: script 0x82C510, D_0081081A (counter 0x42) = 0xFF, then
    # flag 0x42 = 0xFF and the group 0x82A230 (two 0x141D20 actors).
    use_a13c_sampler(r)
    next_long_frames(r)
    a13c_walk(r, A13C_STAIR_UP, "the stair up")
    a13c_walk(r, A13C_ROOF_TO_MACHINE[1:] + [(806, 1150)], "the walkway")
    a04_go(r, [(812, 1135)], limit=150)
    a13c_walk(r, A13C_BOOM[:-1], "the boom", tol=1.2)
    # the last waypoint lies past [7]'s area: its script takes the player
    a04_go(r, A13C_BOOM[-1:], limit=150, tol=1.2, until=lambda row: row["s818"][4:6] == "ff")
    r.until(lambda row: row["s818"][4:6] == "ff", 600)
    a13c_control(r, 3000)
    row = r.rows[-1]
    if row["s798"][4:6] != "ff":
        raise RuntimeError("flag 0x42 not 0xFF: " + summary(row))
    return {"what": "back up the stair, the walkway, down the boom: [7]'s area, counter / flag 0x42 = 0xFF",
            "hp_end": a01_hp(r)}


A13C_TO_SOUTH = [(695, 1025), (680, 1013.5), (665, 1002), (650, 990.5), (645, 985), (660, 972), (658, 955),
                 (655, 940), (650, 920), (640, 900)]


def a13c_beat_south(r: Route) -> dict:
    # On down the boom past [7]'s area to its south-west end (633, 977.3, y
    # 179.3) and off its south side, down the slope south (y 181 -> 153) into
    # the region south of the pipe fence (exploration: it holds g[0], g[1],
    # g[7], [58] (an examine point: its script moved nothing) and [7]'s two
    # 0x141D20 actors, which attacked; the way east to door [20] was not
    # found).
    use_a13c_sampler(r)
    next_long_frames(r)
    a13c_walk(r, A13C_TO_SOUTH, "the boom's south-west end and the slope")
    a13_settle(r, 10)
    row = r.rows[-1]
    if row["pos"][2] > 910 or row["pos"][1] > 160:
        raise RuntimeError("not in the south region: " + summary(row))
    return {"what": "the boom's south-west end and the slope south into the region south of the pipe fence",
            "hp_end": a01_hp(r)}


A13C_BEATS: list[tuple] = [
    ("a13c_00_recharger", "a13b_01_door17", a13c_beat_recharger),
    ("a13c_01_to_machine", "a13c_00_recharger", a13c_beat_to_machine),
    ("a13c_02_battery", "a13c_01_to_machine", a13c_beat_battery),
    ("a13c_03_blast", "a13c_02_battery", a13c_beat_blast),
    ("a13c_04_cure", "a13c_03_blast", a13c_beat_cure),
    ("a13c_05_boom", "a13c_04_cure", a13c_beat_boom),
    ("a13c_06_south", "a13c_05_boom", a13c_beat_south),
]
A13C_SIDE_BEATS: set[str] = set()
A13C_CHANGE_BEATS: set[str] = set()


def eleventh_selected(spec: str) -> list[tuple]:
    """The eleventh-level group `a13c` (opt-in, story order)."""
    wanted = spec.split(",")
    if "a13c" in wanted:
        return [b for b in A13C_BEATS if b[0] not in A13C_SIDE_BEATS]
    return [b for b in A13C_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def eleventh_owners(name: str):
    if name.startswith("a13c_"):
        return A13C_OWNERS
    return None

# ---------------------------------------------------------------------------
# Twelfth level (lane STORY, s92; port docs/TWELFTH_LEVEL_ROUTE.md): from the
# region south of AREA13's pipe fence (a13c_06_south's end) to door [20]'s
# north side and the hatch [63], then down its ladder into AREA19 entry 10
# (decomp docs/WORLD_GRAPH.md section 8e).  Opt-in groups `a13d` (AREA13,
# overlay id 10; the same AREA13 load as a13c, so its pool nodes hold) and
# `a19b` (the hatch's ladder and AREA19 from entry 10, overlay id 16).  The
# AREA13 collision the route uses (grid nodes of the level's id-0x44 file,
# read from the captured RAM): the switchback stair south of the big building
# (attribute 0x35, x 744..787, z 814..835, y 160 -> 179.5 -> 203.5), the
# ladder at x 798.5 (attribute 0x32, y 203.5 -> 240), the roof corridor (y
# 240, z 805..835) with two blocks (x 890..920 and 960..990, top y 280; the
# first has ladders on both faces, the second only on its east face: its
# west face carries no attribute-0x32 node, and Cross there started no climb,
# so the route crosses it by a running jump), and the ladder at z 836.4 (y
# 240 -> 176.5) down to door [20]'s side.
OUT_A13D = ROOT / "build/s87/route_a13d"
A13D_OWNERS = dict(A13C_OWNERS)
A13D_OWNERS.update({
    "beast_7AAB70": 0x7AAB70,     # 00141D20, group 0x82A230[0] (spawned at [7]'s end, a13c_05)
    "beast_7B2CB0": 0x7B2CB0,     # 00141D20, group 0x82A230[1]
    "roof_g16": 0x7A8250,         # 00219870 g[16] model 0x30 at (858.5, 250.8, 835)
    "watch_g17": 0x7A8540,        # overlay 0x824BB0 g[17] at (1050, 201.1, 877.3)
    "watch_g23": 0x7A96E0,        # overlay 0x824BB0 g[23] at (990, 205, 944.1)
})
A13D_SPANS = [sp for sp in A13_SPANS if ":" not in sp[0]]
for _name, _base in A13D_OWNERS.items():
    A13D_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]


def use_a13d_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A13D_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a13(sampler.raw(), owners=A13D_OWNERS)
    r.rows[0] = dict(r.now(), f=0)


def a13d_go(r: Route, x: float, z: float, tol: float = 1.5, limit: int = 300) -> str:
    """Walk to (x, z) with the stick magnitude eased near the point
    (min(1, max(0.4, d / 12))): at full magnitude the walk circled round
    waypoints near walls.  A grab is shaken off.  Returns 'ok', 'blocked' or
    'limit'."""
    hist: list[tuple[float, float]] = []
    for _ in range(limit):
        row = r.rows[-1]
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
            hist = []
            continue
        px, _py, pz = row["pos"]
        d = math.hypot(x - px, z - pz)
        if d <= tol:
            r.set_pad(0)
            return "ok"
        r.stick_toward(x, z, min(1.0, max(0.4, d / 12.0)))
        row = r.step(1)
        hist.append((row["pos"][0], row["pos"][2]))
        if len(hist) > 45 and math.hypot(hist[-1][0] - hist[-45][0], hist[-1][1] - hist[-45][1]) < 0.3:
            r.set_pad(0)
            return "blocked"
    r.set_pad(0)
    return "limit"


def a13d_walk(r: Route, points, what: str, tol: float = 1.5, slack: float = 4.0) -> None:
    for i, p in enumerate(points):
        how = a13d_go(r, p[0], p[1], tol=tol)
        px, _py, pz = r.rows[-1]["pos"]
        if math.hypot(px - p[0], pz - p[1]) > slack:
            raise RuntimeError(f"{what}: waypoint {i} {p} not reached ({how}): " + summary(r.rows[-1]))


def a13d_ladder_up(r: Route, yaw: float) -> None:
    """Cross facing an attribute-0x32 face (actions 0x15 / 0x17), the stick
    up until control at the top."""
    a13_settle(r, 5)
    a13_face(r, yaw)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x16, 0x17))
    y0 = r.rows[-1]["pos"][1]
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["m1F0"] in (0, 1) and row["pos"][1] > y0 + 20, 1500, 0, 0x7F, 0x00)
    r.set_pad(0)
    a13_settle(r, 10)


def a13d_ladder_down(r: Route, yaw: float) -> None:
    """Cross on a ladder's top floor facing the drop (action 0x16), the stick
    down until control at the foot."""
    a13_settle(r, 5)
    a13_face(r, yaw)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x16, 0x17, 0x18))
    y0 = r.rows[-1]["pos"][1]
    r.set_pad(0, 0x7F, 0xFF)
    r.until(lambda row: row["m1F0"] in (0, 1) and row["pos"][1] < y0 - 20, 1500, 0, 0x7F, 0xFF)
    r.set_pad(0)
    a13_settle(r, 10)


A13D_TO_STAIR = [(660, 870), (690, 840), (720, 815), (733, 810.5), (752, 810.5), (775, 809), (791, 811),
                 (791, 820), (791, 829.5)]
A13D_STAIR_UP = [(784, 830), (770, 830), (756, 830), (749, 828), (749, 818), (756, 818), (770, 818),
                 (785, 818), (792, 819.3)]


def a13d_beat_stair(r: Route) -> dict:
    # East through the region south of the pipe fence, past the small box
    # (cell entry 78, x 739..745, z 797..805, y 166..171), north under the
    # upper flight to the lower flight's east end (x 783.2, the flights are
    # railed along z 824.5 and z 835), up it west to the landing (y 179.5),
    # then up the upper flight east to its top (y 203.5).  0015EC50's second
    # AREA13 box (x 720..800, z 800..840, y 150..210) refuses the running
    # jump here.
    use_a13d_sampler(r)
    next_long_frames(r)
    a13d_walk(r, A13D_TO_STAIR, "to the stair's foot")
    a13d_walk(r, A13D_STAIR_UP, "up the stair")
    if r.rows[-1]["pos"][1] < 203:
        raise RuntimeError("not at the stair's top: " + summary(r.rows[-1]))
    return {"what": "south region east to the switchback stair, up it to the y-203.5 landing", "hp_end": a01_hp(r)}


def a13d_beat_ladder405(r: Route) -> dict:
    # The ladder at x 798.5 (grid nodes 405..407: face, foot y 203.5, top y
    # 240) facing +x: the climb to the roof corridor (y 240).
    use_a13d_sampler(r)
    next_long_frames(r)
    a13d_go(r, 795.5, 819.3, tol=0.8, limit=100)
    a13d_ladder_up(r, math.pi / 2)
    if r.rows[-1]["pos"][1] < 239:
        raise RuntimeError("not on the roof: " + summary(r.rows[-1]))
    return {"what": "the ladder at x 798.5 up to the roof corridor (y 240)", "hp_end": a01_hp(r)}


def a13d_beat_block890(r: Route) -> dict:
    # East along the corridor (y 240, z 805..835) to the first block (x
    # 890..920, top y 280); its west-face ladder (x 888.5, z 827.3..834.7)
    # facing +x up to its top.
    use_a13d_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(825, 820), (850, 822), (870, 830), (884, 831)], "the corridor")
    a13d_go(r, 885.0, 831.0, tol=0.8, limit=100)
    a13d_ladder_up(r, math.pi / 2)
    if r.rows[-1]["pos"][1] < 279:
        raise RuntimeError("not on the first block: " + summary(r.rows[-1]))
    return {"what": "the roof corridor east, the first block's ladder up (y 280)", "hp_end": a01_hp(r)}


def a13d_beat_jump(r: Route) -> dict:
    # On the first block's top to (900, 820), face +x, run east and press
    # Cross at x >= 912 (the block ends at x 920): 0015EC50's running jump
    # (action 0x0C) over the 40-unit gap onto the second block's top (x
    # 960..990, y 280).
    use_a13d_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(897, 826), (900, 820)], "the first block's top")
    a13_settle(r, 5)
    a13_face(r, math.pi / 2)
    for _ in range(200):
        if r.rows[-1]["pos"][0] >= 912.0:
            break
        r.stick_toward(990.0, 820.0)
        r.step(1)
    r.set_pad(PAD["CROSS"], r.pad_state[1], r.pad_state[2])
    r.step(2)
    r.set_pad(0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] == 0x0C, 10, 0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] != 0x0C, 200, 0, r.pad_state[1], r.pad_state[2])
    r.set_pad(0)
    a13_settle(r, 20)
    row = r.rows[-1]
    if row["pos"][0] < 960 or row["pos"][1] < 279:
        raise RuntimeError("the jump did not land on the second block: " + summary(row))
    return {"what": "the running jump from the first block's top onto the second block's top",
            "landed": [round(v, 1) for v in row["pos"]], "hp_end": a01_hp(r)}


def a13d_beat_ladder426(r: Route) -> dict:
    # The second block's east-face ladder (x 991.4, z 806.3..813.7; top
    # floor x 981.4..991.4): Cross facing +x on the top, down to the
    # corridor (y 240).
    use_a13d_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(980, 812), (985.5, 810)], "the second block's top")
    a13d_go(r, 986.0, 810.0, tol=0.8, limit=100)
    a13d_ladder_down(r, math.pi / 2)
    if r.rows[-1]["pos"][1] > 241:
        raise RuntimeError("not down in the corridor: " + summary(r.rows[-1]))
    return {"what": "the second block's east ladder down to the corridor", "hp_end": a01_hp(r)}


def a13d_beat_ladder432(r: Route) -> dict:
    # East to the corridor's ladder at z 836.4 (top floor x 1016.2..1027.6,
    # z 826.4..836.4): Cross facing +z, down to its foot (y 176.5), on the
    # side of door [20].
    use_a13d_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(1000, 815), (1012, 824), (1022, 830.5)], "the corridor's east end")
    a13d_go(r, 1022.0, 831.0, tol=0.8, limit=100)
    a13d_ladder_down(r, 0.0)
    if r.rows[-1]["pos"][1] > 180:
        raise RuntimeError("not at the ladder's foot: " + summary(r.rows[-1]))
    return {"what": "the ladder at z 836.4 down to door [20]'s side (y 176.5)", "hp_end": a01_hp(r)}


def a13d_beat_door20(r: Route) -> dict:
    # Down the slope north and east to door [20]'s north side (entry 10's
    # point (1064, 160, 889)); Use facing -z: 001BC350, room move id 4, the
    # side latch gives entry 5 (inside, the hatch [63] room).
    use_a13d_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(1022, 850), (1025, 870), (1035, 885), (1050, 893), (1064, 891)], "to door [20]")
    a13_use(r, 1064.0, 889.5, math.pi, a04_program)
    r.until(lambda row: row["area4"][:6] == "0d0005", 1500)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    return {"what": "door [20] from its north side: entry 5", "hp_end": a01_hp(r)}


def a13d_beat_hatch63(r: Route) -> dict:
    # The hatch [63] (overlay 0x826850, z <= 1000: descriptor 0x82CDF0 at
    # (1071.5, 160.5, 844.9), radius 10, yaw -1.606): item 0x27 makes it
    # class 0x84; Use facing +x (yaw 1.536): script 0x82CA50, state 2,
    # counter 0x61 (D_00810839) |= 2.
    use_a13d_sampler(r)
    next_long_frames(r)
    a13_use(r, 1071.5, 844.9, 1.536, lambda row: row["hatch_r63"]["h"][10:12] != "00")
    r.until(lambda row: row["hatch_r63"]["h"][8:10] == "02", 1500)
    a13_control(r, 3000, 40)
    a13_settle(r, 10)
    if not int(r.rows[-1]["c61"], 16) & 2:
        raise RuntimeError("counter 0x61 bit 1 not set: " + summary(r.rows[-1]))
    return {"what": "the hatch [63] with item 0x27: open, counter 0x61 |= 2", "hp_end": a01_hp(r)}


A13D_BEATS: list[tuple] = [
    ("a13d_00_stair", "a13c_06_south", a13d_beat_stair),
    ("a13d_01_ladder405", "a13d_00_stair", a13d_beat_ladder405),
    ("a13d_02_block890", "a13d_01_ladder405", a13d_beat_block890),
    ("a13d_03_jump", "a13d_02_block890", a13d_beat_jump),
    ("a13d_04_ladder426", "a13d_03_jump", a13d_beat_ladder426),
    ("a13d_05_ladder432", "a13d_04_ladder426", a13d_beat_ladder432),
    ("a13d_06_door20", "a13d_05_ladder432", a13d_beat_door20),
    ("a13d_07_hatch63", "a13d_06_door20", a13d_beat_hatch63),
]
A13D_SIDE_BEATS: set[str] = set()
A13D_CHANGE_BEATS: set[str] = set()


# AREA19 from entry 10.  Pool nodes of this AREA19 sub-0 load (the arrival
# from AREA13's hatch [63]), measured on the exploration snapshot at the
# foot of entry 10's ladder (the deferred groups and placements sit at other
# nodes than in the a13_05 arrival through entry 9).
OUT_A19B = ROOT / "build/s87/route_a19b"
A19B_OWNERS = {
    "r6_8250F0": 0x7AD1A0,        # overlay 0x8250F0 at (956, 210, 1006.5): flag / counter 0x1D
    "r7_8257C0": 0x7AD490,        # overlay 0x8257C0 at (913.7, 198.2, 919.6): spawn entry 0xA, counter 0x1E
    "r9_825C70": 0x7ADA70,        # overlay 0x825C70 at (1012, 130, 916.9)
    "r10_827790": 0x7ADD60,       # overlay 0x827790 (class 9)
    "r18_826100": 0x7AF4E0,       # overlay 0x826100 at (964.9, 280.6, 804.8): flag 0x1F
    "reader_r21": 0x7AFDB0,       # 00158810 model 0x13 at (770, 235, 1044): item 0x24 -> lock bit 0
    "door22_r22": 0x7B00A0,       # 001BB860 model 0x16, room move id 0 (entries 6 / 5)
    "panel_r24": 0x7B0680,        # 00158EC0 model 0x22 at (1013, 280, 1126.9): item 0x1B -> lock bit 1
    "door25_r25": 0x7B0970,       # 001BB860 model 0x16, door id 1|0x80: AREA03 entry 1 (lock bit 1)
    "door27_r27": 0x7B0F50,       # overlay 0x823580 model 0x15, room move id 2 (entries 1 / 2; lock bit 2)
    "door31_r31": 0x7B1B10,       # 001BC350 model 0x03, room move id 4 (entries 3 / 4)
    "r43_8255D0": 0x7B3E50,       # overlay 0x8255D0 at (962.2, 210, 1007)
    "r47_825EE0": 0x7B4A10,       # overlay 0x825EE0 at (1036, 182.5, 824.8)
    "g18_219870": 0x7A8830,       # 00219870 g[18] model 0x30 at (1036, 160, 920)
    "g20_219870": 0x7A8E10,       # 00219870 g[20] model 0x30 at (1036, 160, 940)
    "beast_7BDCF0": 0x7BDCF0,     # 0012E3A0, group 0x82A590 (spawned at the end of [6]'s script 0x82B6E0)
}
A19B_SPANS = [sp for sp in A19_SPANS if ":" not in sp[0]]
for _name, _base in A19B_OWNERS.items():
    A19B_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10)]


def use_a19b_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A19B_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a19(sampler.raw(), owners=A19B_OWNERS)
    r.rows[0] = dict(r.now(), f=0)


def a19b_beat_arrival(r: Route) -> dict:
    # Use facing +x in the open hatch [63]: its ladder (action 0x16); at y
    # 143.1 (entry 7's point) 00193EB0 sees the player at y <= 159 with spawn
    # entry 5 and requests AREA19 sub 0 entry 0xA (13 00 0A 01).  AREA19
    # entry 10 (1077.2, 250.1, 845) starts on a ladder; [7] (0x8257C0) is
    # in state 1 with D_008107F6 == 0, so 0x825AB0 sees D_00810702 == 0xA and
    # runs its scripts 0x82C410, 0x82C4D0 and 0x82C690 (3B8D = 2 throughout;
    # the player is taken down the ladder to (1086.5, 185, 845)); at the end
    # D_008107F6 (counter 0x1E) = 1.  Control.
    use_a19b_sampler(r)
    next_long_frames(r)
    a13_face(r, 1.536)
    use_press(r, lambda row: row["m1F0"] == 0x16, tries=4, wait=30)
    r.until(lambda row: row["area4"][:2] == "13", 1500)
    r.until(lambda row: row["area4"][:8] == "13000a13", 1500)
    r.until(lambda row: row["spad"][2:4] != "00", 1500)
    for _ in range(3000):
        row = r.step(1)
        if row["s7f0"][12:14] == "01" and in_control(row) and all(in_control(x) for x in r.rows[-60:]):
            break
    else:
        raise TimeoutError("counter 0x1E not 1 with control: " + summary(r.rows[-1]))
    return {"what": "the hatch [63]'s ladder: AREA19 entry 10, [7]'s scripts, counter 0x1E = 1, control",
            "hp_end": a01_hp(r)}


def a19b_beat_ledge(r: Route) -> dict:
    # Off the entry-10 platform (y 185) onto the floor at y 170, east to the
    # wall and north along it: the ledge x 1095..1100, z 880..950 carries
    # attribute 0x39 and the walk turns into the wall-side shuffle (action
    # 0x2F); at its north end the step at z 950..960 (y 170), and a ledge
    # climb facing +z (action 8) onto the platform at y 190 (x 1056..1100,
    # z 960..1005).
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(1090, 862), (1096, 874), (1097.5, 890), (1097.5, 930), (1097.5, 948), (1093, 953),
                  (1092, 955.5)], "the east ledge")
    for _ in range(4):
        a13_settle(r, 5)
        a13_face(r, 0.0)
        r.idle(3)
        r.press("CROSS", 2)
        for _ in range(40):
            if r.rows[-1]["m1F0"] in (8, 9):
                break
            r.step(1)
        if r.rows[-1]["m1F0"] in (8, 9):
            break
    else:
        raise TimeoutError("no ledge climb: " + summary(r.rows[-1]))
    r.until(in_control, 400)
    a13_settle(r, 10)
    if r.rows[-1]["pos"][1] < 189:
        raise RuntimeError("not on the y-190 platform: " + summary(r.rows[-1]))
    return {"what": "the east ledge (attribute 0x39 shuffle) and a ledge climb onto the y-190 platform",
            "hp_end": a01_hp(r)}


def a19b_beat_ladder1023(r: Route) -> dict:
    # The ladder at z 974 (grid nodes: foot x 1066.5..1074.5, z 964..974, y
    # 190; top z 974..984, y 220) facing +z, the stick up.  At the top the
    # player stands inside [6]'s area 0x82BD00 above y 210, so its first
    # stage 0x825240 starts script 0x82B6E0 (3B8D = 2; flag 0x1D
    # (D_00810775) = 1; a message that closes by itself); at the script's end
    # D_008107F5 |= 1 | 2 and 001B6660(0x82A590) spawns the group's 0x12E3A0
    # creature at (917, 210, 993) (D_008106C0 = its node).  Control.
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(1080, 965), (1070.5, 967)], "to the ladder")
    a13d_go(r, 1070.5, 968.5, tol=0.8, limit=100)
    a13_settle(r, 5)
    a13_face(r, 0.0)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x16, 0x17))
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["pos"][1] > 218 or row["spad"][2:4] != "00", 1500, 0, 0x7F, 0x00)
    r.set_pad(0)
    for _ in range(4000):
        row = r.step(1)
        if (row["s770"][10:12] == "01" and row["s7f0"][10:12] == "03" and in_control(row)
                and all(in_control(x) for x in r.rows[-60:])):
            break
    else:
        raise TimeoutError("[6]'s first stage not seen to its end: " + summary(r.rows[-1]))
    return {"what": "the ladder up to y 220: [6]'s script 0x82B6E0, flag 0x1D = 1, counter 0x1D = 3, the creature",
            "hp_end": a01_hp(r)}


A19B_BEATS: list[tuple] = [
    ("a19b_00_arrival", "a13d_07_hatch63", a19b_beat_arrival),
    ("a19b_01_ledge", "a19b_00_arrival", a19b_beat_ledge),
    ("a19b_02_ladder1023", "a19b_01_ledge", a19b_beat_ladder1023),
]
A19B_SIDE_BEATS: set[str] = set()
A19B_CHANGE_BEATS: set[str] = {"a19b_00_arrival"}


def twelfth_selected(spec: str) -> list[tuple]:
    """The twelfth-level groups `a13d` and `a19b` (opt-in, story order)."""
    wanted = spec.split(",")
    out = []
    for tag, beats, side in (("a13d", A13D_BEATS, A13D_SIDE_BEATS), ("a19b", A19B_BEATS, A19B_SIDE_BEATS)):
        if tag in wanted:
            out += [b for b in beats if b[0] not in side]
        else:
            out += [b for b in beats if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]
    return out


def twelfth_owners(name: str):
    if name.startswith("a13d_"):
        return A13D_OWNERS
    if name.startswith("a19b_"):
        return A19B_OWNERS
    return None


# Thirteenth level (lane STORY, s93; port docs/THIRTEENTH_LEVEL_ROUTE.md):
# from a19b_02's end (the y-220 platform at the top of the ladder at z 974)
# to [7]'s room, [7]'s Use, door [27] and AREA19 sub 1.  Opt-in group `a19c`
# (AREA19, overlay id 16).  The collision the route uses (AREA19 sub 0 grid,
# the port's export_area19_level.py; sub 1 grid read from the captured RAM):
# the bar (attribute 0x34) at y 253.4 over x 1012.9..1022.9, z 972.4..1039.9
# with its attribute-0x3A pad on the walkway (x 1012.4..1023.4, z
# 1026.3..1039, y 223); the y-195 platform (x 990..1033, z 915..945), whose
# slope to the y-172 floor carries the slide class 0x1000 and stops a walk
# from below at z 972.4; the bar at y 223.2 over x 890.7..1003.2, z
# 925.4..935.4 with its pads on the platform (x 991.7..1004.1, y 198) and in
# [7]'s room (x 888.7..902.7, y 197); door [27] (overlay 0x823580) in the
# wall x 1033..1036; the ladder at z 859.5 (attribute 0x32, y 210 -> 357)
# whose 00196970 circle gives sub 1 entry 7; sub 1's door [52] (001BC350,
# room move id 8).
OUT_A19C = ROOT / "build/s87/route_a19c"
A19C_OWNERS = dict(A19B_OWNERS)       # the same sub-0 load (a19b's nodes); sub-1 rows are not owners


def a19c_until_control(r: Route, limit: int, need: int = 60, pred=None) -> None:
    """Step until `need` frames of control (and `pred` on the last row); no
    input (the scripts of this group play on their own timers)."""
    for _ in range(limit):
        row = r.rows[-1]
        if row["m1F0"] in A04_GRABS:
            a04_shake(r)
            continue
        row = r.step(1)
        if (in_control(row) and len(r.rows) > need and all(in_control(x) for x in r.rows[-need:])
                and (pred is None or pred(row))):
            return
    raise TimeoutError("control not reached: " + summary(r.rows[-1]))


def a19c_bar_grab(r: Route, yaw: float) -> None:
    """Cross on an attribute-0x3A pad facing `yaw`: 0015D4C0's case 0x3A finds
    the attribute-0x34 bar 40 above and enters the hang (+5 0xF, then 0x10)."""
    a13_settle(r, 5)
    a13_face(r, yaw)
    a19_press_until(r, lambda row: row["p5"] in (0xF, 0x10))
    r.until(lambda row: row["p5"] == 0x10 and row["m1F0"] == 0x21, 200)


def a19c_traverse(r: Route, x: float, z: float, limit: int = 900) -> None:
    """Hang-traverse with the stick toward (x, z) until the position holds
    still for 20 frames at the bar's end (or the swing, action 0x28)."""
    for _ in range(limit):
        r.stick_toward(x, z, 1.0)
        row = r.step(1)
        if row["m1F0"] == 0x28:
            return
        if len(r.rows) > 21 and all(abs(a - b) < 0.02 for a, b in zip(r.rows[-21]["pos"], row["pos"])) \
                and r.rows[-21]["m1F0"] == 0x21:
            r.set_pad(0)
            return
    raise TimeoutError("bar end not reached: " + summary(r.rows[-1]))


def a19c_drop(r: Route) -> None:
    """Cross while hanging (00169730 +6 1: use -> +6 0xA, the drop)."""
    for _ in range(6):
        r.idle(5)
        r.press("CROSS", 3)
        for _ in range(40):
            r.step(1)
            if r.rows[-1]["p5"] not in (0xF, 0x10):
                return
    raise TimeoutError("no drop from the bar: " + summary(r.rows[-1]))


def a19c_beat_bar840(r: Route) -> dict:
    # West along the walkway to the bar's pad (attribute 0x3A, y 223); Cross
    # facing -z: the hang (+5 0x10, action 0x21).  The traverse south ends at
    # the bar's end (z 972.4) in the swing (00169730 +6 0x50 -> 0016A4B0,
    # action 0x28); with the stick held the swing grows, Cross (0016A4B0 +7
    # 2 -> 3) releases at the swing's end (+7 4/5: speed D_00248630[+25C]
    # along the heading, 00179880 gravity) and the player lands on the y-195
    # platform (00175900 floor -> 0017C580).
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(1070.5, 1000), (1070, 1035), (1030, 1035), (1018, 1033)], "walkway west to the pad")
    a19c_bar_grab(r, math.pi)
    a19c_traverse(r, 1018.0, 940.0)
    if r.rows[-1]["m1F0"] != 0x28:
        raise RuntimeError("no swing at the bar's end: " + summary(r.rows[-1]))
    for _ in range(120):
        r.stick_toward(1018.0, 940.0, 1.0)
        r.step(1)
    r.set_pad(PAD["CROSS"], *r.pad_state[1:])
    r.step(2)
    for _ in range(400):
        r.stick_toward(1018.0, 940.0, 1.0)
        row = r.step(1)
        if row["p5"] not in (0xF, 0x10) and row["pos"][1] < 196.0:
            break
    else:
        raise TimeoutError("no landing after the release: " + summary(r.rows[-1]))
    r.set_pad(0)
    a13_settle(r, 10)
    x, y, z = r.rows[-1]["pos"]
    if not (y > 194.5 and z < 945.0 and 990.0 < x < 1033.0):
        raise RuntimeError("not on the y-195 platform: " + summary(r.rows[-1]))
    return {"what": "the bar at z 972.4..1039.9: hang, swing at its end, release onto the y-195 platform",
            "hp_end": a01_hp(r)}


def a19c_beat_bar841(r: Route) -> dict:
    # Onto the platform's pad box (y 198), Cross facing -x: the bar at y
    # 223.2; the traverse west to its end (x 890.7) over [7]'s room; Cross
    # drops the player onto the room's pad (y 197).  The room is [6]'s area
    # 0x82BD40 (x 885..933, z 912..959) with 190 <= y <= 200, so 0x825420
    # starts script 0x82BA00 (3B8D = 2, timed message pages); at its end flag
    # 0x1D (D_00810775) = 0xFF and counter 0x1D (D_008107F5) = 0xFF.
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(1010, 931), (998, 930)], "onto the pad box")
    a19c_bar_grab(r, -math.pi / 2)
    a19c_traverse(r, 870.0, 930.0)
    a19c_drop(r)
    r.until(lambda row: row["spad"][2:4] != "00", 300)
    a19c_until_control(r, 9000, pred=lambda row: row["s7f0"][10:12] == "ff" and row["s770"][10:12] == "ff")
    return {"what": "the bar at y 223.2 west to [7]'s room, the drop: [6]'s second stage (script 0x82BA00), "
                    "flag 0x1D = counter 0x1D = 0xFF", "hp_end": a01_hp(r)}


def a19c_beat_use7(r: Route) -> dict:
    # [7] (0x8257C0, class 0x84, descriptor 0x82C6D0) north of it facing -z:
    # Use sets its +0xB bit 2; 0x825930 starts script 0x82BD90 (flag 0x1E 1,
    # counter 0x1E 3, 4 (sound 0x8DF), 5, 3, 0xFF -> D_00810854 |= 4: door
    # [27]'s lock bit 2; flag 0x1E 0xFF).
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(914, 935), (913.7, 928)], "north of [7]")
    a13_settle(r, 5)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00")
    a19c_until_control(r, 6000, pred=lambda row: int(row["l854"], 16) & 4 and row["s7f0"][12:14] == "ff")
    return {"what": "[7]'s Use: script 0x82BD90, counter 0x1E -> 0xFF, flag 0x1E 1 -> 0xFF, "
                    "D_00810854 |= 4 (door [27])", "hp_end": a01_hp(r)}


def a19c_beat_slide(r: Route) -> dict:
    # Back over the bar east (the room's pad facing +x, Cross at the east end
    # onto the platform's pad box), north onto the slope: its class 0x1000
    # starts the slide (00175CF0 / 001796C0, +5 0x1C) down to the y-172 floor.
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_go(r, 896.0, 930.0, tol=1.0, limit=300)
    a19c_bar_grab(r, math.pi / 2)
    a19c_traverse(r, 1012.0, 930.0)
    a19c_drop(r)
    a13_settle(r, 10)
    a13d_go(r, 1015.0, 940.0, tol=1.0, limit=300)
    for _ in range(300):
        r.stick_toward(1015.0, 1000.0, 1.0)
        row = r.step(1)
        if row["pos"][1] < 172.5 and row["pos"][2] > 970:
            break
    else:
        raise TimeoutError("no slide to the y-172 floor: " + summary(r.rows[-1]))
    r.set_pad(0)
    a13_settle(r, 10)
    return {"what": "the bar back east, the slope's slide down to the y-172 floor", "hp_end": a01_hp(r)}


def a19c_beat_walkway(r: Route) -> dict:
    # The stair and the ladder at z 1024 up to the walkway (y 220), east, the
    # ladder at z 974 down to the y-190 platform (a19b_02 reversed).
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(1015, 985), (1012, 1000), (1005, 1014), (1001.5, 1018.5)], "to the ladder's foot")
    a13d_go(r, 1001.5, 1019.5, tol=0.8, limit=100)
    a13d_ladder_up(r, 0.0)
    a13d_walk(r, [(1003, 1036), (1040, 1036), (1070, 1035), (1070.5, 995), (1070.5, 982)], "walkway east")
    a13d_go(r, 1070.5, 979.5, tol=0.8, limit=100)
    a13d_ladder_down(r, math.pi)
    return {"what": "the ladder at z 1024 up, the walkway east, the ladder at z 974 down to y 190",
            "hp_end": a01_hp(r)}


def a19c_beat_door27(r: Route) -> dict:
    # Off the y-190 platform onto the step at y 170, south along the
    # attribute-0x39 ledge (the wall-side shuffle), west to door [27]'s east
    # side; Use facing -x (overlay 0x823580: D_00810854 bit 2 set): room move
    # id 2 to entry 2 (area bytes 13 00 02 13).
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(1080, 968), (1092, 963), (1092, 956.5), (1097.5, 955.5), (1097.5, 946), (1097.5, 930),
                  (1097.5, 890), (1096, 874), (1090, 862), (1065, 845), (1052, 834), (1046, 837),
                  (1040.5, 837.3)], "to door [27]'s east side")
    a13_settle(r, 5)
    a13_face(r, -math.pi / 2)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00")
    a19c_until_control(r, 1500, pred=lambda row: row["area4"] == "13000213")
    return {"what": "the east ledge south, door [27] from the east: entry 2", "hp_end": a01_hp(r)}


def a19c_beat_ladder959(r: Route) -> dict:
    # West over the floor behind door [27] (y 170), the slope down to y 159,
    # the stair (attribute 0x35) up west to the y-210 floor, the ladder at z
    # 859.5 facing -z: climbing above the 00196970 circle's height gives sub
    # 1 entry 7 (13 01 07 13); control on sub 1's y-380 platform.
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(1005, 833), (985, 838), (980, 850), (978, 868), (975, 887), (960, 887.5), (940, 887.5),
                  (925, 887.5), (915, 885), (915, 870), (925.1, 867)], "to the ladder at z 859.5")
    a13d_go(r, 925.1, 865.0, tol=0.8, limit=100)
    a13_settle(r, 5)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x16, 0x17))
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["area4"] == "13010713", 3000, 0, 0x7F, 0x00)
    r.set_pad(0)
    a19c_until_control(r, 3000)
    return {"what": "the floor behind door [27], the stair to y 210, the ladder at z 859.5: sub 1 entry 7",
            "hp_end": a01_hp(r)}


def a19c_beat_door52(r: Route) -> dict:
    # Sub 1's door [52] (001BC350, room move id 8) on the y-380 platform's
    # south rail facing +z: entry 1 (13 01 01 13); [34] (0x8279E0) runs script
    # 0x82E090 for spawn entry 1 (flag 0x46 1 -> 0xFF, counter 0x46 1 -> 0xFF).
    use_a19b_sampler(r)
    next_long_frames(r)
    a13d_walk(r, [(945, 852), (969.7, 853)], "to door [52]")
    a13_settle(r, 5)
    a13_face(r, 0.0)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00")
    a19c_until_control(r, 3000, pred=lambda row: row["area4"] == "13010113" and row["s81c"][4:6] == "ff")
    return {"what": "sub 1 door [52]: entry 1, [34]'s script (flag / counter 0x46 = 0xFF)", "hp_end": a01_hp(r)}


A19C_BEATS: list[tuple] = [
    ("a19c_00_bar840", "a19b_02_ladder1023", a19c_beat_bar840),
    ("a19c_01_bar841", "a19c_00_bar840", a19c_beat_bar841),
    ("a19c_02_use7", "a19c_01_bar841", a19c_beat_use7),
    ("a19c_03_slide", "a19c_02_use7", a19c_beat_slide),
    ("a19c_04_walkway", "a19c_03_slide", a19c_beat_walkway),
    ("a19c_05_door27", "a19c_04_walkway", a19c_beat_door27),
    ("a19c_06_ladder959", "a19c_05_door27", a19c_beat_ladder959),
    ("a19c_07_door52", "a19c_06_ladder959", a19c_beat_door52),
]
A19C_SIDE_BEATS: set[str] = set()
A19C_CHANGE_BEATS: set[str] = {"a19c_05_door27", "a19c_06_ladder959", "a19c_07_door52"}


def thirteenth_selected(spec: str) -> list[tuple]:
    """The thirteenth-level group `a19c` (opt-in)."""
    wanted = spec.split(",")
    if "a19c" in wanted:
        return [b for b in A19C_BEATS if b[0] not in A19C_SIDE_BEATS]
    return [b for b in A19C_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def thirteenth_owners(name: str):
    return A19C_OWNERS if name.startswith("a19c_") else None

# Fourteenth level (lane STORY, s94; port docs/FOURTEENTH_LEVEL_ROUTE.md):
# from a19c_07's end (AREA19 sub 1 entry 1) to AREA15.  Opt-in group `a19d`
# (AREA19, overlay id 16; its last beat changes to AREA15, overlay id 12).
# What the route uses (code, then the captured grids; port doc section 2):
# the two 0012E3A0 creatures of sub 1's hall (+0x34 = 100; the R1 lock D_008106E0
# and Circle); the valve [38] (0x826570: flag / counter 0x20, the fire's cell
# key 7 off); the ladder at (892, 929.5) to the y-410 ledge, the box with the
# attribute-0x3A pad under the attribute-0x1E ceiling (0015D4C0 case 0x3A:
# the hand-over-hand hang, +5 0x11 / 0x12); the valve [37] (0x826840: the
# lift [36] 0x826C10 up 15, D_008107F9 low nibble 1); the pickup g[3] (item
# 0x1E) under the raised lift; the lift's box (850 < x < 859.6, 850.5 < z <
# 855): script 0x82D290, flag 0x21 = 0xFF, the ladder top's cell key 0x15
# off; the ladder down (00193EB0: sub 0 entry 0xD); sub 0's [10] (0x827790);
# the bar 858 (attribute 0x34) from the pad 919, the hang's sideways Use onto
# the post ladder 859 (001787B0), the post's sideways Use onto the bar 1195,
# the drop onto [7]'s room's roof (y 230), the ladder 1016 up to the y-265
# deck, the ladder 694 (00196970: sub 1 entry 8, the stair tower); the seal
# [48] (001581A0, light melee: D_00810854 bit 3); the tower's flights to the
# y-450 landing and door [50] (001BC350, id 0x86: AREA15 entry 0 sub 0).
OUT_A19D = ROOT / "build/s87/route_a19d"
A19D_OWNERS = {                       # the sub-1 load of a19c_07 (sub-0 / tower rows read other nodes)
    "r34_8279E0": 0x7AD780, "r35_827430": 0x7ADA70, "r36_826C10": 0x7ADD60, "r37_826840": 0x7AE050,
    "r38_826570": 0x7AE340, "r39_823D10": 0x7AE630, "r40_823780": 0x7AE920, "r46_829A70": 0x7AFAC0,
    "seal48_1581A0": 0x7B00A0, "door49_1BC350": 0x7B0390,
    "beastA_12E3A0": 0x7A7980, "beastB_12E3A0": 0x7A7C70,
    "bugA_12A5D0": 0x7A70B0, "bugB_12A5D0": 0x7A73A0, "bugC_12A5D0": 0x7A7690,
}
A19D_SPANS = [sp for sp in A19_SPANS if ":" not in sp[0]] + [
    ("s77c", 0x81077C, 0x4),            # D_0081077C..7F (flags 0x24..0x27)
    ("s7fc", 0x8107FC, 0x4),            # D_008107FC..FF (counters 0x24..0x27)
    ("lock6e0", 0x8106E0, 0xC),         # the three R1 locks D_008106E0..E8
    ("ammo", 0x810C60, 0x4),            # D_00810C62 rounds in the gun
    ("resv", 0x810CB4, 0x4),            # D_00810CB4 rounds held (gun included)
    ("inv64", 0x810C64, 0x40),          # D_00810C64.. item counts
]
for _name, _base in A19D_OWNERS.items():
    A19D_SPANS += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                   (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                   (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10),
                   (_name + ":k", _base + 0x30, 0x10)]       # +0x34 hit points, +0x36 damage
A19D_GRABS = (0x3B, 0x3C, 0x3D, 0x3E)  # bug on the back, flyer's hold, bite


def decode_a19d(r: dict[str, bytes], owners=None) -> dict:
    owners = A19D_OWNERS if owners is None else owners
    row = decode_a19(r, owners=owners)
    row["s77c"] = r["s77c"].hex()
    row["s7fc"] = r["s7fc"].hex()
    row["lock"] = [hex(v) for v in struct.unpack("<3I", r["lock6e0"])]
    row["mag"] = r["ammo"][2]
    row["rounds"] = struct.unpack("<H", r["resv"][:2])[0]
    row["inv64"] = r["inv64"].hex()
    for name in owners:
        if name + ":k" in r:
            row[name]["k"] = r[name + ":k"].hex()
    return row


def use_a19d_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=A19D_SPANS)
    r.sampler = sampler
    r.now = lambda: decode_a19d(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)


def a19d_hp(row: dict, owner: str) -> int:
    return struct.unpack_from("<h", bytes.fromhex(row[owner]["k"]), 4)[0]


def a19d_rock(r: Route, limit: int = 300) -> None:
    """Out of a grab: the stick rocked left and right (a04_shake for every
    grab action of this group)."""
    for i in range(limit):
        if r.rows[-1]["m1F0"] not in A19D_GRABS:
            break
        r.set_pad(0, 0x00 if (i // 2) % 2 else 0xFF, 0x7F)
        r.step(1)
    r.set_pad(0)


def a19d_unshake(r: Route) -> None:
    for _ in range(400):
        if r.rows[-1]["m1F0"] in A04_GRABS:
            a04_shake(r)
        else:
            return


def a19d_walk13(r: Route, points, tol: float = 1.0) -> None:
    """a13d_go to each point in turn (no position check: the beats below are
    the exploration's inputs, replayed exactly)."""
    for p in points:
        a13d_go(r, p[0], p[1], tol=tol, limit=400)


def a19d_go_rock(r: Route, x: float, z: float, tol: float = 1.0, limit: int = 400) -> None:
    """a19d_go's twin with a19d_rock(limit 200) (the exploration's `go`)."""
    for _ in range(limit):
        if r.rows[-1]["m1F0"] in A19D_GRABS:
            a19d_rock(r, 200)
            continue
        px, _py, pz = r.rows[-1]["pos"]
        d = math.hypot(x - px, z - pz)
        if d <= tol:
            break
        r.stick_toward(x, z, min(1.0, max(0.4, d / 12.0)))
        r.step(1)
    r.set_pad(0)


def a19d_trav(r: Route, x: float, z: float, n: int = 400) -> None:
    for _ in range(n):
        r.stick_toward(x, z, 1.0)
        r.step(1)
        px, _py, pz = r.rows[-1]["pos"]
        if math.hypot(px - x, pz - z) < 1.5:
            break
    r.set_pad(0)
    r.step(5)


def a19d_ceiling_drop(r: Route) -> None:
    for _ in range(8):
        r.idle(5)
        r.press("CROSS", 3)
        for _ in range(60):
            r.step(1)
            if r.rows[-1]["p5"] not in (0x11, 0x12):
                return
    raise TimeoutError("no drop: " + summary(r.rows[-1]))


def a19d_try_climb(r: Route, yaw: float, tries: int = 3) -> bool:
    a13_settle(r, 5)
    a13_face(r, yaw)
    y0 = r.rows[-1]["pos"][1]
    for _ in range(tries):
        r.idle(3)
        r.press("CROSS", 2)
        r.step(90)
        if r.rows[-1]["pos"][1] > y0 + 3:
            return True
    return False


def a19d_beat_beastA(r: Route) -> dict:
    # From the hall's middle (920, 900) facing -x, the fire between: R1 held
    # (the lock D_008106E0 takes the creature A, 0012E3A0, west of the fire),
    # Circle every 14 frames until its +0x34 is 0.
    use_a19d_sampler(r)
    next_long_frames(r)
    a13_settle(r, 5)
    a19d_walk13(r, [(945, 895), (920, 900)])
    a13_face(r, -math.pi / 2)
    hold = PAD["R1"]
    r.set_pad(hold)
    r.step(20)
    shots = 0
    while a19d_hp(r.rows[-1], "beastA_12E3A0") > 0 and shots < 40:
        r.set_pad(hold | PAD["CIRCLE"])
        r.step(2)
        r.set_pad(hold)
        r.step(12)
        shots += 1
        if r.rows[-1]["hp"] <= 0:
            break
    r.set_pad(hold)
    r.step(20)
    r.set_pad(0)
    r.step(20)
    if a19d_hp(r.rows[-1], "beastA_12E3A0") > 0:
        raise RuntimeError("creature A not down: " + summary(r.rows[-1]))
    return {"what": "creature A shot from (920, 900) across the fire", "presses": shots, "hp_end": a01_hp(r)}


def a19d_beat_ceiling(r: Route) -> dict:
    # The valve [38] (0x826570) at (890, 385, 836.2) facing -z: script
    # 0x82CA20, flag / counter 0x20 = 0xFF, the fire's player-only cell (key
    # 7) off.  The ladder at (892, 929.5) up to the y-410 ledge; the box (y
    # 424) by a ledge climb; Cross on its attribute-0x3A pad under the
    # attribute-0x1E ceiling (y 446.5): the hand-over-hand hang (+5 0x11 /
    # 0x12); west along z 927, south to z 908, west to (853, 906).
    use_a19d_sampler(r)
    next_long_frames(r)
    a13_settle(r, 5)
    a19d_walk13(r, [(897, 908), (893, 880), (893, 855), (890, 841.2)])
    a13_settle(r, 5)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00")
    a19c_until_control(r, 3000, pred=lambda row: row["s778"][0:2] == "ff")
    a19d_walk13(r, [(900, 860), (898, 900), (887, 929.5)])
    a13d_ladder_up(r, math.pi / 2)
    a19d_walk13(r, [(921.0, 928.0)])
    a13_face(r, math.pi / 2)
    a19_press_until(r, lambda row: row["pos"][1] > 420 or row["m1F0"] not in (0, 1))
    r.until(in_control, 600)
    a13_settle(r, 5)
    a19d_walk13(r, [(935.0, 928.0)])
    a13_settle(r, 5)
    a19_press_until(r, lambda row: row["m1F0"] == 0x20 or row["p5"] == 0x11)
    r.until(lambda row: row["p5"] == 0x12, 200)
    a19d_trav(r, 886.0, 927.0)
    a19d_trav(r, 885.0, 908.0)
    a19d_trav(r, 853.0, 906.0)
    if r.rows[-1]["p5"] != 0x12:
        raise RuntimeError("not hanging at (853, 906): " + summary(r.rows[-1]))
    return {"what": "the valve [38] (flag / counter 0x20), the ladder to the y-410 ledge, the box, "
                    "the ceiling hang west to (853, 906)", "hp_end": a01_hp(r)}


def a19d_beat_lockB(r: Route) -> dict:
    # Cross drops from the ceiling; north round the barrel [18] to (790, 908);
    # facing -z with R1 held the lock takes the creature B (0012E3A0) in the
    # alcove west of the lift.
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_ceiling_drop(r)
    r.until(lambda row: in_control(row) and row["pos"][1] < 372, 600)
    a13_settle(r, 5)
    a19d_walk13(r, [(842, 916), (826, 919), (808, 916), (790, 908)])
    a13_face(r, math.pi)
    r.set_pad(PAD["R1"])
    r.step(80)
    r.set_pad(0)
    r.step(10)
    return {"what": "the drop at (853, 906), the walk to (790, 908), R1: the lock on creature B",
            "lock": r.rows[-1]["lock"], "hp_end": a01_hp(r)}


def a19d_beat_beastB(r: Route) -> dict:
    # R1 held, Circle every 14 frames while a lock is held, until the
    # creature B's +0x34 is 0.
    use_a19d_sampler(r)
    next_long_frames(r)
    hold = PAD["R1"]
    r.set_pad(hold)
    r.step(30)
    shots = idle = 0
    while a19d_hp(r.rows[-1], "beastB_12E3A0") > 0 and shots < 60 and idle < 300:
        if r.rows[-1]["lock"][0] == "0x0":
            r.set_pad(hold)
            r.step(1)
            idle += 1
            continue
        r.set_pad(hold | PAD["CIRCLE"])
        r.step(2)
        r.set_pad(hold)
        r.step(12)
        shots += 1
        if r.rows[-1]["hp"] <= 0:
            break
    r.set_pad(0)
    r.step(30)
    if a19d_hp(r.rows[-1], "beastB_12E3A0") > 0:
        raise RuntimeError("creature B not down: " + summary(r.rows[-1]))
    return {"what": "creature B shot from (788, 901.6)", "presses": shots, "hp_end": a01_hp(r)}


def a19d_beat_alcove(r: Route) -> dict:
    # South along the lift's west side; three Cross presses facing +x at x
    # 795.4 (z 878.6, then 868) start no climb onto the lift (its truck [35]
    # and the lift [36] are kind 0x46 owners; exploration lead, section 6);
    # on into the alcove at (819, 842).
    use_a19d_sampler(r)
    next_long_frames(r)
    a13_settle(r, 5)
    a19d_walk13(r, [(792, 890), (795.5, 878)])
    climbed = a19d_try_climb(r, math.pi / 2)
    if not climbed:
        a19d_walk13(r, [(795.5, 868)])
        climbed = a19d_try_climb(r, math.pi / 2)
    if climbed:
        raise RuntimeError("an unexpected climb: " + summary(r.rows[-1]))
    a19d_walk13(r, [(797, 855), (812, 850), (819, 842)])
    return {"what": "the lift's west side (no climb), the alcove", "hp_end": a01_hp(r)}


def a19d_beat_valve37(r: Route) -> dict:
    # The valve [37] (0x826840) at (819, 385, 836.2) facing -z: script
    # 0x82CC70 (D_008107F9 bit 7), the lift [36]'s +0x2EC = 0x8A; after the
    # countdown the lift rises 15 over 180 frames (D_008107F9 low nibble 1).
    use_a19d_sampler(r)
    next_long_frames(r)
    a13_settle(r, 5)
    a19d_walk13(r, [(819, 841.2)], tol=0.6)
    a13_settle(r, 5)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00" or row["r37_826840"]["h"][10:12] != "00")
    for i in range(900):
        row = r.step(1)
        if in_control(row) and i > 400 and row["r36_826C10"]["pos"][1] > 384.9:
            break
    else:
        raise TimeoutError("the lift did not rise: " + summary(r.rows[-1]))
    return {"what": "the valve [37]: the lift up 15 (D_008107F9 = 1)", "hp_end": a01_hp(r)}


def a19d_beat_g3(r: Route) -> dict:
    # North round to the lift's north side, under the raised lift to the
    # pickup g[3] (0015AFA0, item 0x1E) at (822.2, 370.4, 878.8): Cross takes
    # it (the HEALING page).
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_unshake(r)
    a13_settle(r, 5)
    a19d_walk13(r, [(812, 912), (840, 912), (842, 906), (842, 890), (830, 882)])
    px, _py, pz = r.rows[-1]["pos"]
    a13c_take(r, px, pz, math.atan2(822.2 - px, 878.8 - pz))
    for _ in range(400):
        r.step(1)
        if r.rows[-1]["ui"][2:4] == "03":
            break
    r.idle(60)
    return {"what": "under the lift: the pickup g[3] (item 0x1E) taken, the HEALING page", "hp_end": a01_hp(r)}


def a19d_beat_cage(r: Route) -> dict:
    # On the HEALING page: Cross (the item), Cross, Left (Yes), Cross: health
    # +30; Triangle closes it.  On under the lift into the cage round the
    # ladder top: the lift's box (850 < x < 859.6, 850.5 < z < 855) starts
    # script 0x82D290 (the lift drops back); at its end flag 0x21 (D_00810779)
    # = 0xFF and the ladder top's cell (key 0x15) is off.  The ladder (z
    # 844.7) facing -z: below y 356 (x < 872) 00193EB0 requests sub 0 entry
    # 0xD (13 00 0D); [10] (0x827790) plays its scripts for entry 0xD
    # (counter 0x45 = 0x80, item CC3 0x0C); control on the y-265 deck.
    use_a19d_sampler(r)
    next_long_frames(r)
    hp0 = r.rows[-1]["hp"]
    for _ in range(6):
        r.idle(30)
        r.press("CROSS", 2)
        try:
            r.until(lambda row: row["ui"][10:12] == "04", 90)
            break
        except TimeoutError:
            pass
    r.idle(10)
    r.press("LEFT", 2, after=20)
    r.press("CROSS", 2)
    r.until(lambda row: row["hp"] > hp0, 600)
    r.idle(200)
    for _ in range(8):
        r.press("TRIANGLE", 2)
        r.step(90)
        if in_control(r.rows[-1]) and r.rows[-1]["spad"][2:4] == "00":
            break
    a19d_unshake(r)
    a13_settle(r, 5)
    a19d_walk13(r, [(842, 878), (850, 870), (855, 862), (855, 853)])
    a19c_until_control(r, 4000, pred=lambda row: row["s778"][2:4] == "ff")
    a19d_walk13(r, [(855, 849)], tol=0.8)
    a13_settle(r, 5)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x16, 0x17, 0x18))
    r.set_pad(0, 0x7F, 0xFF)
    r.until(lambda row: row["area4"][:4] == "1300", 3000, 0, 0x7F, 0xFF)
    r.set_pad(0)
    a19c_until_control(r, 3000)
    return {"what": "g[3] used, the cage: script 0x82D290 (flag 0x21), the ladder down: sub 0 entry 0xD, "
                    "[10]'s scripts", "hp_end": a01_hp(r)}


def a19d_beat_west(r: Route) -> dict:
    # West along the deck, the slope's slide down to the y-211 floor, the
    # stair north to the y-229 landing; running north off its end (Cross at
    # z 896 starts no jump) the player lands on the far landing (z 909.5).
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_unshake(r)
    a13_settle(r, 5)
    a19d_walk13(r, [(830, 852), (816, 852), (808, 852), (800, 852)])
    a13_settle(r, 10)
    a19d_walk13(r, [(790, 860), (780, 864)])
    a19d_unshake(r)
    a13_settle(r, 3)
    a13_face(r, 0.0)
    for _ in range(200):
        if r.rows[-1]["pos"][2] >= 896.0:
            break
        r.stick_toward(780.0, 960.0)
        r.step(1)
    r.set_pad(PAD["CROSS"], r.pad_state[1], r.pad_state[2])
    r.step(2)
    r.set_pad(0, r.pad_state[1], r.pad_state[2])
    try:
        r.until(lambda row: row["m1F0"] == 0x0C, 10, 0, r.pad_state[1], r.pad_state[2])
    except TimeoutError:
        pass
    r.until(lambda row: row["m1F0"] not in (0x0C, 0x0F), 300, 0, r.pad_state[1], r.pad_state[2])
    r.set_pad(0)
    r.step(30)
    if not (r.rows[-1]["pos"][2] > 905.0 and r.rows[-1]["pos"][1] > 228.0):
        raise RuntimeError("not on the far landing: " + summary(r.rows[-1]))
    return {"what": "the deck west, the slide, the stair, off the landing's end onto the far landing",
            "hp_end": a01_hp(r)}


def a19d_beat_westdeck(r: Route) -> dict:
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_unshake(r)
    a13_settle(r, 5)
    a19d_walk13(r, [(780, 918), (780, 935), (780, 950), (780, 962), (768, 962), (760, 960)])
    a13_settle(r, 5)
    return {"what": "the stair north up to the y-265 west deck", "hp_end": a01_hp(r)}


def a19d_beat_area82E050(r: Route) -> dict:
    # [10]'s area 0x82E050 (x 745..766, z 892..900): script 0x82DD10, flag
    # 0x45 (D_0081079D) = 0xFF.
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_unshake(r)
    a13_settle(r, 5)
    a19d_walk13(r, [(762, 900)])
    r.step(60)
    a19c_until_control(r, 3000)
    return {"what": "[10]'s area on the west deck: script 0x82DD10 (flag 0x45)", "hp_end": a01_hp(r)}


def a19d_beat_bar858(r: Route) -> dict:
    # Onto the pad 919 (y 268) facing +x: Cross enters the hang on the bar
    # 858 (y 291.5); east to x 810.
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a19d_go_rock(r, 761.5, 890)
    a19d_go_rock(r, 761.5, 884, tol=0.8)
    a19d_rock(r, 200)
    r.set_pad(0)
    r.step(3)
    a13_face(r, math.pi / 2)
    for _ in range(8):
        a19d_rock(r, 200)
        r.press("CROSS", 2)
        for _ in range(40):
            r.step(1)
            if r.rows[-1]["p5"] in (0xF, 0x10):
                break
        if r.rows[-1]["p5"] in (0xF, 0x10):
            break
    r.until(lambda row: row["p5"] == 0x10 and row["m1F0"] == 0x21, 200)
    for _ in range(400):
        r.stick_toward(811.0, 883.0, 1.0)
        r.step(1)
        if r.rows[-1]["pos"][0] >= 799.3 or r.rows[-1]["m1F0"] == 0x28:
            break
    r.set_pad(0)
    r.step(10)
    if not (797.0 <= r.rows[-1]["pos"][0] <= 801.5 and r.rows[-1]["m1F0"] == 0x21):
        raise RuntimeError("not hanging at x 797..801.5: " + summary(r.rows[-1]))
    return {"what": "the pad 919, the hang on the bar 858, east to x 799.3", "hp_end": a01_hp(r)}


def a19d_side_try(r: Route, zdir: float, hold_frames: int, offset: float, done, inner: bool = True) -> None:
    px, _py, pz = r.rows[-1]["pos"]
    for _ in range(hold_frames):
        r.stick_toward(px + offset, pz + 100 * zdir, 1.0)
        r.step(1)
    lx, ly = r.pad_state[1], r.pad_state[2]
    for _ in range(3):
        r.set_pad(PAD["CROSS"], lx, ly)
        r.step(2)
        r.set_pad(0, lx, ly)
        for _ in range(40):
            r.step(1)
            if inner and done(r.rows[-1]):
                break
        if done(r.rows[-1]):
            break
    r.step(60)
    r.set_pad(0)
    r.step(5)


def a19d_beat_post(r: Route) -> dict:
    # Hanging at x ~799.5 facing +x: the stick toward +z and Cross: 00169730's
    # sideways case, 001787B0 finds the post's ladder 859 (x 809.5): action
    # 0x12, then the ladder (0x17).  (Exploration: taken from x 799.7 facing
    # +x, not from x 804.2.)
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_side_try(r, 1.0, 50, 0.0, lambda row: row["m1F0"] not in (0x20, 0x21, 0x22))
    if r.rows[-1]["m1F0"] != 0x12:
        raise RuntimeError("no transfer to the post's ladder: " + summary(r.rows[-1]))
    return {"what": "the bar 858 to the post's ladder 859 (sideways Use)", "hp_end": a01_hp(r)}


def a19d_beat_bar1195(r: Route) -> dict:
    # Down the post's ladder to y 244; the stick toward +z and Cross: onto
    # the bar 1195 (y 261.5) under the pipe.
    use_a19d_sampler(r)
    next_long_frames(r)
    r.step(80)
    for _ in range(400):
        r.set_pad(0, 0x7F, 0xFF)
        r.step(1)
        if r.rows[-1]["pos"][1] <= 244.0:
            break
    r.set_pad(0)
    r.step(20)
    a19d_side_try(r, 1.0, 50, 0.01, lambda row: row["m1F0"] in (0x20, 0x21, 0x22, 0x23), inner=False)
    if r.rows[-1]["m1F0"] != 0x21:
        raise RuntimeError("no transfer to the bar 1195: " + summary(r.rows[-1]))
    return {"what": "down the post's ladder, the bar 1195 (sideways Use)", "hp_end": a01_hp(r)}


def a19d_beat_roof(r: Route) -> dict:
    # East along the bar 1195 to x 889; Cross drops onto [7]'s room's roof
    # (y 230).
    use_a19d_sampler(r)
    next_long_frames(r)
    for _ in range(900):
        r.stick_toward(892.0, 922.0, 1.0)
        r.step(1)
        if r.rows[-1]["pos"][0] >= 889.0 or r.rows[-1]["m1F0"] == 0x28:
            break
    r.set_pad(0)
    r.step(10)
    if r.rows[-1]["pos"][0] < 885.0:
        raise RuntimeError("not over the roof: " + summary(r.rows[-1]))
    a19c_drop(r)
    r.until(in_control, 400)
    a13_settle(r, 5)
    return {"what": "the bar 1195 east, the drop onto the roof (y 230)", "hp_end": a01_hp(r)}


def a19d_beat_deckB(r: Route) -> dict:
    # The ladder 1016 (z 939, attribute 0x32) facing +z up to the y-265 deck.
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a19d_go_rock(r, 915, 930)
    a19d_go_rock(r, 915, 934.5, tol=0.6)
    a13d_ladder_up(r, 0.0)
    return {"what": "the ladder 1016 up to the y-265 deck", "hp_end": a01_hp(r)}


def a19d_beat_ladder694(r: Route) -> dict:
    # Round the slab to the ladder 694 (z 960) facing -z; above 00196970's
    # circle height: sub 1 entry 8 (13 01 08) in the stair tower.
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a19d_go_rock(r, 919, 955)
    a19d_go_rock(r, 919, 966)
    a19d_go_rock(r, 907, 966, tol=0.6)
    a13d_go(r, 906.7, 964.5, tol=0.4, limit=100)
    a13_settle(r, 3)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x16, 0x17))
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["area4"][:4] == "1301", 3000, 0, 0x7F, 0x00)
    r.set_pad(0)
    a19c_until_control(r, 3000)
    return {"what": "the ladder 694: sub 1 entry 8 (the stair tower)", "hp_end": a01_hp(r)}


def a19d_beat_seal_far(r: Route) -> dict:
    # Out of the ladder's pocket east under the flights; in the kept capture
    # the player stopped at (984.4, 946.5) facing -z and six Circle presses
    # started three light melees (action 0x36) that left the seal [48]
    # intact.  The census replay of this beat broke it on its first press:
    # the loop stops when D_00810854 changes.
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    for p in ((907, 947), (930, 948), (965, 949)):
        a19d_go_rock(r, *p)
    a19d_go_rock(r, 980.7, 947, tol=0.6)
    a13_settle(r, 3)
    a13_face(r, math.pi)
    for _ in range(6):
        r.press("CIRCLE", 2)
        r.step(45)
        if r.rows[-1]["l854"] != "04":
            break
    a13_settle(r, 10)
    return {"what": "the seal [48]: Circle presses from the far spot (kept: six presses, three melees, no break)", "hp_end": a01_hp(r)}


def a19d_beat_seal(r: Route) -> dict:
    # Closer, (980.9, 946.2) facing -z: the second Circle breaks the seal [48]
    # (001581A0: its +0x36, then D_00810854 |= 8, door [49]'s bit 3).
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a13d_go(r, 980.7, 945.0, tol=0.3, limit=150)
    a13_settle(r, 3)
    a13_face(r, math.pi, tol=0.05)
    for btn in ("CIRCLE", "CIRCLE", "SQUARE", "SQUARE"):
        r.press(btn, 2)
        r.step(60)
        if r.rows[-1]["l854"] != "04":
            break
    a13_settle(r, 10)
    if not int(r.rows[-1]["l854"], 16) & 8:
        raise RuntimeError("the seal held: " + summary(r.rows[-1]))
    return {"what": "the seal [48] broken: D_00810854 bit 3", "hp_end": a01_hp(r)}


A19D_TOWER_TO_450 = [(982, 958), (980, 975), (965, 975), (940, 975), (920, 975), (906, 975), (906, 955),
                     (918, 952), (940, 952), (964, 952), (977, 952), (977, 975), (960, 975), (930, 975),
                     (912, 975), (905, 975), (905, 955), (903, 947)]


def a19d_beat_flights(r: Route) -> dict:
    # The tower's flights: y 370 -> 395.5 (z 965..985 west), -> 421 (z
    # 942..962 east), -> 450 (z 965..985 west); the y-450 landing.
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    for p in A19D_TOWER_TO_450:
        a19d_go_rock(r, *p)
    a13_settle(r, 3)
    return {"what": "the tower's flights to the y-450 landing", "hp_end": a01_hp(r)}


def a19d_beat_door50(r: Route) -> dict:
    # Door [50] (001BC350, id 0x86) facing -z: AREA15 entry 0 sub 0; AREA15
    # sub 0's [0] (0x8235A0) plays script 0x826E70 (D_00810702 = 1, item CC3
    # 0x0D, counter 0x22 = 1, flag 0x22 = 0xFF); control in AREA15.
    use_a19d_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00")
    r.until(lambda row: row["area4"][:2] == "0f", 4000)
    a19c_until_control(r, 30000)
    return {"what": "door [50]: AREA15 sub 0, [0]'s script 0x826E70 (flag 0x22)", "hp_end": a01_hp(r)}


A19D_BEATS: list[tuple] = [
    ("a19d_00_beastA", "a19c_07_door52", a19d_beat_beastA),
    ("a19d_01_ceiling", "a19d_00_beastA", a19d_beat_ceiling),
    ("a19d_02_lockB", "a19d_01_ceiling", a19d_beat_lockB),
    ("a19d_03_beastB", "a19d_02_lockB", a19d_beat_beastB),
    ("a19d_04_alcove", "a19d_03_beastB", a19d_beat_alcove),
    ("a19d_05_valve37", "a19d_04_alcove", a19d_beat_valve37),
    ("a19d_06_g3", "a19d_05_valve37", a19d_beat_g3),
    ("a19d_07_cage", "a19d_06_g3", a19d_beat_cage),
    ("a19d_08_west", "a19d_07_cage", a19d_beat_west),
    ("a19d_09_westdeck", "a19d_08_west", a19d_beat_westdeck),
    ("a19d_10_area82E050", "a19d_09_westdeck", a19d_beat_area82E050),
    ("a19d_11_bar858", "a19d_10_area82E050", a19d_beat_bar858),
    ("a19d_12_post", "a19d_11_bar858", a19d_beat_post),
    ("a19d_13_bar1195", "a19d_12_post", a19d_beat_bar1195),
    ("a19d_14_roof", "a19d_13_bar1195", a19d_beat_roof),
    ("a19d_15_deckB", "a19d_14_roof", a19d_beat_deckB),
    ("a19d_16_ladder694", "a19d_15_deckB", a19d_beat_ladder694),
    ("a19d_17_seal_far", "a19d_16_ladder694", a19d_beat_seal_far),
    ("a19d_18_seal", "a19d_17_seal_far", a19d_beat_seal),
    ("a19d_19_flights", "a19d_18_seal", a19d_beat_flights),
    ("a19d_20_door50", "a19d_19_flights", a19d_beat_door50),
]
A19D_SIDE_BEATS: set[str] = set()
A19D_CHANGE_BEATS: set[str] = {"a19d_07_cage", "a19d_16_ladder694", "a19d_20_door50"}


def fourteenth_selected(spec: str) -> list[tuple]:
    """The fourteenth-level group `a19d` (opt-in)."""
    wanted = spec.split(",")
    if "a19d" in wanted:
        return [b for b in A19D_BEATS if b[0] not in A19D_SIDE_BEATS]
    return [b for b in A19D_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def fourteenth_owners(name: str):
    return A19D_OWNERS if name.startswith("a19d_") else None


# AREA15 (overlay id 12) from a19d_20's end (sub 0 entry 1, after [0]'s
# script): the slider [13] (001BB860, room move id 1: entry 2), door [14]
# (001BC350, id 0x80: AREA19 entry 4 sub 1, the tower's y-450 landing); the
# tower's flights to the y-500 landing and door [51] (id 0x87: AREA15 entry 0
# sub 1).  Opt-in group `a15`; no owner rows.
OUT_A15 = ROOT / "build/s87/route_a15"


def use_a15_sampler(r: Route) -> None:
    sampler = A01USampler(r.s, spans=[sp for sp in A19D_SPANS if ":" not in sp[0]])
    r.sampler = sampler
    r.now = lambda: decode_a19d(sampler.raw(), owners={})
    r.rows[0] = dict(r.now(), f=0)


def a15_beat_door14(r: Route) -> dict:
    use_a15_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a19d_go_rock(r, 935, 911)
    a19d_go_rock(r, 927.3, 913.7, tol=0.5)
    a13_settle(r, 3)
    a13_face(r, -0.66)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00" or row["area4"][4:6] == "02")
    a19c_until_control(r, 2000, pred=lambda row: row["area4"][4:6] == "02")
    a19d_go_rock(r, 905, 932)
    a19d_go_rock(r, 902, 935, tol=0.6)
    a13_settle(r, 3)
    a13_face(r, 0.0)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00")
    r.until(lambda row: row["area4"][:2] == "13", 4000)
    a19c_until_control(r, 4000)
    return {"what": "AREA15 sub 0: the slider [13] (entry 2), door [14]: AREA19 sub 1 entry 4 (the y-450 landing)",
            "hp_end": a01_hp(r)}


def a15_beat_door51(r: Route) -> dict:
    use_a15_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    for p in [(907, 952), (918, 952), (940, 952), (964, 952), (977, 952), (977, 975), (960, 975), (930, 975),
              (912, 975), (905, 975), (905, 955), (903, 947)]:
        a19d_go_rock(r, *p)
    a13_settle(r, 3)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00")
    r.until(lambda row: row["area4"][:2] == "0f", 4000)
    a19c_until_control(r, 20000)
    return {"what": "the tower's flights to the y-500 landing, door [51]: AREA15 sub 1 entry 0", "hp_end": a01_hp(r)}


A15_BEATS: list[tuple] = [
    ("a15_00_door14", "a19d_20_door50", a15_beat_door14),
    ("a15_01_door51", "a15_00_door14", a15_beat_door51),
]
A15_SIDE_BEATS: set[str] = set()
A15_CHANGE_BEATS: set[str] = {"a15_00_door14", "a15_01_door51"}


def fourteenth_a15_selected(spec: str) -> list[tuple]:
    wanted = spec.split(",")
    if "a15" in wanted:
        return list(A15_BEATS)
    return [b for b in A15_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]

# Fifteenth level (lane STORY, s95; port docs/FIFTEENTH_LEVEL_ROUTE.md): from
# a15_01's end (AREA15 sub 1 entry 0) through sub 1's event and the forced
# return to AREA15 sub 0, then back into AREA19 sub 1 for the truck's key.
# Opt-in groups `a15b` (AREA15, overlay id 12; its last beat changes to
# AREA19), `a19e` (AREA19, overlay id 16; its last main-line beat changes to
# AREA03) and `a03` (AREA03, overlay id 4: sub 0's lift).  What the route uses (code,
# then the captured grids; port doc section 2): AREA15 sub 1's [4]
# (0x823850 / 0x8239F0, C: the player's y >= 310 inside the polygon 0x827C80,
# script 0x827400, flag 0x23 = 1, D_008107FB = 1 and a 900-frame count;
# 0x823B40: D_008107FB = 2 at the count's end; 0x823C80: script 0x8277C0,
# items C64 0x06 / CC3 0x0E, the sub-states of five areas, the request AREA15
# entry 0 sub 0); the mezzanine (grid floor 1437, y 315.1) by the stair 794
# and the stair 792; AREA15 sub 0's [1] / [2] (0x824070 NM, 0x824240 /
# 0x824350 C: scripts 0x827D70 (item 0x25) and 0x8281F0); door [14]; in
# AREA19 sub 1 the tower's flights and door [49] into the hall, the ladder
# at (892, 929.5), the ceiling hang, the truck [46] (0x829A70, C): its Use
# point (the descriptor 0x82F790: (817.5, 379, 931.4), radius 5, height 6,
# yaw -2.0944) at the top of the stair 404, the item 0x25 used from the
# status screen's EVENT page (00215870: the device's +0xB = 5), D_00810838 =
# 1; the pickups g[0] (item 0x1C, the 18-gauge battery pack) and g[7] (item
# 0x1F) inside the truck.  Side beats: the ladder 694 down to sub 0 (13 00 0B)
# and the panel [24] refusing the charge (00158EC0 model 0x22 costs 2 * 0x10
# half-units).
OUT_A15B = ROOT / "build/s87/route_a15b"
OUT_A19E = ROOT / "build/s87/route_a19e"
A15B_EXTRA_SPANS = [
    ("s838", 0x810838, 0x4),            # D_00810838 (AREA19 sub 1 [46]: the truck's door)
    ("t130", 0x810130, 0x30),           # the status block D_00810130 (page id, hovers, list cursor)
    ("batt", 0x810CB0, 0x8),            # D_00810CB2 charge, D_00810CB7 capacity (half-units)
]
A19E_OWNERS = {                       # a15b_04's sub-1 load (measured in the exploration)
    "beastC_12E3A0": 0x7A70B0, "beastD_12E3A0": 0x7A73A0, "truck46_829A70": 0x7AF1F0,
}


def decode_a15b(r: dict[str, bytes], owners=None) -> dict:
    owners = {} if owners is None else owners
    row = decode_a19d(r, owners=owners)
    row["s838"] = r["s838"].hex()
    row["t130"] = r["t130"].hex()
    row["batt"] = r["batt"].hex()
    row["inf"] = round(f32(r["player"], 0x228), 3)
    for name in owners:
        if name + ":k" in r:
            row[name]["k"] = r[name + ":k"].hex()
    return row


def use_a15b_sampler(r: Route, owners=None) -> None:
    spans = [sp for sp in A19D_SPANS if ":" not in sp[0]] + A15B_EXTRA_SPANS
    for _name, _base in (owners or {}).items():
        spans += [(_name + ":h", _base, 0x10), (_name + ":p", _base + 0xB0, 0x10),
                  (_name + ":s", _base + 0x1F0, 0x10), (_name + ":t", _base + 0x2DC, 0x14),
                  (_name + ":c", _base + 0x10, 0x4), (_name + ":r", _base + 0xC0, 0x10),
                  (_name + ":k", _base + 0x30, 0x10)]
    sampler = A01USampler(r.s, spans=spans)
    r.sampler = sampler
    r.now = lambda: decode_a15b(sampler.raw(), owners=owners or {})
    r.rows[0] = dict(r.now(), f=0)


def a15b_walk(r: Route, points, stop=None, tol: float = 1.0, limit: int = 400) -> None:
    """a19d_go_rock per point; ends at once when `stop(row)` holds (a script
    took the player)."""
    for x, z in points:
        for _ in range(limit):
            row = r.rows[-1]
            if stop is not None and stop(row):
                r.set_pad(0)
                return
            if row["m1F0"] in A19D_GRABS:
                a19d_rock(r, 200)
                continue
            px, _py, pz = row["pos"]
            d = math.hypot(x - px, z - pz)
            if d <= tol:
                break
            r.stick_toward(x, z, min(1.0, max(0.4, d / 12.0)))
            r.step(1)
        r.set_pad(0)


def a15b_close_page(r: Route, limit: int = 6000) -> None:
    """A status page the game opened (ui byte 1 = 3): 60 frames, then
    Triangle until it closes."""
    r.until(lambda row: row["ui"][2:4] == "03", limit)
    r.idle(60)
    for _ in range(6):
        r.press("TRIANGLE", 2)
        r.step(25)
        if r.rows[-1]["ui"][2:4] != "03":
            return
    raise TimeoutError("page not closed: " + summary(r.rows[-1]))


def a15b_status_open(r: Route) -> None:
    """START until the status hub is open (D_00810131 = D_00810132 = 1)."""
    for _ in range(6):
        r.press("START", 2)
        for _ in range(40):
            r.step(1)
            if r.rows[-1]["t130"][2:6] == "0101":
                r.step(20)
                return
        r.idle(10)
    raise TimeoutError("status hub not opened: " + summary(r.rows[-1]))


def a15b_status_pick(r: Route, lx: int, ly: int) -> None:
    """The hub and the ITEM root select by the stick (the hover byte
    D_00810141 follows it while held): stick, Cross with it held."""
    r.set_pad(0, lx, ly)
    r.step(10)
    r.set_pad(PAD["CROSS"], lx, ly)
    r.step(2)
    r.set_pad(0)
    r.step(50)


def a15b_beat_event(r: Route) -> dict:
    # AREA15 sub 1: from entry 0 up the stair 794 (z 898 -> 845) to the y-339.6
    # platform, west down the stair 792 to the mezzanine (y 315.1) and west
    # into the polygon 0x827C80 (x < 793): [4]'s script 0x827400 (flag 0x23 =
    # 1), then D_008107FB = 1 and the 900-frame count; no input until the
    # count ends (D_008107FB = 2) and script 0x8277C0 starts.
    use_a15b_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, [(898, 912), (897.5, 900), (897.5, 880), (897.5, 860), (897.5, 846), (885, 836), (874, 836),
                  (860, 836), (842, 836), (820, 836), (800, 838), (785, 845)],
              stop=lambda row: row["s778"][6:8] != "00")
    if r.rows[-1]["s778"][6:8] == "00":
        raise RuntimeError("[4]'s event not started: " + summary(r.rows[-1]))
    r.until(lambda row: row["s7f8"][6:8] == "02" and row["spad"][2:4] == "02", 6000)
    r.idle(5)
    return {"what": "AREA15 sub 1: the mezzanine, [4]'s script 0x827400 (flag 0x23 = 1), the 900-frame count "
                    "(D_008107FB 1 -> 2), script 0x8277C0 starts", "hp_end": a01_hp(r)}


def a15b_beat_return(r: Route) -> dict:
    # Script 0x8277C0 to its end (counter 0x23 = 3, flag 0x23 = 0xFF), items
    # C64 0x06 and CC3 0x0E (the U.R.S. page opens: Triangle), 60 frames, the
    # sub-states of AREA00 / 01 / 02 / 04 / 06 and the request AREA15 entry 0
    # sub 0; ends when [1]'s script 0x827D70 has started there.
    use_a15b_sampler(r)
    next_long_frames(r)
    a15b_close_page(r, 8000)
    r.until(lambda row: row["area4"] == "0f00000f", 3000)
    r.until(lambda row: row["spad"][2:4] == "02" and row["area4"] == "0f00000f", 3000)
    r.idle(30)
    return {"what": "script 0x8277C0, items C64 0x06 / CC3 0x0E (the U.R.S. page), the forced return to AREA15 "
                    "sub 0 entry 0, [1]'s script starts", "hp_end": a01_hp(r)}


def a15b_beat_scripts(r: Route) -> dict:
    # AREA15 sub 0: [1]'s script 0x827D70 (flag 0x24 = 0xFF, counter 0x24 =
    # 1, item 0x25; the EVENT page opens: Triangle), [2]'s script 0x8281F0
    # (counter 0x24 = 2, CC3 0x0F), control.
    use_a15b_sampler(r)
    next_long_frames(r)
    a15b_close_page(r, 14000)
    r.until(lambda row: row["s7fc"][0:2] == "02", 8000)
    a19c_until_control(r, 3000)
    return {"what": "AREA15 sub 0: [1]'s script (item 0x25, counter 0x24 = 1), [2]'s script (counter 0x24 = 2)",
            "hp_end": a01_hp(r)}


def a15b_beat_bed(r: Route) -> dict:
    # From where [2]'s script left the player (879.5, 240, 959) to entry 2
    # (916.3, 928.4), the slider [13] (001BB860, room move id 1) facing 2.48:
    # entry 1 (931.1, 909.4); east round the desk group to the bed [16]
    # (00159620 model 0x36, (972, 240, 844.5)); its Use opens the HEALING
    # page's prompt for item 0x20 (kind 2, the device hand-off 0015C750):
    # Yes: health 100, infection 0.
    use_a15b_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, [(882, 945), (895, 934), (910, 930)])
    a19d_go_rock(r, 916.3, 928.4, tol=0.5)
    a13_settle(r, 3)
    a13_face(r, 2.48, tol=0.1)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00" or row["area4"][4:6] == "01")
    a19c_until_control(r, 2000, pred=lambda row: row["area4"][4:6] == "01")
    a15b_walk(r, [(965, 905), (976, 880), (976, 860), (966, 846)], tol=0.8)
    a13_settle(r, 3)
    a13_face(r, math.pi / 2, tol=0.08)
    inf0 = r.rows[-1]["inf"]
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][10:12] == "04", 1500)
    r.idle(30)
    r.press("LEFT", 2, after=20)
    r.press("CROSS", 2)
    r.until(lambda row: row["hp"] >= 99.9 and row["inf"] == 0.0, 1500)
    r.until(in_control, 1500)
    a13_settle(r, 10)
    return {"what": "the slider [13] to entry 1, the bed [16] with item 0x20: health 100, infection 0",
            "inf_before": inf0, "inf_after": r.rows[-1]["inf"], "hp_end": a01_hp(r)}


def a15b_beat_door14(r: Route) -> dict:
    # Back west to the slider [13] facing -0.66 (as a15_00): entry 2; door
    # [14] (001BC350, id 0x80) facing +z: AREA19 sub 1 entry 4 (the tower's
    # y-450 landing).
    use_a15b_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, [(976, 880), (965, 905), (940, 911), (935, 911)])
    a19d_go_rock(r, 927.3, 913.7, tol=0.5)
    a13_settle(r, 3)
    a13_face(r, -0.66)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00" or row["area4"][4:6] == "02")
    a19c_until_control(r, 2000, pred=lambda row: row["area4"][4:6] == "02")
    a19d_go_rock(r, 905, 932)
    a19d_go_rock(r, 902, 935, tol=0.6)
    a13_settle(r, 3)
    a13_face(r, 0.0)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00")
    r.until(lambda row: row["area4"][:2] == "13", 4000)
    a19c_until_control(r, 4000)
    return {"what": "the slider [13] to entry 2, door [14]: AREA19 sub 1 entry 4 (the tower's y-450 landing)",
            "hp_end": a01_hp(r)}


A15B_TOWER_DOWN = [(905, 955), (905, 975), (912, 975), (930, 975), (960, 975), (977, 975), (977, 952), (964, 952),
                   (940, 952), (918, 952), (906, 955), (906, 975), (920, 975), (940, 975), (965, 975), (980, 975),
                   (982, 958)]


def a15b_beat_hall(r: Route) -> dict:
    # The tower's flights down to y 370; door [49] (open since a19d_18) from
    # the tower side facing -z: room move to entry 3, the hall.
    use_a15b_sampler(r, A19E_OWNERS)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, A15B_TOWER_DOWN + [(965, 949)])
    a19d_go_rock(r, 975, 946, tol=0.6)
    a13_settle(r, 3)
    a13_face(r, math.pi)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00")
    a19c_until_control(r, 3000)
    return {"what": "the tower's flights down, door [49]: the hall (entry 3)", "hp_end": a01_hp(r)}


def a19e_beat_ceiling(r: Route) -> dict:
    # West along the tower wall, round the ladder's column, the ladder at
    # (892, 929.5) to the y-410 ledge, the ledge climb onto the box, the
    # ceiling hang (as a19d_01) west, south and north over the truck to (851,
    # 944).  The two 0012E3A0 creatures (+0x34 = 180) spawned at this load
    # (sub 1 deferred records 15 / 16, 001B6660 condition 6: flag 0x23 = 0xFF) do not follow onto the
    # ladder.
    use_a15b_sampler(r, A19E_OWNERS)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, [(960, 933), (930, 933), (903, 933), (902, 921), (887, 921), (887, 929.5)])
    a13d_ladder_up(r, math.pi / 2)
    a19d_walk13(r, [(921.0, 928.0)])
    a13_face(r, math.pi / 2)
    a19_press_until(r, lambda row: row["pos"][1] > 420 or row["m1F0"] not in (0, 1))
    r.until(in_control, 600)
    a13_settle(r, 5)
    a19d_walk13(r, [(935.0, 928.0)])
    a13_settle(r, 5)
    a19_press_until(r, lambda row: row["m1F0"] == 0x20 or row["p5"] == 0x11)
    r.until(lambda row: row["p5"] == 0x12, 200)
    for p in [(886.0, 927.0), (885.0, 908.0), (858.0, 908.0), (858.0, 935.0), (858.0, 950.0), (851.5, 948.0),
              (851.0, 944.0)]:
        a19d_trav(r, *p)
    if r.rows[-1]["p5"] != 0x12:
        raise RuntimeError("not hanging over the truck: " + summary(r.rows[-1]))
    return {"what": "the ladder at (892, 929.5), the ceiling hang to (851, 944) over the truck", "hp_end": a01_hp(r)}


def a19e_beat_truck(r: Route) -> dict:
    # Over the truck's roof (grid floor 372, y 403.2), the drop, south-west
    # across the roof and off its south-west edge onto the step 401 (y 379.9)
    # at the stair 404's top, the Use point (817.5, 379.9, 931.4) facing yaw 1.047 (the descriptor's yaw + pi); the status
    # screen: the hub's ITEM (stick left), the ITEM root's EVENT (stick up
    # and left), the cursor down to item 0x25, Cross, Yes: [46] takes it,
    # D_00810838 = 1.
    use_a15b_sampler(r, A19E_OWNERS)
    next_long_frames(r)
    for p in [(855.0, 952.0), (851.0, 957.0)]:  # over the roof (grid floor 372, y 403.2)
        a19d_trav(r, *p)
    a19d_ceiling_drop(r)
    r.until(in_control, 600)
    a13_settle(r, 5)
    if r.rows[-1]["pos"][1] < 400:
        raise RuntimeError("the drop missed the truck's roof: " + summary(r.rows[-1]))
    for p in [(840, 950), (828, 940), (822, 936)]:
        a13d_go(r, *p, tol=1.0, limit=300)
    for i in range(120):                        # off the roof's south-west edge onto the step 401
        r.stick_toward(816, 930, 0.6)
        r.step(1)
        if r.rows[-1]["pos"][1] < 395 and in_control(r.rows[-1]) and i > 20:
            break
    r.set_pad(0)
    a13_settle(r, 5)
    a19d_go_rock(r, 817.5, 931.4, tol=0.5)
    a13_settle(r, 3)
    a13_face(r, 1.0472, tol=0.08)
    a15b_status_open(r)
    a15b_status_pick(r, 0x00, 0x7F)            # hub hover 4: ITEM
    a15b_status_pick(r, 0x00, 0x00)            # ITEM hover 4: EVENT
    for _ in range(4):                         # the list cursor (D_00810147) to the second entry
        if r.rows[-1]["t130"][0x17 * 2:0x17 * 2 + 2] == "01":
            break
        r.press("DOWN", 2)
        r.step(20)
    r.press("CROSS", 2)
    r.until(lambda row: row["ui"][10:12] == "04", 200)
    r.step(30)
    r.press("LEFT", 2)
    r.step(20)
    r.press("CROSS", 2)
    r.until(lambda row: row["s838"][:2] == "01", 600)
    a19c_until_control(r, 1500)
    return {"what": "the truck [46]: item 0x25 from the EVENT page at its Use point, D_00810838 = 1",
            "hp_end": a01_hp(r)}


def a19e_take(r: Route, x: float, z: float, stand: tuple) -> None:
    a19d_go_rock(r, *stand, tol=1.0)
    a13_face(r, math.atan2(x - r.rows[-1]["pos"][0], z - r.rows[-1]["pos"][2]), tol=0.1)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00" or row["ui"][2:4] == "03", tries=4, wait=30)
    a15b_close_page(r, 600)
    r.until(in_control, 600)


def a19e_beat_battery(r: Route) -> dict:
    # Inside the truck: g[0] (0015AFA0 via 00219550, item 0x1C: the capacity
    # D_00810CB7 and the charge 0x24 half-units) and g[7] (item 0x1F); below
    # health 70 item 0x1F is used from the HEALING page (kind 1: health 100).
    use_a15b_sampler(r, A19E_OWNERS)
    next_long_frames(r)
    a19e_take(r, 831.6, 946.3, (826.0, 940.0))
    a19e_take(r, 827.7, 950.9, (826.0, 946.0))
    if r.rows[-1]["hp"] >= 70.0:               # the HEALING page refuses at full health; keep item 0x1F
        a13_settle(r, 5)
        return {"what": "the truck's pickups g[0] (item 0x1C) and g[7] (item 0x1F)", "hp_end": a01_hp(r)}
    a15b_status_open(r)
    a15b_status_pick(r, 0x00, 0x7F)            # ITEM
    a15b_status_pick(r, 0x00, 0x7F)            # ITEM hover 5: HEALING
    r.press("CROSS", 2)
    r.until(lambda row: row["ui"][10:12] == "04", 200)
    r.step(30)
    r.press("LEFT", 2)
    r.step(20)
    r.press("CROSS", 2)
    r.until(lambda row: row["hp"] >= 99.9, 900)
    for _ in range(8):                         # the HEALING page, the ITEM root and the hub stay open
        if r.rows[-1]["ui"][2:4] == "00" and r.rows[-1]["t130"][2:4] == "00":
            break
        r.press("TRIANGLE", 2)
        r.step(30)
    r.until(in_control, 900)
    a13_settle(r, 5)
    return {"what": "the truck's pickups g[0] (item 0x1C) and g[7] (item 0x1F), item 0x1F used (health 100)",
            "hp_end": a01_hp(r)}


def a19e_beat_tower(r: Route) -> dict:
    # Out of the truck by the step 401 and the stair 404, east between the
    # truck's south-east face and the drums, south of the ladder's column, to
    # door [49] facing +z: room move to entry 2 (the tower).  The creatures
    # close in on this run (bites).
    use_a15b_sampler(r, A19E_OWNERS)
    next_long_frames(r)
    a15b_walk(r, [(822, 937), (817.5, 931.4), (812, 926), (809, 922), (828, 922), (845, 932), (858, 930), (875, 930),
                  (885, 920), (900, 918), (935, 920), (965, 928)], tol=1.5)
    a19d_go_rock(r, 976, 933, tol=0.6)
    a13_settle(r, 3)
    a13_face(r, 0.0, tol=0.1)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00", tries=4)
    a19c_until_control(r, 3000)
    if r.rows[-1]["pos"][2] < 945:
        raise RuntimeError("not in the tower: " + summary(r.rows[-1]))
    return {"what": "out of the truck, across the hall to door [49]: the tower (entry 2)", "hp_end": a01_hp(r)}


def a19e_beat_down(r: Route) -> dict:
    # Entry 2 to the pocket, the ladder 694 facing +z and down: 13 00 0B, sub
    # 0's y-265 deck.
    use_a15b_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, [(965, 949), (930, 948), (907, 947)])
    a13d_go(r, 907.0, 953.0, tol=0.4, limit=150)
    a13_settle(r, 3)
    a13_face(r, 0.0)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x16, 0x17, 0x18))
    r.set_pad(0, 0x7F, 0xFF)
    r.until(lambda row: row["area4"][:4] == "1300" and in_control(row) and row["pos"][1] < 270, 4000, 0, 0x7F, 0xFF)
    r.set_pad(0)
    a13_settle(r, 10)
    return {"what": "the ladder 694 down: 13 00 0B, sub 0's y-265 deck", "hp_end": a01_hp(r)}


def a19e_beat_panel24(r: Route) -> dict:
    # The deck to the panel [24] facing +z; Use, the battery prompt, Yes: the
    # charge drains 2 half-units every 30 frames from 0x24 to 4 (cost 2 *
    # 0x10), D_00810854 |= 2 (door [25]'s lock bit).
    use_a15b_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, [(920, 967), (950, 967), (990, 967), (1017, 967), (1017, 1000), (1017, 1050), (1017, 1090),
                  (1013, 1110), (1013, 1119)])
    a13_settle(r, 3)
    a13_face(r, 0.0)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][10:12] == "04", 1500)
    r.idle(30)
    r.press("LEFT", 2, after=20)
    r.press("CROSS", 2)
    r.until(lambda row: int(row["l854"], 16) & 2, 3000)
    r.until(in_control, 1500)
    a13_settle(r, 10)
    return {"what": "the panel [24] powered: charge 0x24 -> 4, D_00810854 bit 1", "hp_end": a01_hp(r)}


def a19e_beat_door25(r: Route) -> dict:
    # Door [25] (001BB860 model 0x16, id 0x81) facing +z: AREA03 entry 1 sub
    # D_00810730[3]; the arrival, control.
    use_a15b_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a19d_go_rock(r, 1022.5, 1121.0, tol=0.5)
    a13_settle(r, 3)
    a13_face(r, 0.0, tol=0.06)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00", tries=4)
    r.until(lambda row: row["area4"][:2] == "03", 4000)
    a19c_until_control(r, 20000)
    return {"what": "door [25]: AREA03 entry 1", "hp_end": a01_hp(r)}


def a19e_beat_down694(r: Route) -> dict:
    # Side (from a15b_04): the flights down, the pocket, the ladder 694
    # facing +z and down: 00193EB0's band x >= 872, z > 900, y <= 356: 13 00
    # 0B, sub 0 entry 0xB, the ladder's foot on the y-265 deck.
    use_a15b_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, A15B_TOWER_DOWN + [(965, 949), (930, 948), (907, 947)])
    a13d_go(r, 907.0, 953.0, tol=0.4, limit=150)
    a13_settle(r, 3)
    a13_face(r, 0.0)
    a19_press_until(r, lambda row: row["m1F0"] in (0x15, 0x16, 0x17, 0x18))
    r.set_pad(0, 0x7F, 0xFF)
    r.until(lambda row: row["area4"][:4] == "1300" and in_control(row) and row["pos"][1] < 270, 4000, 0, 0x7F, 0xFF)
    r.set_pad(0)
    a13_settle(r, 10)
    return {"what": "the ladder 694 down: 13 00 0B, sub 0's y-265 deck", "hp_end": a01_hp(r)}


def a19e_beat_panel(r: Route) -> dict:
    # Side: the deck east and north to the panel [24] (00158EC0 model 0x22,
    # cost 2 * 0x10 half-units) facing +z; Use, the battery prompt, Yes: the
    # charge (4 half-units) is short, the page refuses; Triangle.
    use_a15b_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, [(920, 967), (950, 967), (990, 967), (1017, 967), (1017, 1000), (1017, 1050), (1017, 1090),
                  (1013, 1110), (1013, 1119)])
    a13_settle(r, 3)
    a13_face(r, 0.0)
    use_press(r, lambda row: not in_control(row))
    r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][10:12] == "04", 1500)
    r.idle(30)
    r.press("LEFT", 2, after=20)
    r.press("CROSS", 2)
    r.until(lambda row: row["ui"][10:12] == "01", 900)
    r.idle(30)
    a15b_close_page(r, 300)
    r.until(in_control, 900)
    a13_settle(r, 10)
    return {"what": "the panel [24]: the battery prompt, Yes, refused (charge 4 < cost 32 half-units)",
            "hp_end": a01_hp(r)}


OUT_A03 = ROOT / "build/s87/route_a03"


def a03_beat_elevator(r: Route) -> dict:
    # AREA03 sub 0 from entry 1 (the y-80 tunnel) north-west to the lift:
    # [11] (0x826340, C; the descriptor 0x828FF0: (545.3, 80, 407.9), radius
    # 5, yaw -2.356) facing 0.785: script 0x828CD0, D_00275CA0 = 1 (the car's
    # doors [7] / [8] open); through the doorway into the car, [9] (0x825980,
    # C; descriptor 0x828BB0: (552.5, 79.5, 416.8), yaw 0.785) facing -2.356:
    # script 0x8282D0, the ride down: room move to entry 2 (554, -25, 417.5).
    use_a15b_sampler(r)
    next_long_frames(r)
    a19d_rock(r, 200)
    a13_settle(r, 3)
    a15b_walk(r, [(590, 352), (575, 375), (562, 395), (553, 404)])
    a19d_go_rock(r, 545.3, 407.9, tol=0.6)
    a13_settle(r, 3)
    a13_face(r, 0.785, tol=0.1)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00", tries=4)
    a19c_until_control(r, 1500)
    a15b_walk(r, [(541.5, 413), (545, 420), (551, 423)], tol=0.8)
    a19d_go_rock(r, 552.5, 417.5, tol=0.6)
    a13_settle(r, 3)
    a13_face(r, -2.356, tol=0.1)
    a19_press_until(r, lambda row: row["spad"][2:4] != "00", tries=4)
    r.until(lambda row: row["area4"] == "03000203", 1500)
    a19c_until_control(r, 1500)
    return {"what": "AREA03 sub 0: the lift's doors ([11]) and the ride down ([9]): entry 2", "hp_end": a01_hp(r)}


A03_BEATS: list[tuple] = [
    ("a03_00_elevator", "a19e_07_door25", a03_beat_elevator),
]
A03_SIDE_BEATS: set[str] = set()
A03_CHANGE_BEATS: set[str] = set()


A15B_BEATS: list[tuple] = [
    ("a15b_00_event", "a15_01_door51", a15b_beat_event),
    ("a15b_01_return", "a15b_00_event", a15b_beat_return),
    ("a15b_02_scripts", "a15b_01_return", a15b_beat_scripts),
    ("a15b_03_bed", "a15b_02_scripts", a15b_beat_bed),
    ("a15b_04_door14", "a15b_03_bed", a15b_beat_door14),
]
A15B_SIDE_BEATS: set[str] = set()
A15B_CHANGE_BEATS: set[str] = {"a15b_01_return", "a15b_04_door14"}
A19E_BEATS: list[tuple] = [
    ("a19e_00_hall", "a15b_04_door14", a15b_beat_hall),
    ("a19e_01_ceiling", "a19e_00_hall", a19e_beat_ceiling),
    ("a19e_02_truck", "a19e_01_ceiling", a19e_beat_truck),
    ("a19e_03_battery", "a19e_02_truck", a19e_beat_battery),
    ("a19e_04_tower", "a19e_03_battery", a19e_beat_tower),
    ("a19e_05_down", "a19e_04_tower", a19e_beat_down),
    ("a19e_06_panel24", "a19e_05_down", a19e_beat_panel24),
    ("a19e_07_door25", "a19e_06_panel24", a19e_beat_door25),
    ("a19e_s0_down694", "a15b_04_door14", a19e_beat_down694),
    ("a19e_s1_panel", "a19e_s0_down694", a19e_beat_panel),
]
A19E_SIDE_BEATS: set[str] = {"a19e_s0_down694", "a19e_s1_panel"}
A19E_CHANGE_BEATS: set[str] = {"a19e_05_down", "a19e_07_door25", "a19e_s0_down694"}


def fifteenth_selected(spec: str) -> list[tuple]:
    """The fifteenth-level groups `a15b`, `a19e` and `a03` (opt-in; the a19e
    side beats run only by name)."""
    wanted = spec.split(",")
    out = []
    for tag, beats, side in (("a15b", A15B_BEATS, A15B_SIDE_BEATS), ("a19e", A19E_BEATS, A19E_SIDE_BEATS),
                             ("a03", A03_BEATS, A03_SIDE_BEATS)):
        if tag in wanted:
            out += [b for b in beats if b[0] not in side]
        else:
            out += [b for b in beats if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]
    return out


def fifteenth_owners(name: str):
    return A19E_OWNERS if name[:7] in ("a19e_00", "a19e_01", "a19e_02", "a19e_03", "a19e_04") else None

# ---------------------------------------------------------------------------
# AIM capture group (opt-in, `--beats aim` or a beat's name): aiming, firing,
# reloading, the gun light, melee and the security gun's cable in AREA11
# (decomp docs/CAPTURES_C10.md, section AIM; the port's aim/fire lane's
# capture beat plan).  Every beat starts from the 08_truck_crossing snapshot
# (armed and in control: magazine D_00810C62 = 30, reserve D_00810CB4 = 60,
# fire mode D_00810C61 = 0, equipment bytes D_00810CA4..CA7 = FF 05 00 07,
# the seven 0018A6B0 equipment nodes with the gun at player +0x20 and the
# knife at player +0x18) and writes build/aimfire/capture/<beat>/.  Pad input
# only; no memory is written.  Pad words: D_00810E70 held / D_00810E74
# pressed, low byte Square 0x80, Cross 0x40, Circle 0x20, Triangle 0x10,
# R1 0x08, L1 0x04, R2 0x02, L2 0x01; high byte Left 0x8000, Down 0x4000,
# Right 0x2000, Up 0x1000, Start 0x800, R3 0x400, L3 0x200, Select 0x100.
OUT_AIM = ROOT / "build/aimfire/capture"
AIM_GUN = 0x7AB730       # player +0x20: 0018A6B0 node, flavour 0 (the gun)
AIM_KNIFE = 0x7AB440     # player +0x18: 0018A6B0 node, flavour 4
AIM_SECGUN = 0x7A6AD0    # AREA11 overlay owner 0x825940, the security gun
AIM_CABLE = 0x7A6DC0     # AREA11 overlay owner 0x827490, its power cable
AIM_POOL = 0x7A5640      # D_007A5640: 0x100 pool records of 0x2F0 bytes
AIM_CABLE_TOP = (387.0, 231.8, 290.3)     # the cable record's +0xB0 (its bone 0)
AIM_SPANS = [(n, a, 0x320 if n == "player" else z) for n, a, z in SPANS] + [
    ("w_c60", 0x810C60, 0x8),       # C61 fire mode, C62 magazine, C63
    ("w_ca4", 0x810CA4, 0x14),      # CA4..CA7 equipment bytes, CA8..CB2 counters, CB4 reserve
    ("w_c70", 0x810C70, 0x4),       # C73 (the selector's third entry)
    ("w_d3c", 0x810D3C, 0x4),       # D_00810D3C gun-light preference
    ("w_6c0", 0x8106C0, 0x30),      # 6C6, 6C7 lamp, 6CC, 6E0/E4/E8 targets
    ("pad", 0x810E70, 0x8),         # processed held / pressed words
    ("uirec", 0x810130, 0x60),      # status page record (t[1] page, t[4] SPR4 state, t[5..6], t[0x50..])
    ("lists", 0x275B54, 0x68),      # per-class list block (B8C/B94 = the bullet victim list)
    ("masks", 0x70003B70, 0x10),    # 3B74..3B7E action masks
    ("sp3a20", 0x70003A20, 0x4),
    ("sp3190", 0x70003190, 0x50),   # ray query block 3190..31DF (31B0 point, 31D0 face, 31D4 owner)
    ("sp3600", 0x70003600, 0x10),
    ("sp36a0", 0x700036A0, 0x50),
    ("sp38a0", 0x700038A0, 0x30),
    ("gun:a", AIM_GUN, 0x40), ("gun:b", AIM_GUN + 0xA0, 0x30), ("gun:c", AIM_GUN + 0x1F0, 0x30),
    ("knife:a", AIM_KNIFE, 0x40), ("knife:b", AIM_KNIFE + 0xA0, 0x10),
    ("secgun:a", AIM_SECGUN, 0x40), ("secgun:c", AIM_SECGUN + 0x1F0, 0x40),
    ("cable:a", AIM_CABLE, 0x40), ("cable:c", AIM_CABLE + 0x98, 0x8),
    ("taken", 0x810860 + (0xB << 5), 0x20),     # area 11's taken-bit row (cable bit 0x50)
] + [(f"pool{i:02x}", AIM_POOL + i * 0x2F0, 0x18) for i in range(0x100)]


class AimSampler(Sampler):
    """One Pine request for AIM_SPANS; then one more for the records whose
    header differs from the beat's first frame (spawned effects, shot nodes,
    impact markers): their +0x18..+0x3F and +0xA0..+0xDF."""

    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = AIM_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))
        self.pool0: dict[str, str] | None = None

    def raw(self) -> dict[str, bytes]:
        for _ in range(5):
            data = self.s.pine.request(self.body)
            out, off = {}, 0
            for name, _a, n in self.spans:
                out[name] = data[off:off + n]
                off += n
            if out["counter"] == out["counter2"]:
                break
        else:
            raise RuntimeError("inconsistent sample")
        pool = {f"{i:02x}": out[f"pool{i:02x}"].hex() for i in range(0x100)}
        if self.pool0 is None:
            self.pool0 = pool
        changed = sorted(k for k, v in pool.items() if v != self.pool0[k])
        deep = [k for k in changed if int(pool[k], 16)]
        if deep:
            body = b"".join(struct.pack("<BI", 2, AIM_POOL + int(k, 16) * 0x2F0 + o + i)
                            for k in deep for o, n in ((0x18, 0x28), (0xA0, 0x40)) for i in range(0, n, 4))
            data = self.s.pine.request(body)
            out["deep"] = {k: data[j * 0x68:(j + 1) * 0x68].hex() for j, k in enumerate(deep)}
        else:
            out["deep"] = {}
        out["pool_delta"] = {k: pool[k] for k in changed}
        return out


def _s16(b: bytes, o: int) -> int:
    return struct.unpack_from("<h", b, o)[0]


def decode_aim(r: dict[str, bytes]) -> dict:
    row = decode(r)
    p = r["player"]
    row["pl"] = p.hex()                         # the whole player record +0..+0x31F
    row["w"] = {
        "p1": p[1], "p4": p[4], "p5": p[5], "p6": p[6], "p7": p[7],
        "a274": p[0x274], "a275": p[0x275], "t276": _s16(p, 0x276), "b28": _s16(p, 0x28),
        "q2A": _s16(p, 0x2A), "e2E": _s16(p, 0x2E), "c2F0": p[0x2F0], "l2F2": p[0x2F2],
        "i2F4": round(f32(p, 0x2F4), 4), "b302": p[0x302], "r317": p[0x317], "m318": p[0x318],
        "b236": p[0x236], "act200": hex(struct.unpack_from("<I", p, 0x200)[0]),
        "pitch278": round(f32(p, 0x278), 5), "yaw27C": round(f32(p, 0x27C), 5),
        "d26C": round(f32(p, 0x26C), 6), "d270": round(f32(p, 0x270), 6), "goal218": round(f32(p, 0x218), 5),
        "aim2D0": vec(p, 0x2D0), "m2A0": p[0x2A0:0x2E0].hex(),
    }
    c60, ca4, lists = r["w_c60"], r["w_ca4"], r["lists"]
    row["g"] = {
        "fire_mode": c60[1], "mag": c60[2], "c63": c60[3], "equip": ca4[:4].hex(),
        "counters": ca4[4:0x10].hex(), "reserve": _s16(ca4, 0x10), "c73": r["w_c70"][3],
        "light_d3c": r["w_d3c"][0], "b6C6": r["w_6c0"][6], "lamp6C7": r["w_6c0"][7],
        "chg6CC": r["w_6c0"][0xC],
        "targets": [hex(x) for x in struct.unpack_from("<3I", r["w_6c0"], 0x20)],
        "held": hex(struct.unpack_from("<H", r["pad"], 0)[0]),
        "pressed": hex(struct.unpack_from("<H", r["pad"], 4)[0]),
        "masks": r["masks"].hex(),
        "list_counts": {hex(0x275B54 + o): struct.unpack_from("<I", lists, o + 8)[0]
                        for o in (0x08, 0x18, 0x28, 0x38, 0x48, 0x58)},
    }
    row["ui_rec"] = r["uirec"].hex()
    row["sp"] = {k: r[k].hex() for k in ("sp3a20", "sp3190", "sp3600", "sp36a0", "sp38a0")}
    row["gun"] = {"h": r["gun:a"][:0x10].hex(), "ev2E": _s16(r["gun:a"], 0x2E), "a": r["gun:a"].hex(),
                  "A0": vec(r["gun:b"], 0), "B0": vec(r["gun:b"], 0x10), "C0": vec(r["gun:b"], 0x20),
                  "s1F0": r["gun:c"].hex()}
    row["knife"] = {"h": r["knife:a"][:0x10].hex(), "a": r["knife:a"].hex(), "A0": vec(r["knife:b"], 0)}
    row["secgun"] = {"h": r["secgun:a"][:0x10].hex(), "a": r["secgun:a"].hex(), "s": r["secgun:c"].hex()}
    row["cable"] = {"h": r["cable:a"][:0x10].hex(), "a": r["cable:a"].hex(),
                    "hit36": _s16(r["cable:a"], 0x36), "c98": r["cable:c"].hex()}
    row["taken"] = r["taken"].hex()
    row["pool_delta"] = r["pool_delta"]         # pool headers +0..+0x17 that differ from row 0
    row["pool_deep"] = r["deep"]                # those records' +0x18..+0x3F, +0xA0..+0xDF
    return row


def use_aim_sampler(r: Route) -> None:
    sampler = AimSampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_aim(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)
    r.rows[0]["pool0"] = {k: v for k, v in sampler.pool0.items() if int(v, 16)}


def _ui(row: dict, i: int) -> int:
    return int(row["ui_rec"][2 * i:2 * i + 2], 16)


def aim_idle_ready(row: dict) -> bool:
    return in_control(row) and row["w"]["p5"] == 0


def aim_ready(row: dict) -> bool:
    """Stance loop (+6 == 2) with the fire sub-machine idle (+7 == 0)."""
    return row["w"]["p6"] == 2 and row["w"]["p7"] == 0 and row["w"]["p5"] in (0x1D, 0x1E, 0x1F, 0x20)


def aim_draw(r: Route, hold: int) -> None:
    """Hold the aim button(s) until the stance top reaches its loop (+6 == 2)."""
    r.until(lambda row: row["w"]["p5"] in (0x1D, 0x1E, 0x1F, 0x20) and row["w"]["p6"] == 2, 180, hold)


def aim_holster(r: Route, frames: int = 30) -> None:
    """Release everything: the stance ramps out (+6 0x63, 0x64), holsters (0x65,
    clip 0x111), waits (0x66) and hands back (0x6E) to idle."""
    r.set_pad(0)
    r.until(aim_idle_ready, 400)
    r.idle(frames)


def aim_tap(r: Route, hold: int, button: str, frames: int = 2) -> None:
    r.set_pad(hold | PAD[button])
    r.step(frames)
    r.set_pad(hold)


def aim_shot(r: Route, hold: int, limit: int = 12) -> dict:
    """Wait for the stance loop, tap Circle (the trigger mask 3B78) once and wait
    up to `limit` frames for the round to leave (the magazine drops); a dry
    trigger (no rounds) leaves the magazine as it is."""
    r.until(aim_ready, 400, hold)
    mag = r.rows[-1]["g"]["mag"]
    aim_tap(r, hold, "CIRCLE")
    for _ in range(limit):
        if r.rows[-1]["g"]["mag"] != mag:
            break
        r.step(1)
    return r.rows[-1]


def aim_at(r: Route, x: float, y: float, z: float, hold: int, tol: float = 0.01, limit: int = 120) -> bool:
    """Steer the stance aim with the left stick (0017ABA0, manual steering)
    until the gun's barrel direction (gun node +0xC0) points from its muzzle
    (+0xA0) at (x, y, z); one stick frame, then three settle frames."""
    for _ in range(limit):
        row = r.rows[-1]
        ax, ay, az = row["gun"]["A0"]
        dx, dy, dz = x - ax, y - ay, z - az
        n = math.sqrt(dx * dx + dy * dy + dz * dz) or 1.0
        dx, dy, dz = dx / n, dy / n, dz / n
        cx, cy, cz = row["gun"]["C0"]
        side = (cz * dx - cx * dz) / (math.hypot(cx, cz) or 1.0)
        ly = 0xFF if cy < dy - tol else 0x00 if cy > dy + tol else 0x7F
        lx = 0x00 if side > tol else 0xFF if side < -tol else 0x7F
        if lx == 0x7F and ly == 0x7F:
            r.set_pad(hold)
            r.step(3)
            return True
        r.set_pad(hold, lx, ly)
        r.step(1)
        r.set_pad(hold)
        r.step(3)
    r.set_pad(hold)
    return False


def aim_meta(r: Route, what: str, **extra) -> dict:
    shots = [row["f"] for a, row in zip(r.rows, r.rows[1:]) if row["g"]["mag"] < a["g"]["mag"]]
    impacts = sorted({k for row in r.rows for k, v in row.get("pool_delta", {}).items()
                      if v[32:40] == "a0ab1800"})
    return dict(extra, what=what, shot_frames=shots, impact_records=impacts,
                end={"mag": r.rows[-1]["g"]["mag"], "reserve": r.rows[-1]["g"]["reserve"],
                     "fire_mode": r.rows[-1]["g"]["fire_mode"], "light": r.rows[-1]["g"]["light_d3c"],
                     "cable": r.rows[-1]["cable"]["h"], "secgun": r.rows[-1]["secgun"]["h"],
                     "taken": r.rows[-1]["taken"]})


def aim_beat_r1_hold(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(10)
    aim_draw(r, PAD["R1"])
    r.step(60)
    aim_holster(r)
    return aim_meta(r, "hold R1 (mask 3B7C): 001607D0 enters +5 0x1D / +1F0 0x31, 0016FCF0 draws "
                       "(clip 0x110) and loops (+6 2, clip 0x112); release ramps out and holsters "
                       "(+6 0x63, 0x64, 0x65 clip 0x111, 0x66, 0x6E) back to idle")


def aim_beat_r2_hold(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(10)
    aim_draw(r, PAD["R2"])
    r.step(60)
    aim_holster(r)
    return aim_meta(r, "hold R2 (mask 3B7E): +5 0x1E / +1F0 0x32 (001703E0), loop, release and holster")


def aim_beat_both(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(10)
    aim_draw(r, PAD["R1"])
    r.step(20)
    r.set_pad(PAD["R1"] | PAD["R2"])        # R2 added while R1 is held
    r.step(40)
    r.set_pad(PAD["R1"])                    # R2 released first
    r.step(40)
    r.set_pad(PAD["R1"] | PAD["R2"])        # R2 again
    r.step(30)
    r.set_pad(PAD["R2"])                    # then R1 released first
    r.step(40)
    aim_holster(r)
    r.set_pad(PAD["R1"] | PAD["R2"])        # both pressed on the same frame from idle
    r.until(lambda row: row["w"]["p5"] in (0x1D, 0x1E) and row["w"]["p6"] == 2, 180, PAD["R1"] | PAD["R2"])
    r.step(20)
    aim_holster(r)
    return aim_meta(r, "R1 and R2 together: R2 added to R1, R2 released first, R2 again, R1 released "
                       "first, then both pressed on one frame from idle (precedence and release order)")


def aim_beat_single_fire(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(10)
    hold = PAD["R1"]
    aim_draw(r, hold)
    r.step(10)
    for _ in range(3):                      # three single presses
        aim_shot(r, hold)
    r.until(aim_ready, 400, hold)
    r.set_pad(hold | PAD["CIRCLE"])         # trigger held 90 frames (fire mode single)
    r.step(90)
    r.set_pad(hold)
    r.step(30)
    aim_holster(r, 10)
    hold = PAD["R2"]                        # the same from the R2 stance
    aim_draw(r, hold)
    r.step(10)
    for _ in range(2):
        aim_shot(r, hold)
    r.until(aim_ready, 400, hold)
    r.set_pad(hold | PAD["CIRCLE"])
    r.step(60)
    r.set_pad(hold)
    r.step(30)
    aim_holster(r)
    return aim_meta(r, "fire mode single (D_00810C61 = 0): three presses and a 90-frame hold of Circle "
                       "(trigger mask 3B78) in the R1 stance, then two presses and a 60-frame hold in "
                       "the R2 stance; 00170A60 states 0x0A/0x0B, magazine and reserve drop together")


def aim_beat_world_hit(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(5)
    r.goto(381.0, 330.0, tol=1.0)
    r.goto(381.0, 312.0, tol=0.8, magnitude=0.5, stuck_ok=True)
    settle(r, 10)
    hold = PAD["R1"]
    aim_draw(r, hold)
    r.step(10)
    targets = [("ground", (381.0, 184.9, 300.0)), ("pillar", (387.0, 205.0, 290.3)),
               ("fence", (372.0, 200.0, 290.3)), ("high", (381.0, 260.0, 280.0))]
    aimed = {}
    for name, (x, y, z) in targets:
        aimed[name] = aim_at(r, x, y, z, hold)
        aim_shot(r, hold)
        r.step(12)
    r.step(30)
    aim_holster(r)
    return aim_meta(r, "R1 stance at (381, 312) facing the fence; manual aim (left stick) at the ground, "
                       "the concrete pillar, the fence and above the fence: each shot's impact marker "
                       "(class 1, 0018ABA0, +0x2E impact code, +0xB0 hit point, +0xC0 normal)",
                    aimed=aimed, targets=dict(targets))


def aim_press_until(r: Route, button: str, pred, wait: int = 30, tries: int = 6) -> None:
    """Press `button` (2 frames) until `pred` holds, waiting `wait` frames after
    each press: a menu can ignore a press that lands while it is busy."""
    for _ in range(tries):
        r.press(button, 2)
        try:
            r.until(pred, wait)
            return
        except TimeoutError:
            pass
    raise TimeoutError(f"{button}: condition not reached after {tries} presses; last {summary(r.rows[-1])}")


def aim_status_selector(r: Route, entry: int) -> None:
    """START opens the status hub; stick right + Cross enters SPR4; stick right +
    Cross enters SELECTOR (00217FA0); Down moves the cursor (t[0x17]) to
    `entry`, Cross asks, Left picks Yes, Cross commits D_00810C61; Triangle
    closes the status screen.  Every press is repeated until its effect shows."""
    settle(r, 10)
    aim_press_until(r, "START", lambda row: _ui(row, 1) == 1 and _ui(row, 2) == 1)
    r.idle(10)
    for page_ok in (lambda row: _ui(row, 1) == 3 and _ui(row, 2) == 1 and _ui(row, 3) == 1,
                    lambda row: _ui(row, 4) == 8 and _ui(row, 5) == 1):
        for _ in range(6):
            r.set_pad(0, 0xFF, 0x7F)
            r.step(15)
            r.set_pad(PAD["CROSS"], 0xFF, 0x7F)
            r.step(2)
            r.set_pad(0, 0xFF, 0x7F)
            r.step(4)
            r.set_pad(0)
            try:
                r.until(page_ok, 240)
                break
            except TimeoutError:
                pass
        else:
            raise TimeoutError("status page did not open: " + summary(r.rows[-1]))
        r.idle(10)
    for i in range(entry):
        aim_press_until(r, "DOWN", lambda row, i=i: _ui(row, 0x17) == i + 1 and _ui(row, 5) == 1)
        r.idle(10)
    aim_press_until(r, "CROSS", lambda row: _ui(row, 5) == 4)
    r.idle(10)
    aim_press_until(r, "LEFT", lambda row: _ui(row, 6) == 0)
    r.idle(10)
    aim_press_until(r, "CROSS", lambda row: row["g"]["fire_mode"] == entry, wait=60)
    r.until(lambda row: _ui(row, 1) == 3 and _ui(row, 2) == 1 and _ui(row, 3) == 1, 120)   # back on SPR4
    r.idle(20)
    aim_press_until(r, "TRIANGLE", in_control, wait=120)
    r.idle(20)


def aim_beat_burst_fire(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(5)
    aim_status_selector(r, 1)
    hold = PAD["R1"]
    aim_draw(r, hold)
    r.step(10)
    aim_shot(r, hold)                       # a 2-frame press
    r.step(20)
    for _ in range(2):                      # the trigger held 60 frames, twice
        r.until(aim_ready, 400, hold)
        r.set_pad(hold | PAD["CIRCLE"])
        r.step(60)
        r.set_pad(hold)
        r.step(20)
    aim_holster(r)
    return aim_meta(r, "status screen: SPR4, SELECTOR (two entries: single, 3-round burst; the third, "
                       "full auto, needs D_00810C73, 0 here), burst picked and confirmed (D_00810C61 = 1); "
                       "then R1, one short press and two 60-frame holds of Circle (00170A60 states "
                       "0x14..0x17)")


def aim_beat_reload_partial(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(10)
    hold = PAD["R1"]
    aim_draw(r, hold)
    r.step(10)
    for _ in range(5):
        aim_shot(r, hold)
    r.until(aim_ready, 400, hold)
    aim_tap(r, hold, "L3")                  # L3 (pressed 0x200): 0017B300(p, 2) top-up
    r.until(lambda row: row["w"]["p6"] == 3, 30, hold)
    r.until(aim_ready, 400, hold)
    r.step(20)
    aim_tap(r, hold, "L3")                  # full magazine: no reload
    r.step(40)
    for _ in range(2):
        aim_shot(r, hold)
    r.until(aim_ready, 400, hold)
    aim_tap(r, hold, "L3")                  # reload again, R1 released during it
    r.until(lambda row: row["w"]["p6"] == 3, 30, hold)
    r.step(12)
    aim_holster(r)
    return aim_meta(r, "R1, five shots, L3 tops the magazine up from the reserve (+6 3, +1F0 0x33, "
                       "clip 0x11B; the reserve keeps the rounds); L3 with a full magazine does nothing; "
                       "two shots, L3 and R1 released 12 frames into the reload")


def aim_beat_reload_empty(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(10)
    hold = PAD["R1"]
    aim_draw(r, hold)
    r.step(10)
    for _ in range(80):
        g = r.rows[-1]["g"]
        if g["mag"] == 0 and g["reserve"] == 0:
            break
        aim_shot(r, hold)
    else:
        raise TimeoutError("magazine and reserve did not empty")
    r.until(aim_ready, 400, hold)
    for _ in range(2):                      # dry trigger: click 0x169, no shot
        aim_tap(r, hold, "CIRCLE")
        r.step(30)
    aim_tap(r, hold, "L3")                  # L3 with no rounds
    r.step(30)
    aim_holster(r)
    return aim_meta(r, "R1 and single shots until the magazine empties (automatic reload 0017B300(p, 1) "
                       "from the reserve), then until the reserve empties; two dry presses (no shot) and "
                       "L3 with no rounds",
                    reload_frames=[b["f"] for a, b in zip(r.rows, r.rows[1:])
                                   if b["w"]["p6"] == 3 and a["w"]["p6"] != 3])


def aim_beat_light_holster(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(10)
    hold = PAD["R1"]
    aim_draw(r, hold)
    r.step(10)
    aim_tap(r, hold, "SQUARE")              # 0017A970(p, 0), CA6 = 0: the gun light on
    r.step(40)
    aim_tap(r, hold, "CROSS")               # 0017AAD0: no attachment mode (CA6 = 0)
    r.step(40)
    aim_holster(r, 20)                      # holster with the light on
    aim_draw(r, PAD["R2"])                  # redraw (R2): 0016F530 relights the lamp
    r.step(30)
    aim_tap(r, PAD["R2"], "SQUARE")         # the light off
    r.step(30)
    aim_holster(r)
    return aim_meta(r, "R1, Square turns the gun light on (D_00810D3C, lamp D_008106C7), Cross does "
                       "nothing (no attachment), holster with the light on, R2 draws with the lamp "
                       "relit, Square turns it off, holster")


def aim_beat_melee(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(10)
    r.press("CIRCLE", 2)                    # 3B78 pressed from idle: +5 0x21 (001735C0)
    r.until(lambda row: row["w"]["p5"] == 0x21, 10)
    r.until(aim_idle_ready, 400)
    r.idle(20)
    r.press("CIRCLE", 2)                    # a combo: Circle again during each hit
    r.until(lambda row: row["w"]["p5"] == 0x21, 10)
    for _ in range(2):
        r.step(12)
        r.press("CIRCLE", 2)
    r.until(aim_idle_ready, 600)
    r.idle(20)
    r.press("SQUARE", 2)                    # 3B74 pressed from idle: +5 0x22 (00173E60)
    r.until(lambda row: row["w"]["p5"] == 0x22, 10)
    r.until(aim_idle_ready, 400)
    r.idle(30)
    return aim_meta(r, "melee from idle: Circle (+5 0x21, the knife node's +0x00 bit 0 raised), a "
                       "Circle chain, then Square (+5 0x22)")


def aim_beat_cable_shots(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(5)
    r.goto(387.0, 335.0, tol=1.0)
    r.goto(387.0, 318.0, tol=0.8, magnitude=0.5, stuck_ok=True)
    settle(r, 10)
    aimed = {}
    for hold in (PAD["R1"], PAD["R2"]):
        aim_draw(r, hold)
        r.step(30)                          # target acquisition / lock window
        cx, cy, cz = AIM_CABLE_TOP
        for name, y in (("top", cy - 1.0), ("joint", cy - 2.6), ("strand", 215.0)):
            aimed[f"{hold:#x}:{name}"] = aim_at(r, cx, y, cz + 0.02, hold,
                                                limit=120 if hold == PAD["R1"] else 40)
            aim_shot(r, hold)
            r.step(15)
        aim_holster(r, 20)
    r.idle(30)
    return aim_meta(r, "the security gun's power cable (0x827490, record 0x7A6DC0, top at "
                       "(387, 231.8, 290.3), hanging down the pillar's north face): R1 and R2 stances "
                       "from (387, 318) aimed (left stick) at the cable top, its bone-1 joint and the "
                       "strand, three shots each; every round ends on the pillar face (impact code "
                       "0x201) and the cable stays unhit", aimed=aimed)


def aim_beat_cable_melee(r: Route) -> dict:
    use_aim_sampler(r)
    r.idle(5)
    r.goto(387.0, 298.0, tol=0.8, magnitude=0.5, stuck_ok=True)   # at the strand's foot
    settle(r, 10)
    r.press("CIRCLE", 2)                    # light melee (+5 0x21) into the strand
    r.until(lambda row: row["w"]["p5"] == 0x21, 10)
    r.until(aim_idle_ready, 400)
    r.idle(300)                             # the cable's and the gun's reaction run out
    return aim_meta(r, "from aim_10's end: walk to the strand's foot (387, 298) and Circle (light "
                       "melee, +5 0x21): the cable takes the hit (+0x36), the gun goes to lifecycle 2, "
                       "the cable effect 0x80000045 spawns, the taken bit 0x50 is set and the cable "
                       "frees itself; 300 idle frames follow the reaction")

AIM_BEATS = [
    ("aim_00_r1_hold", "08_truck_crossing", aim_beat_r1_hold),
    ("aim_01_r2_hold", "08_truck_crossing", aim_beat_r2_hold),
    ("aim_02_r1_r2_both", "08_truck_crossing", aim_beat_both),
    ("aim_03_single_fire", "08_truck_crossing", aim_beat_single_fire),
    ("aim_04_world_hit", "08_truck_crossing", aim_beat_world_hit),
    ("aim_05_burst_fire", "08_truck_crossing", aim_beat_burst_fire),
    ("aim_06_reload_partial", "08_truck_crossing", aim_beat_reload_partial),
    ("aim_07_reload_empty", "08_truck_crossing", aim_beat_reload_empty),
    ("aim_08_light_holster", "08_truck_crossing", aim_beat_light_holster),
    ("aim_09_melee", "08_truck_crossing", aim_beat_melee),
    ("aim_10_cable_shots", "08_truck_crossing", aim_beat_cable_shots),
    ("aim_11_cable_melee", "aim_10_cable_shots", aim_beat_cable_melee),
]


def aim_selected(spec: str) -> list[tuple]:
    """`aim` = every AIM beat; otherwise a comma list of names or name prefixes."""
    wanted = spec.split(",")
    if "aim" in wanted:
        return list(AIM_BEATS)
    return [b for b in AIM_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def aim_event_keys(row: dict) -> dict:
    """The weapon fields events() adds for AIM rows."""
    w, g = row["w"], row["g"]
    return {"p5/6/7": f"{w['p5']:#x}/{w['p6']:#x}/{w['p7']:#x}", "274/275": f"{w['a274']}/{w['a275']}",
            "2E": w["e2E"], "2F2": w["l2F2"], "317/318": f"{w['r317']}/{w['m318']}", "q2A": w["q2A"],
            "mag": g["mag"], "reserve": g["reserve"], "fire_mode": g["fire_mode"],
            "light": f"{g['light_d3c']}/{g['lamp6C7']}", "targets": g["targets"][0],
            "gun2E": row["gun"]["ev2E"], "knife": row["knife"]["h"][:2],
            "secgun": row["secgun"]["h"][8:10], "cable": row["cable"]["h"][8:10],
            "cable36": row["cable"]["hit36"], "taken": row["taken"],
            "spawned": ",".join(f"{k}:{v[32:40]}" for k, v in sorted(row.get("pool_delta", {}).items())
                                if v[:2] != "00")}


# ---------------------------------------------------------------------------
# EXIT capture group (C10 lane EXIT, docs/CAPTURES_C10.md section EXIT), opt-in
# (`--beats exit` or a beat's name).  The level exit of beat 15 re-recorded
# closed loop with a wider per-frame trace and split in two at a resumable
# point before the departure movie:
#   exit_00_departure      from the beat-14 end (west tower top): walk to the
#                          fan pair, wait for fan r2's slow window, cross
#                          Z < 156 (D_008107D8 |= 0x80), Roger r8's departure
#                          script 0x828A10 walks the player to its end point;
#                          ends when the script reaches record 0x828AD0.
#   exit_01_movie_arrival  from there, no input: op 0F's movie (selector 1,
#                          inside one frame), the script end and 001B0C60
#                          (1, 0, 4), 001AD010 / 001ADF50, the AREA01 load,
#                          the AREA01 arrival up to the first frame of control,
#                          then 60 idle frames.
# Rows: the route row (decode + decode_exit fields) plus the whole player
# record +0..+0x31F (`pl`), Roger r8 and attachment r9 (+0..+3F, +A0..+DF,
# +1F0..+22F, +2C0..+2EF), the globals 0x810600..0x81085F, the scratchpad
# 0x70003B40..9F, the processed pad words, the vsync counter 0x810E90, the
# overlay header, and the D_007A5640 pool headers that differ from row 0
# (with +18..+3F, +A0..+DF of the live ones), as the AIM rows do.  Pad input
# only; no memory is written.  Outputs: build/c10/exit/<beat>/.
OUT_EXIT = ROOT / "build/c10/exit"
EXIT_PIN = 15800                     # exit_00 gives its first input at this main-loop counter


def exit_pin() -> int:
    """EXIT_PIN on the running chain.  The constant is a v2.6.3 main-loop
    counter (route 14's end snapshot + 30..39 frames); the fork chain's
    counters are offset from the v2.6.3 ones, so in fork mode the pin keeps
    the same distance from route 14's end snapshot."""
    if not FORK:
        return EXIT_PIN
    snap = json.loads((beat_dir("14_roger_encounter") / "snapshot.json").read_text())
    ref = legacy_reference("14_roger_encounter", "13_east_tower")
    if not ref or not ref.get("end_phase"):
        raise RuntimeError("exit_pin: no v2.6.3 counter for route 14's end snapshot (legacy_refs.json)")
    return EXIT_PIN + snap["main_loop_counter"] - ref["end_phase"]["counter"]
EXIT_ROGER, EXIT_ATTACH = 0x7A8830, 0x7A8B20
EXIT10_SPANS = [(n, a, 0x320 if n == "player" else z) for n, a, z in SPANS] + EXIT_SPANS + [
    ("roger:a", EXIT_ROGER, 0x40), ("roger:b", EXIT_ROGER + 0xA0, 0x40),
    ("roger:c", EXIT_ROGER + 0x1F0, 0x40), ("roger:d", EXIT_ROGER + 0x2C0, 0x30),
    ("attach:a", EXIT_ATTACH, 0x40), ("attach:b", EXIT_ATTACH + 0xA0, 0x40),
    ("attach:c", EXIT_ATTACH + 0x1F0, 0x40), ("attach:d", EXIT_ATTACH + 0x2C0, 0x30),
    ("glob", 0x810600, 0x260),          # 0x810600..0x81085F (area bytes, 758, 7D8.., 840..)
    ("spad_hi", 0x70003B40, 0x60),      # 0x70003B40..9F (counter, 3B8C..93, masks)
    ("pad", 0x810E70, 0x8),             # processed held / pressed words
    ("vsync", 0x810E90, 0x4),           # vsync counter (the movie frame advances it)
    ("ovl16", 0x823500, 0x10),          # overlay header (magic, id, text size)
] + [(f"pool{i:02x}", AIM_POOL + i * 0x2F0, 0x18) for i in range(0x100)]


class Exit10Sampler(AimSampler):
    """AimSampler's pool logic over EXIT10_SPANS."""

    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = EXIT10_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))
        self.pool0 = None


def decode_exit10(r: dict[str, bytes]) -> dict:
    row = decode_exit(r)
    row["pl"] = r["player"].hex()
    for who in ("roger", "attach"):
        row[who] = {k: r[f"{who}:{k}"].hex() for k in "abcd"}
    row["roger_script"] = hex(struct.unpack_from("<I", r["roger:c"], 8)[0])
    row["glob"] = r["glob"].hex()
    row["spad_hi"] = r["spad_hi"].hex()
    row["held"] = hex(struct.unpack_from("<H", r["pad"], 0)[0])
    row["pressed"] = hex(struct.unpack_from("<H", r["pad"], 4)[0])
    row["vsync"] = struct.unpack("<I", r["vsync"])[0]
    row["ovl16"] = r["ovl16"].hex()
    row["pool_delta"] = r["pool_delta"]
    row["pool_deep"] = r["deep"]
    return row


def use_exit10_sampler(r: Route) -> None:
    use_exit_sampler(r)                 # the timed step (slow_frames) of beat 15
    sampler = Exit10Sampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_exit10(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)
    r.rows[0]["pool0"] = {k: v for k, v in sampler.pool0.items() if int(v, 16)}
    r.s.boundary_timeout = 900.0        # the movie plays inside one frame


def exit_ovl_id(row: dict) -> int:
    return struct.unpack("<I", bytes.fromhex(row["ovl"][8:16]))[0]


def exit_marks(rows: list[dict]) -> dict:
    """First frame of each exit event in a trace (None when not in it)."""
    def first(pred, start: int = 0):
        return next((row["f"] for row in rows[start:] if pred(row)), None)
    m = {
        "fan_cross_7D8_81": first(lambda row: row["d2"][:2] == "81"),
        "departure_script_828A10": first(lambda row: row["roger_script"] == "0x828a10"),
        "selector_3B8D_2": first(lambda row: row["spad"][2:4] == "02"),
        "player_action_41": first(lambda row: row["m1F0"] == 0x41),
        "walk_end_330_289_127": first(lambda row: row["m1F0"] == 0x41 and
                                      abs(row["pos"][2] - 127.0) < 1e-3 and abs(row["pos"][0] - 330.0) < 1e-3),
        "script_828AD0": first(lambda row: row["roger_script"] == "0x828ad0"),
        "script_op0F_828B50": first(lambda row: row["roger_script"] == "0x828b50"),
        "movie_selector_set": first(lambda row: row["movie"] != "00000000"),
        "request_B8_set": first(lambda row: row["req"][16:18] != "00"),
        "selector_3B8D_3": first(lambda row: row["spad"][2:4] == "03"),
        "area_bytes_01": first(lambda row: row["area4"][:2] == "01"),
        "overlay_id_2": first(lambda row: exit_ovl_id(row) == 2),
        "loader_pending_BD8": first(lambda row: row["bd8"] == 1),
    }
    if m["loader_pending_BD8"] is not None:
        m["loader_done_BD8_0"] = first(lambda row: row["bd8"] == 0, m["loader_pending_BD8"])
    m["first_control_area01"] = first(lambda row: row["area4"][:2] == "01" and
                                      row["slots"][22:24] == "01" and in_control(row))
    return m


def exit_beat_departure(r: Route) -> dict:
    # Beat 15's walk (beat_level_exit): from the beat-14 release point toward
    # the fan pair, stop outside fan r2's hit band, wait for its slow window
    # (phase 1, timer >= 55), walk under it past Z < 156.  Everything after the
    # crossing is automatic; the beat ends when Roger's departure script has
    # walked the player to its end point and moved on to record 0x828AD0
    # (eight frames before the movie frame, so the end snapshot resumes).
    use_exit10_sampler(r)
    # Pin the start to a main-loop counter: pcsx2_session lets a loaded state
    # run freely until Pine answers (the beat-14 snapshot starts anywhere from
    # counter 15761 to 15770), so idle (neutral pad) up to EXIT_PIN first; every
    # recording and replay then gives the same input at the same counter.
    pin = exit_pin()
    if r.rows[-1]["counter"] > pin:
        raise RuntimeError(f"start counter {r.rows[-1]['counter']} is past the pin {pin}")
    r.until(lambda row: row["counter"] >= pin, 200)
    r.goto(331.0, 177.0, tol=1.5, stuck_ok=True)
    r.goto(329.5, 172.0, tol=0.8, magnitude=0.5, stuck_ok=True)
    settle(r, 5)
    r.until(fan_slow_window, 400)
    f_window = r.frame_index
    walk_path(r, [(329.5, 150.0)], tol=1.0,
              until=lambda row: row["d2"][:2] not in ("01", "00") or row["req"][16:18] != "00")
    r.set_pad(0)
    r.until(lambda row: row["spad"][2:4] == "02", 30)
    r.until(lambda row: row["roger_script"] == "0x828ad0", 400)
    marks = dict(exit_marks(r.rows), fan_slow_window=f_window)
    return {"what": "fan r2 slow window, crossing Z < 156 (D_008107D8 = 0x81), Roger r8's departure "
                    "script 0x828A10 walks the player to (330, 289, 127); ends at script record 0x828AD0",
            "marks": marks, "end": summary(r.rows[-1])}


def exit_beat_movie_arrival(r: Route) -> dict:
    # No input.  Op 0F plays movie selector 1 inside one frame, the script
    # ends (D_00810758[0] = 0xFF) and 001B0C60(1, 0, 4) posts the area change;
    # 001AD010 / 001ADF50 / 001FF080 load AREA01 (overlay id 2); the state-0
    # rebuild places the player at AREA01 sub 0 spawn entry 4 with control.
    use_exit10_sampler(r)
    r.set_pad(0)
    r.until(lambda row: row["movie"] != "00000000", 60)
    r.until(lambda row: row["req"][16:18] != "00", 60)
    r.until(lambda row: row["area4"][:2] == "01", 60)
    r.until(lambda row: exit_ovl_id(row) == 2, 60)
    r.until(lambda row: row["slots"][22:24] == "01" and in_control(row), 1200)
    f_control = r.frame_index
    settle(r, 60)
    marks = dict(exit_marks(r.rows), first_control_beat=f_control)
    return {"what": "departure movie (selector 1), 001B0C60(1, 0, 4), AREA01 load, arrival at AREA01 "
                    "sub 0 spawn entry 4 up to the first frame of control, 60 idle frames",
            "marks": marks, "slow_frames": r.slow_frames, "end": summary(r.rows[-1])}


EXIT_BEATS = [
    ("exit_00_departure", "14_roger_encounter", exit_beat_departure),
    ("exit_01_movie_arrival", "exit_00_departure", exit_beat_movie_arrival),
]


def exit_selected(spec: str) -> list[tuple]:
    """`exit` = both EXIT beats in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "exit" in wanted:
        return list(EXIT_BEATS)
    return [b for b in EXIT_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def exit_event_keys(row: dict) -> dict:
    """The exit fields events() adds for EXIT rows."""
    return {"roger_script": row["roger_script"], "fan_r2": f"{row['fan_r2']['phase']}/{row['fan_r2']['timer']}",
            "758": row["flags758"][:2], "area4": row["area4"], "slots": row["slots"][16:24] + "|" +
            row["slots"][144:152], "bd8": row["bd8"], "cd157": row["cd157"], "ovl": exit_ovl_id(row),
            "movie": row["movie"][:2], "movie_req": row["movie_req"], "pool_changed": len(row["pool_delta"])}


# ---------------------------------------------------------------------------
# DAMAGE capture group (C10 lane DAMAGE, docs/CAPTURES_C10.md section DAMAGE),
# opt-in (`--beats dmg` or a beat's name).  Everything that can hurt the
# player in AREA11, low health, death, the game-over screen and Continue:
#   the flame 0x8235F0 (record r7, node 0x7A8540, contact callback 0x823580
#   through the class-1 sphere pass 001A8660), the fan r2 hit band (0x827630,
#   fast arm), the landing hit (0017C580: a drop of 50 or more, reached by a
#   walking jump that falls short into the crevice), the attribute-0x5D
#   floor under the truck pit (0021D250), then the death fade, the
#   game-over wait (001AD4E0), the continue machine (001AC070 / 001AC480):
#   the title menu, NEW GAME and the LOAD GAME screen (left without choosing
#   a memory card slot).  The heavy landing (a drop of more than 104) was not
#   reached (CAPTURES_C10.md).
# Rows: the route row (decode + decode_exit: fans, area bytes, task slots,
# loader byte, overlay header) plus the whole player record +0..+0x31F
# (`pl`) with its damage fields decoded (`vit`), the globals 0x810600..
# 0x810D3F (`glob`: the area and request bytes, D_008106B9, D_008106C8,
# D_008106CE/CF, D_008106F1, D_00810707, D_0081070A, D_0081083C, the display
# vitals 0x810858/5C, the taken rows, inventory and weapon bytes), the pad
# block 0x810E40..0x810E9F (rumble +0x16/+0x18/+0x19/+0x28, the processed
# pad words, the vsync counter), D_00275BD0..EF (D_00275BD4/BD8/BDC/BE0),
# the per-class lists 0x275B54..BB (the hazard list D_00275BA0), the
# scratchpad 0x70003B40..9F, 0x70003A20 and 0x700038A0..BF, the flame node
# (+0..+3F, +A0..+BF, +1F0..+21F) and, as the AIM rows do, the D_007A5640
# pool headers that differ from row 0 with +18..+3F / +A0..+DF of the live
# ones (the effects 0x80000027 / 0x80000043 ...).  Pad input only; no memory
# is written; no memory-card path is entered.  Outputs: build/c10/damage/<beat>/.
OUT_DMG = ROOT / "build/c10/damage"
DMG_FLAME = 0x7A8540                 # r7, overlay owner 0x8235F0
DMG_FLAME_XZ = (452.3, 277.6)        # the flame node's +0xB0 x / z
DMG_SPANS = [(n, a, 0x320 if n == "player" else z) for n, a, z in SPANS] + EXIT_SPANS + [
    ("glob", 0x810600, 0x740),          # 0x810600..0x810D3F
    ("padblk", 0x810E40, 0x60),         # pad block (rumble), E70/E74 pad words, E90 vsync
    ("tasks", 0x275BD0, 0x20),          # D_00275BD4 / BD8 / BDC / BE0
    ("lists", 0x275B54, 0x68),          # per-class lists (B80 targets, BA0 hazards)
    ("spad_hi", 0x70003B40, 0x60),
    ("sp3a20", 0x70003A20, 0x4),
    ("sp38a0", 0x700038A0, 0x20),
    ("flame:a", DMG_FLAME, 0x40), ("flame:b", DMG_FLAME + 0xA0, 0x20), ("flame:c", DMG_FLAME + 0x1F0, 0x30),
] + [(f"pool{i:02x}", AIM_POOL + i * 0x2F0, 0x18) for i in range(0x100)]


class DmgSampler(AimSampler):
    """AimSampler's pool logic over DMG_SPANS."""

    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = DMG_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))
        self.pool0 = None


def decode_dmg(r: dict[str, bytes]) -> dict:
    row = decode_exit(r)
    p, g, pb = r["player"], r["glob"], r["padblk"]
    row["pl"] = p.hex()
    row["vit"] = {
        "health": round(f32(p, 0x220), 4), "pend": round(f32(p, 0x224), 4),
        "infection": round(f32(p, 0x228), 4), "pend_inf": round(f32(p, 0x22C), 4),
        "ev": p[0], "type": p[0xF], "kind": p[0xD], "p4": p[4], "p5": p[5], "p6": p[6], "p7": p[7],
        "a1F0": p[0x1F0], "a1F1": p[0x1F1], "inv20E": _s16(p, 0x20E), "b234": p[0x234],
        "b235": p[0x235], "b236": p[0x236], "s23A": p[0x23A], "s23B": p[0x23B], "g25C": p[0x25C],
        "b25F": p[0x25F], "h28": _s16(p, 0x28), "b302": p[0x302], "hipB4": round(f32(p, 0xB4), 4),
        "top2F4": round(f32(p, 0x2F4), 4), "kb70": vec(p, 0x70, 4),
    }
    row["gv"] = {
        "b6B9": g[0xB9], "w6C8": hex(struct.unpack_from("<I", g, 0xC8)[0]), "b6CE": g[0xCE], "b6CF": g[0xCF],
        "b6F1": g[0xF1], "b707": g[0x107], "b70A": g[0x10A], "b83C": g[0x23C],
        "disp_health": round(f32(g, 0x258), 4), "disp_inf": round(f32(g, 0x25C), 4),
    }
    row["glob"] = g.hex()
    row["rumble"] = {"active": pb[0x16], "big": pb[0x18], "small": pb[0x19],
                     "dur": struct.unpack_from("<H", pb, 0x28)[0]}
    row["padblk"] = pb.hex()
    row["held"] = hex(struct.unpack_from("<H", pb, 0x30)[0])
    row["pressed"] = hex(struct.unpack_from("<H", pb, 0x34)[0])
    row["vsync"] = struct.unpack_from("<I", pb, 0x50)[0]
    row["tasks"] = r["tasks"].hex()
    row["lists"] = r["lists"].hex()
    row["spad_hi"] = r["spad_hi"].hex()
    row["sp3a20"] = r["sp3a20"].hex()
    row["sp38a0"] = r["sp38a0"].hex()
    row["flame"] = {"a": r["flame:a"].hex(), "B0": vec(r["flame:b"], 0x10), "c": r["flame:c"].hex(),
                    "cool210": struct.unpack_from("<i", r["flame:c"], 0x20)[0]}
    row["pool_delta"] = r["pool_delta"]
    row["pool_deep"] = r["deep"]
    return row


def use_dmg_sampler(r: Route) -> None:
    sampler = DmgSampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_dmg(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)
    r.rows[0]["pool0"] = {k: v for k, v in sampler.pool0.items() if int(v, 16)}
    r.s.boundary_timeout = 120.0        # the area load after Continue


DMG_PIN_AFTER = 30     # frames after the source snapshot's main-loop counter
DMG_PIN_AFTER_TITLE = 45   # from the title-menu snapshot (dmg_03's end) a replay started 31 frames late


def dmg_pin(r: Route, beat: str) -> None:
    """Pin the beat's first input to a main-loop counter (as exit_00 does):
    pcsx2_session lets a loaded state run freely until Pine answers, so a
    replay starts up to ~25 frames after the snapshot (31 from the title
    menu); every recording and replay idles (neutral pad) up to the source
    snapshot's counter + 30 (+ 45 from dmg_03's title-menu snapshot)."""
    source = next(b[1] for b in DMG_BEATS if b[0] == beat)
    snap = json.loads((beat_dir(source) / "snapshot.json").read_text())
    pin = snap["main_loop_counter"] + (DMG_PIN_AFTER_TITLE if source == "dmg_03_gameover_timeout"
                                       else DMG_PIN_AFTER)
    if r.rows[-1]["counter"] > pin:
        raise RuntimeError(f"start counter {r.rows[-1]['counter']} is past the pin {pin}")
    r.set_pad(0)
    r.until(lambda row: row["counter"] >= pin, 200)


def dmg_slot0(row: dict) -> dict:
    """Task slot 0 (0x28A750): +4 the task function, +8 state, +9 sub, +0xA,
    +0xF the prompt cursor, +0x16 the prompt idle timer, +0x18 the hold."""
    b = bytes.fromhex(row["slots"][:0x40])
    return {"fn": struct.unpack_from("<I", b, 4)[0], "s8": b[8], "s9": b[9], "sA": b[0xA], "sB": b[0xB],
            "cur": b[0xF], "t16": struct.unpack_from("<h", b, 0x16)[0], "t18": struct.unpack_from("<h", b, 0x18)[0]}


def dmg_fade(row: dict) -> int:
    return int(row["fade"][:2], 16)


def dmg_controllable(row: dict) -> bool:
    v = row["vit"]
    return v["p4"] == 1 and v["p5"] in (0, 1) and row["spad"][2:4] == "00" and v["ev"] == 1


def dmg_flame_dist(row: dict) -> float:
    return math.hypot(row["pos"][0] - DMG_FLAME_XZ[0], row["pos"][2] - DMG_FLAME_XZ[1])


def dmg_meta(r: Route, what: str, **extra) -> dict:
    hits = [row["f"] for a, row in zip(r.rows, r.rows[1:]) if row["vit"]["health"] < a["vit"]["health"]]
    v = r.rows[-1]["vit"]
    return dict(extra, what=what, hit_frames=hits,
                health_path=[r.rows[0]["vit"]["health"]] + [r.rows[f]["vit"]["health"] for f in hits],
                end={"health": v["health"], "infection": v["infection"], "ev": v["ev"], "p4/p5": [v["p4"], v["p5"]],
                     "b235": v["b235"], "b6B9": r.rows[-1]["gv"]["b6B9"], "slot0": dmg_slot0(r.rows[-1]),
                     "fade": dmg_fade(r.rows[-1]), "area": r.rows[-1]["area4"], "pos": r.rows[-1]["pos"]})


DMG_FLAME_REST = (471.3, 283.2)      # the 11_crevice_prompt release point (out of the flame's reach)


def dmg_flame_hit(r: Route, limit: int = 400) -> dict:
    """One flame contact: walk at the flame (full stick) whenever the player
    is controllable, neutral otherwise, until the health drops (the hit is
    applied by 0021C440 the frame after 001A8660 posts it); then neutral
    until the reaction hands control back (+4 == 1)."""
    hp0 = r.rows[-1]["vit"]["health"]
    for _ in range(limit):
        row = r.rows[-1]
        if row["vit"]["health"] < hp0:
            break
        if dmg_controllable(row) or (row["vit"]["p4"] == 1 and row["vit"]["p5"] in (0, 1)):
            r.stick_toward(*DMG_FLAME_XZ, 1.0)
        else:
            r.set_pad(0)
        r.step(1)
    else:
        raise TimeoutError(f"no flame hit in {limit} frames; last {summary(r.rows[-1])}")
    r.set_pad(0)
    return r.until(lambda row: row["vit"]["p4"] == 1 or row["vit"]["health"] <= 0, 300)


def dmg_flame_hits_to(r: Route, health: float) -> None:
    """Flame hits until the health is at or below `health` (5 per hit)."""
    for _ in range(25):
        if r.rows[-1]["vit"]["health"] <= health:
            return
        dmg_flame_hit(r)
    raise RuntimeError("health target not reached")


def dmg_retreat(r: Route) -> None:
    """Walk back to the release point, wait for the post-hit protection to end."""
    r.until(lambda row: row["vit"]["p4"] == 1 and row["vit"]["p5"] in (0, 1), 300)
    r.goto(*DMG_FLAME_REST, tol=1.0, magnitude=1.0, stuck_ok=True)
    r.set_pad(0)
    r.until(lambda row: row["vit"]["ev"] == 1 and row["vit"]["inv20E"] <= 0 and in_control(row), 300)


def dmg_gameover_screen(row: dict) -> bool:
    """The game-over wait (gameplay task 001ACEC0 state 3 sub 2) in its hold
    sub-state 3 with the fade machine idle: the GAME OVER art is shown."""
    s0 = dmg_slot0(row)
    return s0["fn"] == 0x1ACEC0 and (s0["s8"], s0["s9"], s0["sA"]) == (3, 2, 3) and dmg_fade(row) == 0


def dmg_title_prompt(row: dict) -> bool:
    """The continue machine 001AC070 in its prompt (state 2, 001AC480 sub 2)
    with the fade idle: the title menu takes input."""
    s0 = dmg_slot0(row)
    return s0["fn"] == 0x1AC070 and (s0["s8"], s0["s9"]) == (2, 2) and dmg_fade(row) == 0


def dmg_press_until(r: Route, button: str, pred, wait: int = 20, tries: int = 6) -> None:
    """Tap a button (2 frames) and wait for its effect; repeat when the press
    landed while the machine was not taking input."""
    for _ in range(tries):
        r.press(button, 2)
        for _ in range(wait):
            if pred(r.rows[-1]):
                return
            r.step(1)
        if pred(r.rows[-1]):
            return
    raise TimeoutError(f"{button}: no effect after {tries} presses; last {summary(r.rows[-1])}")


def dmg_death_to_gameover(r: Route, limit: int = 1200) -> dict:
    """No input from the death on: the death reaction, its fade-out, the
    game-over wait and the GAME OVER screen (held 10 frames)."""
    r.set_pad(0)
    r.until(lambda row: row["gv"]["b6B9"] == 1, 300)
    r.until(dmg_gameover_screen, limit)
    r.step(10)
    return r.rows[-1]


def dmg_marks(rows: list[dict]) -> dict:
    """First frame of each damage / death / game-over event in a trace."""
    def first(pred, start: int = 0):
        return next((row["f"] for row in rows[start:] if pred(row)), None)
    m = {
        "pending_damage": first(lambda row: row["vit"]["pend"] != 0),
        "first_hit": first(lambda row: row["vit"]["health"] < rows[0]["vit"]["health"]),
        "low_health_latch_235": first(lambda row: row["vit"]["b235"] & 1 and not rows[0]["vit"]["b235"] & 1),
        "death_health_0": first(lambda row: row["vit"]["health"] <= 0 < rows[0]["vit"]["health"]),
        "death_latch_6B9": first(lambda row: row["gv"]["b6B9"] == 1 and rows[0]["gv"]["b6B9"] == 0),
        "fade_out": first(lambda row: dmg_fade(row) == 3 and dmg_fade(rows[0]) == 0),
        "gameover_wait": first(lambda row: dmg_slot0(row)["fn"] == 0x1ACEC0 and dmg_slot0(row)["s9"] == 2),
        "gameover_screen": first(dmg_gameover_screen),
        "continue_machine": first(lambda row: dmg_slot0(row)["fn"] == 0x1AC070),
        "title_prompt": first(dmg_title_prompt),
        "gameplay_task_back": first(lambda row: dmg_slot0(row)["fn"] == 0x1ACEC0 and
                                    dmg_slot0(rows[0])["fn"] == 0x1AC070),
    }
    return {k: v for k, v in m.items() if v is not None}


def dmg_running_jump(r: Route, start: tuple | None, toward: tuple, at_edge, limit: int = 200,
                     magnitude: float = 1.0, stall: bool = True) -> None:
    """Route beats 12 / 14's running jump: stand at `start`, run toward
    `toward` and press Cross (2 frames, stick held) once `at_edge(row)`
    holds; then keep the stick until the jump (+1F0 0x0C) has left the
    ground and release it."""
    if start is not None:
        r.goto(*start, tol=1.0, magnitude=1.0, stuck_ok=True)
        r.set_pad(0)
        settle(r, 5)
    for i in range(limit):
        if at_edge(r.rows[-1]):
            break
        if stall and i > 20 and math.dist(r.rows[-1]["pos"], r.rows[-4]["pos"]) < 0.02:
            break                       # the walk stopped short of the edge (observed)
        r.stick_toward(*toward, magnitude)
        r.step(1)
    else:
        raise TimeoutError(f"edge not reached; last {summary(r.rows[-1])}")
    r.set_pad(PAD["CROSS"], r.pad_state[1], r.pad_state[2])
    r.step(2)
    r.set_pad(0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] == 0x0C, 10, 0, r.pad_state[1], r.pad_state[2])
    r.until(lambda row: row["m1F0"] != 0x0C, 300, 0, r.pad_state[1], r.pad_state[2])
    r.set_pad(0)


def dmg_beat_flame_hit(r: Route) -> dict:
    # From the 11_crevice_prompt release point on the grating next to the
    # flame: walk at it until one contact lands (+0x224 = 5.0, +0x0F = 0xC,
    # +0 = 3, then 0021C440's generic tail: health 95, +1F1 = 4, flinch +4 2
    # +5 0), walk back to the release point while the 60-frame protection
    # (+0x20E) runs (the knock-back does not always clear the flame's reach),
    # wait it out and idle.
    use_dmg_sampler(r)
    dmg_pin(r, "dmg_00_flame_hit")
    r.idle(5)
    dmg_flame_hit(r)
    dmg_retreat(r)
    r.idle(30)
    return dmg_meta(r, "one flame contact: 100 -> 95, flinch, knock-back, 60-frame protection",
                    marks=dmg_marks(r.rows))


def dmg_beat_flame_low_health(r: Route) -> dict:
    # Flame contacts until the health is 35 (0021C440's tail sets +0x235 bit 0
    # at <= 35), back to the release point, then 300 idle frames: the
    # low-health heartbeat (0015D000: rumble every 121 frames at <= 35).
    use_dmg_sampler(r)
    dmg_pin(r, "dmg_01_flame_low_health")
    r.idle(5)
    dmg_flame_hits_to(r, 35.0)
    dmg_retreat(r)
    r.idle(300)
    return dmg_meta(r, "flame contacts down to 35 (low-health latch), retreat, 300 idle frames (heartbeat)",
                    marks=dmg_marks(r.rows))


def dmg_beat_flame_death(r: Route) -> dict:
    # Down to 10 (the faster heartbeat: every 61 frames at <= 10), 150 idle
    # frames out of reach, then contacts until the health is 0: the death
    # reaction (+4 2 +5 1, clip 0x2A), D_008106B9 = 1, 0021D2E0, the fade-out,
    # the game-over wait 001AD4E0; ends on the GAME OVER screen.
    use_dmg_sampler(r)
    dmg_pin(r, "dmg_02_flame_death")
    r.idle(5)
    dmg_flame_hits_to(r, 10.0)
    dmg_retreat(r)
    r.idle(150)
    dmg_flame_hits_to(r, 0.0)
    dmg_death_to_gameover(r)
    return dmg_meta(r, "flame contacts to 10, 150 idle frames, contacts to 0: death, fade-out, game-over "
                       "wait; ends on the GAME OVER screen", marks=dmg_marks(r.rows))


def dmg_beat_gameover_timeout(r: Route) -> dict:
    # No input: the 240-frame hold runs out (sub 4: fade-out), 001AD4E0 hands
    # over (+9 = 4), 001ADF00 replaces the gameplay task with 001AC070 (from
    # death: D_00275BDC = 1), 001AC480 shows screen module 1 (the title menu)
    # with the cursor on its second entry; ends 30 frames into the prompt.
    use_dmg_sampler(r)
    dmg_pin(r, "dmg_03_gameover_timeout")
    r.set_pad(0)
    r.until(dmg_title_prompt, 1200)
    r.step(30)
    return dmg_meta(r, "GAME OVER hold runs out, fade-out, the continue machine 001AC070 shows the title "
                       "menu (cursor on the second entry); 30 frames into the prompt", marks=dmg_marks(r.rows))


def dmg_beat_new_game(r: Route) -> dict:
    # From the title menu after a death: Up moves the cursor to the first
    # entry (NEW GAME), Cross confirms (001AC480 sub 3, fade-out, state 4:
    # 001AB790(001ACEC0) reinstalls the gameplay task), the new game loads
    # AREA11 and plays the opening up to the first frame of control; 60 idle.
    use_dmg_sampler(r)
    dmg_pin(r, "dmg_04_new_game")
    r.s.boundary_timeout = 900.0        # the new-game movie plays inside one frame
    r.idle(5)
    dmg_press_until(r, "UP", lambda row: dmg_slot0(row)["cur"] == 0)
    r.idle(10)
    dmg_press_until(r, "CROSS", lambda row: dmg_slot0(row)["s9"] == 3 or dmg_slot0(row)["fn"] != 0x1AC070)
    r.set_pad(0)
    r.until(lambda row: dmg_slot0(row)["fn"] == 0x1ACEC0, 600)
    r.until(lambda row: row["area4"][:2] == "0b" and dmg_slot0(row)["sB"] == 1, 4000)
    r.until(cutscene, 2000)
    r.until(in_control, 6000)
    f_control = r.frame_index
    r.idle(60)
    return dmg_meta(r, "title menu after a death: Up to the first entry, Cross: the gameplay task is "
                       "reinstalled, the new game loads AREA11 and plays the opening to first control; "
                       "60 idle frames", marks=dict(dmg_marks(r.rows), first_control=f_control))


def dmg_beat_load_screen(r: Route) -> dict:
    # From the title menu after a death, cursor on its second entry (LOAD
    # GAME): Cross (001AC480 sub 3, fade-out; confirm with cursor 1 calls
    # 00225A00 and enters state 5, which polls 00225AC0).  The load screen
    # asks for a memory card slot; the beat stops there (no slot is chosen,
    # nothing is read from or written to a memory card) and leaves with
    # Triangle (the screen's exit): 00225AC0 returns 1 and state 5 goes back
    # to the prompt (state 2); ends 30 frames into the title menu again.
    use_dmg_sampler(r)
    dmg_pin(r, "dmg_05_load_screen")
    r.idle(5)
    dmg_press_until(r, "CROSS", lambda row: dmg_slot0(row)["s9"] == 3 or dmg_slot0(row)["s8"] != 2)
    r.set_pad(0)
    r.until(lambda row: dmg_slot0(row)["s8"] == 5 and dmg_fade(row) == 0, 600)
    r.idle(60)
    dmg_press_until(r, "TRIANGLE", lambda row: dmg_slot0(row)["s8"] != 5, wait=30)
    r.set_pad(0)
    r.until(dmg_title_prompt, 900)
    r.step(30)
    return dmg_meta(r, "title menu after a death: Cross on LOAD GAME opens the load screen (memory card "
                       "slot choice); Triangle leaves it without choosing a slot; back on the title menu",
                    marks=dict(dmg_marks(r.rows),
                               load_state5=next((x["f"] for x in r.rows if dmg_slot0(x)["s8"] == 5), None)))


def dmg_beat_crevice_fall(r: Route) -> dict:
    # From the 11_crevice_prompt release point: beat 12's way to the plateau
    # edge (485, 275), (477, 262), facing north, then a WALKING jump (stick
    # at 0.45, Cross at z <= 252, where the walk stops the
    # player short of the edge): the
    # running-jump state 6 is entered (+1F0 0x0C, take-off height +2F4 =
    # 269.6) but falls short of the north block into the crevice floor
    # (184.8).  0017C580 measures d = +B4 - +2F4 (-74.8, <= -50): rumble,
    # +0x224 = 5.0, 0021C350 (health 95), sound 0x151, landing +6 = 3
    # (00163E90: clip 0x75, then the hand-back with +0x20E = 60).
    use_dmg_sampler(r)
    dmg_pin(r, "dmg_06_crevice_fall")
    r.idle(5)
    walk_path(r, [(485, 275), (477, 262)], tol=1.0)
    r.set_pad(0)
    settle(r, 5)
    face(r, math.pi)
    dmg_running_jump(r, None, (477.0, 150.0), lambda row: row["pos"][2] <= 252.0, magnitude=0.45,
                     stall=False)
    r.until(lambda row: row["vit"]["p5"] == 8, 200)
    r.until(lambda row: in_control(row) and row["vit"]["p4"] == 1 and row["vit"]["p5"] == 0, 400)
    r.until(lambda row: row["vit"]["inv20E"] <= 0, 200)
    r.idle(30)
    return dmg_meta(r, "walking jump off the plateau edge falls short into the crevice (269.6 -> 184.8): "
                       "landing hit, 5 damage", marks=dmg_marks(r.rows))


def dmg_beat_pit_fall(r: Route) -> dict:
    # From the 07_truck_preview end (the truck still up): walk east onto the
    # truck as beat 08 does and stay on it: its fall carries the player
    # into the pit (truck roof y 154.6, D_00810792 = 0xFF).  Then walk south
    # off the truck: the fall state (+5 5) reaches the pit floor, attribute
    # 0x5D (y 100.8, not solid for the movement walkers), and runs 0021D250
    # (+0 = 2, health 0, +5 = 0x16, sound 0x159) and 0021D2E0; the body keeps
    # falling; the fade-out and the game-over wait follow; ends on the GAME
    # OVER screen.
    use_dmg_sampler(r)
    dmg_pin(r, "dmg_07_pit_fall")
    r.idle(5)
    r.goto(352.0, 392.0, tol=1.5, magnitude=1.0, stuck_ok=True)
    r.set_pad(0)
    r.until(lambda row: row["story792"] == 0xFF, 400)
    r.idle(30)
    for _ in range(300):
        row = r.rows[-1]
        if row["vit"]["p5"] == 5 or row["vit"]["health"] <= 0:
            break
        if row["vit"]["p4"] == 1 and row["vit"]["p5"] in (0, 1):
            r.stick_toward(352.0, 440.0, 1.0)
        else:
            r.set_pad(0)
        r.step(1)
    else:
        raise TimeoutError(f"did not leave the truck; last {summary(r.rows[-1])}")
    r.set_pad(0)
    dmg_death_to_gameover(r)
    return dmg_meta(r, "ride the truck into the pit, walk off it onto the attribute-0x5D pit floor: "
                       "death, fade-out, GAME OVER screen", marks=dmg_marks(r.rows))


def dmg_fan_fast(row: dict) -> bool:
    return row["fan_r2"]["spin"] >= 0.034906585


def dmg_beat_fan_hit(r: Route) -> dict:
    # From the west tower top (14_roger_encounter end): walk to the fan pair
    # as the level exit does, stop outside fan r2's band, wait for its fast
    # arm (spin +0x38 >= 0.0349), then step into the hit band (Z 156..166.5,
    # never below 156, the exit): 0x827630 writes +0x224 = 5.0, +0x0F = 6,
    # +0 = 3, +0x70 = (0, 0, 1, 1); 0021C440 type 6: health 95, +5 = 0x11.
    # Back out of the band after the hit and idle.
    use_dmg_sampler(r)
    dmg_pin(r, "dmg_08_fan_hit")
    pin = exit_pin()
    r.until(lambda row: row["counter"] >= pin, 200)           # the player is held until about 15800 (v2.6.3)
    r.goto(331.0, 177.0, tol=1.5, stuck_ok=True)
    r.goto(329.5, 172.0, tol=0.8, magnitude=0.5, stuck_ok=True)
    settle(r, 5)
    r.until(dmg_fan_fast, 600)
    hp0 = r.rows[-1]["vit"]["health"]
    for _ in range(200):
        row = r.rows[-1]
        if row["vit"]["pend"] != 0 or row["vit"]["health"] < hp0 or row["vit"]["ev"] != 1:
            break
        if row["pos"][2] <= 162.0:
            r.set_pad(0)
        else:
            r.stick_toward(329.5, 158.0, 0.5)
        r.step(1)
    else:
        raise TimeoutError(f"no fan hit; last {summary(r.rows[-1])}")
    r.set_pad(0)
    r.until(lambda row: row["vit"]["p4"] == 1 and row["vit"]["p5"] in (0, 1), 300)
    r.goto(331.0, 180.0, tol=1.5, magnitude=0.6, stuck_ok=True)
    r.set_pad(0)
    r.until(lambda row: row["vit"]["ev"] == 1 and row["vit"]["inv20E"] <= 0 and in_control(row), 300)
    r.idle(30)
    return dmg_meta(r, "fan r2's fast arm hits the player in its band (Z 156..166.5): 5 damage, "
                       "reaction 0x11; back out of the band", marks=dmg_marks(r.rows))


DMG_BEATS = [
    ("dmg_00_flame_hit", "11_crevice_prompt", dmg_beat_flame_hit),
    ("dmg_01_flame_low_health", "dmg_00_flame_hit", dmg_beat_flame_low_health),
    ("dmg_02_flame_death", "dmg_01_flame_low_health", dmg_beat_flame_death),
    ("dmg_03_gameover_timeout", "dmg_02_flame_death", dmg_beat_gameover_timeout),
    ("dmg_04_new_game", "dmg_03_gameover_timeout", dmg_beat_new_game),
    ("dmg_05_load_screen", "dmg_03_gameover_timeout", dmg_beat_load_screen),
    ("dmg_06_crevice_fall", "11_crevice_prompt", dmg_beat_crevice_fall),
    ("dmg_07_pit_fall", "07_truck_preview", dmg_beat_pit_fall),
    ("dmg_08_fan_hit", "14_roger_encounter", dmg_beat_fan_hit),
]


def dmg_selected(spec: str) -> list[tuple]:
    """`dmg` = every DAMAGE beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "dmg" in wanted:
        return list(DMG_BEATS)
    return [b for b in DMG_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def dmg_event_keys(row: dict) -> dict:
    """The damage fields events() adds for DMG rows."""
    v, g, s0 = row["vit"], row["gv"], dmg_slot0(row)
    return {"health": v["health"], "pend": v["pend"], "inf": v["infection"], "ev": v["ev"], "typeF": v["type"],
            "p4/5/6": f"{v['p4']:#x}/{v['p5']:#x}/{v['p6']:#x}", "1F0/1F1": f"{v['a1F0']:#x}/{v['a1F1']}",
            "inv20E": v["inv20E"] > 0, "235": v["b235"], "23A": v["s23A"], "6B9": g["b6B9"],
            "rumble": row["rumble"]["active"], "slot0": f"{s0['fn']:#x}:{s0['s8']}/{s0['s9']}/{s0['sA']} cur{s0['cur']}",
            "bdc/be0": row["tasks"][24:26] + "/" + row["tasks"][32:34], "ovl": exit_ovl_id(row),
            "flame_cool": row["flame"]["cool210"] > 0,
            "spawned": ",".join(f"{k}:{v_[32:40]}" for k, v_ in sorted(row.get("pool_delta", {}).items())
                                if v_[:2] != "00")}


# ---------------------------------------------------------------------------
# BRANCH capture group (C10 lane BRANCH, opt-in, `--beats br` or a beat's
# name): the AREA11 branches the main route skips, found from the placement
# table 0x82A3C0 and the deferred group D_0024D820[11] (0x828180): the five
# optional pickups (g0.1..g0.5, 00219550) and the map item (g0.6, 0015AFA0),
# the west-yard and plateau ladders (attribute-0x32 columns, up and down),
# the breakable boxes 001551B0, the terminal's ride
# back up, the panel's BATTERY prompt declined, and Roger's talk after the
# encounter.  Every beat starts from a route (or earlier br) snapshot, pins
# its first input to a main-loop counter, drives closed loop with pad input
# only and writes build/c10/branch/<beat>/.  Described in
# docs/CAPTURES_C10.md (section BRANCH).
OUT_BR = ROOT / "build/c10/branch"
BR_ITEM_NODE = 0x7A5640                # deferred g0.i is pool node 0x7A5640 + i * 0x2F0
# name -> (deferred index, persistence uid, item type, record position)
BR_ITEMS = {
    "g0.1": (1, 4, 0x1E, (467.3, 184.8, 227.4)),     # the yard floor south of the north block
    "g0.2": (2, 5, 0x1F, (431.9, 354.8, 411.8)),     # the 355 top above the plateau ladder
    "g0.3": (3, 6, 0x1E, (213.6, 219.8, 311.9)),     # the 220 ledge behind the crates
    "g0.4": (4, 7, 0x32, (381.3, 225.3, 266.8)),     # the cage floor (model 2, wall mount)
    "g0.5": (5, 8, 0x10, (311.6, 249.8, 328.7)),     # the 250 ledge, inside box r6
    "g0.6": (6, 9, 0x08, (231.1, 200.9, 428.0)),     # 0015AFA0, south of the slide's foot
}
BR_BOXES = {"r3": 0x7A7980, "r4": 0x7A7C70, "r5": 0x7A7F60, "r6": 0x7A8250,     # 001551B0
            "r14": 0x7A99D0, "r15": 0x7A9CC0}                                 # 00156620
BR_ELEV, BR_PANEL, BR_ROGER = 0x7AA880, 0x7AA590, 0x7A8830
BR_SPANS = DMG_SPANS + [
    ("uirec", 0x810130, 0x60),          # status page record
    ("msgrec", 0x282210, 0x14),         # message service record
    ("sp3190", 0x70003190, 0x50),       # ray query block (the use scan's ray)
    ("masks", 0x70003B70, 0x10),
    ("elev:a", BR_ELEV, 0x40), ("elev:c", BR_ELEV + 0x1F0, 0x30),
    ("elev_y", 0x82A7C0, 0x8), ("elev_y2", 0x82A840, 0x8), ("elev_y3", 0x82A940, 0x8),
    ("elev_y4", 0x82AB10, 0x8),
    ("panel:a", BR_PANEL, 0x40), ("panel:c", BR_PANEL + 0x1F0, 0x30),
    ("roger:a", BR_ROGER, 0x40), ("roger:c", BR_ROGER + 0x1F0, 0x40),
]
for _n, (_i, _u, _t, _p) in BR_ITEMS.items():
    _a = BR_ITEM_NODE + _i * 0x2F0
    BR_SPANS += [(f"it{_i}:a", _a, 0x40), (f"it{_i}:b", _a + 0xB0, 0x10), (f"it{_i}:c", _a + 0x1F0, 0x30),
                 (f"it{_i}:e", _a + 0x2E0, 0x10)]
for _n, _a in BR_BOXES.items():
    BR_SPANS += [(f"bx_{_n}:a", _a, 0xE0), (f"bx_{_n}:c", _a + 0x1F0, 0x30)]


class BrSampler(AimSampler):
    """AimSampler's pool logic over BR_SPANS."""

    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = BR_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))
        self.pool0 = None


def decode_br(r: dict[str, bytes]) -> dict:
    row = decode_dmg(r)
    g = r["glob"]
    row["br"] = 1
    row["ui_rec"] = r["uirec"].hex()
    row["msgrec"] = r["msgrec"].hex()
    row["sp3190"] = r["sp3190"].hex()
    row["masks"] = r["masks"].hex()
    row["taken11"] = g[0x3C0:0x3E0].hex()          # D_00810860 + 32 * 11
    row["inv_br"] = {f"{t:#x}": g[0x664 + t] for t in (0x08, 0x10, 0x1B, 0x1E, 0x1F, 0x32)}
    row["weap"] = {"c61": g[0x661], "mag": g[0x662], "reserve": _s16(g, 0x6B4)}
    row["elev"] = {"a": r["elev:a"].hex(), "c": r["elev:c"].hex(), "down83A": g[0x23A],
                   "y": [round(f32(r[k], 4), 3) for k in ("elev_y", "elev_y2", "elev_y3")] +
                        [round(f32(r["elev_y4"], 4), 3)]}
    row["panel"] = {"a": r["panel:a"].hex(), "c": r["panel:c"].hex()}
    row["roger"] = {"a": r["roger:a"].hex(), "c": r["roger:c"].hex(),
                    "script": hex(struct.unpack_from("<I", r["roger:c"], 8)[0])}
    row["items"] = {}
    for n, (i, _u, _t, _p) in BR_ITEMS.items():
        a, b, c, e = (r[f"it{i}:{k}"] for k in "abce")
        row["items"][n] = {"h": a[:0x10].hex(), "a": a.hex(), "B0": vec(b, 0), "c": c.hex(), "e": e.hex()}
    row["boxes"] = {n: {"h": r[f"bx_{n}:a"][:0x10].hex(), "a": r[f"bx_{n}:a"].hex(),
                        "dmg36": _s16(r[f"bx_{n}:a"], 0x36), "B0": vec(r[f"bx_{n}:a"], 0xB0),
                        "c": r[f"bx_{n}:c"].hex()} for n in BR_BOXES}
    return row


def use_br_sampler(r: Route) -> None:
    sampler = BrSampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_br(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)
    r.rows[0]["pool0"] = {k: v for k, v in sampler.pool0.items() if int(v, 16)}
    r.s.boundary_timeout = 120.0


BR_PIN_AFTER = 30      # frames after the source snapshot's main-loop counter (as DAMAGE)


def br_pin(r: Route, beat: str) -> None:
    """Pin the beat's first input to a main-loop counter: the source
    snapshot's counter + 30 (from 14_roger_encounter at least EXIT_PIN, the
    player is held until about 15800).  pcsx2_session lets a loaded state
    run freely until Pine answers, so recordings and replays start a few
    frames apart; the pin makes them give the same inputs at the same
    counters."""
    source = next(b[1] for b in BR_BEATS if b[0] == beat)
    snap = json.loads((beat_dir(source) / "snapshot.json").read_text())
    pin = snap["main_loop_counter"] + BR_PIN_AFTER
    if source == "14_roger_encounter":
        pin = max(pin, exit_pin())
    if r.rows[-1]["counter"] > pin:
        raise RuntimeError(f"start counter {r.rows[-1]['counter']} is past the pin {pin}")
    r.set_pad(0)
    r.until(lambda row: row["counter"] >= pin, 400)


def br_taken(row: dict, puid: int) -> bool:
    return bool(int(row["taken11"][2 * (puid // 8):2 * (puid // 8) + 2], 16) & (1 << (puid % 8)))


def br_status_open(row: dict) -> bool:
    return row["ui"][2:4] == "03"


def br_bearing(row: dict, x: float, z: float) -> float:
    return math.atan2(x - row["pos"][0], z - row["pos"][2])


def br_status_exit(r: Route, limit: int = 1500) -> None:
    """A take's status page: wait until it browses (ui page state 05 01, as
    beat 01's ITEM page) or until the page record (0x810130..8F, whose
    message countdown runs while the item's text shows) has been still for
    60 frames; then Triangle until the status closes, and wait for control."""
    still, last = 0, None
    for _ in range(limit):
        row = r.rows[-1]
        if row["ui"][8:12] == "0501":
            break
        key = (row["ui"], row["ui_rec"][:0x40])    # +0x20 counts frames
        still = still + 1 if key == last else 0
        last = key
        if still >= 60:
            break
        r.step(1)
    r.idle(20)
    for _ in range(8):
        r.press("TRIANGLE", 2)
        for _ in range(40):
            if not br_status_open(r.rows[-1]):
                break
            r.step(1)
        if not br_status_open(r.rows[-1]):
            break
    else:
        raise TimeoutError("status did not close; last " + summary(r.rows[-1]))
    r.until(in_control, 900)
    settle(r, 30)


def br_take(r: Route, item: str, stand: tuple, tol: float = 0.8, tries: int = 4) -> dict:
    """Walk to `stand`, face the item, press Cross until its take starts
    (the use scan leaves control); then wait for the taken bit, handle the
    status page the take opens and wait for control.  Returns the marks."""
    idx, puid, _t, (ix, _iy, iz) = BR_ITEMS[item]
    r.goto(*stand, tol=tol, magnitude=0.5, stuck_ok=True)
    settle(r, 10)
    face(r, br_bearing(r.rows[-1], ix, iz), tol=0.08)
    f_press = None
    for _ in range(tries):
        f_press = r.frame_index
        r.press("CROSS", 2)
        for _ in range(40):
            if not in_control(r.rows[-1]):
                break
            r.step(1)
        if not in_control(r.rows[-1]):
            break
        settle(r, 10)
        face(r, br_bearing(r.rows[-1], ix, iz), tol=0.08)
    else:
        raise TimeoutError(f"{item}: Cross started nothing; last {summary(r.rows[-1])}")
    f_scan = r.frame_index
    r.until(lambda row: br_taken(row, puid) or br_status_open(row), 900)
    r.until(lambda row: br_status_open(row) or in_control(row), 900)
    f_status = r.frame_index if br_status_open(r.rows[-1]) else None
    if f_status is not None:
        br_status_exit(r)
    else:
        settle(r, 30)
    if not br_taken(r.rows[-1], puid):
        raise RuntimeError(f"{item}: not taken; last {summary(r.rows[-1])}")
    f_taken = next(row["f"] for row in r.rows if br_taken(row, puid))
    return {"item": item, "press": f_press, "scan": f_scan, "taken_bit": f_taken, "status_open": f_status,
            "control": r.frame_index}


def br_ladder_up(r: Route, foot: tuple, yaw: float, tries: int = 3) -> dict:
    """Use (Cross) at an attribute-0x32 ladder face from its foot, climb with
    the stick held up until the climb hands back control at the top."""
    r.goto(*foot, tol=0.6, magnitude=0.5, stuck_ok=True)
    settle(r, 5)
    face(r, yaw, tol=0.08)
    f0 = r.frame_index
    for _ in range(tries):
        r.press("CROSS", 2)
        for _ in range(60):
            if r.rows[-1]["m1F0"] in (0x15, 0x16, 0x17):
                break
            r.step(1)
        if r.rows[-1]["m1F0"] in (0x15, 0x16, 0x17):
            break
        settle(r, 10)
        face(r, yaw, tol=0.08)
    else:
        raise TimeoutError("ladder not grabbed; last " + summary(r.rows[-1]))
    grab = r.rows[-1]["m1F0"]
    r.until(lambda row: row["m1F0"] == 0x17, 120)
    r.set_pad(0, 0x7F, 0x00)
    r.until(lambda row: row["m1F0"] not in (0x15, 0x16, 0x17, 0x18), 1200, 0, 0x7F, 0x00)
    r.set_pad(0)
    settle(r, 30)
    return {"press": f0, "grab_action": grab, "top_y": r.rows[-1]["pos"][1]}


def br_ladder_down(r: Route, top: tuple, yaw: float, tries: int = 3) -> dict:
    """Use (Cross) at the ladder's top floor node facing the drop: the grab
    from above (00180300 decides +1F0 0x15 / 0x16 by the face 10 units above
    the feet); then the stick held down until the climb hands back control
    at the foot."""
    r.goto(*top, tol=0.6, magnitude=0.5, stuck_ok=True)
    settle(r, 5)
    face(r, yaw, tol=0.08)
    f0 = r.frame_index
    for _ in range(tries):
        r.press("CROSS", 2)
        for _ in range(60):
            if r.rows[-1]["m1F0"] in (0x15, 0x16, 0x17, 0x18):
                break
            r.step(1)
        if r.rows[-1]["m1F0"] in (0x15, 0x16, 0x17, 0x18):
            break
        settle(r, 10)
        face(r, yaw, tol=0.08)
    else:
        raise TimeoutError("ladder not grabbed from the top; last " + summary(r.rows[-1]))
    grab = r.rows[-1]["m1F0"]
    y0 = r.rows[-1]["pos"][1]
    r.set_pad(0, 0x7F, 0xFF)
    r.until(lambda row: row["m1F0"] not in (0x15, 0x16, 0x17, 0x18) and row["pos"][1] < y0 - 20, 1500,
            0, 0x7F, 0xFF)
    r.set_pad(0)
    settle(r, 30)
    return {"press": f0, "grab_action": grab, "foot_y": r.rows[-1]["pos"][1]}


def br_climb(r: Route, yaw: float, tries: int = 5) -> None:
    """Use (Cross) facing a ledge until the ledge climb (+1F0 8) starts; wait
    for control on top."""
    for _ in range(tries):
        face(r, yaw, tol=0.08)
        r.press("CROSS", 2)
        for _ in range(40):
            if r.rows[-1]["m1F0"] == 8:
                break
            r.step(1)
        if r.rows[-1]["m1F0"] == 8:
            break
        settle(r, 10)
    else:
        raise TimeoutError("no ledge climb; last " + summary(r.rows[-1]))
    r.until(in_control, 400)
    settle(r, 10)


def br_box_broken(row: dict, box: str) -> bool:
    return row["boxes"][box]["h"][8:10] not in ("00", "01", "04") or row["boxes"][box]["dmg36"] != 0


def br_melee(r: Route, box: str, stand: tuple, tries: int = 6) -> dict:
    """Walk to `stand`, face the box's centre and press Circle (the light
    melee, 001735C0) until the box takes damage (+0x36) or leaves its rest
    state; then wait out the attack and the break."""
    bx, _by, bz = r.rows[-1]["boxes"][box]["B0"]
    r.goto(*stand, tol=0.6, magnitude=0.5, stuck_ok=True)
    settle(r, 10)
    face(r, br_bearing(r.rows[-1], bx, bz), tol=0.06)
    f_hit = None
    presses = []
    for _ in range(tries):
        presses.append(r.frame_index)
        r.press("CIRCLE", 2)
        for _ in range(60):
            if br_box_broken(r.rows[-1], box):
                f_hit = r.frame_index
                break
            r.step(1)
        if f_hit is not None:
            break
        r.until(lambda row: in_control(row) and row["vit"]["p5"] == 0, 300)
        face(r, br_bearing(r.rows[-1], bx, bz), tol=0.06)
    else:
        raise TimeoutError(f"box {box} not hit; last {summary(r.rows[-1])}")
    r.set_pad(0)
    r.until(lambda row: in_control(row) and row["vit"]["p5"] == 0, 400)
    r.idle(120)
    return {"box": box, "presses": presses, "hit": f_hit, "end_header": r.rows[-1]["boxes"][box]["h"]}


def br_meta(r: Route, what: str, **extra) -> dict:
    last = r.rows[-1]
    return dict(extra, what=what,
                end={"pos": last["pos"], "yaw": last["yaw"], "taken11": last["taken11"], "inv": last["inv_br"],
                     "weap": last["weap"], "health": last["vit"]["health"], "elev_down": last["elev"]["down83A"],
                     "power": last["power"], "boxes": {n: v["h"] for n, v in last["boxes"].items()},
                     "items": {n: v["h"] for n, v in last["items"].items()}, "area": last["area4"]})


# -- the beats ---------------------------------------------------------------
WEST_LADDER_FOOT = (326.5, 216.5)       # foot node (320..331, 185, 209.6..221.1), face normal (0.956, -0.292)
WEST_LADDER_YAW = math.atan2(-0.956, 0.292)
WEST_LADDER_TOP = (316.0, 218.5)        # top node (311..322.6, 249.9, 212.4..224.2)
PLATEAU_LADDER_FOOT = (477.5, 403.0)    # foot node (471.8..481.6, 270.2, 398.4..407), normal (0.985, -0.174)
PLATEAU_LADDER_YAW = math.atan2(-0.985, 0.174)
PLATEAU_LADDER_TOP = (467.0, 404.5)     # top node (462.2..473, 355, 400.1..409)
CAGE_LADDER_A = (360.0, 293.5)          # route beat 10's ladder A foot, facing yaw pi


def br_beat_ledge_ammo(r: Route) -> dict:
    # From the 05_boxes end on the 220 ledge: pickup g0.3 (00219550, item
    # 0x1E, puid 6) at (213.6, 219.8, 311.9).
    use_br_sampler(r)
    br_pin(r, "br_00_ledge_ammo")
    r.idle(5)
    take = br_take(r, "g0.3", (216.5, 308.0))
    return br_meta(r, "take pickup g0.3 (item 0x1E) on the 220 ledge", marks=take)


def br_beat_map_item(r: Route) -> dict:
    # From the 06_hill_slide end on the low ground: the map item g0.6
    # (0015AFA0, class 0x87, item 0x08, puid 9) at (231.1, 200.9, 428.0),
    # 16 above the ground (the use scan's below-the-feet allowance is 20.5).
    use_br_sampler(r)
    br_pin(r, "br_01_map_item")
    r.idle(5)
    walk_path(r, [(255.0, 400.0), (238.0, 422.0)], tol=2.0)
    r.set_pad(0)
    take = br_take(r, "g0.6", (235.5, 424.5))
    return br_meta(r, "take the map item g0.6 (0015AFA0, item 0x08) south of the slide's foot", marks=take)


def br_beat_elevator_up(r: Route) -> dict:
    # From the 04_elevator_ride end on the lower floor (D_0081083A = 1, the
    # terminal's heights at 190): Cross at the terminal again runs the
    # powered script 0x82A750 and the carry back up; D_0081083A toggles to 0.
    use_br_sampler(r)
    br_pin(r, "br_02_elevator_up")
    r.idle(5)
    down0 = r.rows[-1]["elev"]["down83A"]
    face(r, -1.3037)
    f0 = r.frame_index
    for _ in range(4):
        r.press("CROSS", 2)
        for _ in range(40):
            if not in_control(r.rows[-1]):
                break
            r.step(1)
        if not in_control(r.rows[-1]):
            break
        settle(r, 10)
    else:
        raise TimeoutError("terminal did not start; last " + summary(r.rows[-1]))
    r.until(lambda row: row["elev"]["down83A"] != down0, 1500)
    r.until(in_control, 1500)
    settle(r, 30)
    return br_meta(r, "Cross at the terminal on the lower floor: the ride back up (D_0081083A 1 -> 0)",
                   marks={"press": f0, "toggle": next(x["f"] for x in r.rows if x["elev"]["down83A"] != down0)})


def br_beat_panel_decline(r: Route) -> dict:
    # From the 02_elevator_refusal end (battery in hand, power off): Cross at
    # the panel (script 0x2477A0, then the BATTERY page on its two-unit
    # prompt, default No), Cross on No, then Triangle until the status
    # closes: with the owner's +0x0A still 0, 00159210 runs the cancel
    # script 0x247DA0 and returns to its use-ready sub-state (+0 = 1,
    # +5 = 0); the power bit stays clear and the charge stays 12.
    use_br_sampler(r)
    br_pin(r, "br_03_panel_decline")
    r.idle(5)
    use_panel(r)
    f0 = r.frame_index
    r.until(lambda row: row["ui"][2:4] == "03" and row["ui"][8:10] == "05" and row["ui"][10:12] == "04", 900)
    f_prompt = r.frame_index
    r.idle(30)
    r.press("CROSS", 2)
    r.idle(60)
    f_no = r.frame_index
    for _ in range(8):
        if not br_status_open(r.rows[-1]):
            break
        r.press("TRIANGLE", 2)
        for _ in range(40):
            if not br_status_open(r.rows[-1]):
                break
            r.step(1)
    if br_status_open(r.rows[-1]):
        raise TimeoutError("status did not close; last " + summary(r.rows[-1]))
    r.until(in_control, 1500)
    settle(r, 30)
    if r.rows[-1]["power"] & 0x80:
        raise RuntimeError("power came on")
    return br_meta(r, "Cross at the power panel with the battery, No on the two-unit prompt, Triangle out: "
                      "the cancel script, power stays off",
                   marks={"left_control": f0, "prompt": f_prompt, "no_pressed": f_no})


def br_beat_crate_stack_break(r: Route) -> dict:
    # From the 04_elevator_ride end on the lower floor: light melee on box r5
    # (214.7, 189.8, 292.8), the support under r3: r5 breaks (damage break,
    # model 6 -> husk 0x22); every damage break wakes the raised r3 (+0x0A),
    # whose corner probes decide whether it holds, tips or falls.
    use_br_sampler(r)
    br_pin(r, "br_04_crate_stack_break")
    r.idle(5)
    walk_path(r, [(222.0, 270.0), (214.7, 282.5)], tol=1.0)
    r.set_pad(0)
    hit = br_melee(r, "r5", (214.7, 284.0))
    r.idle(180)
    return br_meta(r, "light melee on box r5 under r3 on the lower floor: r5 breaks, r3 wakes", marks=hit)


def br_beat_west_ladder_up(r: Route) -> dict:
    # From the 09_fence_door end (behind the fence, entry 2): west through
    # the yard north of the cage to the west-yard ladder (attribute-0x32
    # column x 316.9..325.6, z 213.5..220.2) and up to the 250 ledge.  The
    # yard north of the fence (z < 289.5) is reached only through the door.
    use_br_sampler(r)
    br_pin(r, "br_05_west_ladder_up")
    r.idle(5)
    # The corridor north of the cage (z 205..224) is closed at x 344.4..354.4
    # by a box 10 high: climb it from the east (Use, the ledge climb) and
    # run off its west side (a walk at half stick stops at the edge).
    walk_path(r, [(412.0, 240.0), (398.0, 215.0), (362.0, 214.0)], tol=2.0)
    r.set_pad(0)
    settle(r, 5)
    br_climb(r, -math.pi / 2)
    for _ in range(200):
        if r.rows[-1]["pos"][0] <= 332.0 and in_control(r.rows[-1]):
            break
        if r.rows[-1]["m1F0"] in (0, 1):
            r.stick_toward(326.0, 215.0, 1.0)
        else:
            r.set_pad(0)
        r.step(1)
    else:
        raise TimeoutError("did not get off the box; last " + summary(r.rows[-1]))
    r.set_pad(0)
    settle(r, 10)
    lad = br_ladder_up(r, WEST_LADDER_FOOT, WEST_LADDER_YAW)
    return br_meta(r, "west-yard ladder from the yard (185) to the 250 ledge", marks=lad)


def br_beat_ledge_crate_break(r: Route) -> dict:
    # From br_05's end on the 250 ledge: walk south to box r6 (311.4, 249.7,
    # 328.1) and break it with the light melee.
    use_br_sampler(r)
    br_pin(r, "br_06_ledge_crate_break")
    r.idle(5)
    walk_path(r, [(316.0, 240.0), (316.0, 300.0), (314.0, 316.0)], tol=2.0)
    r.set_pad(0)
    hit = br_melee(r, "r6", (312.0, 319.0))
    return br_meta(r, "light melee on box r6 on the 250 ledge: r6 breaks", marks=hit)


def br_beat_ledge_magazine(r: Route) -> dict:
    # From br_06's end: pickup g0.5 (00219550, item 0x10, puid 8) at
    # (311.6, 249.8, 328.7), where box r6 stood.
    use_br_sampler(r)
    br_pin(r, "br_07_ledge_magazine")
    r.idle(5)
    take = br_take(r, "g0.5", (312.0, 321.0))
    return br_meta(r, "take pickup g0.5 (item 0x10) where box r6 stood", marks=take)


def br_beat_west_ladder_down(r: Route) -> dict:
    # From br_07's end: back north to the ladder's top node and down to the
    # yard (the grab from above).
    use_br_sampler(r)
    br_pin(r, "br_08_west_ladder_down")
    r.idle(5)
    walk_path(r, [(314.0, 300.0), (316.0, 240.0), (316.0, 226.0)], tol=2.0)
    r.set_pad(0)
    lad = br_ladder_down(r, WEST_LADDER_TOP, WEST_LADDER_YAW + math.pi)
    return br_meta(r, "west-yard ladder from the 250 ledge down to the yard", marks=lad)


def br_beat_cage_key(r: Route) -> dict:
    # From the 08_truck_crossing end: route beat 10's ladder A to the cage
    # floor (225), then pickup g0.4 (00219550, model 2 wall mount, item
    # 0x32, puid 7) at (381.3, 225.3, 266.8) on the cage's east side.
    use_br_sampler(r)
    br_pin(r, "br_09_cage_key")
    r.idle(5)
    walk_path(r, [(360.0, 320.0), (360.0, 296.0)], tol=1.0)
    r.set_pad(0)
    settle(r, 5)
    lad = br_ladder_up(r, CAGE_LADDER_A, math.pi)
    walk_path(r, [(366.0, 272.0)], tol=1.0)
    r.set_pad(0)
    take = br_take(r, "g0.4", (375.0, 266.8))
    return br_meta(r, "cage ladder A to the cage floor, take pickup g0.4 (item 0x32)", marks=dict(take, ladder=lad))


def br_beat_yard_ammo(r: Route) -> dict:
    # From the 09_fence_door end (behind the fence, entry 2): across the yard
    # (185) to pickup g0.1 (00219550, item 0x1E, puid 4) at (467.3, 184.8,
    # 227.4), south of the north block.
    use_br_sampler(r)
    br_pin(r, "br_10_yard_ammo")
    r.idle(5)
    walk_path(r, [(440.0, 255.0), (458.0, 235.0)], tol=2.0)
    r.set_pad(0)
    take = br_take(r, "g0.1", (462.0, 231.0))
    return br_meta(r, "take pickup g0.1 (item 0x1E) on the yard floor", marks=take)


def br_beat_plateau_ladder_up(r: Route) -> dict:
    # From the 11_crevice_prompt end on the pipe end (279.9): down onto the
    # 270 plateau, south over the pipe to the plateau ladder (column
    # x 467.6..476.7, z 400..407) and up to the 355 top.
    use_br_sampler(r)
    br_pin(r, "br_11_plateau_ladder_up")
    r.idle(5)
    # The plateau is crossed by a raised pipe (279.1, x 445..495, its north
    # face's normal (0.647, -0.762)): climb it facing south-east (the ledge
    # climb), run off its south side, then on to the ladder's foot east of
    # the column the ladder is fixed to (x 462..473, z 394..418).
    walk_path(r, [(480.0, 300.0), (481.0, 330.0), (484.0, 352.0)], tol=2.0)
    r.set_pad(0)
    settle(r, 5)
    br_climb(r, math.atan2(-0.647, 0.762))
    for _ in range(200):
        row = r.rows[-1]
        if row["pos"][1] < 275.0 and in_control(row):
            break
        if row["m1F0"] in (0, 1):
            r.stick_toward(484.0, 384.0, 1.0)
        else:
            r.set_pad(0)
        r.step(1)
    else:
        raise TimeoutError("did not get off the pipe; last " + summary(r.rows[-1]))
    r.set_pad(0)
    settle(r, 10)
    walk_path(r, [(486.0, 388.0), (481.0, 399.0)], tol=2.0)
    r.set_pad(0)
    lad = br_ladder_up(r, PLATEAU_LADDER_FOOT, PLATEAU_LADDER_YAW)
    return br_meta(r, "plateau ladder from the 270 plateau to the 355 top", marks=lad)


def br_beat_tower_ammo(r: Route) -> dict:
    # From br_11's end on the 355 top: pickup g0.2 (00219550, item 0x1F,
    # puid 5) at (431.9, 354.8, 411.8).
    use_br_sampler(r)
    br_pin(r, "br_12_tower_ammo")
    r.idle(5)
    # A block (x 434..457, z 397..420) stands in the middle of the top:
    # round it by the south side.
    walk_path(r, [(459.5, 393.0), (445.0, 392.0), (432.0, 394.5)], tol=1.5)
    r.set_pad(0)
    take = br_take(r, "g0.2", (430.5, 404.5))
    return br_meta(r, "take pickup g0.2 (item 0x1F) on the 355 top", marks=take)


def br_beat_plateau_ladder_down(r: Route) -> dict:
    # From br_12's end: back to the ladder's top node and down to the
    # plateau (the grab from above).
    use_br_sampler(r)
    br_pin(r, "br_13_plateau_ladder_down")
    r.idle(5)
    walk_path(r, [(432.0, 394.5), (445.0, 392.0), (459.5, 393.0), (461.0, 404.5)], tol=1.5)
    r.set_pad(0)
    lad = br_ladder_down(r, PLATEAU_LADDER_TOP, PLATEAU_LADDER_YAW + math.pi)
    return br_meta(r, "plateau ladder from the 355 top down to the plateau", marks=lad)


def br_beat_roger_talk(r: Route) -> dict:
    # From the 14_roger_encounter end on the west tower top (D_008107D8 = 1):
    # Roger r8's third branch (0x823B70) starts its talk script 0x828810
    # when the use scan marks it (+0x0B & 4); the script's end clears +0x0B.
    use_br_sampler(r)
    br_pin(r, "br_14_roger_talk")
    r.idle(5)
    rx, _ry, rz = r.rows[-1]["roger_r8"]["pos"]
    face(r, br_bearing(r.rows[-1], rx, rz), tol=0.06)
    f0 = None
    for _ in range(4):
        f0 = r.frame_index
        r.press("CROSS", 2)
        for _ in range(40):
            if not in_control(r.rows[-1]):
                break
            r.step(1)
        if not in_control(r.rows[-1]):
            break
        px, _py, pz = r.rows[-1]["pos"]
        r.goto(px + (rx - px) * 0.3, pz + (rz - pz) * 0.3, tol=0.5, magnitude=0.4, stuck_ok=True)
        settle(r, 5)
        face(r, br_bearing(r.rows[-1], rx, rz), tol=0.06)
    else:
        raise TimeoutError("Roger's talk did not start; last " + summary(r.rows[-1]))
    r.until(in_control, 6000)
    settle(r, 60)
    return br_meta(r, "Cross at Roger after the encounter: his talk script 0x828810", marks={"press": f0})


BR_BEATS = [
    ("br_00_ledge_ammo", "05_boxes", br_beat_ledge_ammo),
    ("br_01_map_item", "06_hill_slide", br_beat_map_item),
    ("br_02_elevator_up", "04_elevator_ride", br_beat_elevator_up),
    ("br_03_panel_decline", "02_elevator_refusal", br_beat_panel_decline),
    ("br_04_crate_stack_break", "04_elevator_ride", br_beat_crate_stack_break),
    ("br_05_west_ladder_up", "09_fence_door", br_beat_west_ladder_up),
    ("br_06_ledge_crate_break", "br_05_west_ladder_up", br_beat_ledge_crate_break),
    ("br_07_ledge_magazine", "br_06_ledge_crate_break", br_beat_ledge_magazine),
    ("br_08_west_ladder_down", "br_07_ledge_magazine", br_beat_west_ladder_down),
    ("br_09_cage_key", "08_truck_crossing", br_beat_cage_key),
    ("br_10_yard_ammo", "09_fence_door", br_beat_yard_ammo),
    ("br_11_plateau_ladder_up", "11_crevice_prompt", br_beat_plateau_ladder_up),
    ("br_12_tower_ammo", "br_11_plateau_ladder_up", br_beat_tower_ammo),
    ("br_13_plateau_ladder_down", "br_12_tower_ammo", br_beat_plateau_ladder_down),
    ("br_14_roger_talk", "14_roger_encounter", br_beat_roger_talk),
]


def br_selected(spec: str) -> list[tuple]:
    """`br` = every BRANCH beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "br" in wanted:
        return list(BR_BEATS)
    return [b for b in BR_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def br_event_keys(row: dict) -> dict:
    """The branch fields events() adds for BRANCH rows."""
    return {"taken11": row["taken11"][:4], "inv": ",".join(f"{k}:{v}" for k, v in row["inv_br"].items() if v),
            "weap": f"{row['weap']['mag']}/{row['weap']['reserve']}",
            "items": ",".join(f"{n}:{v['h'][8:10]}" for n, v in row["items"].items()),
            "boxes": ",".join(f"{n}:{v['h'][:2]}/{v['h'][8:10]}/{v['dmg36']}" for n, v in row["boxes"].items()),
            "elev83A": row["elev"]["down83A"], "roger": row["roger"]["a"][8:12] + "/" + row["roger"]["a"][22:24],
            "roger_script": row["roger"]["script"], "uirec": row["ui_rec"][:10]}


# ---------------------------------------------------------------------------
# OPTIONS capture group (C10 lane OPTIONS, opt-in, `--beats opt` or a beat's
# name): the in-game options screen of AREA11 and its memory-card paths.
# SELECT in gameplay makes the classifier 001AE7E0 return 1, the frame machine
# (gameplay task 0x28A750, +0xB frame state) goes to state 2 and runs the
# options screen 0022A650 every frame (world frozen) until it returns: +0xC is
# its state, +0x1C its cursor over the nine rows of D_002672E0 (exit,
# vibration, sound, screen position, brightness, button config, load,
# default, quit game).  The settings live in D_00810118 (+0 button type, +1
# vibration, +3 the default prompt, +4 sound, +8/+0xA the saved screen
# offset); the screen offset itself is 0x70003B94 / 0x70003B96 and the button
# type's action masks are 0x70003B74..0x70003B82.  The status screen (START)
# has no options page: its hub reaches DATABASE, SPR4, MAP and ITEM only.
# Every beat starts from the 08_truck_crossing snapshot, pins its first input
# to a main-loop counter, drives closed loop with pad input only (each press
# repeated until its effect shows) and writes build/c10/options/<beat>/.
# No beat writes a memory card: the LOAD row's screen (00225AC0 in load mode)
# is left at its slot choice.  Described in docs/CAPTURES_C10.md (section
# OPTIONS).
OUT_OPT = ROOT / "build/c10/options"
OPT_TASK = 0x28A750                     # the gameplay task slot (001ACEC0)
OPT_ROWS = ("exit", "vibration", "sound", "screen_position", "brightness", "button_config",
            "load", "default", "quit_game")       # cursor 0..8 (D_002672E0 = 7, 0, 1, 2, 3, 4, 9, 5, 8)
OPT_SPANS = DMG_SPANS + [
    ("opt118", 0x810118, 0x10),         # D_00810118 options record
    ("mc040", 0x810040, 0xD4),          # D_00810040 memory-card screen record (+0 state, +1 sub, +0x14 mode)
    ("msgblk", 0x2821B0, 0xB0),         # D_002821B0..0x28225F (message machine, D_00282240 layout base)
    ("snd150", 0x282150, 0x10),         # 0x282150..5F (D_00282157 loader gate, D_0028215B)
    ("task0x", OPT_TASK, 0x30),         # the gameplay task slot, whole
]


class OptSampler(AimSampler):
    """AimSampler's pool logic over OPT_SPANS."""

    def __init__(self, session: OriginalSession):
        self.s = session
        self.spans = OPT_SPANS
        self.body = b"".join(struct.pack("<BI", 2, a + i)
                             for _n, a, n in self.spans for i in range(0, n, 4))
        self.pool0 = None


def decode_opt(r: dict[str, bytes]) -> dict:
    row = decode_dmg(r)
    o, t, sp, pb, mc = r["opt118"], r["task0x"], r["spad_hi"], r["padblk"], r["mc040"]
    row["opt"] = 1
    row["opt118"] = o.hex()
    row["task0x"] = t.hex()
    row["mc040"] = mc.hex()
    row["msgblk"] = r["msgblk"].hex()
    row["snd150"] = r["snd150"].hex()
    row["ov"] = {
        "type": o[0], "vib": o[1], "dflt": o[3], "sound": o[4], "pos8": _s16(o, 8), "posA": _s16(o, 0xA),
        "fn": struct.unpack_from("<I", t, 4)[0], "s8": t[8], "s9": t[9], "fstate": t[0xB], "mstate": t[0xC],
        "msub": t[0xD], "m12": t[0x12], "m13": t[0x13], "cursor": struct.unpack_from("<H", t, 0x1C)[0],
        "timer": struct.unpack_from("<H", t, 0x1E)[0], "c4": r["glob"][0xC4],
        "x3B94": _s16(sp, 0x54), "y3B96": _s16(sp, 0x56), "b3B90": sp[0x50], "b3B93": sp[0x53],
        "masks": sp[0x34:0x44].hex(), "e6a": pb[0x2A], "e50": pb[0x10],
        "rep": hex(struct.unpack_from("<H", pb, 0x38)[0]),
        "mc_state": mc[0], "mc_sub": mc[1], "mc_mode": mc[0x14], "mc_result": mc[0x16],
        "b15B": r["snd150"][0xB], "bd8": r["bd8"][0],
    }
    return row


def use_opt_sampler(r: Route) -> None:
    sampler = OptSampler(r.s)
    r.sampler = sampler
    r.now = lambda: decode_opt(sampler.raw())
    r.rows[0] = dict(r.now(), f=0)
    r.rows[0]["pool0"] = {k: v for k, v in sampler.pool0.items() if int(v, 16)}
    r.s.boundary_timeout = 120.0


OPT_PIN_AFTER = 30     # frames after the source snapshot's main-loop counter (as DAMAGE, BRANCH)


def opt_pin(r: Route, beat: str) -> None:
    """Pin the beat's first input to the source snapshot's counter + 30 (the
    loaded state runs freely until Pine answers; see br_pin)."""
    source = next(b[1] for b in OPT_BEATS if b[0] == beat)
    snap = json.loads((beat_dir(source) / "snapshot.json").read_text())
    pin = snap["main_loop_counter"] + OPT_PIN_AFTER
    if r.rows[-1]["counter"] > pin:
        raise RuntimeError(f"start counter {r.rows[-1]['counter']} is past the pin {pin}")
    r.set_pad(0)
    r.until(lambda row: row["counter"] >= pin, 400)


def opt_menu(row: dict, state: int | None = None) -> bool:
    """The options screen runs (frame state 2); `state` = its +0xC state."""
    v = row["ov"]
    return v["fn"] == 0x1ACEC0 and v["fstate"] == 2 and (state is None or v["mstate"] == state)


def opt_press(r: Route, button: str, pred, wait: int = 30, tries: int = 6) -> int:
    """Tap `button` (2 frames) until `pred` holds, waiting up to `wait` frames
    after each tap (a press that lands while a screen is busy is ignored).
    Returns the frame index of the press that took effect."""
    for _ in range(tries):
        f = r.frame_index
        r.press(button, 2)
        for _ in range(wait):
            if pred(r.rows[-1]):
                return f
            r.step(1)
        if pred(r.rows[-1]):
            return f
    raise TimeoutError(f"{button}: no effect after {tries} presses; last {summary(r.rows[-1])} "
                       f"ov={r.rows[-1]['ov']}")


def opt_open(r: Route, marks: dict, key: str = "open") -> None:
    """SELECT in gameplay: the options screen in its browse state (1)."""
    settle(r, 10)
    marks.setdefault(key, []).append(opt_press(r, "SELECT", lambda row: opt_menu(row, 1), wait=40))
    r.idle(10)


def opt_cursor_to(r: Route, index: int, marks: dict) -> None:
    """Move the cursor with Down (or Up when it is shorter) to row `index`."""
    while r.rows[-1]["ov"]["cursor"] != index:
        cur = r.rows[-1]["ov"]["cursor"]
        button = "DOWN" if (index - cur) % 9 <= (cur - index) % 9 else "UP"
        nxt = (cur + (1 if button == "DOWN" else -1)) % 9
        marks.setdefault("cursor", []).append(
            [opt_press(r, button, lambda row, n=nxt: row["ov"]["cursor"] == n and opt_menu(row, 1)), nxt])
        r.idle(8)


def opt_close(r: Route, button: str, marks: dict) -> None:
    """Close the options screen with `button` (Cross on the exit row, Circle,
    Triangle or SELECT) and wait for control."""
    marks.setdefault("close", []).append([opt_press(r, button, lambda row: not opt_menu(row), wait=60), button])
    r.until(in_control, 600)
    settle(r, 20)


def opt_meta(r: Route, what: str, marks: dict, **extra) -> dict:
    last = r.rows[-1]["ov"]
    return dict(extra, what=what, marks=marks,
                end={"opt118": r.rows[-1]["opt118"], "x3B94": last["x3B94"], "y3B96": last["y3B96"],
                     "masks": last["masks"], "b15B": last["b15B"], "fstate": last["fstate"],
                     "mstate": last["mstate"], "c4": last["c4"], "pos": r.rows[-1]["pos"],
                     "health": r.rows[-1]["vit"]["health"], "area": r.rows[-1]["area4"]})


def opt_toggle(r: Route, row_index: int, field: str, marks: dict) -> None:
    """Rows 1 (vibration, +1) and 2 (sound, +4): Cross enters 00201720 (state
    5), Right flips the field, Cross keeps it (back to state 1)."""
    opt_cursor_to(r, row_index, marks)
    marks.setdefault("enter", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 5)))
    r.idle(10)
    before = r.rows[-1]["ov"][field]
    marks.setdefault("right", []).append(opt_press(r, "RIGHT", lambda row: row["ov"][field] != before))
    r.idle(20)
    marks.setdefault("keep", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 1)))
    r.idle(20)


def opt_beat_browse_close(r: Route) -> dict:
    # Open, browse every row down (the cursor wraps from quit game to exit)
    # and back up, close with Cross on the exit row; then open and close once
    # each with Circle, Triangle and SELECT (0022A650 state 1: hit, 0x20,
    # 0x10, 0x100 all go to state 12).
    use_opt_sampler(r)
    opt_pin(r, "opt_00_browse_close")
    marks: dict = {}
    opt_open(r, marks)
    for i in range(1, 10):
        opt_cursor_to(r, i % 9, marks)
    for nxt in range(8, -1, -1):           # Up from the exit row wraps to quit game
        marks.setdefault("cursor", []).append(
            [opt_press(r, "UP", lambda row, n=nxt: row["ov"]["cursor"] == n and opt_menu(row, 1)), nxt])
        r.idle(8)
    opt_close(r, "CROSS", marks)
    for button in ("CIRCLE", "TRIANGLE", "SELECT"):
        opt_open(r, marks)
        opt_close(r, button, marks)
    return opt_meta(r, "SELECT opens the options screen; every row browsed down (wrap) and up; "
                       "closed by Cross on the exit row, Circle, Triangle and SELECT", marks)


def opt_beat_vibration(r: Route) -> dict:
    use_opt_sampler(r)
    opt_pin(r, "opt_01_vibration")
    marks: dict = {}
    opt_open(r, marks)
    opt_toggle(r, 1, "vib", marks)          # on -> off
    opt_toggle(r, 1, "vib", marks)          # off -> on
    opt_cursor_to(r, 0, marks)
    opt_close(r, "CROSS", marks)
    return opt_meta(r, "vibration row: Cross, Right (off), Cross; again Right (on), Cross; closed", marks)


def opt_beat_sound(r: Route) -> dict:
    use_opt_sampler(r)
    opt_pin(r, "opt_02_sound")
    marks: dict = {}
    opt_open(r, marks)
    opt_toggle(r, 2, "sound", marks)        # stereo -> mono
    opt_toggle(r, 2, "sound", marks)        # mono -> stereo
    opt_cursor_to(r, 0, marks)
    opt_close(r, "CROSS", marks)
    return opt_meta(r, "sound row: Cross, Right (the other mode), Cross; again Right (back), Cross; closed",
                    marks)


def opt_enter_module(r: Route, row_index: int, state: int, marks: dict) -> None:
    """Rows 3..5: Cross goes through state 10 (0022A590 loads screen module
    0x2B) to the row's screen (states 7, 8, 9)."""
    opt_cursor_to(r, row_index, marks)
    marks.setdefault("enter", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row) and row["ov"]["mstate"]
                                                   in (10, state), wait=20))
    r.until(lambda row: opt_menu(row, state), 600)
    marks.setdefault("screen", []).append([r.frame_index, state])
    r.idle(20)


def opt_screen_moves(r: Route, moves: list[tuple[str, int]], marks: dict) -> None:
    """Screen position (00201F70): each tap moves 0x70003B94 / 0x70003B96 by one."""
    for button, n in moves:
        for _ in range(n):
            before = (r.rows[-1]["ov"]["x3B94"], r.rows[-1]["ov"]["y3B96"])
            marks.setdefault("move", []).append(
                [opt_press(r, button, lambda row, b=before: (row["ov"]["x3B94"], row["ov"]["y3B96"]) != b), button])
            r.idle(6)


def opt_beat_screen_position(r: Route) -> dict:
    # Cross on the screen-position row (module 0x2B, 00201F70): Up 3, Left 2,
    # Cross keeps the new offset; again: Down 3, Right 2, Cross (back to the
    # start); again: Up 2, then Circle restores the offset it was entered
    # with.  Closed by Cross on the exit row.
    use_opt_sampler(r)
    opt_pin(r, "opt_03_screen_position")
    marks: dict = {}
    opt_open(r, marks)
    start = (r.rows[-1]["ov"]["x3B94"], r.rows[-1]["ov"]["y3B96"])
    opt_enter_module(r, 3, 7, marks)
    opt_screen_moves(r, [("UP", 3), ("LEFT", 2)], marks)
    marks.setdefault("keep", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 1), wait=60))
    r.idle(20)
    changed = (r.rows[-1]["ov"]["x3B94"], r.rows[-1]["ov"]["y3B96"])
    opt_enter_module(r, 3, 7, marks)
    opt_screen_moves(r, [("DOWN", 3), ("RIGHT", 2)], marks)
    marks.setdefault("keep", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 1), wait=60))
    r.idle(20)
    opt_enter_module(r, 3, 7, marks)
    opt_screen_moves(r, [("UP", 2)], marks)
    marks.setdefault("back", []).append(opt_press(r, "CIRCLE", lambda row: opt_menu(row, 1), wait=60))
    r.idle(20)
    opt_cursor_to(r, 0, marks)
    opt_close(r, "CROSS", marks)
    return opt_meta(r, "screen position: Up 3 / Left 2 kept, then Down 3 / Right 2 kept (back), then Up 2 "
                       "cancelled with Circle; closed", marks, start_offset=start, changed_offset=changed)


def opt_beat_brightness(r: Route) -> dict:
    # Cross on the brightness row (module 0x2B, 00202BA0): a still screen
    # with no setting.  Opened three times: Cross and Circle return to the
    # list (00202BA0 returns 1, state 2); Triangle returns 2, state 11 and
    # then 12: the options screen closes.
    use_opt_sampler(r)
    opt_pin(r, "opt_04_brightness")
    marks: dict = {}
    opt_open(r, marks)
    for button in ("CROSS", "CIRCLE"):
        opt_enter_module(r, 4, 8, marks)
        r.idle(30)
        marks.setdefault("back", []).append([opt_press(r, button, lambda row: opt_menu(row, 1), wait=60), button])
        r.idle(20)
    opt_enter_module(r, 4, 8, marks)
    r.idle(30)
    marks.setdefault("close", []).append([opt_press(r, "TRIANGLE", lambda row: not opt_menu(row), wait=60),
                                          "TRIANGLE"])
    r.until(in_control, 600)
    settle(r, 20)
    return opt_meta(r, "brightness: the still screen, left with Cross and with Circle (the list) and "
                       "with Triangle (the options screen closes)", marks)


def opt_button_type(r: Route, to: int, marks: dict) -> None:
    """Button config (00202D10): Right / Left move the type cursor (+0 of
    D_00810118 holds it), Cross commits through 001AF470 (the action masks)."""
    opt_enter_module(r, 5, 9, marks)
    while r.rows[-1]["ov"]["type"] != to:
        cur = r.rows[-1]["ov"]["type"]
        button = "RIGHT" if to > cur else "LEFT"
        marks.setdefault("move", []).append(
            [opt_press(r, button, lambda row, c=cur: row["ov"]["type"] != c), button])
        r.idle(10)
    masks = r.rows[-1]["ov"]["masks"]
    marks.setdefault("keep", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 1), wait=60))
    r.idle(20)
    marks.setdefault("masks", []).append([to, masks, r.rows[-1]["ov"]["masks"]])


def opt_beat_button_config(r: Route) -> dict:
    # Types A -> B (kept), B -> C (kept), C -> A (kept); each Cross commits
    # the type's action masks.  Closed by Cross on the exit row.
    use_opt_sampler(r)
    opt_pin(r, "opt_05_button_config")
    marks: dict = {}
    opt_open(r, marks)
    for to in (1, 2, 0):
        opt_button_type(r, to, marks)
    opt_cursor_to(r, 0, marks)
    opt_close(r, "CROSS", marks)
    return opt_meta(r, "button config: type A -> B, B -> C, C -> A, each kept with Cross; closed", marks)


def opt_beat_default(r: Route) -> dict:
    # Vibration off first (row 1).  Default row (00201C50, state 6): Cross,
    # Cross with the prompt on No (nothing changes); Cross, Right (Yes),
    # Cross: the defaults (vibration on for this pad, sound +4 = 0, type A,
    # screen offset 0) are written.  Closed by Cross on the exit row.
    use_opt_sampler(r)
    opt_pin(r, "opt_06_default")
    marks: dict = {}
    opt_open(r, marks)
    opt_toggle(r, 1, "vib", marks)
    opt_cursor_to(r, 7, marks)
    marks.setdefault("enter", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 6)))
    r.idle(20)
    marks.setdefault("no", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 1)))
    r.idle(20)
    marks.setdefault("enter", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 6)))
    r.idle(10)
    marks.setdefault("right", []).append(opt_press(r, "RIGHT", lambda row: row["ov"]["dflt"] == 1))
    r.idle(20)
    marks.setdefault("yes", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 1)))
    r.idle(20)
    opt_cursor_to(r, 0, marks)
    opt_close(r, "CROSS", marks)
    return opt_meta(r, "vibration off; default row: No (nothing), then Yes (the defaults restore "
                       "vibration on); closed", marks)


def opt_beat_load_cancel(r: Route) -> dict:
    # Load row: 001AF6F0 clears D_00810040, state 3 runs 00225AC0(0) (load
    # mode, +0x14 = 2): the sound stops, screen module 0x2A loads, the load
    # screen fades in at its memory-card slot choice.  Triangle leaves it
    # (00225AC0 returns 1): state 2, 00200970(1), back to the list.  No slot
    # is chosen; nothing is read beyond the card check and nothing written.
    use_opt_sampler(r)
    opt_pin(r, "opt_07_load_cancel")
    marks: dict = {}
    opt_open(r, marks)
    opt_cursor_to(r, 6, marks)
    marks.setdefault("enter", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 3)))
    r.until(lambda row: row["ov"]["mc_state"] == 1 and dmg_fade(row) == 0, 900)
    marks["slot_choice"] = r.frame_index
    r.idle(60)
    marks.setdefault("back", []).append(opt_press(r, "TRIANGLE", lambda row: row["ov"]["mc_state"] >= 2,
                                                  wait=40))
    r.until(lambda row: opt_menu(row, 1) and dmg_fade(row) == 0, 900)
    marks["list_again"] = r.frame_index
    r.idle(30)
    opt_cursor_to(r, 0, marks)
    opt_close(r, "CROSS", marks)
    return opt_meta(r, "load row: the load screen up to its memory-card slot choice, left with Triangle "
                       "(no slot chosen); closed", marks)


def opt_beat_quit_cancel(r: Route) -> dict:
    # Quit-game row: state 4 runs the yes/no prompt 0022B420 (its choice is
    # task +0x13, 0 = No, the default; Right flips it).  Right (Yes), Right
    # (No), Cross on No: 0022B420 returns 1, back to the list.  Again,
    # Circle: returns 1, the list.  Again, Triangle: returns 2, state 12, the
    # options screen closes (the original sets 2 when 0x10 is pressed; the
    # NEARMISS C of 0022B420 has the two swapped).  Cross on Yes (0022B420 returns 3, the frame
    # machine calls 001AD140, the game-over wait toward the title menu) is
    # not taken.
    use_opt_sampler(r)
    opt_pin(r, "opt_08_quit_cancel")
    marks: dict = {}
    opt_open(r, marks)
    opt_cursor_to(r, 8, marks)
    marks.setdefault("enter", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 4)))
    r.idle(30)
    for want in (1, 0):
        marks.setdefault("right", []).append(
            [opt_press(r, "RIGHT", lambda row, w=want: row["ov"]["m13"] == w and row["ov"]["msub"] == 1,
                       wait=40), want])
        r.idle(20)
    if r.rows[-1]["ov"]["m13"] != 0:
        raise RuntimeError("the quit prompt is not on No")
    marks.setdefault("no", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 1), wait=60))
    r.idle(20)
    marks.setdefault("enter", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 4)))
    r.idle(30)
    marks.setdefault("back", []).append([opt_press(r, "CIRCLE", lambda row: opt_menu(row, 1), wait=60),
                                         "CIRCLE"])
    r.idle(20)
    marks.setdefault("enter", []).append(opt_press(r, "CROSS", lambda row: opt_menu(row, 4)))
    r.idle(30)
    marks.setdefault("close", []).append([opt_press(r, "TRIANGLE", lambda row: not opt_menu(row), wait=60),
                                          "TRIANGLE"])
    r.until(in_control, 600)
    settle(r, 20)
    return opt_meta(r, "quit-game row: its yes/no prompt, Right to Yes and back to No, Cross on No (the "
                       "list); Circle (the list); Triangle (the options screen closes)", marks)


OPT_BEATS = [
    ("opt_00_browse_close", "08_truck_crossing", opt_beat_browse_close),
    ("opt_01_vibration", "08_truck_crossing", opt_beat_vibration),
    ("opt_02_sound", "08_truck_crossing", opt_beat_sound),
    ("opt_03_screen_position", "08_truck_crossing", opt_beat_screen_position),
    ("opt_04_brightness", "08_truck_crossing", opt_beat_brightness),
    ("opt_05_button_config", "08_truck_crossing", opt_beat_button_config),
    ("opt_06_default", "08_truck_crossing", opt_beat_default),
    ("opt_07_load_cancel", "08_truck_crossing", opt_beat_load_cancel),
    ("opt_08_quit_cancel", "08_truck_crossing", opt_beat_quit_cancel),
]


def opt_selected(spec: str) -> list[tuple]:
    """`opt` = every OPTIONS beat in order; otherwise names or name prefixes."""
    wanted = spec.split(",")
    if "opt" in wanted:
        return list(OPT_BEATS)
    return [b for b in OPT_BEATS if any(b[0] == w or b[0].startswith(w + "_") for w in wanted)]


def opt_event_keys(row: dict) -> dict:
    """The options fields events() adds for OPTIONS rows."""
    v = row["ov"]
    return {"opt118": row["opt118"][:10], "pos": f"{v['pos8']}/{v['posA']}", "off": f"{v['x3B94']}/{v['y3B96']}",
            "frame": f"{v['fstate']}/{v['mstate']}/{v['msub']}", "cursor": v["cursor"], "m13": v["m13"],
            "c4": v["c4"], "masks": v["masks"], "mc": f"{v['mc_state']}/{v['mc_sub']}/{v['mc_mode']}/{v['mc_result']}",
            "15B": v["b15B"], "bd8": v["bd8"], "e6a": v["e6a"]}


def beat_source(source: str) -> Path:
    if len(source) == 2 and source.isdigit():
        return slot_path(source)
    return resumable(beat_dir(source) / "state.p2s")


def beat_dir(name: str) -> Path:
    """Output folder of a beat (see _legacy_beat_dir); in fork mode the same
    path under build/fork_refs/ (phase generation) or
    build/startup-reference/fork-states/beats/ (base generation)."""
    d = _legacy_beat_dir(name)
    if FORK:
        rel = d.relative_to(ROOT / "build")
        return (FORK_REFS if GENERATION == "phase" else FORK_STATES / "beats") / rel
    return d


# ---------------------------------------------------------------------------
# Phase lock (fork, phase generation): reproduce the v2.6.3 capture's timeline.
#
# A legacy session loaded its state, ran free until PINE answered, then
# paused: row 0 of every v2.6.3 beat lies 1..12 frames after its source state
# (the trace's first_counter against the source's counter).  The phase-locked
# fork states sit on the same game point with the same frame index and field
# as the v2.6.3 states, so replaying that lead-in with a neutral pad puts
# every fork row on the v2.6.3 row's tick, frame index (= the main-loop
# counter's parity) and field.  The v2.6.3 retry tail (tail_idle_frames) is
# replayed too, so the end snapshot, and the next beat's source, stay in step.

def _legacy_counter(path: Path) -> int | None:
    """Main-loop counter of a v2.6.3 state or beat folder (offline)."""
    from parse_pcsx2_state import extract_zstd_entry
    folder = path.parent
    try:
        if path.name == "state.p2s" and (folder / "snapshot.json").exists():
            return json.loads((folder / "snapshot.json").read_text())["main_loop_counter"]
        return struct.unpack_from("<I", extract_zstd_entry(path, "Scratchpad.bin"), 0x3B64)[0]
    except (OSError, KeyError, ValueError):
        return None


def _legacy_phase_at(path: Path) -> dict | None:
    """Frame index / field / counter of a v2.6.3 state or beat snapshot (offline)."""
    from parse_pcsx2_state import extract_zstd_entry
    try:
        if path.name == "state.p2s" and (path.parent / "eeMemory.bin").exists():
            ee = (path.parent / "eeMemory.bin").read_bytes()
            sp = (path.parent / "scratchpad.bin").read_bytes()
        else:
            ee = extract_zstd_entry(path, "eeMemory.bin")
            sp = extract_zstd_entry(path, "Scratchpad.bin")
    except (OSError, KeyError, ValueError):
        return None
    return {"frame_index": ee[FRAME_INDEX], "field": ee[FRAME_INDEX + 8],
            "counter": struct.unpack_from("<I", sp, 0x3B64)[0]}


def legacy_source_state(source: str) -> Path:
    if len(source) == 2 and source.isdigit():
        return SSTATES / f"{SERIAL}.{source}.p2s"
    return _legacy_beat_dir(source) / "state.p2s"


# The v2.6.3 references are cached in build/fork_refs/legacy_refs.json, so the
# phase lock keeps working once the user's v2.6.3 slots go to the Trash with
# the app (`route_capture.py legacy-refs` fills the cache for every group).
LEGACY_REFS = FORK_REFS / "legacy_refs.json"


def _legacy_cache() -> dict:
    try:
        return json.loads(LEGACY_REFS.read_text())
    except (OSError, ValueError):
        return {}


def _legacy_cache_put(section: str, key: str, value: dict) -> None:
    c = _legacy_cache()
    if c.get(section, {}).get(key) == value:
        return
    c.setdefault(section, {})[key] = value
    c["what"] = ("v2.6.3 capture facts the fork's phase lock needs (lead-in, tail, row counters, "
                 "phase at the source and end states), cached before the v2.6.3 slots are retired")
    _write_json_atomic(LEGACY_REFS, c)


def legacy_slot_phase(slot: str) -> dict | None:
    """Frame index / field / counter of a v2.6.3 user slot (cached)."""
    path = SSTATES / f"{SERIAL}.{slot}.p2s"
    if path.exists():
        ph = _legacy_phase_at(path)
        if ph is not None:
            _legacy_cache_put("slots", slot, ph)
            return ph
    return _legacy_cache().get("slots", {}).get(slot)


def legacy_reference(name: str, source: str) -> dict | None:
    """The v2.6.3 capture of a beat: lead-in, tail and the phase rule of its
    rows (frame index = counter parity; field = frame index ^ the pairing at
    its source state, valid while no iteration takes two vsyncs).  Served
    from build/fork_refs/legacy_refs.json when the v2.6.3 files are gone."""
    ref = _legacy_reference(name, source)
    if ref is not None:
        _legacy_cache_put("beats", name, ref)
        return ref
    return _legacy_cache().get("beats", {}).get(name)


def _legacy_reference(name: str, source: str) -> dict | None:
    trace = _legacy_beat_dir(name) / "trace.json"
    src = legacy_source_state(source)
    if not trace.exists() or not src.exists():
        return None
    doc = json.loads(trace.read_text())
    src_counter = _legacy_counter(src)
    if src_counter is None:
        return None
    ph_src = _legacy_phase_at(src)
    ph_end = _legacy_phase_at(_legacy_beat_dir(name) / "state.p2s")
    ref = {"trace": str(trace.relative_to(ROOT)), "source_state": str(src.relative_to(ROOT)),
           "source_counter": src_counter, "first_counter": doc["first_counter"],
           "last_counter": doc["last_counter"], "frames": doc["frames"],
           "lead_in": doc["first_counter"] - src_counter, "tail": doc.get("tail_idle_frames") or 0,
           "source_phase": ph_src, "end_phase": ph_end,
           "counters": [r["counter"] for r in doc["rows"]]}
    if ph_src is not None:
        ref["pairing_source"] = ph_src["frame_index"] ^ ph_src["field"]
    if ph_end is not None:
        ref["pairing_end"] = ph_end["frame_index"] ^ ph_end["field"]
    return ref


def legacy_row_phase(ref: dict, i: int) -> tuple[int, int | None]:
    """(frame index, field or None) of v2.6.3 row i, derived from its counter."""
    fi = ref["counters"][i] & 1
    pairing = ref.get("pairing_source")
    if pairing is None or pairing != ref.get("pairing_end"):
        return fi, None                 # the pairing changed inside the beat: field unknown per row
    return fi, fi ^ pairing


def phase_report(rows: list[dict], ref: dict | None) -> dict:
    """Per beat: the fork rows' phase against the v2.6.3 rows' (by row index)."""
    out = {"row0": {"fi": rows[0].get("fi"), "fld": rows[0].get("fld"), "vs": rows[0].get("vs"),
                    "counter": rows[0]["counter"]},
           "last": {"fi": rows[-1].get("fi"), "fld": rows[-1].get("fld"), "vs": rows[-1].get("vs"),
                    "counter": rows[-1]["counter"]}}
    if ref is None:
        out["legacy"] = None
        return out
    n = min(len(rows), len(ref["counters"]))
    fi_eq = fld_eq = fld_known = 0
    first_diff = None
    for i in range(n):
        lfi, lfld = legacy_row_phase(ref, i)
        ok = rows[i].get("fi") == lfi
        fi_eq += ok
        if lfld is not None:
            fld_known += 1
            okf = rows[i].get("fld") == lfld
            fld_eq += okf
            ok = ok and okf
        if not ok and first_diff is None:
            first_diff = {"row": i, "fork": [rows[i].get("fi"), rows[i].get("fld")], "legacy": [lfi, lfld]}
    out["legacy"] = {"rows_compared": n, "frame_index_equal": fi_eq, "field_known": fld_known,
                     "field_equal": fld_eq, "first_difference": first_diff,
                     "lead_in": ref["lead_in"], "tail": ref["tail"],
                     "counter_offset": rows[0]["counter"] - ref["counters"][0]}
    return out


# The v2.6.3 user slots (01..15) were saved with the emulator's save hotkey,
# at a vsync inside a main-loop iteration: that iteration's world update (its
# rand() calls, fan timers, the player's clock, the actor sway) had run, but
# the main-loop counter had not yet been incremented.  A legacy session's first
# lead-in "frame" from such a slot therefore ran only the rest of that
# iteration (the increment, no world update).  Measured 2026-10-10 on route 01:
# v2.6.3 makes 22 rand() calls between slot 04 and row 0 (2 counter steps), a
# fork loop-top state 44.  The fork state that matches a user slot is the
# loop-top state right after that increment, whose frame index and field are
# the slot's flipped; from it the lead-in is one frame shorter.  With the base
# generation's slot04_first_control (counter 3300, frame index 0, field 1)
# and a lead-in of 1, route 01 equals the v2.6.3 trace in every row field
# (517 of 517 rows, the owner nodes and the player's clock included), in frame
# index and field on every row, and in the rand() state at its end.  The
# phase generation's slot04 (counter 3299, skip-at-40 boot) with the full
# lead-in matched the frame index and field but ran one world update ahead.

def slot_source_after_increment(slot: str) -> tuple[Path, int]:
    """(fork state, lead-in correction) for a beat that starts from v2.6.3 user
    slot `slot`: the fork-states manifest's state (either generation) whose
    frame index and field are the slot's flipped (see above), and -1."""
    want = legacy_slot_phase(slot)
    if want is None:
        raise RuntimeError(f"no cached v2.6.3 phase for slot {slot} (route_capture.py legacy-refs)")
    flipped = (want["frame_index"] ^ 1, want["field"] ^ 1)
    tried = []
    for gen in ("phase", "base"):
        try:
            st = fork_state(slot, gen)
        except KeyError:
            continue
        ph = _legacy_phase_at(st)
        tried.append((gen, ph))
        if ph and (ph["frame_index"], ph["field"]) == flipped:
            return st, -1
    raise RuntimeError(f"slot {slot}: no fork state with the frame index / field {flipped} "
                       f"(the v2.6.3 slot's flipped); tried {tried}")


def recorded_lead_in(name: str) -> int:
    """Frames the recorded capture of `name` ran between its source state and
    row 0 (fork phase generation: the replayed v2.6.3 lead-in; 0 otherwise).
    Tools that re-drive a recorded beat and compare rows step these first."""
    path = beat_dir(name) / "trace.json"
    if not path.exists():
        return 0
    return int(json.loads(path.read_text()).get("lead_in_frames") or 0)


def _write_json_atomic(path: Path, doc: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(path.suffix + ".tmp")
    tmp.write_text(json.dumps(doc, indent=1) + "\n")
    tmp.replace(path)


def record_phase_beat(name: str, source: str, out: Path, meta: dict, rows: list[dict],
                      session) -> None:
    """Phase generation: the set manifest in build/fork_refs/<set>/manifest.json
    and the state's hard link in fork-states/phase/beats/<path> (registered by
    fork_states.py manifest)."""
    import hashlib
    rel = out.relative_to(FORK_REFS)
    link_dir = FORK_PHASE_BEATS / rel
    link_dir.mkdir(parents=True, exist_ok=True)
    for f in ("state.p2s", "snapshot.json"):
        dst = link_dir / f
        if dst.exists() or dst.is_symlink():
            dst.unlink()
        if f == "state.p2s":
            dst.hardlink_to(out / f)        # same file, no copy (both under build/)
        else:
            shutil.copyfile(out / f, dst)
    (link_dir / "beat.json").write_text(json.dumps(
        {"beat": name, "source": source, "folder": str(out.relative_to(ROOT)), "what": meta.get("what"),
         "frames": meta.get("frames"), "phase": meta.get("phase")}, indent=1) + "\n")
    set_dir = out.parent
    mpath = set_dir / "manifest.json"
    m = json.loads(mpath.read_text()) if mpath.exists() else {}
    m.setdefault("what", "fork re-recording of a v2.6.3 capture set (route_capture.py --emulator fork "
                         "--generation phase); rows carry fi (D_00810E80), fld (D_00810E88), vs "
                         "(game vsync) and counter; docs/PCSX2_FORK.md")
    m["set"] = str(set_dir.relative_to(ROOT))
    m["legacy_set"] = str(_legacy_beat_dir(name).parent.relative_to(ROOT))
    hello = getattr(session, "hello", {}) or {}
    m.setdefault("beats", {})[name] = {
        "folder": str(out.relative_to(ROOT)), "source": source,
        "source_state": meta.get("source_state"), "state_link": str((link_dir / "state.p2s").relative_to(ROOT)),
        "state_sha256": hashlib.sha256((out / "state.p2s").read_bytes()).hexdigest(),
        "frames": meta.get("frames"), "first_counter": rows[0]["counter"], "last_counter": rows[-1]["counter"],
        "lead_in": meta.get("lead_in_frames"), "tail": meta.get("tail_idle_frames"),
        "phase": meta.get("phase"), "fork_rev": hello.get("rev"), "fork_hash": hello.get("hash"),
        "recorded": time.strftime("%Y-%m-%d %H:%M:%S")}
    _write_json_atomic(mpath, m)


def _legacy_beat_dir(name: str) -> Path:
    """Output folder of a beat: AREA01 beats (a01_*) live in build/s87/route_a01/,
    C7 beats (c7_*) in build/s87/c7cap/<item>/."""
    if name in C7_DIRS:
        return OUT_C7 / C7_DIRS[name] / name
    if name.startswith("a00_"):                 # AREA00 beats: build/s87/route_a00/
        return OUT_A00 / name
    if name.startswith("a01r_"):                # AREA01 revisit beats: build/s87/route_a01r/
        return OUT_A01R / name
    if name.startswith("a02_"):                 # AREA02 beats: build/s87/route_a02/
        return OUT_A02 / name
    if name.startswith("a04_"):                 # AREA04 beats: build/s87/route_a04/
        return OUT_A04 / name
    if name.startswith("a22_"):                 # AREA22 beats: build/s87/route_a22/
        return OUT_A22 / name
    if name.startswith("a01u_"):                # AREA01 upper-floor beats: build/s87/route_a01u/
        return OUT_A01U / name
    if name.startswith("a06_"):                 # AREA06 beats: build/s87/route_a06/
        return OUT_A06 / name
    if name.startswith("a06b_"):                # eighth level, AREA06 after the keypad
        return OUT_A06B / name
    if name.startswith("a01v_"):                # eighth level, AREA01's upper floor again
        return OUT_A01V / name
    if name.startswith("a22b_"):                # eighth level, AREA22 again
        return OUT_A22B / name
    if name.startswith("a04b_"):                # eighth level, AREA04 again
        return OUT_A04B / name
    if name.startswith("a13_"):                 # ninth level, AREA13
        return OUT_A13 / name
    if name.startswith("a19_"):                 # tenth level, AREA19
        return OUT_A19 / name
    if name.startswith("a13b_"):                # tenth level, AREA13 again
        return OUT_A13B / name
    if name.startswith("a13c_"):                # eleventh level, AREA13's battery machine
        return OUT_A13C / name
    if name.startswith("a13d_"):                # twelfth level, AREA13 to door [20] and the hatch [63]
        return OUT_A13D / name
    if name.startswith("a19b_"):                # twelfth level, AREA19 from entry 10
        return OUT_A19B / name
    if name.startswith("a19c_"):                # thirteenth level, [7]'s room to AREA19 sub 1
        return OUT_A19C / name
    if name.startswith("a19d_"):                # fourteenth level, AREA19 sub 1 to AREA15
        return OUT_A19D / name
    if name.startswith("a15_"):                 # fourteenth level, AREA15 to its exits
        return OUT_A15 / name
    if name.startswith("a15b_"):                # fifteenth level, AREA15 sub 1's event and the return
        return OUT_A15B / name
    if name.startswith("a19e_"):                # fifteenth level, AREA19 sub 1's hall and the truck
        return OUT_A19E / name
    if name.startswith("a03_"):                 # fifteenth level, AREA03
        return OUT_A03 / name
    if name.startswith("aim_"):                 # AIM capture group (docs/CAPTURES_C10.md)
        return OUT_AIM / name
    if name.startswith("exit_"):                # EXIT capture group (docs/CAPTURES_C10.md)
        return OUT_EXIT / name
    if name.startswith("dmg_"):                 # DAMAGE capture group (docs/CAPTURES_C10.md)
        return OUT_DMG / name
    if name.startswith("br_"):                  # BRANCH capture group (docs/CAPTURES_C10.md)
        return OUT_BR / name
    if name.startswith("opt_"):                 # OPTIONS capture group (docs/CAPTURES_C10.md)
        return OUT_OPT / name
    return (OUT_A01 if name.startswith("a01_") else OUT) / name


def resumes(state: Path, log_dir: Path) -> bool:
    """A snapshot is only useful if a later session can load it and reach the
    frame boundary.  Some snapshots cannot (the loaded game never returns to
    the main-loop top within the step timeout)."""
    try:
        with open_session(resumable(state), log_dir=log_dir, attempts=2) as s:
            s.step(2)
        return True
    except (RuntimeError, TimeoutError, OSError, EOFError) as exc:
        print(f"snapshot {state} does not resume: {exc}", flush=True)
        return False
    finally:
        if not FORK:
            shutil.rmtree(OUT / "_resume", ignore_errors=True)


def run_beat(name: str, source: str, fn, tries: int = 4) -> None:
    """Capture one beat.  The snapshot is verified to resume; if it does not,
    the beat is captured again with extra idle frames before the snapshot.
    Fork, phase generation: the v2.6.3 capture's lead-in and tail are replayed
    first (see "Phase lock" above) and the phase of every row is reported
    against the v2.6.3 rows."""
    base = beat_dir(name).parent
    phase_mode = FORK and GENERATION == "phase"
    ref = legacy_reference(name, source) if phase_mode else None
    lead_in = ref["lead_in"] if (ref and PHASE_LOCK) else 0
    tail0 = ref["tail"] if (ref and PHASE_LOCK) else 0
    slot_fix = phase_mode and bool(ref) and PHASE_LOCK and len(source) == 2 and source.isdigit()
    if slot_fix:
        slot_state, slot_delta = slot_source_after_increment(source)
        lead_in += slot_delta
    for attempt in range(tries):
        tail = tail0 + 23 * attempt
        src = slot_state if slot_fix else beat_source(source)
        try:
            with open_session(src, log_dir=base / "logs" / name) as s:
                if lead_in:
                    # the pad stays as the session left it (cleared), as in the v2.6.3
                    # session's free-running frames: an explicit neutral pad here
                    # (sticks 0x7F) made beat 02 leave the v2.6.3 path at frame 90
                    s.step(lead_in)
                correction = 0
                if phase_mode and ref and PHASE_LOCK:
                    # When the fork chain already left the v2.6.3 timeline in an earlier
                    # beat (route 13: the v2.6.3 run's host-timed stop at frame 133), the
                    # source lands on the other parity: one more neutral frame restores
                    # the v2.6.3 frame index and field (the game tick is then one later).
                    want_fi, want_fld = legacy_row_phase(ref, 0)
                    b = s.read(FRAME_INDEX, 12)
                    if b[0] != want_fi and (want_fld is None or b[8] != want_fld):
                        s.step(1)
                        correction = 1
                r = Route(s)
                r.begin()
                try:
                    meta = fn(r)
                    if tail:
                        r.idle(tail)
                except Exception as exc:
                    fail = base / "_failed" / name
                    fail.mkdir(parents=True, exist_ok=True)
                    (fail / "trace.json").write_text(json.dumps(
                        {"error": repr(exc), "inputs": r.inputs, "rows": r.rows}, separators=(",", ":")))
                    raise
                meta["source"] = source
                meta["tail_idle_frames"] = tail
                if FORK:
                    meta["emulator"] = "fork"
                    meta["generation"] = GENERATION
                    meta["source_state"] = str(src)
                if phase_mode:
                    meta["lead_in_frames"] = lead_in + correction
                    meta["phase_correction_frames"] = correction
                    if slot_fix:
                        meta["slot_lead_in_correction"] = slot_delta
                    meta["phase_lock"] = bool(ref and PHASE_LOCK)
                    meta["phase"] = phase_report(r.rows, ref)
                    meta["frames"] = r.frame_index
                out = r.save(name, meta)
                if phase_mode:
                    record_phase_beat(name, source, out, meta, r.rows, s)
        finally:
            if not FORK:
                shutil.rmtree(OUT / "_resume", ignore_errors=True)
        if resumes(out / "state.p2s", base / "logs" / (name + "_check")):
            print(name, "->", out, r.frame_index, "frames;", summary(r.rows[-1]), flush=True)
            return
    raise RuntimeError(f"{name}: no resumable snapshot after {tries} captures")


def script_ptr(owner: dict) -> int:
    return struct.unpack_from("<I", bytes.fromhex(owner["s1F0"]), 8)[0]


def events(doc: dict, owners=None) -> list[str]:
    """Compact change log of one beat trace (for docs/FIRST_LEVEL_ROUTE.md).
    `owners` defaults to the AREA11 OWNERS (AREA01 beats: A01_OWNERS)."""
    owners = OWNERS if owners is None else owners
    out: list[str] = []
    prev = None
    for row in doc["rows"]:
        f = row["f"]
        cur = {
            "sel3B8D": row["spad"][2:4], "3B91": row["spad"][10:12], "3B92": row["spad"][12:14],
            "action": row["m1F0"], "state5": row["p5"], "clip": row["clip"], "b2F3": row["b2F3"],
            "cam+4": row["cam_mode"][:2], "msg": row["msg"][:2] + "/" + row["msg"][16:24],
            "bars": row["screen"][:2], "fade": row["fade"][:8], "ui": row["ui"][2:12],
            "req": row["req"], "power": row["power"], "story790": row.get("story790", ""),
            "charge": row["charge"], "item1B": row["battery_item"], "area": row["area"],
        }
        for name in owners:
            o = row.get(name)
            if o:
                cur[name] = (o["h"][8:10], hex(script_ptr(o)))
        if row.get("d2"):
            cur["8107D8"] = row["d2"][:2]
            cur["810813"] = row["d2"][0x3B * 2:0x3B * 2 + 2]
        if "hp" in row:                     # AREA01 rows only (beats 00..15 have no "hp")
            for key in A01_EVENT_KEYS:
                cur[key] = row[key]
            for key in A00_EVENT_KEYS:          # AREA00 rows only
                if key in row:
                    cur[key] = row[key]
            for key in NEXT_EVENT_KEYS:         # AREA01 revisit / AREA02 rows only
                if key in row:
                    cur[key] = row[key]
            for key in A04_EVENT_KEYS:          # AREA04 rows only
                if key in row:
                    cur[key] = row[key]
            for key in A22_EVENT_KEYS:          # AREA22 rows only
                if key in row:
                    cur[key] = row[key]
            for key in A01U_EVENT_KEYS:         # AREA01 upper-floor rows only
                if key in row:
                    cur[key] = row[key]
            for key in A06_EVENT_KEYS:          # AREA06 rows only
                if key in row:
                    cur[key] = row[key]
            for key in A13_EVENT_KEYS:          # AREA13 rows only
                if key in row:
                    cur[key] = row[key]
            for key in A19_EVENT_KEYS:          # AREA19 rows only
                if key in row:
                    cur[key] = row[key]
        if "w" in row:                      # AIM rows only
            cur.update(aim_event_keys(row))
        if "roger_script" in row:           # EXIT rows only
            cur.update(exit_event_keys(row))
        if "vit" in row:                    # DAMAGE rows only
            cur.update(dmg_event_keys(row))
        if "br" in row:                     # BRANCH rows only
            cur.update(br_event_keys(row))
        if "opt" in row:                    # OPTIONS rows only
            cur.update(opt_event_keys(row))
        if prev is not None:
            diff = [f"{k}={cur[k]}" for k in cur if cur[k] != prev.get(k)]
            if diff:
                out.append(f"f{f} c{row['counter']}: " + " ".join(diff))
        prev = cur
    return out


def _term_to_interrupt(signum, frame):
    """SIGTERM ends a run like Ctrl-C: the sessions close (emulator shut down,
    scratch removed, run lock released) instead of leaving them behind."""
    raise KeyboardInterrupt(f"signal {signum}")


if __name__ == "__main__":
    import signal
    signal.signal(signal.SIGTERM, _term_to_interrupt)
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("command", choices=["identify", "probe", "run", "events", "verify", "legacy-refs"])
    ap.add_argument("--beats", default="all")
    ap.add_argument("--state", default="04")
    ap.add_argument("--frames", type=int, default=10)
    add_emulator_args(ap)
    a = ap.parse_args()
    apply_emulator_args(a)
    if a.command == "probe":
        with open_session(slot_path(a.state)) as s:
            r = Route(s)
            r.begin()
            t0 = time.monotonic()
            r.idle(a.frames)
            print(summary(r.rows[-1]), f"{(time.monotonic() - t0) / a.frames * 1000:.1f} ms/frame")
    elif a.command == "run":
        wanted = None if a.beats == "all" else set(a.beats.split(","))
        for name, source, fn in BEATS:
            if wanted is None and name in OPT_IN_BEATS:
                continue
            if wanted is None or name in wanted or name[:2] in wanted:
                run_beat(name, source, fn)
        if wanted is not None:          # the AREA01 group runs only when named
            for name, source, fn in a01_selected(a.beats):
                run_beat(name, source, fn)
            for name, source, fn in c7_selected(a.beats):     # so does the C7 group
                run_beat(name, source, fn)
            for name, source, fn in a00_selected(a.beats):    # and the AREA00 group
                run_beat(name, source, fn)
            for name, source, fn in a01r_selected(a.beats):   # the AREA01 revisit group
                run_beat(name, source, fn)
            for name, source, fn in a02_selected(a.beats):    # and the AREA02 group
                run_beat(name, source, fn)
            for name, source, fn in a04_selected(a.beats):    # and the AREA04 group
                run_beat(name, source, fn)
            for name, source, fn in a22_selected(a.beats):    # and the AREA22 group
                run_beat(name, source, fn)
            for name, source, fn in a01u_selected(a.beats):   # and the AREA01 upper-floor group
                run_beat(name, source, fn)
            for name, source, fn in a06_selected(a.beats):    # and the AREA06 group
                run_beat(name, source, fn)
            for name, source, fn in eighth_selected(a.beats):  # the eighth-level groups
                run_beat(name, source, fn)
            for name, source, fn in a13_selected(a.beats):     # the ninth-level group
                run_beat(name, source, fn)
            for name, source, fn in tenth_selected(a.beats):   # the tenth-level groups
                run_beat(name, source, fn)
            for name, source, fn in eleventh_selected(a.beats):  # the eleventh-level group
                run_beat(name, source, fn)
            for name, source, fn in twelfth_selected(a.beats):  # the twelfth-level groups
                run_beat(name, source, fn)
            for name, source, fn in thirteenth_selected(a.beats):  # the thirteenth-level group
                run_beat(name, source, fn)
            for name, source, fn in fourteenth_selected(a.beats):  # the fourteenth-level groups
                run_beat(name, source, fn)
            for name, source, fn in fourteenth_a15_selected(a.beats):
                run_beat(name, source, fn)
            for name, source, fn in fifteenth_selected(a.beats):  # the fifteenth-level groups
                run_beat(name, source, fn)
            for name, source, fn in aim_selected(a.beats):     # the AIM capture group
                run_beat(name, source, fn)
            for name, source, fn in exit_selected(a.beats):    # the EXIT capture group
                run_beat(name, source, fn)
            for name, source, fn in dmg_selected(a.beats):     # the DAMAGE capture group
                run_beat(name, source, fn)
            for name, source, fn in br_selected(a.beats):      # the BRANCH capture group
                run_beat(name, source, fn)
            for name, source, fn in opt_selected(a.beats):     # the OPTIONS capture group
                run_beat(name, source, fn)
        if FORK and GENERATION == "phase":
            import fork_states                          # register the new states (both generations)
            fork_states.cmd_manifest(None)
    elif a.command == "events":
        chosen = [b for b in BEATS if a.beats == "all" or b[0][:2] in a.beats.split(",")]
        chosen += a01_selected(a.beats) if a.beats != "all" else []
        chosen += c7_selected(a.beats) if a.beats != "all" else []
        chosen += a00_selected(a.beats) if a.beats != "all" else []
        chosen += a01r_selected(a.beats) if a.beats != "all" else []
        chosen += a02_selected(a.beats) if a.beats != "all" else []
        chosen += a04_selected(a.beats) if a.beats != "all" else []
        chosen += a22_selected(a.beats) if a.beats != "all" else []
        chosen += a01u_selected(a.beats) if a.beats != "all" else []
        chosen += a06_selected(a.beats) if a.beats != "all" else []
        chosen += eighth_selected(a.beats) if a.beats != "all" else []
        chosen += a13_selected(a.beats) if a.beats != "all" else []
        chosen += tenth_selected(a.beats) if a.beats != "all" else []
        chosen += eleventh_selected(a.beats) if a.beats != "all" else []
        chosen += twelfth_selected(a.beats) if a.beats != "all" else []
        chosen += thirteenth_selected(a.beats) if a.beats != "all" else []
        chosen += fourteenth_selected(a.beats) if a.beats != "all" else []
        chosen += fourteenth_a15_selected(a.beats) if a.beats != "all" else []
        chosen += fifteenth_selected(a.beats) if a.beats != "all" else []
        chosen += aim_selected(a.beats) if a.beats != "all" else []
        chosen += exit_selected(a.beats) if a.beats != "all" else []
        chosen += dmg_selected(a.beats) if a.beats != "all" else []
        chosen += br_selected(a.beats) if a.beats != "all" else []
        chosen += opt_selected(a.beats) if a.beats != "all" else []
        for name, _source, _fn in chosen:
            path = beat_dir(name) / "trace.json"
            if not path.exists():
                continue
            doc = json.loads(path.read_text())
            print(f"== {name}: {doc['frames']} frames, counters {doc['first_counter']}..{doc['last_counter']}")
            print("   inputs:", [i for i in doc["inputs"] if i["buttons"]])
            print("   teleports:", doc["teleports"])
            owners = (A01_OWNERS if name.startswith("a01_") else
                      A00_OWNERS if name.startswith("a00_") else
                      A01R_OWNERS if name.startswith("a01r_") else
                      A02_OWNERS if name.startswith("a02_") else
                      A04_OWNERS if name.startswith("a04_") else
                      A22_OWNERS if name.startswith("a22_") else
                      A01U_OWNERS if name.startswith("a01u_") else
                      A06_OWNERS if name.startswith("a06_") else
                      eighth_owners(name) or ninth_owners(name) or tenth_owners(name)
                      or eleventh_owners(name) or twelfth_owners(name) or thirteenth_owners(name)
                      or fourteenth_owners(name) or fifteenth_owners(name))
            for line in events(doc, owners):
                print("  ", line)
    elif a.command == "legacy-refs":
        # every group, offline: cache the v2.6.3 facts the phase lock needs
        groups = [BEATS, A01_BEATS, C7_BEATS, A00_BEATS, A01R_BEATS, A02_BEATS, A04_BEATS, A22_BEATS,
                  A01U_BEATS, A06_BEATS]
        allb = {}
        for g in groups:
            for b in g:
                allb[b[0]] = b
        for fn_sel in (eighth_selected, a13_selected, tenth_selected, eleventh_selected, twelfth_selected,
                       thirteenth_selected, fourteenth_selected, fourteenth_a15_selected, fifteenth_selected,
                       aim_selected, exit_selected, dmg_selected, br_selected, opt_selected):
            for prefix in ("a06b", "a01v", "a22b", "a04b", "a13", "a19", "a13b", "a13c", "a13d", "a19b",
                           "a19c", "a19d", "a15", "a15b", "a19e", "a03", "aim", "exit", "dmg", "br", "opt"):
                for b in fn_sel(prefix):
                    allb[b[0]] = b
        slots = {sl: legacy_slot_phase(sl) for sl in ("01", "02", "03", "04", "06", "07", "08", "11", "12",
                                                       "13", "14", "15")}
        n = sum(1 for name, source, _fn in allb.values() if legacy_reference(name, source) is not None)
        print(f"{n} of {len(allb)} beats and {sum(1 for v in slots.values() if v)} slots cached in {LEGACY_REFS}")
    elif a.command == "identify":
        from parse_pcsx2_state import extract_zstd_entry
        for path in sorted(SSTATES.glob(f"{SERIAL}.*.p2s")):
            ee = extract_zstd_entry(path, "eeMemory.bin")
            sp = extract_zstd_entry(path, "Scratchpad.bin")
            p = ee[PLAYER:PLAYER + 0x300]
            roger = ee[0x7A8830:0x7A8830 + 0x200]
            info = {
                "counter": struct.unpack_from("<I", sp, 0x3B64)[0], "spad3B8C": sp[0x3B8C:0x3B94].hex(),
                "area": ee[0x810700:0x810702].hex(),
                "pos": [round(v, 2) for v in struct.unpack_from("<3f", ee, 0x810350)],
                "yaw": round(f32(p, 0xC4), 4), "action": p[0x1F0], "b2F3": p[0x2F3],
                "clip": struct.unpack_from("<h", p, 0x20C)[0], "ui": ee[0x810130:0x810138].hex(),
                "req": ee[0x8106B0:0x8106BA].hex(), "power": hex(ee[0x81084C]),
                "story790": ee[0x810790:0x810794].hex(), "8107D8": hex(ee[0x8107D8]),
                "810813": hex(ee[0x810813]), "item1B": ee[0x810C7F],
                "charge": struct.unpack_from("<H", ee, 0x810CB2)[0],
                "battery_owner_state": ee[0x7A5640 + 4], "panel": ee[0x7AA590:0x7AA590 + 12].hex(),
                "elevator": ee[0x7AA880:0x7AA880 + 12].hex(),
                "roger_script": hex(struct.unpack_from("<I", roger, 0x1F8)[0]),
            }
            print(path.name.split(".")[-2], json.dumps(info))
    elif a.command == "verify":
        chosen = [b for b in BEATS if a.beats == "all" or b[0][:2] in a.beats.split(",")]
        chosen += a01_selected(a.beats) if a.beats != "all" else []
        chosen += c7_selected(a.beats) if a.beats != "all" else []
        chosen += a00_selected(a.beats) if a.beats != "all" else []
        chosen += a01r_selected(a.beats) if a.beats != "all" else []
        chosen += a02_selected(a.beats) if a.beats != "all" else []
        chosen += a04_selected(a.beats) if a.beats != "all" else []
        chosen += a22_selected(a.beats) if a.beats != "all" else []
        chosen += a01u_selected(a.beats) if a.beats != "all" else []
        chosen += a06_selected(a.beats) if a.beats != "all" else []
        chosen += eighth_selected(a.beats) if a.beats != "all" else []
        chosen += a13_selected(a.beats) if a.beats != "all" else []
        chosen += tenth_selected(a.beats) if a.beats != "all" else []
        chosen += eleventh_selected(a.beats) if a.beats != "all" else []
        chosen += twelfth_selected(a.beats) if a.beats != "all" else []
        chosen += thirteenth_selected(a.beats) if a.beats != "all" else []
        chosen += fourteenth_selected(a.beats) if a.beats != "all" else []
        chosen += fourteenth_a15_selected(a.beats) if a.beats != "all" else []
        chosen += fifteenth_selected(a.beats) if a.beats != "all" else []
        chosen += aim_selected(a.beats) if a.beats != "all" else []
        chosen += exit_selected(a.beats) if a.beats != "all" else []
        chosen += dmg_selected(a.beats) if a.beats != "all" else []
        chosen += br_selected(a.beats) if a.beats != "all" else []
        chosen += opt_selected(a.beats) if a.beats != "all" else []
        for name, _source, _fn in chosen:
            state = beat_dir(name) / "state.p2s"
            if state.exists():
                ok = resumes(state, beat_dir(name).parent / "logs" / (name + "_check"))
                print(name, "resumes" if ok else "DOES NOT RESUME", flush=True)
