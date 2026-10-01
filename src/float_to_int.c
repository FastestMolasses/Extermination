// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft float (fp-bit): __fixsfsi, float to int. The float is
// unpacked by func_001278C0; zero, NaN and negative exponents give 0,
// infinity and exponents of 31 or more saturate to INT_MIN / INT_MAX by
// sign; otherwise the fraction is shifted down by 30 - exponent and negated
// for a negative sign.
/* The software-float unpacked form (libgcc fp-bit): class 0 / 1 = signalling
 * / quiet NaN, 2 = zero, 3 = normal number, 4 = infinity; normal_exp is the
 * unbiased exponent; the fraction is left-aligned with the implicit one at
 * bit 30 (float) or bit 62 (double), leaving 7 / 8 guard bits. */
typedef struct FpNumber {
    unsigned int fpclass;
    unsigned int sign;
    int normal_exp;
    unsigned int fraction;
} FpNumber;

extern void func_001278C0(float *in, FpNumber *out);

static __inline__ int isnan(FpNumber *x) {
    return x->fpclass < 2;
}

static __inline__ int iszero(FpNumber *x) {
    return x->fpclass == 2;
}

static __inline__ int isinf(FpNumber *x) {
    return x->fpclass == 4;
}

int float_to_int(float arg) {
    FpNumber a;
    float in;
    unsigned int tmp;

    in = arg;
    func_001278C0(&in, &a);
    if (iszero(&a)) {
        return 0;
    }
    if (isnan(&a)) {
        return 0;
    }
    if (isinf(&a)) {
        return a.sign ? (int)0x80000000 : 0x7FFFFFFF;
    }
    if (a.normal_exp < 0) {
        return 0;
    }
    if (a.normal_exp > 30) {
        return a.sign ? (int)0x80000000 : 0x7FFFFFFF;
    }
    tmp = a.fraction >> (30 - a.normal_exp);
    return a.sign ? -(int)tmp : (int)tmp;
}
