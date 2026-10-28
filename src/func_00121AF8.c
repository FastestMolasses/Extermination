// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib mprec.c): Balloc(reent, k): takes a _Bigint of 2^k words from the
// reent's freelist (+0x4C, allocated on first use as 16 pointers with _calloc_r),
// or allocates a new one; clears its sign and word count. NULL on failure.

typedef struct _Bigint {
    struct _Bigint *_next;
    int _k, _maxwds, _sign, _wds;
    unsigned int _x[1];
} _Bigint;
struct _reent { char pad[0x4C]; _Bigint **_freelist; };
extern void *func_0011E878(struct _reent *ptr, unsigned int n, unsigned int size);

_Bigint *func_00121AF8(struct _reent *ptr, int k)
{
  int x;
  _Bigint *rv;

  if (ptr->_freelist == 0)
    {
      ptr->_freelist = (_Bigint **) func_0011E878 (ptr, sizeof (_Bigint *), 16);
      if (ptr->_freelist == 0)
        return 0;
    }

  if ((rv = ptr->_freelist[k]) != 0)
    {
      ptr->_freelist[k] = rv->_next;
    }
  else
    {
      x = 1 << k;
      rv = (_Bigint *) func_0011E878 (ptr, 1, sizeof (_Bigint) + (x - 1) * sizeof (int));
      if (rv == 0) return 0;
      rv->_k = k;
      rv->_maxwds = x;
    }
  rv->_sign = rv->_wds = 0;
  return rv;
}
