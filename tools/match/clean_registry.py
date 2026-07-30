#!/usr/bin/env python3
"""Reconcile docs/NEARMISS.md against src/.

A row belongs in the registry only if src/<func>.c is CURRENTLY a // NEARMISS
file. Rows go stale whenever a function is promoted to a real byte-match (or
reverted to a stub), and integrate_nearmiss.py only ever appends -- so without
this the registry silently overstates the near-miss count and lists functions
that actually match. Also drops duplicate rows, keeping the first.

Usage: clean_registry.py [--check]   (--check = report only, non-zero if dirty)
"""
import os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.chdir(ROOT)
REG = "docs/NEARMISS.md"

def is_nearmiss(func):
    p = f"src/{func}.c"
    try:
        with open(p, errors="ignore") as f:
            return f.read(200).startswith("// NEARMISS")
    except OSError:
        return False

seen, out, dropped, dupes = set(), [], [], 0
for line in open(REG):
    if line.startswith("| ") and line.count("|") > 4:
        func = line.split("|")[1].strip()
        if func and func != "func" and not func.startswith("-") and not func.startswith("Function"):
            if func in seen:
                dupes += 1
                continue
            seen.add(func)
            if not is_nearmiss(func):
                dropped.append(func)
                continue
    out.append(line)

if "--check" in sys.argv:
    print(f"registry: {len(seen)} unique rows, {len(dropped)} stale, {dupes} duplicate")
    sys.exit(1 if (dropped or dupes) else 0)

open(REG, "w").writelines(out)
print(f"registry cleaned: kept {len(seen)-len(dropped)}, dropped {len(dropped)} stale, {dupes} duplicate")
if dropped:
    print("  stale (now matched or stubbed):", " ".join(dropped[:20]))
