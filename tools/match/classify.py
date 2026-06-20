#!/usr/bin/env python3
"""Classify each baseline candidate's instruction-level delta vs target.

Uses the aligned objdiff JSON (host objdiff-cli). Buckets each near-miss by the
technique most likely to close it, so we can estimate yield per technique.
"""
import json, os, subprocess, sys, re

PROTO = "build/match"
os.chdir("/Users/abe/Documents/Extermination.nosync/Extermination")

def insns(symbols, name):
    for s in symbols:
        if s["name"] == name:
            return [(i.get("instruction") or {}).get("formatted") for i in s["instructions"]], s["match_percent"]
    return None, None

def reg_only_diff(l, r):
    # same mnemonic + same structure, differ only in register tokens
    if not l or not r: return False
    lm, rm = l.split(None, 1), r.split(None, 1)
    if lm[0] != rm[0]: return False
    # strip register names ($.. and gpr names), compare skeleton
    def skel(s):
        return re.sub(r"\b(zero|at|v[01]|a[0-3]|t\d|s\d|k[01]|gp|sp|fp|ra|f\d+|v[01])\b", "R", s)
    return skel(l) == skel(r) and l != r

def classify(name):
    o1, o2 = f"{PROTO}/exp/{name}.o", f"{PROTO}/obj/{name}.o"
    if not (os.path.exists(o1) and os.path.exists(o2)): return None
    r = subprocess.run(["tools/bin/objdiff-cli", "diff", "-1", o1, "-2", o2, name,
                        "-o", "-", "--format", "json"], capture_output=True, text=True)
    try:
        d = json.loads(r.stdout)
    except Exception:
        return None
    L, pct = insns(d["left"]["symbols"], name)
    R, _ = insns(d["right"]["symbols"], name)
    if L is None or R is None: return None
    n = max(len(L), len(R))
    diffs = []
    ins_extra = del_extra = 0
    for i in range(n):
        l = L[i] if i < len(L) else None
        rr = R[i] if i < len(R) else None
        if l == rr: continue
        if l is None: ins_extra += 1; diffs.append(("ins", rr)); continue
        if rr is None: del_extra += 1; diffs.append(("del", l)); continue
        diffs.append(("chg", l, rr))
    nd = len(diffs)
    # categorize
    cat = "other"
    # value mis-decode: sw/sd/li zero<->reg or const<->const
    def is_valuebug(l, r):
        return (("sw " in l or "sd " in l or "sh " in l or "sb " in l or l.startswith("li ") or "addiu" in l)
                and (("zero" in l) != ("zero" in r) or (re.sub(r"0x[0-9a-f]+","K",l)==re.sub(r"0x[0-9a-f]+","K",r) and l!=r)))
    chg = [x for x in diffs if x[0]=="chg"]
    if nd == 0:
        cat = "MATCH"
    elif ins_extra+del_extra > 0 and any("s0" in (x[1] if x[0] != 'chg' else x[2]) or "paddub" in (x[1] if x[0]!='chg' else x[2]) or "sq s" in (x[1] if x[0]!='chg' else x[2]) for x in diffs):
        cat = "save/IPA(multi-TU)"
    elif ins_extra+del_extra > 0:
        cat = "schedule/count"
    elif chg and all(reg_only_diff(x[1], x[2]) for x in chg):
        cat = "regalloc(permuter)"
    elif chg and any(is_valuebug(x[1], x[2]) for x in chg):
        cat = "m2c-value-bug"
    elif chg and any(("b" == x[1][:1] or "j" == x[1][:1]) and x[1].split()[0]!=x[2].split()[0] for x in chg):
        cat = "branch-shape"
    return (name, pct, nd, cat)

def main():
    names = [l.strip() for l in open(sys.argv[1])] if len(sys.argv) > 1 else []
    names = [n for n in names if n.startswith("func_")]
    rows = []
    for n in names:
        c = classify(n)
        if c: rows.append(c)
    # summary
    from collections import Counter
    cc = Counter(r[3] for r in rows)
    print(f"classified {len(rows)} funcs:")
    for cat, k in cc.most_common():
        print(f"  {k:3d}  {cat}")
    print("\n-- detail (sorted by match% desc) --")
    for name, pct, nd, cat in sorted(rows, key=lambda x: -(x[1] or 0))[:60]:
        print(f"  {pct:6.2f}  ndiff={nd:2d}  {cat:20s}  {name}")

if __name__ == "__main__":
    main()
