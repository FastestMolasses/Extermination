// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib dtoa.c): quorem(b, S), the dtoa digit step: q = top word of b /
// (top word of S + 1), b -= q * S in 16-bit halves, then one correction step when
// b >= S still (func_00122458 = cmp); trims b's word count; returns the digit q.

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
int func_0011E990(_Bigint * b, _Bigint * S)
{
  int n;
  int borrow, y;
  unsigned int carry, q, ys;
  unsigned int *bx, *bxe, *sx, *sxe;
  int z;
  unsigned int si, zs;

  n = S->_wds;
  if (b->_wds < n)
    return 0;
  sx = S->_x;
  sxe = sx + --n;
  bx = b->_x;
  bxe = bx + n;
  q = *bxe / (*sxe + 1);	/* ensure q <= true quotient */
  if (q)
    {
      borrow = 0;
      carry = 0;
      do
	{
	  si = *sx++;
	  ys = (si & 0xffff) * q + carry;
	  zs = (si >> 16) * q + (ys >> 16);
	  carry = zs >> 16;
	  y = (*bx & 0xffff) - (ys & 0xffff) + borrow;
	  borrow = y >> 16;
	  Sign_Extend (borrow, y);
	  z = (*bx >> 16) - (zs & 0xffff) + borrow;
	  borrow = z >> 16;
	  Sign_Extend (borrow, z);
	  Storeinc (bx, z, y);
	}
      while (sx <= sxe);
      if (!*bxe)
	{
	  bx = b->_x;
	  while (--bxe > bx && !*bxe)
	    --n;
	  b->_wds = n;
	}
    }
  if (cmp (b, S) >= 0)
    {
      q++;
      borrow = 0;
      carry = 0;
      bx = b->_x;
      sx = S->_x;
      do
	{
	  si = *sx++;
	  ys = (si & 0xffff) + carry;
	  zs = (si >> 16) + (ys >> 16);
	  carry = zs >> 16;
	  y = (*bx & 0xffff) - (ys & 0xffff) + borrow;
	  borrow = y >> 16;
	  Sign_Extend (borrow, y);
	  z = (*bx >> 16) - (zs & 0xffff) + borrow;
	  borrow = z >> 16;
	  Sign_Extend (borrow, z);
	  Storeinc (bx, z, y);
	}
      while (sx <= sxe);
      bx = b->_x;
      bxe = bx + n;
      if (!*bxe)
	{
	  while (--bxe > bx && !*bxe)
	    --n;
	  b->_wds = n;
	}
    }
  return q;
}
