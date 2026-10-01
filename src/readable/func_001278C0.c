// NEARMISS func_001278C0  (vram 0x001278C0, 0x90 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 86.22% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc scheduling of the field extraction and the class stores (structure and constants follow
// the original).
//
// The function links from the asm body in src/func_001278C0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libgcc soft float (fp-bit): unpacks a float into class, sign, exponent
// and fraction (see float_to_int for the layout). A zero exponent field is
// class 2 (zero; denormals are flushed). An all-ones exponent is infinity
// (class 4) for a zero fraction, else a NaN whose class is 1 when fraction
// bit 20 is set, 0 otherwise, keeping the raw fraction. Normal numbers get
// class 3, exponent field - 127 and the fraction with the implicit one at
// bit 30.
typedef struct FpNumber {
    unsigned int fpclass;
    unsigned int sign;
    int normal_exp;
    unsigned int fraction;
} FpNumber;

void func_001278C0(unsigned int *src, FpNumber *dst) {
    unsigned int bits = *src;
    unsigned int fraction = bits & 0x7FFFFF;
    int exp = (bits >> 23) & 0xFF;

    dst->sign = bits >> 31;
    if (exp == 0) {
        dst->fpclass = 2;
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
