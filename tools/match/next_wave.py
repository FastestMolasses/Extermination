#!/usr/bin/env python3
"""Emit the next N fan-out candidates as JSON, and append them to attempted.txt.

Priority: (1) fresh hybrid-asm funcs (readability upgrades, smallest first),
then (2) stub funcs by m2c baseline desc, then (3) any remaining stubs.
Excludes anything in attempted.txt and anything whose src is already clean-readable
(not INCLUDE_ASM and not hybrid-asm). Usage: next_wave.py [N]  (default 96)
"""
import os, re, json, sys, glob
os.chdir("/Users/abe/Documents/Extermination.nosync/Extermination")
A = "build/asm/matchings/main/code"
N = int(sys.argv[1]) if len(sys.argv) > 1 else 96

def lines(fn):
    try: return [x.strip() for x in open(fn) if x.strip()]
    except FileNotFoundError: return []

attempted = set(lines("build/match/attempted.txt"))

def baselines():
    bl = {}
    for fn in ("baseline_results", "backlog_full_results", "word_results", "stubs_unmeasured_results"):
        for l in lines(f"build/match/{fn}.txt"):
            a = l.split()
            if len(a) == 2 and a[0][0].isdigit():
                try: bl[a[1]] = max(bl.get(a[1], 0), float(a[0]))
                except: pass
    return bl

def size(f):
    p = f"{A}/{f}.s"
    if not os.path.exists(p): return 10**9
    m = re.search(r'nonmatching[^\n]*0x([0-9A-Fa-f]+)', open(p).read(4000))
    return int(m.group(1), 16) if m else 10**9

def still_needs_work(f):
    fp = f"src/{f}.c"
    if not os.path.exists(fp): return False
    h = open(fp, errors="ignore").read(400)
    return ("INCLUDE_ASM" in h) or ("Hybrid asm" in h) or ("asm void" in h) or ("__asm" in h) or ("asm " in h[:160]) or ("asm(" in h[:400])

hybrids = [f for f in lines("build/match/hybrids_all.txt") if f not in attempted and os.path.exists(f"{A}/{f}.s")]
hybrids.sort(key=size)
bl = baselines()
stubs = [f for f in lines("build/match/stubs_all.txt") if f not in attempted and os.path.exists(f"{A}/{f}.s")]
stubs.sort(key=lambda f: -bl.get(f, 0))

pick, seen = [], set()
for f in hybrids + stubs:
    if len(pick) >= N: break
    if f in seen or f in attempted: continue
    if not still_needs_work(f): continue
    seen.add(f); pick.append(f)

with open("build/match/attempted.txt", "a") as fh:
    for f in pick: fh.write(f + "\n")
sys.stderr.write(f"next wave: {len(pick)} candidates ({sum(1 for f in pick if f in set(hybrids))} hybrids); "
                 f"remaining unattempted hybrids={len([f for f in hybrids if f not in seen])}, stubs={len([f for f in stubs if f not in seen])}\n")
print(json.dumps(pick))
