#!/usr/bin/env python3
"""Find one address per FPU/VU0-macro op in the original boot ELF (addresses only)."""
import json, struct, sys
from pathlib import Path
REPO = Path(__file__).resolve().parents[2]  # decomp repo root
ELF = (REPO / 'config/SCUS_971.12').read_bytes()
BASE_OFF, BASE_VA, END = 0x300, 0x100000, 0x175e00
S = {0:'add.s',1:'sub.s',2:'mul.s',3:'div.s',4:'sqrt.s',5:'abs.s',6:'mov.s',7:'neg.s',0x16:'rsqrt.s',
     0x18:'adda.s',0x19:'suba.s',0x1a:'mula.s',0x1c:'madd.s',0x1d:'msub.s',0x1e:'madda.s',0x1f:'msuba.s',
     0x24:'cvt.w.s',0x28:'max.s',0x29:'min.s',0x32:'c.eq.s',0x34:'c.lt.s',0x36:'c.le.s'}
SP1 = {0x1c:'vmulq',0x1e:'vmuli',0x20:'vaddq',0x22:'vaddi',0x24:'vsubq',0x28:'vadd',0x29:'vmadd',0x2a:'vmul',
       0x2b:'vmax',0x2c:'vsub',0x2d:'vmsub',0x2e:'vopmsub',0x2f:'vmini',0x21:'vmaddq',0x25:'vmsubq'}
for i,n in enumerate(['vaddbc','vsubbc','vmaddbc','vmsubbc','vmaxbc','vminibc']):
    for b in range(4): SP1[i*4+b] = n
for b in range(4): SP1[0x18+b] = 'vmulbc'
SP2 = {0x28:'vadda',0x29:'vmadda',0x2a:'vmula',0x2c:'vsuba',0x2d:'vmsuba',0x2e:'vopmula',0x38:'vdiv',0x39:'vsqrt',
       0x3a:'vrsqrt',0x1d:'vabs',0x10:'vitof0',0x11:'vitof4',0x12:'vitof12',0x13:'vitof15',0x14:'vftoi0',0x15:'vftoi4',
       0x16:'vftoi12',0x17:'vftoi15',0x1c:'vmulaq',0x21:'vmaddaq',0x25:'vmsubaq',0x1e:'vmulai'}
for b in range(4): SP2[0x00+b]='vaddabc'; SP2[0x04+b]='vsubabc'; SP2[0x08+b]='vmaddabc'; SP2[0x0c+b]='vmsubabc'; SP2[0x18+b]='vmulabc'
found = {}
for off in range(BASE_OFF, END, 4):
    w = struct.unpack_from('<I', ELF, off)[0]; va = off - BASE_OFF + BASE_VA
    op = w >> 26; name = None
    if op == 0x11:
        rs = (w >> 21) & 31; fn = w & 63
        if rs == 0x10 and fn in S: name = S[fn]
        if rs == 0x14 and fn == 0x20: name = 'cvt.s.w'
    elif op == 0x12 and (w >> 25) & 1:
        fn = w & 63
        if fn < 0x3c: name = SP1.get(fn)
        else: name = SP2.get((fn & 3) | (((w >> 6) & 31) << 2))
    if name:
        found.setdefault(name, []).append(va)
if len(sys.argv) == 1: json.dump({k: [hex(a) for a in v[:8]] + [len(v)] for k, v in sorted(found.items())}, sys.stdout, indent=0)

import csv
FUNCS = []
for row in csv.DictReader(open(REPO / 'docs/FUNCTIONS.csv')):
    size = int(row['size_bytes'])
    if size > 4 and row['subsystem'] != 'data':
        FUNCS.append((int(row['vram'], 16), size, row['name']))
FUNCS.sort()
import bisect
def owner(va):
    i = bisect.bisect_right(FUNCS, (va, 1 << 40)) - 1
    if i >= 0 and FUNCS[i][0] <= va < FUNCS[i][0] + FUNCS[i][1]:
        return FUNCS[i]
    return None

QI = ('vaddq','vmulq','vsubq','vmaddq','vmsubq','vmaddaq','vmsubaq','vmulaq','vmuli','vaddi','vsubi','vmulai')
def pick():
    """Choose one usable instance per op: FPU fs != ft; VU fs/ft != vf0, dest xyzw when possible."""
    chosen = {}
    for name, addrs in found.items():
        best = None
        for va in addrs:
            if owner(va) is None: continue
            w = struct.unpack_from('<I', ELF, va - BASE_VA + BASE_OFF)[0]
            fd, fs, ft = (w >> 6) & 31, (w >> 11) & 31, (w >> 16) & 31
            if name.startswith('v'):
                dest = (w >> 21) & 15
                if name in ('vdiv', 'vsqrt', 'vrsqrt'):
                    ok = (name == 'vsqrt' or fs != 0) and ft != 0 and fs != ft
                    score = 1 if ok else -1
                else:
                    ok = fs != 0 and (ft != 0 or name.startswith(('vftoi', 'vitof', 'vabs')) or name in QI) and (fs != ft or name.startswith(('vftoi', 'vitof', 'vabs')) or name in QI)
                    score = (2 if dest == 15 else 1) if ok else -1
                    if ok and name.startswith(('vftoi','vitof','vabs')) and dest != 15: score = 1
            else:
                score = 2 if fs != ft else 0
            if best is None or score > best[0]:
                best = (score, va)
        if best: chosen[name] = best + (owner(best[1])[2],)
    return chosen

if __name__ == '__main__' and len(sys.argv) > 1 and sys.argv[1] == 'pick':
    print(json.dumps({k: [v[0], hex(v[1]), v[2]] for k, v in sorted(pick().items())}))
