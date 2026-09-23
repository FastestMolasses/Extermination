#!/usr/bin/env python3
"""Record original FPU / VU0-macro results from PCSX2 (user's saved configuration).

Usage: battery.py [phase ...]   -> writes build/startup-reference/ee_float/vectors/<phase>.jsonl
Each record: op, instruction address, input bits, measured output bits.
"""
import json, random, struct, sys, time
from harness import *

OUT = DATA / 'vectors'
OUT.mkdir(parents=True, exist_ok=True)

FPU = {'add.s': 0x102a7c, 'sub.s': 0x102a88, 'mul.s': 0x102ec4, 'div.s': 0x102f14,
       'madd.s': 0x154658, 'mula.s': 0x154650, 'msub.s': 0x169524, 'suba.s': 0x169520,
       'adda.s': 0x1286d0, 'cvt.w.s': 0x11c94c, 'cvt.s.w': 0x11c958, 'neg.s': 0x102eac,
       'mov.s': 0x102da8, 'c.eq.s': 0x10d72c, 'c.lt.s': 0x102a64, 'c.le.s': 0x11d0e0}
VU = {'vadd': 0x1028c0, 'vsub': 0x1028d8, 'vmul': 0x1028f0, 'vmulbc': 0x10290c,
      'vaddbc': 0x102a14, 'vsubbc': 0x1cf9c0, 'vmaddbc': 0x1026c0, 'vmulabc': 0x1026b4,
      'vmaddabc': 0x1026b8, 'vopmula': 0x102720, 'vopmsub': 0x102724, 'vmaxbc': 0x102cbc,
      'vminibc': 0x102cc0, 'vabs': 0x1cf9c8, 'vftoi0': 0x102994, 'vftoi4': 0x102984,
      'vitof0': 0x1029b4, 'vitof4': 0x1029a4, 'vdiv': 0x1cfa04, 'vsqrt': 0x102770,
      'vmulq': 0x102864, 'vaddq': 0x1cfa0c}


def fields(va):
    w = word(va)
    return {'fd': (w >> 6) & 31, 'fs': (w >> 11) & 31, 'ft': (w >> 16) & 31,
            'dest': (w >> 21) & 15, 'bc': w & 3, 'fsf': (w >> 21) & 3, 'ftf': (w >> 23) & 3}


SPECIALS = [0x00000000, 0x80000000, 0x00000001, 0x007fffff, 0x80400000, 0x00800000, 0x80800001,
            0x3f800000, 0xbf800000, 0x3fffffff, 0x7f7fffff, 0xff7fffff, 0x7f000000, 0xfeffffff,
            0x7f800000, 0xff800000, 0x7f800001, 0x7fc00000, 0xffc00000, 0x7fffffff, 0xffffffff,
            0x00ffffff, 0x01000000, 0x0c000000, 0x72800000, 0x4b000000, 0xcb7fffff]
PATTERNS = [0, 0x7fffff, 0x400000, 1, 0x555555, 0x2aaaaa, 0x7ffffe, 0x400001, 0x000003, 0x7ffff0]
R = random.Random(20260923)


def mk(sign, exp, mant):
    return (sign << 31) | (exp << 23) | mant


def mant(k):
    return PATTERNS[k] if k < len(PATTERNS) else R.getrandbits(23)


def add_pairs():
    """Exponent difference 0..45, all sign combos, both orders, boundary exponents."""
    out = []
    for d in range(46):
        for sa in (0, 1):
            for sb in (0, 1):
                for k in range(6):
                    ea = R.randint(1 + d, 254) if k >= 4 else R.randint(max(40, 1 + d), 200)
                    a = mk(sa, ea, mant(R.randrange(12)))
                    b = mk(sb, ea - d, mant(R.randrange(12)))
                    out.append((a, b) if k % 2 else (b, a))
    for ea in (1, 2, 3, 24, 25, 26, 252, 253, 254):
        for _ in range(12):
            d = R.randrange(0, 4)
            eb = min(254, max(1, ea - d))
            out.append((mk(R.getrandbits(1), ea, R.getrandbits(23)), mk(R.getrandbits(1), eb, R.getrandbits(23))))
    return out


def mul_pairs(div=False):
    out = []
    for _ in range(900):
        ea, eb = R.randint(64, 190), R.randint(64, 190)
        out.append((mk(R.getrandbits(1), ea, mant(R.randrange(14))), mk(R.getrandbits(1), eb, mant(R.randrange(14)))))
    for target in list(range(250, 258)) + list(range(-4, 4)):
        for _ in range(25):
            ea = R.randint(max(1, target - 126), min(254, target + 126)) if not div else R.randint(1, 254)
            eb = (target + 127 - ea) if not div else (ea - target + 127)
            if not 1 <= eb <= 254:
                continue
            out.append((mk(R.getrandbits(1), ea, R.getrandbits(23)), mk(R.getrandbits(1), eb, R.getrandbits(23))))
    return out


def special_pairs():
    return [(a, b) for a in SPECIALS for b in SPECIALS]


def random_pairs(n):
    return [(R.getrandbits(32), R.getrandbits(32)) for _ in range(n)]


def fpu_binary(rig, op, pairs):
    va = FPU[op]; f = fields(va)
    recs = []
    for a, b in pairs:
        (fpr,) = rig.do([W('FPR', f['fs'], a), W('FPR', f['ft'], b), *STEP(va), READ('FPR')])
        recs.append({'op': op, 'va': va, 'fs': a, 'ft': b, 'fd': fpr[f['fd']][0]})
    return recs


def acc_chain(rig, op, acc, a, b):
    """ACC <- mula(acc, 1.0) then op(a, b); returns fd."""
    ma = fields(FPU['mula.s']); f = fields(FPU[op])
    (fpr,) = rig.do([W('FPR', ma['fs'], acc), W('FPR', ma['ft'], 0x3f800000), *STEP(FPU['mula.s']),
                     W('FPR', f['fs'], a), W('FPR', f['ft'], b), *STEP(FPU[op]), READ('FPR')])
    return fpr[f['fd']][0]


def acc_writer(rig, op, a, b):
    """op (adda/suba/mula) then msub(+0,+0) readback: fd = ACC - (+0)."""
    f = fields(FPU[op]); ms = fields(FPU['msub.s'])
    (fpr,) = rig.do([W('FPR', f['fs'], a), W('FPR', f['ft'], b), *STEP(FPU[op]),
                     W('FPR', ms['fs'], 0), W('FPR', ms['ft'], 0), *STEP(FPU['msub.s']), READ('FPR')])
    return fpr[ms['fd']][0]


def madd_triples():
    out = []
    for d in range(-32, 33):
        for _ in range(16):
            acc = mk(R.getrandbits(1), R.randint(70, 180), R.getrandbits(23))
            pe = ((acc >> 23) & 255) - d                     # product exponent target
            ea = R.randint(max(1, pe - 60), min(254, pe + 60)); eb = pe + 127 - ea
            if not 1 <= eb <= 254:
                ea = 127; eb = pe
            if not 1 <= eb <= 254:
                continue
            out.append((acc, mk(R.getrandbits(1), ea, mant(R.randrange(14))), mk(R.getrandbits(1), eb, mant(R.randrange(14)))))
    sp = [0x00000000, 0x80000000, 0x00000001, 0x3f800000, 0xbf800000, 0x7f7fffff, 0xff7fffff, 0x7f800000, 0xff800000, 0x7fc00000, 0xffffffff, 0x00800000]
    for acc in sp:
        for a in sp[::2]:
            for b in (0x3f800000, 0x7f000000, 0x00800001, 0x7f800000, 0x80000000):
                out.append((acc, a, b))
    return out


def phase_fpu_arith(rig):
    recs = []
    for op in ('add.s', 'sub.s'):
        recs += fpu_binary(rig, op, add_pairs() + special_pairs() + random_pairs(300))
    recs += fpu_binary(rig, 'mul.s', mul_pairs() + special_pairs() + random_pairs(300))
    recs += fpu_binary(rig, 'div.s', mul_pairs(div=True) + special_pairs() + random_pairs(300))
    return recs


def phase_fpu_acc(rig):
    recs = []
    for op in ('madd.s', 'msub.s'):
        for acc, a, b in madd_triples():
            recs.append({'op': op, 'va': FPU[op], 'chain': 'mula1', 'acc': acc, 'fs': a, 'ft': b,
                         'fd': acc_chain(rig, op, acc, a, b)})
    for op in ('adda.s', 'suba.s', 'mula.s'):
        pairs = (add_pairs()[::4] if op != 'mula.s' else mul_pairs()[::3]) + special_pairs()[::3] + random_pairs(100)
        for a, b in pairs:
            recs.append({'op': op, 'va': FPU[op], 'chain': 'msub0', 'fs': a, 'ft': b,
                         'fd': acc_writer(rig, op, a, b)})
    return recs


def phase_fpu_misc(rig):
    recs = []
    f = fields(FPU['cvt.w.s'])
    vals = list(SPECIALS)
    for e in range(118, 160):
        for k in range(8):
            vals.append(mk(R.getrandbits(1), e, mant(R.randrange(14))))
    vals += [0x4effffff, 0x4f000000, 0xcf000000, 0xcf000001, 0x4f000001, 0xbf000000, 0x3f000000, 0xbf7fffff]
    for v in vals:
        (fpr,) = rig.do([W('FPR', f['fs'], v), *STEP(FPU['cvt.w.s']), READ('FPR')])
        recs.append({'op': 'cvt.w.s', 'va': FPU['cvt.w.s'], 'fs': v, 'fd': fpr[f['fd']][0]})
    f = fields(FPU['cvt.s.w'])
    ints = [0, 1, 0xffffffff, 0x7fffffff, 0x80000000, 0x80000001, 0x00ffffff, 0x01000000, 0x01000001, 0x01000003,
            0xfeffffff, 0xff000001]
    for sh in range(0, 31):
        for _ in range(8):
            v = R.getrandbits(sh + 1) | (1 << sh)
            ints.append(v if R.getrandbits(1) else (-v) & 0xffffffff)
    for v in ints:
        (fpr,) = rig.do([W('FPR', f['fs'], v), *STEP(FPU['cvt.s.w']), READ('FPR')])
        recs.append({'op': 'cvt.s.w', 'va': FPU['cvt.s.w'], 'fs': v, 'fd': fpr[f['fd']][0]})
    for op in ('neg.s', 'mov.s'):
        f = fields(FPU[op])
        for v in SPECIALS + [R.getrandbits(32) for _ in range(20)]:
            (fpr,) = rig.do([W('FPR', f['fs'], v), *STEP(FPU[op]), READ('FPR')])
            recs.append({'op': op, 'va': FPU[op], 'fs': v, 'fd': fpr[f['fd']][0]})
    for op in ('c.eq.s', 'c.lt.s', 'c.le.s'):
        f = fields(FPU[op])
        pairs = special_pairs() + [(a, a ^ R.choice((0, 1, 0x80000000))) for a, _ in random_pairs(60)] + random_pairs(60)
        for a, b in pairs:
            fpr, fcr = rig.do([W('FCR', 31, 0x01000001 if R.getrandbits(1) else 0x01800001),
                               W('FPR', f['fs'], a), W('FPR', f['ft'], b), *STEP(FPU[op]), READ('FPR'), READ('FCR')])
            recs.append({'op': op, 'va': FPU[op], 'fs': a, 'ft': b, 'c': (fcr[31][0] >> 23) & 1})
    return recs


def lanes4(pairs):
    out = []
    for i in range(0, len(pairs) - 3, 4):
        chunk = pairs[i:i + 4]
        out.append(([p[0] for p in chunk], [p[1] for p in chunk]))
    return out


def vu_step(rig, op, s, t, acc=None, q=None, read_q=False):
    va = VU[op]; f = fields(va)
    special2 = op in ('vmulabc', 'vmaddabc', 'vopmula', 'vabs', 'vftoi0', 'vftoi4', 'vitof0', 'vitof4')
    dst = f['ft'] if op in ('vabs', 'vftoi0', 'vftoi4', 'vitof0', 'vitof4') else f['fd']
    cmds = []
    if f['fs'] != 0: cmds.append(W('VU0F', f['fs'], s))
    if t is not None and f['ft'] != 0 and dst != f['ft']: cmds.append(W('VU0F', f['ft'], t))
    if acc is not None: cmds.append(W('VU0F', 32, acc))
    if q is not None: cmds.append(W('VU0I', 22, q))
    sentinel = [0x5a5a5a5a, 0x5a5a5a5a, 0x5a5a5a5a, 0x5a5a5a5a]
    wrote_sentinel = op not in ('vdiv', 'vsqrt') and dst not in (f['fs'], f['ft']) and not op in ('vmulabc', 'vmaddabc', 'vopmula')
    if wrote_sentinel: cmds.append(W('VU0F', dst, sentinel))
    cmds += STEP(va)
    cmds.append(READ('VU0F'))
    if read_q: cmds.append(READ('VU0I'))
    reads = rig.do(cmds)
    vf = reads[0]
    rec = {'op': op, 'va': va, 'vs': s, 'vt': t, 'acc_in': acc, 'q_in': q,
           'vd': vf[dst] if op not in ('vdiv', 'vsqrt', 'vmulabc', 'vmaddabc', 'vopmula') else None,
           'acc': vf[32], 'prior': sentinel if wrote_sentinel else None}
    if read_q: rec['q'] = reads[1][22][0]
    return rec


def phase_vu(rig):
    recs = []
    base = add_pairs() + special_pairs() + random_pairs(200)
    mulp = mul_pairs() + special_pairs() + random_pairs(200)
    for op, pairs in (('vadd', base), ('vsub', base), ('vmul', mulp)):
        for s, t in lanes4(pairs):
            recs.append(vu_step(rig, op, s, t))
    for op, pairs in (('vaddbc', base[::3]), ('vsubbc', base[::3]), ('vmulbc', mulp[::3]),
                      ('vmaxbc', special_pairs() + random_pairs(200)), ('vminibc', special_pairs() + random_pairs(200))):
        for s, t in lanes4(pairs):
            recs.append(vu_step(rig, op, s, t))
    tri = madd_triples()
    for op in ('vmaddbc', 'vmaddabc', 'vmulabc', 'vopmula', 'vopmsub'):
        for i in range(0, len(tri) - 3, 4):
            ch = tri[i:i + 4]
            recs.append(vu_step(rig, op, [c[1] for c in ch], [c[2] for c in ch], acc=[c[0] for c in ch]))
    vals = SPECIALS + [R.getrandbits(32) for _ in range(120)]
    for e in range(118, 165):
        for k in range(4):
            vals.append(mk(R.getrandbits(1), e, mant(R.randrange(14))))
    for op in ('vabs', 'vftoi0', 'vftoi4'):
        for i in range(0, len(vals) - 3, 4):
            recs.append(vu_step(rig, op, vals[i:i + 4], None))
    ints = [0, 1, 0xffffffff, 0x7fffffff, 0x80000000, 0x01000001, 0xfeffffff] + [R.getrandbits(32) for _ in range(200)]
    for sh in range(31):
        v = R.getrandbits(sh + 1) | (1 << sh); ints += [v, (-v) & 0xffffffff]
    for op in ('vitof0', 'vitof4'):
        for i in range(0, len(ints) - 3, 4):
            recs.append(vu_step(rig, op, ints[i:i + 4], None))
    f = fields(VU['vdiv'])
    for a, b in mul_pairs(div=True)[::2] + special_pairs() + random_pairs(200):
        s = [0] * 4; t = [0] * 4; s[f['fsf']] = a; t[f['ftf']] = b
        s = [x if k == f['fsf'] else R.getrandbits(32) for k, x in enumerate(s)]
        recs.append(vu_step(rig, 'vdiv', s, t, q=0x12345678, read_q=True))
    f = fields(VU['vsqrt'])
    sq = SPECIALS + [R.getrandbits(32) for _ in range(300)] + [mk(0, R.randint(1, 254), R.getrandbits(23)) for _ in range(400)]
    for v in sq:
        t = [R.getrandbits(32) for _ in range(4)]; t[f['ftf']] = v
        recs.append(vu_step(rig, 'vsqrt', [0] * 4, t, q=0x12345678, read_q=True))
    for op in ('vmulq', 'vaddq'):
        for q, s4 in zip([p[0] for p in mulp[::4]], lanes4([(p[1], 0) for p in mulp])):
            recs.append(vu_step(rig, op, s4[0], None, q=q, read_q=True))
    return recs


PHASES = {'fpu_arith': phase_fpu_arith, 'fpu_acc': phase_fpu_acc, 'fpu_misc': phase_fpu_misc, 'vu': phase_vu}

if __name__ == '__main__':
    phases = sys.argv[1:] or list(PHASES)
    with session() as s:
        rig = Rig(s)
        for name in phases:
            t0 = time.time()
            recs = PHASES[name](rig)
            with open(OUT / f'{name}.jsonl', 'w') as fh:
                for r in recs:
                    fh.write(json.dumps(r) + '\n')
            print(name, len(recs), 'records', rig.steps, 'steps', '%.1fs' % (time.time() - t0), flush=True)
        rig.link.close()
