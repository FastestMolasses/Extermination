#!/usr/bin/env python3
"""Permute driver (PROTOTYPE): m2c base.c -> decomp-permuter.

For each func: regenerate m2c C, inline the exterm prelude, write
tools/permuter/work/<func>/base.c, then run tools/permuter/run_func.sh inside the
container (auto-randomize unless base.c already has PERM_* macros). A true 100%
lands in tools/permuter/work/<func>/output-0-*/source.c.

Usage: permute.py [--sdt N] [--tmo SECONDS] [--jobs J] func_XXXX [func_YYYY ...]
"""
import os, subprocess, sys

ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
A = "build/asm/matchings/main/code"
PRELUDE = "tools/m2c/exterm_prelude.h"
os.chdir(ROOT)

def m2c(func):
    r = subprocess.run([".venv/bin/python3", "tools/m2c/m2c.py",
                        "--target", "mipsee-mwcc-c", "--valid-syntax", f"{A}/{func}.s"],
                       capture_output=True, text=True, timeout=90)
    if not (r.returncode == 0 and r.stdout.strip()):
        return None
    sys.path.insert(0, "tools/match")
    from m2c_clean import clean
    return clean(r.stdout)

def write_base(func):
    body = m2c(func)
    if not body:
        return False
    d = f"tools/permuter/work/{func}"
    os.makedirs(d, exist_ok=True)
    prelude = open(PRELUDE).read()
    with open(f"{d}/base.c", "w") as f:
        f.write(prelude + "\n" + body)
    return True

def run_one(func, sdt, tmo, jobs):
    log = f"tools/permuter/work/{func}/run.log"
    cmd = f"tools/permuter/run_func.sh {func} {sdt} {tmo} {jobs} > {log} 2>&1; cat {log}"
    r = subprocess.run(["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
                        "exterm-permuter", "sh", "-c", cmd], capture_output=True, text=True)
    out = r.stdout + r.stderr
    # surface the meaningful lines
    keep = [l for l in out.splitlines()
            if any(k in l for k in ("score", "found", "output-", "ERROR", "base ", "perm", "Traceback", "Error"))]
    print(f"\n===== {func} (sdt={sdt}) =====")
    print("\n".join(keep[-25:]) if keep else out[-1500:])
    found = os.path.exists(f"tools/permuter/work/{func}") and any(
        n.startswith("output-0-") for n in os.listdir(f"tools/permuter/work/{func}"))
    print(f"RESULT {func}: {'MATCH(0)' if found else 'no-zero'}")
    return found

def main():
    args = sys.argv[1:]
    sdt, tmo, jobs = "0", "600", "4"
    funcs = []
    i = 0
    while i < len(args):
        a = args[i]
        if a == "--sdt": sdt = args[i+1]; i += 2
        elif a == "--tmo": tmo = args[i+1]; i += 2
        elif a == "--jobs": jobs = args[i+1]; i += 2
        elif a.startswith("func_"): funcs.append(a); i += 1
        else: i += 1
    matched = []
    for fn in funcs:
        if not write_base(fn):
            print(f"{fn}: m2c failed, skip"); continue
        if run_one(fn, sdt, tmo, jobs):
            matched.append(fn)
    print(f"\n==== permute summary: {len(matched)}/{len(funcs)} reached score 0 ====")
    for m in matched:
        print(" MATCH:", m)

if __name__ == "__main__":
    main()
