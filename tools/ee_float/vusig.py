#!/usr/bin/env python3
"""Record every VU0-macro instruction signature (op, dest mask, broadcast lane)
that occurs in the original boot ELF's code, one real instance each.
Records carry the EFFECTIVE operands (vf0 reads as 0,0,0,1)."""
import json, random, struct, sys, time
from harness import *
src = open(HERE / 'scan.py').read()
ns = {}
exec(src.split("if len(sys.argv) == 1")[0], ns)
exec("import csv" + src.split("import csv")[1].split("QI =")[0], ns)
found, owner = ns['found'], ns['owner']

UNARY = ('vabs', 'vftoi0', 'vftoi4', 'vitof0', 'vitof4')
ACCW = ('vmulabc', 'vmaddabc', 'vopmula')
QOPS = ('vmulq', 'vaddq')
VF0 = [0, 0, 0, 0x3F800000]
S = [0x00000000, 0x80000000, 0x00000001, 0x80400000, 0x3f800000, 0xbf800000, 0x7f7fffff, 0xff7fffff,
     0x7f800000, 0xff800000, 0x7fc00000, 0xffc00000, 0x7f800001, 0xffffffff, 0x00800000, 0x7f000000]
R = random.Random(9923)


def fields(w):
    return {'fd': (w >> 6) & 31, 'fs': (w >> 11) & 31, 'ft': (w >> 16) & 31, 'dest': (w >> 21) & 15,
            'bc': w & 3, 'fsf': (w >> 21) & 3, 'ftf': (w >> 23) & 3}


def signatures():
    sig = {}
    for name, addrs in found.items():
        if not name.startswith('v'):
            continue
        for va in addrs:
            if owner(va) is None:
                continue
            f = fields(word(va))
            if name in ('vdiv', 'vsqrt'):
                key = (name, f['fsf'], f['ftf'])
            else:
                key = (name, f['dest'], f['bc'] if name.endswith('bc') else None)
            sig.setdefault(key, []).append((va, f))
    chosen = {}
    for key, insts in sig.items():
        name = key[0]
        def score(it):
            va, f = it
            dst = f['ft'] if name in UNARY else f['fd']
            s = 0
            if f['fs'] != 0: s += 4
            if name not in UNARY + QOPS + ('vsqrt',) and f['ft'] != 0: s += 4
            if f['fs'] != f['ft']: s += 2
            if name not in ('vdiv', 'vsqrt') + ACCW and dst not in (f['fs'], f['ft']): s += 1
            return s
        chosen[key] = max(insts, key=score)
    return chosen


def rnd_normal():
    return (R.getrandbits(1) << 31) | (R.randint(100, 150) << 23) | R.getrandbits(23)


def run_sig(rig, key, va, f):
    name = key[0]
    unary = name in UNARY
    dst = f['ft'] if unary else f['fd']
    recs = []
    if name in ('vdiv', 'vsqrt'):
        pairs = [(a, b) for a in S for b in S] if name == 'vdiv' else [(0, b) for b in S]
        pairs += [(rnd_normal(), rnd_normal() & 0x7fffffff if name == 'vsqrt' else rnd_normal()) for _ in range(24)]
        for a, b in pairs:
            s = [R.getrandbits(32) for _ in range(4)]; t = [R.getrandbits(32) for _ in range(4)]
            s[f['fsf']] = a; t[f['ftf']] = b
            if name == 'vsqrt': s = None
            cmds = []
            if s is not None and f['fs']: cmds.append(W('VU0F', f['fs'], s))
            if f['ft']: cmds.append(W('VU0F', f['ft'], t))
            cmds += [W('VU0I', 22, 0x12345678), *STEP(va), READ('VU0F'), READ('VU0I')]
            vf, vi = rig.do(cmds)
            es = vf[f['fs']] if s is not None else None
            recs.append({'op': name, 'sig': list(key[1:]), 'va': va, 'vs': es, 'vt': vf[f['ft']],
                         'q': vi[22][0], 'fsf': f['fsf'], 'ftf': f['ftf']})
        return recs
    # lane vectors
    steps = []
    if unary:
        vals = S + [R.getrandbits(32) for _ in range(24)] + [rnd_normal() for _ in range(24)]
        for i in range(0, len(vals), 4):
            steps.append((vals[i:i + 4], None, None, None))
    else:
        pairs = [(a, b) for b in S for a in S]
        R.shuffle(pairs)
        pairs += [(rnd_normal(), rnd_normal()) for _ in range(64)]
        bc = name.endswith('bc') or name in QOPS
        if bc:
            # one broadcast value per step: group by t
            byt = {}
            for a, b in pairs:
                byt.setdefault(b, []).append(a)
            for b, alist in byt.items():
                for i in range(0, len(alist), 4):
                    chunk = (alist[i:i + 4] + [rnd_normal()] * 4)[:4]
                    acc = [R.choice(S + [rnd_normal()] * 4) for _ in range(4)]
                    steps.append((chunk, b, acc, None))
        else:
            for i in range(0, len(pairs), 4):
                ch = pairs[i:i + 4]
                acc = [R.choice(S + [rnd_normal()] * 4) for _ in range(4)]
                steps.append(([c[0] for c in ch], [c[1] for c in ch], acc, None))
    for s, t, acc, _ in steps:
        cmds = []
        if name in QOPS:
            tvec = None
            cmds.append(W('VU0I', 22, t))
        elif name.endswith('bc'):
            tvec = [R.getrandbits(32) for _ in range(4)]; tvec[f['bc']] = t
        else:
            tvec = t
        if f['fs']:
            cmds.append(W('VU0F', f['fs'], s))
        if tvec is not None and f['ft'] and f['ft'] != f['fs']:
            cmds.append(W('VU0F', f['ft'], tvec))
        uses_acc = name in ('vmaddbc', 'vmaddabc', 'vopmsub', 'vopmula', 'vmulabc')
        if uses_acc:
            cmds.append(W('VU0F', 32, acc))
        sentinel = None
        if name not in ACCW and dst not in (f['fs'], f['ft']) and dst != 0:
            sentinel = [0x5a5a5a5a] * 4
            cmds.append(W('VU0F', dst, sentinel))
        pre = None
        if name not in ACCW and sentinel is None:
            pre = 'aliased'
        cmds += [*STEP(va), READ('VU0F')]
        before_needed = sentinel is None and name not in ACCW
        if before_needed:
            # read the register image right before the step so unwritten lanes are known
            (vfb,) = rig.do(cmds[:-3] + [READ('VU0F')])
            (vf,) = rig.do(cmds[-3:])
            prior = vfb[dst]
            es, et = vfb[f['fs']], (vfb[f['ft']] if tvec is not None else None)
            eacc = vfb[32]
        else:
            (vf,) = rig.do(cmds)
            prior = sentinel
            es = s if f['fs'] else VF0
            et = (tvec if f['ft'] else VF0) if tvec is not None else None
            if tvec is not None and f['ft'] == f['fs']:
                et = es
            eacc = acc if uses_acc else None
        rec = {'op': name, 'sig': list(key[1:]), 'va': va, 'dest': f['dest'], 'bc': f['bc'],
               'vs': es, 'vt': et, 'acc_in': eacc if uses_acc else None,
               'q_in': t if name in QOPS else None, 'prior': prior}
        if name in ACCW:
            rec['acc'] = vf[32]
        else:
            rec['vd'] = vf[dst]
        recs.append(rec)
    return recs


if __name__ == '__main__':
    chosen = signatures()
    only = sys.argv[1:]
    out = DATA / 'vectors' / 'vu_signatures.jsonl'
    t0 = time.time()
    with session() as s:
        rig = Rig(s)
        with open(out, 'w') as fh:
            for key in sorted(chosen, key=str):
                if only and key[0] not in only:
                    continue
                va, f = chosen[key]
                recs = run_sig(rig, key, va, f)
                for r in recs:
                    fh.write(json.dumps(r) + '\n')
                print(key, hex(va), len(recs), '%.0fs' % (time.time() - t0), flush=True)
        rig.link.close()
    print('steps', rig.steps)
