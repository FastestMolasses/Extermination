// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 28.89% NEARMISS
// companion in src/readable/). Fix read from the original instructions:
//  - the routine is newlib's isnanf (sf_isnan.c), written in newlib's own shape:
//    GET_FLOAT_WORD as a do { } while (0) union block, then `ix &= 0x7fffffff;`
//    and `ix = 0x7f800000 - ix;` as separate statements before the unsigned
//    shift. Folding the mask and the subtraction into one expression gave the
//    old operand order.

// isnanf(x): 1 when the float's magnitude bits (bits & 0x7FFFFFFF) exceed
// 0x7F800000, i.e. the value is a NaN; else 0.
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

int func_0011E080(float x)
{
	int ix;
	GET_FLOAT_WORD(ix,x);
	ix &= 0x7fffffff;
	ix = 0x7f800000 - ix;
	return (int)(((unsigned int)(ix))>>31);
}
