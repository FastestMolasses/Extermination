// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 78.21% NEARMISS
// companion in src/readable/). Fix read from the original instructions:
//  - the routine is newlib's modff (sf_modf.c), written in newlib's own shape:
//    GET_FLOAT_WORD / SET_FLOAT_WORD do { } while (0) union blocks, the
//    if/else chain and the no-fraction branch's *iptr = x*one. Keeping x's word
//    in a general register and storing the integral part with integer stores
//    falls out of that shape; the old single-union spelling did not.

// modff(x, iptr): *iptr = the integral part of x, returns the fraction. Exponent
// j0 = ((bits >> 23) & 0xFF) - 0x7F. j0 < 0: *iptr = signed zero, returns x.
// j0 >= 23: *iptr = x, returns signed zero. Otherwise mask = 0x7FFFFF >> j0; an
// integral x gives *iptr = x and returns signed zero; else *iptr = x with the
// mask bits cleared and the result is x - *iptr.
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

float func_0011E0A8(float x, float *iptr)
{
	int i0,j0;
	unsigned int i;
	GET_FLOAT_WORD(i0,x);
	j0 = ((i0>>23)&0xff)-0x7f;	/* exponent of x */
	if(j0<23) {			/* integer part in x */
	    if(j0<0) {			/* |x|<1 */
	        SET_FLOAT_WORD(*iptr,i0&0x80000000);	/* *iptr = +-0 */
		return x;
	    } else {
		i = (0x007fffff)>>j0;
		if((i0&i)==0) {			/* x is integral */
		    unsigned int ix;
		    *iptr = x;
		    GET_FLOAT_WORD(ix,x);
		    SET_FLOAT_WORD(x,ix&0x80000000);	/* return +-0 */
		    return x;
		} else {
		    SET_FLOAT_WORD(*iptr,i0&(~i));
		    return x - *iptr;
		}
	    }
	} else {			/* no fraction part */
	    unsigned int ix;
	    *iptr = x*one;
	    GET_FLOAT_WORD(ix,x);
	    SET_FLOAT_WORD(x,ix&0x80000000);	/* return +-0 */
	    return x;
	}
}
