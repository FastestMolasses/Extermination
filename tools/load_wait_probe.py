#!/usr/bin/env python3
"""load_wait_probe.py - per-dispatch I/O trace of the module loader (slot 2).

Replays route beats 01 (battery pop-up, status ITEM root) and 03 (panel,
BATTERY prompt) of tools/route_capture.py from their recorded source
snapshots in the hidden PCSX2 (tools/pcsx2_session.py) and characterizes
the 24-dispatch module-load wait of the port's docs/STATUS_SCENE.md
section 3.

Breakpoints (EE exec, addresses only):
  001FF080  load request: a0 = initial state, a1 = module id.  Its hit arms
            the rest; they are removed once slot 2 is idle again.
  001FF0D0  one loader dispatch: slot 2 record 0x28A790 (+8 state, +9 step,
            +0xA, +0xB sub-step, +0xE module, +0x14 count, +0x16 index) and
            D_00282157 at dispatch entry.
  00200780  read kick: a1 buffer, a2 byte offset, a3 byte size.
  00200730  poll: at entry the return address is read and a breakpoint is
            placed there, so the poll's return value (v0) is recorded at
            the return site of each caller.
At every frame boundary the same record, D_00282157 and D_00275BD8 are read.
Mode "boundary" arms none of the probe breakpoints and keeps only those reads.

The probe only pauses the EE and reads registers and memory; it never
writes memory.  Outputs go to the ignored build/s87/loadwait/<beat>/.
Nothing here embeds original code or data; it names addresses only.

Usage (decomp .venv python, repo root):
    .venv/bin/python tools/load_wait_probe.py run --beats 01,03 --mode boundary
    .venv/bin/python tools/load_wait_probe.py run --beats 01,03 --mode breakpoints
    .venv/bin/python tools/load_wait_probe.py table --beats 01,03 --mode boundary
"""
from __future__ import annotations

import argparse
import json
import shutil
import struct
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import route_capture as rc  # noqa: E402
from pcsx2_session import LOOP_TOP  # noqa: E402

OUT = ROOT / "build/s87/loadwait"
SLOT2 = 0x28A790
REQUEST, DISPATCH, READ, POLL = 0x1FF080, 0x1FF0D0, 0x200780, 0x200730


class ProbeSession(rc.RouteSession):
    """RouteSession whose frame step services the probe breakpoints."""

    def __init__(self, *a, **k):
        super().__init__(*a, **k)
        self.events: list[dict] = []
        self.detail = False
        self.return_sites: set[int] = set()
        self.pending_poll: dict | None = None
        self.gpr_cat: int | None = None

    # -- helpers -------------------------------------------------------------
    def regs(self) -> dict[str, int]:
        if self.gpr_cat is None:
            data = self.debug.call({"cmd": "read_registers"})["data"]
            names = [k for k in data if k not in ("pc", "hi", "lo")]
            self.gpr_cat = next(i for i, k in enumerate(names) if k.upper() == "GPR")
        data = self.debug.call({"cmd": "read_registers", "category": self.gpr_cat})["data"]
        cat = next(v for k, v in data.items() if k not in ("pc", "hi", "lo"))
        return {r["name"]: int(r["value"], 16) & 0xFFFFFFFF for r in cat["regs"]}

    def slot2(self) -> dict:
        b = self.read(SLOT2, 0x20)
        return {"state": b[0], "s8": b[8], "s9": b[9], "sA": b[0xA], "sB": b[0xB],
                "module": b[0xE], "kind": b[0xF], "count14": struct.unpack_from("<H", b, 0x14)[0],
                "index16": struct.unpack_from("<H", b, 0x16)[0]}

    def gates(self) -> dict:
        return {"d282157": self.read(0x282154, 4)[3], "bd8": self.read(0x275BD8, 4)[0]}

    def set_bp(self, addr: int) -> None:
        self.debug.call({"cmd": "set_breakpoint", "address": addr})

    def remove_bp(self, addr: int) -> None:
        self.debug.call({"cmd": "remove_breakpoint", "address": addr})

    def arm_request(self) -> None:
        self.set_bp(REQUEST)

    def arm_detail(self) -> None:
        if not self.detail:
            for a in (DISPATCH, READ, POLL):
                self.set_bp(a)
            self.detail = True

    def disarm_detail(self) -> None:
        if self.detail:
            for a in (DISPATCH, READ, POLL, *sorted(self.return_sites)):
                self.remove_bp(a)
            self.return_sites.clear()
            self.detail = False

    def _wait(self, timeout: float) -> int:
        deadline = time.monotonic() + timeout
        last, since = None, time.monotonic()
        while time.monotonic() < deadline:
            st = self.debug.call({"cmd": "status"})
            data = st.get("data", st)
            if data.get("paused"):
                return int(data["pc"], 16)
            # A breakpoint change can leave the VM stopped while it reports
            # "not paused" (route_census.py); resume again after 3 s frozen.
            now = time.monotonic()
            if data.get("cycles") != last:
                last, since = data.get("cycles"), now
            elif now - since > 3.0:
                self.debug.call({"cmd": "resume"})
                since = now
            time.sleep(0.002)
        raise TimeoutError("no pause")

    # -- frame step ----------------------------------------------------------
    def _resume_to_boundary(self, timeout: float | None = None) -> None:
        frame = self.frames_stepped + 1
        while True:
            self.debug.call({"cmd": "resume"})
            pc = self._wait(timeout or self.boundary_timeout)
            if pc == LOOP_TOP:
                return
            ev: dict = {"frame": frame, "pc": hex(pc)}
            if pc == REQUEST:
                r = self.regs()
                ev.update(kind="request", state=r["a0"] & 0xFF, module=r["a1"] & 0xFF,
                          ra=hex(r["ra"]))
                self.arm_detail()
            elif pc == DISPATCH:
                ev.update(kind="dispatch", slot2=self.slot2(), **self.gates())
            elif pc == READ:
                r = self.regs()
                ev.update(kind="read", buf=hex(r["a1"]), offset=hex(r["a2"]), size=hex(r["a3"]),
                          ra=hex(r["ra"]))
            elif pc == POLL:
                r = self.regs()
                site = r["ra"]
                ev.update(kind="poll_call", ra=hex(site), slot2=self.slot2())
                self.pending_poll = {"site": site, "frame": frame}
                if site not in self.return_sites:
                    self.set_bp(site)
                    self.return_sites.add(site)
            elif pc in self.return_sites:
                if self.pending_poll and self.pending_poll["site"] == pc:
                    r = self.regs()
                    ev.update(kind="poll_return", v0=r["v0"])
                    self.pending_poll = None
                else:
                    ev.update(kind="return_site_without_call")
            else:
                ev.update(kind="unexpected")
            self.events.append(ev)


def open_probe(state: Path, log_dir: Path, attempts: int = 6) -> ProbeSession:
    for attempt in range(attempts):
        rc.wait_for_free_emulator()
        s = ProbeSession(state, log_dir=log_dir)
        try:
            return s.__enter__()
        except (RuntimeError, TimeoutError, OSError, EOFError) as exc:
            print(f"start attempt {attempt + 1} failed: {exc}", flush=True)
            time.sleep(2)
    raise RuntimeError(f"emulator did not start from {state}")


def run(name: str, source: str, fn, mode: str = "breakpoints") -> Path:
    """mode 'breakpoints': the per-dispatch probe above.  mode 'boundary':
    no probe breakpoints at all, only the per-frame reads at the frame
    boundary (the mid-frame pauses of the breakpoint mode shift the disc
    timing, so the unperturbed frame counts come from this mode)."""
    src = rc.beat_source(source)
    out = OUT / name
    out.mkdir(parents=True, exist_ok=True)
    s = open_probe(src, OUT / "logs" / (name + "_" + mode))
    frames: list[dict] = []
    try:
        if mode == "breakpoints":
            s.arm_request()
        r = rc.Route(s)
        r.begin()
        plain_step = r.step

        def step(n: int = 1) -> dict:
            row = None
            for _ in range(n):
                row = plain_step(1)
                g = s.gates()
                frames.append({"f": r.frame_index, "counter": row["counter"], "slot2": s.slot2(),
                               "ui": row["ui"], **g})
                if s.detail and g["bd8"] == 0 and s.slot2()["state"] == 0:
                    s.disarm_detail()
            return row
        r.step = step
        meta = fn(r)
        recorded = json.loads((rc.OUT / name / "trace.json").read_text())
        same = sum(1 for a, b in zip(r.rows, recorded["rows"]) if a == b)
        doc = {"beat": name, "source": source, "mode": mode, "what": meta.get("what"),
               "frames": r.frame_index, "first_differing_row": next(
                   (i for i, (x, y) in enumerate(zip(r.rows, recorded["rows"])) if x != y), None),
               "trace_rows_identical_to_recorded": same, "recorded_rows": len(recorded["rows"]),
               "rows_compared": min(len(r.rows), len(recorded["rows"])),
               "events": s.events, "frame_rows": frames}
        (out / f"probe_{mode}.json").write_text(json.dumps(doc, indent=1) + "\n")
    finally:
        s.close()
        shutil.rmtree(rc.OUT / "_resume", ignore_errors=True)
    print(name, mode, "->", out / f"probe_{mode}.json", flush=True)
    return out


# Poll steps of the loader: (+9, +0xB) -> the step the poll result 1 moves to.
POLL_STEPS = {(1, None): "header", (4, None): "payload tail", (2, 2): "chunk"}


def boundary_table(name: str) -> None:
    """Per-frame table from the boundary-only run: the state after each
    frame's single slot-2 dispatch, and the poll result that the transition
    implies (0 stays, 1 advances, 2 steps back)."""
    doc = json.loads((OUT / name / "probe_boundary.json").read_text())
    rows = doc["frame_rows"]
    print(f"== {name} (boundary): first row differing from the recorded trace: "
          f"{doc['first_differing_row']} of {doc['rows_compared']}")
    prev = None
    for r in rows:
        q = r["slot2"]
        active = r["bd8"] or (prev is not None and prev["bd8"])
        if active:
            note = ""
            if prev is not None:
                p = prev["slot2"]
                if p["s8"] == 0 and p["s9"] in (1, 4) and prev["bd8"]:
                    note = ("poll 1" if q["s9"] == p["s9"] + 1 else
                            "poll 0 (or gated)" if q["s9"] == p["s9"] else "poll 2")
                elif p["s8"] == 0 and p["s9"] == 2 and p["sB"] == 2:
                    note = ("poll 1" if q["sB"] == 3 else
                            "poll 0 (or gated)" if q["sB"] == 2 else "poll 2")
            print(f"f{r['f']} c{r['counter']}: +8={q['s8']:#x} +9={q['s9']} +B={q['sB']} "
                  f"+16={q['index16']} 157={r['d282157']} BD8={r['bd8']} ui={r['ui']} {note}")
        prev = r


def table(name: str) -> None:
    doc = json.loads((OUT / name / "probe_breakpoints.json").read_text())
    by_frame: dict[int, list[dict]] = {}
    for ev in doc["events"]:
        by_frame.setdefault(ev["frame"], []).append(ev)
    reqs = sorted(f for f, evs in by_frame.items() if any(e["kind"] == "request" for e in evs))
    print(f"== {name}: rows identical to recorded {doc['trace_rows_identical_to_recorded']}"
          f"/{doc['rows_compared']}; requests at {reqs}")
    rows = {r["f"]: r for r in doc["frame_rows"]}
    for f in sorted(by_frame):
        evs = by_frame[f]
        parts = []
        for e in evs:
            k = e["kind"]
            if k == "request":
                parts.append(f"REQ(state {e['state']}, module {e['module']:#x})")
            elif k == "dispatch":
                q = e["slot2"]
                parts.append(f"D[+8={q['s8']:#x} +9={q['s9']} +A={q['sA']} +B={q['sB']} "
                             f"+16={q['index16']} 157={e['d282157']}]")
            elif k == "read":
                parts.append(f"READ(off {e['offset']}, size {e['size']})")
            elif k == "poll_call":
                parts.append(f"poll@{e['ra']}")
            elif k == "poll_return":
                parts.append(f"v0={e['v0']}")
            else:
                parts.append(k)
        after = rows.get(f, {})
        s2 = after.get("slot2", {})
        print(f"f{f} c{after.get('counter')}: " + " ".join(parts)
              + f" | after: +8={s2.get('s8', 0):#x} +9={s2.get('s9')} +B={s2.get('sB')}"
                f" 157={after.get('d282157')} BD8={after.get('bd8')}")


BEATS = {"01": ("01_battery", "04", rc.beat_battery),
         "03": ("03_panel_power", "02_elevator_refusal", rc.beat_panel_power)}

if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("command", choices=["run", "table"])
    ap.add_argument("--beats", default="01,03")
    ap.add_argument("--mode", choices=["breakpoints", "boundary"], default="boundary")
    a = ap.parse_args()
    for key in a.beats.split(","):
        name, source, fn = BEATS[key]
        if a.command == "run":
            run(name, source, fn, a.mode)
        elif a.mode == "boundary":
            boundary_table(name)
        else:
            table(name)
