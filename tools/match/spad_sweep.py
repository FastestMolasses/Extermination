#!/usr/bin/env python3
"""Measure idiom-32 (scratchpad literal vs relocated extern) per file and per address.

usage: spad_sweep.py OUTDIR FUNC [FUNC ...]
       spad_sweep.py OUTDIR @LIST.json          (a JSON list of function names)

Measurement only: src/ is never written. For every function whose C
dereferences `*(T *)0x7000xxxx` literals, it compiles variants that spell a
chosen set of those addresses as `extern T D_7000xxxx;` (optionally
`volatile`), opted in with a `// SPAD:` line, and scores each against an
expected object whose scratchpad symbolization follows THAT variant's text
(build.py's own _symbolize_scratchpad, with build.SRC pointed at the variant).
That is the same measurement integrate_nearmiss.py makes after promotion.

Sets tried: none; every subset when a file has at most 8 such addresses;
otherwise all, each single and all-but-one, then up to three greedy rounds that
toggle one address at a time from the best set. Plain and volatile externs are
both tried. Variant sources, objects and a results JSON go under OUTDIR (use an
ignored build/<task>/ folder and delete it when done). Addresses the file
already names as D_7000xxxx, or dereferences at more than one type, are left
alone.

Why per address (decomp lane round 6, 2026-10-20): the original is not
uniform. In func_0019BA80 the clears of 0x700031AC / 0x7000319C only match as
literal stores (as externs mwcc reorders them), while 0x70003194 / 0x700031D8
only match as externs; func_0019B4C0 wants all of them as externs. Runs in the
exterm-permuter container (x86_64 Linux) through the Apple `container` CLI.
"""
import itertools
import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "decomp"))
import build as B  # noqa: E402

EXE = {"mwcc": "tools/mwccps2/mwccmips.exe", "991202": "tools/mwccps2/mwccmips.exe",
       "mwcc233": "tools/mwccps2-233/mwccps2.exe", "mwcc24": "tools/mwccps2-24/mwccps2.exe",
       "mwcc30": "tools/mwccps2-30/mwccps2.exe", "mwcc301": "tools/mwccps2-301/mwccps2.exe"}
PAT = re.compile(r"\*\((?P<t>(?:volatile\s+)?(?:unsigned\s+|signed\s+)?"
                 r"(?:char|short|int|float|long)\s*)\*\)\s*0x(?P<a>7000[0-9A-Fa-f]{4})\b")
ALL_SUBSETS_MAX = 8


def directives(text):
    comp, fl = "991202", "-O4,p"
    for line in text.splitlines():
        s = line.strip()
        if s.startswith("// COMPILER:"):
            comp = s.split(":", 1)[1].strip()
        elif s.startswith("// CFLAGS:"):
            fl = s.split(":", 1)[1].strip()
        elif s and not s.startswith("//"):
            break
    return comp, fl


def literal_addresses(text):
    """Addresses dereferenced as literals at exactly one type and not yet named."""
    code = re.sub(r"//[^\n]*|/\*.*?\*/", " ", text, flags=re.S)
    types = {}
    for m in PAT.finditer(code):
        types.setdefault(m.group("a").upper(), set()).add(re.sub(r"\s+", " ", m.group("t").strip()))
    named = {x.upper() for x in re.findall(r"\bD_(7000[0-9A-Fa-f]{4})\b", code)}
    addrs = sorted(a for a, t in types.items() if len(t) == 1 and a not in named)
    return addrs, {a: next(iter(t)) for a, t in types.items() if a in addrs}


def variant(text, types, sel, vol):
    out = PAT.sub(lambda m: f"D_{m.group('a').upper()}" if m.group("a").upper() in sel else m.group(0), text)
    if not sel:
        return out
    decl = []
    for a in sel:
        ty = types[a]
        if vol and not ty.startswith("volatile"):
            ty = "volatile " + ty
        decl.append(f"extern {ty} D_{a};")
    lines = out.split("\n")
    i = 0
    while i < len(lines) and (lines[i].startswith("//") or not lines[i].strip()):
        i += 1
    lines.insert(i, "\n".join(decl))
    out = "\n".join(lines)
    spad = "// SPAD: " + " ".join("0x" + a for a in sel) + "\n"
    out, n = re.subn(r"(// CFLAGS:[^\n]*\n)", lambda m: m.group(1) + spad, out, count=1)
    if not n:
        raise SystemExit("no // CFLAGS: line to anchor the // SPAD: directive")
    return out


def tag_of(f, sel, vol):
    return f"{f}__{'v' if vol else 'p'}_{'_'.join(a[4:] for a in sel) or 'none'}"


def expected_asm(f, text, out, tag):
    """build.normalize_asm, with the scratchpad opt-in read from `text`."""
    src = (ROOT / B.ASM_DIR / f"{f}.s").read_text(errors="ignore")
    for pat, rep in B._ASM_FIXUPS:
        src = pat.sub(rep, src)
    src = B._symbolize_hoisted_hi(src)
    view = out / "view" / tag
    view.mkdir(parents=True, exist_ok=True)
    (view / f"{f}.c").write_text(text)
    old = B.SRC
    B.SRC = view
    try:
        src = B._symbolize_scratchpad(f, src)
    finally:
        B.SRC = old
    jt = ROOT / "build" / "jtblrodata" / f"{f}.s"
    if jt.exists():
        src += "\n" + jt.read_text(errors="ignore")
    (out / "asm" / f"{tag}.s").write_text(src)


def score(out, jobs):
    """jobs: [(func, tag, text)] -> {tag: (match_percent, target_size, ours_size) or None}"""
    rel = out.relative_to(ROOT)
    for sub in ("src", "asm", "exp", "obj"):
        (out / sub).mkdir(parents=True, exist_ok=True)
    asm_cmds, cc_cmds = [], []
    for f, tag, text in jobs:
        (out / "src" / f"{tag}.c").write_text(text)
        expected_asm(f, text, out, tag)
        comp, fl = directives(text)
        if comp not in EXE:
            raise SystemExit(f"{f}: compiler {comp!r} is not an mwcc build")
        for p in (out / "exp" / f"{tag}.o", out / "obj" / f"{tag}.o"):
            if p.exists():
                p.unlink()
        asm_cmds.append(f"mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc "
                        f"{rel}/asm/{tag}.s -o {rel}/exp/{tag}.o 2>/dev/null")
        cc_cmds.append(f"qemu-i386 tools/bin/wibo32 {EXE[comp]} -c {fl} -o {rel}/obj/{tag}.o "
                       f"{rel}/src/{tag}.c >/dev/null 2>&1")
    (out / "asm_cmds.txt").write_text("\n".join(asm_cmds) + "\n")
    (out / "cc_cmds.txt").write_text("\n".join(cc_cmds) + "\n")
    subprocess.run(["container", "run", "--rm", "--cpus", "8", "-v", f"{ROOT}:/work", "-w", "/work",
                    "exterm-permuter", "sh", "-c",
                    f"xargs -d '\\n' -P 8 -n 1 sh -c < {rel}/asm_cmds.txt; "
                    f"xargs -d '\\n' -P 8 -n 1 sh -c < {rel}/cc_cmds.txt"],
                   cwd=ROOT, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)
    res = {}
    for f, tag, _ in jobs:
        e, o = out / "exp" / f"{tag}.o", out / "obj" / f"{tag}.o"
        if not (e.exists() and o.exists()):
            res[tag] = None
            continue
        p = subprocess.run([str(ROOT / "tools/bin/objdiff-cli"), "diff", "-1", str(e), "-2", str(o), f,
                            "-o", "-", "--format", "json"], capture_output=True, text=True, cwd=ROOT)
        try:
            d = json.loads(p.stdout)
            left = [s for s in d["left"]["symbols"] if s["name"] == f][0]
            right = [s for s in d.get("right", {}).get("symbols", []) if s["name"] == f]
            res[tag] = (left.get("match_percent") or 0.0, int(left["size"]),
                        int(right[0]["size"]) if right else None)
        except (ValueError, KeyError, IndexError):
            res[tag] = None
    return res


def rank(v):
    """A size mismatch never outranks an equal-size result of the same score."""
    if not v:
        return -1.0
    return v[0] - (0.0 if v[1] == v[2] else 1e-4)


def main():
    if len(sys.argv) < 3:
        raise SystemExit(__doc__)
    out = Path(sys.argv[1]).resolve()
    if ROOT / "build" not in out.parents:
        raise SystemExit("OUTDIR must be inside build/")
    funcs = sys.argv[2:]
    if len(funcs) == 1 and funcs[0].startswith("@"):
        funcs = json.load(open(funcs[0][1:]))
    state, jobs = {}, []
    for f in funcs:
        text = (ROOT / "src" / f"{f}.c").read_text()
        addrs, types = literal_addresses(text)
        if not addrs:
            continue
        state[f] = (text, addrs, types)
        if len(addrs) <= ALL_SUBSETS_MAX:
            sets = [s for k in range(len(addrs) + 1) for s in itertools.combinations(addrs, k)]
        else:
            sets = [(), tuple(addrs)] + [(a,) for a in addrs] + [tuple(x for x in addrs if x != a) for a in addrs]
        for sel in dict.fromkeys(sets):
            for vol in ((False, True) if sel else (False,)):
                jobs.append((f, tag_of(f, sel, vol), variant(text, types, sel, vol), sel, vol))
    print(f"{len(state)} functions, {len(jobs)} variants", flush=True)
    res = score(out, [(f, t, x) for f, t, x, _, _ in jobs])
    best = {}
    for f, t, _, sel, vol in jobs:
        if f not in best or rank(res.get(t)) > rank(res.get(best[f][0])):
            best[f] = (t, sel, vol)
    for _ in range(3):
        more = []
        for f, (t, sel, vol) in best.items():
            text, addrs, types = state[f]
            if len(addrs) <= ALL_SUBSETS_MAX or rank(res.get(t)) >= 100.0:
                continue
            for a in addrs:
                s2 = tuple(sorted(set(sel) ^ {a}))
                t2 = tag_of(f, s2, vol)
                if s2 and t2 not in res:
                    more.append((f, t2, variant(text, types, s2, vol), s2, vol))
        if not more:
            break
        print(f"greedy round: {len(more)} variants", flush=True)
        res.update(score(out, [(f, t, x) for f, t, x, _, _ in more]))
        moved = False
        for f, t, _, sel, vol in more:
            if rank(res.get(t)) > rank(res.get(best[f][0])) + 1e-9:
                best[f], moved = (t, sel, vol), True
        if not moved:
            break
    summary = {f: {"baseline": res.get(tag_of(f, (), False)), "best_tag": best[f][0],
                   "best": res.get(best[f][0]), "spad": ["0x" + a for a in best[f][1]],
                   "volatile": best[f][2]} for f in state}
    (out / "spad_sweep.json").write_text(json.dumps(summary, indent=1))
    for f, s in sorted(summary.items(), key=lambda kv: -rank(kv[1]["best"])):
        print(f, s["baseline"], "->", s["best"], " ".join(s["spad"]) or "-", "volatile" if s["volatile"] else "")


if __name__ == "__main__":
    main()
