// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft float (fp-bit): packs an unpacked number back into a double
// (the double form of func_001277B0: fraction at +0x10 with the implicit one
// at bit 62 and 8 guard bits). NaNs keep their fraction with quiet bit 51
// set and exponent 0x7FF; infinities and exponents above 1023 become
// infinity; zero and a zero fraction keep exponent 0. Exponents below -1022
// shift the fraction down into a denormal (nothing left beyond 56 places).
// Normal numbers round to nearest even on the guard bits and renormalise on
// carry. The fields are stored through a double bitfield union.
typedef struct FpNumberD {
    unsigned int fpclass;
    unsigned int sign;
    int normal_exp;
    int pad;
    unsigned long long fraction;
} FpNumberD;

typedef union DoubleUnion {
    double value;
    struct {
        unsigned long long fraction : 52;
        unsigned int exp : 11;
        unsigned int sign : 1;
    } bits;
} DoubleUnion;

static __inline__ int isnan(FpNumberD *x) {
    return x->fpclass < 2;
}

static __inline__ int iszero(FpNumberD *x) {
    return x->fpclass == 2;
}

static __inline__ int isinf(FpNumberD *x) {
    return x->fpclass == 4;
}

double func_00126AB8(FpNumberD *src) {
    DoubleUnion dst;
    unsigned long long fraction = src->fraction;
    int sign = src->sign;
    int exp = 0;
    int shift;

    if (isnan(src)) {
        exp = 0x7FF;
        fraction |= 0x8000000000000ULL;
    } else if (isinf(src)) {
        exp = 0x7FF;
        fraction = 0;
    } else if (iszero(src)) {
        fraction = 0;
    } else if (fraction != 0) {
        if (src->normal_exp < -1022) {
            shift = -1022 - src->normal_exp;
            if (shift > 56) {
                fraction = 0;
            } else {
                fraction >>= shift;
            }
            fraction >>= 8;
        } else if (src->normal_exp > 1023) {
            exp = 0x7FF;
            fraction = 0;
        } else {
            exp = src->normal_exp + 1023;
            if ((fraction & 0xFF) == 0x80) {
                if (fraction & 0x100) {
                    fraction += 0x80;
                }
            } else {
                fraction += 0x7F;
            }
            if (fraction >= 0x2000000000000000ULL) {
                fraction >>= 1;
                exp += 1;
            }
            fraction >>= 8;
        }
    }
    dst.bits.fraction = fraction;
    dst.bits.exp = exp;
    dst.bits.sign = sign;
    return dst.value;
}
