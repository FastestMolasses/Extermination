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
from pcsx2_session import OriginalSession, PAD, SSTATES  # noqa: E402

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


def resumable(state: Path) -> Path:
    """pcsx2_session.snapshot() derives the slot file name from the source
    state's name, so a beat's state.p2s is resumed through a correctly named
    copy next to it (never inside the sstates directory)."""
    if state.name.startswith(SERIAL):
        return state
    folder = OUT / "_resume" / state.parent.name
    folder.mkdir(parents=True, exist_ok=True)
    copy = folder / f"{SERIAL}.resume.p2s"
    shutil.copyfile(state, copy)
    return copy


def wait_for_free_emulator(timeout: float = 600.0) -> None:
    """The Pine socket and DebugServer port are global: never start a second
    emulator while another session (any lane) is running one."""
    import subprocess
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if subprocess.run(["pgrep", "-f", "PCSX2.app/Contents/MacOS/PCSX2"],
                          capture_output=True).returncode != 0:
            return
        time.sleep(2)
    raise RuntimeError("another PCSX2 session is still running")


class RouteSession(OriginalSession):
    """pcsx2_session.OriginalSession with a longer frame-boundary timeout.
    Right after -statefile some states take longer than the default 5 s to
    reach the main-loop top the first time (observed on several beat
    snapshots); the per-frame timeout is only a fault detector."""

    boundary_timeout = 30.0     # beat 15 raises it: Roger's departure plays an FMV inside one frame

    def _resume_to_boundary(self, timeout: float | None = None) -> None:
        super()._resume_to_boundary(timeout=timeout or self.boundary_timeout)


class RetrySession:
    """Context manager: start the hidden emulator, retrying start-up races
    (DebugServer not yet listening, first boundary missed)."""

    def __init__(self, state: Path, log_dir: Path | None = None, attempts: int = 6):
        self.state, self.log_dir, self.attempts = state, log_dir or OUT / "logs", attempts
        self.session: OriginalSession | None = None

    def __enter__(self) -> OriginalSession:
        for attempt in range(self.attempts):
            wait_for_free_emulator()
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


def beat_source(source: str) -> Path:
    if len(source) == 2 and source.isdigit():
        return slot_path(source)
    return resumable(beat_dir(source) / "state.p2s")


def beat_dir(name: str) -> Path:
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
        shutil.rmtree(OUT / "_resume", ignore_errors=True)


def run_beat(name: str, source: str, fn, tries: int = 4) -> None:
    """Capture one beat.  The snapshot is verified to resume; if it does not,
    the beat is captured again with extra idle frames before the snapshot."""
    base = beat_dir(name).parent
    for attempt in range(tries):
        tail = 23 * attempt
        src = beat_source(source)
        try:
            with open_session(src, log_dir=base / "logs" / name) as s:
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
                out = r.save(name, meta)
        finally:
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
        if prev is not None:
            diff = [f"{k}={cur[k]}" for k in cur if cur[k] != prev.get(k)]
            if diff:
                out.append(f"f{f} c{row['counter']}: " + " ".join(diff))
        prev = cur
    return out


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("command", choices=["identify", "probe", "run", "events", "verify"])
    ap.add_argument("--beats", default="all")
    ap.add_argument("--state", default="04")
    ap.add_argument("--frames", type=int, default=10)
    a = ap.parse_args()
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
                      eighth_owners(name))
            for line in events(doc, owners):
                print("  ", line)
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
        for name, _source, _fn in chosen:
            state = beat_dir(name) / "state.p2s"
            if state.exists():
                ok = resumes(state, beat_dir(name).parent / "logs" / (name + "_check"))
                print(name, "resumes" if ok else "DOES NOT RESUME", flush=True)
