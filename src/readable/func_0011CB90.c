// NEARMISS func_0011CB90  (vram 0x0011CB90, 0x138 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 84.29% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register allocation and loop layout of the bit-by-bit root (the method and rounding
// follow the original).
//
// The function links from the asm body in src/func_0011CB90.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libm (newlib): the core single-precision square root (bit-by-bit
// restoring method). Inf / NaN return x * x + x; zero returns x; negative
// numbers return (x - x) / (x - x) (NaN). Denormals are normalised first.
// The 24-bit root is generated one bit at a time from the mantissa (shifted
// by one for an odd exponent), rounded to nearest on the remainder, and the
// halved exponent is put back.
typedef union {
    float f;
    int i;
} FloatBits;

float func_0011CB90(float x) {
    FloatBits b;
    int ix;
    int m;
    int i;
    int q;
    int s;
    int t;
    unsigned int r;

    b.f = x;
    ix = b.i;
    if ((ix & 0x7F800000) == 0x7F800000) {
        return x * x + x;
    }
    if (ix <= 0) {
        if ((ix & 0x7FFFFFFF) == 0) {
            return x;
        } else if (ix < 0) {
            return (x - x) / (x - x);
        }
    }
    m = ix >> 23;
    if (m == 0) {
        for (i = 0; (ix & 0x00800000) == 0; i++) {
            ix <<= 1;
        }
        m -= i - 1;
    }
    m -= 127;
    ix = (ix & 0x007FFFFF) | 0x00800000;
    if (m & 1) {
        ix += ix;
    }
    m >>= 1;
    ix += ix;
    q = 0;
    s = 0;
    r = 0x01000000;
    while (r != 0) {
        t = s + r;
        if (t <= ix) {
            s = t + r;
            ix -= t;
            q += r;
        }
        ix += ix;
        r >>= 1;
    }
    if (ix != 0) {
        q += q & 1;
    }
    ix = (q >> 1) + 0x3F000000;
    ix += m << 23;
    b.i = ix;
    return b.f;
}
