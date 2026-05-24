#!/usr/bin/env python3
"""
tools/overlay/build.py — End-to-end overlay pipeline driver.

Runs the full pipeline for one or all overlays:
  1. Extract AREA*.BIN from ISO (if not already done).
  2. Generate splat YAML configs (if not already done).
  3. Run splat to disassemble each overlay.
  4. fill_overlay.py to assemble all .s → .o.
  5. link_overlay.py to link + pack MWo3 + verify byte-identity.

Usage (run from repo root):

  # Full pipeline for AREA18 only:
  container run --rm -v "$PWD:/work" -w /work exterm-toolchain \
      python3 tools/overlay/build.py --area AREA18

  # Full pipeline for all 19 overlays:
  container run --rm -v "$PWD:/work" -w /work exterm-toolchain \
      python3 tools/overlay/build.py --all

  # Skip extraction + yaml gen (assume already done), just fill+link:
  container run --rm -v "$PWD:/work" -w /work exterm-toolchain \
      python3 tools/overlay/build.py --area AREA18 --no-extract --no-yaml --no-splat

  # Steps 1-3 run on the host (no container needed); steps 4-5 need the container.
  # On macOS host: run steps 1-3 first, then use container for 4-5.

Flags:
  --area AREA18        process only this overlay
  --all                process all 19 overlays (default if no --area)
  --no-extract         skip ISO extraction (extract/OVERLAY/ already populated)
  --no-yaml            skip splat YAML generation (config/overlays/ already populated)
  --no-splat           skip splat disassembly (build/overlays/*/asm/ already populated)
  --no-fill            skip fill_overlay.py (build/overlays/*/filler/ already populated)
  --no-verify          skip byte-identity check
  --dry-run            stop before mwldmips invocation
  --iso PATH           ISO path (default: Extermination-rebuilt.iso)
  --jobs N             parallel jobs for fill_overlay (default: 4)
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
VENV_PYTHON = ROOT / ".venv" / "bin" / "python"
SPLAT = [str(VENV_PYTHON), "-m", "splat", "split"] if VENV_PYTHON.exists() else [
    sys.executable, "-m", "splat", "split"]

# All 19 shipped overlays in order (gap-numbered on disc).
ALL_OVERLAYS = [
    "AREA00", "AREA01", "AREA02", "AREA03", "AREA04",
    "AREA06", "AREA07", "AREA08", "AREA11", "AREA13",
    "AREA14", "AREA15", "AREA16", "AREA17", "AREA18",
    "AREA19", "AREA20", "AREA21", "AREA22",
]


def run(cmd: list[str], desc: str = "", **kw) -> int:
    if desc:
        print(f"[build] {desc}")
    r = subprocess.run(cmd, cwd=ROOT, **kw)
    return r.returncode


def step_extract(iso_path: Path, area: str | None) -> int:
    """Extract AREA*.BIN files from the ISO."""
    cmd = [sys.executable,
           str(ROOT / "tools" / "overlay" / "extract_overlays.py"),
           "--iso", str(iso_path)]
    if area:
        cmd += ["--area", area]
    return run(cmd, f"extracting overlay(s) from {iso_path.name}")


def step_gen_yaml(area: str | None) -> int:
    """Generate per-overlay splat YAML configs."""
    cmd = [sys.executable,
           str(ROOT / "tools" / "overlay" / "gen_splat_yaml.py")]
    if area:
        cmd += ["--area", area]
    else:
        cmd += ["--all"]
    return run(cmd, "generating splat YAML configs")


def step_splat(name: str) -> int:
    """Run splat on the per-overlay YAML."""
    yaml = ROOT / "config" / "overlays" / f"{name}.yaml"
    if not yaml.exists():
        print(f"[build] ERROR: {yaml} not found", file=sys.stderr)
        return 1
    return run(SPLAT + [str(yaml)], f"splat: disassembling {name}")


def step_fill(name: str, jobs: int) -> int:
    """Assemble all .s → .o via fill_overlay.py."""
    return run([sys.executable,
                str(ROOT / "tools" / "overlay" / "fill_overlay.py"),
                name, "--jobs", str(jobs)],
               f"fill_overlay: assembling {name}")


def step_link(name: str, no_verify: bool, dry_run: bool) -> int:
    """Link + pack MWo3 + verify."""
    cmd = [sys.executable,
           str(ROOT / "tools" / "overlay" / "link_overlay.py"),
           name, "--no-fill"]
    if no_verify:
        cmd.append("--no-verify")
    if dry_run:
        cmd.append("--dry-run")
    return run(cmd, f"link_overlay: linking {name}")


def process_one(name: str, args: argparse.Namespace) -> int:
    """Run the full pipeline for one overlay. Returns 0 on success."""
    print(f"\n{'='*60}")
    print(f"  {name}")
    print(f"{'='*60}")

    if not args.no_extract:
        rc = step_extract(args.iso, name)
        if rc != 0:
            print(f"[build] extraction failed for {name}", file=sys.stderr)
            return rc

    if not args.no_yaml:
        rc = step_gen_yaml(name)
        if rc != 0:
            print(f"[build] YAML gen failed for {name}", file=sys.stderr)
            return rc

    if not args.no_splat:
        rc = step_splat(name)
        if rc != 0:
            print(f"[build] splat failed for {name}", file=sys.stderr)
            return rc

    if not args.no_fill:
        rc = step_fill(name, args.jobs)
        if rc != 0:
            print(f"[build] fill failed for {name}", file=sys.stderr)
            return rc

    rc = step_link(name, args.no_verify, args.dry_run)
    return rc


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description="Overlay pipeline driver")
    ap.add_argument("--area", help="Process only this overlay (e.g. AREA18)")
    ap.add_argument("--all", action="store_true",
                    help="Process all 19 overlays")
    ap.add_argument("--iso", type=Path,
                    default=ROOT / "Extermination-rebuilt.iso")
    ap.add_argument("--no-extract", action="store_true")
    ap.add_argument("--no-yaml", action="store_true")
    ap.add_argument("--no-splat", action="store_true")
    ap.add_argument("--no-fill", action="store_true")
    ap.add_argument("--no-verify", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--jobs", "-j", type=int, default=4)
    args = ap.parse_args(argv)

    if args.area:
        overlays = [args.area.upper()]
    else:
        overlays = ALL_OVERLAYS

    results: dict[str, str] = {}
    for name in overlays:
        rc = process_one(name, args)
        results[name] = "PASS" if rc == 0 else f"FAIL(rc={rc})"

    print(f"\n{'='*60}")
    print("  Overlay pipeline summary")
    print(f"{'='*60}")
    passed = sum(1 for v in results.values() if v == "PASS")
    for name, status in results.items():
        print(f"  {name:<10}  {status}")
    print(f"{'='*60}")
    print(f"  {passed}/{len(results)} overlays passed")

    return 0 if passed == len(results) else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
