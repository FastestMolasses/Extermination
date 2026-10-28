// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 92.33% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - the routine is newlib's single-precision cosf (sf_cos.c); written in newlib's
//    own shape: the float word is read with the GET_FLOAT_WORD do { } while (0)
//    union block, then masked in place (ix &= 0x7fffffff) as its own statement;
//    the zero tail is a local (float y[2], z = 0.0) passed to the kernel; the
//    tests are newlib's if / else if / else chain (the finite test spelled
//    !FLT_UWORD_IS_FINITE), and the quadrant switch keeps newlib's case order
//    with the last case as default. The old flat early-return form with the
//    mask folded into the read gave a different register allocation and
//    branch layout.
//  - callees match their src definitions: __ieee754_rem_pio2f = func_0011C7B0
//    (float, float *), __kernel_cosf = func_0011CCC8 (float, float),
//    __kernel_sinf = func_0011D770 (float, float, int). Plain 2.9 gas; the
//    2.96 assembler is not needed (both also match).
//
// newlib cosf(x): |x| <= pi/4 goes straight to the cosine kernel with a zero tail;
// Inf / NaN give x - x; otherwise __ieee754_rem_pio2f reduces x by pi/2 into
// y[0] + y[1] and the quadrant n & 3 picks cos, -sin, -cos or sin of the
// remainder.
typedef union {
    float value;
    int word;
} ieee_float_shape_type;

#define GET_FLOAT_WORD(i, d)            \
    do {                                \
        ieee_float_shape_type gf_u;     \
        gf_u.value = (d);               \
        (i) = gf_u.word;                \
    } while (0)

#define FLT_UWORD_IS_FINITE(x) ((x) < 0x7f800000)

#define __ieee754_rem_pio2f func_0011C7B0
#define __kernel_cosf func_0011CCC8
#define __kernel_sinf func_0011D770

extern int __ieee754_rem_pio2f(float x, float *y);
extern float __kernel_cosf(float x, float y);
extern float __kernel_sinf(float x, float y, int iy);

float func_0011DE90(float x)
{
    float y[2], z = 0.0;
    int n, ix;

    GET_FLOAT_WORD(ix, x);

    ix &= 0x7fffffff;
    if (ix <= 0x3f490fd8) return __kernel_cosf(x, z);

    else if (!FLT_UWORD_IS_FINITE(ix)) return x - x;

    else {
        n = __ieee754_rem_pio2f(x, y);
        switch (n & 3) {
        case 0: return  __kernel_cosf(y[0], y[1]);
        case 1: return -__kernel_sinf(y[0], y[1], 1);
        case 2: return -__kernel_cosf(y[0], y[1]);
        default:
                return  __kernel_sinf(y[0], y[1], 1);
        }
    }
}
