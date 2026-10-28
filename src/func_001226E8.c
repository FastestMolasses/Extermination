// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib mprec.c): b2d(a, &e): the top 53 bits of _Bigint a as a double,
// with the binary exponent in e (func_00121E30 = hi0bits).

typedef struct _Bigint {
    struct _Bigint *_next;
    int _k, _maxwds, _sign, _wds;
    unsigned int _x[1];
} _Bigint;
struct _reent;
union double_union { double d; unsigned int i[2]; };
#define word0(x) ((x).i[1])
#define word1(x) ((x).i[0])
#define Exp_shift 20
#define Exp_shift1 20
#define Exp_msk1 0x100000
#define Exp_mask 0x7ff00000
#define P 53
#define Bias 1023
#define Exp_1 0x3ff00000
#define Ebits 11
#define Frac_mask 0xfffff
#define Frac_mask1 0xfffff
extern _Bigint *func_00121AF8(struct _reent *, int);
extern void func_00121BA0(struct _reent *, _Bigint *);
extern _Bigint *func_00121BD0(struct _reent *, _Bigint *, int, int);
extern int func_00121E30(unsigned int);
extern int func_00121EB8(unsigned int *);
extern void *block_copy(void *dst, const void *src, unsigned int len);
#define Balloc func_00121AF8
#define Bfree func_00121BA0
#define multadd func_00121BD0
#define hi0bits func_00121E30
#define lo0bits func_00121EB8
#define Bcopy(x,y) block_copy((char *)&x->_sign, (char *)&y->_sign, y->_wds*sizeof(int) + 2*sizeof(int))
extern int func_00122458(_Bigint *, _Bigint *);
extern double func_001226E8(_Bigint *, int *);
#define cmp func_00122458
#define b2d func_001226E8
#define Storeinc(a,b,c) (((unsigned short *)a)[1] = (unsigned short)b, \
((unsigned short *)a)[0] = (unsigned short)c, a++)
#define Sign_Extend(a,b) /* no-op: arithmetic right shifts */
double func_001226E8(_Bigint *a, int *e)
{
  unsigned int *xa, *xa0, w, y, z;
  int k;
  union double_union d;
#define d0 word0(d)
#define d1 word1(d)

  xa0 = a->_x;
  xa = xa0 + a->_wds;
  y = *--xa;
  k = hi0bits (y);
  *e = 32 - k;
  if (k < Ebits)
    {
      d0 = Exp_1 | y >> (Ebits - k);
      w = xa > xa0 ? *--xa : 0;
      d1 = y << ((32 - Ebits) + k) | w >> (Ebits - k);
      goto ret_d;
    }
  z = xa > xa0 ? *--xa : 0;
  if (k -= Ebits)
    {
      d0 = Exp_1 | y << k | z >> (32 - k);
      y = xa > xa0 ? *--xa : 0;
      d1 = z << k | y >> (32 - k);
    }
  else
    {
      d0 = Exp_1 | y;
      d1 = z;
    }
ret_d:
#undef d0
#undef d1
  return d.d;
}
