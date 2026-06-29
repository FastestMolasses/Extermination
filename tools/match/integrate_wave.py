#!/usr/bin/env python3
"""Hardened wave integrator: parse a mwcc233_wave workflow output, integrate the
matched funcs, verify each at canonical objdiff 100%, keep only true matches.

ONE container call for all compiles (avoids the rapid-fire `container run` daemon
wedge). inject_relocs + objdiff run host-side in this single process (avoids the
background shell-loop spawn stalls).

Usage: integrate_wave.py <workflow_output.json>
Prints per-func result; writes the kept (100%) func list to /tmp/wave_pass.txt.
Reverts src for any func that does not reach 100% (keeps the prior stub/asm-void).
"""
import json, os, re, subprocess, sys, shutil
ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
os.chdir(ROOT)
A = "build/asm/matchings/main/code"
CC = {"mwcc": "tools/mwccps2/mwccmips.exe", "mwcc233": "tools/mwccps2-233/mwccps2.exe",
      "mwcc24": "tools/mwccps2-24/mwccps2.exe", "mwcc30": "tools/mwccps2-30/mwccps2.exe",
      "mwcc301": "tools/mwccps2-301/mwccps2.exe"}

d = json.load(open(sys.argv[1]))
res = d.get("result", d)
if isinstance(res, str):
    res = json.loads(res)
rows = res["results"] if isinstance(res, dict) else res
matched = [r for r in rows if r.get("matched") and r.get("c_source", "").strip()]
print(f"workflow: {len(rows)} funcs, {len(matched)} claimed matched")
if not matched:
    open("/tmp/wave_pass.txt", "w").write("")
    sys.exit(0)

# 1. back up + write candidate src
bak = {}
for r in matched:
    f = r["func"]; p = f"src/{f}.c"
    if os.path.exists(p):
        bak[f] = f"/tmp/bakwave_{f}.c"; shutil.copy(p, bak[f])
    open(p, "w").write(r["c_source"])

# 2. one container call: assemble missing expected + compile each
def directives(f):
    comp, fl = "mwcc", "-O4,p -sdatathreshold 0"
    for l in open(f"src/{f}.c"):
        l = l.strip()
        if l.startswith("// COMPILER:"): comp = l.split(":", 1)[1].strip()
        elif l.startswith("// CFLAGS:"): fl = l.split(":", 1)[1].strip()
        elif l and not l.startswith("//"): break
    return comp, fl
# eegcc compiles need ee-compile.sh's i386 libs (exterm-permuter image); mwcc needs
# exterm-toolchain. Route per-image; run a container call per image actually used.
img_parts = {"exterm-toolchain": [], "exterm-permuter": []}
for r in matched:
    f = r["func"]; comp, fl = directives(f)
    cc = CC.get(comp, CC["mwcc"])
    asm = (f'[ -f build/expected/{f}.o ] || mipsel-linux-gnu-as -march=r5900 '
           f'config/asm_prelude.inc build/macro.inc {A}/{f}.s -o build/expected/{f}.o 2>/dev/null')
    if comp == "eegcc":
        img_parts["exterm-permuter"] += [asm, f'tools/eegcc/ee-compile.sh src/{f}.c build/obj/{f}.o {fl} >/dev/null 2>&1']
    else:
        img_parts["exterm-toolchain"] += [asm, f'qemu-i386 tools/bin/wibo32 {cc} -c {fl} -o build/obj/{f}.o src/{f}.c >/dev/null 2>&1']
print("compiling (one container call per image)...")
for img, parts in img_parts.items():
    if not parts: continue
    subprocess.run(["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
                    img, "sh", "-c", "; ".join(parts) + "; echo BATCH_DONE"], capture_output=True, text=True, timeout=900)

# 3. inject relocs (host) + objdiff (host)
def text_size(path):
    """ELF32-LE .text section size, host-side (no toolchain). objdiff scores the
    EXPECTED instructions and ignores EXTRA trailing compiled instrs, so a func
    that compiles oversized can false-positive at 100% (e.g. func_001DEE80 s84:
    0x6c vs 0x34 -> +0x2c link shift broke boot-ELF byte-identity). A size==size
    guard catches that class."""
    try:
        d = open(path, "rb").read()
        if d[:4] != b"\x7fELF": return None
        import struct
        shoff = struct.unpack_from("<I", d, 0x20)[0]
        shentsize = struct.unpack_from("<H", d, 0x2E)[0]
        shnum = struct.unpack_from("<H", d, 0x30)[0]
        shstrndx = struct.unpack_from("<H", d, 0x32)[0]
        strtab_off = struct.unpack_from("<I", d, shoff + shstrndx*shentsize + 0x10)[0]
        for i in range(shnum):
            base = shoff + i*shentsize
            name_off = struct.unpack_from("<I", d, base)[0]
            name = d[strtab_off+name_off:d.index(b"\0", strtab_off+name_off)].decode("latin1")
            if name == ".text":
                return struct.unpack_from("<I", d, base + 0x14)[0]
    except Exception:
        return None
    return None

def pct(f):
    e, o = f"build/expected/{f}.o", f"build/obj/{f}.o"
    if not (os.path.exists(e) and os.path.exists(o)): return None
    r = subprocess.run(["tools/bin/objdiff-cli", "diff", "-1", e, "-2", o, f, "-o", "-", "--format", "json"],
                       capture_output=True, text=True)
    try:
        dd = json.loads(r.stdout)
        for s in dd["left"]["symbols"]:
            if s.get("name") == f: return s.get("match_percent")
    except Exception:
        return None
    return None
kept = []
for r in matched:
    f = r["func"]; comp, _ = directives(f)
    if comp != "eegcc":
        subprocess.run([sys.executable, "tools/decomp/inject_relocs.py", f], capture_output=True)
    p = pct(f)
    # Reject only OVERSIZE: objdiff scores the expected instrs and ignores EXTRA
    # trailing compiled instrs, so compiled>expected can false-positive (func_001DEE80
    # s84: 0x6c vs 0x40 -> link shift). Undersize (compiled<=expected) is just
    # alignment/gap padding that fill_unmatched pads back, and objdiff already
    # catches missing real instructions as <100%, so it's safe.
    ts_o, ts_e = text_size(f"build/obj/{f}.o"), text_size(f"build/expected/{f}.o")
    oversize = (ts_o is not None and ts_e is not None and ts_o > ts_e)
    ok = p == 100.0 and not oversize
    note = f" OVERSIZE text={ts_o:#x} vs expected={ts_e:#x}" if oversize else ""
    print(f"  {f} -> {p} {'KEEP' if ok else 'REVERT'}{note}")
    if ok:
        kept.append(f)
    elif f in bak:
        shutil.copy(bak[f], f"src/{f}.c")  # restore prior stub/asm-void
    elif f in [m['func'] for m in matched]:
        pass
open("/tmp/wave_pass.txt", "w").write("\n".join(kept) + ("\n" if kept else ""))
print(f"KEPT {len(kept)}/{len(matched)} at canonical 100%")
