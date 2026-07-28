// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft-float (ee-gcc 2.9, fp-bit.c): __mulsf3 — single-precision
// multiply with _fpmul_parts inlined. Unpacks both operands via
// func_001278C0, multiplies the 32-bit fractions into a 64-bit product,
// normalizes, rounds to even on the 7 guard bits, and repacks via
// func_001277B0. class: 0=SNAN 1=QNAN 2=ZERO 3=NUMBER 4=INFINITY.

typedef struct {
    int cls;                /* fp_class_type */
    unsigned int sign;
    int normal_exp;
    unsigned int fraction;  /* implicit 1 at 0x40000000 */
} FpNumberSf;

extern void func_001278C0(float *in, FpNumberSf *out); /* unpack_f */
extern float func_001277B0(FpNumberSf *in);            /* pack_f */
extern FpNumberSf D_00281B40;                          /* __thenan_sf */

static __inline__ int isnanSf (FpNumberSf *x) { return x->cls == 0 || x->cls == 1; } /* SNAN/QNAN */
static __inline__ int isinfSf (FpNumberSf *x) { return x->cls == 4; }
static __inline__ int iszeroSf (FpNumberSf *x) { return x->cls == 2; }

static __inline__ FpNumberSf *
_fpmul_parts (FpNumberSf *a, FpNumberSf *b, FpNumberSf *tmp)
{
    unsigned int low = 0;
    unsigned int high = 0;

    if (isnanSf (a))
    {
        a->sign = a->sign != b->sign;
        return a;
    }
    if (isnanSf (b))
    {
        b->sign = a->sign != b->sign;
        return b;
    }
    if (isinfSf (a))
    {
        if (iszeroSf (b))
            return &D_00281B40;
        a->sign = a->sign != b->sign;
        return a;
    }
    if (isinfSf (b))
    {
        if (iszeroSf (a))
        {
            return &D_00281B40;
        }
        b->sign = a->sign != b->sign;
        return b;
    }
    if (iszeroSf (a))
    {
        a->sign = a->sign != b->sign;
        return a;
    }
    if (iszeroSf (b))
    {
        b->sign = a->sign != b->sign;
        return b;
    }

    /* Calculate the mantissa by multiplying both fractions to get a
       64 bit number */
    {
        unsigned long long answer =
            (unsigned long long)a->fraction * (unsigned long long)b->fraction;

        high = answer >> 32;
        low = answer;
    }

    tmp->normal_exp = a->normal_exp + b->normal_exp;
    tmp->sign = a->sign != b->sign;
    tmp->normal_exp += 2;
    while (high >= 0x80000000)
    {
        tmp->normal_exp++;
        if (high & 1)
        {
            low >>= 1;
            low |= 0x80000000;
        }
        high >>= 1;
    }
    while (high < 0x40000000)
    {
        tmp->normal_exp--;

        high <<= 1;
        if (low & 0x80000000)
            high |= 1;
        low <<= 1;
    }
    /* rounding is tricky. if we only round if it won't make us round
       later.  */
    if ((high & 0x7F) == 0x40)
    {
        if (high & 0x80)
        {
            /* half way, so round to even */
            high += 0x40;
        }
        else if (low)
        {
            /* but we really weren't half way */
            high += 0x40;
        }
    }
    tmp->fraction = high;
    tmp->cls = 3; /* CLASS_NUMBER */
    return tmp;
}

float func_00127C48(float arg_a, float arg_b)
{
    FpNumberSf a;
    FpNumberSf b;
    FpNumberSf tmp;
    FpNumberSf *res;

    func_001278C0(&arg_a, &a);
    func_001278C0(&arg_b, &b);

    res = _fpmul_parts (&a, &b, &tmp);

    return func_001277B0(res);
}
