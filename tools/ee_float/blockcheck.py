#!/usr/bin/env python3
"""Block-execution check: run original code free (compiled as whole blocks,
no single-stepping) and compare with the model applied instruction by
instruction. Records go to build/startup-reference/ee_float/blockcheck.json."""
import json, random, struct, sys, time
from harness import *
sys.path.insert(0, str(REPO.parent / 'extermination-port/tools'))
import ee_float_model as m

LOOP_TOP = 0x001AAF28
R = random.Random(4242)
SPEC = [0x7f800000, 0xff800000, 0x7fc00000, 0xffffffff, 0x00000001, 0x80000000, 0x7f7fffff, 0x00000000]


def val(p_special=0.06, lo=100, hi=150):
    if R.random() < p_special:
        return R.choice(SPEC)
    return (R.getrandbits(1) << 31) | (R.randint(lo, hi) << 23) | R.getrandbits(23)


def run_until_paused(rig, pc_expected):
    rig.link.batch([{'cmd': 'resume'}])
    t0 = time.time()
    while True:
        (st,) = rig.link.batch([{'cmd': 'status'}])
        if st['data']['paused']:
            pc = int(st['data']['pc'], 16)
            if pc != pc_expected:
                raise RuntimeError(f'paused at {pc:#x}, expected {pc_expected:#x}')
            return
        if time.time() - t0 > 5:
            raise TimeoutError('did not pause')
        time.sleep(0.002)


def hexwords(ws):
    return ''.join(struct.pack('<I', w).hex() for w in ws)


def vu_apply_matrix(rig, n):
    """func_001026A0 (227 callers): out = M * v via VMULAx, VMADDAy, VMADDAz, VMADDw."""
    M, V, OUT = 0x01F00100, 0x01F00200, 0x01F00300
    res = []
    for _ in range(n):
        mat = [val() for _ in range(16)]
        vec = [val() for _ in range(4)]
        rig.link.batch([{'cmd': 'write_memory', 'address': M, 'data': hexwords(mat)},
                        {'cmd': 'write_memory', 'address': V, 'data': hexwords(vec)},
                        {'cmd': 'write_memory', 'address': OUT, 'data': hexwords([0x5a5a5a5a] * 4)},
                        W('GPR', 4, [OUT, 0, 0, 0]), W('GPR', 5, [M, 0, 0, 0]), W('GPR', 6, [V, 0, 0, 0]),
                        W('GPR', 31, [LOOP_TOP, 0, 0, 0]), {'cmd': 'set_pc', 'value': 0x1026A0}])
        run_until_paused(rig, LOOP_TOP)
        (mem,) = rig.link.batch([{'cmd': 'read_memory', 'address': OUT, 'length': 16}])
        out = list(struct.unpack('<4I', bytes.fromhex(mem['hex'])))
        res.append({'fn': 0x1026A0, 'M': mat, 'v': vec, 'out': out})
    return res


def model_apply_matrix(mat, vec):
    c = [mat[0:4], mat[4:8], mat[8:12], mat[12:16]]
    acc = [m.vu_lane('vmulabc', 15, 0, c[0][k], vec[0]) for k in range(4)]
    acc = [m.vu_lane('vmaddabc', 15, 1, c[1][k], vec[1], acc[k]) for k in range(4)]
    acc = [m.vu_lane('vmaddabc', 15, 2, c[2][k], vec[2], acc[k]) for k in range(4)]
    return [m.vu_lane('vmaddbc', 15, 3, c[3][k], vec[3], acc[k]) for k in range(4)]


def ee_prefix(rig, n):
    """func_00102EA8 run from entry to 0x102F24: a block of NEG/ADD/MUL/DIV."""
    SP = 0x01F01000
    rig.link.batch([{'cmd': 'set_breakpoint', 'address': 0x102F24}])
    res = []
    try:
        for _ in range(n):
            f = {k: val(0.04, 110, 140) for k in range(12, 20)}
            mem = val(0.04, 110, 140)
            cmds = [W('FPR', k, v) for k, v in f.items()]
            cmds += [{'cmd': 'write_memory', 'address': SP, 'data': hexwords([mem])},
                     W('GPR', 29, [SP, 0, 0, 0]), {'cmd': 'set_pc', 'value': 0x102EA8}]
            rig.link.batch(cmds)
            run_until_paused(rig, 0x102F24)
            (fpr,) = rig.do([READ('FPR')])
            res.append({'fn': 0x102EA8, 'f': f, 'mem': mem,
                        'out': {k: fpr[k][0] for k in (0, 1, 17, 19, 20, 21)}})
    finally:
        rig.link.batch([{'cmd': 'remove_breakpoint', 'address': 0x102F24}])
    return res


def model_ee_prefix(f, mem):
    f = dict(f)
    f[0] = m.ee_neg(f[17]); f[20] = m.ee_neg(f[18]); f[1] = mem
    f[0] = m.ee_add(f[0], f[18]); f[21] = m.ee_mul(f[1], f[19]); f[20] = m.ee_mul(f[20], f[19])
    f[17] = m.ee_mul(f[17], f[1]); f[19] = m.ee_neg(f[19]); f[21] = m.ee_mul(f[21], f[0])
    f[20] = m.ee_add(f[20], f[17]); f[19] = m.ee_add(f[19], f[1])
    f[21] = m.ee_div(f[21], f[19]); f[20] = m.ee_div(f[20], f[19])
    return {k: f[k] for k in (0, 1, 17, 19, 20, 21)}


if __name__ == '__main__':
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 300
    with session() as s:
        rig = Rig(s)
        a = vu_apply_matrix(rig, n)
        b = ee_prefix(rig, n)
        rig.link.close()
    json.dump({'vu_apply_matrix': a, 'ee_prefix': b}, open(DATA / 'blockcheck.json', 'w'))
    bad_a = sum(model_apply_matrix(r['M'], r['v']) != r['out'] for r in a)
    bad_b = sum(model_ee_prefix({int(k): v for k, v in r['f'].items()}, r['mem']) != r['out'] for r in b)
    print('func_001026A0 runs', len(a), 'mismatches', bad_a)
    print('func_00102EA8 prefix runs', len(b), 'mismatches', bad_b)
