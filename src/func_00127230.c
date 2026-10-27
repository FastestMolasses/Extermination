// COMPILER: eegcc
// CFLAGS: -O2
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 94.56% NEARMISS).
// Fixes read from the original instructions:
//  - func_00126AB8 (pack_d) takes ONE argument (its byte-matched definition in
//    src/ shows it); the old C passed a second pointer.
//  - Written in the libgcc fp-bit shape of the matched twin func_00127C48
//    (__mulsf3): the parts routine is a static inline returning the result
//    pointer, the operands are taken by address as doubles, and the class
//    tests are the isnan/isinf/iszero inlines. That restores the original
//    register colouring.
//
// libgcc soft float (ee-gcc 2.9, fp-bit.c): __divdf3 with _fpdiv_parts
// inlined. Both operands are unpacked by func_00126BE8 (unpack_d) into
// 0x18-byte records (class, sign, exponent, 64-bit fraction at +0x10).
// NaN operands are returned as-is; the sign is a XOR of the operands' signs;
// inf/inf and 0/0 give the shared NaN D_00281B28 (__thenan_df), inf or zero
// numerators return themselves, x/inf gives zero and x/0 gives infinity.
// Otherwise the fractions are divided one bit at a time (restoring division)
// and rounded to nearest even on the 8 guard bits, then the result is
// repacked by func_00126AB8 (pack_d). class: 0=SNAN 1=QNAN 2=ZERO
// 3=NUMBER 4=INFINITY.
typedef struct {
    int cls;
    unsigned int sign;
    int normal_exp;
    unsigned long long fraction;
} FpNumberDf;

extern void func_00126BE8(double *in, FpNumberDf *out);
extern double func_00126AB8(FpNumberDf *in);
extern FpNumberDf D_00281B28;

static __inline__ int isnanDf (FpNumberDf *x) { return x->cls == 0 || x->cls == 1; }
static __inline__ int isinfDf (FpNumberDf *x) { return x->cls == 4; }
static __inline__ int iszeroDf (FpNumberDf *x) { return x->cls == 2; }

static __inline__ FpNumberDf *
_fpdiv_parts (FpNumberDf *a, FpNumberDf *b)
{
    unsigned long long bit;
    unsigned long long numerator;
    unsigned long long denominator;
    unsigned long long quotient;

    if (isnanDf (a))
        return a;
    if (isnanDf (b))
        return b;

    a->sign = a->sign ^ b->sign;

    if (isinfDf (a) || iszeroDf (a))
    {
        if (a->cls == b->cls)
            return &D_00281B28;
        return a;
    }
    if (isinfDf (b))
    {
        a->fraction = 0;
        a->normal_exp = 0;
        return a;
    }
    if (iszeroDf (b))
    {
        a->cls = 4;
        return a;
    }

    {
        a->normal_exp = a->normal_exp - b->normal_exp;
        numerator = a->fraction;
        denominator = b->fraction;

        if (numerator < denominator)
        {
            numerator *= 2;
            a->normal_exp--;
        }
        bit = 1ULL << 60;
        quotient = 0;
        while (bit)
        {
            if (numerator >= denominator)
            {
                quotient |= bit;
                numerator -= denominator;
            }
            bit >>= 1;
            numerator *= 2;
        }

        if ((quotient & 0xFF) == 0x80)
        {
            if (quotient & (1 << 8))
                quotient += 0x80;
            else if (numerator)
                quotient += 0x80;
        }

        a->fraction = quotient;
        return a;
    }
}

double func_00127230(double arg_a, double arg_b)
{
    FpNumberDf a;
    FpNumberDf b;
    FpNumberDf *res;

    func_00126BE8(&arg_a, &a);
    func_00126BE8(&arg_b, &b);

    res = _fpdiv_parts (&a, &b);

    return func_00126AB8(res);
}
