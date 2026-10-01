// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft float (fp-bit): __extendsfdf2, float to double. The float is
// unpacked by func_001278C0 and repacked as a double by func_00127728 with
// the fraction widened to 64 bits (shifted up by 30: the guard bits of a
// double sit one bit higher).
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
extern double func_00127728(unsigned int fpclass, unsigned int sign, int exp, unsigned long long fraction);

double func_00128350(float arg) {
    FpNumber a;
    float in;

    in = arg;
    func_001278C0(&in, &a);
    return func_00127728(a.fpclass, a.sign, a.normal_exp, (unsigned long long)a.fraction << 30);
}
