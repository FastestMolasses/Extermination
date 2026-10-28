// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 42.67% NEARMISS
// companion in src/readable/). Fix read from the original instructions:
//  - the routine is newlib's _mbtowc_r (mbtowc_r.c) built without MB_CAPABLE,
//    written in newlib's own shape: the n == 0 test is `s != NULL && n == 0`
//    and comes BEFORE the separate `s == NULL` test (that order is what puts
//    the -1 return ahead of the 0 return and loads the byte in a likely-branch
//    slot), and the terminator test is its own `if (*t == '\0') return 0;`.

// _mbtowc_r(reent, pwc, s, n, state) for the single-byte "C" locale: a null s
// returns 0 (no shift states); n == 0 returns -1; otherwise the byte *s is
// stored as the wide character (to pwc, or to a scratch word when pwc is null)
// and the result is 1 for a non-zero byte, 0 for the terminator. The reent and
// state arguments are unused.
typedef int wchar_t;
typedef unsigned int size_t;
struct _reent;

int func_001216B8(struct _reent *r, wchar_t *pwc, const char *s, size_t n, int *state)
{
  wchar_t dummy;
  unsigned char *t = (unsigned char *)s;

  if (pwc == 0)
    pwc = &dummy;

  if (s != 0 && n == 0)
    return -1;

  if (s == 0)
    return 0;  /* not state-dependent */

  *pwc = (wchar_t)*t;

  if (*t == '\0')
    return 0;

  return 1;
}
