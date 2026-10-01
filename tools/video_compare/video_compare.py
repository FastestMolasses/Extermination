#!/usr/bin/env python3
"""video_compare.py - side-by-side video of the original game (PCSX2) and the
native port, driven by the same recorded inputs.  docs/VIDEO_COMPARE.md.

  all      recording -> native playback -> PCSX2 playback -> drift report ->
           composed MP4 (one command)
  native   play a recording on the native port (headless), keep frames/audio
  ps2      play a recording on the original in PCSX2, keep frames/audio
           (waits for build/.pcsx2.lock)
  compose  align, label, speed up, encode (+ the drift report)
  drift    the drift report only (position/heading per tick)
  synth    write a recording from a small input script (tests, demos)
  info     summarise a recording's segments
  clean    delete a run folder under build/video_compare/

Each stage runs under a python that has its modules: the PCSX2 stage needs
zstandard (the decomp .venv), compose needs Pillow; the dispatcher picks an
interpreter that has them.  Everything written goes under
build/video_compare/<run>/ (ignored: the frames and audio are derived from
the user's own disc).
"""
from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
BUILD = ROOT / "build/video_compare"
sys.path.insert(0, str(HERE))
import emrec  # noqa: E402

NEEDS = {"ps2": ["zstandard", "numpy"], "native": ["numpy"], "compose": ["numpy", "PIL"],
         "drift": [], "synth": [], "info": [], "clean": [], "all": []}


def interpreter_for(mods: list[str]) -> str:
    cands = [sys.executable, str(ROOT / ".venv/bin/python"), shutil.which("python3") or "",
             "/Library/Frameworks/Python.framework/Versions/Current/bin/python3", "/opt/homebrew/bin/python3",
             "/usr/bin/python3"]
    probe = "import importlib,sys; [importlib.import_module(m) for m in sys.argv[1:]]"
    for c in cands:
        if c and Path(c).exists() and subprocess.run([c, "-c", probe, *mods], capture_output=True).returncode == 0:
            return c
    raise SystemExit(f"no python with {mods} found (tried {cands}); see docs/VIDEO_COMPARE.md 'Setup'")


def run_stage(stage: str, argv: list[str]) -> int:
    py = interpreter_for(NEEDS[stage])
    mod = {"ps2": "ps2.py", "native": "native.py", "compose": "compose.py"}[stage]
    return subprocess.run([py, str(HERE / mod), *argv]).returncode


# ------------------------------------------------------------------ synth

DEMO = {
    "_doc": "New Game, both movies skipped, the AREA11 opening skipped with START, then a short walk.",
    "title_ticks": 1400, "title_cross_at": 1300, "boot_movie_at": 1109, "skip_movies": True,
    "segments": [
        {"phase": "P", "ticks": 2},
        {"phase": "L", "ticks": 64},
        {"phase": "P", "ticks": 4},
        {"phase": "C", "ticks": 1400, "inputs": [{"at": 300, "for": 2, "buttons": ["START"]}]},
        {"phase": "P", "ticks": 600, "inputs": [
            {"at": 40, "for": 140, "ly": 0x00},
            {"at": 180, "for": 45, "lx": 0x00},
            {"at": 225, "for": 150, "ly": 0x00},
            {"at": 375, "for": 30, "lx": 0xFF, "ly": 0x00}]},
    ],
}
BUTTONS = {"SELECT": 0x0001, "L3": 0x0002, "R3": 0x0004, "START": 0x0008, "UP": 0x0010, "RIGHT": 0x0020,
           "DOWN": 0x0040, "LEFT": 0x0080, "L2": 0x0100, "R2": 0x0200, "L1": 0x0400, "R1": 0x0800,
           "TRIANGLE": 0x1000, "CIRCLE": 0x2000, "CROSS": 0x4000, "SQUARE": 0x8000}


def synth(spec: dict, out: Path) -> None:
    """A recording from a script: the title ticks (counter = tick index, the
    port boots at 0) with Cross on NEW GAME, a skipped movie row after the
    boot movie and after the Cross, then one recorded segment per entry.
    Anchors other than the phase are zero: playback needs only phase + pad."""
    rows, step = [], 0

    def add(counter, mv, ph, pad):
        nonlocal step
        rows.append({"step": step, "counter": counter, "mv": mv, "ph": ph, "btn": pad[0], "lx": pad[1],
                     "ly": pad[2], "rx": pad[3], "ry": pad[4], "seg": -1, "off": -1, "cap": -1, "af": -1})
        step += 1
    skip = (emrec.START if spec.get("skip_movies", True) else 0, 0x80, 0x80, 0x80, 0x80)
    cross = spec["title_cross_at"]
    for c in range(spec["title_ticks"]):
        pad = (BUTTONS["CROSS"], 0x80, 0x80, 0x80, 0x80) if c in (cross, cross + 1) else emrec.NEUTRAL
        add(c, 0, "T", pad)
        if c == spec.get("boot_movie_at") or c == cross + 1:
            add(c, 1, "M", skip)
    c = spec["title_ticks"]
    for seg in spec["segments"]:
        n = seg["ticks"]
        pads = [list(emrec.NEUTRAL) for _ in range(n)]
        for inp in seg.get("inputs", []):
            for k in range(inp["at"], min(n, inp["at"] + inp["for"])):
                p = pads[k]
                p[0] |= sum(BUTTONS[b] for b in inp.get("buttons", []))
                for i, key in ((1, "lx"), (2, "ly"), (3, "rx"), (4, "ry")):
                    if key in inp:
                        p[i] = inp[key]
        for p in pads:
            add(c, 0, seg["phase"], tuple(p))
            c += 1
    emrec.write(out, {"source": "synth", "mode": "recording", "ps2_disc_drive_timing": 0, "tick_hz": "59.94",
                      "note": spec.get("_doc", "")}, rows)


# ------------------------------------------------------------------- info

def info(path: Path) -> None:
    rec = emrec.read(path)
    print(json.dumps(rec.header, indent=1))
    segs = rec.segments()
    print(f"{len(rec.rows)} rows, {len(rec.ticks())} ticks, {len(segs)} segments, movies "
          f"{rec.movies()}")
    for i, (ph, rows) in enumerate(segs):
        pads = sum(1 for r in rows if (r["btn"], r["lx"], r["ly"], r["rx"], r["ry"]) != emrec.NEUTRAL)
        print(f"  seg {i:3d} {ph} ticks {len(rows):6d}  counters {rows[0]['counter']}..{rows[-1]['counter']}"
              f"  non-neutral pads {pads}")


# -------------------------------------------------------------------- all

def run_all(a) -> int:
    run = BUILD / a.name
    run.mkdir(parents=True, exist_ok=True)
    stride = a.stride or max(1, round(emrec.TICK_HZ * a.speed / a.fps))
    if a.stride is None and a.speed * emrec.TICK_HZ / (a.fps * stride) < 0.999:
        stride = max(1, int(emrec.TICK_HZ * a.speed / a.fps))
    rec = str(Path(a.recording).resolve())
    want_native_audio = a.audio in ("native", "both")
    want_ps2_audio = a.audio in ("original", "both")
    if not a.skip_native:
        argv = [rec, "--out", str(run / "native"), "--port", a.port, "--stride", str(stride), "--sampling",
                a.sampling, "--disc-timing", a.disc_timing]
        if want_native_audio:
            argv.append("--audio")
        if run_stage("native", argv) != 0:
            print("all: the native playback failed (see native/port.log); composing what exists")
    if not a.skip_ps2:
        argv = [rec, "--out", str(run / "ps2"), "--stride", str(stride), "--frames", a.ps2_frames]
        if want_ps2_audio:
            argv.append("--audio")
        if run_stage("ps2", argv) != 0:
            return 1
    out = Path(a.out) if a.out else run / f"{a.name}.mp4"
    argv = ["--native", str(run / "native"), "--ps2", str(run / "ps2"), "--out", str(out),
            "--speed", str(a.speed), "--fps", str(a.fps), "--loads", a.loads, "--audio", a.audio,
            "--height", str(a.height)]
    for flag, val in (("--title", a.title), ("--subtitle", a.subtitle), ("--target-mb", a.target_mb)):
        if val:
            argv += [flag, str(val)]
    if a.info:
        argv.append("--info")
    if a.summary_seconds:
        argv += ["--summary-seconds", str(a.summary_seconds)]
    return run_stage("compose", argv)


def main() -> int:
    if len(sys.argv) > 1 and sys.argv[1] in ("native", "ps2", "compose"):
        return run_stage(sys.argv[1], sys.argv[2:])      # the stage parses its own options
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0],
                                 formatter_class=argparse.RawDescriptionHelpFormatter, epilog=__doc__)
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("all", help="the whole pipeline")
    p.add_argument("recording")
    p.add_argument("--name", default="run", help="run folder under build/video_compare/")
    p.add_argument("--out", help="output MP4 (default build/video_compare/<name>/<name>.mp4)")
    p.add_argument("--port", default=str(ROOT.parent / "extermination-port"))
    p.add_argument("--speed", type=float, default=2.0)
    p.add_argument("--fps", type=float, default=30.0)
    p.add_argument("--stride", type=int, default=None, help="capture stride (default from --speed/--fps)")
    p.add_argument("--loads", choices=["trim", "hold"], default="trim")
    p.add_argument("--audio", choices=["original", "native", "both", "none"], default="none")
    p.add_argument("--sampling", choices=["gs", "area"], default="gs")
    p.add_argument("--ps2-frames", choices=["gs", "screenshot"], default="gs")
    p.add_argument("--disc-timing", choices=["recorded", "0", "1"], default="recorded")
    p.add_argument("--title")
    p.add_argument("--subtitle")
    p.add_argument("--height", type=int, default=480)
    p.add_argument("--target-mb", type=float)
    p.add_argument("--info", action="store_true")
    p.add_argument("--summary-seconds", type=float, default=0.0)
    p.add_argument("--skip-native", action="store_true", help="reuse <run>/native")
    p.add_argument("--skip-ps2", action="store_true", help="reuse <run>/ps2")
    for stage in ("native", "ps2", "compose"):
        sub.add_parser(stage, help=f"one stage; '{stage} -h' lists its options")
    q = sub.add_parser("drift", help="drift report from two run folders")
    q.add_argument("--native", required=True)
    q.add_argument("--ps2", required=True)
    q.add_argument("--tol-pos", type=float, default=0.01)
    q.add_argument("--tol-yaw", type=float, default=0.001)
    q = sub.add_parser("synth", help="write a recording from an input script")
    q.add_argument("out")
    q.add_argument("--spec", help="JSON script (default: the built-in demo)")
    q = sub.add_parser("info", help="summarise a recording")
    q.add_argument("recording")
    q = sub.add_parser("clean", help="delete build/video_compare/<name>")
    q.add_argument("name")
    a = ap.parse_args()
    if a.cmd == "all":
        return run_all(a)
    if a.cmd == "drift":
        import compose
        n = [r for r in emrec.read(Path(a.native) / "native.rec").ticks() if r["seg"] >= 0]
        p_ = [r for r in emrec.read(Path(a.ps2) / "ps2.rec").ticks() if r["seg"] >= 0]
        print(compose.drift_text(compose.drift(n, p_, a.tol_pos, a.tol_yaw)))
        return 0
    if a.cmd == "synth":
        spec = json.loads(Path(a.spec).read_text()) if a.spec else DEMO
        synth(spec, Path(a.out))
        info(Path(a.out))
        return 0
    if a.cmd == "info":
        info(Path(a.recording))
        return 0
    if a.cmd == "clean":
        target = (BUILD / a.name).resolve()
        if BUILD.resolve() not in target.parents:
            raise SystemExit("clean: only folders under build/video_compare/")
        shutil.rmtree(target, ignore_errors=True)
        print(f"removed {target}")
        return 0
    return 1


if __name__ == "__main__":
    sys.exit(main())
