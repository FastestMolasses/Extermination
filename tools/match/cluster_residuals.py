#!/usr/bin/env python3
"""Cluster every // NEARMISS by its RESIDUAL SIGNATURE to find matchable families.

WHY (s86). Grinding near-misses one at a time yields 2-4 matches per 16-function
batch. The step changes come from FAMILY cracks: idiom-29 was one tie-break that
improved seven functions at once and byte-matched one of them. That family was
found by accident, from a wave agent's corpus comparison. This finds them on
purpose.

For each near-miss it aligns the expected and compiled objects, reduces every
differing row to a token describing the KIND of divergence (register permutation,
mnemonic swap, a delay slot the target left as nop, an extra/missing
instruction), and clusters functions by their dominant token. A large cluster of
functions sharing one residual kind is a candidate family: crack the tie-break
once, apply it everywhere.

Functions whose residual is a SINGLE token kind are ranked first — they are the
cleanest experiments, with nothing else confounding the measurement.

Usage: cluster_residuals.py [--refresh] [--min-cluster N]
  --refresh   rebuild stale objects first (src/<f>.c newer than its object)
"""
import json, os, re, subprocess, sys, collections
from pathlib import Path

ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
os.chdir(ROOT)
sys.path.insert(0, os.path.join(ROOT, "tools", "decomp"))
import build as B

P = "build/spadmeasure"
REFRESH = "--refresh" in sys.argv
MIN = 3
if "--min-cluster" in sys.argv:
    MIN = int(sys.argv[sys.argv.index("--min-cluster") + 1])

REGS = (r"\b(zero|at|v[01]|a[0-3]|t\d|s[0-7]|k[01]|gp|sp|fp|ra|"
        r"f\d+|vf\d+|vi\d+)\b")


def nearmiss():
    out = []
    for p in sorted(Path("src").glob("*.c")):
        if B.is_asm_stub(p) and p.read_text(errors="ignore")[:200].lstrip().startswith("// NEARMISS"):
            out.append(p.stem)
    return out


def mnem(s):
    return (s or "").split(None, 1)[0] if s else None


def skel(s):
    return re.sub(REGS, "R", s or "")


def token(l, r):
    """Reduce one differing row to a signature token."""
    if l is None:
        return "EXTRA:" + (mnem(r) or "?")
    if r is None:
        return "MISSING:" + (mnem(l) or "?")
    lm, rm = mnem(l), mnem(r)
    if lm == "nop" and rm != "nop":
        return "SLOTFILL:" + rm          # target left the slot empty; we filled it
    if rm == "nop" and lm != "nop":
        return "SLOTEMPTY:" + lm         # target filled it; we did not
    if lm == rm:
        return ("REG:" + lm) if skel(l) == skel(r) else ("OPND:" + lm)
    return "MNEM:%s->%s" % (lm, rm)


def rows(name):
    """Signature tokens for `name`, using OBJDIFF'S OWN row markers.

    Do NOT diff the two instruction lists by index. objdiff already aligns them
    and tags each divergent row with a `diff_kind`; a naive index compare treats
    one inserted instruction as a cascade of mismatches for the whole remainder
    of the function. That inflation is visible on sight — it reported 23
    differing rows for a function measuring 99.99%, which would be ~96%.
    """
    o1, o2 = f"{P}/{name}.exp.o", f"{P}/{name}.o"
    if not (os.path.exists(o1) and os.path.exists(o2)):
        return None
    r = subprocess.run(["tools/bin/objdiff-cli", "diff", "-1", o1, "-2", o2, name,
                        "-o", "-", "--format", "json"], capture_output=True, text=True)
    try:
        d = json.loads(r.stdout)
    except Exception:
        return None

    def side(which):
        for s in d[which]["symbols"]:
            if s["name"] == name:
                return s
        return None

    sl, sr = side("left"), side("right")
    if sl is None or sr is None:
        return None
    L, R = sl["instructions"], sr["instructions"]
    toks = []
    for i, row in enumerate(L):
        kind = row.get("diff_kind")
        if not kind or kind == "DIFF_NONE":
            continue
        lf = (row.get("instruction") or {}).get("formatted")
        rf = ((R[i].get("instruction") or {}).get("formatted")) if i < len(R) else None
        toks.append(kind.replace("DIFF_", "") + "|" + token(lf, rf))
    return toks, sl["match_percent"]


def refresh(names):
    stale = [n for n in names
             if not os.path.exists(f"{P}/{n}.o")
             or os.path.getmtime(f"src/{n}.c") > os.path.getmtime(f"{P}/{n}.o")]
    if not stale:
        print("[refresh] all objects current")
        return
    print(f"[refresh] rebuilding {len(stale)} stale object(s)")
    cmds = []
    for f in stale:
        B.normalize_asm(f)
        cc = B.compile_cmd(f).replace(f"build/obj/{f}.o", f"{P}/{f}.o")
        cmds.append(f"mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc "
                    f"build/.asmnorm/{f}.s -o {P}/{f}.exp.o 2>/dev/null; {cc} 2>/dev/null")
    for i in range(0, len(cmds), 12):
        B.container(" ; ".join(cmds[i:i + 12]))


names = nearmiss()
os.makedirs(P, exist_ok=True)
if REFRESH:
    refresh(names)

by_tok = collections.defaultdict(list)
single = collections.defaultdict(list)
missing = 0
for n in names:
    got = rows(n)
    if not got:
        missing += 1
        continue
    toks, pct = got
    if not toks:
        continue
    c = collections.Counter(toks)
    dom, dn = c.most_common(1)[0]
    by_tok[dom].append((n, pct, len(toks)))
    if len(c) == 1:
        single[dom].append((n, pct, len(toks)))

print(f"\nnear-misses: {len(names)}   analysed: {len(names) - missing}   no object: {missing}")
print("\n=== clusters by DOMINANT residual kind (>= %d members) ===" % MIN)
for tok, fs in sorted(by_tok.items(), key=lambda kv: -len(kv[1])):
    if len(fs) < MIN:
        continue
    pure = len(single.get(tok, []))
    fs.sort(key=lambda r: -(r[1] or 0))
    print(f"\n{tok:28} {len(fs):4} functions   ({pure} with NO other residual kind)")
    for n, pct, k in fs[:6]:
        print(f"      {n:22} {pct:8.4f}  ({k} rows)")

print("\n=== best FAMILY candidates: pure single-kind residuals, ranked by size ===")
for tok, fs in sorted(single.items(), key=lambda kv: -len(kv[1])):
    if len(fs) < MIN:
        continue
    fs.sort(key=lambda r: (r[2], -(r[1] or 0)))
    ex = fs[0]
    print(f"{tok:28} {len(fs):4} funcs   cleanest exemplar: {ex[0]} "
          f"({ex[1]:.4f}, {ex[2]} row{'s' if ex[2] != 1 else ''})")

json.dump({t: [f[0] for f in v] for t, v in single.items()},
          open(f"{P}/families.json", "w"), indent=1)
print(f"\nwrote {P}/families.json")
