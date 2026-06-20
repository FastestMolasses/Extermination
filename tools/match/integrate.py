#!/usr/bin/env python3
"""Integrate fan-out matches into the canonical build, verifying each at true objdiff 100%.

Input: a JSON file [{"func": "func_XXXX", "c_source": "..."}, ...] (matched candidates).
For each: write src/<func>.c, compile via build.py --single-file, objdiff vs build/expected.
ACCEPT only at exactly 100.0 (leave real C in src/). REJECT otherwise -> git checkout restores
the original committed stub. Prints an ACCEPT/REJECT ledger. Run verify_all + commit afterwards.
"""
import json, os, subprocess, sys

ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
os.chdir(ROOT)

def objdiff_pct(func):
    o1, o2 = f"build/expected/{func}.o", f"build/obj/{func}.o"
    if not (os.path.exists(o1) and os.path.exists(o2)):
        return None
    r = subprocess.run(["tools/bin/objdiff-cli", "diff", "-1", o1, "-2", o2, func,
                        "-o", "-", "--format", "json"], capture_output=True, text=True)
    try:
        d = json.loads(r.stdout)
        for s in d["left"]["symbols"]:
            if s["name"] == func:
                return s["match_percent"]
    except Exception:
        return None
    return None

A = "build/asm/matchings/main/code"

def ensure_expected(funcs):
    """Assemble canonical build/expected/<f>.o for any missing (one container call)."""
    need = [f for f in funcs if not os.path.exists(f"build/expected/{f}.o")]
    if not need:
        return
    cmds = " ".join(
        f"mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc "
        f"{A}/{f}.s -o build/expected/{f}.o 2>/dev/null;" for f in need)
    subprocess.run(["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
                    "exterm-toolchain", "sh", "-c", "mkdir -p build/expected; " + cmds],
                   capture_output=True, text=True)

def integrate(func, c_source):
    src = f"src/{func}.c"
    # write candidate C
    with open(src, "w") as f:
        f.write(c_source if c_source.endswith("\n") else c_source + "\n")
    # compile one unit into canonical build/obj (+ inject relocs)
    r = subprocess.run([".venv/bin/python", "tools/decomp/build.py", "--single-file",
                        f"build/obj/{func}.o"], capture_output=True, text=True)
    pct = objdiff_pct(func)
    if pct is not None and abs(pct - 100.0) < 1e-6:
        return ("ACCEPT", pct)
    # restore committed original (stub)
    subprocess.run(["git", "checkout", "--", src], capture_output=True, text=True)
    return ("REJECT", pct)

def main():
    data = json.load(open(sys.argv[1]))
    ensure_expected([item["func"] for item in data])
    accepted, rejected = [], []
    for item in data:
        func, c = item["func"], item.get("c_source", "")
        if not c.strip():
            rejected.append((func, "no source")); continue
        status, pct = integrate(func, c)
        print(f"  {status:7s} {func}  pct={pct}")
        (accepted if status == "ACCEPT" else rejected).append((func, pct))
    print(f"\n==== integrate: {len(accepted)} ACCEPTED / {len(data)} ====")
    for f, p in accepted:
        print("  ACCEPT", f)
    # write accepted list for the commit step
    with open("build/match/accepted.txt", "w") as fh:
        fh.write("\n".join(f for f, _ in accepted) + ("\n" if accepted else ""))

if __name__ == "__main__":
    main()
