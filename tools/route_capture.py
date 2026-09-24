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


def decode(r: dict[str, bytes]) -> dict:
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
    for name in OWNERS:
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
        out = OUT / name
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


def beat_source(source: str) -> Path:
    if len(source) == 2 and source.isdigit():
        return slot_path(source)
    return resumable(OUT / source / "state.p2s")


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
    for attempt in range(tries):
        tail = 23 * attempt
        src = beat_source(source)
        try:
            with open_session(src, log_dir=OUT / "logs" / name) as s:
                r = Route(s)
                r.begin()
                try:
                    meta = fn(r)
                    if tail:
                        r.idle(tail)
                except Exception as exc:
                    fail = OUT / "_failed" / name
                    fail.mkdir(parents=True, exist_ok=True)
                    (fail / "trace.json").write_text(json.dumps(
                        {"error": repr(exc), "inputs": r.inputs, "rows": r.rows}, separators=(",", ":")))
                    raise
                meta["source"] = source
                meta["tail_idle_frames"] = tail
                out = r.save(name, meta)
        finally:
            shutil.rmtree(OUT / "_resume", ignore_errors=True)
        if resumes(out / "state.p2s", OUT / "logs" / (name + "_check")):
            print(name, "->", out, r.frame_index, "frames;", summary(r.rows[-1]), flush=True)
            return
    raise RuntimeError(f"{name}: no resumable snapshot after {tries} captures")


def script_ptr(owner: dict) -> int:
    return struct.unpack_from("<I", bytes.fromhex(owner["s1F0"]), 8)[0]


def events(doc: dict) -> list[str]:
    """Compact change log of one beat trace (for docs/FIRST_LEVEL_ROUTE.md)."""
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
        for name in OWNERS:
            o = row.get(name)
            if o:
                cur[name] = (o["h"][8:10], hex(script_ptr(o)))
        if row.get("d2"):
            cur["8107D8"] = row["d2"][:2]
            cur["810813"] = row["d2"][0x3B * 2:0x3B * 2 + 2]
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
    elif a.command == "events":
        for name, _source, _fn in BEATS:
            if a.beats != "all" and name[:2] not in a.beats.split(","):
                continue
            path = OUT / name / "trace.json"
            if not path.exists():
                continue
            doc = json.loads(path.read_text())
            print(f"== {name}: {doc['frames']} frames, counters {doc['first_counter']}..{doc['last_counter']}")
            print("   inputs:", [i for i in doc["inputs"] if i["buttons"]])
            print("   teleports:", doc["teleports"])
            for line in events(doc):
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
        for name, _source, _fn in BEATS:
            if a.beats != "all" and name[:2] not in a.beats.split(","):
                continue
            state = OUT / name / "state.p2s"
            if state.exists():
                ok = resumes(state, OUT / "logs" / (name + "_check"))
                print(name, "resumes" if ok else "DOES NOT RESUME", flush=True)
