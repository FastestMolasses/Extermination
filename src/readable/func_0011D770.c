// NEARMISS func_0011D770  (vram 0x0011D770, 0x104 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 94.23% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register allocation: the original moves iy out of a0 before extracting the float word and
// schedules the bound constants differently (pointer and union float-word spellings measured).
//
// The function links from the asm body in src/func_0011D770.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libm (newlib single precision): the sine kernel on [-pi/4, pi/4].
// x is the reduced argument, y its tail and iy says whether y is used.
// Tiny |x| (below 2^-27) whose integer truncation is 0 return x. Otherwise
// z = x * x, v = z * x, r = S2 + z * (S3 + z * (S4 + z * (S5 + z * S6))),
// and the result is x + v * (S1 + z * r) without the tail, else
// x - ((z * (0.5 * y - v * r) - y) - v * S1).
float func_0011D770(float x, float y, int iy) {
    float z;
    float r;
    float v;
    int ix = *(int *)&x & 0x7FFFFFFF;

    if (ix < 0x32000000) {
        if ((int)x == 0) {
            return x;
        }
    }
    z = x * x;
    v = z * x;
    r = 0.008333334f + z * (-0.0001984127f + z * (0.0000027557314f + z * (-2.505076e-8f + z * 1.589691e-10f)));
    if (iy == 0) {
        return x + v * (-0.16666667f + z * r);
    }
    return x - ((z * (0.5f * y - v * r) - y) - v * -0.16666667f);
}
