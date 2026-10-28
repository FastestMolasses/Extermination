// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib vfprintf.c, FLOATING_POINT, _NO_LONGLONG with a 64-bit long):
// _vfprintf_r(reent, fp, fmt, ap), the engine behind printf/fprintf/sprintf.
// Fails with EOF when the stream cannot be written (__swsetup); unbuffered
// write-only streams with a valid file go through __sbprintf. Otherwise it
// scans the format with _mbtowc_r (on _impure_ptr, MB_CUR_MAX), collects
// output fragments in an 8-entry iovec flushed through __sprint, parses
// flags (' ', '#', '*', '-', '+', '.', '0', width digits, L, h, l/ll, q) and
// converts c, d/i/D, e/E/f/g/G (cvt + exponent; "Inf"/"NaN" first), n, o/O,
// p, s, u/U, x/X; any other character prints itself, NUL ends. Integers are
// 64-bit (u_long): 'l' loads a 64-bit slot, decimal digits use the libgcc
// 64-bit divide/modulo. Pads with 16-byte blank/zero runs. Returns the count
// of characters written, or EOF if the stream's error flag is set.
// The switch over conversion characters is the jump table jtbl_0026C9D0
// (' ' .. 'x'); the PRINT/PAD/FLUSH macros are newlib's own.

typedef unsigned int size_t;
typedef int wchar_t;
typedef unsigned long u_long;
typedef unsigned int u_int;
typedef unsigned short u_short;
#define NULL ((void *)0)
#define EOF (-1)

struct _reent;
typedef struct __sFILE {
    unsigned char *_p;
    int _r;
    int _w;
    short _flags;
    short _file;
    struct { unsigned char *_base; int _size; } _bf;
    int _lbfsize;
    void *_cookie;
    int (*_read)(void *, char *, int);
    int (*_write)(void *, const char *, int);
    long (*_seek)(void *, long, int);
    int (*_close)(void *);
    struct { unsigned char *_base; int _size; } _ub;
    unsigned char *_up;
    int _ur;
    unsigned char _ubuf[3];
    unsigned char _nbuf[1];
    struct { unsigned char *_base; int _size; } _lb;
    int _blksize;
    int _offset;
    struct _reent *_data;
} FILE;

#define __SWR   0x0008
#define __SRW   0x0010
#define __SNBF  0x0002
#define __SERR  0x0040
#define __sferror(p) (((p)->_flags & __SERR) != 0)
#define cantwrite(fp) \
    ((((fp)->_flags & __SWR) == 0 || (fp)->_bf._base == NULL) && __swsetup(fp))

struct __siov { void *iov_base; size_t iov_len; };
struct __suio { struct __siov *uio_iov; int uio_iovcnt; int uio_resid; };

struct lconv { char *decimal_point; };

/* ee-gcc's EABI varargs (gcc 2.9 va-mips.h): va_list is a plain cursor over
   8-byte argument slots; each va_arg advances it by one slot. */
typedef char *va_list;
#define va_arg(ap, type) (*(type *)(((ap) += 8) - 8))

extern struct _reent *D_0024295C;           /* _impure_ptr */
extern int D_00242960;                      /* __mb_cur_max */
#define _REENT D_0024295C
#define MB_CUR_MAX D_00242960

extern struct lconv *func_00120B10(void);
extern int func_00124F58(FILE *fp);
extern int func_00123620(FILE *fp, const char *fmt, va_list ap);
extern int func_001216B8(struct _reent *r, wchar_t *pwc, const char *s, size_t n, int *state);
extern int func_001235D8(FILE *fp, struct __suio *uio);
extern void *func_001216F8(const void *s, int c, size_t n);
extern size_t func_001232E0(const char *s);
extern int func_0011DB10(double x);
extern int func_0011DB58(double x);
extern char *func_00124C68(struct _reent *data, double value, int ndigits, int flags,
                           char *sign, int *decpt, int ch, int *length);
extern int func_00124E18(char *p0, int exp, int fmtch);
extern int func_001274B0(double a, double b);
extern u_long func_00126578(u_long a, u_long b);
extern u_long func_00125FA8(u_long a, u_long b);
#define localeconv func_00120B10
#define __swsetup func_00124F58
#define __sbprintf func_00123620
#define _mbtowc_r func_001216B8
#define __sprint func_001235D8
#define memchr func_001216F8
#define strlen func_001232E0
#define isinf func_0011DB10
#define isnan func_0011DB58
#define cvt func_00124C68
#define exponent func_00124E18
#define dpcmp func_001274B0
#define __umoddi3 func_00126578
#define __udivdi3 func_00125FA8
/* ee-gcc emits the 64-bit divide/modulo (and the soft-float compare) under
   runtime names; this binary links them under their address names, so they
   are called by those names. */

extern char D_0026C930[];     /* blanks */
extern char D_0026C940[];     /* zeroes */
#define blanks D_0026C930
#define zeroes D_0026C940
extern char D_0026C950[], D_0026C958[], D_0026C960[], D_0026C978[];
extern char D_0026C980[], D_0026C998[], D_0026C9B8[], D_0026C9C0[];

#define BUF 348
#define DEFPREC 6
#define to_digit(c) ((c) - '0')
#define is_digit(c) ((unsigned)to_digit(c) <= 9)
#define to_char(n) ((n) + '0')

#define ALT       0x001
#define HEXPREFIX 0x002
#define LADJUST   0x004
#define LONGDBL   0x008
#define LONGINT   0x010
#define QUADINT   0x020
#define SHORTINT  0x040
#define ZEROPAD   0x080
#define FPT       0x100

int sub__0000000000000000Inf(struct _reent *data, FILE *fp, const char *fmt0, va_list ap)
{
	register char *fmt;
	register int ch;
	register int n, m;
	register char *cp;
	register struct __siov *iovp;
	register int flags;
	int ret;
	int width;
	int prec;
	char sign;
	wchar_t wc;
	char *decimal_point = localeconv()->decimal_point;
	char softsign;
	double _double;
	int expt;
	int expsize;
	int ndig;
	char expstr[7];
	u_long _uquad;
	enum { OCT, DEC, HEX } base;
	int dprec;
	int realsz;
	int size;
	char *xdigs;
#define NIOV 8
	struct __suio uio;
	struct __siov iov[NIOV];
	char buf[BUF];
	char ox[2];
	int state = 0;

#define PADSIZE 16

#define	PRINT(ptr, len) { \
	iovp->iov_base = (ptr); \
	iovp->iov_len = (len); \
	uio.uio_resid += (len); \
	iovp++; \
	if (++uio.uio_iovcnt >= NIOV) { \
		if (__sprint(fp, &uio)) \
			goto error; \
		iovp = iov; \
	} \
}
#define	PAD(howmany, with) { \
	if ((n = (howmany)) > 0) { \
		while (n > PADSIZE) { \
			PRINT(with, PADSIZE); \
			n -= PADSIZE; \
		} \
		PRINT(with, n); \
	} \
}
#define	FLUSH() { \
	if (uio.uio_resid && __sprint(fp, &uio)) \
		goto error; \
	uio.uio_iovcnt = 0; \
	iovp = iov; \
}

#define	SARG() \
	(flags&LONGINT ? va_arg(ap, long) : \
	    flags&SHORTINT ? (long)(short)va_arg(ap, int) : \
	    (long)va_arg(ap, int))
#define	UARG() \
	(flags&LONGINT ? va_arg(ap, u_long) : \
	    flags&SHORTINT ? (u_long)(u_short)va_arg(ap, int) : \
	    (u_long)va_arg(ap, u_int))

	/* sorry, fprintf(read_only_file, "") returns EOF, not 0 */
	if (cantwrite(fp))
		return (EOF);

	/* optimise fprintf(stderr) (and other unbuffered Unix files) */
	if ((fp->_flags & (__SNBF|__SWR|__SRW)) == (__SNBF|__SWR) &&
	    fp->_file >= 0)
		return (__sbprintf(fp, fmt0, ap));

	fmt = (char *)fmt0;
	uio.uio_iov = iovp = iov;
	uio.uio_resid = 0;
	uio.uio_iovcnt = 0;
	ret = 0;

	/*
	 * Scan the format for conversions (`%' character).
	 */
	for (;;) {
		cp = fmt;
		while ((n = _mbtowc_r(_REENT, &wc, fmt, MB_CUR_MAX, &state)) > 0) {
			fmt += n;
			if (wc == '%') {
				fmt--;
				break;
			}
		}
		if ((m = fmt - cp) != 0) {
			PRINT(cp, m);
			ret += m;
		}
		if (n <= 0)
			goto done;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
		width = 0;
		prec = -1;
		sign = '\0';

rflag:		ch = *fmt++;
reswitch:	switch (ch) {
		case ' ':
			if (!sign)
				sign = ' ';
			goto rflag;
		case '#':
			flags |= ALT;
			goto rflag;
		case '*':
			if ((width = va_arg(ap, int)) >= 0)
				goto rflag;
			width = -width;
			/* FALLTHROUGH */
		case '-':
			flags |= LADJUST;
			goto rflag;
		case '+':
			sign = '+';
			goto rflag;
		case '.':
			if ((ch = *fmt++) == '*') {
				n = va_arg(ap, int);
				prec = n < 0 ? -1 : n;
				goto rflag;
			}
			n = 0;
			while (is_digit(ch)) {
				n = 10 * n + to_digit(ch);
				ch = *fmt++;
			}
			prec = n < 0 ? -1 : n;
			goto reswitch;
		case '0':
			flags |= ZEROPAD;
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			do {
				n = 10 * n + to_digit(ch);
				ch = *fmt++;
			} while (is_digit(ch));
			width = n;
			goto reswitch;
		case 'L':
			flags |= LONGDBL;
			goto rflag;
		case 'h':
			flags |= SHORTINT;
			goto rflag;
		case 'l':
			if (*fmt == 'l') {
				fmt++;
				flags |= QUADINT;
			} else {
				flags |= LONGINT;
			}
			goto rflag;
		case 'q':
			flags |= QUADINT;
			goto rflag;
		case 'c':
			*(cp = buf) = va_arg(ap, int);
			size = 1;
			sign = '\0';
			break;
		case 'D':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'd':
		case 'i':
			_uquad = SARG();
			if ((long) _uquad < 0) {
				_uquad = -_uquad;
				sign = '-';
			}
			base = DEC;
			goto number;
		case 'e':
		case 'E':
		case 'f':
		case 'g':
		case 'G':
			if (prec == -1) {
				prec = DEFPREC;
			} else if ((ch == 'g' || ch == 'G') && prec == 0) {
				prec = 1;
			}

			if (flags & LONGDBL) {
				_double = (double) va_arg(ap, long double);
			} else {
				_double = va_arg(ap, double);
			}

			/* do this before tricky precision changes */
			if (isinf(_double)) {
				if (dpcmp(_double, 0) < 0)
					sign = '-';
				cp = D_0026C950;	/* "Inf" */
				size = 3;
				break;
			}
			if (isnan(_double)) {
				cp = D_0026C958;	/* "NaN" */
				size = 3;
				break;
			}

			flags |= FPT;
			cp = cvt(data, _double, prec, flags, &softsign,
				&expt, ch, &ndig);
			if (ch == 'g' || ch == 'G') {
				if (expt <= -4 || expt > prec)
					ch = (ch == 'g') ? 'e' : 'E';
				else
					ch = 'g';
			}
			if (ch <= 'e') {	/* 'e' or 'E' fmt */
				--expt;
				expsize = exponent(expstr, expt, ch);
				size = expsize + ndig;
				if (ndig > 1 || flags & ALT)
					++size;
			} else if (ch == 'f') {		/* f fmt */
				if (expt > 0) {
					size = expt;
					if (prec || flags & ALT)
						size += prec + 1;
				} else	/* "0.X" */
					size = prec + 2;
			} else if (expt >= ndig) {	/* fixed g fmt */
				size = expt;
				if (flags & ALT)
					++size;
			} else
				size = ndig + (expt > 0 ?
					1 : 2 - expt);

			if (softsign)
				sign = '-';
			break;
		case 'n':
			if (flags & LONGINT)
				*va_arg(ap, long *) = ret;
			else if (flags & SHORTINT)
				*va_arg(ap, short *) = ret;
			else
				*va_arg(ap, int *) = ret;
			continue;	/* no output */
		case 'O':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG();
			base = OCT;
			goto nosign;
		case 'p':
			/* NOSTRICT */
			_uquad = (u_long)va_arg(ap, void *);
			base = HEX;
			xdigs = D_0026C960;	/* "0123456789abcdef" */
			flags |= HEXPREFIX;
			ch = 'x';
			goto nosign;
		case 's':
			if ((cp = va_arg(ap, char *)) == NULL)
				cp = D_0026C978;	/* "(null)" */
			if (prec >= 0) {
				/*
				 * can't use strlen; can only look for the
				 * NUL in the first `prec' characters, and
				 * strlen() will go further.
				 */
				char *p = memchr(cp, 0, prec);

				if (p != NULL) {
					size = p - cp;
					if (size > prec)
						size = prec;
				} else
					size = prec;
			} else
				size = strlen(cp);
			sign = '\0';
			break;
		case 'U':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG();
			base = DEC;
			goto nosign;
		case 'X':
			xdigs = D_0026C980;	/* "0123456789ABCDEF" */
			goto hex;
		case 'x':
			xdigs = D_0026C960;	/* "0123456789abcdef" */
hex:			_uquad = UARG();
			base = HEX;
			/* leading 0x/X only if non-zero */
			if (flags & ALT && _uquad != 0)
				flags |= HEXPREFIX;

			/* unsigned conversions */
nosign:			sign = '\0';
number:			if ((dprec = prec) >= 0)
				flags &= ~ZEROPAD;

			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
			if (_uquad != 0 || prec != 0) {
				/*
				 * Unsigned mod is hard, and unsigned mod
				 * by a constant is easier than that by
				 * a variable; hence this switch.
				 */
				switch (base) {
				case OCT:
					do {
						*--cp = to_char(_uquad & 7);
						_uquad >>= 3;
					} while (_uquad);
					/* handle octal leading 0 */
					if (flags & ALT && *cp != '0')
						*--cp = '0';
					break;

				case DEC:
					/* many numbers are 1 digit */
					while (_uquad >= 10) {
						*--cp = to_char(__umoddi3(_uquad, 10));
						_uquad = __udivdi3(_uquad, 10);
					}
					*--cp = to_char(_uquad);
					break;

				case HEX:
					do {
						*--cp = xdigs[_uquad & 15];
						_uquad >>= 4;
					} while (_uquad);
					break;

				default:
					cp = D_0026C998;	/* "bug in vfprintf: bad base" */
					size = strlen(cp);
					goto skipsize;
				}
			}
			size = buf + BUF - cp;
		skipsize:
			break;
		default:	/* "%?" prints ?, unless ? is NUL */
			if (ch == '\0')
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
			*cp = ch;
			size = 1;
			sign = '\0';
			break;
		}

		/*
		 * All reasonable formats wind up here.  At this point, `cp'
		 * points to a string which (if not flags&LADJUST) should be
		 * padded out to `width' places.  If flags&ZEROPAD, it should
		 * first be prefixed by any sign or other prefix; otherwise,
		 * it should be blank padded before the prefix is emitted.
		 * After any left-hand padding and prefixing, emit zeroes
		 * required by a decimal [diouxX] precision, then print the
		 * string proper, then emit zeroes required by any leftover
		 * floating precision; finally, if LADJUST, pad with blanks.
		 *
		 * Compute actual size, so we know how much to pad.
		 * size excludes decimal prec; realsz includes it.
		 */
		realsz = dprec > size ? dprec : size;
		if (sign)
			realsz++;
		else if (flags & HEXPREFIX)
			realsz+= 2;

		/* right-adjusting blank padding */
		if ((flags & (LADJUST|ZEROPAD)) == 0)
			PAD(width - realsz, blanks);

		/* prefix */
		if (sign) {
			PRINT(&sign, 1);
		} else if (flags & HEXPREFIX) {
			ox[0] = '0';
			ox[1] = ch;
			PRINT(ox, 2);
		}

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
			PAD(width - realsz, zeroes);

		/* leading zeroes from decimal precision */
		PAD(dprec - size, zeroes);

		/* the string or number proper */
		if ((flags & FPT) == 0) {
			PRINT(cp, size);
		} else {	/* glue together f_p fragments */
			if (ch >= 'f') {	/* 'f' or 'g' */
				if (dpcmp(_double, 0) == 0) {
					/* kludge for __dtoa irregularity */
					PRINT(D_0026C9B8, 1);	/* "0" */
					if (expt < ndig || (flags & ALT) != 0) {
						PRINT(decimal_point, 1);
						PAD(ndig - 1, zeroes);
					}
				} else if (expt <= 0) {
					PRINT(D_0026C9B8, 1);	/* "0" */
					PRINT(decimal_point, 1);
					PAD(-expt, zeroes);
					PRINT(cp, ndig);
				} else if (expt >= ndig) {
					PRINT(cp, ndig);
					PAD(expt - ndig, zeroes);
					if (flags & ALT)
						PRINT(D_0026C9C0, 1);	/* "." */
				} else {
					PRINT(cp, expt);
					cp += expt;
					PRINT(D_0026C9C0, 1);	/* "." */
					PRINT(cp, ndig-expt);
				}
			} else {	/* 'e' or 'E' */
				if (ndig > 1 || flags & ALT) {
					ox[0] = *cp++;
					ox[1] = '.';
					PRINT(ox, 2);
					if (dpcmp(_double, 0) != 0) {
						PRINT(cp, ndig-1);
					} else	/* 0.[0..] */
						/* __dtoa irregularity */
						PAD(ndig - 1, zeroes);
				} else	/* XeYYY */
					PRINT(cp, 1);
				PRINT(expstr, expsize);
			}
		}
		/* left-adjusting padding (always blank) */
		if (flags & LADJUST)
			PAD(width - realsz, blanks);

		/* finally, adjust ret */
		ret += width > realsz ? width : realsz;

		FLUSH();	/* copy out the I/O vectors */
	}
done:
	FLUSH();
error:
	return (__sferror(fp) ? EOF : ret);
	/* NOTREACHED */
}
