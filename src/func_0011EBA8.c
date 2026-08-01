// NEARMISS func_0011EBA8  (vram 0x0011EBA8, 0x11CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 48.54% via ee-gcc 2.9-991111-01 (-O2 -mno-split-addresses). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-colouring + branch-likely + list-scheduler residual across a 1139-instruction function. Structure is fully recovered (every basic block, branch polarity, loop shape, call sequence and stack slot sp+0x00..0x58 matches). Residuals, none source-fixable: (1) colouring - target parks `m2` an...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2 -mno-split-addresses

/* SDK: the PS2 newlib/MSL C library's `dtoa()` -- David M. Gay's arbitrary-precision
   double -> shortest-decimal converter, the engine behind printf %e/%f/%g and
   ecvt/fcvt/gcvt.  This build is the re-entrant variant: the Bigint arena / free
   lists live in a per-locale state block passed as `ctx`, and the returned digit
   buffer is cached in ctx->result / ctx->result_k so the next dtoa() call frees it.

   Signature (reconstructed):
     char *dtoa(ctx, d, mode, ndigits, &decpt, &sign, &rve)

   The EE has no hardware double, so every double is a soft-float value carried in
   one 64-bit GPR and every operation is an explicit call into the soft-float
   runtime:
     func_00126EC8 __adddf3     func_00126F20 __subdf3
     func_00126F88 __muldf3     func_00127230 __divdf3
     func_001274B0 __cmpdf2 (<0 / 0 / >0)
     func_00127500 __floatsidf  func_001275B8 __fixdfsi
   Bigint helpers (dtoa.c names):
     func_00121AF8 Balloc    func_00121BA0 Bfree     func_00121BD0 multadd
     func_00121E30 hi0bits   func_00121F78 i2b       func_00121FB0 mult
     func_001221E0 pow5mult  func_001222E0 lshift    func_00122458 cmp
     func_001224C0 diff      func_00122868 d2b       func_0011E990 quorem
     block_copy    Bcopy
   Data:
     D_0026C818 tens[0..22]   D_0026C8E0 bigtens[0..4]   D_0026C900 bigtens[4]
     D_0026C780 0.289529654602168   D_0026C788 0.1760912590558
     D_0026C790 0.301029995663981
     D_0026C760 "Infinity"   D_0026C770 "NaN"   D_0026C778 "0"          */

typedef struct Bigint {
    struct Bigint *next; /* 0x00 */
    int k;               /* 0x04  2^k words allocated */
    int maxwds;          /* 0x08 */
    int sign;            /* 0x0C */
    int wds;             /* 0x10 */
    unsigned int x[1];   /* 0x14 */
} Bigint;

typedef struct DtoaCtx {
    char pad[0x40];
    Bigint *result; /* 0x40  cached digit buffer from the previous call */
    int result_k;   /* 0x44  its Balloc size class */
} DtoaCtx;

typedef union {
    double d;
    long L;           /* the EE `long` is 64-bit: the raw IEEE-754 bit pattern */
    unsigned long UL;
} U;

/* The compiler keeps the value in one 64-bit GPR, so the classic word0()/word1()
   accessors are plain shifts.  hi() is the signed view (a single dsra32); uhi()
   the unsigned one, needed only for the sign test. */
#define hi(u)        ((u).L >> 32)
#define word0(u)     ((int)((u).UL >> 32))
#define word1(u)     ((int)(u).L)
#define sethi(u, e)  ((u).L = ((u).L & 0xFFFFFFFFL) | ((long)(e) << 32))

#define Sign_bit  0x80000000
#define Exp_mask  0x7FF00000
#define Frac_mask 0x000FFFFF
#define Exp_11    0x3FF00000
#define Exp_msk1  0x00100000
#define Bias      0x3FF
#define P         53
#define Log2P     1
#define Ten_pmax  22
#define Quick_max 14
#define Int_max   14
#define Bletch    0x10
#define n_bigtens 5
/* !word1(d) && !(word0(d) & Frac_mask) as one 64-bit test */
#define Frac64    0x000FFFFFFFFFFFFFUL

extern Bigint *func_00121AF8(DtoaCtx *, int);              /* Balloc   */
extern void func_00121BA0(DtoaCtx *, Bigint *);            /* Bfree    */
extern Bigint *func_00121BD0(DtoaCtx *, Bigint *, int, int); /* multadd */
extern int func_00121E30(unsigned int);                    /* hi0bits  */
extern Bigint *func_00121F78(DtoaCtx *, int);              /* i2b      */
extern Bigint *func_00121FB0(DtoaCtx *, Bigint *, Bigint *); /* mult   */
extern Bigint *func_001221E0(DtoaCtx *, Bigint *, int);    /* pow5mult */
extern Bigint *func_001222E0(DtoaCtx *, Bigint *, int);    /* lshift   */
extern int func_00122458(Bigint *, Bigint *);              /* cmp      */
extern Bigint *func_001224C0(DtoaCtx *, Bigint *, Bigint *); /* diff   */
extern Bigint *func_00122868(DtoaCtx *, double, int *, int *); /* d2b   */
extern int func_0011E990(Bigint *, Bigint *);              /* quorem   */
extern void block_copy(void *, void *, int);               /* Bcopy    */

extern double func_00126EC8(double, double); /* + */
extern double func_00126F20(double, double); /* - */
extern double func_00126F88(double, double); /* * */
extern double func_00127230(double, double); /* / */
extern int func_001274B0(double, double);    /* compare */
extern double func_00127500(int);            /* (double)int */
extern int func_001275B8(double);            /* (int)double */

extern double D_0026C818[]; /* tens[]    */
extern double D_0026C8E0[]; /* bigtens[] */
extern double D_0026C900;   /* bigtens[n_bigtens-1] */
extern double D_0026C780;   /* 0.289529654602168 */
extern double D_0026C788;   /* 0.1760912590558   */
extern double D_0026C790;   /* 0.301029995663981 */
extern char D_0026C760[];   /* "Infinity" */
extern char D_0026C770[];   /* "NaN"      */
extern char D_0026C778[];   /* "0"        */

char *func_0011EBA8(DtoaCtx *ctx, double dd, int mode, int ndigits, int *decpt,
                    int *sign, char **rve)
{
    int be;
    int bbits;
    int b2;
    int b5;
    int ilim;
    int ilim0;
    int ilim1;
    int k0;
    int k_check;
    int leftright;
    int s2;
    int s5;
    int spec_case;
    int denorm;
    Bigint *b;
    Bigint *mlo;
    Bigint *mhi;
    Bigint *S;
    char *s0;
    char *s;
    Bigint *b1;
    Bigint *delta;
    U u;
    U d2;
    U eps;
    double ds;
    int dig;
    int i;
    int ieps;
    int j;
    int j1;
    int k;
    int m2;
    int m5;
    int try_quick;
    int L;
    unsigned int x;

    u.d = dd;

    /* Free the buffer handed out by the previous call. */
    if (ctx->result) {
        ctx->result->k = ctx->result_k;
        ctx->result->maxwds = 1 << ctx->result_k;
        func_00121BA0(ctx, ctx->result);
        ctx->result = 0;
    }

    if (word0(u) & Sign_bit) {
        *sign = 1;
        sethi(u, word0(u) & ~Sign_bit);
    } else {
        *sign = 0;
    }

    if ((word0(u) & Exp_mask) == Exp_mask) {
        /* Infinity or NaN */
        *decpt = 9999;
        if (!(u.UL & Frac64))
            s = D_0026C760; /* "Infinity" */
        else
            s = D_0026C770; /* "NaN" */
        if (rve)
            *rve = s[3] ? s + 8 : s + 3;
        return s;
    }

    if (func_001274B0(u.d, 0.0) == 0) {
        *decpt = 1;
        s = D_0026C778; /* "0" */
        if (rve)
            *rve = s + 1;
        return s;
    }

    b = func_00122868(ctx, u.d, &be, &bbits);
    i = (int)((unsigned int)word0(u) >> 20) & (Exp_mask >> 20);
    if (i) {
        d2 = u;
        sethi(d2, hi(d2) & Frac_mask);
        sethi(d2, hi(d2) | Exp_11);
        i -= Bias;
        denorm = 0;
    } else {
        /* d is denormalized */
        i = bbits + be + (Bias + (P - 1) - 1);
        if (i > 32)
            x = ((unsigned int)word0(u) << (64 - i)) |
                ((unsigned int)word1(u) >> (i - 32));
        else
            x = (unsigned int)word1(u) << (32 - i);
        d2.d = func_00127500((int)x);
        if ((int)x < 0)
            d2.d = func_00126EC8(d2.d, 4294967296.0);
        sethi(d2, (int)hi(d2) - 31 * Exp_msk1);
        i -= (Bias + (P - 1) - 1) + 1;
        denorm = 1;
    }

    /* ds = (d2-1.5)*0.289529654602168 + 0.1760912590558 + i*0.301029995663981 */
    ds = func_00126EC8(
        func_00126EC8(func_00126F88(func_00126F20(d2.d, 1.5), D_0026C780),
                      D_0026C788),
        func_00126F88(func_00127500(i), D_0026C790));
    k = func_001275B8(ds);
    if (func_001274B0(ds, 0.0) < 0 && func_001274B0(ds, func_00127500(k)) != 0)
        k--; /* want k = floor(ds) */
    k_check = 1;
    if ((unsigned int)k <= Ten_pmax) {
        if (func_001274B0(u.d, D_0026C818[k]) < 0)
            k--;
        k_check = 0;
    }

    j = bbits - i - 1;
    if (j < 0) {
        b2 = -j;
        s2 = 0;
    } else {
        b2 = 0;
        s2 = j;
    }
    if (k < 0) {
        b2 -= k;
        b5 = -k;
        s5 = 0;
    } else {
        b5 = 0;
        s5 = k;
        s2 += k;
    }

    try_quick = 1;
    if ((unsigned int)mode > 9)
        mode = 0;
    if (mode > 5) {
        mode -= 4;
        try_quick = 0;
    }
    leftright = 1;
    switch (mode) {
    case 0:
    case 1:
        i = 18;
        ndigits = 0;
        ilim = -1;
        ilim1 = -1;
        break;
    case 2:
        leftright = 0;
        /* fallthrough */
    case 4:
        if (ndigits <= 0)
            ndigits = 1;
        i = ndigits;
        ilim = ndigits;
        ilim1 = ndigits;
        break;
    case 3:
        leftright = 0;
        /* fallthrough */
    case 5:
        i = ndigits + k + 1;
        ilim = i;
        ilim1 = i - 1;
        if (i <= 0)
            i = 1;
        break;
    }

    /* rv_alloc(i), inlined: pick the Balloc size class that holds i chars. */
    j = 4;
    ctx->result_k = 0;
    while ((unsigned int)i >= (unsigned int)(j + 20)) {
        j *= 2;
        ctx->result_k++;
    }
    ctx->result = func_00121AF8(ctx, ctx->result_k);
    s0 = (char *)ctx->result;
    s = s0;

    if ((unsigned int)ilim < 15 && try_quick) {
        /* Try to get by with floating-point arithmetic. */
        d2 = u;
        k0 = k;
        ilim0 = ilim;
        ieps = 2; /* conservative */
        if (k > 0) {
            ds = D_0026C818[k & 0xF];
            j = k >> 4;
            if (j & Bletch) {
                /* prevent overflows */
                j &= Bletch - 1;
                u.d = func_00127230(u.d, D_0026C900);
                ieps++;
            }
            if (j) {
                double *bt = D_0026C8E0;
                do {
                    if (j & 1) {
                        ieps++;
                        ds = func_00126F88(ds, *bt);
                    }
                    j >>= 1;
                    bt++;
                } while (j);
            }
            u.d = func_00127230(u.d, ds);
        } else if ((j1 = -k) != 0) {
            u.d = func_00126F88(D_0026C818[j1 & 0xF], u.d);
            j = j1 >> 4;
            if (j) {
                double *bt = D_0026C8E0;
                do {
                    if (j & 1) {
                        ieps++;
                        u.d = func_00126F88(*bt, u.d);
                    }
                    j >>= 1;
                    bt++;
                } while (j);
            }
        }
        if (k_check && func_001274B0(u.d, 1.0) < 0 && ilim > 0) {
            if (ilim1 <= 0)
                goto fast_failed;
            ilim = ilim1;
            k--;
            u.d = func_00126F88(u.d, 10.0);
            ieps++;
        }
        eps.d = func_00126EC8(func_00126F88(func_00127500(ieps), u.d), 7.0);
        sethi(eps, (int)hi(eps) - (P - 1) * Exp_msk1);
        if (ilim == 0) {
            S = 0;
            mhi = 0;
            u.d = func_00126F20(u.d, 5.0);
            if (func_001274B0(u.d, eps.d) > 0)
                goto one_digit;
            if (func_001274B0(u.d, func_00126F20(0.0, eps.d)) < 0)
                goto no_digits;
            goto fast_failed;
        }
        if (leftright) {
            /* Use Steele & White's method of only generating the digits needed. */
            eps.d = func_00126F20(func_00127230(0.5, D_0026C818[ilim - 1]), eps.d);
            i = 0;
            for (;;) {
                L = func_001275B8(u.d);
                u.d = func_00126F20(u.d, func_00127500(L));
                *s++ = '0' + L;
                if (func_001274B0(u.d, eps.d) < 0)
                    goto ret1;
                if (func_001274B0(func_00126F20(1.0, u.d), eps.d) < 0)
                    goto bump_up;
                if (++i >= ilim)
                    break;
                eps.d = func_00126F88(10.0, eps.d);
                u.d = func_00126F88(10.0, u.d);
            }
        } else {
            /* Generate ilim digits, then fix them up. */
            eps.d = func_00126F88(D_0026C818[ilim - 1], eps.d);
            i = 1;
            for (;;) {
                L = func_001275B8(u.d);
                u.d = func_00126F20(u.d, func_00127500(L));
                *s++ = '0' + L;
                if (i == ilim)
                    break;
                i++;
                u.d = func_00126F88(10.0, u.d);
            }
            if (func_001274B0(u.d, func_00126EC8(0.5, eps.d)) > 0)
                goto bump_up;
            if (func_001274B0(u.d, func_00126F20(0.5, eps.d)) < 0) {
                while (*--s == '0')
                    ;
                s++;
                goto ret1;
            }
        }
    fast_failed:
        s = s0;
        u = d2;
        k = k0;
        ilim = ilim0;
    }

    /* Do we have a "small" integer? */
    if (be >= 0 && k <= Int_max) {
        ds = D_0026C818[k];
        if (ndigits < 0 && ilim <= 0) {
            S = 0;
            mhi = 0;
            if (ilim < 0 || func_001274B0(u.d, func_00126F88(ds, 5.0)) <= 0)
                goto no_digits;
            goto one_digit;
        }
        i = 1;
        for (;;) {
            L = func_001275B8(func_00127230(u.d, ds));
            u.d = func_00126F20(u.d, func_00126F88(func_00127500(L), ds));
            *s++ = '0' + L;
            if (i == ilim) {
                u.d = func_00126EC8(u.d, u.d);
                if (func_001274B0(u.d, ds) > 0 ||
                    (func_001274B0(u.d, ds) == 0 && (L & 1)))
                    goto bump_up;
                break;
            }
            u.d = func_00126F88(10.0, u.d);
            if (func_001274B0(u.d, 0.0) == 0)
                break;
            i++;
        }
        goto ret1;
    }

    m2 = b2;
    m5 = b5;
    mhi = 0;
    mlo = 0;
    if (leftright) {
        if (mode < 2) {
            i = denorm ? be + (Bias + (P - 1) - 1 + 1) : 1 + P - bbits;
        } else {
            j = ilim - 1;
            if (b5 >= j) {
                m5 = b5 - j;
            } else {
                m5 = 0;
                j -= b5;
                b5 += j;
                s5 += j;
            }
            i = ilim;
            if (i < 0) {
                m2 = b2 - i;
                i = 0;
            }
        }
        mhi = func_00121F78(ctx, 1);
        s2 += i;
        b2 += i;
    }
    spec_case = 0;
    if (mode < 2) {
        /* Is d a normalized power of 2?  Then the lower bound is closer. */
        if (!(u.UL & Frac64) && ((int)hi(u) & Exp_mask)) {
            spec_case = 1;
            b2 += Log2P;
            s2 += Log2P;
        }
    }

    if (m2 > 0 && s2 > 0) {
        i = m2 < s2 ? m2 : s2;
        s2 -= i;
        m2 -= i;
        b2 -= i;
    }
    if (b5 > 0) {
        if (leftright) {
            if (m5 > 0) {
                mhi = func_001221E0(ctx, mhi, m5);
                b1 = func_00121FB0(ctx, mhi, b);
                func_00121BA0(ctx, b);
                b = b1;
            }
            if ((j = b5 - m5) != 0)
                b = func_001221E0(ctx, b, j);
        } else {
            b = func_001221E0(ctx, b, b5);
        }
    }
    S = func_00121F78(ctx, 1);
    if (s5 > 0)
        S = func_001221E0(ctx, S, s5);

    /* Shift left so the divisor has 4 leading zero bits, for quorem(). */
    if (s5)
        i = (s2 - func_00121E30(S->x[S->wds - 1])) & 0x1F;
    else
        i = (s2 + 1) & 0x1F;
    if (i)
        i = 32 - i;
    if (i > 4) {
        i -= 4;
        m2 += i;
        s2 += i;
        b2 += i;
    } else if (i < 4) {
        i += 28;
        m2 += i;
        s2 += i;
        b2 += i;
    }
    if (b2 > 0)
        b = func_001222E0(ctx, b, b2);
    if (s2 > 0)
        S = func_001222E0(ctx, S, s2);
    if (k_check) {
        if (func_00122458(b, S) < 0) {
            /* we botched the k estimate */
            b = func_00121BD0(ctx, b, 10, 0);
            k--;
            if (leftright)
                mhi = func_00121BD0(ctx, mhi, 10, 0);
            ilim = ilim1;
        }
    }
    if (ilim <= 0 && mode >= 3) {
        if (ilim < 0)
            goto no_digits;
        S = func_00121BD0(ctx, S, 5, 0);
        if (func_00122458(b, S) <= 0)
            goto no_digits;
        goto one_digit;
    }

    if (leftright) {
        if (m2 > 0)
            mhi = func_001222E0(ctx, mhi, m2);

        /* mlo/mhi: the lower and upper half-ulp bounds. */
        mlo = mhi;
        if (spec_case) {
            mhi = func_00121AF8(ctx, mhi->k);
            block_copy(&mhi->sign, &mlo->sign, mlo->wds * 4 + 8);
            mhi = func_001222E0(ctx, mhi, Log2P);
        }

        i = 1;
        for (;;) {
            dig = func_0011E990(b, S) + '0';
            /* Do we yet have the shortest decimal string that rounds to d? */
            j = func_00122458(b, mlo);
            delta = func_001224C0(ctx, S, mhi);
            j1 = delta->sign ? 1 : func_00122458(b, delta);
            func_00121BA0(ctx, delta);
            if (j1 == 0 && mode == 0 && !(word1(u) & 1)) {
                if (dig == '9')
                    goto round_9_up;
                if (j > 0)
                    dig++;
                *s++ = dig;
                goto ret;
            }
            if (j < 0 || (j == 0 && mode == 0 && !(word1(u) & 1))) {
                if (j1 > 0) {
                    /* possible if i == 1 */
                    b = func_001222E0(ctx, b, 1);
                    j1 = func_00122458(b, S);
                    if ((j1 > 0 || (j1 == 0 && (dig & 1))) && dig++ == '9')
                        goto round_9_up;
                }
                *s++ = dig;
                goto ret;
            }
            if (j1 > 0) {
                if (dig == '9') { /* possible if i == 1 */
                round_9_up:
                    *s++ = '9';
                    goto roundoff;
                }
                *s++ = dig + 1;
                goto ret;
            }
            *s++ = dig;
            if (i == ilim)
                break;
            b = func_00121BD0(ctx, b, 10, 0);
            if (mlo == mhi) {
                mhi = func_00121BD0(ctx, mhi, 10, 0);
                mlo = mhi;
            } else {
                mlo = func_00121BD0(ctx, mlo, 10, 0);
                mhi = func_00121BD0(ctx, mhi, 10, 0);
            }
            i++;
        }
    } else {
        i = 1;
        for (;;) {
            dig = func_0011E990(b, S) + '0';
            *s++ = dig;
            if (i >= ilim)
                break;
            i++;
            b = func_00121BD0(ctx, b, 10, 0);
        }
    }

    /* Round off the last digit. */
    b = func_001222E0(ctx, b, 1);
    j = func_00122458(b, S);
    if (j > 0 || (j == 0 && (dig & 1))) {
    roundoff:
        while (*--s == '9') {
            if (s == s0) {
                k++;
                *s++ = '1';
                goto ret;
            }
        }
        ++*s++;
    } else {
        while (*--s == '0')
            ;
        s++;
    }
    goto ret;

no_digits:
    k = -1 - ndigits;
    goto ret;

one_digit:
    *s++ = '1';
    k++;
    goto ret;

bump_up:
    while (*--s == '9') {
        if (s == s0) {
            k++;
            *s = '0';
            break;
        }
    }
    ++*s++;
    goto ret1;

ret:
    func_00121BA0(ctx, S);
    if (mhi) {
        if (mlo && mlo != mhi)
            func_00121BA0(ctx, mlo);
        func_00121BA0(ctx, mhi);
    }
ret1:
    func_00121BA0(ctx, b);
    *s = 0;
    *decpt = k + 1;
    if (rve)
        *rve = s;
    return s0;
}
