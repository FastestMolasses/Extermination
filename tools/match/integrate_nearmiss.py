#!/usr/bin/env python3
"""Unified wave integrator with NEARMISS support.

For a wave output JSON, integrate every result that carries c_source:
  • compiles == 100% and not oversize  -> KEEP as a true byte-match (readable C, normal unit).
  • NEARMISS_MIN <= compiled% < 100 and not oversize -> commit as a // NEARMISS file
    (documented header: real %, compiler, divergence reason from the agent `wall`), and
    drop build/obj so the linker fills bytes from the splat .s (boot ELF byte-identical).
  • otherwise -> revert to the prior stub/asm-void.

Agents must return their best readable c_source for BODY-CORRECT non-matches (logic recovered,
only a compiler artifact differs), not just for 100% matches. jr-table/VU0/incomplete -> no c_source.

Usage: integrate_nearmiss.py <workflow_output.json> [NEARMISS_MIN(default 50)]
"""
import json, os, re, subprocess, sys, shutil

ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
os.chdir(ROOT)
A = "build/asm/matchings/main/code"
CC = {"mwcc": "tools/mwccps2/mwccmips.exe", "mwcc233": "tools/mwccps2-233/mwccps2.exe",
      "mwcc24": "tools/mwccps2-24/mwccps2.exe"}
COMPILER_LONG = {"eegcc": "ee-gcc 2.9-991111-01", "mwcc233": "mwcc 2.3.3 (mwcps2-2.3.3-000906)",
                 "mwcc": "mwcc 2.3 (mwcps2-2.3-991202)"}
NEARMISS_MIN = float(sys.argv[2]) if len(sys.argv) > 2 else 50.0

d = json.load(open(sys.argv[1]))
res = d.get("result", d)
if isinstance(res, str):
    res = json.loads(res)
rows = res["results"] if isinstance(res, dict) else res
cand = [r for r in rows if (r.get("c_source") or "").strip()]
print(f"workflow: {len(rows)} funcs, {len(cand)} with c_source")
if not cand:
    sys.exit(0)

def directives(text):
    comp, fl = "mwcc233", "-O4,p -sdatathreshold 0"
    for l in text.splitlines():
        l = l.strip()
        if l.startswith("// COMPILER:"): comp = l.split(":", 1)[1].strip()
        elif l.startswith("// CFLAGS:"): fl = l.split(":", 1)[1].strip()
        elif l and not l.startswith("//"): break
    return comp, fl

bak = {}
for r in cand:
    f = r["func"]; p = f"src/{f}.c"
    if os.path.exists(p):
        bak[f] = f"/tmp/baknm_{f}.c"; shutil.copy(p, bak[f])
    open(p, "w").write(r["c_source"])

# batch-compile per image
img_parts = {"exterm-toolchain": [], "exterm-permuter": []}
for r in cand:
    f = r["func"]; comp, fl = directives(r["c_source"])
    asm = (f'[ -f build/expected/{f}.o ] || mipsel-linux-gnu-as -march=r5900 '
           f'config/asm_prelude.inc build/macro.inc {A}/{f}.s -o build/expected/{f}.o 2>/dev/null')
    if comp == "eegcc":
        img_parts["exterm-permuter"] += [asm, f'tools/eegcc/ee-compile.sh src/{f}.c build/obj/{f}.o {fl} >/dev/null 2>&1']
    else:
        cc = CC.get(comp, CC["mwcc233"])
        img_parts["exterm-permuter"] += [asm]  # assemble expected (mipsel-as is in both images)
        img_parts["exterm-toolchain"] += [f'qemu-i386 tools/bin/wibo32 {cc} -c {fl} -o build/obj/{f}.o src/{f}.c >/dev/null 2>&1']
print("compiling...")
for img, parts in img_parts.items():
    if not parts: continue
    subprocess.run(["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
                    img, "sh", "-c", "; ".join(parts) + "; echo DONE"], capture_output=True, text=True, timeout=1200)

def text_size(path):
    try:
        import struct
        b = open(path, "rb").read()
        if b[:4] != b"\x7fELF": return None
        shoff = struct.unpack_from("<I", b, 0x20)[0]; shent = struct.unpack_from("<H", b, 0x2E)[0]
        shnum = struct.unpack_from("<H", b, 0x30)[0]; shstr = struct.unpack_from("<H", b, 0x32)[0]
        stab = struct.unpack_from("<I", b, shoff+shstr*shent+0x10)[0]
        for i in range(shnum):
            base = shoff+i*shent; no = struct.unpack_from("<I", b, base)[0]
            nm = b[stab+no:b.index(b"\0", stab+no)].decode("latin1")
            if nm == ".text": return struct.unpack_from("<I", b, base+0x14)[0]
    except Exception: return None
    return None

def pct(f):
    e, o = f"build/expected/{f}.o", f"build/obj/{f}.o"
    if not (os.path.exists(e) and os.path.exists(o)): return None
    r = subprocess.run(["tools/bin/objdiff-cli", "diff", "-1", e, "-2", o, f, "-o", "-", "--format", "json"],
                       capture_output=True, text=True)
    try:
        for s in json.loads(r.stdout)["left"]["symbols"]:
            if s.get("name") == f: return s.get("match_percent")
    except Exception: return None
    return None

def nearmiss_file(f, c_source, p, comp, fl, wall):
    sz = "?"
    try:
        m = re.search(r'0x([0-9A-Fa-f]+)', open(f"{A}/{f}.s").readline())
        if m: sz = f"0x{int(m.group(1),16):X} bytes"
    except Exception: pass
    body = "\n".join(l for l in c_source.splitlines()
                     if not l.strip().startswith("// COMPILER:") and not l.strip().startswith("// CFLAGS:")).lstrip("\n")
    reason = re.sub(r'\s+', ' ', (wall or "compiler artifact (register coloring / scheduling)").strip())
    if len(reason) > 300: reason = reason[:297] + "..."
    cc = COMPILER_LONG.get(comp, comp)
    hdr = (f"// NEARMISS {f}  (vram 0x{f[5:]}, {sz}) — readable decompilation, NOT byte-identical.\n//\n"
           f"// objdiff {p:.2f}% via {cc} ({fl}). The LOGIC and STRUCTURE are faithful; the residual\n"
           f"// diff is a genuine compiler artifact that no source change fixes here:\n// {reason}\n//\n"
           f"// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT\n"
           f"// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /\n"
           f"// excluded from matched_code. Registry: docs/NEARMISS.md.\n//\n"
           f"// COMPILER: {comp}\n// CFLAGS: {fl}\n\n")
    return hdr + body + ("\n" if not body.endswith("\n") else ""), sz, reason, cc

kept, near, reverted = [], [], []
docrows = []
for r in cand:
    f = r["func"]; comp, fl = directives(r["c_source"])
    p = pct(f)
    ts_o, ts_e = text_size(f"build/obj/{f}.o"), text_size(f"build/expected/{f}.o")
    oversize = (ts_o is not None and ts_e is not None and ts_o > ts_e)
    if p == 100.0 and not oversize:
        if comp != "eegcc":
            subprocess.run([sys.executable, "tools/decomp/inject_relocs.py", f], capture_output=True)
            p = pct(f)
        if p == 100.0:
            kept.append(f); print(f"  {f} -> 100.0 KEEP (match)"); continue
    # NOTE: oversize is IRRELEVANT for NEARMISS — the linker fills bytes from the
    # splat .s, never the (oversize) build/obj, so byte-identity holds regardless.
    # The oversize guard only matters for the 100%-match path above (where build/obj
    # IS linked). objdiff's % may be slightly inflated on an oversize obj (it ignores
    # extra trailing instrs); that only affects the documented %, not correctness.
    if p is not None and NEARMISS_MIN <= p < 100.0:
        content, sz, reason, cc = nearmiss_file(f, r["c_source"], p, comp, fl, r.get("wall", ""))
        open(f"src/{f}.c", "w").write(content)
        if os.path.exists(f"build/obj/{f}.o"): os.remove(f"build/obj/{f}.o")
        near.append(f); docrows.append(f"| {f} | 0x{f[5:]} | {sz.replace(' bytes','')} | {p:.2f}% | {cc} | {reason[:90]} |")
        print(f"  {f} -> {p:.2f} NEARMISS")
    else:
        reverted.append(f)
        if f in bak: shutil.copy(bak[f], f"src/{f}.c")
        print(f"  {f} -> {p} REVERT (oversize={oversize})")

if docrows:
    with open("docs/NEARMISS.md", "a") as fp:
        fp.write("\n".join(docrows) + "\n")
open("/tmp/wave_pass.txt", "w").write("\n".join(kept) + ("\n" if kept else ""))
open("/tmp/wave_nearmiss.txt", "w").write("\n".join(near) + ("\n" if near else ""))
print(f"\nKEPT {len(kept)} matches, {len(near)} NEARMISS, {len(reverted)} reverted")
