#!/usr/bin/env python3
"""route_lanes.py - re-record route_capture groups on the PCSX2 fork in parallel lanes.

The fork's guide (pcsx2-fork/EXTERMINATION.md, golden rule 5) allows one run
lock per session and several instances inside it.  This tool is that session:
it takes build/.pcsx2.lock once, starts up to --lanes worker processes, and
each worker runs one route_capture beat (route_capture.run_beat: the same
driving, closed loop where the beat is, phase-locked, fork-saved snapshot
checked to resume) on its own DebugServer port and PINE slot
(pcsx2_session.FORK_LANE).  Beats start as soon as their source beat exists,
critical path first.  Every --yield-every seconds the scheduler stops
dispatching, lets the running beats finish, releases the lock for a few
seconds so other runs waiting on it (they poll every 2 s) get their turn, and
takes it again.

Output is route_capture's own (fork, phase generation): build/fork_refs/<the
v2.6.3 path>/<beat>/ with trace.json and the snapshot, a manifest.json per
set, the state hard-linked into fork-states/phase/beats/; at the end the
fork-states manifest is rebuilt once (fork_states.cmd_manifest).

`compare` sets every re-recorded beat against its v2.6.3 trace, aligned by
game state (difflib over per-row fingerprints without the counters), and
writes build/fork_refs/compare/<group>.json plus a summary.

Usage (macOS arm64 host; the fork runs x86_64 under Rosetta; decomp .venv):
    .venv/bin/python tools/route_lanes.py list
    .venv/bin/python tools/route_lanes.py run --groups 15,c7,aim,exit,dmg,br,opt --lanes 4
    .venv/bin/python tools/route_lanes.py compare --groups aim,dmg

Everything it writes comes from the user's own disc and stays in ignored
build/ folders.  No original code or data is in this file.
"""
from __future__ import annotations

import argparse
import difflib
import hashlib
import json
import os
import signal
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

import route_capture as rc  # noqa: E402

LOCK = ROOT / "build/.pcsx2.lock"
LOGS = rc.FORK_REFS / "logs/lanes"
COMPARE = rc.FORK_REFS / "compare"
PY = sys.executable

# Rows keys that count time rather than game state: the main-loop counter, the
# row index, the phase (frame index, field, game vsync) and the game's vsync.
TIME_KEYS = {"counter", "f", "fi", "fld", "vs", "vsync"}

LEVEL_ORDER = ["a01", "a00", "a01r", "a02", "a04", "a22", "a01u", "a06", "a06b", "a01v", "a22b", "a04b",
               "a13", "a19", "a13b", "a13c", "a13d", "a19b", "a19c", "a19d", "a15", "a15b", "a19e", "a03"]
FIRST_LEVEL_GROUPS = ["main", "c7", "aim", "exit", "dmg", "br", "opt"]


def group_of(name: str) -> str:
    head = name.split("_")[0]
    return "main" if head.isdigit() else head


def all_beats() -> dict[str, tuple[str, object, str]]:
    """name -> (source, beat function, group), every route_capture beat."""
    out: dict[str, tuple[str, object, str]] = {}
    for g in (rc.BEATS, rc.A01_BEATS, rc.C7_BEATS, rc.A00_BEATS, rc.A01R_BEATS, rc.A02_BEATS, rc.A04_BEATS,
              rc.A22_BEATS, rc.A01U_BEATS, rc.A06_BEATS):
        for name, source, fn in g:
            out[name] = (source, fn, group_of(name))
    for sel in (rc.eighth_selected, rc.a13_selected, rc.tenth_selected, rc.eleventh_selected,
                rc.twelfth_selected, rc.thirteenth_selected, rc.fourteenth_selected,
                rc.fourteenth_a15_selected, rc.fifteenth_selected, rc.aim_selected, rc.exit_selected,
                rc.dmg_selected, rc.br_selected, rc.opt_selected):
        for prefix in LEVEL_ORDER[9:] + ["aim", "exit", "dmg", "br", "opt"]:
            for name, source, fn in sel(prefix):
                out.setdefault(name, (source, fn, group_of(name)))
    return out


def legacy_frames(name: str) -> int:
    ref = rc._legacy_cache().get("beats", {}).get(name)
    return int(ref["frames"]) if ref else 1000


def select(spec: str, beats: dict) -> list[str]:
    """Comma list of groups (main, c7, aim, a01, ...), beat names, or `15`
    (route beat 15).  `levels` = every level group in story order."""
    want = []
    for w in spec.split(","):
        if w == "levels":
            want += LEVEL_ORDER
        elif w:
            want.append(w)
    names = []
    for w in want:
        for name, (_s, _f, g) in beats.items():
            if (g == w or name == w or (w.isdigit() and name.startswith(w + "_"))) and name not in names:
                names.append(name)
    return names


def done(name: str) -> bool:
    d = rc.beat_dir(name)
    return (d / "trace.json").exists() and (d / "state.p2s").exists()


# ---------------------------------------------------------------------------
# run lock

def take_lock(log) -> None:
    t0 = time.monotonic()
    while True:
        try:
            LOCK.mkdir()
            break
        except FileExistsError:
            if time.monotonic() - t0 > 6 * 3600:
                raise RuntimeError(f"run lock {LOCK} still held after 6 h")
            time.sleep(2)
    # another run that just released the lock may still be shutting its emulator down
    while subprocess.run(["pgrep", "-f", "^[^ ]*PCSX2.app/Contents/MacOS/PCSX2"],
                         capture_output=True).returncode == 0:
        time.sleep(2)
    log(f"run lock taken after {time.monotonic() - t0:.0f} s")


def release_lock(log) -> None:
    try:
        LOCK.rmdir()
        log("run lock released")
    except OSError as exc:
        log(f"run lock release failed: {exc}")


# ---------------------------------------------------------------------------
# run

def out_done(name: str, out_root: Path | None) -> bool:
    """The beat's output exists: in the official chain, or under --out-root."""
    if out_root is None:
        return done(name)
    d = out_root / rc._legacy_beat_dir(name).relative_to(ROOT / "build")
    return (d / "trace.json").exists() and (d / "state.p2s").exists()


def cmd_run(a) -> None:
    rc.use_fork("phase", True)
    beats = all_beats()
    names = select(a.groups, beats)
    out_root = Path(a.out_root).resolve() if a.out_root else None
    if out_root is not None:
        a.redo = True
    if a.redo:
        todo = list(names)
    else:
        todo = [n for n in names if not done(n)]
    # A repeat into --out-root keeps its logs there: the official chain's lane
    # logs (build/fork_refs/logs/lanes/<beat>.log) are never overwritten by it.
    # Every beat log is opened for append, with a header per run.
    logs = (out_root / "logs/lanes") if out_root is not None else LOGS
    logs.mkdir(parents=True, exist_ok=True)
    runlog = open(logs / "scheduler.log", "a")

    def log(msg: str) -> None:
        line = time.strftime("%H:%M:%S ") + msg
        print(line, flush=True)
        runlog.write(line + "\n")
        runlog.flush()

    # critical path: frames of the beat plus its longest chain of selected dependents
    children: dict[str, list[str]] = {}
    for n in todo:
        children.setdefault(beats[n][0], []).append(n)
    memo: dict[str, int] = {}

    def weight(n: str) -> int:
        if n not in memo:
            memo[n] = legacy_frames(n) + max([weight(c) for c in children.get(n, [])] or [0])
        return memo[n]

    pending = sorted(todo, key=lambda n: -weight(n))
    log(f"{len(todo)} beats to record of {len(names)} selected; lanes {a.lanes}; "
        f"critical path about {max([weight(n) for n in todo] or [0])} v2.6.3 frames")
    if a.dry_run:
        for n in pending:
            print(f"  {n:34s} <- {beats[n][0]:30s} weight {weight(n)}")
        return
    finished: set[str] = set()
    failed: dict[str, str] = {}
    running: dict[int, tuple[str, subprocess.Popen, float]] = {}
    times: dict[str, float] = {}
    stop = {"flag": False}

    def on_term(signum, frame):
        stop["flag"] = True
    signal.signal(signal.SIGTERM, on_term)
    signal.signal(signal.SIGINT, on_term)

    def ready(n: str) -> bool:
        src = beats[n][0]
        if src.isdigit():
            return True
        if src in failed:
            return False
        if src in todo:
            return src in finished
        return done(src)

    take_lock(log)
    lock_since = time.monotonic()
    draining = False
    try:
        while (pending or running) and not stop["flag"]:
            for lane, (n, p, t0) in list(running.items()):
                if p.poll() is None:
                    continue
                del running[lane]
                times[n] = time.monotonic() - t0
                if p.returncode == 0 and out_done(n, out_root):
                    finished.add(n)
                    log(f"lane {lane}: {n} done in {times[n]:.0f} s")
                else:
                    failed[n] = f"exit {p.returncode}"
                    log(f"lane {lane}: {n} FAILED (exit {p.returncode}); log {logs / (n + '.log')}")
            # dependents of failed beats cannot run
            for n in list(pending):
                if beats[n][0] in failed:
                    pending.remove(n)
                    failed[n] = f"source {beats[n][0]} failed"
                    log(f"{n} skipped: {failed[n]}")
            if not draining and a.yield_every and time.monotonic() - lock_since > a.yield_every:
                draining = True
                log("yielding the run lock once the running beats finish")
            if draining:
                if not running:
                    release_lock(log)
                    time.sleep(a.yield_pause)
                    take_lock(log)
                    lock_since = time.monotonic()
                    draining = False
                else:
                    time.sleep(1)
                    continue
            free = [k for k in range(1, a.lanes + 1) if k not in running]
            for lane in free:
                nxt = next((n for n in pending if ready(n)), None)
                if nxt is None:
                    break
                pending.remove(nxt)
                env = dict(os.environ, EXTERMINATION_FORK_LANE=str(lane),
                           EXTERMINATION_FORK_LOCK_HELD=str(LOCK), EXTERMINATION_PCSX2="fork")
                out = open(logs / (nxt + ".log"), "a")
                cmd = [PY, str(Path(__file__).resolve()), "worker", nxt, "--fork-mtvu", a.fork_mtvu,
                       "--attempts", str(a.attempts)]
                if out_root is not None:
                    cmd += ["--out-root", str(out_root)]
                out.write(f"=== {time.strftime('%Y-%m-%d %H:%M:%S')} lane {lane}: {' '.join(cmd[1:])}\n")
                out.flush()
                p = subprocess.Popen(cmd, env=env, stdout=out, stderr=subprocess.STDOUT, cwd=str(ROOT))
                running[lane] = (nxt, p, time.monotonic())
                log(f"lane {lane}: {nxt} <- {beats[nxt][0]}")
            if pending and not running:
                for n in pending:
                    failed[n] = f"source {beats[n][0]} never recorded"
                log(f"stuck: {pending}")
                break
            time.sleep(1)
    finally:
        for lane, (n, p, _t0) in running.items():
            if p.poll() is None:
                p.send_signal(signal.SIGTERM)           # the worker closes its sessions
        for lane, (n, p, _t0) in running.items():
            try:
                p.wait(120)
            except subprocess.TimeoutExpired:
                p.kill()
                p.wait()
            failed.setdefault(n, "interrupted")
        release_lock(log)
    if finished and out_root is None:
        import fork_states
        fork_states.cmd_manifest(None)
    summary = {"finished": sorted(finished), "failed": failed, "seconds": times,
               "out_root": str(out_root) if out_root is not None else None,
               "recorded": time.strftime("%Y-%m-%d %H:%M:%S")}
    path = logs / f"run_{time.strftime('%Y%m%d_%H%M%S')}.json"
    path.write_text(json.dumps(summary, indent=1) + "\n")
    log(f"finished {len(finished)}, failed {len(failed)} -> {path}")


def _arm_hash_capture(dest: Path) -> None:
    """Diagnostics: a hash-only frame store (no contents, no state keyframes, so
    no observer effect; fork DETERMINISM.md) from the beat's row 0, copied to
    dest/<n> when the session closes, for pxfs.diverge between repeat runs."""
    import shutil
    from pcsx2_session import ForkSession
    begin, close = rc.Route.begin, ForkSession.close
    count = {"n": 0}

    def begin_cap(self, *a, **k):
        if not getattr(self.s, "_hash_cap", False):
            self.s._client.call("capture_start", dir="hashcap", regions="all", state_every=0,
                                keyframe_every=600, contents=False)
            self.s._hash_cap = True
        return begin(self, *a, **k)

    def close_cap(self):
        if getattr(self, "_hash_cap", False) and self._client is not None:
            try:
                self._client.call("capture_stop")
                count["n"] += 1
                d = dest / str(count["n"])
                if d.exists():
                    shutil.rmtree(d)
                shutil.copytree(self._inst.root / "hashcap", d)
            except Exception as exc:  # noqa: BLE001
                print("hash capture copy failed:", exc, flush=True)
            self._hash_cap = False
        return close(self)
    rc.Route.begin, ForkSession.close = begin_cap, close_cap


def _arm_rand_log(dest: Path) -> None:
    """Diagnostics: every call of the game's rand() (0x00122BB8) from the load on,
    as a fork call trace (an inline log probe: no observer effect; FORMATS.md
    section 4: tick, RA, ...), plus the tick of the beat's row 0; copied to
    dest/ when the beat's session closes."""
    import shutil
    from pcsx2_session import ForkSession
    start, close, begin = ForkSession._start, ForkSession.close, rc.Route.begin
    state = {"n": 0}

    def start_log(self):
        out = start(self)
        if state["n"] == 0:
            state["n"] = 1
            self._rand_log = True
            r = self._client.call("probe_add", pc=0x00122BB8, action="log", path="randlog.bin")
            dest.mkdir(parents=True, exist_ok=True)
            (dest / "arm.json").write_text(json.dumps({"probe": r, "state": self._client.call("state")}) + "\n")
        return out

    def begin_mark(self, *a, **k):
        if getattr(self.s, "_rand_log", False):
            (dest / "row0.json").write_text(json.dumps(self.s._client.call("state")) + "\n")
        return begin(self, *a, **k)

    def close_log(self):
        if getattr(self, "_rand_log", False) and self._client is not None:
            try:
                self._client.call("probe_remove", all=True)
                shutil.copyfile(self._inst.root / "randlog.bin", dest / "randlog.bin")
            except Exception as exc:  # noqa: BLE001
                print("rand log copy failed:", exc, flush=True)
            self._rand_log = False
        return close(self)
    ForkSession._start, ForkSession.close, rc.Route.begin = start_log, close_log, begin_mark


def cmd_worker(a) -> None:
    """One beat in this lane (started by cmd_run with the lane's environment)."""
    rc.use_fork("phase", True)
    import pcsx2_session
    pcsx2_session.FORK_MTVU = None if a.fork_mtvu == "ini" else (a.fork_mtvu == "on")
    signal.signal(signal.SIGTERM, rc._term_to_interrupt)
    beats = all_beats()
    source, fn, _g = beats[a.beat]
    if a.out_root:
        # a repeat into another root (determinism checks): sources still come from
        # the real build/fork_refs chain, outputs and state links go under out_root
        real = rc.FORK_REFS
        rc.beat_source = lambda src: (rc.slot_path(src) if len(src) == 2 and src.isdigit() else
                                      real / rc._legacy_beat_dir(src).relative_to(ROOT / "build") / "state.p2s")
        root = Path(a.out_root).resolve()
        rc.FORK_REFS, rc.FORK_PHASE_BEATS = root, root / "_state_links"
    if a.hash_capture:
        _arm_hash_capture(Path(a.hash_capture))
    if a.rand_log:
        _arm_rand_log(Path(a.rand_log))
    t0 = time.monotonic()
    if a.attempts <= 1:
        rc.run_beat(a.beat, source, fn)
    else:
        run_with_attempts(a.beat, source, fn, a.attempts)
    print(f"{a.beat}: {time.monotonic() - t0:.0f} s, lane {pcsx2_session.FORK_LANE}", flush=True)


# Retries.  The fork chain is not perfectly repeatable: of ten closed-loop
# runs of route beat 10 from one state, eight were identical in every row and
# machine region and two left the path (one frame of the player's turn lost at
# row 168; a different action at row 724) with the same inputs up to there.
# So a beat whose traced fields leave the v2.6.3 path is run again: identical
# reruns mean the difference is the fork's own (kept); otherwise the attempt
# closest to the v2.6.3 trace is kept.  Every attempt is listed in the manifest.

def _core(row: dict) -> str:
    return json.dumps([row.get(k) for k in CORE_KEYS], separators=(",", ":"))


CORE_KEYS = ("pos", "yaw", "m1F0", "clip", "spad", "ui", "req", "power", "charge", "battery_item", "area",
             "fade", "story790", "m1F1", "p5", "hp")


def core_score(name: str, folder: Path) -> dict:
    """The traced fields against the v2.6.3 trace: same-index equality and the
    rows matched when aligned by game state (holds = repeated rows, loads)."""
    o = rc._legacy_beat_dir(name) / "trace.json"
    b = json.loads((folder / "trace.json").read_text())["rows"]
    if not o.exists():
        return {"v263": False, "ok": True}
    a = json.loads(o.read_text())["rows"]
    ca, cb = [_core(r) for r in a], [_core(r) for r in b]
    same = sum(1 for x, y in zip(ca, cb) if x == y)
    sm = difflib.SequenceMatcher(None, ca, cb, autojunk=False)
    matched, nonhold = 0, 0
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            matched += i2 - i1
            continue
        if tag == "insert" and j1 > 0 and all(cb[j] == cb[j1 - 1] for j in range(j1, j2)):
            continue
        if tag == "delete" and i1 > 0 and all(ca[i] == ca[i1 - 1] for i in range(i1, i2)):
            continue
        nonhold += 1
    return {"v263": True, "same_index_equal": same, "rows": [len(a), len(b)], "aligned_matched": matched,
            "non_hold_blocks": nonhold, "ok": nonhold == 0}


def _trace_digest(folder: Path) -> str:
    rows = json.loads((folder / "trace.json").read_text())["rows"]
    return hashlib.sha256(json.dumps(rows, sort_keys=True).encode()).hexdigest()


def run_with_attempts(name: str, source: str, fn, attempts: int) -> None:
    import shutil
    import types
    out = rc.beat_dir(name)
    keep = out.parent / "_attempts" / name
    if keep.exists():
        shutil.rmtree(keep)                     # absolute path under build/fork_refs
    tried = []
    errors = []
    for k in range(1, attempts + 1):
        try:
            rc.run_beat(name, source, fn)
        except (RuntimeError, TimeoutError, AssertionError, ValueError, OSError) as exc:
            import traceback
            errors.append(f"attempt {k}: {exc!r}")
            print(f"{name} attempt {k} failed: {exc!r}\n{traceback.format_exc()}", flush=True)
            if k == attempts and not tried:
                raise
            continue
        sc = core_score(name, out)
        sc["attempt"], sc["digest"] = k, _trace_digest(out)
        tried.append(sc)
        print(f"{name} attempt {k}: {json.dumps({x: sc.get(x) for x in ('same_index_equal', 'rows', 'aligned_matched', 'non_hold_blocks', 'ok')})}",
              flush=True)
        if sc["ok"] or k == attempts or any(t["digest"] == sc["digest"] for t in tried[:-1]):
            break
        dst = keep / str(k)
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(out, dst, copy_function=shutil.copy2)
    if not tried:
        raise RuntimeError(f"{name}: every attempt failed: {errors}")
    best = max(tried, key=lambda t: (t.get("ok", False), t.get("same_index_equal", 0), t.get("aligned_matched", 0)))
    if best is not tried[-1]:
        src = keep / str(best["attempt"])
        for f in ("trace.json", "state.p2s", "snapshot.json", "eeMemory.bin", "gs.bin", "scratchpad.bin",
                  "original.png"):
            if (src / f).exists():
                (out / f).unlink(missing_ok=True)
                shutil.copy2(src / f, out / f)
        doc = json.loads((out / "trace.json").read_text())
        snap = doc.get("snapshot") or {}
        sess = types.SimpleNamespace(hello={"rev": snap.get("fork_rev"), "hash": snap.get("fork_hash")})
        rc.record_phase_beat(name, source, out, doc, doc["rows"], sess)
    mpath = out.parent / "manifest.json"
    m = json.loads(mpath.read_text())
    m["beats"][name]["attempts"] = [{x: t.get(x) for x in ("attempt", "same_index_equal", "rows", "aligned_matched",
                                                           "non_hold_blocks", "ok", "digest")} for t in tried]
    m["beats"][name]["kept_attempt"] = best["attempt"]
    if errors:
        m["beats"][name]["failed_attempts"] = errors
    rc._write_json_atomic(mpath, m)


# ---------------------------------------------------------------------------
# compare with the v2.6.3 traces

# Counters inside sampled blobs (byte ranges): the game's vsync counter
# D_00810E90 in the DAMAGE/BRANCH/OPTIONS pad block (0x810E40 + 0x50) and the
# main-loop counter 0x70003B64 in their scratchpad slice (0x70003B40 + 0x24).
BLOB_TIME = {"padblk": [(0x50, 0x54)], "spad_hi": [(0x24, 0x28)]}


def _untimed(row: dict) -> dict:
    out = dict(row)
    for k, spans in BLOB_TIME.items():
        v = out.get(k)
        if isinstance(v, str):
            b = bytearray(bytes.fromhex(v))
            for lo, hi in spans:
                b[lo:hi] = bytes(min(hi, len(b)) - lo) if lo < len(b) else b""
            out[k] = b.hex()
    return out


def _fp(row: dict) -> str:
    row = _untimed(row)
    body = {k: v for k, v in row.items() if k not in TIME_KEYS}
    return hashlib.sha1(json.dumps(body, sort_keys=True, separators=(",", ":")).encode()).hexdigest()


def _diff_keys(a: dict, b: dict) -> list[str]:
    a, b = _untimed(a), _untimed(b)
    keys = (set(a) | set(b)) - TIME_KEYS
    return sorted(k for k in keys if a.get(k) != b.get(k))


def _maxdev(a, b) -> float | None:
    """Largest numeric difference between two values of the same shape."""
    if isinstance(a, (int, float)) and isinstance(b, (int, float)):
        return abs(a - b)
    if isinstance(a, list) and isinstance(b, list) and len(a) == len(b):
        ds = [_maxdev(x, y) for x, y in zip(a, b)]
        return None if any(d is None for d in ds) else max(ds or [0.0])
    if isinstance(a, dict) and isinstance(b, dict) and set(a) == set(b):
        ds = [_maxdev(a[k], b[k]) for k in a]
        return None if any(d is None for d in ds) else max(ds or [0.0])
    return None if a != b else 0.0


def _fields(a, b) -> list[str]:
    """The sub-fields of two owner-node dicts that differ."""
    if isinstance(a, dict) and isinstance(b, dict):
        return sorted(k for k in set(a) | set(b) if a.get(k) != b.get(k))
    return []


# The game's rand() (0x00122BB8, docs/CAPTURES_C7.md section 3): an LCG whose
# state word sits at 0x00242670 + 0x58.  The step count between two states says
# how many more (or fewer) rand calls one run made.
RAND_STATE = 0x002426C8
_A, _C = 1103515245, 12345
_AINV = pow(_A, -1, 1 << 32)


def rand_calls_between(s_from: int, s_to: int, limit: int = 2_000_000) -> int | None:
    fwd = bwd = s_from
    if s_from == s_to:
        return 0
    for n in range(1, limit):
        fwd = (fwd * _A + _C) & 0xFFFFFFFF
        if fwd == s_to:
            return n
        bwd = ((bwd - _C) * _AINV) & 0xFFFFFFFF
        if bwd == s_to:
            return -n
    return None


def _rand_state(folder: Path) -> int | None:
    import struct
    ee = folder / "eeMemory.bin"
    if ee.exists():
        with open(ee, "rb") as f:
            f.seek(RAND_STATE)
            return struct.unpack("<I", f.read(4))[0]
    st = folder / "state.p2s"
    if not st.exists():
        return None
    from parse_pcsx2_state import extract_zstd_entry
    return struct.unpack_from("<I", extract_zstd_entry(st, "eeMemory.bin"), RAND_STATE)[0]


def compare_beat(name: str) -> dict:
    f, o = rc.beat_dir(name) / "trace.json", rc._legacy_beat_dir(name) / "trace.json"
    if not f.exists():
        return {"missing": "fork"}
    if not o.exists():
        return {"missing": "v263"}
    A, B = json.loads(o.read_text()), json.loads(f.read_text())      # A = v2.6.3, B = fork
    ra, rb = A["rows"], B["rows"]
    res = {"frames_v263": len(ra), "frames_fork": len(rb),
           "inputs_equal": A["inputs"] == B["inputs"],
           "phase": (B.get("phase") or {}).get("legacy"),
           "lead_in": B.get("lead_in_frames"), "phase_correction": B.get("phase_correction_frames"),
           "tail": [A.get("tail_idle_frames"), B.get("tail_idle_frames")]}
    if not res["inputs_equal"]:
        ia, ib = A["inputs"], B["inputs"]
        k = next((i for i in range(min(len(ia), len(ib))) if ia[i] != ib[i]), min(len(ia), len(ib)))
        res["first_input_difference"] = {"index": k, "v263": ia[k] if k < len(ia) else None,
                                         "fork": ib[k] if k < len(ib) else None}
    # same index (the phase lock puts row i on the v2.6.3 row's tick)
    n = min(len(ra), len(rb))
    same = 0
    first = None
    keys: dict[str, dict] = {}
    for i in range(n):
        d = _diff_keys(ra[i], rb[i])
        if not d:
            same += 1
            continue
        if first is None:
            first = i
        for k in d:
            e = keys.setdefault(k, {"first_row": i, "rows": 0, "max_dev": 0.0, "fields": []})
            e["rows"] += 1
            dev = _maxdev(ra[i].get(k), rb[i].get(k))
            e["max_dev"] = None if dev is None or e["max_dev"] is None else max(e["max_dev"], dev)
            for fld in _fields(ra[i].get(k), rb[i].get(k)):
                if fld not in e["fields"]:
                    e["fields"].append(fld)
    res["same_index"] = {"compared": n, "rows_equal": same, "first_difference_row": first, "keys": keys}
    # aligned by game state: difflib over row fingerprints without the time keys
    fa, fb = [_fp(r) for r in ra], [_fp(r) for r in rb]
    sm = difflib.SequenceMatcher(None, fa, fb, autojunk=False)
    blocks = []
    eq = 0
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            eq += i2 - i1
            continue
        blk = {"op": tag, "v263_rows": [i1, i2], "fork_rows": [j1, j2]}
        if tag in ("insert", "delete"):
            rows = rb[j1:j2] if tag == "insert" else ra[i1:i2]
            ref = (rb[j1 - 1] if j1 > 0 else None) if tag == "insert" else (ra[i1 - 1] if i1 > 0 else None)
            # a hold: the extra rows repeat the game state of the row before (a wait, a load)
            blk["hold"] = ref is not None and all(_fp(r) == _fp(ref) for r in rows)
            if not blk["hold"]:
                blk["keys"] = sorted({k for r in rows for k in _diff_keys(r, ref or {})})[:30]
        else:
            ks = set()
            for x, y in zip(ra[i1:i2], rb[j1:j2]):
                ks.update(_diff_keys(x, y))
            blk["keys"] = sorted(ks)[:40]
        blk["area"] = (rows[0] if tag in ("insert", "delete") else rb[j1]).get("area")
        blocks.append(blk)
    res["aligned"] = {"rows_matched": eq, "blocks": blocks[:60], "blocks_total": len(blocks),
                      "holds": sum(1 for b in blocks if b.get("hold")),
                      "hold_rows": sum((b["fork_rows"][1] - b["fork_rows"][0]) - (b["v263_rows"][1] - b["v263_rows"][0])
                                       for b in blocks if b.get("hold"))}
    last_a, last_b = ra[-1], rb[-1]
    res["end"] = {"keys_differing": _diff_keys(last_a, last_b)[:40],
                  "pos_dev": _maxdev(last_a.get("pos"), last_b.get("pos")),
                  "v263": rc.summary(last_a), "fork": rc.summary(last_b)}
    ra_end, rb_end = _rand_state(o.parent), _rand_state(f.parent)
    if ra_end is not None and rb_end is not None:
        res["end"]["rand_state"] = {"v263": hex(ra_end), "fork": hex(rb_end),
                                    "fork_extra_rand_calls": rand_calls_between(ra_end, rb_end)}
    return res


def verdict(r: dict) -> str:
    if "missing" in r:
        return "missing " + r["missing"]
    si, al = r["same_index"], r["aligned"]
    if si["rows_equal"] == si["compared"] and r["frames_v263"] == r["frames_fork"]:
        return "identical"
    nonhold = [b for b in al["blocks"] if not b.get("hold")]
    if not nonhold:
        return f"identical but for holds ({al['holds']} blocks, {al['hold_rows']:+d} rows)"
    return f"differs ({len(nonhold)} non-hold blocks; first same-index difference row {si['first_difference_row']})"


def cmd_compare(a) -> None:
    rc.use_fork("phase", True)
    beats = all_beats()
    names = select(a.groups, beats)
    COMPARE.mkdir(parents=True, exist_ok=True)
    by_group: dict[str, dict] = {}
    for n in names:
        r = compare_beat(n)
        r["verdict"] = verdict(r)
        by_group.setdefault(beats[n][2], {})[n] = r
        ph = r.get("phase") or {}
        print(f"{n:34s} {r.get('frames_fork')}/{r.get('frames_v263')} inputs_eq={r.get('inputs_equal')} "
              f"phase fi {ph.get('frame_index_equal')}/{ph.get('rows_compared')} "
              f"fld {ph.get('field_equal')}/{ph.get('field_known')}  {r['verdict']}", flush=True)
    for g, d in by_group.items():
        (COMPARE / f"{g}.json").write_text(json.dumps(
            {"group": g, "compared": time.strftime("%Y-%m-%d %H:%M:%S"),
             "what": "fork re-recording against the v2.6.3 trace per beat: same-index rows (time keys "
                     f"{sorted(TIME_KEYS)} left out), then aligned by game state (difflib over row "
                     "fingerprints); 'hold' blocks are extra or missing rows that repeat the previous "
                     "row's game state (waits, loads)", "beats": d}, indent=1) + "\n")


def cmd_report(a) -> None:
    """Per group: beats, rows (fork / v2.6.3), verdicts and recording seconds,
    from compare/<group>.json and the scheduler's run summaries."""
    secs: dict[str, float] = {}
    for f in sorted(LOGS.glob("run_*.json")):
        try:
            secs.update(json.loads(f.read_text()).get("seconds", {}))
        except (OSError, ValueError):
            pass
    rows_out = []
    for g in a.groups.split(","):
        path = COMPARE / f"{g}.json"
        if not path.exists():
            continue
        beats = json.loads(path.read_text())["beats"]
        n = len(beats)
        ident = sum(1 for r in beats.values() if r["verdict"] == "identical")
        holds = sum(1 for r in beats.values() if r["verdict"].startswith("identical but"))
        diff = [b for b, r in beats.items() if r["verdict"].startswith("differs")]
        miss = [b for b, r in beats.items() if r["verdict"].startswith("missing")]
        fr = sum(r.get("frames_fork") or 0 for r in beats.values())
        lr = sum(r.get("frames_v263") or 0 for r in beats.values())
        t = sum(secs.get(b, 0.0) for b in beats)
        rows_out.append({"group": g, "beats": n, "rows_fork": fr, "rows_v263": lr, "identical": ident,
                         "holds_only": holds, "differs": diff, "missing": miss, "seconds": round(t)})
        print(f"{g:5s} beats {n:3d} rows {fr}/{lr} identical {ident} holds-only {holds} differs {len(diff)} "
              f"missing {len(miss)} time {t / 60:.1f} min  {diff[:6]}")
    (COMPARE / "summary.json").write_text(json.dumps(rows_out, indent=1) + "\n")


def cmd_list(a) -> None:
    rc.use_fork("phase", True)
    beats = all_beats()
    names = select(a.groups, beats) if a.groups else list(beats)
    for n in names:
        s, _f, g = beats[n]
        print(f"{g:5s} {n:34s} <- {s:30s} v263 {legacy_frames(n):6d} frames  fork {'yes' if done(n) else '-'}")


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("run")
    p.add_argument("--groups", required=True, help="groups (main, c7, aim, exit, dmg, br, opt, a01, ...), "
                                                   "`levels`, or beat names, comma separated")
    p.add_argument("--lanes", type=int, default=3)
    p.add_argument("--redo", action="store_true", help="record beats that already have a fork folder")
    p.add_argument("--yield-every", type=float, default=900.0, help="seconds between run-lock yields (0 = never)")
    p.add_argument("--yield-pause", type=float, default=10.0)
    p.add_argument("--fork-mtvu", choices=["off", "on", "ini"], default="off")
    p.add_argument("--attempts", type=int, default=3,
                   help="runs per beat while its traced fields leave the v2.6.3 path and reruns differ")
    p.add_argument("--dry-run", action="store_true")
    p.add_argument("--out-root", help="write the beats under this root instead of build/fork_refs "
                                      "(repeat runs; sources stay the real chain; implies --redo)")
    p = sub.add_parser("worker")
    p.add_argument("beat")
    p.add_argument("--out-root")
    p.add_argument("--hash-capture", help="diagnostics: keep a hash-only frame store of the beat here")
    p.add_argument("--attempts", type=int, default=3)
    p.add_argument("--fork-mtvu", choices=["off", "on", "ini"], default="off")
    p.add_argument("--rand-log", help="diagnostics: log every rand() call of the beat here")
    p = sub.add_parser("compare")
    p.add_argument("--groups", required=True)
    p = sub.add_parser("list")
    p.add_argument("--groups", default="")
    p = sub.add_parser("report")
    p.add_argument("--groups", default=",".join(FIRST_LEVEL_GROUPS + LEVEL_ORDER))
    a = ap.parse_args()
    {"run": cmd_run, "worker": cmd_worker, "compare": cmd_compare, "list": cmd_list, "report": cmd_report}[a.cmd](a)
