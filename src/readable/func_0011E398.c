// NEARMISS func_0011E398  (vram 0x0011E398, 0x88 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 86.91% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc scheduling of the even/odd selector and the kernel argument moves.
//
// The function links from the asm body in src/func_0011E398.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libm (newlib): tanf. |x| <= pi/4 goes to the tangent kernel
// func_0011D878(x, 0, 1); Inf / NaN give x - x; otherwise func_0011C7B0
// reduces x by pi/2 into y[0] + y[1] and the kernel gets 1 for an even
// quadrant, -1 (the cotangent) for an odd one.
typedef union {
    float f;
    int i;
} FloatBits;

extern int func_0011C7B0(float x, float *y);
extern float func_0011D878(float x, float y, int k);

float func_0011E398(float x) {
    float y[2];
    int ix;
    int n;
    FloatBits b;

    b.f = x;
    ix = b.i & 0x7FFFFFFF;
    if (ix <= 0x3F490FDA) {
        return func_0011D878(x, 0.0f, 1);
    }
    if (ix >= 0x7F800000) {
        return x - x;
    }
    n = func_0011C7B0(x, y);
    return func_0011D878(y[0], y[1], 1 - ((n & 1) << 1));
}
