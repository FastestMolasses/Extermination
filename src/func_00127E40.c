// COMPILER: eegcc
// CFLAGS: -O2
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 50.84% NEARMISS).
// Fixes read from the original instructions:
//  - Wrong compiler: the doubleword saves of $ra/$s0, the 0x50 frame and the
//    unfilled call slot are ee-gcc's, not CodeWarrior's. This is libgcc fp-bit
//    code, built with ee-gcc -O2 like its byte-matched neighbours
//    func_00127230 (__divdf3) and func_00127C48 (__mulsf3).
//  - Identified as __divsf3: written in fp-bit's own shape (static inline
//    _fpdiv_parts returning the result pointer, isnan/isinf/iszero inlines,
//    operands taken by address). It returns a float (pack_f's result) and
//    pack_f (func_001277B0) takes ONE argument, not two.
//
// libgcc soft float (ee-gcc 2.9, fp-bit.c): __divsf3 with _fpdiv_parts
// inlined. Both operands are unpacked by func_001278C0 (unpack_f) into 4-word
// records (class, sign, exponent, fraction with the implicit 1 at 0x40000000).
// NaN operands are returned as-is; the sign is the XOR of the operands' signs;
// inf/inf and 0/0 give the shared NaN D_00281B40 (__thenan_sf), inf or zero
// numerators return themselves, x/inf gives zero and x/0 gives infinity.
// Otherwise the fractions are divided one bit at a time (restoring division)
// and rounded to nearest even on the 7 guard bits, then the result is
// repacked by func_001277B0 (pack_f). class: 0=SNAN 1=QNAN 2=ZERO
// 3=NUMBER 4=INFINITY.
typedef struct {
    int cls;
    unsigned int sign;
    int normal_exp;
    unsigned int fraction;
} FpNumberSf;

extern void func_001278C0(float *in, FpNumberSf *out);
extern float func_001277B0(FpNumberSf *in);
extern FpNumberSf D_00281B40;

static __inline__ int isnanSf (FpNumberSf *x) { return x->cls == 0 || x->cls == 1; }
static __inline__ int isinfSf (FpNumberSf *x) { return x->cls == 4; }
static __inline__ int iszeroSf (FpNumberSf *x) { return x->cls == 2; }

static __inline__ FpNumberSf *
_fpdiv_parts (FpNumberSf *a, FpNumberSf *b)
{
    unsigned int bit;
    unsigned int numerator;
    unsigned int denominator;
    unsigned int quotient;

    if (isnanSf (a))
        return a;
    if (isnanSf (b))
        return b;

    a->sign = a->sign ^ b->sign;

    if (isinfSf (a) || iszeroSf (a))
    {
        if (a->cls == b->cls)
            return &D_00281B40;
        return a;
    }
    if (isinfSf (b))
    {
        a->fraction = 0;
        a->normal_exp = 0;
        return a;
    }
    if (iszeroSf (b))
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
        bit = 0x40000000;
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

        if ((quotient & 0x7F) == 0x40)
        {
            if (quotient & (1 << 7))
                quotient += 0x40;
            else if (numerator)
                quotient += 0x40;
        }

        a->fraction = quotient;
        return a;
    }
}

float func_00127E40(float arg_a, float arg_b)
{
    FpNumberSf a;
    FpNumberSf b;
    FpNumberSf *res;

    func_001278C0(&arg_a, &a);
    func_001278C0(&arg_b, &b);

    res = _fpdiv_parts (&a, &b);

    return func_001277B0(res);
}
