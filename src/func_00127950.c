// COMPILER: eegcc
// CFLAGS: -O2
// libgcc soft-float (ee-gcc 2.9, fp-bit.c): _fpadd_parts, single precision.
// Adds two unpacked floats (class/sign/exponent/fraction records produced by
// func_001278C0) into *tmp, handling NaN/Inf/zero special cases, aligning
// exponents with sticky right-shifts, and renormalizing the result.
// class: 0=SNAN 1=QNAN 2=ZERO 3=NUMBER 4=INFINITY; fraction has the implicit
// one at bit 30 (23 mantissa bits + 7 guard bits).

typedef struct {
    int cls;                /* fp_class_type */
    unsigned int sign;
    int normal_exp;
    unsigned int fraction;  /* implicit 1 at 0x40000000 */
} FpNumberSf;

extern FpNumberSf D_00281B40; /* __thenan_sf */

static __inline__ int isnanSf (FpNumberSf *x) { return x->cls == 0 || x->cls == 1; } /* SNAN/QNAN */
static __inline__ int isinfSf (FpNumberSf *x) { return x->cls == 4; }
static __inline__ int iszeroSf (FpNumberSf *x) { return x->cls == 2; }

FpNumberSf *func_00127950(FpNumberSf *a, FpNumberSf *b, FpNumberSf *tmp)
{
    int tfraction;

    int a_normal_exp;
    int b_normal_exp;
    unsigned int a_fraction;
    unsigned int b_fraction;

    if (isnanSf (a))
    {
        return a;
    }
    if (isnanSf (b))
    {
        return b;
    }
    if (isinfSf (a))
    {
        /* Adding infinities with opposite signs yields a NaN.  */
        if (isinfSf (b) && a->sign != b->sign)
            return &D_00281B40;
        return a;
    }
    if (isinfSf (b))
    {
        return b;
    }
    if (iszeroSf (b))
    {
        if (iszeroSf (a))
        {
            *tmp = *a;
            tmp->sign = a->sign & b->sign;
            return tmp;
        }
        return a;
    }
    if (iszeroSf (a))
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
        if (diff < 32)
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

        while (tmp->fraction < 0x40000000 && tmp->fraction)
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

    if (tmp->fraction >= 0x80000000)
    {
        tmp->fraction = (tmp->fraction & 1) | (tmp->fraction >> 1);
        tmp->normal_exp++;
    }
    return tmp;
}
