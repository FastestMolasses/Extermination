// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib vfprintf.c): cvt(reent, value, ndigits, flags, &sign, &decpt, ch,
// &length): the %e/%f/%g digit generator over _dtoa_r (func_0011EBA8): mode 3
// for 'f', 2 otherwise ('e'/'E' get one more digit); negative values (sign bit
// of the high word, so also -0.0) are negated and give '-'; pads trailing zeros
// unless %g without '#'. The double negate and compares are the soft-float
// dpsub / dpcmp, called by their linked names.

struct _reent;
union double_union { double d; unsigned int i[2]; };
#define word0(x) ((x).i[1])
#define Sign_bit 0x80000000
#define ALT 0x001
/* ee-gcc lowers double arithmetic to the SCE soft-float runtime by name;
   this binary links those routines under their address names. */
extern double func_00126F20(double a, double b);   /* dpsub: a - b */
extern int func_001274B0(double a, double b);      /* dpcmp: <0, 0, >0 */
extern char *func_0011EBA8(struct _reent *ptr, double d, int mode, int ndigits,
                           int *decpt, int *sign, char **rve);   /* _dtoa_r */
#define dpsub func_00126F20
#define dpcmp func_001274B0
#define _dtoa_r func_0011EBA8

char *func_00124C68(struct _reent *data, double value, int ndigits, int flags,
                    char *sign, int *decpt, int ch, int *length)
{
	int mode, dsgn;
	char *digits, *bp, *rve;
        union double_union tmp;

	if (ch == 'f') {
		mode = 3;		/* ndigits after the decimal point */
	} else {
		/* To obtain ndigits after the decimal point for the 'e'
		 * and 'E' formats, round to ndigits + 1 significant
		 * figures.
		 */
		if (ch == 'e' || ch == 'E') {
			ndigits++;
		}
		mode = 2;		/* ndigits significant digits */
	}

        tmp.d = value;
	if (word0(tmp) & Sign_bit) { /* this will check for < 0 and -0.0 */
		value = dpsub(0.0, value);	/* value = -value */
		*sign = '-';
        } else
		*sign = '\000';
	digits = _dtoa_r(data, value, mode, ndigits, decpt, &dsgn, &rve);
	if ((ch != 'g' && ch != 'G') || flags & ALT) {	/* Print trailing zeros */
		bp = digits + ndigits;
		if (ch == 'f') {
			if (*digits == '0' && dpcmp(value, 0.0) != 0)
				*decpt = -ndigits + 1;
			bp += *decpt;
		}
		if (dpcmp(value, 0.0) == 0)	/* kludge for __dtoa irregularity */
			rve = bp;
		while (rve < bp)
			*rve++ = '0';
	}
	*length = rve - digits;
	return (digits);
}
