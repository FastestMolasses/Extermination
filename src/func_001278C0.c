// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an 86.22% NEARMISS
// companion in src/readable/). Fixes read from the original instructions:
//  - written in libgcc fp-bit's own unpack_f shape: the source is FLO_union_type
//    and fraction, exp and sign are read as its bit-fields (fraction:23, exp:8,
//    sign:1) into separate locals, instead of shifting and masking one word (that
//    let gcc reuse the word's register and move the sign store). The
//    zero-exponent arm keeps fp-bit's denormal branch, disabled by the PS2 build's
//    NO_DENORMALS, as in the double twin func_00126BE8.
//
// libgcc soft float (fp-bit): unpacks a float into class, sign, exponent and
// fraction. A zero exponent field is class 2 (zero; denormals are flushed). An
// all-ones exponent is infinity (class 4) for a zero fraction, else a NaN whose
// class is 1 when fraction bit 20 is set, 0 otherwise, keeping the raw fraction.
// Normal numbers get class 3, exponent field - 127 and the fraction with the
// implicit one at bit 30 (7 guard bits).
#define NO_DENORMALS 1

typedef struct FpNumber {
    unsigned int fpclass;
    unsigned int sign;
    int normal_exp;
    unsigned int fraction;
} FpNumber;

typedef union {
    float value;
    struct {
        unsigned int fraction:23;
        unsigned int exp:8;
        unsigned int sign:1;
    } bits;
} FLO_union_type;

void func_001278C0(FLO_union_type *src, FpNumber *dst) {
    unsigned int fraction;
    int exp;
    int sign;

    fraction = src->bits.fraction;
    exp = src->bits.exp;
    sign = src->bits.sign;

    dst->sign = sign;
    if (exp == 0) {
        if (fraction == 0 || NO_DENORMALS) {
            dst->fpclass = 2;
        } else {
            /* denormal: no implicit one, shift until there is one */
            dst->normal_exp = exp - 127 + 1;
            fraction <<= 7;
            dst->fpclass = 3;
            while (fraction < 0x40000000) {
                fraction <<= 1;
                dst->normal_exp--;
            }
            dst->fraction = fraction;
        }
    } else if (exp == 0xFF) {
        if (fraction == 0) {
            dst->fpclass = 4;
        } else {
            if (fraction & 0x100000) {
                dst->fpclass = 1;
            } else {
                dst->fpclass = 0;
            }
            dst->fraction = fraction;
        }
    } else {
        dst->normal_exp = exp - 127;
        dst->fpclass = 3;
        dst->fraction = (fraction << 7) | 0x40000000;
    }
}
