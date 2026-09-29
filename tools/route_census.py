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
The a01r_* segments arm the boot functions plus the AREA01 overlay, the a02_*
segments the boot functions plus the AREA02 overlay (docs/FOURTH_LEVEL_ROUTE.md),
the a04_* segments the boot functions plus the AREA04 overlay
(docs/FIFTH_LEVEL_ROUTE.md), the a22_* segments the boot functions plus the
AREA22 overlay pieces (docs/SIXTH_LEVEL_ROUTE.md).
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
                                        "a01-delta", "a00-delta", "a02-delta", "a04-delta", "a22-delta"])
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
    elif a.command == "exit-delta":
        d = exit_delta(a.passes.split(","))
        print(json.dumps({k: v for k, v in d.items() if k != "new_functions"}, indent=1))
    elif a.command == "compare-startup":
        ps = a.passes.split(",")
        print(json.dumps(compare_startup(ps[0], ps[1]), indent=1))
