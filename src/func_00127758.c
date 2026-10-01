// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft float (fp-bit): __truncdfsf2, double to float. The double is
// unpacked by func_00126BE8; its 64-bit fraction is cut to the float's 32
// (shifted down by 30, any lost bit kept as a sticky low bit) and the parts
// are repacked as a float by func_00128320.
typedef struct FpNumberD {
    unsigned int fpclass;
    unsigned int sign;
    int normal_exp;
    int pad;
    unsigned long long fraction;
} FpNumberD;

extern void func_00126BE8(double *src, FpNumberD *dst);
extern float func_00128320(unsigned int fpclass, unsigned int sign, int exp, unsigned int fraction);

float func_00127758(double arg) {
    FpNumberD a;
    double in;
    unsigned int tmp;

    in = arg;
    func_00126BE8(&in, &a);
    tmp = a.fraction >> 30;
    if (a.fraction & 0x3FFFFFFF) {
        tmp |= 1;
    }
    return func_00128320(a.fpclass, a.sign, a.normal_exp, tmp);
}
