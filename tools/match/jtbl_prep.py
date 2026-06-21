#!/usr/bin/env python3
"""Re-symbolize jump tables so m2c can decompile jtbl-dispatcher functions.

splat stores PS2 jump tables as raw `.word 0xADDR` blobs (it didn't recognise them as
jtbls), so m2c errors "jump table is not provided". This tool, for one function:
  - finds every jtbl_XXXX it references (lui %hi(jtbl_XXXX));
  - reads that jtbl's `.word 0x001xxxxx` code-address entries from wherever it's defined;
  - emits an m2c-feedable pair: the function .s with `.L<addr>:` labels inserted at each
    jtbl target, plus a .rodata block `glabel jtbl_XXXX / .word .L<addr> ...`.
Output goes to build/match/jtbl/<func>.{s,jtbl.s}; nothing canonical is touched (the real
splat .s / build are unchanged — this is m2c-input only).

Usage: jtbl_prep.py func_XXXX   (prints the m2c input paths, or NOJTBL / error)
"""
import os, re, sys, glob
os.chdir("/Users/abe/Documents/Extermination.nosync/Extermination")
A = "build/asm/matchings/main/code"
OUT = "build/match/jtbl"
os.makedirs(OUT, exist_ok=True)

VADDR = re.compile(r'/\*\s*[0-9A-Fa-f]+\s+([0-9A-Fa-f]{6,8})\s+[0-9A-Fa-f]+\s*\*/')
WORDADDR = re.compile(r'\.word\s+0x([0-9A-Fa-f]{6,8})')

def find_jtbl_data(sym):
    """Return list of target vaddrs (ints) for jtbl symbol `sym`."""
    for fp in glob.glob(f"{A}/*.s"):
        txt = open(fp, errors='ignore').read()
        if re.search(rf'(d|g)label\s+{sym}\b', txt) or f'{sym}:' in txt:
            lines = txt.splitlines()
            # find the label line
            for i, l in enumerate(lines):
                if re.search(rf'(d|g)label\s+{sym}\b', l) or l.strip().startswith(f'{sym}:'):
                    targets = []
                    for l2 in lines[i+1:]:
                        m = WORDADDR.search(l2)
                        if m:
                            v = int(m.group(1), 16)
                            if 0x100000 <= v < 0x300000:   # code-address range
                                targets.append(v); continue
                        # stop at terminator (0x0) / next label / non-.word
                        if '.word 0x00000000' in l2 or re.search(r'(d|g)label\s', l2) or (l2.strip().endswith(':') and 'jtbl' not in l2):
                            break
                        if '.word' not in l2 and l2.strip() and not l2.strip().startswith('/*'):
                            break
                    return targets
    return None

def main():
    func = sys.argv[1]
    sp = f"{A}/{func}.s"
    if not os.path.exists(sp):
        print("ERROR no .s"); return
    lines = open(sp, errors='ignore').read().splitlines()
    syms = sorted(set(re.findall(r'%hi\((jtbl_[0-9A-Fa-f]+)\)', "\n".join(lines))))
    if not syms:
        print("NOJTBL"); return
    # collect all target vaddrs across jtbls
    jtbls = {}
    all_targets = set()
    for s in syms:
        t = find_jtbl_data(s)
        if not t:
            print(f"ERROR jtbl data not found for {s}"); return
        jtbls[s] = t; all_targets.update(t)
    # insert .L<addr>: labels before the instruction at each target vaddr
    out = []
    for l in lines:
        m = VADDR.search(l)
        if m:
            v = int(m.group(1), 16)
            if v in all_targets and not any(o.strip() == f'.L{v:08X}:' for o in out[-1:]):
                out.append(f'.L{v:08X}:')
        out.append(l)
    func_s = f"{OUT}/{func}.s"
    open(func_s, "w").write("\n".join(out) + "\n")
    # emit the symbolized jtbl(s)
    jt = [".section .rodata"]
    for s, targets in jtbls.items():
        jt.append(f"glabel {s}")
        for v in targets:
            jt.append(f"    .word .L{v:08X}")
    jtbl_s = f"{OUT}/{func}.jtbl.s"
    open(jtbl_s, "w").write("\n".join(jt) + "\n")
    print(f"{func_s} {jtbl_s}")

if __name__ == "__main__":
    main()
