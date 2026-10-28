// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib mprec.c): ulp(x): the value of one unit in the last place of x
// (soft-float double built from words).

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
extern int func_00121E98(unsigned int *);
extern void *block_copy(void *dst, const void *src, unsigned int len);
#define Balloc func_00121AF8
#define Bfree func_00121BA0
#define multadd func_00121BD0
#define hi0bits func_00121E30
#define lo0bits func_00121E98
#define Bcopy(x,y) block_copy((char *)&x->_sign, (char *)&y->_sign, y->_wds*sizeof(int) + 2*sizeof(int))
double func_00122650(double _x)
{
  union double_union x, a;
  register int L;

  x.d = _x;
  L = (word0 (x) & Exp_mask) - (P - 1) * Exp_msk1;
  if (L > 0)
    {
      word0 (a) = L;
      word1 (a) = 0;
    }
  else
    {
      L = -L >> Exp_shift;
      if (L < Exp_shift)
	{
	  word0 (a) = 0x80000 >> L;
	  word1 (a) = 0;
	}
      else
	{
	  word0 (a) = 0;
	  L -= Exp_shift;
	  word1 (a) = L >= 31 ? 1 : 1 << (31 - L);
	}
    }
  return a.d;
}
