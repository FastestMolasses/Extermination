// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 89.70% NEARMISS).
// Fixes read from the original instructions:
//  - the routine is newlib's single-precision cosine kernel __kernel_cosf
//    (kf_cos.c), written in newlib's own shape: the float word is read with the
//    GET_FLOAT_WORD do { } while (0) union block and masked in place
//    (ix &= 0x7fffffff); the quarter-x is chosen by newlib's if / else with
//    SET_FLOAT_WORD in the else arm, and the tail keeps newlib's hz and a
//    temporaries inside the else of the |x| < 0.3 test. The old pointer-cast
//    reads/writes and flattened tail gave the different scheduling.
//  - the newlib constants are #defined float literals rather than static const
//    objects, so the object carries no unused .rodata table.
//
// newlib __kernel_cosf(x, y): cosine of x + y on [-pi/4, pi/4]; y is the tail
// of the reduced argument. Tiny |x| (below 2^-27) whose integer truncation is 0
// return 1. Otherwise z = x * x and r = z * (C1 + z * (C2 + ... + z * C6));
// below |x| = 0.3 the result is 1 - (0.5 * z - (z * r - x * y)); above, a
// quarter of x (0.28125 beyond 0.78125, else the float with x's exponent minus
// 2) is split off as qx and the result is
// (1 - qx) - ((0.5 * z - qx) - (z * r - x * y)).

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

#define SET_FLOAT_WORD(d, i)            \
    do {                                \
        ieee_float_shape_type sf_u;     \
        sf_u.word = (i);                \
        (d) = sf_u.value;               \
    } while (0)

#define one  1.0000000000e+00f
#define C1   4.1666667908e-02f
#define C2  -1.3888889225e-03f
#define C3   2.4801587642e-05f
#define C4  -2.7557314297e-07f
#define C5   2.0875723372e-09f
#define C6  -1.1359647598e-11f

float func_0011CCC8(float x, float y) {
    float a, hz, z, r, qx;
    int ix;
    GET_FLOAT_WORD(ix, x);
    ix &= 0x7fffffff;
    if (ix < 0x32000000) {
        if (((int)x) == 0) return one;
    }
    z = x * x;
    r = z * (C1 + z * (C2 + z * (C3 + z * (C4 + z * (C5 + z * C6)))));
    if (ix < 0x3e99999a)
        return one - ((float)0.5 * z - (z * r - x * y));
    else {
        if (ix > 0x3f480000) {
            qx = (float)0.28125;
        } else {
            SET_FLOAT_WORD(qx, ix - 0x01000000);
        }
        hz = (float)0.5 * z - qx;
        a = one - qx;
        return a - (hz - (z * r - x * y));
    }
}
