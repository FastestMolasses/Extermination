// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft float (fp-bit): packs an unpacked number (layout as in
// func_001278C0) back into a float. NaNs keep their fraction with quiet bit
// 20 set and exponent 0xFF; infinities and exponents above 127 become
// infinity; zero and a zero fraction keep exponent 0. Exponents below -126
// shift the fraction down into a denormal (nothing left beyond 25 places).
// Normal numbers round to nearest even on the 7 guard bits (0x40 exactly
// halfway rounds up only from an odd result) and renormalise on carry. The
// fields are stored through a float bitfield union.
typedef struct FpNumber {
    unsigned int fpclass;
    unsigned int sign;
    int normal_exp;
    unsigned int fraction;
} FpNumber;

typedef union FloatUnion {
    float value;
    struct {
        unsigned int fraction : 23;
        unsigned int exp : 8;
        unsigned int sign : 1;
    } bits;
} FloatUnion;

static __inline__ int isnan(FpNumber *x) {
    return x->fpclass < 2;
}

static __inline__ int iszero(FpNumber *x) {
    return x->fpclass == 2;
}

static __inline__ int isinf(FpNumber *x) {
    return x->fpclass == 4;
}

float func_001277B0(FpNumber *src) {
    FloatUnion dst;
    unsigned int fraction = src->fraction;
    int sign = src->sign;
    int exp = 0;
    int shift;

    if (isnan(src)) {
        exp = 0xFF;
        fraction |= 0x100000;
    } else if (isinf(src)) {
        exp = 0xFF;
        fraction = 0;
    } else if (iszero(src)) {
        fraction = 0;
    } else if (fraction != 0) {
        if (src->normal_exp < -126) {
            shift = -126 - src->normal_exp;
            if (shift > 25) {
                fraction = 0;
            } else {
                fraction >>= shift;
            }
            fraction >>= 7;
        } else if (src->normal_exp > 127) {
            exp = 0xFF;
            fraction = 0;
        } else {
            exp = src->normal_exp + 127;
            if ((fraction & 0x7F) == 0x40) {
                if (fraction & 0x80) {
                    fraction += 0x40;
                }
            } else {
                fraction += 0x3F;
            }
            if (fraction >= 0x80000000) {
                fraction >>= 1;
                exp += 1;
            }
            fraction >>= 7;
        }
    }
    dst.bits.fraction = fraction;
    dst.bits.exp = exp;
    dst.bits.sign = sign;
    return dst.value;
}
