// NEARMISS func_0011CCC8  (vram 0x0011CCC8, 0x158 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 89.70% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc scheduling of the bound constants and the quarter-x selection (the original materialises
// 0.28125 through a GPR and merges both arms before one move to the FPU).
//
// The function links from the asm body in src/func_0011CCC8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libm (newlib single precision): the cosine kernel on [-pi/4, pi/4].
// x is the reduced argument and y its tail. Tiny |x| (below 2^-27) whose
// integer truncation is 0 return 1. Otherwise z = x * x and
// r = z * (C1 + z * (C2 + ... + z * C6)); below |x| = 0.3 the result is
// 1 - (0.5 * z - (z * r - x * y)); above, a quarter of x (0.28125 beyond
// 0.78125, else the float with x's exponent minus 2) is split off as qx and
// the result is (1 - qx) - ((0.5 * z - qx) - (z * r - x * y)).
float func_0011CCC8(float x, float y) {
    float z;
    float r;
    float qx;
    int ix = *(int *)&x & 0x7FFFFFFF;

    if (ix < 0x32000000) {
        if ((int)x == 0) {
            return 1.0f;
        }
    }
    z = x * x;
    r = z * (0.041666668f + z * (-0.0013888889f + z * (0.000024801588f + z * (-0.00000027557314f
        + z * (2.0875723e-9f + z * -1.1359648e-11f)))));
    if (ix < 0x3E99999A) {
        return 1.0f - (0.5f * z - (z * r - x * y));
    }
    if (ix > 0x3F480000) {
        qx = 0.28125f;
    } else {
        *(int *)&qx = ix - 0x01000000;
    }
    return (1.0f - qx) - ((0.5f * z - qx) - (z * r - x * y));
}
