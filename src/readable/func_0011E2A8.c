// NEARMISS func_0011E2A8  (vram 0x0011E2A8, 0xF0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 92.58% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register allocation and the quadrant switch's branch layout (the reduction buffer and
// kernel calls follow the original).
//
// The function links from the asm body in src/func_0011E2A8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libm (newlib): sinf. |x| <= pi/4 goes straight to the sine kernel
// func_0011D770(x, 0, 0); Inf / NaN give x - x; otherwise func_0011C7B0
// reduces x by pi/2 into y[0] + y[1] and the quadrant n & 3 picks sin, cos,
// -sin or -cos of the remainder (func_0011CCC8 is the cosine kernel).
typedef union {
    float f;
    int i;
} FloatBits;

extern int func_0011C7B0(float x, float *y);
extern float func_0011CCC8(float x, float y);
extern float func_0011D770(float x, float y, int iy);

float func_0011E2A8(float x) {
    float y[2];
    int ix;
    int n;
    FloatBits b;

    b.f = x;
    ix = b.i & 0x7FFFFFFF;
    if (ix <= 0x3F490FD8) {
        return func_0011D770(x, 0.0f, 0);
    }
    if (ix >= 0x7F800000) {
        return x - x;
    }
    n = func_0011C7B0(x, y);
    switch (n & 3) {
    case 0:
        return func_0011D770(y[0], y[1], 1);
    case 1:
        return func_0011CCC8(y[0], y[1]);
    case 2:
        return -func_0011D770(y[0], y[1], 1);
    default:
        return -func_0011CCC8(y[0], y[1]);
    }
}
