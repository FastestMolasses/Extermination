#!/usr/bin/env python3
"""Verify a list of funcs at once: objdiff each src/obj vs build/expected, print match%.
One process (avoids per-func python/shell spawn stalls). Usage: verify_batch.py <listfile>"""
import json, subprocess, sys, os
os.chdir("/Users/abe/Documents/Extermination.nosync/Extermination")
funcs = [l.strip() for l in open(sys.argv[1]) if l.strip()]
passed = []
for f in funcs:
    e, o = f"build/expected/{f}.o", f"build/obj/{f}.o"
    if not (os.path.exists(e) and os.path.exists(o)):
        print(f"{f} MISSING"); continue
    r = subprocess.run(["tools/bin/objdiff-cli", "diff", "-1", e, "-2", o, f, "-o", "-", "--format", "json"],
                       capture_output=True, text=True)
    pct = None
    try:
        d = json.loads(r.stdout)
        for s in d["left"]["symbols"]:
            if s.get("name") == f:
                pct = s.get("match_percent")
    except Exception:
        pct = "ERR"
    print(f"{f} {pct}")
    if pct == 100.0:
        passed.append(f)
open("/tmp/h9pass.txt", "w").write("\n".join(passed) + ("\n" if passed else ""))
print(f"PASS {len(passed)}/{len(funcs)}")
