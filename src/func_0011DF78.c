// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 85.00% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - the routine is newlib's fabsf (sf_fabs.c); written in newlib's own shape: an
//    unsigned word read with GET_FLOAT_WORD and written back with SET_FLOAT_WORD
//    (separate do { } while (0) union blocks) into x itself, then return x. The old
//    single-union spelling moved the word through an extra register.
//
// fabsf(x): x with its sign bit cleared.
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

float func_0011DF78(float x)
{
	unsigned int ix;
	GET_FLOAT_WORD(ix,x);
	SET_FLOAT_WORD(x,ix&0x7fffffff);
        return x;
}
