// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libgcc (libgcc2.c): __floatdidf: signed 64-bit to double = (double)(high word)
// * 65536 * 65536 + (double)(unsigned low word), with the soft-float litodp /
// dpmul / dpadd calls by their linked names (an unsigned word converts as signed
// plus 2^32 when its top bit is set).

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
#define WORD_SIZE 32
#define HIGH_HALFWORD_COEFF 65536.0
#define HIGH_WORD_COEFF 4294967296.0

double func_00125848(long u)
{
  double d;
  int low;
  double dl;

  d = litodp ((int) (u >> WORD_SIZE));
  d = dpmul (d, HIGH_HALFWORD_COEFF);
  d = dpmul (d, HIGH_HALFWORD_COEFF);
  /* d += (unsigned int) (u & 0xffffffff): the unsigned word converts as a
     signed one, plus 2^32 when its top bit is set. */
  low = (int) (u & 0xffffffffUL);
  dl = litodp (low);
  if (low < 0)
    dl = dpadd (dl, HIGH_WORD_COEFF);
  return dpadd (d, dl);
}
