#!/usr/bin/env python3
"""route_census.py - which ORIGINAL functions run along the AREA11 first-level route.

Replays the route of tools/route_capture.py (hidden PCSX2 through
tools/pcsx2_session.py, exact one-frame steps, pad input only) with an exec
breakpoint on every candidate function entry:

  * boot ELF: every row of docs/FUNCTIONS.csv (address, size, name);
  * AREA11 overlay: every splat function of build/overlays/AREA11 (runtime
    address = splat label + 0x40, because the loader copies the MWo3 header
    to 0x823500 and the text follows it).

Each breakpoint is one-shot: on a hit the address is recorded with the beat
and frame and the breakpoint is removed, so every function costs one pause
per beat.  All breakpoints are re-armed at the start of every beat, which
gives each beat its own complete set.

Segments (one emulator session each; a segment is split into labels, and
the breakpoints are re-armed at every label):
  startup  user slot 01 (title) -> Cross on NEW GAME -> movie + AREA11 load
           -> opening cinematic -> first control -> 200 idle frames
           (labels S0_title, S1_newgame_load, S2_opening,
           S3_first_control_idle; every frame is sampled into
           runs/<pass>/traces/ so two passes can be compared row by row);
  00..14   route_capture's beats, each from its RECORDED source snapshot
           (build/s87/route/<source>/state.p2s, or user slot 04), driven by
           route_capture's own closed-loop beat function and compared with
           the recorded trace and end snapshot.
  15       the level exit (15_level_exit), OPT-IN: `--segments all` leaves it
           out; run it by name (`--segments 15`, ~500 s, the departure FMV
           plays inside one frame).  `report` never reads it, so the
           first-level outputs stay beats 00..14 (its hits include the AREA01
           arrival and overlay-2 code at AREA11 candidate addresses);
           `exit-delta` reports what it adds beyond every earlier label.

Outputs (ignored): build/s87/census/{candidates.json, route_functions.json,
per_beat.json, runs/<pass>/<label>.json}.  Nothing here embeds original code
or data; it names addresses only.  Source save states are hashed before/after
by pcsx2_session; temporary copies live in build/s87/census/_resume/ and are
deleted after each session.  The emulator runs hidden and is closed after
every segment; no save-state slot is written.

Inputs: docs/FUNCTIONS.csv, src/ markers, build/s87/census/provenance.json
(tools/decomp/audit_link_provenance.py --output ...), the AREA11 overlay
splat output under build/overlays/AREA11 and extract/OVERLAY/AREA11.BIN
(header text size only), and the route captures of tools/route_capture.py.

Usage (decomp .venv python, repo root):
    .venv/bin/python tools/route_census.py candidates
    .venv/bin/python tools/route_census.py run --segments all [--pass A]   # startup + 00..14
    .venv/bin/python tools/route_census.py run --segments startup --pass B [--arm-chunk 100]
    .venv/bin/python tools/route_census.py compare-startup --passes A,B
    .venv/bin/python tools/route_census.py report --passes A,B   # first pass is primary; S0..S3 + 00..14
    .venv/bin/python tools/route_census.py run --segments 15 --pass A   # beat 15, level exit
    .venv/bin/python tools/route_census.py exit-delta --passes A,B      # beat 15's new functions
    .venv/bin/python tools/route_census.py run --segments a01 --pass A01  # AREA01 beats (opt-in)
    .venv/bin/python tools/route_census.py a01-delta --passes A01         # AREA01 beyond the first level
    .venv/bin/python tools/route_census.py run --segments a00 --pass A00  # AREA00 beats (opt-in)
    .venv/bin/python tools/route_census.py a00-delta --passes A00 [--a01-passes A01]  # AREA00 beyond both
    .venv/bin/python tools/route_census.py run --segments a01r --pass A01R  # AREA01 revisit (opt-in)
    .venv/bin/python tools/route_census.py run --segments a02 --pass A02    # AREA02 beats (opt-in)
    .venv/bin/python tools/route_census.py a02-delta --passes A02 --a01r-passes A01R --a00-passes A00 --a01-passes A01
                                                     # the revisit and AREA02 beyond all three
    .venv/bin/python tools/route_census.py run --segments a04 --pass A04    # AREA04 beats (opt-in)
    .venv/bin/python tools/route_census.py a04-delta --passes A04 --a02-passes A02 --a01r-passes A01R \
                                                     --a00-passes A00 --a01-passes A01
                                                     # AREA04 beyond all of the above
    .venv/bin/python tools/route_census.py run --segments a22 --pass A22    # AREA22 beats (opt-in)
    .venv/bin/python tools/route_census.py a22-delta --passes A22 --a04-passes A04 --a02-passes A02 \
                                                     --a01r-passes A01R --a00-passes A00 --a01-passes A01
                                                     # AREA22 beyond all of the above
    .venv/bin/python tools/route_census.py run --segments a01u --pass A01U  # AREA01 upper floor (opt-in)
    .venv/bin/python tools/route_census.py a01u-delta --passes A01U          # beyond every earlier group
    .venv/bin/python tools/route_census.py run --segments a06 --pass A06    # AREA06 beats (opt-in)
    .venv/bin/python tools/route_census.py a06-delta --passes A06           # beyond every earlier group
                                                     # (earlier passes: --a01-passes .. --a22-passes,
                                                     # --a01u-passes; defaults A01 .. A22, A01U)
The a01r_* segments arm the boot functions plus the AREA01 overlay, the a02_*
segments the boot functions plus the AREA02 overlay (docs/FOURTH_LEVEL_ROUTE.md),
the a04_* segments the boot functions plus the AREA04 overlay
(docs/FIFTH_LEVEL_ROUTE.md), the a22_* segments the boot functions plus the
AREA22 overlay pieces (docs/SIXTH_LEVEL_ROUTE.md), the a01u_* segments the boot
functions plus the AREA01 overlay and the a06_* segments the boot functions
plus the AREA06 overlay (docs/SEVENTH_LEVEL_ROUTE.md).
The a01_* segments (route_capture's AREA01 group, docs/SECOND_LEVEL_ROUTE.md in
the port) arm the boot functions plus the AREA01 overlay, the a00_* segments
(docs/THIRD_LEVEL_ROUTE.md) the boot functions plus the AREA00 overlay;
`report` and `exit-delta` never read them.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import os
import re
import shutil
import socket
import struct
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import route_capture as rc  # noqa: E402
from pcsx2_session import FRAME_COUNTER, LOOP_TOP, PAD, OriginalSession  # noqa: E402

OUT = ROOT / "build/s87/census"
ROUTE = ROOT / "build/s87/route"
OVERLAY_BASE = 0x823500
OVERLAY_TEXT = 0x823540          # runtime start of the overlay .text
AREA11_ID = 9
ARM_CHUNK = 200                  # breakpoint commands per round trip (--arm-chunk)
SOCKET_TIMEOUT = 300.0           # DebugServer reply timeout; arming 3000 entries mid-load is slow


# ---------------------------------------------------------------------------
# Candidates

def _asm_kind(path: Path) -> str:
    text = path.read_text(errors="replace")
    body = text[text.find("asm "):] if "asm " in text else text
    lines = [ln.strip() for ln in body.splitlines()[1:]
             if ln.strip() and not ln.strip().startswith(("//", "}"))]
    words = sum(1 for ln in lines if ln.startswith(".word"))
    if "All-word" in text or (lines and words * 2 >= len(lines)):
        return "asm_word"
    return "asm_inline"


def boot_status() -> dict[str, dict]:
    """Per function name: decomp status from the src markers plus the link
    provenance audit (tools/decomp/audit_link_provenance.py) when present."""
    prov_path = OUT / "provenance.json"
    prov = {}
    if prov_path.exists():
        prov = {r["name"]: r for r in json.loads(prov_path.read_text())["rows"]}
    out = {}
    for row in csv.DictReader(open(ROOT / "docs/FUNCTIONS.csv")):
        name = row["name"]
        src = ROOT / "src" / f"{name}.c"
        p = prov.get(name, {})
        route = p.get("route", "")
        perfect = p.get("objdiff_perfect")
        if not src.exists():
            status = "missing_source"
        else:
            first = next((ln.strip() for ln in src.read_text(errors="replace").splitlines()
                          if ln.strip()), "")
            if first.startswith("// INCLUDE_ASM"):
                status = "asm_undecompiled"
            elif first.startswith("// NEARMISS"):
                status = "nearmiss"
            elif p.get("source") == "inline_assembly" or re.search(
                    r"^\s*asm\s+\w", src.read_text(errors="replace"), re.M):
                status = _asm_kind(src)
            elif route.startswith("compiled_object_") and perfect:
                status = "byte_matched_c"
            elif route.startswith("compiled_object_"):
                status = "c_nonperfect"
            elif route.startswith("original_assembly_"):
                status = "c_linked_from_asm"
            else:
                status = "c_unknown_link"
        out[name] = {"status": status, "link_route": route or None,
                     "objdiff_perfect": perfect, "csv_status": row["status"],
                     "subsystem": row["subsystem"]}
    return out


def candidates(overlay: str = "AREA11") -> list[dict]:
    """Boot functions plus one overlay's splat functions (AREA11 for the
    first level; AREA01 for the opt-in a01_* segments)."""
    status = boot_status()
    cands = []
    for row in csv.DictReader(open(ROOT / "docs/FUNCTIONS.csv")):
        name = row["name"]
        cands.append({"addr": int(row["vram"], 16), "name": name, "size": int(row["size_bytes"]),
                      "region": "boot", **status[name]})
    code = ROOT / f"build/overlays/{overlay}/asm/matchings/{overlay}/code"
    blob = (ROOT / f"extract/OVERLAY/{overlay}.BIN").read_bytes()
    text_size = struct.unpack_from("<I", blob, 0xC)[0]
    text_end = OVERLAY_TEXT + text_size
    rows = []
    for f in sorted(os.listdir(code)):
        m = re.search(r"([0-9A-F]{8})\.s$", f)
        if m:
            rows.append((int(m.group(1), 16) + 0x40, f[:-2]))
    rows.sort()
    for i, (addr, name) in enumerate(rows):
        size = (rows[i + 1][0] if i + 1 < len(rows) else text_end) - addr
        src = ROOT / f"src/overlays/{overlay}" / f"{name}.c"
        if not src.exists():
            st = "asm_undecompiled"
        else:
            first = next((ln.strip() for ln in src.read_text(errors="replace").splitlines()
                          if ln.strip()), "")
            if first.startswith("// NEARMISS"):
                st = "nearmiss"
            elif re.search(r"^\s*asm\s+\w", src.read_text(errors="replace"), re.M):
                st = _asm_kind(src)
            else:
                st = "c_overlay"     # overlay C objects: see docs/OVERLAYS.md
        cands.append({"addr": addr, "name": name, "size": size, "region": f"overlay:{overlay}",
                      "splat_label": hex(addr - 0x40), "status": st, "link_route": None,
                      "objdiff_perfect": None, "csv_status": None, "subsystem": f"overlay_{overlay}"})
    cands.sort(key=lambda c: c["addr"])
    return cands


# ---------------------------------------------------------------------------
# DebugServer with one persistent connection (the server accepts many
# newline-delimited requests per connection), so thousands of breakpoint
# commands and status polls do not exhaust local TCP ports.

class PersistentDebug:
    def __init__(self, port: int = 21512):
        self.port = port
        self.sock: socket.socket | None = None
        self.buf = b""

    def _connect(self) -> None:
        self.sock = socket.create_connection(("127.0.0.1", self.port), timeout=SOCKET_TIMEOUT)
        self.buf = b""

    def _line(self) -> dict:
        while b"\n" not in self.buf:
            chunk = self.sock.recv(1 << 16)
            if not chunk:
                raise EOFError("DebugServer closed the connection")
            self.buf += chunk
        line, self.buf = self.buf.split(b"\n", 1)
        return json.loads(line)

    def call(self, cmd: dict) -> dict:
        for attempt in range(2):
            try:
                if self.sock is None:
                    self._connect()
                self.sock.sendall((json.dumps(cmd) + "\n").encode())
                resp = self._line()
                break
            except (OSError, EOFError):
                self.close()
                if attempt:
                    raise
        if not resp.get("ok"):
            raise RuntimeError(resp)
        return resp

    def call_many(self, cmds: list[dict], chunk: int = 200) -> None:
        if self.sock is None:
            self._connect()
        for i in range(0, len(cmds), chunk):
            part = cmds[i:i + chunk]
            self.sock.sendall("".join(json.dumps(c) + "\n" for c in part).encode())
            for _ in part:
                resp = self._line()
                if not resp.get("ok"):
                    raise RuntimeError(resp)

    def close(self) -> None:
        if self.sock is not None:
            try:
                self.sock.close()
            except OSError:
                pass
        self.sock = None


# ---------------------------------------------------------------------------
# Session: frame steps that record and disarm census breakpoints on the way.

class CensusSession(rc.RouteSession):
    stall_timeout = 1200.0          # seconds without any pause = fault (host may be saturated)

    def __init__(self, *a, **k):
        super().__init__(*a, **k)
        self.debug = PersistentDebug()
        self.armed: set[int] = set()
        self.label = "?"
        self.hits: list[dict] = []          # this label's hits, in order
        self.unexpected: list[dict] = []
        self.kicks: list[dict] = []
        self.frame_counter = 0

    def close(self) -> None:
        try:
            if self.pid is not None and self.armed:
                self.debug.call({"cmd": "clear_breakpoints"})
                self.armed.clear()
        except Exception:
            pass
        super().close()
        self.debug.close()

    def arm(self, addrs: list[int], label: str) -> float:
        t0 = time.monotonic()
        if self.armed:
            self.disarm_all()
        self.label = label
        self.hits = []
        addrs = [a for a in addrs if a != LOOP_TOP]
        self.debug.call_many([{"cmd": "set_breakpoint", "address": a} for a in addrs],
                             chunk=ARM_CHUNK)
        self.armed = set(addrs)
        return time.monotonic() - t0

    def disarm_all(self) -> None:
        self.debug.call_many([{"cmd": "remove_breakpoint", "address": a} for a in sorted(self.armed)],
                             chunk=ARM_CHUNK)
        self.armed.clear()

    def _wait_pause(self, timeout: float) -> int:
        deadline = time.monotonic() + timeout
        last_cycles, still_since = None, time.monotonic()
        while True:
            st = self.debug.call({"cmd": "status"})
            data = st.get("data", st)
            if data.get("paused"):
                return int(data["pc"], 16)
            now = time.monotonic()
            if now > deadline:
                raise TimeoutError(f"no pause within {timeout} s ({self.label})")
            # A breakpoint change can leave the VM stopped while the EE
            # reports "not paused" (cycles frozen).  Re-issue resume; it
            # continues the same emulated state.
            cycles = data.get("cycles")
            if cycles != last_cycles:
                last_cycles, still_since = cycles, now
            elif now - still_since > 3.0:
                frozen_kicks = sum(1 for k in self.kicks if k.get("cycles") == cycles
                                   and k["label"] == self.label)
                self.kicks.append({"label": self.label, "frame": self.frames_stepped + 1,
                                   "pc": data.get("pc"), "cycles": cycles,
                                   "pause_first": frozen_kicks >= 3})
                if frozen_kicks >= 3:
                    # plain resumes did not restart it: cycle the VM through pause
                    self.debug.call({"cmd": "pause"})
                    time.sleep(0.2)
                self.debug.call({"cmd": "resume"})
                still_since = now
            time.sleep(0.001)

    def _resume_to_boundary(self, timeout: float = 30.0) -> None:
        # frame being executed = frames_stepped + 1 (route rows are sampled
        # after the frame, so a hit at frame f ran during row f's frame).
        frame = self.frames_stepped + 1
        counter = self.frame_counter
        limit = max(timeout, self.stall_timeout)
        while True:
            self.debug.call({"cmd": "resume"})
            pc = self._wait_pause(limit)
            if pc == LOOP_TOP:
                if self.pine is not None:
                    self.frame_counter = self.u32(FRAME_COUNTER)
                return
            ev = {"pc": pc, "label": self.label, "frame": frame, "counter_before": counter}
            if OVERLAY_BASE <= pc < 0x900000:
                ev["overlay_id"] = self.u32(OVERLAY_BASE + 4)
            if pc not in self.armed and pc + 4 in self.armed:
                # An armed address that is a branch delay slot in the code now
                # resident (another overlay after an area change) pauses with
                # the branch's pc; the one-shot is the armed slot, pc + 4.
                ev["reported_pc"] = hex(pc)
                pc += 4
                ev["pc"] = pc
            if pc in self.armed:
                self.debug.call({"cmd": "remove_breakpoint", "address": pc})
                self.armed.discard(pc)
                self.hits.append(ev)
            else:
                self.unexpected.append(ev)
                if len(self.unexpected) > 50:
                    raise RuntimeError(f"unexpected pauses: {self.unexpected[-3:]}")


def open_census(state: Path, log_dir: Path, attempts: int = 6) -> CensusSession:
    for attempt in range(attempts):
        rc.wait_for_free_emulator()
        session = CensusSession(state, log_dir=log_dir)
        try:
            return session.__enter__()
        except (RuntimeError, TimeoutError, OSError, EOFError) as exc:
            print(f"start attempt {attempt + 1} failed: {exc}", flush=True)
            time.sleep(2)
    raise RuntimeError(f"emulator did not start from {state}")


def resumable(state: Path, tag: str) -> Path:
    """Serial-prefixed copy of a beat snapshot (pcsx2_session derives names
    from it); lives in build/s87/census/_resume/ and is deleted afterwards."""
    folder = OUT / "_resume" / tag
    folder.mkdir(parents=True, exist_ok=True)
    copy = folder / f"{rc.SERIAL}.resume.p2s"
    shutil.copyfile(state, copy)
    return copy


# ---------------------------------------------------------------------------
# Determinism probes: digests of stable RAM ranges.

DIGEST_SPANS = [("player", 0x8102B0, 0x300), ("globals", 0x810600, 0x800),
                ("owners", 0x7A5640, 0x9000)]


def digests_live(s: OriginalSession) -> dict:
    return {n: hashlib.sha256(s.read(a, size)).hexdigest()[:16] for n, a, size in DIGEST_SPANS}


def digests_file(ee: bytes) -> dict:
    return {n: hashlib.sha256(ee[a:a + size]).hexdigest()[:16] for n, a, size in DIGEST_SPANS}


def compare_rows(mine: list[dict], ref: list[dict]) -> dict:
    same = sum(1 for a, b in zip(mine, ref) if a == b)
    first = next((i for i, (a, b) in enumerate(zip(mine, ref)) if a != b), None)
    strip = ("counter", "clock")
    def norm(r):
        return {k: v for k, v in r.items() if k not in strip and k != "attach_r9"}
    same_phase = sum(1 for a, b in zip(mine, ref) if norm(a) == norm(b))
    return {"frames_mine": len(mine) - 1, "frames_recorded": len(ref) - 1,
            "rows_identical": same, "rows_compared": min(len(mine), len(ref)),
            "first_differing_row": first,
            "rows_identical_ignoring_counter_clock_r9": same_phase,
            "counter_offset_row0": (mine[0]["counter"] - ref[0]["counter"]) if mine and ref else None}


# ---------------------------------------------------------------------------
# Segments

def save_run(pass_name: str, label: str, doc: dict) -> None:
    d = OUT / "runs" / pass_name
    d.mkdir(parents=True, exist_ok=True)
    (d / f"{label}.json").write_text(json.dumps(doc, indent=1) + "\n")


def label_doc(s: CensusSession, label: str, extra: dict) -> dict:
    return dict(extra, label=label, hits=[dict(h, pc=hex(h["pc"])) for h in s.hits],
                unexpected=[dict(h, pc=hex(h["pc"])) for h in s.unexpected],
                kicks=list(s.kicks),
                functions=sorted({hex(h["pc"]) for h in s.hits}))


def overlay_header(s: OriginalSession) -> dict:
    """Which overlay is resident in the arena (MWo3 magic and overlay id)."""
    head = s.read(OVERLAY_BASE, 8)
    return {"magic_ok": head[:3] == b"MWo", "overlay_id": struct.unpack_from("<I", head, 4)[0]}


def run_startup(addrs: list[int], pass_name: str, idle_after: int = 200,
                opening_limit: int = 6000) -> None:
    """User slot 01 (title) -> NEW GAME -> AREA11 load -> opening -> first
    control -> idle.  Every frame is sampled with route_capture's sampler
    (runs/<pass>/traces/<label>.json) so two passes can be compared row by
    row; each label also records RAM digests at its start and end."""
    src = rc.slot_path("01")
    log = OUT / "logs" / pass_name / "startup"
    t_all = time.monotonic()
    s = open_census(src, log)
    s.stall_timeout = 900.0                       # movie + disc loads inside one frame
    sampler = rc.Sampler(s)
    rows: list[dict] = []

    def sample() -> dict:
        row = dict(rc.decode(sampler.raw()), f=s.frames_stepped)
        rows.append(row)
        return row

    def start_label(label: str) -> dict:
        rows.clear()
        arm_t = s.arm(addrs, label)
        head = {"arm_seconds": round(arm_t, 2), "start_digest": digests_live(s),
                "start_overlay": overlay_header(s), "start_frame": s.frames_stepped}
        sample()
        return head

    def finish_label(label: str, head: dict, t0: float, extra: dict) -> None:
        doc = dict(head, **extra)
        doc.update(seconds=round(time.monotonic() - t0, 1), end_digest=digests_live(s),
                   end_overlay=overlay_header(s), start_counter=rows[0]["counter"],
                   end_counter=rows[-1]["counter"], trace_rows=len(rows))
        d = OUT / "runs" / pass_name / "traces"
        d.mkdir(parents=True, exist_ok=True)
        (d / f"{label}.json").write_text(json.dumps({"label": label, "rows": rows},
                                                    separators=(",", ":")) + "\n")
        save_run(pass_name, label, label_doc(s, label, doc))

    try:
        def area() -> int:
            return s.read(0x810700, 4)[0]

        def task_b() -> int:
            return s.read(0x28A750, 0x10)[0xB]

        def boundary() -> dict:
            s._resume_to_boundary()
            s.frames_stepped += 1
            return sample()

        inputs: list[dict] = []

        def pad(buttons: int) -> None:
            s.pad(buttons)
            inputs.append({"label": s.label, "f": s.frames_stepped, "buttons": buttons})

        # S0: title menu until NEW GAME commits the area (0x810700 != 0).
        t0 = time.monotonic()
        head = start_label("S0_title")
        f0 = s.frames_stepped
        presses = 0
        while area() == 0:
            n = s.frames_stepped - f0
            if n in (5, 125, 245, 365) and presses < 4 and s.read(0x28A9A0, 4) == b"\0\0\0\0":
                pad(PAD["CROSS"]); presses += 1
            elif n % 120 in (9,):
                pad(0)
            boundary()
            if n > 1500:
                raise TimeoutError("title did not accept NEW GAME")
        pad(0)
        finish_label("S0_title", head, t0, {"frames": s.frames_stepped - f0, "presses": presses,
                                           "inputs": list(inputs)})
        # S1: movie + AREA11 load until the in-game frame machine runs (task +B == 1).
        t0 = time.monotonic()
        head = start_label("S1_newgame_load")
        f0 = s.frames_stepped
        while not (area() == 0x0B and task_b() == 1):
            boundary()
            if s.frames_stepped - f0 > 3000:
                raise TimeoutError("AREA11 did not start")
        finish_label("S1_newgame_load", head, t0, {"frames": s.frames_stepped - f0,
                                                  "overlay_id": s.u32(OVERLAY_BASE + 4)})
        # S2: opening cinematic until first control (3B8D back to 0, action 0, no status).
        t0 = time.monotonic()
        head = start_label("S2_opening")
        f0 = s.frames_stepped
        seen_cutscene = False
        spad_changes = []
        prev = None
        row = rows[-1]
        while True:
            if row["spad"] != prev:
                spad_changes.append({"f": s.frames_stepped - f0, "counter": row["counter"],
                                     "spad": row["spad"]})
                prev = row["spad"]
            if rc.cutscene(row):
                seen_cutscene = True
            if seen_cutscene and rc.in_control(row):
                break
            row = boundary()
            if s.frames_stepped - f0 > opening_limit:
                raise TimeoutError("opening did not return control: " + rc.summary(row))
        finish_label("S2_opening", head, t0, {
            "frames": s.frames_stepped - f0, "first_control_counter": row["counter"],
            "first_control_row": rc.summary(row), "spad_changes": spad_changes})
        # S3: idle after first control.
        t0 = time.monotonic()
        head = start_label("S3_first_control_idle")
        for _ in range(idle_after):
            boundary()
        finish_label("S3_first_control_idle", head, t0, {"frames": idle_after})
    except Exception as exc:
        keep = OUT / "runs" / pass_name / "_failed"
        keep.mkdir(parents=True, exist_ok=True)
        stamp = time.strftime("%H%M%S")
        (keep / f"startup.{stamp}.json").write_text(json.dumps(label_doc(s, s.label, {
            "error": repr(exc), "frames_stepped": s.frames_stepped,
            "last_row": rc.summary(rows[-1]) if rows else None}), indent=1) + "\n")
        raise
    finally:
        s.close()
        shutil.rmtree(OUT / "_resume", ignore_errors=True)
    print(f"startup done in {time.monotonic() - t_all:.0f} s", flush=True)


STARTUP_KEYS = ("frames", "presses", "start_counter", "end_counter", "first_control_counter",
                "first_control_row", "overlay_id", "start_digest", "end_digest")


def compare_startup(pass_a: str, pass_b: str) -> dict:
    """Determinism of the startup segment between two passes: per label the
    function set, the frame/counter facts and digests, and (when both passes
    have traces) the per-frame rows."""
    out = {}
    for lab in ORDER[:4]:
        fa, fb = (OUT / "runs" / p / f"{lab}.json" for p in (pass_a, pass_b))
        if not (fa.exists() and fb.exists()):
            out[lab] = {"missing": [str(f.relative_to(OUT)) for f in (fa, fb) if not f.exists()]}
            continue
        a, b = json.loads(fa.read_text()), json.loads(fb.read_text())
        sa, sb = set(a["functions"]), set(b["functions"])
        res = {"same_function_set": sa == sb, "only_" + pass_a: sorted(sa - sb),
               "only_" + pass_b: sorted(sb - sa),
               "facts_compared": {k: a[k] == b[k] for k in STARTUP_KEYS if k in a and k in b}}
        ta, tb = (OUT / "runs" / p / "traces" / f"{lab}.json" for p in (pass_a, pass_b))
        if ta.exists() and tb.exists():
            ra, rb = json.loads(ta.read_text())["rows"], json.loads(tb.read_text())["rows"]
            res["trace"] = compare_rows(ra, rb)
        out[lab] = res
    return out


def run_beat(name: str, source: str, fn, addrs: list[int], pass_name: str) -> None:
    rec_path = rc.beat_dir(name) / "trace.json"      # AREA01 beats: build/s87/route_a01/
    recorded = json.loads(rec_path.read_text())
    tail = recorded.get("tail_idle_frames", 0) or 0
    if len(source) == 2 and source.isdigit():
        src = rc.slot_path(source)
        src_ee = None
    else:
        src = resumable(rc.beat_dir(source) / "state.p2s", name)
        src_ee = rc.beat_dir(source) / "eeMemory.bin"
    t_all = time.monotonic()
    doc: dict = {"beat": name, "source": source, "tail_idle_frames": tail}
    s = open_census(src, OUT / "logs" / pass_name / name)
    try:
        r = rc.Route(s)
        r.begin()
        doc["start_counter"] = r.rows[0]["counter"]
        doc["start_digest"] = digests_live(s)
        if src_ee is not None:
            doc["source_snapshot_digest"] = digests_file(src_ee.read_bytes())
        doc["arm_seconds"] = round(s.arm(addrs, name), 2)
        t0 = time.monotonic()
        try:
            meta = fn(r)
            if tail:
                r.idle(tail)
            doc["what"] = meta.get("what")
            doc["completed"] = True
        except Exception as exc:          # record how far the beat got
            doc["completed"] = False
            doc["error"] = repr(exc)
        doc["seconds"] = round(time.monotonic() - t0, 1)
        doc["frames"] = r.frame_index
        doc["end_counter"] = r.rows[-1]["counter"]
        try:
            doc["end_digest"] = digests_live(s)
        except (OSError, EOFError, RuntimeError) as exc:
            doc["end_digest"] = {n: None for n, _a, _z in DIGEST_SPANS}
            doc["end_digest_error"] = repr(exc)
        doc["recorded_end_digest"] = digests_file((rc.beat_dir(name) / "eeMemory.bin").read_bytes())
        doc["end_digest_equal"] = {k: doc["end_digest"][k] == doc["recorded_end_digest"][k]
                                   for k in doc["end_digest"]}
        try:   # where the stable spans differ from the recorded snapshot (addresses only)
            ref = (rc.beat_dir(name) / "eeMemory.bin").read_bytes()
            diff = {}
            for n, a, size in DIGEST_SPANS:
                live = s.read(a, size)
                offs = [a + i for i in range(0, size, 4) if live[i:i + 4] != ref[a + i:a + i + 4]]
                diff[n] = {"words_differing": len(offs), "first": [hex(o) for o in offs[:24]]}
            doc["end_diff_vs_recorded"] = diff
        except (OSError, EOFError, RuntimeError) as exc:
            doc["end_diff_error"] = repr(exc)
        doc["trace_vs_recorded"] = compare_rows(r.rows, recorded["rows"])
        doc["inputs"] = r.inputs
        doc["recorded_inputs_equal"] = r.inputs == recorded["inputs"]
        save_run(pass_name, name, label_doc(s, name, doc))
    finally:
        s.close()
        shutil.rmtree(OUT / "_resume", ignore_errors=True)
    print(f"{name}: {doc['frames']} frames, {len(set(h['pc'] for h in s.hits))} functions, "
          f"{time.monotonic() - t_all:.0f} s, completed={doc['completed']}, "
          f"trace={doc['trace_vs_recorded']['rows_identical']}/{doc['trace_vs_recorded']['rows_compared']}",
          flush=True)


# Beat 15 (level exit) is kept apart from the first-level census: it runs only
# when named (`run --segments 15`), `run --segments all` leaves it out, and
# report() never reads it (ORDER excludes it), so route_functions.json,
# per_beat.json and summary.main_line stay beats 00..14.  Its functions are
# reported by `exit-delta` only.
EXIT_BEAT = "15_level_exit"
SEGMENTS = ["startup"] + [b[0] for b in rc.BEATS]
DEFAULT_SEGMENTS = [s for s in SEGMENTS if s not in rc.OPT_IN_BEATS]
ORDER = ["S0_title", "S1_newgame_load", "S2_opening", "S3_first_control_idle"] + \
    [b[0] for b in rc.BEATS if b[0] != EXIT_BEAT]
STARTUP_LABELS = {"S0_title", "S1_newgame_load", "S2_opening"}
SIDE_BEATS = {"00_panel_no_battery", "09_fence_door"}


def report(passes: list[str]) -> None:
    cands = {c["addr"]: c for c in candidates()}
    primary = passes[0]
    runs = {}
    for p in passes:
        d = OUT / "runs" / p
        runs[p] = {f.stem: json.loads(f.read_text()) for f in d.glob("*.json")} if d.exists() else {}
    base = runs[primary]
    missing = [lab for lab in ORDER if lab not in base]
    funcs: dict[int, dict] = {}
    per_beat = []
    unknown = []
    for lab in ORDER:
        doc = base.get(lab)
        if doc is None:
            continue
        # The label's set is the union over passes: a function that ran in
        # any replay of this label ran on the route.  The primary pass gives
        # the frame; a function only another pass saw carries that pass name.
        merged: dict[str, dict] = {}
        for p in passes:
            other = runs[p].get(lab)
            for h in (other or {}).get("hits", []):
                if h["pc"] not in merged:
                    merged[h["pc"]] = dict(h, **{"pass": p})
        seen_here = []
        for h in sorted(merged.values(), key=lambda h: (h["frame"], h["pass"] != primary)):
            pc = int(h["pc"], 16)
            c = cands.get(pc)
            if c is None:
                unknown.append(h)
                continue
            if c["region"].startswith("overlay") and h.get("overlay_id") != AREA11_ID:
                unknown.append(dict(h, note="overlay-range hit with a different overlay resident"))
                continue
            seen_here.append(pc)
            f = funcs.setdefault(pc, {"first_beat": lab, "first_frame": h["frame"],
                                      "first_counter_before": h["counter_before"],
                                      "first_pass": h["pass"], "beats": [],
                                      "beats_only_in_other_pass": []})
            f["beats"].append(lab)
            if h["pass"] != primary:
                f["beats_only_in_other_pass"].append(f"{lab}:{h['pass']}")
        new = [pc for pc in seen_here if funcs[pc]["first_beat"] == lab]
        entry = {k: v for k, v in doc.items() if k not in ("hits", "functions")}
        entry.update(label=lab, side_beat=lab in SIDE_BEATS, startup=lab in STARTUP_LABELS,
                     function_count=len(seen_here), new_function_count=len(new),
                     functions=[hex(pc) for pc in sorted(seen_here)],
                     new_functions=[hex(pc) for pc in sorted(new)])
        by_status: dict[str, int] = {}
        for pc in seen_here:
            by_status[cands[pc]["status"]] = by_status.get(cands[pc]["status"], 0) + 1
        entry["by_status"] = by_status
        # cross-pass reproducibility of this label's function set
        for p in passes[1:]:
            other = runs[p].get(lab)
            if other is not None:
                a = {h["pc"] for h in doc["hits"]}
                b = {h["pc"] for h in other["hits"]}
                entry.setdefault("reproducibility", {})[p] = {
                    "same_set": a == b, "only_in_" + primary: sorted(a - b),
                    "only_in_" + p: sorted(b - a),
                    "trace_rows_identical": (other.get("trace_vs_recorded") or {}).get("rows_identical"),
                    "end_digest_equal_between_passes": (other.get("end_digest") == doc.get("end_digest"))
                    if doc.get("end_digest") else None}
        per_beat.append(entry)
    rows = []
    for pc in sorted(funcs):
        c = cands[pc]
        f = funcs[pc]
        beats = f["beats"]
        rows.append({"addr": hex(pc), "name": c["name"], "size": c["size"],
                     "overlay": "AREA11" if c["region"].startswith("overlay") else None,
                     "splat_label": c.get("splat_label"), "status": c["status"],
                     "link_route": c["link_route"], "objdiff_perfect": c["objdiff_perfect"],
                     "subsystem": c["subsystem"],
                     "first_beat": f["first_beat"], "first_frame": f["first_frame"],
                     "first_counter_before": f["first_counter_before"],
                     "first_pass": f["first_pass"], "beats": beats,
                     "beats_only_in_other_pass": f["beats_only_in_other_pass"],
                     "startup_only": all(b in STARTUP_LABELS for b in beats),
                     "side_beat_only": all(b in SIDE_BEATS for b in beats)})
    summary = {"candidates": len(cands),
               "candidates_boot": sum(1 for c in cands.values() if c["region"] == "boot"),
               "candidates_overlay_AREA11": sum(1 for c in cands.values() if c["region"] != "boot"),
               "executed": len(rows),
               "executed_boot": sum(1 for r in rows if not r["overlay"]),
               "executed_overlay_AREA11": sum(1 for r in rows if r["overlay"]),
               "executed_excluding_startup_only": sum(1 for r in rows if not r["startup_only"]),
               "startup_only": sum(1 for r in rows if r["startup_only"]),
               "side_beat_only": sum(1 for r in rows if r["side_beat_only"]),
               "executed_bytes": sum(r["size"] for r in rows),
               "by_status": {}, "by_status_bytes": {},
               "labels_missing": missing, "unattributed_hits": unknown[:50],
               "unattributed_hit_count": len(unknown), "primary_pass": primary, "passes": passes,
               "labels_per_pass": {p: sorted(lab for lab in runs[p] if lab in ORDER) for p in passes},
               "functions_seen_only_in_other_passes": sum(1 for r in rows if r["first_pass"] != primary
                                                          and len(r["beats_only_in_other_pass"]) == len(r["beats"])),
               "function_label_pairs_only_in_other_passes": sum(len(r["beats_only_in_other_pass"]) for r in rows),
               "main_line": [b for b in ORDER if b not in SIDE_BEATS]}
    if len(passes) > 1:
        summary["startup_determinism"] = {p: compare_startup(primary, p) for p in passes[1:]}
    for r in rows:
        summary["by_status"][r["status"]] = summary["by_status"].get(r["status"], 0) + 1
        summary["by_status_bytes"][r["status"]] = summary["by_status_bytes"].get(r["status"], 0) + r["size"]
    route_only = [r for r in rows if not r["startup_only"]]
    summary["by_status_excluding_startup_only"] = {}
    for r in route_only:
        k = r["status"]
        summary["by_status_excluding_startup_only"][k] = summary["by_status_excluding_startup_only"].get(k, 0) + 1
    (OUT / "route_functions.json").write_text(json.dumps(
        {"summary": summary, "order": ORDER,
         "startup_only_functions": [r["addr"] for r in rows if r["startup_only"]],
         "functions": rows}, indent=1) + "\n")
    (OUT / "per_beat.json").write_text(json.dumps({"order": ORDER, "beats": per_beat}, indent=1) + "\n")
    print(json.dumps(summary, indent=1))


# ---------------------------------------------------------------------------
# Beat 15 (level exit): the functions it adds beyond every earlier label.

# Phase markers: the area-change request 001B0C60 and the spawn placement
# 001B07C0 of the arrival's state-0 rebuild (both one-shot hits in beat 15).
EXIT_REQUEST, EXIT_ARRIVAL = 0x1B0C60, 0x1B07C0


def exit_delta(passes: list[str]) -> dict:
    """Functions beat 15 executes that no earlier label (startup S0..S3 and
    beats 00..14, any of `passes`) executed; each is tagged with its phase:
    'area11' (before the area-change request), 'change_load' (request to
    arrival placement) or 'area01' (from the arrival placement on).
    Overlay-range hits while another overlay is resident are listed apart
    (AREA01 code at an AREA11 candidate address, not an AREA11 function)."""
    cands = {c["addr"]: c for c in candidates()}
    primary = passes[0]
    doc = json.loads((OUT / "runs" / primary / f"{EXIT_BEAT}.json").read_text())
    earlier: dict[int, list[str]] = {}
    for p in passes:
        for lab in ORDER:
            if lab == EXIT_BEAT:
                continue
            f = OUT / "runs" / p / f"{lab}.json"
            if f.exists():
                for h in json.loads(f.read_text())["hits"]:
                    earlier.setdefault(int(h["pc"], 16), []).append(f"{lab}:{p}")
    hits = {int(h["pc"], 16): h for h in doc["hits"]}
    f_req = hits.get(EXIT_REQUEST, {}).get("frame")
    f_arr = hits.get(EXIT_ARRIVAL, {}).get("frame")

    def phase(frame: int) -> str:
        if f_req is None or frame < f_req:
            return "area11"
        if f_arr is None or frame < f_arr:
            return "change_load"
        return "area01"

    new, other_overlay, unknown = [], [], []
    for pc, h in sorted(hits.items(), key=lambda kv: (kv[1]["frame"], kv[0])):
        c = cands.get(pc)
        if c is None:
            unknown.append(h)
            continue
        if c["region"].startswith("overlay") and h.get("overlay_id") != AREA11_ID:
            other_overlay.append(dict(h, name=c["name"]))
            continue
        if pc in earlier:
            continue
        new.append({"addr": hex(pc), "name": c["name"], "size": c["size"], "region": c["region"],
                    "status": c["status"], "link_route": c["link_route"],
                    "objdiff_perfect": c["objdiff_perfect"], "subsystem": c["subsystem"],
                    "frame": h["frame"], "counter_before": h["counter_before"],
                    "phase": phase(h["frame"])})
    by_phase: dict[str, dict[str, int]] = {}
    for r in new:
        d = by_phase.setdefault(r["phase"], {})
        d[r["status"]] = d.get(r["status"], 0) + 1
    out = {"beat": EXIT_BEAT, "passes": passes, "frames": doc.get("frames"),
           "completed": doc.get("completed"), "trace_vs_recorded": doc.get("trace_vs_recorded"),
           "request_frame": f_req, "arrival_frame": f_arr,
           "functions_in_beat": len(hits), "new_function_count": len(new),
           "new_by_phase_and_status": by_phase,
           "new_bytes": sum(r["size"] for r in new),
           "overlay_hits_other_overlay": other_overlay, "unattributed_hits": unknown,
           "new_functions": new}
    (OUT / "exit_delta.json").write_text(json.dumps(out, indent=1) + "\n")
    return out


# ---------------------------------------------------------------------------
# AREA01 (the second level, opt-in): route_capture's a01_* beats.  They are
# replayed only when named (`run --segments a01` or a01_03,...), with the
# breakpoints on the boot functions plus the AREA01 overlay (runtime = splat
# label + 0x40, like AREA11).  `a01-delta` lists what they execute beyond the
# first-level census (route_functions.json: startup S0..S3 + beats 00..14);
# nothing here changes that census or `report`.  It counts AREA01 overlay hits
# per real function (split pieces regrouped, a01_real_functions), with the
# decomp status of the src/overlays/AREA01 tree it runs on.

AREA01_ID = 2
A01_EXIT_BEAT = "a01_07_level_exit"
# The s88 completeness side beats (east room, duct, blocked bridge, third NPC
# talk; SECOND_LEVEL_ROUTE.md section 9).  a01-delta counts what only they ran.
A01_ROOM_BEATS = ("a01_s4_east_room", "a01_s5_duct", "a01_s6_bridge_blocked", "a01_s7_npc_third_talk")
A01_CHANGE, A01_ARRIVAL = 0x1AD010, 0x1B07C0


def real_functions(cands: dict[int, dict], overlay: str = "AREA01") -> dict[int, int]:
    """Regroup one overlay's splat pieces into the overlay's real functions.

    Splat splits many overlay functions in two (an intra-overlay call target
    sits 0x40 into the callee; see tools/overlay/overlay_match.py), so a
    breakpoint on a split piece is a point inside a real function, not an
    entry.  This rewrites cands in place: each real function keeps its entry
    runtime address with its true size and its src/overlays/<overlay> status
    (working tree), absorbed pieces are dropped, and the returned map sends
    every piece's runtime address to its function's entry."""
    sys.path.insert(0, str(ROOT / "tools" / "overlay"))
    import overlay_match as om  # noqa: E402
    kind = {"C": "c_overlay", "nearmiss": "nearmiss", "asm": "asm_undecompiled",
            "asm-void": "asm_word"}
    piece_to_real: dict[int, int] = {}
    for f in om.true_functions(overlay):
        rt = f["runtime"]
        for piece in f["pieces"]:
            prt = int(piece[-8:], 16) + 0x40
            piece_to_real[prt] = rt
            if prt != rt:
                cands.pop(prt, None)
        c = cands[rt]
        c.update(size=f["size"], status=kind[om.c_status(overlay, f["name"])],
                 pieces=[hex(int(x[-8:], 16) + 0x40) for x in f["pieces"]],
                 slot_end=f["slot_end"] + 0x40)
    return piece_to_real


def a01_real_functions(cands: dict[int, dict]) -> dict[int, int]:
    """AREA01's real functions (real_functions for AREA01)."""
    return real_functions(cands, "AREA01")


def a01_delta(passes: list[str]) -> dict:
    cands = {c["addr"]: c for c in candidates("AREA01")}
    piece_to_real = a01_real_functions(cands)
    # beat 15 armed the AREA11 overlay only: an AREA01 function can show as
    # "in beat 15" only where an AREA11 candidate shares its runtime address.
    beat15_armed_overlay = {c["addr"] for c in candidates("AREA11") if c["region"] != "boot"}
    first = json.loads((OUT / "route_functions.json").read_text())
    first_boot = {int(f["addr"], 16) for f in first["functions"] if not f.get("overlay")}
    beat15 = {}
    for p in first["summary"].get("passes", ["A"]):     # the first-level census passes
        f15 = OUT / "runs" / p / f"{EXIT_BEAT}.json"
        if f15.exists():
            for h in json.loads(f15.read_text())["hits"]:
                beat15.setdefault(int(h["pc"], 16), h)
    funcs: dict[int, dict] = {}
    other_overlay, unknown, per_beat, missing = [], [], [], []
    phase_marks = {}
    beat_sets: dict[str, set[int]] = {}
    for name, _src, _fn in rc.A01_BEATS:
        seen_here = set()
        found = False
        for p in passes:
            f = OUT / "runs" / p / f"{name}.json"
            if not f.exists():
                continue
            found = True
            doc = json.loads(f.read_text())
            # The exit beat crosses into AREA00: phase its hits by the area-change
            # consumer 001AD010 and the arrival placement 001B07C0 (one-shot hits).
            marks = {int(h["pc"], 16): h["frame"] for h in doc["hits"]}
            f_chg = marks.get(A01_CHANGE) if name == A01_EXIT_BEAT else None
            f_arr = marks.get(A01_ARRIVAL) if name == A01_EXIT_BEAT else None
            if name == A01_EXIT_BEAT:
                phase_marks[p] = {"change_001AD010": f_chg, "arrival_001B07C0": f_arr}

            def phase(frame: int) -> str:
                if f_chg is None or frame < f_chg:
                    return "area01"
                return "change_load" if f_arr is None or frame < f_arr else "area00"
            for h in doc["hits"]:
                piece_pc = int(h["pc"], 16)
                pc = piece_to_real.get(piece_pc, piece_pc)   # a split piece counts for its function
                c = cands.get(pc)
                if c is None:
                    unknown.append(dict(h, beat=name))
                    continue
                if c["region"].startswith("overlay") and h.get("overlay_id") != AREA01_ID:
                    other_overlay.append(dict(h, beat=name, name=c["name"]))
                    continue
                seen_here.add(pc)
                e = funcs.setdefault(pc, {"first_beat": name, "first_frame": h["frame"],
                                          "first_pass": p, "beats": [], "phases": [],
                                          "hit_points": []})
                if name == e["first_beat"] and h["frame"] < e["first_frame"]:
                    e["first_frame"] = h["frame"]
                if hex(piece_pc) not in e["hit_points"]:
                    e["hit_points"].append(hex(piece_pc))
                if name not in e["beats"]:
                    e["beats"].append(name)
                ph = phase(h["frame"])
                if ph not in e["phases"]:
                    e["phases"].append(ph)
        if not found:
            missing.append(name)
            continue
        per_beat.append({"beat": name, "side": name in rc.A01_SIDE_BEATS,
                         "functions": len(seen_here),
                         "new_vs_first_level": sum(1 for pc in seen_here
                                                   if cands[pc]["region"] != "boot" or pc not in first_boot)})
        beat_sets[name] = seen_here
    rows = []
    for pc in sorted(funcs):
        c, e = cands[pc], funcs[pc]
        overlay = c["region"] != "boot"
        in_first = (not overlay) and pc in first_boot
        h15 = beat15.get(pc)
        in15 = bool(h15) and (not overlay or h15.get("overlay_id") == AREA01_ID)
        rows.append({"addr": hex(pc), "name": c["name"], "size": c["size"],
                     "region": c["region"], "splat_label": c.get("splat_label"),
                     "status": c["status"], "subsystem": c["subsystem"],
                     "pieces": c.get("pieces"), "hit_points": e["hit_points"],
                     "in_first_level_census": in_first, "in_beat15_exit": in15,
                     "beat15_armed": (not overlay) or pc in beat15_armed_overlay,
                     "first_beat": e["first_beat"], "first_frame": e["first_frame"],
                     "beats": e["beats"], "phases": e["phases"],
                     "exit_change_or_area00_only": "area01" not in e["phases"],
                     "side_beat_only": all(b in rc.A01_SIDE_BEATS for b in e["beats"])})
    new = [r for r in rows if not r["in_first_level_census"]]
    def count(rs, key):
        out: dict[str, int] = {}
        for r in rs:
            out[r[key]] = out.get(r[key], 0) + 1
        return out
    summary = {"passes": passes, "beats_missing": missing,
               "executed": len(rows), "executed_boot": sum(1 for r in rows if r["region"] == "boot"),
               "executed_overlay_AREA01": sum(1 for r in rows if r["region"] != "boot"),
               "new_vs_first_level": len(new), "new_bytes": sum(r["size"] for r in new),
               "new_boot": sum(1 for r in new if r["region"] == "boot"),
               "new_overlay_AREA01": sum(1 for r in new if r["region"] != "boot"),
               "new_already_in_beat15": sum(1 for r in new if r["in_beat15_exit"]),
               "new_side_beat_only": sum(1 for r in new if r["side_beat_only"]),
               "new_exit_change_or_area00_only": sum(1 for r in new if r["exit_change_or_area00_only"]),
               "new_in_area01_play": sum(1 for r in new if not r["exit_change_or_area00_only"]),
               "exit_phase_marks": phase_marks,
               "new_by_status": count(new, "status"), "new_by_subsystem": count(new, "subsystem"),
               "overlay_hits_other_overlay": len(other_overlay), "unattributed_hits": len(unknown)}
    # s88 room beats (additive): what each beat adds beyond the first level
    # and every OTHER AREA01 beat, and the functions only the room beats ran.
    for e in per_beat:
        others = set().union(*(v for k, v in beat_sets.items() if k != e["beat"])) if len(beat_sets) > 1 else set()
        e["new_vs_first_level_and_other_a01"] = sum(
            1 for pc in beat_sets[e["beat"]] - others
            if cands[pc]["region"] != "boot" or pc not in first_boot)
    room_new = [r for r in new if all(b in A01_ROOM_BEATS for b in r["beats"])]
    summary.update(room_beats=list(A01_ROOM_BEATS),
                   room_beats_run=[b for b in A01_ROOM_BEATS if b in beat_sets],
                   new_only_in_room_beats=len(room_new),
                   new_only_in_room_beats_bytes=sum(r["size"] for r in room_new),
                   new_only_in_room_beats_by_status=count(room_new, "status"))
    out = {"summary": summary, "per_beat": per_beat, "new_functions": new, "functions": rows,
           "room_beats_new_functions": room_new,
           "overlay_hits_other_overlay": other_overlay, "unattributed_hits": unknown}
    (OUT / "a01_delta.json").write_text(json.dumps(out, indent=1) + "\n")
    return out


# ---------------------------------------------------------------------------
# AREA00 (the third level, opt-in): route_capture's a00_* beats.  Replayed
# only when named (`run --segments a00` or a00_03,...), with the breakpoints
# on the boot functions plus the AREA00 overlay (runtime = splat label +
# 0x40).  `a00-delta` lists what AREA00 runs that neither the first level nor
# AREA01 ran.  Its segments:
#   arrival  a01_07_level_exit (A01 passes) from the area-change consumer
#            001AD010 on: the AREA00 load and arrival script.  That replay
#            armed the AREA01 overlay, so its AREA00 code shows only where an
#            AREA01 candidate address falls inside an AREA00 function (a point
#            inside the function, overlay id 1 resident), mapped by range;
#   main     a00_00..a00_10 (route_capture's A00_BEATS main line; a00_10
#            before its area change);
#   side     a00_s0 before its own area change (AREA00 play);
#   exit     from the area change on in the two beats that leave AREA00:
#            a00_10 (the progression exit, shaft door in sub-state 6) and
#            a00_s0 (the plain shaft door back); each row names its beats.
# "Already ran" = the first-level census (route_functions.json), beat 15
# (every hit: the AREA11 exit and the AREA01 arrival) and the AREA01 beats in
# their AREA01 phase (a01_07 before its change).  Nothing here changes the
# earlier outputs.

AREA00_ID = 1
A00_SIDE_BEATS = {"a00_s0_shaft_door_back"}
A00_EXIT_BEAT = "a00_s0_shaft_door_back"
A00_CHANGE_BEATS = {"a00_s0_shaft_door_back", "a00_10_progression_exit"}   # beats that leave AREA00


def _hits(p: str, name: str) -> list[dict] | None:
    f = OUT / "runs" / p / f"{name}.json"
    return json.loads(f.read_text())["hits"] if f.exists() else None


def a00_delta(passes: list[str], a01_passes: list[str]) -> dict:
    cands = {c["addr"]: c for c in candidates("AREA00")}
    piece_to_real = real_functions(cands, "AREA00")
    ov_funcs = sorted((c["addr"], c["slot_end"]) for c in cands.values() if c["region"] != "boot")

    def containing(pc: int) -> int | None:
        for lo, hi in ov_funcs:
            if lo <= pc < hi:
                return lo
        return None

    # --- what already ran: first level + beat 15 + AREA01 play
    first = json.loads((OUT / "route_functions.json").read_text())
    prior: dict[int, str] = {}
    for f in first["functions"]:
        if not f.get("overlay"):
            prior.setdefault(int(f["addr"], 16), "first_level")
    for p in first["summary"].get("passes", ["A"]):
        for h in _hits(p, EXIT_BEAT) or []:
            if not (OVERLAY_BASE <= int(h["pc"], 16) < 0x900000):
                prior.setdefault(int(h["pc"], 16), "beat15")
    a01_marks, a01_read = {}, []
    for name, _src, _fn in rc.A01_BEATS:
        for p in a01_passes:
            hs = _hits(p, name)
            if hs is None:
                continue
            a01_read.append(f"{name}:{p}")
            f_chg = None
            if name == A01_EXIT_BEAT:
                f_chg = {int(h["pc"], 16): h["frame"] for h in hs}.get(A01_CHANGE)
                a01_marks[p] = f_chg
            for h in hs:
                pc = int(h["pc"], 16)
                if OVERLAY_BASE <= pc < 0x900000:
                    continue
                if f_chg is None or h["frame"] < f_chg:
                    prior.setdefault(pc, "area01")

    # --- AREA00 segments
    funcs: dict[int, dict] = {}
    other_overlay, unknown, per_seg, missing = [], [], [], []

    def add(pc: int, piece_pc: int, seg: str, beat: str, frame: int, via: str) -> None:
        e = funcs.setdefault(pc, {"first_segment": seg, "first_beat": beat, "first_frame": frame,
                                  "segments": [], "beats": [], "hit_points": [], "via": []})
        if seg not in e["segments"]:
            e["segments"].append(seg)
        if beat not in e["beats"]:
            e["beats"].append(beat)
        if hex(piece_pc) not in e["hit_points"]:
            e["hit_points"].append(hex(piece_pc))
        if via not in e["via"]:
            e["via"].append(via)

    # arrival: a01_07 from its area change on
    seen = set()
    found = False
    for p in a01_passes:
        hs = _hits(p, A01_EXIT_BEAT)
        f_chg = a01_marks.get(p)
        if hs is None or f_chg is None:
            continue
        found = True
        for h in hs:
            if h["frame"] < f_chg:
                continue
            pc = int(h["pc"], 16)
            if OVERLAY_BASE <= pc < 0x900000:
                if h.get("overlay_id") != AREA00_ID:
                    continue
                real = containing(pc)
                if real is None:
                    unknown.append(dict(h, beat=A01_EXIT_BEAT))
                    continue
                add(real, pc, "arrival", A01_EXIT_BEAT, h["frame"], "inside (AREA01 candidate address)")
                seen.add(real)
            elif pc in cands:
                add(pc, pc, "arrival", A01_EXIT_BEAT, h["frame"], "entry")
                seen.add(pc)
            else:
                unknown.append(dict(h, beat=A01_EXIT_BEAT))
    if found:
        per_seg.append({"segment": "arrival", "beat": A01_EXIT_BEAT, "passes": a01_passes,
                        "change_frame_001AD010": a01_marks, "functions": len(seen),
                        "new": sum(1 for pc in seen if pc not in prior)})
    else:
        missing.append(A01_EXIT_BEAT)
    exit_marks = {}
    for name, _src, _fn in rc.A00_BEATS:
        seen_main, seen_exit = set(), set()
        found = False
        for p in passes:
            hs = _hits(p, name)
            if hs is None:
                continue
            found = True
            f_chg = {int(h["pc"], 16): h["frame"] for h in hs}.get(A01_CHANGE) if name in A00_CHANGE_BEATS else None
            if name in A00_CHANGE_BEATS:
                exit_marks.setdefault(name, {})[p] = f_chg
            for h in hs:
                piece_pc = int(h["pc"], 16)
                pc = piece_to_real.get(piece_pc, piece_pc)
                c = cands.get(pc)
                if c is None:
                    unknown.append(dict(h, beat=name))
                    continue
                if c["region"] != "boot" and h.get("overlay_id") != AREA00_ID:
                    other_overlay.append(dict(h, beat=name, name=c["name"]))
                    continue
                if f_chg is not None and h["frame"] >= f_chg:
                    seg = "exit"
                    seen_exit.add(pc)
                else:
                    seg = "side" if name in A00_SIDE_BEATS else "main"
                    seen_main.add(pc)
                add(pc, piece_pc, seg, name, h["frame"], "entry" if pc == piece_pc else "piece")
        if not found:
            missing.append(name)
            continue
        per_seg.append({"segment": "side" if name in A00_SIDE_BEATS else "main", "beat": name,
                        "functions": len(seen_main | seen_exit),
                        "new": sum(1 for pc in seen_main | seen_exit if pc not in prior),
                        **({"exit_phase_functions": len(seen_exit),
                            "change_frame_001AD010": exit_marks.get(name)} if name in A00_CHANGE_BEATS else {})})
    rows = []
    for pc in sorted(funcs):
        c, e = cands[pc], funcs[pc]
        segs = e["segments"]
        group = ("main" if {"arrival", "main"} & set(segs) else "side" if "side" in segs else "exit")
        rows.append({"addr": hex(pc), "name": c["name"], "size": c["size"], "region": c["region"],
                     "splat_label": c.get("splat_label"), "status": c["status"],
                     "subsystem": c["subsystem"], "pieces": c.get("pieces"),
                     "hit_points": e["hit_points"], "hit_via": e["via"],
                     "already_ran": prior.get(pc) if c["region"] == "boot" else None,
                     "first_segment": e["first_segment"], "first_beat": e["first_beat"],
                     "first_frame": e["first_frame"], "segments": segs, "beats": e["beats"],
                     "group": group})
    new = [r for r in rows if not r["already_ran"]]

    def count(rs, key):
        out: dict[str, int] = {}
        for r in rs:
            out[r[key]] = out.get(r[key], 0) + 1
        return out
    by_group = {}
    for g in ("main", "side", "exit"):
        rs = [r for r in new if r["group"] == g]
        by_group[g] = {"functions": len(rs), "bytes": sum(r["size"] for r in rs),
                       "boot": sum(1 for r in rs if r["region"] == "boot"),
                       "overlay_AREA00": sum(1 for r in rs if r["region"] != "boot"),
                       "by_status": count(rs, "status")}
    ov_all = [c for c in cands.values() if c["region"] != "boot"]
    summary = {"passes": passes, "a01_passes": a01_passes, "segments_missing": missing,
               "a01_beats_read": a01_read,
               "executed": len(rows), "executed_boot": sum(1 for r in rows if r["region"] == "boot"),
               "executed_overlay_AREA00": sum(1 for r in rows if r["region"] != "boot"),
               "overlay_AREA00_functions": len(ov_all),
               "new": len(new), "new_bytes": sum(r["size"] for r in new),
               "new_boot": sum(1 for r in new if r["region"] == "boot"),
               "new_overlay_AREA00": sum(1 for r in new if r["region"] != "boot"),
               "new_by_group": by_group, "new_by_status": count(new, "status"),
               "new_exit_by_beat": {b: sum(1 for r in new if r["group"] == "exit" and b in r["beats"])
                                    for b in sorted(A00_CHANGE_BEATS)},
               "new_by_subsystem": count(new, "subsystem"),
               "already_ran_by_source": count([r for r in rows if r["already_ran"]], "already_ran"),
               "overlay_hits_other_overlay": len(other_overlay), "unattributed_hits": len(unknown),
               "overlay_status_source": "working tree src/overlays/AREA00 at the time of the run"}
    out = {"summary": summary, "per_segment": per_seg, "new_functions": new, "functions": rows,
           "overlay_not_run": [{"addr": hex(c["addr"]), "name": c["name"], "size": c["size"],
                                "status": c["status"], "pieces": c.get("pieces")}
                               for c in sorted(ov_all, key=lambda c: c["addr"]) if c["addr"] not in funcs],
           "overlay_hits_other_overlay": other_overlay, "unattributed_hits": unknown,
           # for tools/area_overview.py --area 0: the boot functions that ran before AREA00
           "prior_name": "first-level, beat-15 and AREA01 route censuses",
           "prior_boot": [hex(a) for a in sorted(prior)]}
    (OUT / "a00_delta.json").write_text(json.dumps(out, indent=1) + "\n")
    # the arrival list in exit_delta.json's shape (tools/area_overview.py --exit-delta)
    arr = [r for r in new if "arrival" in r["segments"] and r["region"] == "boot"]
    (OUT / "a00_arrival.json").write_text(json.dumps(
        {"beat": A01_EXIT_BEAT, "segment": "arrival (from 001AD010)", "passes": a01_passes,
         "new_function_count": len(arr), "new_functions": [{"addr": r["addr"], "name": r["name"],
                                                            "size": r["size"], "status": r["status"],
                                                            "frame": r["first_frame"]} for r in arr]},
        indent=1) + "\n")
    return out


# ---------------------------------------------------------------------------
# The AREA01 revisit (a01r) and AREA02 (a02), s88 lane NEXT, both opt-in:
# route_capture's a01r_* and a02_* beats, replayed only when named (`run
# --segments a01r`, `--segments a02` or beat names).  The a01r segments arm the
# boot functions plus the AREA01 overlay, the a02 segments the boot functions
# plus the AREA02 overlay (runtime = splat label + 0x40).  `a02-delta` lists
# what the revisit and AREA02 run that neither the first level, nor AREA01
# (first visit), nor AREA00 ran.  Segments:
#   a01r         the revisit's AREA01 play (a01r beats; the beat that enters
#                AREA02 up to its area-change consumer 001AD010);
#   a01r_side    the revisit's side beats before any area change;
#   a02_arrival  the AREA02 load and arrival, from 001AD010 on in the beat that
#                enters AREA02.  That replay armed the AREA01 overlay, so its
#                AREA02 code shows only where an AREA01 candidate address falls
#                inside an AREA02 function (overlay id 3 resident), mapped by range;
#   a02          the AREA02 main-line beats (a beat that leaves AREA02 up to its
#                change);
#   a02_side     AREA02 side beats before any area change;
#   a02_exit     from the area change on in the beats that leave AREA02.
# "Already ran" = the first-level census (route_functions.json), beat 15, every
# AREA01 beat (all phases: a01_07's exit phase is AREA00's arrival) and every
# AREA00 beat (all phases: a00_10's exit phase is the revisit's arrival).  AREA01
# overlay functions count as already run when the first visit ran them (A01
# passes, overlay id 2) or a00_10's exit phase ran them (mapped by range).
# Nothing here changes the earlier outputs.

AREA02_ID = 3


def _overlay_map(overlay: str):
    cands = {c["addr"]: c for c in candidates(overlay)}
    p2r = real_functions(cands, overlay)
    spans = sorted((c["addr"], c["slot_end"]) for c in cands.values() if c["region"] != "boot")
    return cands, p2r, spans


def _containing(spans, pc: int) -> int | None:
    for lo, hi in spans:
        if lo <= pc < hi:
            return lo
    return None


def a02_delta(passes: list[str], a01r_passes: list[str], a00_passes: list[str],
              a01_passes: list[str]) -> dict:
    c02, p2r02, span02 = _overlay_map("AREA02")
    c01, p2r01, span01 = _overlay_map("AREA01")
    boot = {a: c for a, c in c02.items() if c["region"] == "boot"}

    # --- what already ran
    first = json.loads((OUT / "route_functions.json").read_text())
    prior: dict[int, str] = {}
    for f in first["functions"]:
        if not f.get("overlay"):
            prior.setdefault(int(f["addr"], 16), "first_level")
    for p in first["summary"].get("passes", ["A"]):
        for h in _hits(p, EXIT_BEAT) or []:
            if not (OVERLAY_BASE <= int(h["pc"], 16) < 0x900000):
                prior.setdefault(int(h["pc"], 16), "beat15")
    prior_ov01: dict[int, str] = {}
    read = []
    for name, _src, _fn in rc.A01_BEATS:
        for p in a01_passes:
            for h in _hits(p, name) or []:
                pc = int(h["pc"], 16)
                if OVERLAY_BASE <= pc < 0x900000:
                    if h.get("overlay_id") == AREA01_ID:
                        prior_ov01.setdefault(p2r01.get(pc, pc), "area01")
                else:
                    prior.setdefault(pc, "area01")
            if _hits(p, name) is not None:
                read.append(f"{name}:{p}")
    for name, _src, _fn in rc.A00_BEATS:
        for p in a00_passes:
            hs = _hits(p, name)
            if hs is None:
                continue
            read.append(f"{name}:{p}")
            for h in hs:
                pc = int(h["pc"], 16)
                if OVERLAY_BASE <= pc < 0x900000:
                    if h.get("overlay_id") == AREA01_ID:      # a00_10 / a00_s0 exit: AREA01 code
                        real = _containing(span01, pc)
                        if real is not None:
                            prior_ov01.setdefault(real, "area00_exit")
                else:
                    prior.setdefault(pc, "area00")

    # --- the revisit and AREA02
    funcs: dict[tuple[str, int], dict] = {}
    other_overlay, unknown, per_seg, missing = [], [], [], []

    def add(key, piece_pc, seg, beat, frame, via):
        e = funcs.setdefault(key, {"first_segment": seg, "first_beat": beat, "first_frame": frame,
                                   "segments": [], "beats": [], "hit_points": [], "via": []})
        for k, v in (("segments", seg), ("beats", beat), ("hit_points", hex(piece_pc)), ("via", via)):
            if v not in e[k]:
                e[k].append(v)

    change_marks: dict[str, dict] = {}
    groups = [(rc.A01R_BEATS, "AREA01", AREA01_ID, p2r01, rc.A01R_SIDE_BEATS, rc.A01R_CHANGE_BEATS,
               "a01r", "a01r_side", "a02_arrival", a01r_passes),
              (rc.A02_BEATS, "AREA02", AREA02_ID, p2r02, rc.A02_SIDE_BEATS, rc.A02_CHANGE_BEATS,
               "a02", "a02_side", "a02_exit", passes)]
    for beats, ovl, ovl_id, p2r, side, changes, seg_main, seg_side, seg_after, ps in groups:
        for name, _src, _fn in beats:
            seen: dict[str, set] = {}
            found = False
            for p in ps:
                hs = _hits(p, name)
                if hs is None:
                    continue
                found = True
                f_chg = ({int(h["pc"], 16): h["frame"] for h in hs}.get(A01_CHANGE)
                         if name in changes else None)
                if name in changes:
                    change_marks.setdefault(name, {})[p] = f_chg
                for h in hs:
                    piece_pc = int(h["pc"], 16)
                    after = f_chg is not None and h["frame"] >= f_chg
                    seg = seg_after if after else (seg_side if name in side else seg_main)
                    if OVERLAY_BASE <= piece_pc < 0x900000:
                        oid = h.get("overlay_id")
                        if oid == ovl_id:
                            key, via = ("overlay:" + ovl, p2r.get(piece_pc, piece_pc)), "entry"
                            if key[1] != piece_pc:
                                via = "piece"
                        elif oid == AREA02_ID and ovl == "AREA01":
                            real = _containing(span02, piece_pc)
                            if real is None:
                                unknown.append(dict(h, beat=name))
                                continue
                            key, via = ("overlay:AREA02", real), "inside (AREA01 candidate address)"
                        else:
                            other_overlay.append(dict(h, beat=name))
                            continue
                    elif piece_pc in boot:
                        key, via = ("boot", piece_pc), "entry"
                    else:
                        unknown.append(dict(h, beat=name))
                        continue
                    add(key, piece_pc, seg, name, h["frame"], via)
                    seen.setdefault(seg, set()).add(key)
            if not found:
                missing.append(name)
                continue
            allk = set().union(*seen.values()) if seen else set()
            per_seg.append({"beat": name, "segments": {k: len(v) for k, v in seen.items()},
                            "functions": len(allk),
                            **({"change_frame_001AD010": change_marks.get(name)} if name in changes else {})})

    def cand(key):
        region, pc = key
        return c01[pc] if region == "overlay:AREA01" else c02[pc]

    order = ("a01r", "a01r_side", "a02_arrival", "a02", "a02_side", "a02_exit")
    rows = []
    for key in sorted(funcs, key=lambda k: (k[0] != "boot", k[0], k[1])):
        region, pc = key
        c, e = cand(key), funcs[key]
        if region == "boot":
            already = prior.get(pc)
        elif region == "overlay:AREA01":
            already = prior_ov01.get(pc)
        else:
            already = None
        group = next(g for g in order if g in e["segments"])
        rows.append({"addr": hex(pc), "name": c["name"], "size": c["size"], "region": region,
                     "splat_label": c.get("splat_label"), "status": c["status"],
                     "subsystem": c["subsystem"], "pieces": c.get("pieces"),
                     "hit_points": e["hit_points"], "hit_via": e["via"], "already_ran": already,
                     "first_segment": e["first_segment"], "first_beat": e["first_beat"],
                     "first_frame": e["first_frame"], "segments": e["segments"], "beats": e["beats"],
                     "group": group})
    new = [r for r in rows if not r["already_ran"]]

    def count(rs, k):
        out: dict[str, int] = {}
        for r in rs:
            out[r[k]] = out.get(r[k], 0) + 1
        return out
    by_group = {}
    for g in order:
        rs = [r for r in new if r["group"] == g]
        by_group[g] = {"functions": len(rs), "bytes": sum(r["size"] for r in rs),
                       "boot": sum(1 for r in rs if r["region"] == "boot"),
                       "overlay_AREA01": sum(1 for r in rs if r["region"] == "overlay:AREA01"),
                       "overlay_AREA02": sum(1 for r in rs if r["region"] == "overlay:AREA02"),
                       "by_status": count(rs, "status")}
    ov02 = [c for c in c02.values() if c["region"] != "boot"]
    ran02 = {pc for (reg, pc) in funcs if reg == "overlay:AREA02"}
    summary = {"passes": passes, "a01r_passes": a01r_passes, "a00_passes": a00_passes,
               "a01_passes": a01_passes, "beats_missing": missing, "prior_beats_read": read,
               "executed": len(rows),
               "executed_boot": sum(1 for r in rows if r["region"] == "boot"),
               "executed_overlay_AREA01": sum(1 for r in rows if r["region"] == "overlay:AREA01"),
               "executed_overlay_AREA02": sum(1 for r in rows if r["region"] == "overlay:AREA02"),
               "overlay_AREA02_functions": len(ov02),
               "new": len(new), "new_bytes": sum(r["size"] for r in new),
               "new_boot": sum(1 for r in new if r["region"] == "boot"),
               "new_overlay_AREA01": sum(1 for r in new if r["region"] == "overlay:AREA01"),
               "new_overlay_AREA02": sum(1 for r in new if r["region"] == "overlay:AREA02"),
               "new_by_group": by_group, "new_by_status": count(new, "status"),
               "new_by_subsystem": count(new, "subsystem"),
               "already_ran_by_source": count([r for r in rows if r["already_ran"]], "already_ran"),
               "change_frames_001AD010": change_marks,
               "overlay_hits_other_overlay": len(other_overlay), "unattributed_hits": len(unknown),
               "overlay_status_source": "working tree src/overlays/AREA01 and AREA02 at the time of the run"}
    out = {"summary": summary, "per_beat": per_seg, "new_functions": new, "functions": rows,
           "overlay_AREA02_not_run": [{"addr": hex(c["addr"]), "name": c["name"], "size": c["size"],
                                       "status": c["status"], "pieces": c.get("pieces")}
                                      for c in sorted(ov02, key=lambda c: c["addr"])
                                      if c["addr"] not in ran02],
           "overlay_hits_other_overlay": other_overlay, "unattributed_hits": unknown}
    (OUT / "a02_delta.json").write_text(json.dumps(out, indent=1) + "\n")
    return out


# ---------------------------------------------------------------------------
# AREA04 (a04), s88 lane NEXT, opt-in: route_capture's a04_* beats, replayed
# only when named (`run --segments a04` or beat names), arming the boot
# functions plus the AREA04 overlay (runtime = splat label + 0x40).
# `a04-delta` lists what AREA04 runs that neither the first level, nor AREA01
# (first visit), nor AREA00, nor the AREA01 revisit, nor AREA02 ran.  Segments:
#   a04       the AREA04 main-line beats (the beat that leaves AREA04 up to its
#             area-change consumer 001AD010);
#   a04_side  the AREA04 side beats (none of them leaves the area);
#   a04_exit  from the area change on in the beat that leaves AREA04 (the
#             AREA22 load and arrival; its overlay-range hits are AREA22 code
#             at AREA04 candidate addresses and are listed apart).
# "Already ran" = the first-level census (route_functions.json), beat 15, and
# every beat of the AREA01, AREA00, revisit and AREA02 passes in all phases
# (a02_05's exit phase is AREA04's load and arrival).  An AREA04 overlay
# function counts as already run when a02_05's exit phase paused inside it
# (an AREA02 candidate address inside an AREA04 function, overlay id 5,
# mapped by range).  Nothing here changes the earlier outputs.

AREA04_ID = 5
AREA22_ID = 19


def a04_delta(passes: list[str], a02_passes: list[str], a01r_passes: list[str],
              a00_passes: list[str], a01_passes: list[str]) -> dict:
    c04, p2r04, span04 = _overlay_map("AREA04")
    boot = {a: c for a, c in c04.items() if c["region"] == "boot"}

    # --- what already ran
    first = json.loads((OUT / "route_functions.json").read_text())
    prior: dict[int, str] = {}
    for f in first["functions"]:
        if not f.get("overlay"):
            prior.setdefault(int(f["addr"], 16), "first_level")
    for p in first["summary"].get("passes", ["A"]):
        for h in _hits(p, EXIT_BEAT) or []:
            if not (OVERLAY_BASE <= int(h["pc"], 16) < 0x900000):
                prior.setdefault(int(h["pc"], 16), "beat15")
    prior_ov04: dict[int, str] = {}
    read = []
    earlier = [(rc.A01_BEATS, a01_passes, "area01"), (rc.A00_BEATS, a00_passes, "area00"),
               (rc.A01R_BEATS, a01r_passes, "area01_revisit"), (rc.A02_BEATS, a02_passes, "area02")]
    for beats, ps, source in earlier:
        for name, _src, _fn in beats:
            for p in ps:
                hs = _hits(p, name)
                if hs is None:
                    continue
                read.append(f"{name}:{p}")
                for h in hs:
                    pc = int(h["pc"], 16)
                    if OVERLAY_BASE <= pc < 0x900000:
                        if h.get("overlay_id") == AREA04_ID:      # a02_05's exit phase: AREA04 code
                            real = _containing(span04, pc)
                            if real is not None:
                                prior_ov04.setdefault(real, "area02_exit")
                    else:
                        prior.setdefault(pc, source)

    # --- AREA04
    funcs: dict[tuple[str, int], dict] = {}
    other_overlay, unknown, per_seg, missing = [], [], [], []
    change_marks: dict[str, dict] = {}
    for name, _src, _fn in rc.A04_BEATS:
        seen: dict[str, set] = {}
        found = False
        for p in passes:
            hs = _hits(p, name)
            if hs is None:
                continue
            found = True
            f_chg = ({int(h["pc"], 16): h["frame"] for h in hs}.get(A01_CHANGE)
                     if name in rc.A04_CHANGE_BEATS else None)
            if name in rc.A04_CHANGE_BEATS:
                change_marks.setdefault(name, {})[p] = f_chg
            for h in hs:
                piece_pc = int(h["pc"], 16)
                after = f_chg is not None and h["frame"] >= f_chg
                seg = "a04_exit" if after else ("a04_side" if name in rc.A04_SIDE_BEATS else "a04")
                if OVERLAY_BASE <= piece_pc < 0x900000:
                    if h.get("overlay_id") == AREA04_ID:
                        key = ("overlay:AREA04", p2r04.get(piece_pc, piece_pc))
                        via = "entry" if key[1] == piece_pc else "piece"
                    else:
                        other_overlay.append(dict(h, beat=name))
                        continue
                elif piece_pc in boot:
                    key, via = ("boot", piece_pc), "entry"
                else:
                    unknown.append(dict(h, beat=name))
                    continue
                e = funcs.setdefault(key, {"first_segment": seg, "first_beat": name, "first_frame": h["frame"],
                                           "segments": [], "beats": [], "hit_points": [], "via": []})
                for k, v in (("segments", seg), ("beats", name), ("hit_points", hex(piece_pc)), ("via", via)):
                    if v not in e[k]:
                        e[k].append(v)
                seen.setdefault(seg, set()).add(key)
        if not found:
            missing.append(name)
            continue
        allk = set().union(*seen.values()) if seen else set()
        per_seg.append({"beat": name, "segments": {k: len(v) for k, v in seen.items()},
                        "functions": len(allk),
                        **({"change_frame_001AD010": change_marks.get(name)}
                           if name in rc.A04_CHANGE_BEATS else {})})

    order = ("a04", "a04_side", "a04_exit")
    rows = []
    for key in sorted(funcs, key=lambda k: (k[0] != "boot", k[1])):
        region, pc = key
        c, e = c04[pc], funcs[key]
        already = prior.get(pc) if region == "boot" else prior_ov04.get(pc)
        group = next(g for g in order if g in e["segments"])
        rows.append({"addr": hex(pc), "name": c["name"], "size": c["size"], "region": region,
                     "splat_label": c.get("splat_label"), "status": c["status"],
                     "subsystem": c["subsystem"], "pieces": c.get("pieces"),
                     "hit_points": e["hit_points"], "hit_via": e["via"], "already_ran": already,
                     "first_segment": e["first_segment"], "first_beat": e["first_beat"],
                     "first_frame": e["first_frame"], "segments": e["segments"], "beats": e["beats"],
                     "group": group})
    new = [r for r in rows if not r["already_ran"]]

    def count(rs, k):
        out: dict[str, int] = {}
        for r in rs:
            out[r[k]] = out.get(r[k], 0) + 1
        return out
    by_group = {}
    for g in order:
        rs = [r for r in new if r["group"] == g]
        by_group[g] = {"functions": len(rs), "bytes": sum(r["size"] for r in rs),
                       "boot": sum(1 for r in rs if r["region"] == "boot"),
                       "overlay_AREA04": sum(1 for r in rs if r["region"] == "overlay:AREA04"),
                       "by_status": count(rs, "status")}
    ov04 = [c for c in c04.values() if c["region"] != "boot"]
    ran04 = {pc for (reg, pc) in funcs if reg == "overlay:AREA04"}
    summary = {"passes": passes, "a02_passes": a02_passes, "a01r_passes": a01r_passes,
               "a00_passes": a00_passes, "a01_passes": a01_passes, "beats_missing": missing,
               "prior_beats_read": read, "executed": len(rows),
               "executed_boot": sum(1 for r in rows if r["region"] == "boot"),
               "executed_overlay_AREA04": sum(1 for r in rows if r["region"] == "overlay:AREA04"),
               "overlay_AREA04_functions": len(ov04),
               "new": len(new), "new_bytes": sum(r["size"] for r in new),
               "new_boot": sum(1 for r in new if r["region"] == "boot"),
               "new_overlay_AREA04": sum(1 for r in new if r["region"] == "overlay:AREA04"),
               "new_by_group": by_group, "new_by_status": count(new, "status"),
               "new_by_subsystem": count(new, "subsystem"),
               "already_ran_by_source": count([r for r in rows if r["already_ran"]], "already_ran"),
               "change_frames_001AD010": change_marks,
               "overlay_hits_other_overlay": len(other_overlay),
               "overlay_hits_other_overlay_ids": sorted({h.get("overlay_id") for h in other_overlay},
                                                        key=lambda v: (v is None, v)),
               "unattributed_hits": len(unknown),
               "overlay_status_source": "working tree src/overlays/AREA04 at the time of the run"}
    out = {"summary": summary, "per_beat": per_seg, "new_functions": new, "functions": rows,
           "overlay_AREA04_not_run": [{"addr": hex(c["addr"]), "name": c["name"], "size": c["size"],
                                       "status": c["status"], "pieces": c.get("pieces")}
                                      for c in sorted(ov04, key=lambda c: c["addr"])
                                      if c["addr"] not in ran04],
           "overlay_hits_other_overlay": other_overlay, "unattributed_hits": unknown}
    (OUT / "a04_delta.json").write_text(json.dumps(out, indent=1) + "\n")
    return out


# ---------------------------------------------------------------------------
# AREA22 (a22), s88 lane NEXT, opt-in: route_capture's a22_* beats, replayed
# only when named (`run --segments a22` or beat names), arming the boot
# functions plus the AREA22 overlay pieces (runtime = splat label + 0x40; the
# overlay's text is the 0x40 entry pad 0x823540 and the area init 0x823580,
# which boot 001E7780 calls for key 0x1600 at the area load, so it runs before
# every a22 replay starts; the candidate list groups both pieces into one
# function named after the pad).  `a22-delta` lists what AREA22 runs
# that neither the first level, nor AREA01 (first visit), nor AREA00, nor the
# AREA01 revisit, nor AREA02, nor AREA04 ran.  Segments:
#   a22            the AREA22 main-line beats (a22_02 up to its area-change
#                  consumer 001AD010);
#   a22_side       the AREA22 side beats (a22_s3 up to its 001AD010);
#   a22_exit       a22_02 from 001AD010 on (the AREA01 load and the arrival at
#                  its entry 6);
#   a22_side_exit  a22_s3 from 001AD010 on (the AREA04 reload at its entry 1).
# Overlay-range hits whose resident overlay is not AREA22 (id 19) are another
# area's code at AREA22 candidate addresses and are listed apart.
# "Already ran" = the first-level census (route_functions.json), beat 15, and
# every beat of the AREA01, AREA00, revisit, AREA02 and AREA04 passes in all
# phases (a04_05's exit phase is AREA22's load and arrival).  An AREA22
# overlay function counts as already run when a04_05's exit phase paused
# inside it (an AREA04 candidate address inside it with overlay id 19 resident,
# mapped by range).  The A04 pass cannot show the AREA22 init: its a04_05
# one-shot at 0x823580 was spent at f1 by AREA04's own code there (overlay id
# 5), before the load (001E7780 at f514).  Nothing here changes the earlier
# outputs.

def a22_delta(passes: list[str], a04_passes: list[str], a02_passes: list[str], a01r_passes: list[str],
              a00_passes: list[str], a01_passes: list[str]) -> dict:
    c22, p2r22, span22 = _overlay_map("AREA22")
    boot = {a: c for a, c in c22.items() if c["region"] == "boot"}

    # --- what already ran
    first = json.loads((OUT / "route_functions.json").read_text())
    prior: dict[int, str] = {}
    for f in first["functions"]:
        if not f.get("overlay"):
            prior.setdefault(int(f["addr"], 16), "first_level")
    for p in first["summary"].get("passes", ["A"]):
        for h in _hits(p, EXIT_BEAT) or []:
            if not (OVERLAY_BASE <= int(h["pc"], 16) < 0x900000):
                prior.setdefault(int(h["pc"], 16), "beat15")
    prior_ov22: dict[int, str] = {}
    read = []
    earlier = [(rc.A01_BEATS, a01_passes, "area01"), (rc.A00_BEATS, a00_passes, "area00"),
               (rc.A01R_BEATS, a01r_passes, "area01_revisit"), (rc.A02_BEATS, a02_passes, "area02"),
               (rc.A04_BEATS, a04_passes, "area04")]
    for beats, ps, source in earlier:
        for name, _src, _fn in beats:
            for p in ps:
                hs = _hits(p, name)
                if hs is None:
                    continue
                read.append(f"{name}:{p}")
                for h in hs:
                    pc = int(h["pc"], 16)
                    if OVERLAY_BASE <= pc < 0x900000:
                        if h.get("overlay_id") == AREA22_ID:      # a04_05's exit phase: AREA22 code
                            real = _containing(span22, pc)
                            if real is not None:
                                prior_ov22.setdefault(real, "area04_exit")
                    else:
                        prior.setdefault(pc, source)

    # --- AREA22
    funcs: dict[tuple[str, int], dict] = {}
    other_overlay, unknown, per_seg, missing = [], [], [], []
    change_marks: dict[str, dict] = {}
    for name, _src, _fn in rc.A22_BEATS:
        seen: dict[str, set] = {}
        found = False
        side = name in rc.A22_SIDE_BEATS
        for p in passes:
            hs = _hits(p, name)
            if hs is None:
                continue
            found = True
            f_chg = ({int(h["pc"], 16): h["frame"] for h in hs}.get(A01_CHANGE)
                     if name in rc.A22_CHANGE_BEATS else None)
            if name in rc.A22_CHANGE_BEATS:
                change_marks.setdefault(name, {})[p] = f_chg
            for h in hs:
                piece_pc = int(h["pc"], 16)
                after = f_chg is not None and h["frame"] >= f_chg
                seg = ("a22_side" if side else "a22") + ("_exit" if after else "")
                if OVERLAY_BASE <= piece_pc < 0x900000:
                    if h.get("overlay_id") == AREA22_ID:
                        key = ("overlay:AREA22", p2r22.get(piece_pc, piece_pc))
                        via = "entry" if key[1] == piece_pc else "piece"
                    else:
                        other_overlay.append(dict(h, beat=name, segment=seg))
                        continue
                elif piece_pc in boot:
                    key, via = ("boot", piece_pc), "entry"
                else:
                    unknown.append(dict(h, beat=name))
                    continue
                e = funcs.setdefault(key, {"first_segment": seg, "first_beat": name, "first_frame": h["frame"],
                                           "segments": [], "beats": [], "hit_points": [], "via": []})
                for k, v in (("segments", seg), ("beats", name), ("hit_points", hex(piece_pc)), ("via", via)):
                    if v not in e[k]:
                        e[k].append(v)
                seen.setdefault(seg, set()).add(key)
        if not found:
            missing.append(name)
            continue
        allk = set().union(*seen.values()) if seen else set()
        per_seg.append({"beat": name, "segments": {k: len(v) for k, v in seen.items()},
                        "functions": len(allk),
                        **({"change_frame_001AD010": change_marks.get(name)}
                           if name in rc.A22_CHANGE_BEATS else {})})

    order = ("a22", "a22_side", "a22_exit", "a22_side_exit")
    rows = []
    for key in sorted(funcs, key=lambda k: (k[0] != "boot", k[1])):
        region, pc = key
        c, e = c22[pc], funcs[key]
        already = prior.get(pc) if region == "boot" else prior_ov22.get(pc)
        group = next(g for g in order if g in e["segments"])
        rows.append({"addr": hex(pc), "name": c["name"], "size": c["size"], "region": region,
                     "splat_label": c.get("splat_label"), "status": c["status"],
                     "subsystem": c["subsystem"], "pieces": c.get("pieces"),
                     "hit_points": e["hit_points"], "hit_via": e["via"], "already_ran": already,
                     "first_segment": e["first_segment"], "first_beat": e["first_beat"],
                     "first_frame": e["first_frame"], "segments": e["segments"], "beats": e["beats"],
                     "group": group})
    new = [r for r in rows if not r["already_ran"]]

    def count(rs, k):
        out: dict[str, int] = {}
        for r in rs:
            out[r[k]] = out.get(r[k], 0) + 1
        return out
    by_group = {}
    for g in order:
        rs = [r for r in new if r["group"] == g]
        by_group[g] = {"functions": len(rs), "bytes": sum(r["size"] for r in rs),
                       "boot": sum(1 for r in rs if r["region"] == "boot"),
                       "overlay_AREA22": sum(1 for r in rs if r["region"] == "overlay:AREA22"),
                       "by_status": count(rs, "status")}
    ov22 = [c for c in c22.values() if c["region"] != "boot"]
    ran22 = {pc for (reg, pc) in funcs if reg == "overlay:AREA22"}
    summary = {"passes": passes, "a04_passes": a04_passes, "a02_passes": a02_passes,
               "a01r_passes": a01r_passes, "a00_passes": a00_passes, "a01_passes": a01_passes,
               "beats_missing": missing, "prior_beats_read": read, "executed": len(rows),
               "executed_boot": sum(1 for r in rows if r["region"] == "boot"),
               "executed_overlay_AREA22": sum(1 for r in rows if r["region"] == "overlay:AREA22"),
               "overlay_AREA22_functions": len(ov22),
               "new": len(new), "new_bytes": sum(r["size"] for r in new),
               "new_boot": sum(1 for r in new if r["region"] == "boot"),
               "new_overlay_AREA22": sum(1 for r in new if r["region"] == "overlay:AREA22"),
               "new_by_group": by_group, "new_by_status": count(new, "status"),
               "new_by_subsystem": count(new, "subsystem"),
               "already_ran_by_source": count([r for r in rows if r["already_ran"]], "already_ran"),
               "change_frames_001AD010": change_marks,
               "overlay_hits_other_overlay": len(other_overlay),
               "overlay_hits_other_overlay_ids": sorted({h.get("overlay_id") for h in other_overlay},
                                                        key=lambda v: (v is None, v)),
               "unattributed_hits": len(unknown),
               "overlay_status_source": "working tree src/overlays/AREA22 at the time of the run"}
    out = {"summary": summary, "per_beat": per_seg, "new_functions": new, "functions": rows,
           "overlay_AREA22_not_run": [{"addr": hex(c["addr"]), "name": c["name"], "size": c["size"],
                                       "status": c["status"], "pieces": c.get("pieces")}
                                      for c in sorted(ov22, key=lambda c: c["addr"])
                                      if c["addr"] not in ran22],
           "overlay_hits_other_overlay": other_overlay, "unattributed_hits": unknown}
    (OUT / "a22_delta.json").write_text(json.dumps(out, indent=1) + "\n")
    return out


# ---------------------------------------------------------------------------
# AREA01 upper floor (a01u) and AREA06 (a06), s88 lane NEXT, opt-in:
# route_capture's a01u_* / a06_* beats, replayed only when named (`run
# --segments a01u` / `--segments a06` or beat names), arming the boot
# functions plus the AREA01 (a01u) or AREA06 (a06) overlay (runtime = splat
# label + 0x40).  `a01u-delta` and `a06-delta` list what the group runs that
# no earlier level ran (chain_delta below).  Segments, for a group <g>:
#   <g>            the main-line beats (the beat that leaves the area up to
#                  its area-change consumer 001AD010);
#   <g>_side       the side beats (a side beat that leaves the area, up to its
#                  001AD010);
#   <g>_exit       the main-line beat from 001AD010 on (the next area's load
#                  and arrival);
#   <g>_side_exit  a side beat from 001AD010 on.
# Overlay-range hits whose resident overlay is not the group's own are
# another area's code at the group's candidate addresses and are listed apart.
# "Already ran" = the first-level census (route_functions.json), beat 15, and
# every beat of every earlier group's passes in all phases (boot functions);
# an overlay function of the group's area counts as already run when an
# earlier pass paused inside it with that area's overlay resident (a hit at
# the earlier group's candidate address, mapped by range into this overlay's
# functions, or a hit of an earlier pass that armed the same overlay).
# Nothing here changes the earlier outputs.

AREA06_ID = 6


def chain_delta(tag: str, overlay: str, overlay_id: int, beats, side_beats, change_beats,
                passes: list[str], earlier: list[tuple], out_name: str) -> dict:
    """Generic delta of one opt-in route group against everything before it.
    `earlier` is a list of (beats, passes, source, armed overlay)."""
    cx, p2rx, spanx = _overlay_map(overlay)
    boot = {a: c for a, c in cx.items() if c["region"] == "boot"}

    first = json.loads((OUT / "route_functions.json").read_text())
    prior: dict[int, str] = {}
    for f in first["functions"]:
        if not f.get("overlay"):
            prior.setdefault(int(f["addr"], 16), "first_level")
    for p in first["summary"].get("passes", ["A"]):
        for h in _hits(p, EXIT_BEAT) or []:
            if not (OVERLAY_BASE <= int(h["pc"], 16) < 0x900000):
                prior.setdefault(int(h["pc"], 16), "beat15")
    prior_ov: dict[int, str] = {}
    read = []
    for e_beats, ps, source, _armed in earlier:
        for name, _src, _fn in e_beats:
            for p in ps:
                hs = _hits(p, name)
                if hs is None:
                    continue
                read.append(f"{name}:{p}")
                for h in hs:
                    pc = int(h["pc"], 16)
                    if OVERLAY_BASE <= pc < 0x900000:
                        if h.get("overlay_id") == overlay_id:
                            real = _containing(spanx, pc)
                            if real is not None:
                                prior_ov.setdefault(real, source)
                    else:
                        prior.setdefault(pc, source)

    funcs: dict[tuple[str, int], dict] = {}
    other_overlay, unknown, per_seg, missing = [], [], [], []
    change_marks: dict[str, dict] = {}
    region_ov = f"overlay:{overlay}"
    for name, _src, _fn in beats:
        seen: dict[str, set] = {}
        found = False
        side = name in side_beats
        for p in passes:
            hs = _hits(p, name)
            if hs is None:
                continue
            found = True
            f_chg = ({int(h["pc"], 16): h["frame"] for h in hs}.get(A01_CHANGE)
                     if name in change_beats else None)
            if name in change_beats:
                change_marks.setdefault(name, {})[p] = f_chg
            for h in hs:
                piece_pc = int(h["pc"], 16)
                after = f_chg is not None and h["frame"] >= f_chg
                seg = (tag + "_side" if side else tag) + ("_exit" if after else "")
                if OVERLAY_BASE <= piece_pc < 0x900000:
                    if h.get("overlay_id") == overlay_id:
                        key = (region_ov, p2rx.get(piece_pc, piece_pc))
                        via = "entry" if key[1] == piece_pc else "piece"
                    else:
                        other_overlay.append(dict(h, beat=name, segment=seg))
                        continue
                elif piece_pc in boot:
                    key, via = ("boot", piece_pc), "entry"
                else:
                    unknown.append(dict(h, beat=name))
                    continue
                e = funcs.setdefault(key, {"first_segment": seg, "first_beat": name, "first_frame": h["frame"],
                                           "segments": [], "beats": [], "hit_points": [], "via": []})
                for k, v in (("segments", seg), ("beats", name), ("hit_points", hex(piece_pc)), ("via", via)):
                    if v not in e[k]:
                        e[k].append(v)
                seen.setdefault(seg, set()).add(key)
        if not found:
            missing.append(name)
            continue
        allk = set().union(*seen.values()) if seen else set()
        per_seg.append({"beat": name, "segments": {k: len(v) for k, v in seen.items()},
                        "functions": len(allk),
                        **({"change_frame_001AD010": change_marks.get(name)}
                           if name in change_beats else {})})

    order = (tag, tag + "_side", tag + "_exit", tag + "_side_exit")
    rows = []
    for key in sorted(funcs, key=lambda k: (k[0] != "boot", k[1])):
        region, pc = key
        c, e = cx[pc], funcs[key]
        already = prior.get(pc) if region == "boot" else prior_ov.get(pc)
        group = next(g for g in order if g in e["segments"])
        rows.append({"addr": hex(pc), "name": c["name"], "size": c["size"], "region": region,
                     "splat_label": c.get("splat_label"), "status": c["status"],
                     "subsystem": c["subsystem"], "pieces": c.get("pieces"),
                     "hit_points": e["hit_points"], "hit_via": e["via"], "already_ran": already,
                     "first_segment": e["first_segment"], "first_beat": e["first_beat"],
                     "first_frame": e["first_frame"], "segments": e["segments"], "beats": e["beats"],
                     "group": group})
    new = [r for r in rows if not r["already_ran"]]

    def count(rs, k):
        out: dict[str, int] = {}
        for r in rs:
            out[r[k]] = out.get(r[k], 0) + 1
        return out
    by_group = {}
    for g in order:
        rs = [r for r in new if r["group"] == g]
        by_group[g] = {"functions": len(rs), "bytes": sum(r["size"] for r in rs),
                       "boot": sum(1 for r in rs if r["region"] == "boot"),
                       "overlay_" + overlay: sum(1 for r in rs if r["region"] == region_ov),
                       "by_status": count(rs, "status")}
    ovx = [c for c in cx.values() if c["region"] != "boot"]
    ranx = {pc for (reg, pc) in funcs if reg == region_ov}
    summary = {"passes": passes, "earlier_passes": {src: ps for _b, ps, src, _a in earlier},
               "beats_missing": missing, "prior_beats_read": read, "executed": len(rows),
               "executed_boot": sum(1 for r in rows if r["region"] == "boot"),
               "executed_overlay_" + overlay: sum(1 for r in rows if r["region"] == region_ov),
               "overlay_" + overlay + "_functions": len(ovx),
               "new": len(new), "new_bytes": sum(r["size"] for r in new),
               "new_boot": sum(1 for r in new if r["region"] == "boot"),
               "new_overlay_" + overlay: sum(1 for r in new if r["region"] == region_ov),
               "new_by_group": by_group, "new_by_status": count(new, "status"),
               "new_by_subsystem": count(new, "subsystem"),
               "already_ran_by_source": count([r for r in rows if r["already_ran"]], "already_ran"),
               "overlay_already_ran": {hex(k): v for k, v in sorted(prior_ov.items())},
               "change_frames_001AD010": change_marks,
               "overlay_hits_other_overlay": len(other_overlay),
               "overlay_hits_other_overlay_ids": sorted({h.get("overlay_id") for h in other_overlay},
                                                        key=lambda v: (v is None, v)),
               "unattributed_hits": len(unknown),
               "overlay_status_source": f"working tree src/overlays/{overlay} at the time of the run"}
    out = {"summary": summary, "per_beat": per_seg, "new_functions": new, "functions": rows,
           "overlay_" + overlay + "_not_run": [{"addr": hex(c["addr"]), "name": c["name"], "size": c["size"],
                                                "status": c["status"], "pieces": c.get("pieces"),
                                                "already_ran": prior_ov.get(c["addr"])}
                                               for c in sorted(ovx, key=lambda c: c["addr"])
                                               if c["addr"] not in ranx],
           "overlay_hits_other_overlay": other_overlay, "unattributed_hits": unknown}
    (OUT / out_name).write_text(json.dumps(out, indent=1) + "\n")
    return out


def _earlier_groups(a) -> list[tuple]:
    """The earlier groups for a01u-delta / a06-delta, oldest first."""
    return [(rc.A01_BEATS, a.a01_passes.split(","), "area01", "AREA01"),
            (rc.A00_BEATS, a.a00_passes.split(","), "area00", "AREA00"),
            (rc.A01R_BEATS, a.a01r_passes.split(","), "area01_revisit", "AREA01"),
            (rc.A02_BEATS, a.a02_passes.split(","), "area02", "AREA02"),
            (rc.A04_BEATS, a.a04_passes.split(","), "area04", "AREA04"),
            (rc.A22_BEATS, a.a22_passes.split(","), "area22", "AREA22")]


# ---------------------------------------------------------------------------
# World graph (`graph`): static, from the code and data only (no emulator).
#
# Reads the pinned boot ELF (config/SCUS_971.12), every extract/OVERLAY/AREAnn.BIN
# (flat at 0x823500) and the local splat trees (function starts only), and
# writes build/s87/census/world_graph.json plus world_graph_tables.md (Markdown
# tables of addresses and numbers; no instructions, no disc text). The port's
# docs and the decomp's docs/WORLD_GRAPH.md explain the rules; in short:
#   * area registries through tools/area_overview.py (placements D_0024D7C0,
#     deferred groups D_0024D820, spawn D_0024D650, doors D_0024E140);
#   * a linear scan of every function (boot and overlay): %hi/%lo pairs give
#     absolute loads/stores in the story RAM 0x810600..0x810E00 (an `addu` of a
#     register holding such an address marks the access "indexed"), and every
#     `jal` with the constant arguments a0..a3 known at the call (delay slot
#     included; values from lui/addiu/ori and register moves in straight-line
#     code, so a value set on another path can be missed: "?" = not known);
#   * script chains (0x40-byte records) from the second argument of every
#     001BA1A0 call and from data words pointing into the same data section;
#     op06 subs 0/1 write D_00810758[slot] = 1 / 0xFF, subs 3/5/6 the counter
#     D_008107D8[slot]; op07 sub 5 writes D_00810758[slot] = 0xFF, sub 6
#     D_008107D8[slot] = operand; op09's word +4 is its callback;
#   * door gates: 001BC350 tests D_00810841[area] & (1 << id) for model 0x15
#     only, 001BB860 for models 0x16, 0x17 and 0x3E (both read from their
#     instructions); the destination record is D_0024E140[area][id & 0x7F]
#     (id bit 7: area change to rec[0] entry rec[1], sub rec[3] when rec[2] != 0
#     else D_00810730[rec[0]] & 0x7F; bit 7 clear: a room move).
# The scan is a lead generator: every progression claim in the docs is checked
# against the function's C or instructions by hand.

WG_AREAS = (0, 1, 2, 3, 4, 6, 7, 8, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22)
WG_LO, WG_HI = 0x810600, 0x810E00
WG_LOADS = {0x20: 1, 0x21: 2, 0x22: 4, 0x23: 4, 0x24: 1, 0x25: 2, 0x26: 4, 0x27: 4, 0x37: 8,
            0x1E: 16, 0x31: 4, 0x1A: 8, 0x1B: 8}
WG_STORES = {0x28: 1, 0x29: 2, 0x2A: 4, 0x2B: 4, 0x2E: 4, 0x3F: 8, 0x1F: 16, 0x39: 4, 0x2C: 8, 0x2D: 8}
WG_CALLER_SAVED = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 24, 25, 31)
WG_SPLAT_LINE = re.compile(r"/\*\s*([0-9A-F]+)\s+([0-9A-F]{8})\s+[0-9A-F]{8}\s*\*/")
WG_DOORS = {0x1BC350: ("001BC350 (hinged)", (0x15,)), 0x1BB860: ("001BB860 (slider)", (0x16, 0x17, 0x3E)),
            0x1BD560: ("001BD560 (lift)", None), 0x1BD9F0: ("001BD9F0 (door)", None)}
WG_LOCK_WRITERS = {
    0x1581A0: "seal 001581A0: a hit (+0x36 != 0) sets the bit",
    0x1582E0: "seal 001582E0: a hit sets the bit and D_00810842 bit 1",
    0x158430: "seal 00158430: a hit sets the bit and D_00810845 bit 2",
    0x158810: "reader 00158810 (001576E0): item 0x23 (models 0x12, 0x2F) / 0x24 (other models) "
              "-> script 0x246C20 -> 001580C0",
    0x158EC0: "panel 00158EC0 (00157860 arg 1): item 0x1B -> script 0x2478A0 -> 001580C0",
    0x159210: "panel 00159210 (00157860 arg 0): item 0x1B -> script 0x247BA0 -> 001580C0 "
              "(001580C0 skips model 0x2C)",
    0x159E70: "socket 00159E70: item 0x29 (area 4) / 0x2A (other areas) non-zero -> the bit",
    0x15A070: "switch 0015A070: Use -> 00159FC0 -> script 0x246A20 -> 00157360 sets the bit",
}
WG_PICKUPS = (0x15AFA0, 0x219550)
WG_ITEM_CALLS = {0x1C47A0: "C64", 0x1C4720: "CB8", 0x1C4760: "CC3"}


def _wg_elf() -> bytes:
    return (ROOT / "config/SCUS_971.12").read_bytes()


def _wg_first_addr(path: Path, overlay: bool) -> int | None:
    for ln in path.open():
        m = WG_SPLAT_LINE.search(ln)
        if m:
            return OVERLAY_BASE + int(m.group(1), 16) if overlay else int(m.group(2), 16)
    return None


def _wg_scan(words: list[int], base: int, starts: set[int]) -> tuple[list[dict], list[dict]]:
    """Linear scan (see the section comment). Returns (accesses, calls) keyed by pc."""
    acc, calls, regs, pending = [], [], {}, None
    for i, w in enumerate(words):
        pc = base + 4 * i
        if pc in starts:
            regs = {}
        op, rs, rt, rd, imm = w >> 26, (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31, w & 0xFFFF
        simm = imm - 0x10000 if imm & 0x8000 else imm
        dest, newval = None, None
        if op == 0x0F:
            dest, newval = rt, ("v", imm << 16)
        elif op in (0x09, 0x19, 0x0D):
            src = regs.get(rs)
            if rs == 0:
                dest, newval = rt, ("v", (imm if op == 0x0D else simm) & 0xFFFFFFFF)
            elif src and src[0] == "v":
                dest, newval = rt, ("v", (src[1] | imm) if op == 0x0D else (src[1] + simm) & 0xFFFFFFFF)
            else:
                dest = rt
        elif op in WG_LOADS or op in WG_STORES:
            b = regs.get(rs)
            ea, idx = None, False
            if rs == 0:
                ea = simm & 0xFFFFFFFF
            elif b:
                ea, idx = (b[1] + simm) & 0xFFFFFFFF, b[0] == "i"
            if ea is not None and WG_LO <= ea < WG_HI:
                val = None
                if op in WG_STORES:
                    r = regs.get(rt)
                    val = 0 if rt == 0 else (r[1] if r and r[0] == "v" else None)
                acc.append(dict(pc=pc, ea=ea, store=op in WG_STORES, indexed=idx, val=val))
            if op in WG_LOADS and op != 0x31:
                dest = rt
        elif op == 0x00:
            funct = w & 0x3F
            if funct in (0x21, 0x2D, 0x25):
                a_, c_ = regs.get(rs), regs.get(rt)
                if rs == 0 and rt == 0:
                    dest, newval = rd, ("v", 0)
                elif rt == 0 and a_:
                    dest, newval = rd, a_
                elif rs == 0 and c_:
                    dest, newval = rd, c_
                elif a_ and a_[0] == "v" and WG_LO <= a_[1] < WG_HI:
                    dest, newval = rd, ("i", a_[1])
                elif c_ and c_[0] == "v" and WG_LO <= c_[1] < WG_HI:
                    dest, newval = rd, ("i", c_[1])
                else:
                    dest = rd
            elif funct not in (0x08, 0x0C, 0x0D, 0x0F):
                dest = rd
        elif op == 0x1C:
            if (w & 0x3F) == 0x28 and ((w >> 6) & 31) == 0x18 and rt == 0:
                dest, newval = rd, ("v", 0) if rs == 0 else regs.get(rs)
            else:
                dest = rd
        elif op in (0x08, 0x0A, 0x0B, 0x0C, 0x0E, 0x18):
            dest = rt
        elif op == 0x11 and rs in (0, 1):
            dest = rt
        elif op == 0x03:
            pending = (pc, ((w & 0x3FFFFFF) << 2) | (pc & 0xF0000000))
        if dest:
            if newval is None:
                regs.pop(dest, None)
            else:
                regs[dest] = newval
        if pending and pending[0] == pc - 4:
            args = [regs.get(r) for r in (4, 5, 6, 7)]
            calls.append(dict(pc=pending[0], target=pending[1],
                              args=[a_[1] if a_ and a_[0] == "v" else None for a_ in args]))
            for r in WG_CALLER_SAVED:
                regs.pop(r, None)
            pending = None
    return acc, calls


def _wg_modules(elf: bytes) -> dict[str, dict]:
    mods = {}
    bstarts = {a for a in (_wg_first_addr(p, False) for p in (ROOT / "build/asm/matchings/main/code").glob("*.s"))
               if a is not None}
    lo, hi = 0x100000, max(bstarts) + 0x4000
    words = list(struct.unpack_from(f"<{(hi - lo) // 4}I", elf, 0x300))
    mods["boot"] = dict(base=lo, words=words, starts=bstarts, text=[lo, hi], data=[0x246000, 0x275B00])
    for area in WG_AREAS:
        ov = (ROOT / f"extract/OVERLAY/AREA{area:02d}.BIN").read_bytes()
        ov_id, tsz, dsz, bsz = struct.unpack_from("<I4xIII", ov, 4)
        d = ROOT / f"build/overlays/AREA{area:02d}/asm/matchings/AREA{area:02d}/code"
        st = {a for a in (_wg_first_addr(p, True) for p in d.glob("*.s")) if a is not None}
        tlo = OVERLAY_TEXT
        words = list(struct.unpack_from(f"<{tsz // 4}I", ov, 0x40))
        mods[f"AREA{area:02d}"] = dict(base=tlo, words=words, starts=st, text=[tlo, tlo + tsz],
                                       data=[tlo + tsz, tlo + tsz + dsz], ov=ov, ov_id=ov_id, area=area)
    for name, m in mods.items():
        # function starts: splat pieces plus every jal target inside the module's text
        _, calls = _wg_scan(m["words"], m["base"], m["starts"])
        # a splat piece starts a function only after a function-return pair (overlay pieces
        # can split one function in two); jal targets always start one
        if name != "boot":
            m["starts"] = {s for s in m["starts"] if s == m["base"]
                           or (s - m["base"] >= 8 and m["words"][(s - m["base"]) // 4 - 2] == 0x03E00008)}
            # placement / deferred behaviours and script op09 callbacks start functions
            # (other code pointers in the data can be switch-table targets)
            tab = _wg_tables(elf, m["area"], m["ov"])
            beh = {r["behavior"] for s in tab["subs"] for r in tab["placements"][s]["records"]}
            beh |= {r["behavior"] for s in tab["subs"] for g_ in tab["deferred"][s] for r in g_["records"]}
            m["calls"] = calls
            beh |= {r["w1"] for recs in _wg_scripts(elf, m).values() for r in recs if r["op"] == 9}
            m["starts"] |= {v for v in beh if m["text"][0] <= v < m["text"][1]}
        m["starts"] |= {c["target"] for c in calls if m["text"][0] <= c["target"] < m["text"][1]}
        acc, calls = _wg_scan(m["words"], m["base"], m["starts"])
        ss = sorted(m["starts"])
        import bisect
        for e in acc + calls:
            k = bisect.bisect_right(ss, e["pc"]) - 1
            e["fn"] = ss[k] if k >= 0 else m["base"]
        m["acc"], m["calls"] = acc, calls
    return mods


def _wg_read(elf: bytes, ov: bytes | None, a: int, n: int) -> bytes:
    if 0x100000 <= a and a + n <= 0x100000 + 0x175B00:
        return elf[a - 0x100000 + 0x300:a - 0x100000 + 0x300 + n]
    if ov is not None and OVERLAY_BASE <= a and a + n <= OVERLAY_BASE + len(ov):
        return ov[a - OVERLAY_BASE:a - OVERLAY_BASE + n]
    raise KeyError(hex(a))


def _wg_scripts(elf: bytes, m: dict) -> dict[int, list[dict]]:
    """Script chains of one module: {entry: [records]}."""
    ov = m.get("ov")
    lo, hi = m["data"]
    ents = {c["args"][1] for c in m["calls"] if c["target"] == 0x1BA1A0 and c["args"][1]
            and lo <= c["args"][1] < hi}
    for a in range(lo, hi, 4):
        try:
            v = struct.unpack("<I", _wg_read(elf, ov, a, 4))[0]
        except KeyError:
            continue
        if lo <= v < hi and v % 4 == 0:
            ents.add(v)
    out = {}
    for e in sorted(ents):
        pc, recs = e, []
        for _ in range(128):
            if not (lo <= pc and pc + 0x40 <= hi):
                recs = None
                break
            w = struct.unpack("<16I", _wg_read(elf, ov, pc, 0x40))
            if w[0] & 0x1FFFF000 or (w[0] & 0xFFF) > 0x1A:
                recs = None
                break
            recs.append(dict(addr=pc, op=w[0] & 0xFFF, sub=w[2], w1=w[1], slot=w[5], operand=w[6]))
            if w[0] & 0x80000000:
                break
            pc = w[1] if w[0] & 0x40000000 else pc + 0x40
        else:
            recs = None
        if recs and (len(recs) >= 2 or recs[0]["op"] in (6, 7, 9)):
            out[e] = recs
    return out


def _wg_tables(elf: bytes, area: int, ov: bytes) -> dict:
    import area_overview as AO  # lazy: only `graph` needs it
    img = AO.Image(elf, ov, None)
    spawn = {}
    try:
        spawn = AO.spawn_tables(img, area)
    except Exception:
        pass
    place_desc = img.u32(AO.D_PLACE + 4 * area)
    defer_desc = img.u32(AO.D_DEFER + 4 * area)
    nest_base = struct.unpack("<h", img.read(0x24A850 + 2 * area, 2))[0] or 1
    subs, placements, deferred = [], {}, {}
    for s in sorted(spawn):
        try:
            t = img.u32(place_desc + 4 * s)
            placements[s] = dict(table=t, records=AO.place_records(img, t))
        except Exception:
            continue          # a descriptor word past the area's own subs
        subs.append(s)
        items = []
        if defer_desc and s < nest_base:
            q = img.u32(defer_desc + 4 * s)
            while q:
                it = img.u32(q)
                if not it:
                    break
                items.append(dict(item=it, records=AO.defer_records(img, it)))
                q += 4
        deferred[s] = items
    return dict(subs=subs, spawn={s: spawn[s] for s in subs}, placements=placements, deferred=deferred,
                doors=AO.door_table(img, area), place_desc=place_desc, defer_desc=defer_desc)


def world_graph() -> dict:
    elf = _wg_elf()
    mods = _wg_modules(elf)
    boot = mods["boot"]

    def key(mod: str, a: int) -> tuple[str, int]:
        return ("boot", a) if a < OVERLAY_BASE else (mod, a)

    # reverse edges for "who reaches this function": direct calls, op09 callbacks, script starters
    callers: dict[tuple, set] = {}
    for name, m in mods.items():
        for c in m["calls"]:
            callers.setdefault(key(name, c["target"]), set()).add(key(name, c["fn"]))
    scripts = {name: _wg_scripts(elf, m) for name, m in mods.items()}
    starters: dict[tuple, set] = {}   # (module, chain entry) -> functions starting it
    for name, m in mods.items():
        for c in m["calls"]:
            if c["target"] == 0x1BA1A0 and c["args"][1]:
                starters.setdefault(key(name, c["args"][1]), set()).add(key(name, c["fn"]))
    cb_of: dict[tuple, set] = {}      # callback function -> chains holding it
    for name, ch in scripts.items():
        for e, recs in ch.items():
            for r in recs:
                if r["op"] == 9:
                    cb_of.setdefault(key(name, r["w1"]), set()).add(key(name, e))

    out = dict(areas={}, rules=dict(doors={f"{k:#x}": v[0] for k, v in WG_DOORS.items()},
                                     lock_writers={f"{k:#x}": v for k, v in WG_LOCK_WRITERS.items()}))
    for area in WG_AREAS:
        name = f"AREA{area:02d}"
        m = mods[name]
        tab = _wg_tables(elf, area, m["ov"])
        owners_of: dict[tuple, set] = {}
        for s in tab["subs"]:
            for r in tab["placements"][s]["records"]:
                owners_of.setdefault(key(name, r["behavior"]), set()).add(f"s{s}[{r['index']}]")
            for g in tab["deferred"][s]:
                for r in g["records"]:
                    owners_of.setdefault(key(name, r["behavior"]), set()).add(f"s{s} g[{r['index']}]")

        def owners(f: tuple, depth: int = 0, seen: set | None = None) -> set:
            seen = seen if seen is not None else set()
            if f in seen or depth > 8:
                return set()
            seen.add(f)
            res = set(owners_of.get(f, ()))
            if f[0] != name and f[0] != "boot":
                return res
            for c in callers.get(f, ()):
                if c[0] in (name, "boot") and (c[0] == name or depth == 0):
                    res |= owners(c, depth + 1, seen)
            for ch in cb_of.get(f, ()):
                for st in starters.get(ch, ()):
                    res |= owners(st, depth + 1, seen)
            return res

        def own(fn: int) -> str:
            o = owners(key(name, fn)) if fn >= OVERLAY_BASE else set()
            return ", ".join(sorted(o)) or "—"

        doors_rec = tab["doors"]["records"]
        doors = []
        seen_tables = {}
        for s in tab["subs"]:
            t = tab["placements"][s]["table"]
            if t in seen_tables.values():
                continue
            seen_tables[s] = t
            same = [x for x in tab["subs"] if tab["placements"][x]["table"] == t]
            for r in tab["placements"][s]["records"]:
                b = r["behavior"]
                ov_door = b >= OVERLAY_BASE and _wg_reaches(m, b, (0x1BC150, 0x1BC240))
                if b not in WG_DOORS and not ov_door:
                    continue
                did = r["flags2"] & 0x7F
                rec = doors_rec[did]["bytes"] if did < len(doors_rec) else None
                if rec is None:
                    dest = "?"
                elif r["flags2"] & 0x80:
                    dest = (f"AREA{rec[0]:02d} e{rec[1]} s={rec[3]}" if rec[2]
                            else f"AREA{rec[0]:02d} e{rec[1]} s=D_00810730[{rec[0]}]")
                else:
                    dest = f"room move e{rec[0]} / e{rec[1]}"
                if b in WG_DOORS:
                    label, gated = WG_DOORS[b]
                    if gated is None:
                        gate = ("flag 0x14 key (first Use sets it to 1, runs on at 0xFF)" if b == 0x1BD560
                                and r["model"] == 0x0B else "see text")
                    else:
                        gate = (f"D_{0x810841 + area:08X} bit {r['flags2'] & 31}" if r["model"] in gated
                                else "none")
                else:
                    label, gate = f"{b:#x} (overlay)", "see text"
                doors.append(dict(subs=same, index=r["index"], behavior=b, label=label, model=r["model"],
                                  id=r["flags2"], dest=dest, gate=gate, record=rec, pos=r["pos"]))
        # other area-change sites
        changes = []
        for c in m["calls"]:
            if c["target"] == 0x1B0C60:
                changes.append(dict(fn=c["fn"], pc=c["pc"], owners=own(c["fn"]),
                                    area=c["args"][0], sub=c["args"][1], entry=c["args"][2]))
        bstores = {}
        for a_ in m["acc"]:
            if a_["store"] and 0x8106B5 <= a_["ea"] <= 0x8106B8 and a_["fn"] >= OVERLAY_BASE:
                bstores.setdefault(a_["fn"], {})[f"{a_['ea'] - 0x810600:X}"] = a_["val"]
        for fn, v in bstores.items():
            changes.append(dict(fn=fn, owners=own(fn), direct=v))
        # lock bits
        lock_writers = []
        for s in tab["subs"]:
            for r in tab["placements"][s]["records"]:
                if r["behavior"] in WG_LOCK_WRITERS:
                    lock_writers.append(dict(where=f"s{s}[{r['index']}]", behavior=r["behavior"],
                                             model=r["model"], bit=r["flags2"] & 31,
                                             rule=WG_LOCK_WRITERS[r["behavior"]], pos=r["pos"]))
        code_locks = [dict(fn=a_["fn"], owners=own(a_["fn"]), ea=a_["ea"], indexed=a_["indexed"], val=a_["val"])
                      for a_ in m["acc"] if a_["store"] and 0x810840 <= a_["ea"] < 0x810858]
        # flags / counters
        fc = []
        for a_ in m["acc"]:
            if a_["store"] and 0x810758 <= a_["ea"] < 0x810840 and not a_["indexed"]:
                arr = "flag" if a_["ea"] < 0x8107D8 else "counter"
                idx = a_["ea"] - (0x810758 if arr == "flag" else 0x8107D8)
                fc.append(dict(src="code", fn=a_["fn"], owners=own(a_["fn"]), array=arr, index=idx, val=a_["val"]))
        for e, recs in scripts[name].items():
            st = sorted({f"{x[1]:#x}" for x in starters.get(key(name, e), ())})
            for r in recs:
                w = None
                if r["op"] == 6 and r["sub"] in (0, 1):
                    w = ("flag", 1 if r["sub"] == 0 else 0xFF)
                elif r["op"] == 6 and r["sub"] == 3:
                    w = ("counter", r["operand"] & 0xFF)
                elif r["op"] == 6 and r["sub"] in (5, 6):
                    w = ("counter", "+1" if r["sub"] == 5 else "-1")
                elif r["op"] == 7 and r["sub"] == 5:
                    w = ("flag", 0xFF)
                elif r["op"] == 7 and r["sub"] == 6:
                    w = ("counter", r["operand"] & 0xFF)
                if w:
                    fc.append(dict(src="script", chain=e, record=r["addr"], op=f"{r['op']:02X}/{r['sub']}",
                                   array=w[0], index=r["slot"], val=w[1], started_by=st))
        tests = sorted({c["args"][1] for c in m["calls"] if c["target"] == 0x1BA1C0 and c["args"][1] is not None})
        # items
        pickups = []
        for s in tab["subs"]:
            recs = [("", r) for r in tab["placements"][s]["records"]]
            recs += [("g", r) for g in tab["deferred"][s] for r in g["records"]]
            for tag, r in recs:
                if r["behavior"] in WG_PICKUPS:
                    arr = {0: "C64", 1: "CB8"}.get(r["model"], "CC3")
                    pickups.append(dict(where=f"s{s} {tag}[{r['index']}]" if tag else f"s{s}[{r['index']}]",
                                        behavior=r["behavior"], array=arr, index=r["flags2"], pos=r["pos"]))
        item_calls = [dict(fn=c["fn"], owners=own(c["fn"]), array=WG_ITEM_CALLS[c["target"]],
                           index=c["args"][0], n=c["args"][1])
                      for c in m["calls"] if c["target"] in WG_ITEM_CALLS]
        item_takes = [dict(fn=c["fn"], owners=own(c["fn"]), index=c["args"][0], n=c["args"][1])
                      for c in m["calls"] if c["target"] == 0x1C47E0]
        sub_writes = [dict(fn=a_["fn"], owners=own(a_["fn"]), area=a_["ea"] - 0x810730, val=a_["val"])
                      for a_ in m["acc"] if a_["store"] and 0x810730 <= a_["ea"] < 0x810748 and not a_["indexed"]]
        # op11 (001B7700): D_008106CE / CF request an in-place sub change of the
        # current area (sub 1: sub = slot; subs 0 / 2: slot | 0x80, kept in
        # D_00810730[area] by 001FEFE0 / 001FF030)
        for e, recs in scripts[name].items():
            for r in recs:
                if r["op"] == 0x11:
                    sub_writes.append(dict(script=e, record=r["addr"], area=area,
                                           val=r["slot"] if r["sub"] == 1 else r["slot"] | 0x80,
                                           started_by=sorted({f"{x[1]:#x}" for x in starters.get(key(name, e), ())})))
        out["areas"][name] = dict(
            area=area, overlay_id=m["ov_id"], subs=tab["subs"],
            spawn={s: [dict(index=e["index"], pos=e["pos"], yaw=e["yaw"]) for e in v["entries"]]
                   for s, v in tab["spawn"].items()},
            placements={s: len(v["records"]) for s, v in tab["placements"].items()},
            door_table=tab["doors"]["base"], doors=doors, changes=changes, lock_byte=0x810841 + area,
            lock_writers=lock_writers, code_lock_writes=code_locks, flags_counters=fc, flag_tests=tests,
            pickups=pickups, item_calls=item_calls, item_takes=item_takes, sub_writes=sub_writes,
            scripts=len(scripts[name]))
    # boot-level facts: area-change requests and sub writers
    out["boot"] = dict(
        area_changes=[dict(fn=c["fn"], pc=c["pc"], area=c["args"][0], sub=c["args"][1], entry=c["args"][2])
                      for c in boot["calls"] if c["target"] == 0x1B0C60],
        b_stores=sorted({a_["fn"] for a_ in boot["acc"] if a_["store"] and 0x8106B5 <= a_["ea"] <= 0x8106B8}),
        sub_writers=sorted({a_["fn"] for a_ in boot["acc"] if a_["store"] and 0x810730 <= a_["ea"] < 0x810748}),
        lock_writers=sorted({a_["fn"] for a_ in boot["acc"] if a_["store"] and 0x810840 <= a_["ea"] < 0x810858}),
        flag_writes=[dict(fn=a_["fn"], index=a_["ea"] - 0x810758, val=a_["val"]) for a_ in boot["acc"]
                     if a_["store"] and 0x810758 <= a_["ea"] < 0x8107D8 and not a_["indexed"]],
        script_op09=[dict(chain=e, record=r["addr"], callback=r["w1"])
                     for e, recs in scripts["boot"].items() for r in recs if r["op"] == 9])
    return out


def _wg_reaches(m: dict, fn: int, targets, depth: int = 3) -> bool:
    """fn (an overlay function) calls one of targets directly or through its own
    module's functions, at most `depth` levels down."""
    todo, seen = [fn], set()
    for _ in range(depth + 1):
        nxt = []
        for f in todo:
            if f in seen:
                continue
            seen.add(f)
            for c in m["calls"]:
                if c["fn"] != f:
                    continue
                if c["target"] in targets:
                    return True
                if m["text"][0] <= c["target"] < m["text"][1]:
                    nxt.append(c["target"])
        todo = nxt
    return False


def _wg_hex(v) -> str:
    return "?" if v is None else (f"{v:#x}" if isinstance(v, int) else str(v))


def world_graph_markdown(g: dict) -> str:
    b = g["boot"]
    L = ["### Boot ELF\n",
         "Area-change requests 001B0C60: " + "; ".join(
             f"{c['fn']:#x} (area {_wg_hex(c['area'])}, s={_wg_hex(c['sub'])}, e={_wg_hex(c['entry'])})"
             for c in b["area_changes"]) + "\n",
         "Direct stores to D_008106B5..B8: " + ", ".join(f"{x:#x}" for x in b["b_stores"]) + "\n",
         "Stores to D_00810730[] (sub-states): " + ", ".join(f"{x:#x}" for x in b["sub_writers"]) + "\n",
         "Stores to the lock bytes D_00810840..57: " + ", ".join(f"{x:#x}" for x in b["lock_writers"]) + "\n",
         "Direct flag stores: " + "; ".join(f"flag {x['index']:#x} = {_wg_hex(x['val'])} ({x['fn']:#x})"
                                            for x in b["flag_writes"]) + "\n",
         "Boot script op09 callbacks: " + "; ".join(f"{x['record']:#x} -> {x['callback']:#x} (chain {x['chain']:#x})"
                                                  for x in b["script_op09"]) + "\n"]
    for name, a in g["areas"].items():
        L.append(f"### {name} (overlay id {a['overlay_id']}, subs {', '.join(map(str, a['subs']))}, "
                 f"lock byte D_{a['lock_byte']:08X}, door table {a['door_table']:#x})\n")
        if a["doors"]:
            L.append("| Subs | Record | Behaviour | Model | Id | Destination | Gate |")
            L.append("|---|---:|---|---:|---:|---|---|")
            for d in a["doors"]:
                L.append(f"| {'+'.join(map(str, d['subs']))} | [{d['index']}] | {d['label']} | {d['model']:#04x} | "
                         f"{d['id']:#04x} | {d['dest']} | {d['gate']} |")
            L.append("")
        if a["changes"]:
            L.append("Other area-change sites: " + "; ".join(
                (f"{c['fn']:#x} (owner {c['owners']}) -> 001B0C60(area {_wg_hex(c['area'])}, s={_wg_hex(c['sub'])}, "
                 f"e={_wg_hex(c['entry'])})") if "direct" not in c else
                (f"{c['fn']:#x} (owner {c['owners']}) stores "
                 + ", ".join(f"{k} = {_wg_hex(v)}" for k, v in sorted(c['direct'].items())))
                for c in a["changes"]) + "\n")
        if a["lock_writers"] or a["code_lock_writes"]:
            parts = [f"{w['where']} {w['rule'].split(':')[0]} model {w['model']:#04x} bit {w['bit']}"
                     for w in a["lock_writers"]]
            parts += [f"{w['fn']:#x} (owner {w['owners']}) stores D_{w['ea']:08X}{' [indexed]' if w['indexed'] else ''}"
                      f"{'' if w['val'] is None else ' = ' + _wg_hex(w['val'])}" for w in a["code_lock_writes"]]
            L.append("Lock-bit writers: " + "; ".join(parts) + "\n")
        fc = a["flags_counters"]
        if fc:
            code = [f"{x['array']} {x['index']:#x} = {_wg_hex(x['val'])} ({x['fn']:#x}, owner {x['owners']})"
                    for x in fc if x["src"] == "code"]
            scr = [f"{x['array']} {x['index']:#x} = {_wg_hex(x['val'])} (script {x['chain']:#x} op{x['op']}"
                   f"{', started by ' + ' '.join(x['started_by']) if x['started_by'] else ''})"
                   for x in fc if x["src"] == "script"]
            if code:
                L.append("Flags / counters written by code: " + "; ".join(code) + "\n")
            if scr:
                L.append("Flags / counters written by scripts: " + "; ".join(scr) + "\n")
        if a["flag_tests"]:
            L.append("Flags tested (001BA1C0): " + ", ".join(f"{x:#x}" for x in a["flag_tests"]) + "\n")
        if a["pickups"]:
            by = {}
            for p in a["pickups"]:
                by.setdefault(f"{p['array']} {p['index']:#x}", []).append(p["where"])
            L.append("Pickups: " + "; ".join(f"{k}: {', '.join(v)}" for k, v in sorted(by.items())) + "\n")
        if a["item_calls"] or a["item_takes"]:
            parts = [f"{x['array']} {_wg_hex(x['index'])} += {_wg_hex(x['n'])} ({x['fn']:#x}, owner {x['owners']})"
                     for x in a["item_calls"]]
            parts += [f"C64 {_wg_hex(x['index'])} -= {_wg_hex(x['n'])} (001C47E0, {x['fn']:#x}, owner {x['owners']})"
                      for x in a["item_takes"]]
            L.append("Items by code: " + "; ".join(parts) + "\n")
        if a["sub_writes"]:
            L.append("Sub-state writes: " + "; ".join(
                (f"D_00810730[{x['area']}] = {_wg_hex(x['val'])} ({x['fn']:#x}, owner {x['owners']})" if "fn" in x else
                 f"op11 in script {x['script']:#x} (started by {' '.join(x['started_by']) or '?'}): "
                 f"sub request {_wg_hex(x['val'])}")
                for x in a["sub_writes"]) + "\n")
    return "\n".join(L)


# Eighth level (route_capture's opt-in groups a06b, a01v, a22b, a04b;
# port docs/EIGHTH_LEVEL_ROUTE.md).  Each group arms the boot functions and
# its own area's overlay; `eighth-delta` measures each group against the
# first level, beat 15, every earlier level's groups and the eighth-level
# groups before it.
EIGHTH_GROUPS = [   # (tag, overlay, overlay id, pass, beats-attr prefix)
    ("a06b", "AREA06", 6, "A06B", "A06B"),
    ("a01v", "AREA01", 2, "A01V", "A01V"),
    ("a22b", "AREA22", 0x13, "A22B", "A22B"),
    ("a04b", "AREA04", 5, "A04B", "A04B"),
]


def _eighth_beats(prefix: str):
    return (getattr(rc, prefix + "_BEATS", []), getattr(rc, prefix + "_SIDE_BEATS", set()),
            getattr(rc, prefix + "_CHANGE_BEATS", set()))


def eighth_delta(a) -> dict:
    earlier = _earlier_groups(a) + [(rc.A01U_BEATS, a.a01u_passes.split(","), "area01_upper", "AREA01"),
                                    (rc.A06_BEATS, ["A06"], "area06", "AREA06")]
    out = {}
    for tag, ov, ov_id, pass_name, prefix in EIGHTH_GROUPS:
        beats, side, change = _eighth_beats(prefix)
        if not beats or not any(_hits(pass_name, b[0]) is not None for b in beats):
            continue
        d = chain_delta(tag, ov, ov_id, beats, side, change, [pass_name], earlier, f"{tag}_delta.json")
        # chain_delta counts a beat as present when its run file exists; mark the
        # runs that did not complete their beat's own checks (their hits cover
        # only the frames before the failure) and the frames each replay ran.
        runs = {}
        for name, _src, _fn in beats:
            f = OUT / "runs" / pass_name / f"{name}.json"
            if f.exists():
                doc = json.loads(f.read_text())
                runs[name] = {"completed": bool(doc.get("completed")), "frames": doc.get("frames"),
                              "error": doc.get("error")}
        d["summary"]["beats_incomplete"] = sorted(n for n, v in runs.items() if not v["completed"])
        d["summary"]["replay_runs"] = runs
        (OUT / f"{tag}_delta.json").write_text(json.dumps(d, indent=1) + "\n")
        out[tag] = d["summary"]
        earlier = earlier + [(beats, [pass_name], "eighth_" + tag, ov)]
    return out


# Ninth level (route_capture's opt-in group a13; port docs/NINTH_LEVEL_ROUTE.md).
# `ninth-delta` measures each group against the first level, beat 15, every
# earlier level's groups, all eighth-level groups and the ninth-level groups
# before it (story order).
NINTH_GROUPS = [   # (tag, overlay, overlay id, pass, beats-attr prefix)
    ("a13", "AREA13", 10, "A13", "A13"),
]


def ninth_delta(a) -> dict:
    earlier = _earlier_groups(a) + [(rc.A01U_BEATS, a.a01u_passes.split(","), "area01_upper", "AREA01"),
                                    (rc.A06_BEATS, ["A06"], "area06", "AREA06")]
    for tag, ov, _ov_id, pass_name, prefix in EIGHTH_GROUPS:
        earlier = earlier + [(_eighth_beats(prefix)[0], [pass_name], "eighth_" + tag, ov)]
    out = {}
    for tag, ov, ov_id, pass_name, prefix in NINTH_GROUPS:
        beats, side, change = _eighth_beats(prefix)
        if not beats or not any(_hits(pass_name, b[0]) is not None for b in beats):
            continue
        d = chain_delta(tag, ov, ov_id, beats, side, change, [pass_name], earlier, f"{tag}_delta.json")
        runs = {}
        for name, _src, _fn in beats:
            f = OUT / "runs" / pass_name / f"{name}.json"
            if f.exists():
                doc = json.loads(f.read_text())
                runs[name] = {"completed": bool(doc.get("completed")), "frames": doc.get("frames"),
                              "error": doc.get("error")}
        d["summary"]["beats_incomplete"] = sorted(n for n, v in runs.items() if not v["completed"])
        d["summary"]["replay_runs"] = runs
        (OUT / f"{tag}_delta.json").write_text(json.dumps(d, indent=1) + "\n")
        out[tag] = d["summary"]
        earlier = earlier + [(beats, [pass_name], "ninth_" + tag, ov)]
    return out


def _run_group(selected, addrs: list[int], pass_name: str) -> None:
    for name, source, fn in selected:
        for attempt in range(3):
            run_beat(name, source, fn, addrs, pass_name)
            doc = json.loads((OUT / "runs" / pass_name / f"{name}.json").read_text())
            if doc.get("completed"):
                break
            keep = OUT / "runs" / pass_name / "_failed"
            keep.mkdir(parents=True, exist_ok=True)
            (keep / f"{name}.attempt{attempt + 1}.json").write_text(json.dumps(doc, indent=1) + "\n")
            print(f"{name}: attempt {attempt + 1} incomplete ({doc.get('error')}); retrying", flush=True)


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("command", choices=["candidates", "run", "report", "compare-startup", "exit-delta",
                                        "a01-delta", "a00-delta", "a02-delta", "a04-delta", "a22-delta",
                                        "a01u-delta", "a06-delta", "graph", "eighth-delta",
                                        "ninth-delta"])
    ap.add_argument("--arm-chunk", type=int, default=200,
                    help="breakpoint commands per DebugServer round trip")
    ap.add_argument("--segments", default="all")
    ap.add_argument("--pass", dest="pass_name", default="A")
    ap.add_argument("--passes", default="A")
    ap.add_argument("--a01-passes", default="A01", help="a00-delta: the AREA01 census passes")
    ap.add_argument("--a01r-passes", default="A01R", help="a02-delta: the AREA01 revisit census passes")
    ap.add_argument("--a00-passes", default="A00", help="a02-delta: the AREA00 census passes")
    ap.add_argument("--a02-passes", default="A02", help="a04-delta: the AREA02 census passes")
    ap.add_argument("--a04-passes", default="A04", help="a22-delta: the AREA04 census passes")
    ap.add_argument("--a22-passes", default="A22", help="a01u-delta: the AREA22 census passes")
    ap.add_argument("--a01u-passes", default="A01U", help="a06-delta: the AREA01 upper-floor census passes")
    a = ap.parse_args()
    ARM_CHUNK = max(1, a.arm_chunk)
    if a.command == "candidates":
        c = candidates()
        (OUT).mkdir(parents=True, exist_ok=True)
        (OUT / "candidates.json").write_text(json.dumps(c, indent=1) + "\n")
        counts: dict[str, int] = {}
        for x in c:
            counts[x["region"] + ":" + x["status"]] = counts.get(x["region"] + ":" + x["status"], 0) + 1
        print(len(c), json.dumps(counts, indent=1))
    elif a.command == "run":
        if a.segments != "all" and any(w.startswith("a01") for w in a.segments.split(",")):
            a01_addrs = [c["addr"] for c in candidates("AREA01")]
            for name, source, fn in rc.a01_selected(a.segments):
                for attempt in range(3):
                    run_beat(name, source, fn, a01_addrs, a.pass_name)
                    doc = json.loads((OUT / "runs" / a.pass_name / f"{name}.json").read_text())
                    if doc.get("completed"):
                        break
                    keep = OUT / "runs" / a.pass_name / "_failed"
                    keep.mkdir(parents=True, exist_ok=True)
                    (keep / f"{name}.attempt{attempt + 1}.json").write_text(json.dumps(doc, indent=1) + "\n")
                    print(f"{name}: attempt {attempt + 1} incomplete ({doc.get('error')}); retrying", flush=True)
        if a.segments != "all" and any(w.startswith("a00") for w in a.segments.split(",")):
            a00_addrs = [c["addr"] for c in candidates("AREA00")]
            for name, source, fn in rc.a00_selected(a.segments):
                for attempt in range(3):
                    run_beat(name, source, fn, a00_addrs, a.pass_name)
                    doc = json.loads((OUT / "runs" / a.pass_name / f"{name}.json").read_text())
                    if doc.get("completed"):
                        break
                    keep = OUT / "runs" / a.pass_name / "_failed"
                    keep.mkdir(parents=True, exist_ok=True)
                    (keep / f"{name}.attempt{attempt + 1}.json").write_text(json.dumps(doc, indent=1) + "\n")
                    print(f"{name}: attempt {attempt + 1} incomplete ({doc.get('error')}); retrying", flush=True)
        if a.segments != "all" and any(w.startswith("a01r") for w in a.segments.split(",")):
            _run_group(rc.a01r_selected(a.segments), [c["addr"] for c in candidates("AREA01")], a.pass_name)
        if a.segments != "all" and any(w.startswith("a02") for w in a.segments.split(",")):
            _run_group(rc.a02_selected(a.segments), [c["addr"] for c in candidates("AREA02")], a.pass_name)
        if a.segments != "all" and any(w.startswith("a04") for w in a.segments.split(",")):
            _run_group(rc.a04_selected(a.segments), [c["addr"] for c in candidates("AREA04")], a.pass_name)
        if a.segments != "all" and any(w.startswith("a22") for w in a.segments.split(",")):
            _run_group(rc.a22_selected(a.segments), [c["addr"] for c in candidates("AREA22")], a.pass_name)
        if a.segments != "all" and any(w.startswith("a01u") for w in a.segments.split(",")):
            _run_group(rc.a01u_selected(a.segments), [c["addr"] for c in candidates("AREA01")], a.pass_name)
        if a.segments != "all" and any(w.startswith("a06") for w in a.segments.split(",")):
            _run_group(rc.a06_selected(a.segments), [c["addr"] for c in candidates("AREA06")], a.pass_name)
        if a.segments != "all":
            for tag, ov, _ov_id, _pass, _prefix in EIGHTH_GROUPS:
                sel = [b for b in rc.eighth_selected(a.segments) if b[0].startswith(tag + "_")]
                if sel:
                    _run_group(sel, [c["addr"] for c in candidates(ov)], a.pass_name)
            for tag, ov, _ov_id, _pass, _prefix in NINTH_GROUPS:
                sel = [b for b in rc.a13_selected(a.segments) if b[0].startswith(tag + "_")]
                if sel:
                    _run_group(sel, [c["addr"] for c in candidates(ov)], a.pass_name)
        addrs = [c["addr"] for c in candidates()]
        wanted = DEFAULT_SEGMENTS if a.segments == "all" else a.segments.split(",")
        for seg in SEGMENTS:
            if seg not in wanted and seg[:2] not in wanted:
                continue
            if seg == "startup":
                for attempt in range(3):
                    try:
                        run_startup(addrs, a.pass_name)
                        break
                    except (TimeoutError, OSError, EOFError, RuntimeError) as exc:
                        print(f"startup: attempt {attempt + 1} failed ({exc!r}); retrying", flush=True)
                else:
                    raise RuntimeError("startup failed three times")
            else:
                name, source, fn = next(b for b in rc.BEATS if b[0] == seg)
                for attempt in range(3):
                    run_beat(name, source, fn, addrs, a.pass_name)
                    doc = json.loads((OUT / "runs" / a.pass_name / f"{name}.json").read_text())
                    if doc.get("completed"):
                        break
                    keep = OUT / "runs" / a.pass_name / "_failed"
                    keep.mkdir(parents=True, exist_ok=True)
                    (keep / f"{name}.attempt{attempt + 1}.json").write_text(json.dumps(doc, indent=1) + "\n")
                    print(f"{name}: attempt {attempt + 1} incomplete ({doc.get('error')}); retrying", flush=True)
    elif a.command == "report":
        report(a.passes.split(","))
    elif a.command == "a01-delta":
        d = a01_delta(a.passes.split(","))
        print(json.dumps(d["summary"], indent=1))
        print(json.dumps(d["per_beat"], indent=1))
    elif a.command == "a00-delta":
        d = a00_delta(a.passes.split(","), a.a01_passes.split(","))
        print(json.dumps(d["summary"], indent=1))
        print(json.dumps(d["per_segment"], indent=1))
    elif a.command == "a02-delta":
        d = a02_delta(a.passes.split(","), a.a01r_passes.split(","), a.a00_passes.split(","),
                      a.a01_passes.split(","))
        print(json.dumps(d["summary"], indent=1))
        print(json.dumps(d["per_beat"], indent=1))
    elif a.command == "a04-delta":
        d = a04_delta(a.passes.split(","), a.a02_passes.split(","), a.a01r_passes.split(","),
                      a.a00_passes.split(","), a.a01_passes.split(","))
        print(json.dumps(d["summary"], indent=1))
        print(json.dumps(d["per_beat"], indent=1))
    elif a.command == "a22-delta":
        d = a22_delta(a.passes.split(","), a.a04_passes.split(","), a.a02_passes.split(","),
                      a.a01r_passes.split(","), a.a00_passes.split(","), a.a01_passes.split(","))
        print(json.dumps(d["summary"], indent=1))
        print(json.dumps(d["per_beat"], indent=1))
    elif a.command == "a01u-delta":
        d = chain_delta("a01u", "AREA01", AREA01_ID, rc.A01U_BEATS, rc.A01U_SIDE_BEATS, rc.A01U_CHANGE_BEATS,
                        a.passes.split(","), _earlier_groups(a), "a01u_delta.json")
        print(json.dumps(d["summary"], indent=1))
        print(json.dumps(d["per_beat"], indent=1))
    elif a.command == "a06-delta":
        d = chain_delta("a06", "AREA06", AREA06_ID, rc.A06_BEATS, rc.A06_SIDE_BEATS, rc.A06_CHANGE_BEATS,
                        a.passes.split(","),
                        _earlier_groups(a) + [(rc.A01U_BEATS, a.a01u_passes.split(","), "area01_upper", "AREA01")],
                        "a06_delta.json")
        print(json.dumps(d["summary"], indent=1))
        print(json.dumps(d["per_beat"], indent=1))
    elif a.command == "eighth-delta":
        print(json.dumps(eighth_delta(a), indent=1))
    elif a.command == "ninth-delta":
        print(json.dumps(ninth_delta(a), indent=1))
    elif a.command == "graph":
        g = world_graph()
        OUT.mkdir(parents=True, exist_ok=True)
        (OUT / "world_graph.json").write_text(json.dumps(g, indent=1, default=list) + "\n")
        (OUT / "world_graph_tables.md").write_text(world_graph_markdown(g) + "\n")
        print(f"wrote {OUT / 'world_graph.json'} and {OUT / 'world_graph_tables.md'}: "
              f"{len(g['areas'])} areas, {sum(len(x['doors']) for x in g['areas'].values())} door records")
    elif a.command == "exit-delta":
        d = exit_delta(a.passes.split(","))
        print(json.dumps({k: v for k, v in d.items() if k != "new_functions"}, indent=1))
    elif a.command == "compare-startup":
        ps = a.passes.split(",")
        print(json.dumps(compare_startup(ps[0], ps[1]), indent=1))
