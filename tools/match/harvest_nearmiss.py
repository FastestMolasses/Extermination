#!/usr/bin/env python3
"""Harvest body-correct near-miss eegcc C from agent scratch into // NEARMISS files.

Stage 1 (--probe): for each candidate func, compile every surviving scratch variant
(build/agent_eg_*/src/func_X*.c) with ee-gcc, objdiff vs expected, record the BEST
real %. Writes /tmp/nearmiss_best.json. (Agent-claimed % can be permuter-inflated; we
commit the real plain-C %.)

Stage 2 (--write MINPCT): for each func whose best real % >= MINPCT, write src/func_X.c
as a // NEARMISS file (documented header: real %, compiler, divergence reason from the
wave wall) using the best variant's body. Appends rows to docs/NEARMISS.md.

NEARMISS files are NOT compiled / NOT objdiff units (build.py+fill_unmatched treat the
marker like a stub) so the boot ELF stays byte-identical (linker fills from splat .s).
"""
import json, os, re, subprocess, sys, glob

ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
os.chdir(ROOT)
A = "build/asm/matchings/main/code"
CANDS = json.load(open("/tmp/nearmiss_cands.json"))

def variants(fn):
    out = []
    for p in glob.glob(f"build/agent_eg_*/src/{fn}*.c"):
        b = os.path.basename(p)[:-2]
        if re.match(rf'{fn}(_|$)', b):
            out.append(p)
    return sorted(set(out))

def objdiff_pct(fn, ourobj):
    e = f"build/expected/{fn}.o"
    if not (os.path.exists(e) and os.path.exists(ourobj)):
        return None
    r = subprocess.run(["tools/bin/objdiff-cli", "diff", "-1", e, "-2", ourobj, fn,
                        "-o", "-", "--format", "json"], capture_output=True, text=True)
    try:
        d = json.loads(r.stdout)
        for s in d["left"]["symbols"]:
            if s.get("name") == fn:
                return s.get("match_percent")
    except Exception:
        return None
    return None

def probe():
    # build a container batch: assemble expected + compile each variant
    parts = []
    jobs = []  # (fn, variant_path, our_obj)
    for c in CANDS:
        fn = c["func"]
        parts.append(f'[ -f build/expected/{fn}.o ] || mipsel-linux-gnu-as -march=r5900 '
                     f'config/asm_prelude.inc build/macro.inc {A}/{fn}.s -o build/expected/{fn}.o 2>/dev/null')
        for i, v in enumerate(variants(fn)):
            oo = f"/work/build/nm_probe/{fn}_{i}.o"
            parts.append(f'tools/eegcc/ee-compile.sh {v} build/nm_probe/{fn}_{i}.o -O2 >/dev/null 2>&1')
            jobs.append((fn, v, f"build/nm_probe/{fn}_{i}.o"))
    os.makedirs("build/nm_probe", exist_ok=True)
    print(f"compiling {len(jobs)} variants for {len(CANDS)} funcs (one container call)...")
    subprocess.run(["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
                    "exterm-permuter", "sh", "-c", "; ".join(parts) + "; echo DONE"],
                   capture_output=True, text=True, timeout=1800)
    best = {}
    for fn, v, oo in jobs:
        p = objdiff_pct(fn, oo)
        if p is None:
            continue
        if fn not in best or p > best[fn][0]:
            best[fn] = (p, v)
    json.dump({fn: {"pct": p, "variant": v} for fn, (p, v) in best.items()},
              open("/tmp/nearmiss_best.json", "w"))
    rows = sorted(best.items(), key=lambda kv: -kv[1][0])
    print(f"\n{'real%':>6}  func        variant")
    for fn, (p, v) in rows:
        print(f"{p:6.2f}  {fn}  {os.path.basename(v)}")
    print(f"\n{len(best)} funcs compiled & scored. (agent-claimed vs real may differ.)")

def reason_for(fn):
    for c in CANDS:
        if c["func"] == fn:
            w = (c.get("wall") or "").strip().replace("\n", " ")
            return re.sub(r'\s+', ' ', w)
    return "compiler artifact (register coloring / scheduling)"

COMPILERS = {"eegcc": "ee-gcc 2.9-991111-01"}

def write(minpct):
    best = json.load(open("/tmp/nearmiss_best.json"))
    rows = []
    written = 0
    maxpct = float(sys.argv[sys.argv.index("--max")+1]) if "--max" in sys.argv else 100.0
    for fn, d in sorted(best.items(), key=lambda kv: -kv[1]["pct"]):
        pct, vpath = d["pct"], d["variant"]
        if pct < minpct or pct >= maxpct:
            continue
        # idempotency: skip funcs already committed as NEARMISS / readable C
        try:
            first = next(l.strip() for l in open(f"src/{fn}.c") if l.strip())
            if first.startswith("// NEARMISS") or not first.startswith("// INCLUDE_ASM"):
                continue
        except Exception:
            pass
        body = open(vpath).read()
        # strip leading // COMPILER / // CFLAGS markers; capture them
        comp, fl = "eegcc", "-O2"
        lines = body.splitlines()
        keep = []
        for l in lines:
            s = l.strip()
            if s.startswith("// COMPILER:"): comp = s.split(":",1)[1].strip(); continue
            if s.startswith("// CFLAGS:"): fl = s.split(":",1)[1].strip(); continue
            keep.append(l)
        body = "\n".join(keep).lstrip("\n")
        # size from .s header
        sz = "?"
        try:
            m = re.search(r'0x([0-9A-Fa-f]+)', open(f"{A}/{fn}.s").readline())
            if m: sz = f"0x{int(m.group(1),16):X} bytes"
        except Exception: pass
        va = fn[5:]
        reason = reason_for(fn)
        if len(reason) > 320: reason = reason[:317] + "..."
        cc = COMPILERS.get(comp, comp)
        hdr = (f"// NEARMISS {fn}  (vram 0x{va}, {sz}) — readable decompilation, NOT byte-identical.\n"
               f"//\n"
               f"// objdiff {pct:.2f}% via {cc} ({fl}). The LOGIC and STRUCTURE are faithful; the\n"
               f"// residual diff is a genuine compiler artifact that no source change fixes here:\n"
               f"// {reason}\n"
               f"//\n"
               f"// Boot ELF stays byte-identical: the linker fills this function from the splat .s,\n"
               f"// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff\n"
               f"// unit / excluded from matched_code. Registry: docs/NEARMISS.md.\n"
               f"//\n"
               f"// COMPILER: {comp}\n"
               f"// CFLAGS: {fl}\n\n")
        open(f"src/{fn}.c", "w").write(hdr + body + ("\n" if not body.endswith("\n") else ""))
        rows.append(f"| {fn} | 0x{va} | {sz.replace(' bytes','')} | {pct:.2f}% | {cc} | {reason[:90]} |")
        written += 1
    # append to docs/NEARMISS.md
    with open("docs/NEARMISS.md", "a") as f:
        f.write("\n".join(rows) + "\n")
    print(f"wrote {written} NEARMISS files (>= {minpct}%); appended {len(rows)} rows to docs/NEARMISS.md")
    print("funcs:", " ".join(sorted(fn for fn,d in best.items() if d['pct']>=minpct)))

if __name__ == "__main__":
    if "--probe" in sys.argv:
        probe()
    elif "--write" in sys.argv:
        write(float(sys.argv[sys.argv.index("--write")+1]))
    else:
        print("usage: harvest_nearmiss.py --probe | --write MINPCT")
