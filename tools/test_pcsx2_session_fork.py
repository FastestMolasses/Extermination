#!/usr/bin/env python3
"""ForkSession frame steps: a tick stop that executed nothing is not a frame.

A fork state saved at the main-loop top loads with the PC on the tick PC, and
the fork counts that tick at once: the first `run {until: {ticks: 1}}` after
the load stops where it started.  With align=False, step() used to take that
stop for a frame and raise "frame step skipped".

    # macOS (arm64), decomp .venv; the unit checks need no emulator (< 1 s)
    .venv/bin/python tools/test_pcsx2_session_fork.py
    # plus a live check on the agent-debug fork (fork state 04, about 15 s;
    # takes the run lock, runs hidden, leaves nothing running)
    .venv/bin/python tools/test_pcsx2_session_fork.py --live
"""
from __future__ import annotations

import argparse
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pcsx2_session as ps  # noqa: E402


class FakeClient:
    """Replies to `run` with the queued stops, in order."""

    def __init__(self, stops: list[dict]):
        self.stops = list(stops)
        self.runs = 0

    def call(self, cmd: str, **_kw) -> dict:
        assert cmd == "run", cmd
        self.runs += 1
        return {"stop": self.stops.pop(0)}


def fake_session(start_cycle: int, stops: list[dict]) -> tuple[ps.ForkSession, FakeClient]:
    s = object.__new__(ps.ForkSession)   # no launch: only the stepping logic is under test
    s._client = FakeClient(stops)
    s._ee_cycle = start_cycle
    s.boundary_timeout = 30.0
    return s, s._client


def tick(cycle: int) -> dict:
    return {"reason": "tick", "pc": "0x001aaf28", "ee_cycle": cycle}


def unit() -> list[tuple[str, bool]]:
    out = []
    # loaded at the loop top: the first stop executed nothing, the second is the frame
    s, c = fake_session(1000, [tick(1000), tick(5000)])
    s._resume_to_boundary()
    out.append(("zero-advance stop after a load is repeated", c.runs == 2 and s._ee_cycle == 5000))
    # an ordinary step: one run
    s, c = fake_session(5000, [tick(9000)])
    s._resume_to_boundary()
    out.append(("ordinary step is one run", c.runs == 1 and s._ee_cycle == 9000))
    # two stops without execution: an error, not a silent frame
    s, c = fake_session(5000, [tick(5000), tick(5000)])
    try:
        s._resume_to_boundary()
        out.append(("two zero-advance stops raise", False))
    except RuntimeError:
        out.append(("two zero-advance stops raise", c.runs == 2))
    # a stop that is not the tick (timeout, movie probe) still raises
    s, c = fake_session(5000, [{"reason": "timeout", "ee_cycle": 7000}])
    try:
        s._resume_to_boundary()
        out.append(("non-tick stop raises", False))
    except TimeoutError:
        out.append(("non-tick stop raises", True))
    return out


def live() -> list[tuple[str, bool]]:
    out = []
    with tempfile.TemporaryDirectory(prefix="fork-session-test-") as tmp:
        for align in (False, True):
            with ps.ForkSession(ps.fork_state("04"), log_dir=Path(tmp) / f"align{int(align)}", align=align) as s:
                loaded = s.load_state_info
                c0 = s.u32(ps.FRAME_COUNTER)
                counters = s.step(3)
                out.append((f"align={align}: state 04 loaded at the loop top",
                            int(str(loaded.get("pc")), 16) == ps.LOOP_TOP))
                out.append((f"align={align}: step(3) advances the loop counter by 1, 1, 1 ({c0} -> {counters})",
                            counters == [c0 + 1, c0 + 2, c0 + 3] and s.frames_stepped == 3))
    return out


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--live", action="store_true", help="also step the original on the agent-debug fork")
    a = ap.parse_args()
    results = unit() + (live() if a.live else [])
    for name, ok in results:
        print(f"[{'PASS' if ok else 'FAIL'}] {name}")
    passed = sum(ok for _, ok in results)
    print(f"{passed}/{len(results)} checks passed")
    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    sys.exit(main())
