// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libc (newlib mprec.c): ratio(a, b): b2d(a) / b2d(b) with the exponent
// difference folded into the high words. The double divide is the soft-float
// dpdiv, which ee-gcc names by its runtime name; it is called here by the linked
// name func_00127230.

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
/* ee-gcc lowers a double divide to the soft-float runtime's dpdiv, which
   this binary links as func_00127230; calling it by that name keeps the
   relocation on the linked symbol. */
extern double func_00127230(double, double);

double func_001229E8(_Bigint *a, _Bigint *b)
{
  union double_union da, db;
  int k, ka, kb;

  da.d = b2d (a, &ka);
  db.d = b2d (b, &kb);
  k = ka - kb + 32 * (a->_wds - b->_wds);
  if (k > 0)
    word0 (da) += k * Exp_msk1;
  else
    {
      k = -k;
      word0 (db) += k * Exp_msk1;
    }
  return func_00127230 (da.d, db.d);	/* da.d / db.d */
}
