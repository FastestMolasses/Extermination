// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft-float (ee-gcc 2.9, dp-bit.c): _fpadd_parts, double precision.
// Adds two unpacked doubles (class/sign/exponent/fraction records produced by
// func_00126BE8) into *tmp, handling NaN/Inf/zero special cases, aligning
// exponents with sticky right-shifts, and renormalizing the result.
// class: 0=SNAN 1=QNAN 2=ZERO 3=NUMBER 4=INFINITY; fraction has the implicit
// one at bit 60 (52 mantissa bits + 8 guard bits).

typedef struct {
    int cls;                      /* fp_class_type */
    unsigned int sign;
    int normal_exp;
    unsigned long long fraction;  /* at 0x10; implicit 1 at 1<<60 */
} FpNumberDf;

extern FpNumberDf D_00281B28; /* __thenan_df */

static __inline__ int isnanDf (FpNumberDf *x) { return x->cls == 0 || x->cls == 1; } /* SNAN/QNAN */
static __inline__ int isinfDf (FpNumberDf *x) { return x->cls == 4; }
static __inline__ int iszeroDf (FpNumberDf *x) { return x->cls == 2; }

FpNumberDf *func_00126C88(FpNumberDf *a, FpNumberDf *b, FpNumberDf *tmp)
{
    long long tfraction;

    int a_normal_exp;
    int b_normal_exp;
    unsigned long long a_fraction;
    unsigned long long b_fraction;

    if (isnanDf (a))
    {
        return a;
    }
    if (isnanDf (b))
    {
        return b;
    }
    if (isinfDf (a))
    {
        /* Adding infinities with opposite signs yields a NaN.  */
        if (isinfDf (b) && a->sign != b->sign)
            return &D_00281B28;
        return a;
    }
    if (isinfDf (b))
    {
        return b;
    }
    if (iszeroDf (b))
    {
        if (iszeroDf (a))
        {
            *tmp = *a;
            tmp->sign = a->sign & b->sign;
            return tmp;
        }
        return a;
    }
    if (iszeroDf (a))
    {
        return b;
    }

    /* Got two numbers. shift the smaller and increment the exponent till
       they're the same */
    {
        int diff;

        a_normal_exp = a->normal_exp;
        b_normal_exp = b->normal_exp;
        a_fraction = a->fraction;
        b_fraction = b->fraction;

        diff = a_normal_exp - b_normal_exp;

        if (diff < 0)
            diff = -diff;
        if (diff < 64)
        {
            while (a_normal_exp > b_normal_exp)
            {
                b_normal_exp++;
                b_fraction = (b_fraction & 1) | (b_fraction >> 1);
            }
            while (b_normal_exp > a_normal_exp)
            {
                a_normal_exp++;
                a_fraction = (a_fraction & 1) | (a_fraction >> 1);
            }
        }
        else
        {
            /* Somethings's up.. choose the biggest */
            if (a_normal_exp > b_normal_exp)
            {
                b_normal_exp = a_normal_exp;
                b_fraction = 0;
            }
            else
            {
                a_normal_exp = b_normal_exp;
                a_fraction = 0;
            }
        }
    }

    if (a->sign != b->sign)
    {
        if (a->sign)
        {
            tfraction = -a_fraction + b_fraction;
        }
        else
        {
            tfraction = a_fraction - b_fraction;
        }
        if (tfraction >= 0)
        {
            tmp->sign = 0;
            tmp->normal_exp = a_normal_exp;
            tmp->fraction = tfraction;
        }
        else
        {
            tmp->sign = 1;
            tmp->normal_exp = a_normal_exp;
            tmp->fraction = -tfraction;
        }
        /* and renormalize it */

        while (tmp->fraction < (1ULL << 60) && tmp->fraction)
        {
            tmp->fraction <<= 1;
            tmp->normal_exp--;
        }
    }
    else
    {
        tmp->sign = a->sign;
        tmp->normal_exp = a_normal_exp;
        tmp->fraction = a_fraction + b_fraction;
    }
    tmp->cls = 3; /* CLASS_NUMBER */
    /* Now the fraction is added, we have to shift down to renormalize the
       number */

    if (tmp->fraction >= (1ULL << 61))
    {
        tmp->fraction = (tmp->fraction & 1) | (tmp->fraction >> 1);
        tmp->normal_exp++;
    }
    return tmp;
}
