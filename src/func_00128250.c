// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft float (fp-bit): __fixunssfsi, float to unsigned int. Zero,
// NaN, negative numbers and negative exponents give 0; infinity and
// exponents of 32 or more give 0xFFFFFFFF; otherwise the fraction is
// shifted down by 30 - exponent, or up by exponent - 30 for exponent 31.
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

unsigned int func_00128250(float arg) {
    FpNumber a;
    float in;

    in = arg;
    func_001278C0(&in, &a);
    if (iszero(&a)) {
        return 0;
    }
    if (isnan(&a)) {
        return 0;
    }
    if (a.sign) {
        return 0;
    }
    if (isinf(&a)) {
        return 0xFFFFFFFF;
    }
    if (a.normal_exp < 0) {
        return 0;
    }
    if (a.normal_exp > 31) {
        return 0xFFFFFFFF;
    }
    if (a.normal_exp > 30) {
        return a.fraction << (a.normal_exp - 30);
    }
    return a.fraction >> (30 - a.normal_exp);
}
