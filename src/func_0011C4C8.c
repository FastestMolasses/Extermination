// COMPILER: eegcc
// CFLAGS: -O2

// fdlibm __ieee754_atan2f(y, x)
//   func_0011DBB8 = atanf, func_0011DF78 = fabsf

extern float func_0011DBB8(float);
extern float func_0011DF78(float);

/* the -0.0f literal lives in the game's read-only data */
extern const float D_0026C170; /* -0.0f */

typedef union {
    float value;
    int word;
} ieee_float_shape_type;

#define GET_FLOAT_WORD(i, d)                                                   \
    do {                                                                       \
        ieee_float_shape_type gf_u;                                            \
        gf_u.value = (d);                                                      \
        (i) = gf_u.word;                                                       \
    } while (0)

#define SET_FLOAT_WORD(d, i)                                                   \
    do {                                                                       \
        ieee_float_shape_type sf_u;                                            \
        sf_u.word = (i);                                                       \
        (d) = sf_u.value;                                                      \
    } while (0)

#define tiny   1.0e-30f
#define zero   0.0f
#define pi_o_4 7.8539818525e-01f  /* 0x3f490fdb */
#define pi_o_2 1.5707963705e+00f  /* 0x3fc90fdb */
#define pi     3.1415925026e+00f  /* 0x40490fda */
#define pi_lo  1.5099578832e-07f  /* 0x34222168 */

float func_0011C4C8(float y, float x)
{
    float z;
    int k, m, hx, hy, ix, iy;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;
    GET_FLOAT_WORD(hy, y);
    iy = hy & 0x7fffffff;
    if ((ix > 0x7f800000) || (iy > 0x7f800000)) /* x or y is NaN */
        return x + y;
    if (hx == 0x3f800000)
        return func_0011DBB8(y); /* x = 1.0 */
    m = ((hy >> 31) & 1) | ((hx >> 30) & 2); /* 2*sign(x)+sign(y) */

    /* when y = 0 */
    if (iy == 0) {
        switch (m) {
        case 0:
        case 1:
            return y; /* atan(+-0, +anything) = +-0 */
        case 2:
            return pi + tiny; /* atan(+0, -anything) = pi */
        case 3:
            return -pi - tiny; /* atan(-0, -anything) = -pi */
        }
    }
    /* when x = 0 */
    if (ix == 0)
        return (hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny;

    /* when x is INF */
    if (ix == 0x7f800000) {
        if (iy == 0x7f800000) {
            switch (m) {
            case 0:
                return pi_o_4 + tiny;
            case 1:
                return -pi_o_4 - tiny;
            case 2:
                return 3.0f * pi_o_4 + tiny;
            case 3:
                return -3.0f * pi_o_4 - tiny;
            }
        } else {
            switch (m) {
            case 0:
                return zero;
            case 1:
                return D_0026C170; /* -zero */
            case 2:
                return pi + tiny;
            case 3:
                return -pi - tiny;
            }
        }
    }
    /* when y is INF */
    if (iy == 0x7f800000)
        return (hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny;

    /* compute y/x */
    k = (iy - ix) >> 23;
    if (k > 60)
        z = pi_o_2 + 0.5f * pi_lo; /* |y/x| > 2**60 */
    else if (hx < 0 && k < -60)
        z = 0.0f; /* |y|/x < -2**60 */
    else
        z = func_0011DBB8(func_0011DF78(y / x));

    switch (m) {
    case 0:
        return z; /* atan(+,+) */
    case 1: {
        int zh;
        GET_FLOAT_WORD(zh, z);
        SET_FLOAT_WORD(z, zh ^ 0x80000000);
    }
        return z; /* atan(-,+) */
    case 2:
        return pi - (z - pi_lo); /* atan(+,-) */
    default:
        return (z - pi_lo) - pi; /* atan(-,-) */
    }
}
