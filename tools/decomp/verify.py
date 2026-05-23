#!/usr/bin/env python3
"""Run objdiff-cli over every unit and report aggregate match status.

Reads `objdiff.json`, diffs each unit's target + base object, and prints a
summary (overall match %, perfect/partial counts, and the partial list with
percentages). Useful as a "where are we?" check after `build.py build`.

Usage:
    .venv/bin/python tools/decomp/verify.py            # summary only
    .venv/bin/python tools/decomp/verify.py --list     # also list every match
    .venv/bin/python tools/decomp/verify.py --threshold 95   # show partials under N%
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OBJDIFF_JSON = ROOT / "objdiff.json"
OBJDIFF_CLI = ROOT / "tools/bin/objdiff-cli"


def match_percent(unit: dict) -> float | None:
    """Return the .text match % for one unit, or None if it can't be diffed."""
    name = unit["name"]
    target = ROOT / unit["target_path"]
    base = ROOT / unit["base_path"]
    if not (target.exists() and base.exists()):
        return None
    proc = subprocess.run(
        [str(OBJDIFF_CLI), "diff", "-1", str(target), "-2", str(base),
         "-o", "-", "--format", "json", name],
        capture_output=True, text=True,
    )
    if proc.returncode != 0:
        return None
    try:
        d = json.loads(proc.stdout)
    except json.JSONDecodeError:
        return None
    section = next((s for s in d.get("left", {}).get("sections", [])
                    if s.get("name") == ".text"), None)
    if section is None:
        return None
    return section.get("match_percent")


def main(argv: list[str]) -> int:
    p = argparse.ArgumentParser(description="Match-status report.")
    p.add_argument("--list", action="store_true",
                   help="list every unit (perfect ones included)")
    p.add_argument("--threshold", type=float, default=100.0,
                   help="only show partials below this percent (default 100)")
    args = p.parse_args(argv)

    if not OBJDIFF_JSON.exists():
        sys.exit("error: objdiff.json missing — run `build.py setup` first")
    cfg = json.loads(OBJDIFF_JSON.read_text())
    units = cfg.get("units", [])

    perfect: list[str] = []
    partial: list[tuple[str, float]] = []
    missing: list[str] = []
    for u in units:
        pct = match_percent(u)
        if pct is None:
            missing.append(u["name"])
        elif pct == 100.0:
            perfect.append(u["name"])
        else:
            partial.append((u["name"], pct))

    total = len(units)
    print(f"  perfect: {len(perfect)} / {total}  ({100.0 * len(perfect) / total:.1f}%)")
    print(f"  partial: {len(partial)}")
    if missing:
        print(f"  missing objects: {len(missing)}")
    print()

    if args.list:
        for name in sorted(perfect):
            print(f"  100.0%  {name}")
    for name, pct in sorted(partial, key=lambda x: -x[1]):
        if pct < args.threshold:
            print(f"  {pct:5.1f}%  {name}")
    if missing and args.list:
        for name in sorted(missing):
            print(f"  ----    {name}  (no .o pair)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
