// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 94.23% NEARMISS).
// Fixes read from the original instructions:
//  - the routine is newlib's single-precision sine kernel __kernel_sinf
//    (kf_sin.c); written in newlib's own shape: the float word is read with the
//    GET_FLOAT_WORD do { } while (0) union block into ix, then masked in place
//    (ix &= 0x7fffffff), and the tail is an if / else pair of returns. The old
//    pointer-cast read with the mask folded into the declaration gave the
//    different register allocation.
//  - the newlib constants are #defined float literals rather than static const
//    objects, so the object carries no unused .rodata table (the original ELF
//    has no such table; the folded constants are materialised inline).
//
// newlib __kernel_sinf(x, y, iy): sine of x + y on [-pi/4, pi/4]; y is the tail
// of the reduced argument and iy says whether it is used. Tiny |x| (below
// 2^-27) whose integer truncation is 0 return x. Otherwise z = x * x,
// v = z * x, r = S2 + z * (S3 + z * (S4 + z * (S5 + z * S6))) and the result is
// x + v * (S1 + z * r) without the tail, else
// x - ((z * (0.5 * y - v * r) - y) - v * S1).

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

#define half  5.0000000000e-01f
#define S1   -1.6666667163e-01f
#define S2    8.3333337680e-03f
#define S3   -1.9841270114e-04f
#define S4    2.7557314297e-06f
#define S5   -2.5050759689e-08f
#define S6    1.5896910177e-10f

float func_0011D770(float x, float y, int iy) {
    float z, r, v;
    int ix;
    GET_FLOAT_WORD(ix, x);
    ix &= 0x7fffffff;
    if (ix < 0x32000000) {
        if ((int)x == 0) return x;
    }
    z = x * x;
    v = z * x;
    r = S2 + z * (S3 + z * (S4 + z * (S5 + z * S6)));
    if (iy == 0) return x + v * (S1 + z * r);
    else return x - ((z * (half * y - v * r) - y) - v * S1);
}
