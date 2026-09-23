import sys, json, itertools
from collections import defaultdict, Counter
from pathlib import Path
REPO = Path(__file__).resolve().parents[3]  # decomp repo root
sys.path.insert(0, str(REPO.parent / 'extermination-port/tools'))
import ee_float_model as m
exec(open(Path(__file__).resolve().parent / 'vufit.py').read().split("def variants")[0].split("import ee_float_model as m")[1])
recs = [json.loads(l) for l in open(REPO / 'build/startup-reference/ee_float/vectors/vu_signatures.jsonl')]
KIND = {'vadd': 'add', 'vaddbc': 'add', 'vaddq': 'add', 'vsub': 'sub', 'vsubbc': 'sub',
        'vmul': 'mul', 'vmulbc': 'mul', 'vmulq': 'mul', 'vmulabc': 'mul', 'vopmula': 'mul',
        'vmaddbc': 'madd', 'vmaddabc': 'madd', 'vopmsub': 'msub', 'vmaxbc': 'max', 'vminibc': 'min'}
OPS = (1, 2, 0), (2, 0, 1)
def lane_inputs(r):
    op = r['op']; out = []
    meas = r['acc'] if 'acc' in r and r['op'] in ('vmulabc', 'vmaddabc', 'vopmula') else r['vd']
    for k in range(4):
        written = (r['dest'] >> (3 - k)) & 1
        if not written:
            continue
        if op in ('vopmula', 'vopmsub'):
            s, t = r['vs'][OPS[0][k]], r['vt'][OPS[1][k]]
        elif op in ('vmulq', 'vaddq'):
            s, t = r['vs'][k], r['q_in']
        elif op.endswith('bc'):
            s, t = r['vs'][k], r['vt'][r['bc']]
        else:
            s, t = r['vs'][k], r['vt'][k]
        acc = r['acc_in'][k] if r.get('acc_in') else None
        out.append((s, t, acc, meas[k]))
    return out
def unwritten_ok(r):
    op = r['op']
    if op in ('vmulabc', 'vmaddabc', 'vopmula'):
        return all(r['acc'][k] == r['acc_in'][k] for k in range(4) if not (r['dest'] >> (3 - k)) & 1)
    return all(r['vd'][k] == r['prior'][k] for k in range(4) if not (r['dest'] >> (3 - k)) & 1)
C = {0: lambda x: x, 1: c_minmax}
def lane_fn(kind, cs, ct, ca, order):
    def f(s, t, acc):
        s2, t2 = C[cs](s), C[ct](t)
        if kind == 'add': return sse_add(s2, t2) if order == 0 else sse_add(t2, s2)
        if kind == 'sub': return sse_sub(s2, t2)
        if kind == 'mul': return sse_mul(s2, t2) if order == 0 else sse_mul(t2, s2)
        p = sse_mul(s2, t2); a = C[ca](acc)
        if kind == 'madd': return sse_add(a, p) if order == 0 else sse_add(p, a)
        return sse_sub(a, p)
    return f
def key_raw(x): return -(x & 0x7fffffff) if x >> 31 else x
def mm_fn(kind, tie):
    def f(s, t, acc):
        ks, kt = key_raw(s), key_raw(t)
        if ks == kt: return s if tie == 0 else t
        return s if (ks > kt) == (kind == 'max') else t
    return f
by = defaultdict(list)
for r in recs:
    if r['op'] in KIND:
        by[(r['op'], tuple(r['sig']))].append(r)
table = {}
for sig, rs in sorted(by.items(), key=str):
    op = sig[0]; kind = KIND[op]
    uw = sum(not unwritten_ok(r) for r in rs)
    lanes = [x for r in rs for x in lane_inputs(r)]
    res = []
    if kind in ('max', 'min'):
        for tie in (0, 1):
            f = mm_fn(kind, tie); res.append((sum(f(s, t, a) != w for s, t, a, w in lanes), ('tie', tie)))
    else:
        for cs, ct, ca, order in itertools.product((0, 1), (0, 1), (0, 1) if kind in ('madd', 'msub') else (0,), (0, 1)):
            f = lane_fn(kind, cs, ct, ca, order)
            res.append((sum(f(s, t, a) != w for s, t, a, w in lanes), (cs, ct, ca, order)))
    res.sort()
    best = [k for b, k in res if b == res[0][0]]
    table[sig] = best
    print(sig, 'lanes', len(lanes), 'unwritten-lane faults', uw, 'bad', res[0][0], 'best', best)

# ---- determined flags + instance coverage
import struct
ELF = open(REPO / 'config/SCUS_971.12', 'rb').read()
src = open(REPO / 'tools/ee_float/scan.py').read(); ns = {}
exec(src.split("if len(sys.argv) == 1")[0], ns)
exec("import csv" + src.split("import csv")[1].split("QI =")[0], ns)
def fields(va):
    w = struct.unpack_from('<I', ELF, va - 0x100000 + 0x300)[0]
    return (w >> 11) & 31, (w >> 16) & 31, (w >> 6) & 31
measured_va = {}
for r in recs: measured_va[(r['op'], tuple(r['sig']))] = r['va']
out = {}
for sig, best in sorted(table.items(), key=str):
    op = sig[0]
    if KIND[op] in ('max', 'min'):
        continue
    flags = []
    for i in range(3):
        vals = {b[i] for b in best}
        flags.append(vals.pop() if len(vals) == 1 else None)
    order = {b[3] for b in best}; order = order.pop() if len(order) == 1 else None
    if KIND[op] not in ('madd', 'msub'): flags[2] = 0
    insts = [va for va in ns['found'][op] if ns['owner'](va) and
             (lambda w: ((w >> 21) & 15, (w & 3) if op.endswith('bc') else None))(struct.unpack_from('<I', ELF, va - 0x100000 + 0x300)[0]) == sig[1]]
    mfs, mft, _ = fields(measured_va[sig])
    pattern = lambda fs, ft: (fs == 0, ft == 0, fs == ft)
    differing = [hex(va) for va in insts if pattern(*fields(va)[:2]) != pattern(mfs, mft)]
    out['%s/%d/%s' % (op, sig[1][0], sig[1][1])] = {'clamp_fs': flags[0], 'clamp_ft': flags[1], 'clamp_acc': flags[2],
        'nan_order': order, 'instances': len(insts), 'measured': hex(measured_va[sig]),
        'pattern': pattern(mfs, mft), 'other_patterns': differing}
json.dump(out, open(REPO / 'build/startup-reference/ee_float/vectors/vu_signature_fit.json', 'w'), indent=1)
for k, v in out.items():
    print(k, v['clamp_fs'], v['clamp_ft'], v['clamp_acc'], v['nan_order'], v['instances'], v['pattern'], v['other_patterns'][:4])
