// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libgcc (libgcc2.c): __fixunsdfdi: double to unsigned 64-bit. Negative gives 0;
// the high word is (unsigned) (a * 2^-32), the remainder converted and added (or
// subtracted when it comes out negative). ee-gcc's own lowering of the
// unsigned 64-bit to double conversion (halve with the low bit kept, convert with
// __floatdidf = func_00125848, double) and the soft-float dpcmp / dpmul / dpsub /
// dpadd / dptoul calls are written out by their linked names.

/* ee-gcc lowers double arithmetic to the SCE soft-float runtime by name
   (dpadd, dpmul, ...); this binary links those routines under their address
   names, so the calls are written out explicitly. */
extern double func_00126EC8(double a, double b);   /* dpadd: a + b */
extern double func_00126F20(double a, double b);   /* dpsub: a - b */
extern double func_00126F88(double a, double b);   /* dpmul: a * b */
extern int func_001274B0(double a, double b);      /* dpcmp: <0, 0, >0 */
extern double func_00127500(int i);                /* litodp: (double)i */
extern unsigned int func_00127650(double a);       /* dptoul: (unsigned int)a */
#define dpadd func_00126EC8
#define dpsub func_00126F20
#define dpmul func_00126F88
#define dpcmp func_001274B0
#define litodp func_00127500
#define dptoul func_00127650
extern double func_00125848(long u);       /* __floatdidf */
#define floatdidf func_00125848
#define WORD_SIZE 32

unsigned long func_00125758(double a)
{
  double b;
  unsigned long v;
  double dv;

  if (dpcmp (a, 0.0) < 0)
    return 0;

  /* Compute high word of result, as a flonum.  */
  b = dpmul (a, 1.0 / 4294967296.0);	/* a / 2^32 */
  /* Convert that to fixed (but not to DItype!),
     and shift it into the high word.  */
  v = dptoul (b);
  v <<= WORD_SIZE;
  /* Remove high part from the DFtype, leaving the low part as flonum.
     (double)v of an unsigned 64-bit value: halve it, keeping the low bit,
     when its top bit is set, then double the result.  */
  if ((long) v >= 0)
    dv = floatdidf (v);
  else
    {
      dv = floatdidf ((v & 1) | (v >> 1));
      dv = dpadd (dv, dv);
    }
  a = dpsub (a, dv);
  /* Convert that to fixed (but not to DItype!) and add it in.
     Sometimes A comes out negative.  This is significant, since
     A has more bits than a long int does.  */
  if (dpcmp (a, 0.0) < 0)
    v -= dptoul (dpsub (0.0, a));
  else
    v += dptoul (a);
  return v;
}
