import sys, itertools
from collections import Counter, defaultdict
from pathlib import Path
REPO = Path(__file__).resolve().parents[3]  # decomp repo root
sys.path.insert(0, str(REPO.parent / 'extermination-port/tools'))
import ee_float_model as m
INDEF = 0xFFC00000
def q(x): return x | 0x00400000
def nan(x): return m._is_nan(x)
def inf(x): return m._is_inf(x)
def sse_add(x, y):
    x, y = m._daz(x), m._daz(y)
    if nan(x): return q(x)
    if nan(y): return q(y)
    if inf(x) and inf(y): return INDEF if (x ^ y) & m.SIGN else x
    if inf(x): return x
    if inf(y): return y
    return m._sum(x, y, False)
def sse_sub(x, y):
    x2, y2 = m._daz(x), m._daz(y)
    if nan(x2): return q(x2)
    if nan(y2): return q(y2)
    return sse_add(x2, y2 ^ m.SIGN)
def sse_mul(x, y):
    x, y = m._daz(x), m._daz(y)
    if nan(x): return q(x)
    if nan(y): return q(y)
    s = (x ^ y) & m.SIGN
    if inf(x) or inf(y):
        if m._is_zero(x) or m._is_zero(y): return INDEF
        return s | 0x7F800000
    return m._product(x, y)
def c_none(x): return x
def c_minmax(x):
    if nan(x): return m.MAX
    if inf(x): return (x & m.SIGN) | m.MAX
    return x
def c_sign(x):
    return (x & m.SIGN) | m.MAX if m._exp(x) == 255 else x
CL = {'none': c_none, 'minmax': c_minmax, 'sign': c_sign}

def variants(kind):
    for oc, rc, order in itertools.product(CL, CL, ('st', 'ts')):
        def f(s, t, acc=None, oc=oc, rc=rc, order=order):
            s, t = CL[oc](s), CL[oc](t)
            if kind == 'add': r = sse_add(s, t) if order == 'st' else sse_add(t, s)
            elif kind == 'sub': r = sse_sub(s, t)
            elif kind == 'mul': r = sse_mul(s, t) if order == 'st' else sse_mul(t, s)
            return CL[rc](r)
        yield (oc, rc, order), f
def mac_variants(sub):
    for oc, pc, ac, rc in itertools.product(CL, CL, CL, CL):
        def f(s, t, acc, oc=oc, pc=pc, ac=ac, rc=rc):
            p = CL[pc](sse_mul(CL[oc](s), CL[oc](t)))
            a = CL[ac](acc)
            r = sse_sub(a, p) if sub else sse_add(a, p)
            return CL[rc](r)
        yield (oc, pc, ac, rc), f

I = lambda op: m._inst(op)
def lanes(rec):
    """(s, t, acc, measured) per written lane."""
    op = rec['op']; i = I(op); out = []
    meas = m.measured(rec)
    for k in range(4):
        if not (i['dest'] >> (3 - k)) & 1: continue
        if op in ('vopmula', 'vopmsub'):
            s, t = rec['vs'][(1, 2, 0)[k]], rec['vt'][(2, 0, 1)[k]]
        elif op in ('vmulq', 'vaddq'):
            s = rec['vs'][k] if op == 'vmulq' else [0, 0, 0, 0x3F800000][k]; t = rec['q_in']
        elif op.endswith('bc'):
            s, t = rec['vs'][k], rec['vt'][i['bc']]
        else:
            s, t = rec['vs'][k], rec['vt'][k]
        acc = rec['acc_in'][k] if rec.get('acc_in') else None
        out.append((s, t, acc, meas[k]))
    return out
KIND = {'vadd': 'add', 'vaddbc': 'add', 'vaddq': 'add', 'vsub': 'sub', 'vsubbc': 'sub',
        'vmul': 'mul', 'vmulbc': 'mul', 'vmulq': 'mul', 'vmulabc': 'mul', 'vopmula': 'mul'}
MAC = {'vmaddbc': False, 'vmaddabc': False, 'vopmsub': True}
recs = [r for r in m.load_vectors() if r['op'] in KIND or r['op'] in MAC]
by = defaultdict(list)
for r in recs: by[r['op']].extend(lanes(r))
for op, ls in sorted(by.items()):
    vs = list(variants(KIND[op])) if op in KIND else list(mac_variants(MAC[op]))
    scores = []
    for key, f in vs:
        bad = sum(1 for s, t, acc, w in ls if f(s, t, acc) != w)
        scores.append((bad, key))
    scores.sort()
    best = [k for b, k in scores if b == scores[0][0]]
    print(op, len(ls), 'best bad', scores[0][0], 'n_best', len(best), best[:6])

if len(sys.argv) > 1:
    op = sys.argv[1]
    key = tuple(sys.argv[2].split(','))
    vs = dict(variants(KIND[op])) if op in KIND else dict(mac_variants(MAC[op]))
    f = vs[key]
    n = 0
    for s, t, acc, w in by[op]:
        g = f(s, t, acc)
        if g != w:
            n += 1
            if n <= 25: print(hex(s), hex(t), acc and hex(acc), 'meas', hex(w), 'model', hex(g))
