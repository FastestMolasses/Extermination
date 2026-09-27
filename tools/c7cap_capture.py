#!/usr/bin/env python3
"""c7cap_capture.py - C7 original captures: stream drive latency, lane-3 writers, rand trace.

Drives the ORIGINAL game in the hidden, MCP-enabled PCSX2 through
tools/pcsx2_session.py (exact one-frame steps at the main-loop top 0x1AAF28)
and records three things the C6 chain asked for (docs/CAPTURES_C7.md):

  stream  per-field records of the stream lanes and the drive around four
          stream starts: the New Game opening's lane-0 request (user slot 01,
          the same title inputs as route_census's startup) and route beats
          10 / 11 / 13 (Roger's line 0x7F, the director lines 0x97 / 0x99).
          Route beats are re-driven by route_capture's own closed-loop beat
          function from the beat's recorded source snapshot (reloaded
          through Pine, then neutral frames to the recorded first counter)
          and aligned on an event anchor (an owner's script start); an
          open-loop replay of the recorded pad values does not reproduce the
          recordings (docs/CAPTURES_C7.md).  Fields are sampled at every
          vsync-ISR entry (0x1AB140) and every main-loop top in the window.
  lane3   the effect lane-3 ring's +0x40 parameter quadwords (0x76D9C0 +
          0x60*slot + 0x40, 32 slots): a bracket pass samples them at every
          field; a pinpoint pass repeats the run with EE write memchecks
          armed only over chosen changed fields.  `--from title` runs user
          slot 01 through New Game to first control; `--from boot`
          cold-boots the disc (no save state) up to the title.
  rng     every call of rand (0x122BB8, the byte-matched LCG of
          src/func_00122BB8.c) and srand (0x122BA8) with the caller's return
          address, frame, vsync, the state before and the value returned:
          New Game (slot 01) from the AREA11 load through the area entry
          (0x1AE040 state 0), the opening, first control and 300 frames after
          it; route 01 (whole beat) and route 10 (the director / Roger stretch).

Everything written here is derived from the user's own disc and stays in the
ignored build/s87/c7cap/<item>/ tree.  This file embeds no original code or
data; it names addresses only.  Save states are only read (pcsx2_session
hashes each source before and after); the user's slots 01..15 are never
written (a route source is loaded from a temporary copy in a free slot >= 40
that is deleted right after the load); the emulator runs hidden and is closed
at the end of every run.

Usage (decomp .venv python, repo root):
    .venv/bin/python tools/c7cap_capture.py stream --stretch opening|r10|r11|r13|all
    .venv/bin/python tools/c7cap_capture.py lane3 --from title|boot [--mode bracket|pinpoint|both]
    .venv/bin/python tools/c7cap_capture.py rng --stretch newgame|r01|r10|all
"""
from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import struct
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import route_capture as rc  # noqa: E402
from pcsx2_session import (DEFAULT_EMULATOR, DEFAULT_ISO, ELF, FRAME_COUNTER, LOOP_TOP, PAD,  # noqa: E402
                           VSYNC_COUNTER, Pine)
from route_census import PersistentDebug  # noqa: E402

OUT = ROOT / "build/s87/c7cap"
ROUTE = ROOT / "build/s87/route"
ISR = 0x1AB140                  # vsync ISR entry (increments 0x810E90)
RAND, SRAND = 0x122BB8, 0x122BA8
REENT_PTR = 0x24295C            # D_0024295C -> block; rand state at +0x58
LANE3 = 0x76D9C0                # D_0028F700 + 0x4DBEC0 + 3 * 0xC00
AREA_ENTRY = 0x1AFCF0           # called from 0x1AE040 state 0 (area entry)

# Stream-side events (exec breakpoints; a0/a1/ra recorded).
STREAM_EVENTS = {
    0x1FD4C0: "001FD4C0 stream request",
    0x1FA790: "001FA790 lane start",
    0x1FABF0: "001FABF0 lane restart",
    0x1FA5A0: "001FA5A0 voice push",
    0x112610: "00112610 disc read issue",
    0x11A6A0: "0011A6A0 key-on packer",
    0x1FAAC0: "001FAAC0 lane release",
    0x1FAD70: "001FAD70 fade-out step",
}


def u32(b: bytes, o: int = 0) -> int:
    return struct.unpack_from("<I", b, o)[0]


# ---------------------------------------------------------------------------
# Session: frame steps that service extra breakpoints / memchecks on the way.

class C7Session(rc.RouteSession):
    """RouteSession with one persistent DebugServer connection and a
    breakpoint dispatcher: every pause that is not the frame boundary goes
    to handlers[pc] (exec breakpoints) or to on_other(pc) (memchecks)."""

    stall_timeout = 3000.0          # the intro movie plays inside one frame (~160 s, longer under host load)

    def __init__(self, *a, **k):
        super().__init__(*a, **k)
        self.debug = PersistentDebug()
        self.handlers: dict[int, callable] = {}
        self.on_other = None
        self.unexpected: list[dict] = []
        self.kicks: list[dict] = []

    def close(self) -> None:
        try:
            if self.pid is not None:
                self.debug.call({"cmd": "clear_breakpoints"})
        except Exception:
            pass
        super().close()
        self.debug.close()

    def arm(self, addr: int, fn) -> None:
        self.handlers[addr] = fn
        self.debug.call({"cmd": "set_breakpoint", "address": addr, "description": "c7cap"})

    def disarm(self, addr: int) -> None:
        if self.handlers.pop(addr, None) is not None:
            self.debug.call({"cmd": "remove_breakpoint", "address": addr})

    def reg(self, name: str) -> int:
        return int(self.debug.call({"cmd": "evaluate", "expression": name})["result"]) & 0xFFFFFFFF

    def _wait_pause(self, timeout: float) -> int:
        deadline = time.monotonic() + timeout
        last_cycles, still_since = None, time.monotonic()
        while True:
            data = self.debug.call({"cmd": "status"})
            data = data.get("data", data)
            if data.get("paused"):
                self._pause_cycles = data.get("cycles")
                return int(data["pc"], 16)
            now = time.monotonic()
            if now > deadline:
                raise TimeoutError(f"no pause within {timeout} s")
            cycles = data.get("cycles")
            if cycles != last_cycles:
                last_cycles, still_since = cycles, now
            elif now - still_since > 3.0:           # route_census: VM stopped, EE not paused
                pc_now = int(data["pc"], 16)
                moved = cycles != getattr(self, "_pause_cycles", None)
                if moved and (pc_now == LOOP_TOP or pc_now in self.handlers):
                    # Stopped ON one of our breakpoints while the status says
                    # "running" (seen under heavy host load).  Re-issuing resume
                    # here would skip that breakpoint (the resume skips a
                    # breakpoint at the current PC): the first rand trace lost
                    # 12 calls that way.  Treat it as the pause it is.  The
                    # cycle check excludes the other case, a resume that has
                    # not yet taken effect (frozen at the PAUSE we resumed
                    # from, same cycle count), which still gets a plain kick.
                    self.kicks.append({"pc": data.get("pc"), "cycles": cycles, "taken_as_pause": True})
                    self._pause_cycles = cycles
                    return pc_now
                self.kicks.append({"pc": data.get("pc"), "cycles": cycles})
                if sum(1 for k in self.kicks if k["cycles"] == cycles) > 3:
                    self.debug.call({"cmd": "pause"})
                    time.sleep(0.2)
                self.debug.call({"cmd": "resume"})
                still_since = now
            time.sleep(0.001)

    def _resume_to_boundary(self, timeout: float | None = None) -> None:
        limit = max(timeout or 0, self.stall_timeout)
        while True:
            self.debug.call({"cmd": "resume"})
            pc = self._wait_pause(limit)
            if pc == LOOP_TOP:
                return
            fn = self.handlers.get(pc)
            if fn is not None:
                fn(pc)
            elif self.on_other is not None:
                self.on_other(pc)
            else:
                self.unexpected.append({"pc": hex(pc)})
                if len(self.unexpected) > 50:
                    raise RuntimeError(f"unexpected pauses: {self.unexpected[-3:]}")


def open_c7(state: Path, log_dir: Path, attempts: int = 6) -> C7Session:
    for attempt in range(attempts):
        rc.wait_for_free_emulator()
        session = C7Session(state, log_dir=log_dir)
        try:
            return session.__enter__()
        except (RuntimeError, TimeoutError, OSError, EOFError) as exc:
            print(f"start attempt {attempt + 1} failed: {exc}", flush=True)
            time.sleep(2)
    raise RuntimeError(f"emulator did not start from {state}")


class ColdSession(C7Session):
    """Cold boot of the user's disc: no save state.  The DebugServer comes up
    before the game code runs; the caller arms breakpoints/memchecks through
    `prepare(session)` before the VM is let go, then stops at the first
    main-loop top."""

    def __init__(self, prepare, log_dir: Path):
        self.state = ELF                       # hashed like a source state (read only)
        self.emulator, self.iso = Path(DEFAULT_EMULATOR), Path(DEFAULT_ISO)
        self.log_dir = log_dir
        self.ready_timeout = 60.0
        self.proc = None
        self.pid = None
        self.visible = False
        self.pine = None
        self.frames_stepped = 0
        self._digest = hashlib.sha256(self.state.read_bytes()).hexdigest()
        self.debug = PersistentDebug()
        self.handlers, self.on_other, self.unexpected, self.kicks = {}, None, [], []
        self.prepare = prepare
        self.boot_notes: dict = {}

    def _start(self):
        self.log_dir.mkdir(parents=True, exist_ok=True)
        self._log = open(self.log_dir / "launch.log", "w")
        args = ["-portable", "-fastboot", "-elf", str(ELF),
                "-logfile", str(self.log_dir / "emulator.log"), str(self.iso)]
        bundle = self.emulator.parents[2]
        before = set(self._emulator_pids())
        subprocess.run(["open", "-g", "-j", "-n", "-a", str(bundle), "--args", *args],
                       check=True, stdout=self._log, stderr=subprocess.STDOUT)
        deadline = time.monotonic() + 20
        while time.monotonic() < deadline and self.pid is None:
            fresh = [p for p in self._emulator_pids() if p not in before]
            self.pid = max(fresh) if fresh else None
            time.sleep(0.02)
        if self.pid is None:
            raise RuntimeError("hidden emulator launch: process not found")
        deadline = time.monotonic() + self.ready_timeout
        t0 = time.monotonic()
        while time.monotonic() < deadline:          # DebugServer up -> pause at once
            if not self._alive():
                raise RuntimeError("emulator exited early")
            self._hide()
            try:
                st = self.debug.call({"cmd": "status"})
                if st.get("data", st).get("alive"):
                    self.debug.call({"cmd": "pause"})
                    break
            except (OSError, EOFError, RuntimeError):
                self.debug.close()
            time.sleep(0.01)
        else:
            raise RuntimeError("DebugServer did not come up")
        st = self.debug.call({"cmd": "status"})
        st = st.get("data", st)
        self.boot_notes = {"debugserver_seconds": round(time.monotonic() - t0, 3),
                           "first_status": st}
        self.debug.call({"cmd": "pad_set", "clear": True})
        self.prepare(self)
        self.debug.call({"cmd": "set_breakpoint", "address": LOOP_TOP,
                         "description": "c7cap frame boundary"})
        self._resume_to_boundary()
        self.pine = Pine()
        self._hide()
        return self


# ---------------------------------------------------------------------------
# Common helpers

def startup_title_to_area(s: C7Session, on_frame=None, log: list | None = None) -> dict:
    """route_census.run_startup's S0 + S1 inputs: Cross at title frames 5
    (125, 245, 365 if needed) while the fade machine is idle, release at
    n % 120 == 9, until NEW GAME commits the area (0x810700 != 0); then plain
    frames until AREA11 runs (0x810700 == 0x0B and task +B == 1)."""
    def area() -> int:
        return s.read(0x810700, 4)[0]

    def task_b() -> int:
        return s.read(0x28A750, 0x10)[0xB]

    f0 = s.frames_stepped
    presses, inputs = 0, []
    start_counter = s.u32(FRAME_COUNTER)
    while area() == 0:
        n = s.frames_stepped - f0
        if n in (5, 125, 245, 365) and presses < 4 and s.read(0x28A9A0, 4) == b"\0\0\0\0":
            s.pad(PAD["CROSS"]); presses += 1
            inputs.append({"n": n, "buttons": PAD["CROSS"]})
        elif n % 120 in (9,):
            s.pad(0)
            inputs.append({"n": n, "buttons": 0})
        s.step(1)
        if on_frame:
            on_frame("S0")
        if n > 1500:
            raise TimeoutError("title did not accept NEW GAME")
    s.pad(0)
    s0_end = s.u32(FRAME_COUNTER)
    while not (area() == 0x0B and task_b() == 1):
        s.step(1)
        if on_frame:
            on_frame("S1")
        if s.frames_stepped - f0 > 4000:
            raise TimeoutError("AREA11 did not start")
    return {"title_start_counter": start_counter, "newgame_commit_counter": s0_end,
            "area11_running_counter": s.u32(FRAME_COUNTER), "presses": presses, "inputs": inputs}


def pine_reload(s: C7Session, src: Path) -> int:
    """Load `src` into the paused VM through Pine from a temporary copy in a
    free slot >= 40 (the user's slots 01..15 are never touched; the copy is
    deleted right after the load).  Returns the loaded main-loop counter."""
    used = set()
    for p in rc.SSTATES.glob(f"{rc.SERIAL}.*.p2s"):
        try:
            used.add(int(p.name.split(".")[-2]))
        except ValueError:
            pass
    slot = next(x for x in range(40, 64) if x not in used)
    tmp = rc.SSTATES / f"{rc.SERIAL}.{slot:02d}.p2s"
    if tmp.exists():
        raise FileExistsError(tmp)
    shutil.copyfile(src, tmp)
    try:
        s.pine.request(bytes((10, slot)))
        time.sleep(0.5)
    finally:
        tmp.unlink()
    data = s.debug.call({"cmd": "status"})
    data = data.get("data", data)
    if not data.get("paused"):
        raise RuntimeError("VM not paused after the Pine load")
    return s.u32(FRAME_COUNTER)


def load_trace(beat: str) -> dict:
    return json.loads((ROUTE / beat / "trace.json").read_text())


def beat_source_state(doc: dict) -> Path:
    src = doc["source"]
    if len(src) == 2 and src.isdigit():
        return rc.slot_path(src)
    return ROUTE / src / "state.p2s"


def write_jsonl(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w") as fh:
        for r in rows:
            fh.write(json.dumps(r, separators=(",", ":")) + "\n")


# ---------------------------------------------------------------------------
# Item 1: STREAM

# One Pine request: (name, address, length)
STREAM_SPANS = [
    ("counter", FRAME_COUNTER, 4), ("vsync", VSYNC_COUNTER, 4), ("vwait", 0x810E98, 4),
    ("lb", 0x282154, 8),                 # D_00282154..5B
    ("lanes", 0x281FD0, 0x120),          # three lane records, 0x60 each
    ("hold", 0x8106F4, 4),               # D_008106F4 lane-0 hold, D_008106F5 voice hold
    ("ring", 0x281CF0, 0x40), ("ringidx", 0x275B30, 8),
    ("spad", 0x70003B8C, 8), ("fade", 0x28A9A0, 0x10), ("msg", 0x2821B0, 0x10),
]


class StreamSampler:
    def __init__(self, s: C7Session):
        self.s = s
        self.body = b"".join(struct.pack("<BI", 2, a + i) for _n, a, n in STREAM_SPANS
                             for i in range(0, n, 4))

    def raw(self) -> dict[str, bytes]:
        data = self.s.pine.request(self.body)
        out, off = {}, 0
        for name, _a, n in STREAM_SPANS:
            out[name] = data[off:off + n]
            off += n
        return out

    def iop(self) -> dict:
        """IOP side through the DebugServer (r3000 debug interface):
        CDVD registers 0x1F402004..0x1F40200F (N-command, N-ready, error,
        break, interrupt status, 0x1F402009, drive status, tray/status 2,
        current-position bytes, disc type) and IOP DMA channel 3 (CDVD)
        MADR/BCR/CHCR at 0x1F8010B0..0x1F8010BB.  The command FIFOs
        (0x1F402018 etc.) are never read: reading them pops data."""
        d = self.s.debug
        cd = d.call({"cmd": "read_memory", "cpu": "iop", "address": 0x1F402004, "length": 12})["hex"]
        dma = d.call({"cmd": "read_memory", "cpu": "iop", "address": 0x1F8010B0, "length": 12})["hex"]
        st = d.call({"cmd": "status", "cpu": "iop"})
        st = st.get("data", st)
        return {"cdvd": cd, "dma3": dma, "iop_pc": st.get("pc")}

    def record(self, phase: str, f: int, full: bool = True) -> dict:
        r = self.raw()
        lanes = r["lanes"]
        rec = {"f": f, "phase": phase, "counter": u32(r["counter"]), "vsync": u32(r["vsync"]),
               "vwait": u32(r["vwait"]), "lb": r["lb"].hex(),
               "hold_F4": r["hold"][0], "hold_F5": r["hold"][1],
               "spad": r["spad"].hex(), "fade": r["fade"][:8].hex(), "msg": r["msg"][:8].hex()}
        for lane in range(3):
            b = lanes[lane * 0x60:(lane + 1) * 0x60]
            rec[f"L{lane}"] = {"b0_3": b[0:4].hex(), "sector": u32(b, 0x30), "count": u32(b, 0x34),
                               "addr": hex(u32(b, 0x38)), "start": u32(b, 0x50),
                               "dur": u32(b, 0x4C), "cue": u32(b, 0x20)}
        if full:
            rec["lanes_raw"] = lanes.hex()
            rec["ring"] = r["ring"].hex()
            rec["ringidx"] = r["ringidx"].hex()
            rec["iop"] = self.iop()
        return rec


STREAM_STRETCHES = {
    # beat, anchor (director script start, recorded), per-field window and
    # per-frame end in RECORDED frame indices (end = the line's teardown row + 15)
    "r10": ("10_cage_roof_roger", ("director_r12", 0x8294C0), (1160, 1180), 3398,
            "Roger's line 0x7F (director beat 0 script 0x8294C0 at f1089, msg 02/7f at f1163, teardown f3383)"),
    "r11": ("11_crevice_prompt", ("director_r12", 0x829A40), (870, 890), 1059,
            "director line 0x97 (script 0x829A40 at f705, msg 02/97 at f873, teardown f1044)"),
    "r13": ("13_east_tower", ("director_r12", 0x829CC0), (533, 550), 762,
            "director line 0x99 (script 0x829CC0 at f530, msg 02/99 at f536, teardown f747)"),
}

# Row keys compared at equal anchor-relative offsets (the event side of the
# route; the player's position is closed-loop and differs by design).
ALIGN_KEYS = ("spad", "msg", "cam_mode", "screen", "fade", "req", "story790", "d2")


def owner_script(row: dict, owner: str) -> int:
    return rc.script_ptr(row[owner])


class StopBeat(Exception):
    pass


class HookRoute(rc.Route):
    """rc.Route whose every frame step calls hook(row) afterwards."""

    def __init__(self, session, hook):
        super().__init__(session)
        self.hook = hook

    def step(self, n: int = 1) -> dict:
        row = None
        for _ in range(n):
            row = super().step(1)
            self.hook(row)
        return row


class ClosedLoop:
    """Runs route_capture's own closed-loop beat function (as route_census
    does) from the beat's recorded source snapshot, reloaded through Pine and
    stepped neutrally to the recorded first counter, so f0 equals the
    recorded row 0.  Open-loop replay of the recorded pad values is NOT
    used: the recorded runs contain host-timed pad-application effects (on
    route 13 three replays agreed with each other bit for bit and all left
    the recording at f133, where the recording shows a one-frame stop
    with no input change), so the beat is re-driven closed loop and the
    stretch is aligned on an event anchor (owner, script): the first frame
    whose owner script pointer equals `script`."""

    def __init__(self, beat: str, anchor: tuple[str, int] | None, log_dir: Path):
        self.beat, self.anchor, self.log_dir = beat, anchor, log_dir
        self.doc = load_trace(beat)
        self.fn = {n: f for n, _src, f in rc.BEATS}[beat]
        self.rec_anchor = None
        if anchor is not None:
            self.rec_anchor = next(r["f"] for r in self.doc["rows"] if owner_script(r, anchor[0]) == anchor[1])
        self.my_anchor = None
        self.prefix_identical = None

    def rec_f(self, f: int) -> int | None:
        """Recorded frame index of my frame f (anchor-aligned)."""
        if self.anchor is None:
            return f
        if self.my_anchor is None:
            return None
        return f - self.my_anchor + self.rec_anchor

    def run(self, hook) -> C7Session:
        src = beat_source_state(self.doc)
        target = self.doc["first_counter"]
        s = open_c7(src, self.log_dir)
        self.s = s
        self.loaded_counter = pine_reload(s, src)
        if self.loaded_counter > target:
            s.close()
            raise RuntimeError(f"loaded counter {self.loaded_counter} > recorded first counter {target}")
        s.pad(0)
        while s.u32(FRAME_COUNTER) < target:
            s.step(1)
        self.pre_frames = target - self.loaded_counter

        def wrapped(row: dict) -> None:
            if self.prefix_identical is None and row["f"] < len(self.doc["rows"]) \
                    and row != self.doc["rows"][row["f"]]:
                self.prefix_identical = row["f"]
            if self.anchor is not None and self.my_anchor is None and owner_script(row, self.anchor[0]) == self.anchor[1]:
                self.my_anchor = row["f"]
            hook(row)

        self.route = HookRoute(s, wrapped)
        self.route.begin()
        self.row0_equal = self.route.rows[0] == self.doc["rows"][0]
        hook(self.route.rows[0])
        try:
            self.fn(self.route)
            self.completed = True
        except StopBeat:
            self.completed = False
        return s

    def alignment(self, fs: list[int]) -> list[dict]:
        out = []
        for f in fs:
            rf = self.rec_f(f)
            if rf is None or f >= len(self.route.rows) or not (0 <= rf < len(self.doc["rows"])):
                continue
            mine, ref = self.route.rows[f], self.doc["rows"][rf]
            eq = {k: mine[k] == ref[k] for k in ALIGN_KEYS}
            eq["director_r12"] = mine["director_r12"]["s1F0"] == ref["director_r12"]["s1F0"]
            eq["roger_script"] = rc.script_ptr(mine["roger_r8"]) == rc.script_ptr(ref["roger_r8"])
            out.append({"f": f, "rec_f": rf, "equal": eq, "all_equal": all(eq.values())})
        return out

    def summary(self) -> dict:
        return {"beat": self.beat, "source": self.doc["source"], "method": "closed-loop beat function",
                "recorded_first_counter": self.doc["first_counter"], "loaded_counter": self.loaded_counter,
                "neutral_frames_before_f0": self.pre_frames, "row0_equal_recorded": self.row0_equal,
                "rows_identical_to_recording_before_f": self.prefix_identical,
                "anchor": [self.anchor[0], hex(self.anchor[1])] if self.anchor else None,
                "anchor_f_recorded": self.rec_anchor, "anchor_f_mine": self.my_anchor,
                "frames_run": self.route.frame_index, "beat_completed": self.completed}


def stream_events_arm(s: C7Session, events: list[dict], cur: dict, sampler: StreamSampler) -> None:
    def hit(pc: int) -> None:
        r = sampler.raw()
        events.append({"f": cur["f"], "pc": hex(pc), "what": STREAM_EVENTS[pc],
                       "counter": u32(r["counter"]), "vsync": u32(r["vsync"]),
                       "ra": hex(s.reg("ra")), "a0": hex(s.reg("a0")), "a1": hex(s.reg("a1"))})
    for a in STREAM_EVENTS:
        s.arm(a, hit)


def stream_route(key: str) -> dict:
    beat, anchor, (w0, w1), end, what = STREAM_STRETCHES[key]
    out = OUT / "stream" / key
    out.mkdir(parents=True, exist_ok=True)
    cl = ClosedLoop(beat, anchor, out / "logs")
    fields, frames, events = [], [], []
    cur = {"f": 1}
    st: dict = {}
    t0 = time.monotonic()

    def hook(row: dict) -> None:
        s = cl.s
        if "sampler" not in st:
            st["sampler"] = StreamSampler(s)
            stream_events_arm(s, events, cur, st["sampler"])
        sampler = st["sampler"]
        f = row["f"]
        rf = cl.rec_f(f)
        rec = sampler.record("top", f, full=False)
        rec["rec_f"] = rf
        frames.append(rec)
        if rf is not None and w0 <= rf <= w1:
            full = sampler.record("top", f)
            full["rec_f"] = rf
            fields.append(full)
        nxt = cl.rec_f(f + 1)
        cur["f"] = f + 1
        if nxt == w0:
            def on_isr(pc: int) -> None:
                x = sampler.record("isr", cur["f"])
                x["rec_f"] = cl.rec_f(cur["f"])
                fields.append(x)
            s.arm(ISR, on_isr)
        if nxt == w1 + 1:
            s.disarm(ISR)
        if rf is not None and rf >= end:
            raise StopBeat

    try:
        s = cl.run(hook)
    finally:
        if getattr(cl, "s", None) is not None:
            cl.s.close()
    for rec in fields + frames + events:          # anchor known now: map every record
        rec["rec_f"] = cl.rec_f(rec["f"])
    write_jsonl(out / "fields.jsonl", fields)
    write_jsonl(out / "frames.jsonl", frames)
    write_jsonl(out / "events.jsonl", events)
    probe = []
    if cl.my_anchor is not None:
        probe = [cl.my_anchor + d for d in (-1, 0, 1, 2, 3)] + \
                [cl.my_anchor + (x - cl.rec_anchor) for x in (w0, (w0 + w1) // 2, w1, end - 15, end - 14)]
    meta = {"item": "stream", "stretch": key, "what": what, "field_window_recorded_f": [w0, w1],
            "per_frame_until_recorded_f": end, "run": cl.summary(), "alignment": cl.alignment(probe),
            "seconds": round(time.monotonic() - t0, 1), "kicks": s.kicks, "unexpected": s.unexpected}
    (out / "meta.json").write_text(json.dumps(meta, indent=1) + "\n")
    print(key, json.dumps(meta["run"]), flush=True)
    return meta


def stream_opening() -> dict:
    out = OUT / "stream" / "opening"
    out.mkdir(parents=True, exist_ok=True)
    s = open_c7(rc.slot_path("01"), out / "logs")
    fields, frames, events = [], [], []
    cur = {"f": 0}
    sampler = StreamSampler(s)
    t0 = time.monotonic()
    anchors: dict = {}
    try:
        s.pad(0)
        start = startup_title_to_area(s)
        # From the AREA11 running frame: per-field records until the fade-in
        # (3B92 == 1) + 20 frames, per-frame records for 120 more.
        stream_events_arm(s, events, cur, sampler)
        s.arm(ISR, lambda pc: fields.append(sampler.record("isr", cur["f"])))
        base = s.u32(FRAME_COUNTER)
        frames.append(sampler.record("top", 0, full=False))
        fields.append(sampler.record("top", 0))
        n, fade_in_at, field_on = 0, None, True
        while True:
            n += 1
            cur["f"] = n
            s.step(1)
            rec = sampler.record("top", n, full=field_on)
            (fields if field_on else frames).append(rec)
            if field_on:
                frames.append({k: v for k, v in rec.items() if k not in ("lanes_raw", "ring", "ringidx", "iop")})
            sp = bytes.fromhex(rec["spad"])
            if "request" not in anchors and sp[1] == 0x02:
                anchors["request"] = {"n": n, "counter": rec["counter"], "vsync": rec["vsync"]}
            if fade_in_at is None and sp[6] == 0x01:
                fade_in_at = n
                anchors["fade_in"] = {"n": n, "counter": rec["counter"], "vsync": rec["vsync"]}
            if field_on and fade_in_at is not None and n >= fade_in_at + 20:
                s.disarm(ISR)
                field_on = False
            if fade_in_at is not None and n >= fade_in_at + 140:
                break
            if n > 600:
                raise TimeoutError("no fade-in within 600 frames of the AREA11 start")
    finally:
        s.close()
    # newgame_samples numbering: request frame == 2642
    if "request" in anchors:
        off = 2642 - anchors["request"]["counter"]
        anchors["newgame_samples_offset"] = off
        for rec in fields + frames + events:
            rec["ng"] = rec["counter"] + off
    write_jsonl(out / "fields.jsonl", fields)
    write_jsonl(out / "frames.jsonl", frames)
    write_jsonl(out / "events.jsonl", events)
    meta = {"item": "stream", "stretch": "opening", "startup": start, "area11_running_counter": base,
            "anchors": anchors, "seconds": round(time.monotonic() - t0, 1),
            "kicks": s.kicks, "unexpected": s.unexpected}
    (out / "meta.json").write_text(json.dumps(meta, indent=1) + "\n")
    print("opening", json.dumps(anchors), flush=True)
    return meta


# ---------------------------------------------------------------------------
# Item 2: LANE3

def lane3_body() -> bytes:
    return b"".join(struct.pack("<BI", 2, LANE3 + slot * 0x60 + 0x40 + i)
                    for slot in range(32) for i in range(0, 16, 4))


def lane3_run(start_from: str, mode: str) -> dict:
    """mode 'bracket': no memchecks; the 32 quadwords are sampled at every
    vsync-ISR entry and every main-loop top (so a change is bracketed to
    one field, also inside the intro movie, which plays within one main-loop
    frame).  mode 'pinpoint': the same run again, with the 32 write
    memchecks armed only across the samples where the bracket run saw a
    change; every hit pauses and records the store's PC, the frame and a
    backtrace (entry/pc pairs only).  Memchecks armed for the whole run are
    impractical: the region is rewritten continuously while the movie plays
    (about 9 pauses per second of host time, measured and abandoned)."""
    out = OUT / "lane3" / start_from
    out.mkdir(parents=True, exist_ok=True)
    writes, changes, samples = [], [], []
    body = lane3_body()
    state = {"prev": None, "idx": 0, "armed": False, "hits": 0, "k": 0, "field_hits": 0}
    MAX_HITS_PER_FIELD = 600
    sess: dict = {}
    arm_at: set[tuple] = set()
    disarm_at: set[tuple] = set()
    chosen: list[dict] = []
    if mode == "pinpoint":
        # Samples are keyed (frames stepped, ISR count since the last
        # main-loop top, label), which does not depend on the start counter.
        # Memchecks are armed over the field before each chosen bracket
        # change: the first 3, 3 in the middle and the last 3 changes.
        ref = json.loads((out / "bracket_samples.json").read_text())
        ch = [json.loads(line) for line in (out / "bracket_changes.jsonl").read_text().splitlines()]
        ch = [c for c in ch if c["idx"] > 0]
        mid = len(ch) // 2
        pick = ch[:3] + ch[mid - 1:mid + 2] + ch[-3:]
        seen_idx = set()
        for c in pick:
            if c["idx"] in seen_idx:
                continue
            seen_idx.add(c["idx"])
            arm_at.add(tuple(ref[c["idx"] - 1][4]))
            disarm_at.add(tuple(ref[c["idx"]][4]))
            chosen.append({"bracket_idx": c["idx"], "key": ref[c["idx"]][4], "vsync": c["vsync"]})

    def arm_memchecks(on: bool) -> None:
        s = sess["s"]
        if on == state["armed"]:
            return
        for slot in range(32):
            a = LANE3 + slot * 0x60 + 0x40
            if on:
                s.debug.call({"cmd": "set_memcheck", "address": a, "end": a + 16, "type": "write",
                              "description": f"lane3 slot {slot}"})
            else:
                s.debug.call({"cmd": "remove_memcheck", "address": a, "end": a + 16})
        state["armed"] = on

    hits_seen: dict[int, int] = {}

    def on_memcheck(pc: int) -> None:
        s = sess["s"]
        mcs = s.debug.call({"cmd": "list_memchecks"})["memchecks"]
        which = []
        for m in mcs:
            st = int(m["start"], 16)
            if hits_seen.get(st, 0) != m["hits"]:
                which.append({"slot": (st - LANE3 - 0x40) // 0x60, "hits": m["hits"],
                              "last_pc": m["last_pc"], "last_addr": m["last_addr"]})
                hits_seen[st] = m["hits"]
        try:
            bt = s.debug.call({"cmd": "get_backtrace", "max_frames": 8})["frames"]
            bt = [{"entry": f["entry"], "pc": f["pc"]} for f in bt]      # no disassembly kept
        except Exception as exc:
            bt = [repr(exc)]
        c = s.u32(FRAME_COUNTER) if s.pine else None
        v = s.u32(VSYNC_COUNTER) if s.pine else None
        state["field_hits"] += 1
        if state["field_hits"] >= MAX_HITS_PER_FIELD:      # runaway guard: stop this field's memchecks
            arm_memchecks(False)
            state["capped"] = state.get("capped", 0) + 1
        writes.append({"sample_idx_next": state["idx"], "counter": c, "vsync": v,
                       "pause_pc": hex(pc), "ra": hex(s.reg("ra")), "memchecks": which, "backtrace": bt})
        state["hits"] += 1

    def sample(label: str) -> None:
        s = sess["s"]
        if s.pine is None:
            return
        idx = state["idx"]
        cur = s.pine.request(body)
        c, v = s.u32(FRAME_COUNTER), s.u32(VSYNC_COUNTER)
        state["k"] = state["k"] + 1 if label == "isr" else 0
        key = [s.frames_stepped, state["k"], label]
        samples.append([idx, label, c, v, key])
        if cur != state["prev"]:
            diff = [slot for slot in range(32) if state["prev"] is None
                    or cur[slot * 16:(slot + 1) * 16] != state["prev"][slot * 16:(slot + 1) * 16]]
            changes.append({"idx": idx, "phase": label, "frame": s.frames_stepped, "counter": c, "vsync": v,
                            "slots_changed": diff, "memcheck_pauses_since_last_sample": state["hits"],
                            "nonzero_slots": [slot for slot in range(32) if any(cur[slot * 16:(slot + 1) * 16])],
                            "values": {str(slot): cur[slot * 16:(slot + 1) * 16].hex() for slot in diff}})
            state["prev"] = cur
        state["hits"] = 0
        state["idx"] = idx + 1
        if mode == "pinpoint":
            if tuple(key) in disarm_at:
                arm_memchecks(False)
            if tuple(key) in arm_at:
                state["field_hits"] = 0
                arm_memchecks(True)

    def on_isr(pc: int) -> None:
        sample("isr")

    t0 = time.monotonic()
    info: dict = {"mode": mode}
    if start_from == "title":
        s = open_c7(rc.slot_path("01"), out / "logs" / mode)
        sess["s"] = s
        s.on_other = on_memcheck
        try:
            s.pad(0)
            sample("title_start")
            s.arm(ISR, on_isr)
            info.update(startup_title_to_area(s, on_frame=sample))
            seen_cut, n = False, 0
            sampler = rc.Sampler(s)
            while True:
                s.step(1)
                sample("S2")
                row = rc.decode(sampler.raw())
                seen_cut = seen_cut or rc.cutscene(row)
                n += 1
                if seen_cut and rc.in_control(row):
                    break
                if n > 6000:
                    raise TimeoutError("no first control")
            info["first_control_counter"] = s.u32(FRAME_COUNTER)
            for _ in range(30):
                s.step(1)
                sample("S3")
        finally:
            s.close()
    else:
        def prepare(sx: C7Session) -> None:
            sess["s"] = sx
            sx.on_other = on_memcheck
            sx.arm(ISR, on_isr)
            if mode == "pinpoint":
                arm_memchecks(True)          # boot: armed until the first sample
        s = ColdSession(prepare, out / "logs" / mode)
        sess["s"] = s
        try:
            s.__enter__()
            info.update(boot=s.boot_notes, first_boundary_counter=s.u32(FRAME_COUNTER),
                        memcheck_pauses_before_first_boundary=len(writes))
            sample("boot_first_boundary")
            n, title_at = 0, None
            while True:
                s.step(1)
                sample("boot")
                n += 1
                t = s.read(0x28A750, 8)
                if title_at is None and u32(t, 4) == 0x1AC070 and s.u32(0x821058) == 0:
                    title_at = s.u32(FRAME_COUNTER)
                if title_at is not None and s.u32(FRAME_COUNTER) >= title_at + 90:
                    break
                if n > 5000:
                    raise TimeoutError("title not reached")
            info["title_counter"] = title_at
            info["end_counter"] = s.u32(FRAME_COUNTER)
        finally:
            s.close()
    if mode == "pinpoint":
        info["pinpoint_fields"] = chosen
        info["fields_capped"] = state.get("capped", 0)
    prefix = "bracket" if mode == "bracket" else "pinpoint"
    write_jsonl(out / f"{prefix}_changes.jsonl", changes)
    (out / f"{prefix}_samples.json").write_text(json.dumps(samples, separators=(",", ":")) + "\n")
    if mode == "pinpoint":
        write_jsonl(out / "writes.jsonl", writes)
    meta = {"item": "lane3", "from": start_from, "info": info, "samples": len(samples),
            "memcheck_pauses": len(writes), "change_samples": len(changes),
            "seconds": round(time.monotonic() - t0, 1), "kicks": s.kicks, "unexpected": s.unexpected}
    (out / f"{prefix}_meta.json").write_text(json.dumps(meta, indent=1) + "\n")
    print("lane3", start_from, mode, json.dumps(meta)[:800], flush=True)
    return meta


# ---------------------------------------------------------------------------
# Item 3: RNG

class RandTracer:
    """Exec breakpoints on rand / srand (and optional markers).  Each rand
    call records the caller's return address, the frame, the vsync, the
    state word before the call and the value returned, computed from the
    byte-matched C (state * 1103515245 + 12345, & 0x7FFFFFFF).  The first
    `verify` calls are also checked against v0 at the return address."""

    def __init__(self, s: C7Session, cur: dict, verify: int = 25):
        self.s, self.cur = s, cur
        self.rows: list[dict] = []
        self.verify_left = verify
        self.verified, self.verify_fail = 0, []
        self.chain_breaks: list[dict] = []
        self.expect: int | None = None
        blk = s.u32(REENT_PTR)
        self.state_addr = blk + 0x58
        self.body = b"".join(struct.pack("<BI", 2, a) for a in (self.state_addr, FRAME_COUNTER, VSYNC_COUNTER))
        self.markers: dict[int, str] = {}

    def _read(self) -> tuple[int, int, int]:
        d = self.s.pine.request(self.body)
        return u32(d, 0), u32(d, 4), u32(d, 8)

    def on_rand(self, pc: int) -> None:
        st, c, v = self._read()
        ra = self.s.reg("ra")
        new = (st * 1103515245 + 12345) & 0xFFFFFFFF
        ret = new & 0x7FFFFFFF
        row = {"f": self.cur["f"], "c": c, "v": v, "ra": f"{ra:06x}", "s": f"{st:08x}", "r": f"{ret:08x}"}
        if self.expect is not None and st != self.expect:
            self.chain_breaks.append({"f": self.cur["f"], "c": c, "expected": f"{self.expect:08x}",
                                      "found": f"{st:08x}"})
            row["chain_break"] = 1
        self.expect = new
        if self.verify_left > 0 and ra not in self.s.handlers:
            self.verify_left -= 1
            self.s.debug.call({"cmd": "set_breakpoint", "address": ra, "temporary": True})
            self.s.debug.call({"cmd": "resume"})
            pc2 = self.s._wait_pause(60)
            if pc2 != ra:
                self.verify_fail.append({"f": self.cur["f"], "ra": hex(ra), "paused_at": hex(pc2)})
                self.s.debug.call({"cmd": "remove_breakpoint", "address": ra})
                h = self.s.handlers.get(pc2)
                if h is not None and pc2 != LOOP_TOP:
                    h(pc2)
            else:
                v0 = self.s.reg("v0")
                self.s.debug.call({"cmd": "remove_breakpoint", "address": ra})
                if v0 == ret:
                    self.verified += 1
                else:
                    self.verify_fail.append({"f": self.cur["f"], "ra": hex(ra), "v0": hex(v0), "ret": hex(ret)})
        self.rows.append(row)

    def on_srand(self, pc: int) -> None:
        _st, c, v = self._read()
        a0 = self.s.reg("a0")
        self.rows.append({"f": self.cur["f"], "c": c, "v": v, "srand": f"{a0:08x}",
                          "ra": f"{self.s.reg('ra'):06x}"})
        self.expect = a0

    def on_marker(self, pc: int) -> None:
        _st, c, v = self._read()
        self.rows.append({"f": self.cur["f"], "c": c, "v": v, "mark": self.markers[pc],
                          "ra": f"{self.s.reg('ra'):06x}", "a0": f"{self.s.reg('a0'):x}"})

    def arm(self, markers: dict[int, str] | None = None) -> None:
        self.expect = self._read()[0]            # the chain check starts at the armed state
        self.state_at_arm = f"{self.expect:08x}"
        self.s.arm(RAND, self.on_rand)
        self.s.arm(SRAND, self.on_srand)
        for a, name in (markers or {}).items():
            self.markers[a] = name
            self.s.arm(a, self.on_marker)

    def disarm(self) -> None:
        for a in [RAND, SRAND, *self.markers]:
            self.s.disarm(a)

    def summary(self) -> dict:
        calls = [r for r in self.rows if "r" in r]
        return {"state_word": hex(self.state_addr), "state_at_arm": getattr(self, "state_at_arm", None),
                "rand_calls": len(calls),
                "srand_calls": sum(1 for r in self.rows if "srand" in r),
                "returns_verified_at_ra": self.verified, "verify_failures": self.verify_fail,
                "chain_breaks": self.chain_breaks[:20], "chain_break_count": len(self.chain_breaks)}


RNG_MARKERS = {AREA_ENTRY: "001AFCF0 (0x1AE040 state 0 area entry)", 0x1FAE70: "001FAE70 music cue select"}


def rng_newgame() -> dict:
    out = OUT / "rng" / "newgame"
    out.mkdir(parents=True, exist_ok=True)
    s = open_c7(rc.slot_path("01"), out / "logs")
    cur = {"f": 0}
    t0 = time.monotonic()
    info: dict = {}
    try:
        s.pad(0)
        # S0 without tracing; tracing starts once NEW GAME committed the area.
        def area() -> int:
            return s.read(0x810700, 4)[0]
        f0, presses = s.frames_stepped, 0
        while area() == 0:
            n = s.frames_stepped - f0
            if n in (5, 125, 245, 365) and presses < 4 and s.read(0x28A9A0, 4) == b"\0\0\0\0":
                s.pad(PAD["CROSS"]); presses += 1
            elif n % 120 in (9,):
                s.pad(0)
            s.step(1)
        s.pad(0)
        info["newgame_commit_counter"] = s.u32(FRAME_COUNTER)
        tr = RandTracer(s, cur)
        tr.arm(RNG_MARKERS)
        n = 0
        sampler = rc.Sampler(s)
        seen_cut, fc_n, labels = False, None, []
        area_running = None
        while True:
            n += 1
            cur["f"] = n
            s.step(1)
            if area_running is None and s.read(0x810700, 4)[0] == 0x0B and s.read(0x28A750, 0x10)[0xB] == 1:
                area_running = n
                labels.append({"n": n, "counter": s.u32(FRAME_COUNTER), "label": "AREA11 running (S2 start)"})
            if area_running is not None and fc_n is None:
                row = rc.decode(sampler.raw())
                seen_cut = seen_cut or rc.cutscene(row)
                if seen_cut and rc.in_control(row):
                    fc_n = n
                    labels.append({"n": n, "counter": row["counter"], "label": "first control"})
            if fc_n is not None and n >= fc_n + 300:
                break
            if n > 8000:
                raise TimeoutError("no first control")
        tr.disarm()
        info.update(labels=labels, first_control_n=fc_n)
    finally:
        s.close()
    write_jsonl(out / "rand.jsonl", tr.rows)
    meta = {"item": "rng", "stretch": "newgame", "frame_index": "n = frames stepped after NEW GAME committed the area",
            "info": info, "trace": tr.summary(), "seconds": round(time.monotonic() - t0, 1),
            "kicks": s.kicks, "unexpected": s.unexpected}
    (out / "meta.json").write_text(json.dumps(meta, indent=1) + "\n")
    print("rng newgame", json.dumps(meta["trace"])[:500], flush=True)
    return meta


RNG_ROUTE = {
    # beat, anchor, traced window in RECORDED frame indices, what
    "r01": ("01_battery", ("battery_g0_0", 0x266620), (1, 516), "the whole battery beat (anchor: pickup take script 0x266620, recorded f125)"),
    "r10": ("10_cage_roof_roger", ("director_r12", 0x8294C0), (1080, 1400),
            "director beat 0 start (script 0x8294C0, recorded f1089), Roger's conversation 0x828990, line 0x7F start"),
}


def rng_route(key: str) -> dict:
    beat, anchor, (w0, w1), what = RNG_ROUTE[key]
    out = OUT / "rng" / key
    out.mkdir(parents=True, exist_ok=True)
    cl = ClosedLoop(beat, anchor, out / "logs")
    cur = {"f": 1}
    st: dict = {}
    t0 = time.monotonic()

    def hook(row: dict) -> None:
        f = row["f"]
        cur["f"] = f + 1
        if "tr" not in st and f >= max(0, w0 - 60 - 1):
            # The anchor is not known before it happens: arm 60 frames ahead
            # of the window in my own frame numbering (a closed-loop run that
            # drifts further than that from the recording is reported by the
            # window check below) and cut to the recorded window afterwards.
            st["tr"] = RandTracer(cl.s, cur)
            st["tr"].arm(RNG_MARKERS)
            st["armed_f"] = f + 1
        rf = cl.rec_f(f)
        if rf is not None and rf >= w1:
            raise StopBeat

    try:
        cl.run(hook)
    finally:
        if getattr(cl, "s", None) is not None:
            cl.s.close()
    tr = st["tr"]
    first_traced_rec_f = cl.rec_f(st["armed_f"])
    if first_traced_rec_f is None or first_traced_rec_f > w0:
        print(f"WARNING: tracing began at recorded f{first_traced_rec_f}, after the window start f{w0}", flush=True)
    for r in tr.rows:
        r["rec_f"] = cl.rec_f(r["f"])
    kept = [r for r in tr.rows if r["rec_f"] is not None and w0 <= r["rec_f"] <= w1]
    write_jsonl(out / "rand.jsonl", kept)
    probe = []
    if cl.my_anchor is not None:
        probe = [cl.my_anchor + d for d in (-1, 0, 1, 5)] + [cl.my_anchor + (w1 - cl.rec_anchor) - 1]
    summ = tr.summary()
    summ["rand_calls_in_window"] = sum(1 for r in kept if "r" in r)
    meta = {"item": "rng", "stretch": key, "what": what, "window_recorded_f": [w0, w1],
            "traced_from_f": st["armed_f"], "traced_from_recorded_f": first_traced_rec_f,
            "run": cl.summary(), "alignment": cl.alignment(probe), "trace": summ,
            "seconds": round(time.monotonic() - t0, 1), "kicks": cl.s.kicks, "unexpected": cl.s.unexpected}
    (out / "meta.json").write_text(json.dumps(meta, indent=1) + "\n")
    print("rng", key, json.dumps(meta["run"]), json.dumps(summ)[:400], flush=True)
    return meta


# ---------------------------------------------------------------------------

def no_emulator_left() -> bool:
    return subprocess.run(["pgrep", "-f", "PCSX2.app/Contents/MacOS/PCSX2"],
                          capture_output=True).returncode != 0


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("item", choices=["stream", "lane3", "rng"])
    ap.add_argument("--stretch", default="all")
    ap.add_argument("--from", dest="start_from", default="title", choices=["title", "boot"])
    ap.add_argument("--mode", default="both", choices=["bracket", "pinpoint", "both"])
    a = ap.parse_args()
    try:
        if a.item == "stream":
            keys = ["opening", "r10", "r11", "r13"] if a.stretch == "all" else a.stretch.split(",")
            for k in keys:
                stream_opening() if k == "opening" else stream_route(k)
        elif a.item == "lane3":
            for m in (["bracket", "pinpoint"] if a.mode == "both" else [a.mode]):
                lane3_run(a.start_from, m)
        else:
            keys = ["newgame", "r01", "r10"] if a.stretch == "all" else a.stretch.split(",")
            for k in keys:
                rng_newgame() if k == "newgame" else rng_route(k)
    finally:
        print("no emulator process left:", no_emulator_left(), flush=True)
