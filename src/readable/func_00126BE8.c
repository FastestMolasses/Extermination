// NEARMISS func_00126BE8  (vram 0x00126BE8, 0x9C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 97.44% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// One ee-gcc nop in the exponent dispatch; body otherwise identical.
//
// The function links from the asm body in src/func_00126BE8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libgcc soft float (fp-bit): unpacks a double into class, sign, exponent
// and fraction (the double form of func_001278C0; fraction at +0x10). A
// zero exponent field is class 2 (zero; denormals are flushed). An all-ones
// exponent is infinity (class 4) for a zero fraction, else a NaN whose
// class is 1 when fraction bit 51 is set, 0 otherwise, keeping the raw
// fraction. Normal numbers get class 3, exponent field - 1023 and the
// fraction with the implicit one at bit 60 (8 guard bits).
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
        dst->fpclass = 2;
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
