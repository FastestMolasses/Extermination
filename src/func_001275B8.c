// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libgcc (fp-bit.c): float_to_si for double (dptoli): unpack_d (func_00126BE8),
// zero/NaN give 0, infinities and |x| >= 2^31 saturate by sign, small values
// give 0, else the fraction shifted down, negated for a negative sign.

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
int func_001275B8(double arg_a)
{
  fp_number_type a;
  int tmp;
  FLO_union_type au;

  au.value = arg_a;
  unpack_d (&au, &a);
  if (iszero (&a))
    return 0;
  if (isnan (&a))
    return 0;
  /* get reasonable MAX_SI_INT... */
  if (isinf (&a))
    return a.sign ? (-MAX_SI_INT)-1 : MAX_SI_INT;
  /* it is a number, but a small one */
  if (a.normal_exp < 0)
    return 0;
  if (a.normal_exp > BITS_PER_SI - 2)
    return a.sign ? (-MAX_SI_INT)-1 : MAX_SI_INT;
  tmp = a.fraction.ll >> ((FRACBITS + NGARDS) - a.normal_exp);
  return a.sign ? (-tmp) : (tmp);
}
