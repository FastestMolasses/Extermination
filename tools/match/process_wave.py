#!/usr/bin/env python3
"""Process one fan-out workflow output: integrate verified matches end-to-end.

  process_wave.py <workflow_output.json>

Extract matched {func,c_source} -> write src -> build.py --single-file (+ensure expected)
-> objdiff must be 100% (else restore stub) -> trim prelude -> recompile -> re-verify 100%
-> classify NEW (stub at HEAD) vs upgrade -> update FUNCTIONS.csv -> regenerate objdiff.json.
Prints the accepted ledger. Caller then runs verify_all + git commit.
"""
import json, os, subprocess, sys
sys.path.insert(0, "tools/match")
from integrate import ensure_expected, objdiff_pct
from trim_prelude import trim

ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
os.chdir(ROOT)

def sh(cmd):
    return subprocess.run(cmd, capture_output=True, text=True)

def container_build(funcs):
    if not funcs: return
    lst = " ".join(funcs)
    sh(["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work", "exterm-toolchain",
        "sh", "-c", f'for f in {lst}; do .venv/bin/python tools/decomp/build.py --single-file build/obj/$f.o >/dev/null 2>&1; done'])

def was_stub_at_head(func):
    r = sh(["git", "show", f"HEAD:src/{func}.c"])
    return "INCLUDE_ASM" in (r.stdout.splitlines() or [""])[0] if r.stdout else False

def main():
    d = json.load(open(sys.argv[1]))
    root = d.get("result", d)
    res = []
    for b in root.get("batches", []) + root.get("results", []):
        if isinstance(b, dict) and "results" in b:
            res += b["results"]
        elif isinstance(b, dict) and "func" in b:
            res.append(b)
    cand = [(r["func"], r["c_source"]) for r in res if r.get("matched") and r.get("c_source", "").strip()]
    print(f"claimed matches: {len(cand)}")
    ensure_expected([f for f, _ in cand])
    # 1) write + compile all
    funcs = []
    for f, c in cand:
        open(f"src/{f}.c", "w").write(c if c.endswith("\n") else c + "\n")
        funcs.append(f)
    container_build(funcs)
    # 2) verify each; RETRY rejects once (transient container-contention failures lose valid matches)
    accepted = []
    rejects = [f for f in funcs if objdiff_pct(f) != 100.0]
    accepted = [f for f in funcs if f not in rejects]
    if rejects:
        container_build(rejects)  # idle retry pass
        for f in rejects:
            if objdiff_pct(f) == 100.0:
                accepted.append(f); print(f"  RECOVERED {f} (transient)")
            else:
                sh(["git", "checkout", "--", f"src/{f}.c"]); print(f"  REJECT {f}")
    # 3) trim + recompile + re-verify
    for f in accepted:
        open(f"src/{f}.c", "w").write(trim(open(f"src/{f}.c").read()))
    container_build(accepted)
    final, news = [], 0
    for f in accepted:
        if objdiff_pct(f) == 100.0:
            isnew = was_stub_at_head(f)
            news += isnew
            final.append(f)
            print(f"  ACCEPT {f}  {'NEW(stub)' if isnew else 'upgrade'}")
        else:
            sh(["git", "checkout", "--", f"src/{f}.c"]); print(f"  REJECT(post-trim) {f}")
    # 4) FUNCTIONS.csv
    mset = set(final)
    lines = open("docs/FUNCTIONS.csv").read().splitlines(); out = []; ch = 0
    for i, l in enumerate(lines):
        if i == 0: out.append(l); continue
        p = l.split(",")
        if len(p) >= 4 and p[1] in mset and p[3] != "readable":
            p[3] = "readable"; ch += 1
        out.append(",".join(p))
    open("docs/FUNCTIONS.csv", "w").write("\n".join(out) + "\n")
    # 5) regenerate objdiff.json
    sh([".venv/bin/python", "tools/decomp/build.py", "objdiff"])
    print(f"\n==== accepted {len(final)} ({news} NEW stubs, {len(final)-news} upgrades); CSV {ch} -> readable ====")
    open("build/match/last_accepted.txt", "w").write("\n".join(final) + ("\n" if final else ""))

if __name__ == "__main__":
    main()
