// NEARMISS func_0011EBA8  (vram 0x0011EBA8, 0x11CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.68% via ee-gcc 2.9-991111-01 (-O2). The instruction stream is IDENTICAL to the
// original (same 0x11CC bytes of code, every opcode, register and branch); what remains is
// relocation spelling only:
// Remaining differences: (1) the function's own .rodata literal pool ("Infinity", "NaN", "0",
// the three log10 constants and the mode-switch jump table, laid out exactly as the original
// 0x0026C760..0x0026C7B8 block: offsets 0/16/24/32/40/48/64) is relocated against the local
// .rodata section, while the expected object names splat's split symbols D_0026C760..D_0026C790
// (only the jump table is local, via build/jtblrodata); (2) ee-gcc emits the SCE soft-float
// libcalls by name (dpadd dpsub dpmul dpdiv dpcmp litodp dptoli), which the symbol map knows as
// func_00126EC8 / func_00126F20 / func_00126F88 / func_00127230 / func_001274B0 / func_00127500 /
// func_001275B8.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

/* SDK: newlib's _dtoa_r (David M. Gay's dtoa, newlib libc/stdlib/dtoa.c), written in
   newlib's own shape with native double arithmetic: ee-gcc lowers every double
   operation to the soft-float runtime (add/sub/mul/div/compare/int<->double), and
   the unsigned->double conversion of the denormal path adds 2^32 itself.
   The previous call's digit buffer is cached in ptr->_result / _result_k (+0x40/+0x44)
   and freed first. Bigint helpers: func_00121AF8 Balloc, func_00121BA0 Bfree,
   func_00121BD0 multadd, func_00121E30 hi0bits, func_00121F78 i2b, func_00121FB0 mult,
   func_001221E0 pow5mult, func_001222E0 lshift, func_00122458 cmp, func_001224C0 diff,
   func_00122868 d2b, func_0011E990 quorem, block_copy (Bcopy's memcpy).
   Data: D_0026C818 tens[0..22], D_0026C8E0 bigtens[0..4] (both const). */

typedef struct _Bigint {
    struct _Bigint *_next;
    int _k, _maxwds, _sign, _wds;
    unsigned int _x[1];
} _Bigint;

struct _reent {
    char pad[0x40];
    _Bigint *_result;
    int _result_k;
};

union double_union {
    double d;
    unsigned int i[2];
};

#define word0(x) ((x).i[1])
#define word1(x) ((x).i[0])

#define Exp_shift1 20
#define Exp_msk1 0x100000
#define Exp_mask 0x7ff00000
#define P 53
#define Bias 1023
#define Exp_1 0x3ff00000
#define Exp_11 0x3ff00000
#define Ebits 11
#define Frac_mask 0xfffff
#define Frac_mask1 0xfffff
#define Ten_pmax 22
#define Bletch 0x10
#define Bndry_mask 0xfffff
#define Log2P 1
#define Sign_bit 0x80000000
#define Quick_max 14
#define Int_max 14
#define n_bigtens 5

extern _Bigint *func_00121AF8(struct _reent *, int);
extern void func_00121BA0(struct _reent *, _Bigint *);
extern _Bigint *func_00121BD0(struct _reent *, _Bigint *, int, int);
extern int func_00121E30(unsigned int);
extern _Bigint *func_00121F78(struct _reent *, int);
extern _Bigint *func_00121FB0(struct _reent *, _Bigint *, _Bigint *);
extern _Bigint *func_001221E0(struct _reent *, _Bigint *, int);
extern _Bigint *func_001222E0(struct _reent *, _Bigint *, int);
extern int func_00122458(_Bigint *, _Bigint *);
extern _Bigint *func_001224C0(struct _reent *, _Bigint *, _Bigint *);
extern _Bigint *func_00122868(struct _reent *, double, int *, int *);
extern int func_0011E990(_Bigint *, _Bigint *);
extern void block_copy(void *, void *, int);

#define Balloc func_00121AF8
#define Bfree func_00121BA0
#define multadd func_00121BD0
#define hi0bits func_00121E30
#define i2b func_00121F78
#define mult func_00121FB0
#define pow5mult func_001221E0
#define lshift func_001222E0
#define cmp func_00122458
#define diff func_001224C0
#define d2b func_00122868
#define quorem func_0011E990
#define Bcopy(x, y) block_copy((char *)&x->_sign, (char *)&y->_sign, y->_wds * sizeof(int) + 2 * sizeof(int))

extern const double D_0026C818[];
extern const double D_0026C8E0[];
#define tens D_0026C818
#define bigtens D_0026C8E0

char *func_0011EBA8(struct _reent *ptr, double _d, int mode, int ndigits, int *decpt,
                    int *sign, char **rve)
{
    int bbits, b2, b5, be, dig, i, ieps, ilim, ilim0, ilim1, j, j1, k, k0,
        k_check, leftright, m2, m5, s2, s5, spec_case, try_quick;
    union double_union d, d2, eps;
    int L;
    int denorm;
    unsigned int x;
    _Bigint *b, *b1, *delta, *mlo, *mhi, *S;
    double ds;
    char *s, *s0;

    d.d = _d;

    if (ptr->_result) {
        ptr->_result->_k = ptr->_result_k;
        ptr->_result->_maxwds = 1 << ptr->_result_k;
        Bfree(ptr, ptr->_result);
        ptr->_result = 0;
    }

    if (word0(d) & Sign_bit) {
        /* set sign for everything, including 0's */
        *sign = 1;
        word0(d) &= ~Sign_bit; /* clear sign bit */
    } else
        *sign = 0;

    if ((word0(d) & Exp_mask) == Exp_mask) {
        /* Infinity or NaN */
        *decpt = 9999;
        s = !word1(d) && !(word0(d) & 0xfffff) ? "Infinity" : "NaN";
        if (rve)
            *rve = s[3] ? s + 8 : s + 3;
        return s;
    }
    if (!d.d) {
        *decpt = 1;
        s = "0";
        if (rve)
            *rve = s + 1;
        return s;
    }

    b = d2b(ptr, d.d, &be, &bbits);
    if ((i = (int)(word0(d) >> Exp_shift1 & (Exp_mask >> Exp_shift1))) != 0) {
        d2.d = d.d;
        word0(d2) &= Frac_mask1;
        word0(d2) |= Exp_11;
        i -= Bias;
        denorm = 0;
    } else {
        /* d is denormalized */
        i = bbits + be + (Bias + (P - 1) - 1);
        x = i > 32 ? word0(d) << (64 - i) | word1(d) >> (i - 32)
                   : word1(d) << (32 - i);
        d2.d = x;
        word0(d2) -= 31 * Exp_msk1; /* adjust exponent */
        i -= (Bias + (P - 1) - 1) + 1;
        denorm = 1;
    }
    ds = (d2.d - 1.5) * 0.289529654602168 + 0.1760912590558 + i * 0.301029995663981;
    k = (int)ds;
    if (ds < 0. && ds != k)
        k--; /* want k = floor(ds) */
    k_check = 1;
    if (k >= 0 && k <= Ten_pmax) {
        if (d.d < tens[k])
            k--;
        k_check = 0;
    }
    j = bbits - i - 1;
    if (j >= 0) {
        b2 = 0;
        s2 = j;
    } else {
        b2 = -j;
        s2 = 0;
    }
    if (k >= 0) {
        b5 = 0;
        s5 = k;
        s2 += k;
    } else {
        b2 -= k;
        b5 = -k;
        s5 = 0;
    }
    if (mode < 0 || mode > 9)
        mode = 0;
    try_quick = 1;
    if (mode > 5) {
        mode -= 4;
        try_quick = 0;
    }
    leftright = 1;
    switch (mode) {
    case 0:
    case 1:
        ilim = ilim1 = -1;
        i = 18;
        ndigits = 0;
        break;
    case 2:
        leftright = 0;
        /* no break */
    case 4:
        if (ndigits <= 0)
            ndigits = 1;
        ilim = ilim1 = i = ndigits;
        break;
    case 3:
        leftright = 0;
        /* no break */
    case 5:
        i = ndigits + k + 1;
        ilim = i;
        ilim1 = i - 1;
        if (i <= 0)
            i = 1;
    }
    j = sizeof(unsigned int);
    for (ptr->_result_k = 0; sizeof(_Bigint) - sizeof(unsigned int) + j <= i; j <<= 1)
        ptr->_result_k++;
    ptr->_result = Balloc(ptr, ptr->_result_k);
    s = s0 = (char *)ptr->_result;

    if (ilim >= 0 && ilim <= Quick_max && try_quick) {
        /* Try to get by with floating-point arithmetic. */
        i = 0;
        d2.d = d.d;
        k0 = k;
        ilim0 = ilim;
        ieps = 2; /* conservative */
        if (k > 0) {
            ds = tens[k & 0xf];
            j = k >> 4;
            if (j & Bletch) {
                /* prevent overflows */
                j &= Bletch - 1;
                d.d /= bigtens[n_bigtens - 1];
                ieps++;
            }
            for (; j; j >>= 1, i++)
                if (j & 1) {
                    ieps++;
                    ds *= bigtens[i];
                }
            d.d /= ds;
        } else if ((j1 = -k) != 0) {
            d.d *= tens[j1 & 0xf];
            for (j = j1 >> 4; j; j >>= 1, i++)
                if (j & 1) {
                    ieps++;
                    d.d *= bigtens[i];
                }
        }
        if (k_check && d.d < 1. && ilim > 0) {
            if (ilim1 <= 0)
                goto fast_failed;
            ilim = ilim1;
            k--;
            d.d *= 10.;
            ieps++;
        }
        eps.d = ieps * d.d + 7.;
        word0(eps) -= (P - 1) * Exp_msk1;
        if (ilim == 0) {
            S = mhi = 0;
            d.d -= 5.;
            if (d.d > eps.d)
                goto one_digit;
            if (d.d < -eps.d)
                goto no_digits;
            goto fast_failed;
        }
        if (leftright) {
            /* Use Steele & White method of only
             * generating digits needed.
             */
            eps.d = 0.5 / tens[ilim - 1] - eps.d;
            for (i = 0;;) {
                L = d.d;
                d.d -= L;
                *s++ = '0' + (int)L;
                if (d.d < eps.d)
                    goto ret1;
                if (1. - d.d < eps.d)
                    goto bump_up;
                if (++i >= ilim)
                    break;
                eps.d *= 10.;
                d.d *= 10.;
            }
        } else {
            /* Generate ilim digits, then fix them up. */
            eps.d *= tens[ilim - 1];
            for (i = 1;; i++, d.d *= 10.) {
                L = d.d;
                d.d -= L;
                *s++ = '0' + (int)L;
                if (i == ilim) {
                    if (d.d > 0.5 + eps.d)
                        goto bump_up;
                    else if (d.d < 0.5 - eps.d) {
                        while (*--s == '0')
                            ;
                        s++;
                        goto ret1;
                    }
                    break;
                }
            }
        }
    fast_failed:
        s = s0;
        d.d = d2.d;
        k = k0;
        ilim = ilim0;
    }

    /* Do we have a "small" integer? */

    if (be >= 0 && k <= Int_max) {
        /* Yes. */
        ds = tens[k];
        if (ndigits < 0 && ilim <= 0) {
            S = mhi = 0;
            if (ilim < 0 || d.d <= 5 * ds)
                goto no_digits;
            goto one_digit;
        }
        for (i = 1;; i++) {
            L = d.d / ds;
            d.d -= L * ds;
            *s++ = '0' + (int)L;
            if (i == ilim) {
                d.d += d.d;
                if (d.d > ds || (d.d == ds && L & 1)) {
                bump_up:
                    while (*--s == '9')
                        if (s == s0) {
                            k++;
                            *s = '0';
                            break;
                        }
                    ++*s++;
                }
                break;
            }
            if (!(d.d *= 10.))
                break;
        }
        goto ret1;
    }

    m2 = b2;
    m5 = b5;
    mhi = mlo = 0;
    if (leftright) {
        if (mode < 2) {
            i = denorm ? be + (Bias + (P - 1) - 1 + 1) : 1 + P - bbits;
        } else {
            j = ilim - 1;
            if (m5 >= j)
                m5 -= j;
            else {
                s5 += j -= m5;
                b5 += j;
                m5 = 0;
            }
            if ((i = ilim) < 0) {
                m2 -= i;
                i = 0;
            }
        }
        b2 += i;
        s2 += i;
        mhi = i2b(ptr, 1);
    }
    if (m2 > 0 && s2 > 0) {
        i = m2 < s2 ? m2 : s2;
        b2 -= i;
        m2 -= i;
        s2 -= i;
    }
    if (b5 > 0) {
        if (leftright) {
            if (m5 > 0) {
                mhi = pow5mult(ptr, mhi, m5);
                b1 = mult(ptr, mhi, b);
                Bfree(ptr, b);
                b = b1;
            }
            if ((j = b5 - m5) != 0)
                b = pow5mult(ptr, b, j);
        } else
            b = pow5mult(ptr, b, b5);
    }
    S = i2b(ptr, 1);
    if (s5 > 0)
        S = pow5mult(ptr, S, s5);

    /* Check for special case that d is a normalized power of 2. */

    if (mode < 2) {
        if (!word1(d) && !(word0(d) & Bndry_mask) && word0(d) & Exp_mask) {
            /* The special case */
            b2 += Log2P;
            s2 += Log2P;
            spec_case = 1;
        } else
            spec_case = 0;
    }

    if ((i = ((s5 ? 32 - hi0bits(S->_x[S->_wds - 1]) : 1) + s2) & 0x1f) != 0)
        i = 32 - i;
    if (i > 4) {
        i -= 4;
        b2 += i;
        m2 += i;
        s2 += i;
    } else if (i < 4) {
        i += 28;
        b2 += i;
        m2 += i;
        s2 += i;
    }
    if (b2 > 0)
        b = lshift(ptr, b, b2);
    if (s2 > 0)
        S = lshift(ptr, S, s2);
    if (k_check) {
        if (cmp(b, S) < 0) {
            k--;
            b = multadd(ptr, b, 10, 0); /* we botched the k estimate */
            if (leftright)
                mhi = multadd(ptr, mhi, 10, 0);
            ilim = ilim1;
        }
    }
    if (ilim <= 0 && mode > 2) {
        if (ilim < 0 || cmp(b, S = multadd(ptr, S, 5, 0)) <= 0) {
            /* no digits, fcvt style */
        no_digits:
            k = -1 - ndigits;
            goto ret;
        }
    one_digit:
        *s++ = '1';
        k++;
        goto ret;
    }
    if (leftright) {
        if (m2 > 0)
            mhi = lshift(ptr, mhi, m2);

        mlo = mhi;
        if (spec_case) {
            mhi = Balloc(ptr, mhi->_k);
            Bcopy(mhi, mlo);
            mhi = lshift(ptr, mhi, Log2P);
        }

        for (i = 1;; i++) {
            dig = quorem(b, S) + '0';
            /* Do we yet have the shortest decimal string
             * that will round to d?
             */
            j = cmp(b, mlo);
            delta = diff(ptr, S, mhi);
            j1 = delta->_sign ? 1 : cmp(b, delta);
            Bfree(ptr, delta);
            if (j1 == 0 && !mode && !(word1(d) & 1)) {
                if (dig == '9')
                    goto round_9_up;
                if (j > 0)
                    dig++;
                *s++ = dig;
                goto ret;
            }
            if (j < 0 || (j == 0 && !mode && !(word1(d) & 1))) {
                if (j1 > 0) {
                    b = lshift(ptr, b, 1);
                    j1 = cmp(b, S);
                    if ((j1 > 0 || (j1 == 0 && dig & 1)) && dig++ == '9')
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
            b = multadd(ptr, b, 10, 0);
            if (mlo == mhi)
                mlo = mhi = multadd(ptr, mhi, 10, 0);
            else {
                mlo = multadd(ptr, mlo, 10, 0);
                mhi = multadd(ptr, mhi, 10, 0);
            }
        }
    } else
        for (i = 1;; i++) {
            *s++ = dig = quorem(b, S) + '0';
            if (i >= ilim)
                break;
            b = multadd(ptr, b, 10, 0);
        }

    /* Round off last digit */

    b = lshift(ptr, b, 1);
    j = cmp(b, S);
    if (j > 0 || (j == 0 && dig & 1)) {
    roundoff:
        while (*--s == '9')
            if (s == s0) {
                k++;
                *s++ = '1';
                goto ret;
            }
        ++*s++;
    } else {
        while (*--s == '0')
            ;
        s++;
    }
ret:
    Bfree(ptr, S);
    if (mhi) {
        if (mlo && mlo != mhi)
            Bfree(ptr, mlo);
        Bfree(ptr, mhi);
    }
ret1:
    Bfree(ptr, b);
    *s = 0;
    *decpt = k + 1;
    if (rve)
        *rve = s;
    return s0;
}
