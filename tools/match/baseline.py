#!/usr/bin/env python3
"""m2c -> mwcc -> objdiff baseline harness (PROTOTYPE).

For each func: run m2c (host) on the splat .s, prepend the exterm prelude, compile
with mwccmips (container), assemble the splat target (container), objdiff (host).
Records baseline match% so we can triage which parks are worth feeding to the permuter.

Scratch only: writes build/match/{src,obj,exp}. Never touches canonical build/obj|expected.
"""
import json, os, subprocess, sys, shlex

ROOT = "/Users/abe/Documents/Extermination.nosync/Extermination"
A = "build/asm/matchings/main/code"
PROTO = "build/match"
PRELUDE = "tools/m2c/exterm_prelude.h"
DEFAULT_FLAGS = "-O4,p -sdatathreshold 0"

os.chdir(ROOT)
for d in ("src", "obj", "exp"):
    os.makedirs(f"{PROTO}/{d}", exist_ok=True)

def m2c(func):
    sp = f"{A}/{func}.s"
    if not os.path.exists(sp):
        return None
    cmd = [".venv/bin/python3", "tools/m2c/m2c.py", "--target", "mipsee-mwcc-c", "--valid-syntax"]
    if os.path.exists(f"{PROTO}/context.c"):
        cmd += ["--context", f"{PROTO}/context.c"]
    cmd.append(sp)
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
    except subprocess.TimeoutExpired:
        return None
    if r.returncode != 0 or not r.stdout.strip():
        return None
    sys.path.insert(0, "tools/match")
    from m2c_clean import clean
    return clean(r.stdout)

def write_src(func, body, flags):
    prelude = open(PRELUDE).read()
    with open(f"{PROTO}/src/{func}.c", "w") as f:
        f.write(f"// CFLAGS: {flags}\n")
        f.write(prelude)
        f.write("\n")
        f.write(body)

def container_build(funcs, flags):
    # write a batch script to disk and run it in ONE container (robust vs huge -c args)
    listfile = f"{PROTO}/funcs.lst"
    with open(listfile, "w") as f:
        f.write("\n".join(funcs) + "\n")
    batch = f"""#!/bin/sh
A={A}; P={PROTO}; FLAGS="{flags}"
while read fn; do
  [ -z "$fn" ] && continue
  timeout 60 mipsel-linux-gnu-as -march=r5900 config/asm_prelude.inc build/macro.inc \
      "$A/$fn.s" -o "$P/exp/$fn.o" 2>/dev/null
  timeout 60 qemu-i386 tools/bin/wibo32 tools/mwccps2/mwccmips.exe -c $FLAGS \
      -o "$P/obj/$fn.o" "$P/src/$fn.c" >/dev/null 2>>"$P/cc_err.txt" \
      || echo "CCFAIL:$fn" >>"$P/cc_err.txt"
done < "$P/funcs.lst"
echo BATCH_DONE
"""
    with open(f"{PROTO}/batch.sh", "w") as f:
        f.write(batch)
    r = subprocess.run(["container", "run", "--rm", "-v", f"{ROOT}:/work", "-w", "/work",
                        "exterm-toolchain", "sh", f"{PROTO}/batch.sh"],
                       capture_output=True, text=True)
    with open(f"{PROTO}/baseline_log.txt", "w") as f:
        f.write("STDOUT:\n" + r.stdout + "\nSTDERR:\n" + r.stderr)

def objdiff(func):
    o1 = f"{PROTO}/exp/{func}.o"
    o2 = f"{PROTO}/obj/{func}.o"
    if not (os.path.exists(o1) and os.path.exists(o2)):
        return None
    r = subprocess.run(["tools/bin/objdiff-cli", "diff", "-1", o1, "-2", o2, func,
                        "-o", "-", "--format", "json"], capture_output=True, text=True)
    try:
        d = json.loads(r.stdout)
        for s in d["left"]["symbols"]:
            if s["name"] == func:
                return s["match_percent"]
    except Exception:
        return None
    return None

def main():
    funcs = [x for x in sys.argv[1:] if x.startswith("func_")]
    flags = DEFAULT_FLAGS
    open(f"{PROTO}/cc_err.txt", "w").close()
    ok, m2c_fail = [], []
    for fn in funcs:
        body = m2c(fn)
        if body is None:
            m2c_fail.append(fn); continue
        write_src(fn, body, flags)
        ok.append(fn)
    if ok:
        container_build(ok, flags)
    results = []
    cc_err = open(f"{PROTO}/cc_err.txt").read()
    ccfail = set(l.split(":")[1] for l in cc_err.splitlines() if l.startswith("CCFAIL:"))
    for fn in ok:
        if fn in ccfail or not os.path.exists(f"{PROTO}/obj/{fn}.o"):
            results.append((fn, "CCFAIL")); continue
        pct = objdiff(fn)
        results.append((fn, pct if pct is not None else "NODIFF"))
    for fn in m2c_fail:
        results.append((fn, "M2CFAIL"))
    # sort: numeric desc, then failures
    def key(r):
        return r[1] if isinstance(r[1], float) else -1
    results.sort(key=key, reverse=True)
    for fn, pct in results:
        print(f"{pct if not isinstance(pct,float) else f'{pct:6.2f}'}  {fn}")

if __name__ == "__main__":
    main()
