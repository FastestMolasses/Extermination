#!/usr/bin/env python3
"""Audit `extern` declarations in parked files against the byte-matched corpus.

WHY THIS EXISTS (s85). A wrong extern does far more than block a match: it makes
the compiler emit different argument setup, which shows up as a plausible-looking
register-coloring or FP-arg-order residual. That fiction then gets written into
the file's // NEARMISS header as the "wall", and every later attempt trusts it.
Three such fictitious walls were found and disproved in a single wave; one file's
header blamed an "s0-spill / delay-slot artifact" that simply did not exist once
the prototype was corrected (97.857 -> 99.905, then a match).

The byte-matched corpus is the authority. If N matched files agree that a callee
takes K arguments, a parked file declaring something else is almost certainly
wrong — matched files compile to the original bytes, so their view of the ABI is
confirmed by construction.

Usage:
  audit_protos.py                 # report every disagreement, worst first
  audit_protos.py --min-votes 5   # only callees with >=5 agreeing matched files
  audit_protos.py --list          # bare list of offending files (for wave input)
"""
import argparse
import glob
import os
import re
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.chdir(ROOT)

DECL = re.compile(
    r'extern\s+[\w \*]+?\s+(func_[0-9A-Fa-f]{8}|anim_\w+|bone_\w+|copy_qw4|block_copy'
    r'|float_to_int|quat_\w+|build_trs_matrix|skin_\w+|vif_\w+|gs_\w+|dma_\w+)\s*\(([^)]*)\)\s*;')


def arity(args: str) -> int:
    args = args.strip()
    if args in ("void", ""):
        return 0
    # crude but adequate: top-level commas only (no function-pointer params in this corpus)
    return args.count(",") + 1


def classify(path: str) -> str:
    try:
        with open(path, errors="ignore") as f:
            head = f.read(200)
    except OSError:
        return "missing"
    if head.startswith("// NEARMISS"):
        return "nearmiss"
    if "INCLUDE_ASM" in head:
        return "stub"
    return "matched"


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--min-votes", type=int, default=3)
    ap.add_argument("--list", action="store_true")
    a = ap.parse_args()

    # authoritative arity per callee, voted by byte-matched files only
    votes = defaultdict(lambda: defaultdict(int))
    for fp in glob.glob("src/*.c"):
        if classify(fp) != "matched":
            continue
        with open(fp, errors="ignore") as f:
            text = f.read(8000)
        for m in DECL.finditer(text):
            votes[m.group(1)][arity(m.group(2))] += 1

    auth = {}
    for name, tally in votes.items():
        k, n = max(tally.items(), key=lambda kv: kv[1])
        # only trust a callee whose matched declarations are consistent
        if n >= a.min_votes and n >= sum(tally.values()) * 0.8:
            auth[name] = (k, n)

    findings = defaultdict(list)
    for fp in glob.glob("src/func_*.c"):
        if classify(fp) != "nearmiss":
            continue
        with open(fp, errors="ignore") as f:
            text = f.read(8000)
        for m in DECL.finditer(text):
            name = m.group(1)
            if name not in auth:
                continue
            got = arity(m.group(2))
            want, n = auth[name]
            if got != want:
                findings[os.path.basename(fp)[:-2]].append((name, got, want, n))

    if a.list:
        for f in sorted(findings, key=lambda k: -max(v[3] for v in findings[k])):
            print(f)
        return

    ranked = sorted(findings.items(), key=lambda kv: -max(v[3] for v in kv[1]))
    print(f"parked files with an extern contradicting the matched corpus: {len(ranked)}")
    print(f"(callees trusted only with >={a.min_votes} agreeing matched files and >=80% consensus)\n")
    for f, items in ranked[:30]:
        worst = max(items, key=lambda v: v[3])
        print(f"  {f:20s} {len(items)} bad decl(s); worst: {worst[0]} "
              f"declared {worst[1]}, corpus says {worst[2]} ({worst[3]} files)")


if __name__ == "__main__":
    main()
