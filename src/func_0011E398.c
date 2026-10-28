// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 86.91% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - the routine is newlib's tanf (sf_tan.c); written in newlib's own shape:
//    GET_FLOAT_WORD do { } while (0) union block, `ix &= 0x7fffffff;` as its own
//    statement, the zero tail as a local (z = 0.0), and the if / else if / else
//    chain with the finite test spelled !FLT_UWORD_IS_FINITE.
//  - callees match their src definitions: __ieee754_rem_pio2f = func_0011C7B0
//    (float, float *), __kernel_tanf = func_0011D878 (float, float, int).
//    Plain 2.9 gas (2.96 also matches).
//
// tanf(x): |x| <= pi/4 goes to the tangent kernel with a zero tail; Inf / NaN give
// x - x; otherwise x is reduced by pi/2 into y[0] + y[1] and the kernel gets 1 for
// an even quadrant, -1 (the cotangent, -1/tan) for an odd one.
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

#define FLT_UWORD_IS_FINITE(x) ((x) < 0x7f800000)
#define __ieee754_rem_pio2f func_0011C7B0
#define __kernel_tanf func_0011D878

extern int __ieee754_rem_pio2f(float x, float *y);
extern float __kernel_tanf(float x, float y, int iy);

float func_0011E398(float x)
{
	float y[2],z=0.0;
	int n,ix;

	GET_FLOAT_WORD(ix,x);

    /* |x| ~< pi/4 */
	ix &= 0x7fffffff;
	if(ix <= 0x3f490fda) return __kernel_tanf(x,z,1);

    /* tan(Inf or NaN) is NaN */
	else if (!FLT_UWORD_IS_FINITE(ix)) return x-x;		/* NaN */

    /* argument reduction needed */
	else {
	    n = __ieee754_rem_pio2f(x,y);
	    return __kernel_tanf(y[0],y[1],1-((n&1)<<1)); /*   1 -- n even
							      -1 -- n odd */
	}
}
