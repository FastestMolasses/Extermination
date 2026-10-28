// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libm (newlib s_isinf.c): isinf(double) on the soft-float double held in one
// 64-bit register: 1 for +-infinity, else 0 (branch-free bit test).

typedef union {
    double value;
    struct { unsigned int lsw; unsigned int msw; } parts;
} ieee_double_shape_type;

#define EXTRACT_WORDS(ix0,ix1,d)        \
do {                                    \
  ieee_double_shape_type ew_u;          \
  ew_u.value = (d);                     \
  (ix0) = ew_u.parts.msw;               \
  (ix1) = ew_u.parts.lsw;               \
} while (0)

int func_0011DB10(double x)
{
	int hx,lx;
	EXTRACT_WORDS(hx,lx,x);
	hx &= 0x7fffffff;
	hx |= (unsigned int)(lx|(-lx))>>31;
	hx = 0x7ff00000 - hx;
	return 1 - (int)((unsigned int)(hx|(-hx))>>31);
}
