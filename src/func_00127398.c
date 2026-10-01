// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft float (fp-bit): compares two unpacked doubles (layout as in
// func_00126BE8). Returns 1 when either is a NaN (unordered); otherwise
// -1, 0 or 1 for a < b, a == b, a > b, ordering infinities and zeros by
// sign, then by sign, exponent and fraction (reversed for negatives).
typedef struct FpNumberD {
    unsigned int fpclass;
    unsigned int sign;
    int normal_exp;
    int pad;
    unsigned long long fraction;
} FpNumberD;

static __inline__ int isnan(FpNumberD *x) {
    return x->fpclass < 2;
}

static __inline__ int iszero(FpNumberD *x) {
    return x->fpclass == 2;
}

static __inline__ int isinf(FpNumberD *x) {
    return x->fpclass == 4;
}

int func_00127398(FpNumberD *a, FpNumberD *b) {
    if (isnan(a) || isnan(b)) {
        return 1;
    }
    if (isinf(a) && isinf(b)) {
        return b->sign - a->sign;
    }
    if (isinf(a)) {
        return a->sign ? -1 : 1;
    }
    if (isinf(b)) {
        return b->sign ? 1 : -1;
    }
    if (iszero(a) && iszero(b)) {
        return 0;
    }
    if (iszero(a)) {
        return b->sign ? 1 : -1;
    }
    if (iszero(b)) {
        return a->sign ? -1 : 1;
    }
    if (a->sign != b->sign) {
        return a->sign ? -1 : 1;
    }
    if (a->normal_exp > b->normal_exp) {
        return a->sign ? -1 : 1;
    }
    if (a->normal_exp < b->normal_exp) {
        return a->sign ? 1 : -1;
    }
    if (a->fraction > b->fraction) {
        return a->sign ? -1 : 1;
    }
    if (a->fraction < b->fraction) {
        return a->sign ? 1 : -1;
    }
    return 0;
}
