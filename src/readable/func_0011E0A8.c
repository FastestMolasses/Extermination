// NEARMISS func_0011E0A8  (vram 0x0011E0A8, 0x9C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 78.21% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// the original keeps x's bits in a GPR for the whole function,
// stores the integral part with integer stores and fills the likely-branch delay slot
// of the exponent test with the store for |x| >= 2^23; ee-gcc keeps x in f12 and stores
// it with swc1 (union, int-pointer and separate-union spellings measured).
//
// The function links from the asm body in src/func_0011E0A8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// modff: *ip = the integral part of x, the result the fraction. Exponent e =
// ((bits >> 23) & 0xFF) - 0x7F. e < 0: *ip = sign of x, returns x. e >= 23: *ip = x,
// returns the sign alone. Otherwise mask = 0x7FFFFF >> e; an integral x gives *ip = x
// and returns the sign alone; else *ip = x with the mask bits cleared and the result
// is x - *ip.
typedef union {
    float f;
    int i;
} FloatBits;

float func_0011E0A8(float x, float *ip) {
    FloatBits u;
    int e;
    unsigned int m;
    u.f = x;
    e = ((u.i >> 23) & 0xFF) - 0x7F;
    if (e < 23) {
        if (e < 0) {
            FloatBits s;
            s.i = u.i & 0x80000000;
            *ip = s.f;
            return x;
        }
        m = 0x007FFFFF >> e;
        if ((u.i & m) == 0) {
            *ip = x;
            u.i &= 0x80000000;
            return u.f;
        } else {
            FloatBits s;
            s.i = u.i & ~m;
            *ip = s.f;
            return x - *ip;
        }
    } else {
        *ip = x;
        u.i &= 0x80000000;
        return u.f;
    }
}
