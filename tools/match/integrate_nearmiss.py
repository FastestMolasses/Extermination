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
import json, os, re, subprocess, sys

ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
os.chdir(ROOT)
A = "build/asm/matchings/main/code"
CC = {"mwcc": "tools/mwccps2/mwccmips.exe", "mwcc233": "tools/mwccps2-233/mwccps2.exe",
      "mwcc24": "tools/mwccps2-24/mwccps2.exe"}
COMPILER_LONG = {"eegcc": "ee-gcc 2.9-991111-01", "mwcc233": "mwcc 2.3.3 (mwcps2-2.3.3-000906)",
                 "mwcc": "mwcc 2.3 (mwcps2-2.3-991202)"}
NEARMISS_MIN = float(sys.argv[2]) if len(sys.argv) > 2 else 50.0

with open(sys.argv[1]) as fp:
    d = json.load(fp)
res = d.get("result", d) if isinstance(d, dict) else d
if isinstance(res, str):
    res = json.loads(res)
rows = res["results"] if isinstance(res, dict) else res
cand = [r for r in rows if (r.get("c_source") or "").strip()]
print(f"workflow: {len(rows)} funcs, {len(cand)} with c_source")
if not cand:
    sys.exit(0)

# One candidate per function: repeated entries would overwrite the original
# backup and measure the last source while reporting an earlier candidate.
names = [r["func"] for r in cand]
if len(set(names)) != len(names):
    sys.exit("duplicate function candidates; no files changed")

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
    with open(p, "rb") as fp:
        bak[f] = fp.read()

def restore_source(f):
    """Restore the exact prior source, including an existing NEARMISS header."""
    p = f"src/{f}.c"
    with open(p, "wb") as fp:
        fp.write(bak[f])
    with open(p, "rb") as fp:
        if fp.read() != bak[f]:
            raise RuntimeError(f"{f}: source backup restoration failed")
    for directory in ("obj", "expected"):
        path = f"build/{directory}/{f}.o"
        if os.path.exists(path):
            os.remove(path)

# The expected object MUST be built from the normalized .s, exactly as
# build.py does it: that applies the VU0 $ACC/$Q and invented-symbol fixups AND
# appends the function's own jump-table rodata. Assembling the raw splat .s here
# instead left jtbl_XXXX undefined-external, so a dispatcher that genuinely
# byte-matches measured ~99.9 and was filed as a NEARMISS. Always REBUILD it (no
# `[ -f ] ||` guard): a stale object from an earlier, un-normalized run is the
# exact trap this is fixing.
sys.path.insert(0, os.path.join(ROOT, "tools", "decomp"))
import build as _b  # noqa: E402

# Require fresh outputs and successful commands. A failed compiler can leave an
# old or partial .o behind; existence alone must never authorize promotion.
try:
    for r in cand:
        f = r["func"]
        for directory in ("obj", "expected"):
            os.makedirs(f"build/{directory}", exist_ok=True)
            path = f"build/{directory}/{f}.o"
            if os.path.exists(path):
                os.remove(path)
        with open(f"src/{f}.c", "w") as fp:
            fp.write(r["c_source"])

    # Batch per image, serially. Fail the batch if any command fails.
    img_parts = {"exterm-toolchain": [], "exterm-permuter": []}
    for r in cand:
        f = r["func"]; comp, fl = directives(r["c_source"])
        _b.normalize_asm(f)
        asm = (f'mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc '
               f'build/macro.inc build/.asmnorm/{f}.s -o build/expected/{f}.o 2>/dev/null')
        if comp == "eegcc":
            img_parts["exterm-permuter"] += [asm, f'tools/eegcc/ee-compile.sh src/{f}.c build/obj/{f}.o {fl} >/dev/null 2>&1']
        else:
            cc = CC.get(comp, CC["mwcc233"])
            img_parts["exterm-permuter"] += [asm]
            img_parts["exterm-toolchain"] += [f'qemu-i386 tools/bin/wibo32 {cc} -c {fl} -o build/obj/{f}.o src/{f}.c >/dev/null 2>&1']
    print("compiling...")
    for img, parts in img_parts.items():
        if not parts: continue
        subprocess.run(["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
                        img, "sh", "-c", "set -e; " + "; ".join(parts)],
                       capture_output=True, text=True, timeout=1200, check=True)
    for f in names:
        for directory in ("obj", "expected"):
            path = f"build/{directory}/{f}.o"
            if not os.path.isfile(path) or os.path.getsize(path) == 0:
                raise RuntimeError(f"{f}: missing fresh {directory} output")
except (Exception, KeyboardInterrupt):
    for f in names:
        restore_source(f)
    print(f"build failed; restored {len(names)} candidate source(s)")
    raise

def text_size(path):
    try:
        import struct
        with open(path, "rb") as fp:
            b = fp.read()
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
    try:
        r = subprocess.run(["tools/bin/objdiff-cli", "diff", "-1", e, "-2", o, f, "-o", "-", "--format", "json"],
                           capture_output=True, text=True)
    except OSError:
        return None
    if r.returncode != 0:
        return None
    try:
        for s in json.loads(r.stdout)["left"]["symbols"]:
            if s.get("name") == f: return s.get("match_percent")
    except Exception: return None
    return None

def nearmiss_file(f, c_source, p, comp, fl, wall):
    sz = "?"
    try:
        with open(f"{A}/{f}.s") as fp:
            m = re.search(r'0x([0-9A-Fa-f]+)', fp.readline())
        if m: sz = f"0x{int(m.group(1),16):X} bytes"
    except Exception: pass
    # Body = everything AFTER the last // COMPILER / // CFLAGS directive line. This
    # drops any agent-supplied // NEARMISS header (which precedes // COMPILER) so we
    # never double-wrap, while KEEPING a // SEMANTICS comment block that follows the
    # directives (valuable function documentation).
    lines = c_source.splitlines()
    start = 0
    for i, l in enumerate(lines):
        s = l.strip()
        if s.startswith("// COMPILER:") or s.startswith("// CFLAGS:"):
            start = i + 1
    body = "\n".join(lines[start:]).lstrip("\n")
    reason = re.sub(r'\s+', ' ', (wall or "compiler artifact (register coloring / scheduling)").strip())
    if len(reason) > 300: reason = reason[:297] + "..."
    cc = COMPILER_LONG.get(comp, comp)
    hdr = (f"// NEARMISS {f}  (vram 0x{f[5:]}, {sz}) — readable decompilation, NOT byte-identical.\n//\n"
           f"// objdiff {p:.2f}% via {cc} ({fl}). The LOGIC and STRUCTURE are faithful.\n"
           f"// Remaining differences in this candidate:\n// {reason}\n//\n"
           f"// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT\n"
           f"// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /\n"
           f"// excluded from matched_code. Registry: docs/NEARMISS.md.\n//\n"
           f"// COMPILER: {comp}\n// CFLAGS: {fl}\n\n")
    return hdr + body + ("\n" if not body.endswith("\n") else ""), sz, reason, cc

kept, near, reverted = [], [], []
docrows = []
try:
    for r in cand:
        f = r["func"]; comp, fl = directives(r["c_source"])
        p = pct(f)
        ts_o, ts_e = text_size(f"build/obj/{f}.o"), text_size(f"build/expected/{f}.o")
        oversize = (ts_o is not None and ts_e is not None and ts_o > ts_e)
        # GNU as pads the reference .text to its section alignment, while mwcc
        # can end it at the function boundary. Require known sizes and reject
        # overflow, but allow the reference's trailing alignment bytes.
        if p == 100.0 and ts_o is not None and ts_e is not None and 0 < ts_o <= ts_e:
            if comp != "eegcc":
                result = subprocess.run([sys.executable, "tools/decomp/inject_relocs.py", f], capture_output=True)
                p = pct(f) if result.returncode == 0 else None
            if p == 100.0:
                kept.append(f); print(f"  {f} -> 100.0 KEEP (match)"); continue
        # NOTE: oversize is IRRELEVANT for NEARMISS — the linker fills bytes from the
        # splat .s, never the (oversize) build/obj, so byte-identity holds regardless.
        # The oversize guard only matters for the 100%-match path above (where build/obj
        # IS linked). objdiff's % may be slightly inflated on an oversize obj (it ignores
        # extra trailing instrs); that only affects the documented %, not correctness.
        if p is not None and NEARMISS_MIN <= p < 100.0:
            content, sz, reason, cc = nearmiss_file(f, r["c_source"], p, comp, fl, r.get("wall", ""))
            with open(f"src/{f}.c", "w") as fp:
                fp.write(content)
            if os.path.exists(f"build/obj/{f}.o"): os.remove(f"build/obj/{f}.o")
            near.append(f); docrows.append(f"| {f} | 0x{f[5:]} | {sz.replace(' bytes','')} | {p:.2f}% | {cc} | {reason[:90]} |")
            print(f"  {f} -> {p:.2f} NEARMISS")
        else:
            reverted.append(f)
            restore_source(f)
            print(f"  {f} -> {p} REVERT (oversize={oversize})")
except (Exception, KeyboardInterrupt):
    # The whole wave remains provisional until every candidate is validated.
    # Roll back even candidates already reported KEEP/NEARMISS in this loop.
    for f in names:
        restore_source(f)
    print(f"validation failed; restored {len(names)} candidate source(s)")
    raise

if docrows:
    with open("docs/NEARMISS.md", "a") as fp:
        fp.write("\n".join(docrows) + "\n")
with open("/tmp/wave_pass.txt", "w") as fp:
    fp.write("\n".join(kept) + ("\n" if kept else ""))
with open("/tmp/wave_nearmiss.txt", "w") as fp:
    fp.write("\n".join(near) + ("\n" if near else ""))
print(f"\nKEPT {len(kept)} matches, {len(near)} NEARMISS, {len(reverted)} reverted")
