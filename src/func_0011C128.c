// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 87.62% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - the routine is newlib's single-precision __ieee754_asinf (ef_asin.c, the
//    fdlibm-derived version with pio2_hi = 1.5707962513); written in newlib's own
//    shape: GET_FLOAT_WORD / SET_FLOAT_WORD do { } while (0) union blocks, newlib's
//    names and if / else-if chain, including its misleading-indent quirk where the
//    polynomial after `else t = x*x;` runs for both tiny and small |x|. Constants
//    are #define float literals (the original builds them inline, no .rodata table).
//  - callees: fabsf = func_0011DF78, __ieee754_sqrtf = func_0011CB90, both
//    (float) -> float as in their src definitions. Plain 2.9 gas (2.96 also matches).
//
// asinf(x): |x| == 1 gives +-pi/2; |x| > 1 gives NaN; |x| < 0.5 uses the rational
// approximation x + x*p(t)/q(t), t = x*x (x itself when |x| < 2^-27); otherwise
// t = (1 - |x|)/2, s = sqrt(t), and the result is built from s with a split of s
// into a high part (low 12 bits cleared) when |x| < 0.975; the sign follows x.
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

#define one      1.0000000000e+00f
#define huge     1.000e+30f
#define pio2_hi  1.5707962513e+00f
#define pio2_lo  7.5497894159e-08f
#define pio4_hi  7.8539818525e-01f
#define pS0      1.6666667163e-01f
#define pS1     -3.2556581497e-01f
#define pS2      2.0121252537e-01f
#define pS3     -4.0055535734e-02f
#define pS4      7.9153501429e-04f
#define pS5      3.4793309169e-05f
#define qS1     -2.4033949375e+00f
#define qS2      2.0209457874e+00f
#define qS3     -6.8828397989e-01f
#define qS4      7.7038154006e-02f

#define fabsf func_0011DF78
#define __ieee754_sqrtf func_0011CB90

extern float fabsf(float x);
extern float __ieee754_sqrtf(float x);

float func_0011C128(float x)
{
	float t,w,p,q,c,r,s;
	int hx,ix;
	GET_FLOAT_WORD(hx,x);
	ix = hx&0x7fffffff;
	if(ix==0x3f800000) {
		/* asin(1)=+-pi/2 with inexact */
	    return x*pio2_hi+x*pio2_lo;
	} else if(ix> 0x3f800000) {	/* |x|>= 1 */
	    return (x-x)/(x-x);		/* asin(|x|>1) is NaN */
	} else if (ix<0x3f000000) {	/* |x|<0.5 */
	    if(ix<0x32000000) {		/* if |x| < 2**-27 */
		if(huge+x>one) return x;/* return x with inexact if x!=0*/
	    } else
		t = x*x;
		p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
		q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
		w = p/q;
		return x+x*w;
	}
	/* 1> |x|>= 0.5 */
	w = one-fabsf(x);
	t = w*(float)0.5;
	p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
	q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
	s = __ieee754_sqrtf(t);
	if(ix>=0x3F79999A) { 	/* if |x| > 0.975 */
	    w = p/q;
	    t = pio2_hi-((float)2.0*(s+s*w)-pio2_lo);
	} else {
	    int iw;
	    w  = s;
	    GET_FLOAT_WORD(iw,w);
	    SET_FLOAT_WORD(w,iw&0xfffff000);
	    c  = (t-w*w)/(s+w);
	    r  = p/q;
	    p  = (float)2.0*s*r-(pio2_lo-(float)2.0*c);
	    q  = pio4_hi-(float)2.0*w;
	    t  = pio4_hi-(p-q);
	}
	if(hx>0) return t; else return -t;
}
