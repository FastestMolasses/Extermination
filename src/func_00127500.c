// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libgcc (fp-bit.c): si_to_float for double (the SCE runtime's litodp): int to
// double via fp_number_type and pack_d (func_00126AB8); INT_MIN is a constant.

typedef enum {
    CLASS_SNAN, CLASS_QNAN, CLASS_ZERO, CLASS_NUMBER, CLASS_INFINITY
} fp_class_type;

typedef struct {
    fp_class_type class;
    unsigned int sign;
    int normal_exp;
    union {
        unsigned long long ll;
        unsigned int l[2];
    } fraction;
} fp_number_type;

typedef union {
    double value;
    unsigned long long value_raw;
} FLO_union_type;

#define FRACBITS 52
#define NGARDS 8
#define BITS_PER_SI 32
#define MAX_SI_INT 0x7fffffff
#define MAX_USI_INT 0xffffffffU

extern double func_00126AB8(fp_number_type *src);              /* pack_d */
extern void func_00126BE8(FLO_union_type *src, fp_number_type *dst); /* unpack_d */
#define pack_d func_00126AB8
#define unpack_d func_00126BE8

static __inline__ int isnan(fp_number_type *x) { return x->class == CLASS_SNAN || x->class == CLASS_QNAN; }
static __inline__ int isinf(fp_number_type *x) { return x->class == CLASS_INFINITY; }
static __inline__ int iszero(fp_number_type *x) { return x->class == CLASS_ZERO; }
double func_00127500(int arg_a)
{
  fp_number_type in;

  in.class = CLASS_NUMBER;
  in.sign = arg_a < 0;
  if (!arg_a)
    {
      in.class = CLASS_ZERO;
    }
  else
    {
      in.normal_exp = FRACBITS + NGARDS;
      if (in.sign)
	{
	  /* Special case for minint, since there is no +ve integer
	     representation for it */
	  if (arg_a == (- MAX_SI_INT - 1))
	    {
	      return (double)(- MAX_SI_INT - 1);
	    }
	  in.fraction.ll = (-arg_a);
	}
      else
	in.fraction.ll = arg_a;

      while (in.fraction.ll < (1LL << (FRACBITS + NGARDS)))
	{
	  in.fraction.ll <<= 1;
	  in.normal_exp -= 1;
	}
    }
  return pack_d (&in);
}
