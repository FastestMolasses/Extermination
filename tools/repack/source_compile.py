"""Bounded parallel orchestration around the unchanged canonical compilers.

Executed only in source_build's disposable Linux workspace. Each command and
compiler flag comes from tools/decomp/build.py or compile_overlay_src.py; every
compiler return code is checked before any filler/link step can run.
"""
from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor
import importlib.util
import json
from pathlib import Path
import shlex
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]


def _module(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


def checked_parallel(jobs, compile_one, workers):
    """Collect all compiler statuses; do not let later successes mask failures."""
    with ThreadPoolExecutor(max_workers=workers) as executor:
        statuses = list(executor.map(compile_one, jobs))
    return [name for name, status in statuses if status != 0]


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("kind", choices=("boot", "overlays"))
    parser.add_argument("--jobs", type=int, default=4)
    args = parser.parse_args(argv)
    if not 1 <= args.jobs <= 8:
        parser.error("--jobs must be between 1 and 8")
    if args.kind == "boot":
        driver = _module(ROOT / "tools/decomp/build.py", "canonical_boot_build")
        jobs = driver.units()
        (ROOT / "build/obj").mkdir(parents=True, exist_ok=True)

        def compile_one(name):
            command = shlex.split(driver.compile_cmd(name))
            return name, subprocess.run(command, cwd=ROOT).returncode

    else:
        driver = _module(ROOT / "tools/overlay/compile_overlay_src.py", "canonical_overlay_compile")
        jobs = sorted(source for source in (ROOT / "src/overlays").glob("AREA*/*.c")
                      if not driver.is_nearmiss(source))

        def compile_one(source):
            output = ROOT / "build/overlays" / source.parent.name / "obj" / f"{source.stem}.o"
            return str(source.relative_to(ROOT)), driver.compile_one(source, output)

    failures = checked_parallel(jobs, compile_one, args.jobs)
    report = dict(kind=args.kind, workers=args.jobs, units=len(jobs), failures=failures,
                  compiler_rules="unchanged canonical per-file compiler and flags")
    (ROOT / "build" / f"source-{args.kind}-compile.json").write_text(json.dumps(report, indent=2) + "\n")
    print(f"[source-compile] {args.kind}: {len(jobs)} objects, {len(failures)} failures, {args.jobs} workers")
    if failures:
        return 1
    if args.kind == "boot":
        subprocess.run([sys.executable, str(ROOT / "tools/decomp/inject_relocs.py")], cwd=ROOT, check=True)
        driver.write_objdiff()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
