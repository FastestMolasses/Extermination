// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 97.44% NEARMISS).
// Fixes read from the original instructions:
// - Written in libgcc fp-bit's own unpack_d shape: the zero-exponent arm keeps the
//   denormal-normalising branch (with its shift loop), disabled by the PS2 build's
//   NO_DENORMALS setting. gcc deletes the dead branch but the loop it held still
//   aligns the following label, which is the lone padding word after the zero
//   case's return that the old C lacked. Behaviour is unchanged.

// libgcc soft float (fp-bit): unpacks a double into class, sign, exponent
// and fraction (the double form of func_001278C0; fraction at +0x10). A
// zero exponent field is class 2 (zero; denormals are flushed). An all-ones
// exponent is infinity (class 4) for a zero fraction, else a NaN whose
// class is 1 when fraction bit 51 is set, 0 otherwise, keeping the raw
// fraction. Normal numbers get class 3, exponent field - 1023 and the
// fraction with the implicit one at bit 60 (8 guard bits).
#define NO_DENORMALS 1

typedef struct FpNumberD {
    unsigned int fpclass;
    unsigned int sign;
    int normal_exp;
    int pad;
    unsigned long long fraction;
} FpNumberD;

void func_00126BE8(unsigned long long *src, FpNumberD *dst) {
    unsigned long long bits = *src;
    unsigned long long fraction = bits & 0xFFFFFFFFFFFFFULL;
    int exp = (int)(bits >> 52) & 0x7FF;

    dst->sign = bits >> 63;
    if (exp == 0) {
        if (fraction == 0 || NO_DENORMALS) {
            dst->fpclass = 2;
        } else {
            /* denormal: no implicit one, shift until there is one */
            dst->normal_exp = exp - 1023 + 1;
            fraction <<= 8;
            dst->fpclass = 3;
            while (fraction < 0x1000000000000000ULL) {
                fraction <<= 1;
                dst->normal_exp--;
            }
            dst->fraction = fraction;
        }
    } else if (exp == 0x7FF) {
        if (fraction == 0) {
            dst->fpclass = 4;
        } else {
            if (fraction & 0x8000000000000ULL) {
                dst->fpclass = 1;
            } else {
                dst->fpclass = 0;
            }
            dst->fraction = fraction;
        }
    } else {
        dst->normal_exp = exp - 1023;
        dst->fpclass = 3;
        dst->fraction = (fraction << 8) | 0x1000000000000000ULL;
    }
}
