// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib mprec.c): d2b(reent, d, &e, &bits): double to _Bigint mantissa plus
// exponent and significant bit count (func_00121EB8 = lo0bits, func_00121E30 =
// hi0bits).

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
_Bigint *func_00122868(struct _reent *ptr, double _d, int *e, int *bits)
{
  union double_union d;
  _Bigint *b;
  int de, i, k;
  unsigned int *x, y, z;
#define d0 word0(d)
#define d1 word1(d)

  d.d = _d;
  b = Balloc (ptr, 1);
  x = b->_x;

  z = d0 & Frac_mask;
  d0 &= 0x7fffffff;		/* clear sign bit, which we ignore */
  if ((de = (int) (d0 >> Exp_shift)) != 0)
    z |= Exp_msk1;
  if ((y = d1) != 0)
    {
      if ((k = lo0bits (&y)) != 0)
	{
	  x[0] = y | z << (32 - k);
	  z >>= k;
	}
      else
	x[0] = y;
      i = b->_wds = (x[1] = z) ? 2 : 1;
    }
  else
    {
      k = lo0bits (&z);
      x[0] = z;
      i = b->_wds = 1;
      k += 32;
    }
  if (de)
    {
      *e = de - Bias - (P - 1) + k;
      *bits = P - k;
    }
  else
    {
      *e = de - Bias - (P - 1) + 1 + k;
      *bits = 32 * i - hi0bits (x[i - 1]);
    }
#undef d0
#undef d1
  return b;
}
