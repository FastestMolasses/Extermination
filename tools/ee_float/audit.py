#!/usr/bin/env python3
"""Differential audit: the port's float formulas vs tools/ee_float_model.py.

Formula classes are reproduced exactly as written in each helper (double op,
then the helper's own rounding); em_pose_math.h and em_effect_color.h are
compiled from the port's source and called through ctypes. Counts only;
nothing in the port is modified."""
import ctypes as C, math, random, struct, subprocess, sys, json
from pathlib import Path
REPO = Path(__file__).resolve().parents[2]  # decomp repo root
PORT = REPO.parent / 'extermination-port'
sys.path.insert(0, str(PORT / 'tools'))
import ee_float_model as m

HERE = REPO / 'build/ee_float/audit'  # scratch: shim.c / shim.dylib
HERE.mkdir(parents=True, exist_ok=True)
shim = HERE / 'shim.c'
shim.write_text('''
#include "game/em_pose_math.h"
#include "game/em_effect_color.h"
float p_add(float a,float b){return pose_add(a,b);}
float p_sub(float a,float b){return pose_sub(a,b);}
float p_mul(float a,float b){return pose_mul(a,b);}
float p_div(float a,float b){return pose_div(a,b);}
float p_madd(float c,float a,float b){return pose_madd(c,a,b);}
float p_msub(float c,float a,float b){return pose_msub(c,a,b);}
float e_add(float a,float b){return em_effect_float32((double)a+b);}
float e_sub(float a,float b){return em_effect_float32((double)a-b);}
float e_mul(float a,float b){return em_effect_float32((double)a*b);}
float e_div(float a,float b){return em_effect_float32((double)a/b);}
float rn_div(float a,float b){return (float)((double)a/b);}
''')
lib = HERE / 'shim.dylib'
subprocess.run(['cc', '-O1', '-ffp-contract=off', '-shared', '-fPIC', '-I', str(PORT / 'src'), str(shim), '-o', str(lib)], check=True)
L = C.CDLL(str(lib))
for n in ('p_add', 'p_sub', 'p_mul', 'p_div', 'e_add', 'e_sub', 'e_mul', 'e_div', 'rn_div'):
    getattr(L, n).restype = C.c_float; getattr(L, n).argtypes = [C.c_float, C.c_float]
for n in ('p_madd', 'p_msub'):
    getattr(L, n).restype = C.c_float; getattr(L, n).argtypes = [C.c_float] * 3

B, F = m.f2b, m.b2f
def trunc(v):  # the fp()/truncate()/rtz()/em_effect_float32 formula: RN pack, step toward zero
    r = F(B(v)); return F(B(r) - 1) if abs(r) > abs(v) else r
def rn(v): return F(B(v))
def trim_add(a, b):  # test_pose_transition_reference.add
    aa, bb = B(a), B(b); d = ((aa >> 23) & 255) - ((bb >> 23) & 255)
    if d:
        small = bb if d > 0 else aa; dist = abs(d)
        small &= 0x80000000 if dist >= 25 else (0xffffffff << (dist - 1)) & 0xffffffff
        if d > 0: b = F(small)
        else: a = F(small)
    return trunc(a + b)

CLASSES = {
  # name: {op: fn(a,b[,c]) -> float}; floats in/out
  'native em_pose_math.h (pose_*)': {'add': L.p_add, 'sub': L.p_sub, 'mul': L.p_mul, 'div': L.p_div,
                                      'madd': L.p_madd, 'msub': L.p_msub},
  'native em_effect_float32((double)a op b)': {'add': L.e_add, 'sub': L.e_sub, 'mul': L.e_mul, 'div': L.e_div},
  'native (float)((double)a/b)': {'div': L.rn_div},
  'py trunc(x op y)  [fp/truncate/rtz]': {'add': lambda a, b: trunc(a + b), 'sub': lambda a, b: trunc(a - b),
      'mul': lambda a, b: trunc(a * b), 'div': lambda a, b: trunc(a / b),
      'madd': lambda c, a, b: trunc(c + trunc(a * b)), 'msub': lambda c, a, b: trunc(c - trunc(a * b)),
      'madd_unrounded_product': lambda c, a, b: trunc(c + a * b),
      'msub_product_minus_acc': lambda c, a, b: trunc(trunc(a * b) - c)},
  'py RN float_bits(x op y)': {'add': lambda a, b: rn(a + b), 'sub': lambda a, b: rn(a - b),
      'mul': lambda a, b: rn(a * b), 'div': lambda a, b: rn(a / b)},
  'py pose_transition add/rounded': {'add': trim_add, 'sub': lambda a, b: trim_add(a, -b),
      'madd': lambda c, a, b: trim_add(c, trunc(a * b)), 'msub': lambda c, a, b: trim_add(c, -trunc(a * b))},
}
MODEL = {'add': m.ee_add, 'sub': m.ee_sub, 'mul': m.ee_mul, 'div': m.ee_div,
         'madd': m.ee_madd, 'msub': m.ee_msub, 'madd_unrounded_product': m.ee_madd,
         'msub_product_minus_acc': m.ee_msub}

R = random.Random(7)
def finite_normal(lo=64, hi=190):
    return (R.getrandbits(1) << 31) | (R.randint(lo, hi) << 23) | R.getrandbits(23)
def near(x, d):
    e = (x >> 23) & 255
    return (R.getrandbits(1) << 31) | (max(1, min(254, e - d)) << 23) | R.getrandbits(23)

N = int(sys.argv[1]) if len(sys.argv) > 1 else 20000
pairs = []
for _ in range(N):
    a = finite_normal(); pairs.append((a, near(a, R.randint(-30, 30))))
triples = [(near(b, R.randint(-30, 30)), a, b) for a, b in pairs[:N // 2]]
report = {}
for cname, ops in CLASSES.items():
    for op, fn in ops.items():
        want = MODEL[op]; bad = 0; total = 0; ex = None
        items = triples if op.startswith(('madd', 'msub')) else pairs
        for it in items:
            try:
                got = B(fn(*[F(x) for x in it]))
            except (OverflowError, ZeroDivisionError):
                continue
            total += 1
            w = want(*it)
            if got != w:
                bad += 1
                if ex is None: ex = [hex(x) for x in it] + [hex(got), hex(w)]
        report[f'{cname} :: {op}'] = (bad, total, ex)
for k, (bad, total, ex) in report.items():
    print(f'{bad:6d}/{total:6d}  {k}   e.g. {ex}' if bad else f'{bad:6d}/{total:6d}  {k}')
