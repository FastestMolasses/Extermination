// COMPILER: eegcc
// CFLAGS: -O2
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 59.59% NEARMISS).
// Fixes read from the original instructions:
//  - This is libgcc fp-bit __muldf3 with _fpmul_parts inlined, written in the
//    shape of the matched twins func_00127230 (__divdf3) and func_00127C48
//    (__mulsf3): double arguments taken by address, isnan/isinf/iszero
//    inlines, a static inline parts routine returning the result pointer,
//    and func_00126AB8 (pack_d) with one argument returning a double.
//  - The class checks follow fp-bit's order (NaN a, NaN b, inf a, inf b,
//    zero a, zero b); the old C had a different, partly wrong cascade.
//  - The fraction is fp-bit's union (64-bit ll / two 32-bit halves); a plain
//    64-bit field schedules the final fraction store differently.
//  - The 64x64 product is fp-bit's four __muldi3 partial products
//    (func_00125F48, named explicitly so gcc does not emit __muldi3), with
//    the cross-sum shifted straight into the low sum (no reassigned pp_hl
//    temporary) and its high half masked with 0xffffffff before adding.
//
// libgcc soft float (ee-gcc 2.9, fp-bit.c): __muldf3. Both operands are
// unpacked by func_00126BE8 (unpack_d) into 0x18-byte records (class, sign,
// exponent, 64-bit fraction at +0x10). NaN, infinity and zero operands are
// returned with the XOR of the signs; inf*0 gives the shared NaN D_00281B28
// (__thenan_df). Otherwise the fractions are multiplied into a 128-bit
// product, normalized so the implicit bit sits at bit 60, rounded to nearest
// even on the 8 guard bits, and repacked by func_00126AB8 (pack_d).
// class: 0=SNAN 1=QNAN 2=ZERO 3=NUMBER 4=INFINITY.
typedef unsigned long long UDItype;
typedef struct {
    int cls;
    unsigned int sign;
    int normal_exp;
    union { unsigned long long ll; unsigned int l[2]; } fraction;
} FpNumberDf;

extern void func_00126BE8(double *in, FpNumberDf *out);
extern double func_00126AB8(FpNumberDf *in);
extern FpNumberDf D_00281B28;
extern UDItype func_00125F48(UDItype a, UDItype b); /* __muldi3 */

static __inline__ int isnanDf (FpNumberDf *x) { return x->cls == 0 || x->cls == 1; }
static __inline__ int isinfDf (FpNumberDf *x) { return x->cls == 4; }
static __inline__ int iszeroDf (FpNumberDf *x) { return x->cls == 2; }

static __inline__ FpNumberDf *
_fpmul_parts (FpNumberDf *a, FpNumberDf *b, FpNumberDf *tmp)
{
    UDItype low = 0;
    UDItype high = 0;

    if (isnanDf (a))
    {
        a->sign = a->sign != b->sign;
        return a;
    }
    if (isnanDf (b))
    {
        b->sign = a->sign != b->sign;
        return b;
    }
    if (isinfDf (a))
    {
        if (iszeroDf (b))
            return &D_00281B28;
        a->sign = a->sign != b->sign;
        return a;
    }
    if (isinfDf (b))
    {
        if (iszeroDf (a))
        {
            return &D_00281B28;
        }
        b->sign = a->sign != b->sign;
        return b;
    }
    if (iszeroDf (a))
    {
        a->sign = a->sign != b->sign;
        return a;
    }
    if (iszeroDf (b))
    {
        b->sign = a->sign != b->sign;
        return b;
    }

    {
        UDItype nl = a->fraction.ll & 0xffffffff;
        UDItype nh = a->fraction.ll >> 32;
        UDItype ml = b->fraction.ll & 0xffffffff;
        UDItype mh = b->fraction.ll >> 32;
        UDItype pp_ll = func_00125F48(ml, nl);
        UDItype pp_hl = func_00125F48(mh, nl);
        UDItype pp_lh = func_00125F48(ml, nh);
        UDItype pp_hh = func_00125F48(mh, nh);
        UDItype res2 = 0;
        UDItype res0 = 0;
        UDItype ps_hh__ = pp_hl + pp_lh;
        if (ps_hh__ < pp_hl)
            res2 += (UDItype)1 << 32;
        res0 = pp_ll + (ps_hh__ << 32);
        if (res0 < pp_ll)
            res2++;
        res2 += ((ps_hh__ >> 32) & 0xffffffff) + pp_hh;
        high = res2;
        low = res0;
    }

    tmp->normal_exp = a->normal_exp + b->normal_exp;
    tmp->sign = a->sign != b->sign;
    tmp->normal_exp += 4;
    while (high >= (1ULL << 61))
    {
        tmp->normal_exp++;
        if (high & 1)
        {
            low >>= 1;
            low |= 0x8000000000000000ULL;
        }
        high >>= 1;
    }
    while (high < (1ULL << 60))
    {
        tmp->normal_exp--;

        high <<= 1;
        if (low & 0x8000000000000000ULL)
            high |= 1;
        low <<= 1;
    }
    if ((high & 0xFF) == 0x80)
    {
        if (high & (1 << 8))
        {
            high += 0x80;
        }
        else if (low)
        {
            high += 0x80;
        }
    }
    tmp->fraction.ll = high;
    tmp->cls = 3;
    return tmp;
}

double func_00126F88(double arg_a, double arg_b)
{
    FpNumberDf a;
    FpNumberDf b;
    FpNumberDf tmp;
    FpNumberDf *res;

    func_00126BE8(&arg_a, &a);
    func_00126BE8(&arg_b, &b);

    res = _fpmul_parts (&a, &b, &tmp);

    return func_00126AB8(res);
}
