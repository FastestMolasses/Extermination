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
    .venv/bin/python tools/route_census.py run --segments all [--pass A]
    .venv/bin/python tools/route_census.py run --segments startup --pass B [--arm-chunk 100]
    .venv/bin/python tools/route_census.py compare-startup --passes A,B
    .venv/bin/python tools/route_census.py report --passes A,B   # first pass is primary
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


def candidates() -> list[dict]:
    status = boot_status()
    cands = []
    for row in csv.DictReader(open(ROOT / "docs/FUNCTIONS.csv")):
        name = row["name"]
        cands.append({"addr": int(row["vram"], 16), "name": name, "size": int(row["size_bytes"]),
                      "region": "boot", **status[name]})
    code = ROOT / "build/overlays/AREA11/asm/matchings/AREA11/code"
    blob = (ROOT / "extract/OVERLAY/AREA11.BIN").read_bytes()
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
        src = ROOT / "src/overlays/AREA11" / f"{name}.c"
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
        cands.append({"addr": addr, "name": name, "size": size, "region": "overlay:AREA11",
                      "splat_label": hex(addr - 0x40), "status": st, "link_route": None,
                      "objdiff_perfect": None, "csv_status": None, "subsystem": "overlay_AREA11"})
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
    rec_path = ROUTE / name / "trace.json"
    recorded = json.loads(rec_path.read_text())
    tail = recorded.get("tail_idle_frames", 0) or 0
    if len(source) == 2 and source.isdigit():
        src = rc.slot_path(source)
        src_ee = None
    else:
        src = resumable(ROUTE / source / "state.p2s", name)
        src_ee = ROUTE / source / "eeMemory.bin"
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
        doc["recorded_end_digest"] = digests_file((ROUTE / name / "eeMemory.bin").read_bytes())
        doc["end_digest_equal"] = {k: doc["end_digest"][k] == doc["recorded_end_digest"][k]
                                   for k in doc["end_digest"]}
        try:   # where the stable spans differ from the recorded snapshot (addresses only)
            ref = (ROUTE / name / "eeMemory.bin").read_bytes()
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


SEGMENTS = ["startup"] + [b[0] for b in rc.BEATS]
ORDER = ["S0_title", "S1_newgame_load", "S2_opening", "S3_first_control_idle"] + [b[0] for b in rc.BEATS]
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
               "labels_per_pass": {p: sorted(runs[p]) for p in passes},
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


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("command", choices=["candidates", "run", "report", "compare-startup"])
    ap.add_argument("--arm-chunk", type=int, default=200,
                    help="breakpoint commands per DebugServer round trip")
    ap.add_argument("--segments", default="all")
    ap.add_argument("--pass", dest="pass_name", default="A")
    ap.add_argument("--passes", default="A")
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
        addrs = [c["addr"] for c in candidates()]
        wanted = SEGMENTS if a.segments == "all" else a.segments.split(",")
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
    elif a.command == "compare-startup":
        ps = a.passes.split(",")
        print(json.dumps(compare_startup(ps[0], ps[1]), indent=1))
