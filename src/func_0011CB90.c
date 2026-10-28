// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 84.29% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - the routine is newlib's __ieee754_sqrtf (ef_sqrtf.c, the fdlibm-derived
//    version that tests (ix & 0x7f800000) == 0x7f800000 and ix <= 0); written in
//    newlib's own shape, including the floating rounding-direction probe
//    (z = one - tiny; if (z >= one) ...), which gcc folds to q += q & 1 at
//    compile time, and SET_FLOAT_WORD for the result. one, tiny and the sign mask
//    are #defines: newlib's static consts would leave unused .rodata in the
//    object (they match the text too, but the original has no table here).
//
// sqrtf(x), bit-by-bit restoring method: Inf / NaN give x*x + x; +-0 gives x;
// negatives give NaN. Denormals are normalised first; the 24-bit root is generated
// one bit at a time from the mantissa (doubled for an odd exponent), rounded to
// nearest on the remainder, and the halved exponent put back.
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

#define one 1.0f
#define tiny 1.0e-30f
#define sign ((int)0x80000000)

float func_0011CB90(float x)
{
	float z;
	unsigned int r;
	int ix,s,q,m,t,i;

	GET_FLOAT_WORD(ix,x);

    /* take care of Inf and NaN */
	if((ix&0x7f800000)==0x7f800000) {
	    return x*x+x;		/* sqrt(NaN)=NaN, sqrt(+inf)=+inf
					   sqrt(-inf)=sNaN */
	}
    /* take care of zero */
	if(ix<=0) {
	    if((ix&(~sign))==0) return x;/* sqrt(+-0) = +-0 */
	    else if(ix<0)
		return (x-x)/(x-x);		/* sqrt(-ve) = sNaN */
	}
    /* normalize x */
	m = (ix>>23);
	if(m==0) {				/* subnormal x */
	    for(i=0;(ix&0x00800000)==0;i++) ix<<=1;
	    m -= i-1;
	}
	m -= 127;	/* unbias exponent */
	ix = (ix&0x007fffff)|0x00800000;
	if(m&1)	/* odd m, double x to make it even */
	    ix += ix;
	m >>= 1;	/* m = [m/2] */

    /* generate sqrt(x) bit by bit */
	ix += ix;
	q = s = 0;		/* q = sqrt(x) */
	r = 0x01000000;		/* r = moving bit from right to left */

	while(r!=0) {
	    t = s+r;
	    if(t<=ix) {
		s    = t+r;
		ix  -= t;
		q   += r;
	    }
	    ix += ix;
	    r>>=1;
	}

    /* use floating add to find out rounding direction */
	if(ix!=0) {
	    z = one-tiny; /* trigger inexact flag */
	    if (z>=one) {
	        z = one+tiny;
		if (z>one)
		    q += 2;
		else
		    q += (q&1);
	    }
	}
	ix = (q>>1)+0x3f000000;
	ix += (m <<23);
	SET_FLOAT_WORD(z,ix);
	return z;
}
