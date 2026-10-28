// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// libm (fdlibm/newlib sf_scalbn.c): scalbnf(x, n) = x * 2^n with subnormal,
// overflow (huge * copysignf(huge, x)) and underflow (tiny * copysignf(tiny, x))
// handling; func_0011DE60 = copysignf.

typedef union { float value; int word; } ieee_float_shape_type;
#define GET_FLOAT_WORD(i,d) do { ieee_float_shape_type gf_u; gf_u.value = (d); (i) = gf_u.word; } while (0)
#define SET_FLOAT_WORD(d,i) do { ieee_float_shape_type sf_u; sf_u.word = (i); (d) = sf_u.value; } while (0)

extern float func_0011DE60(float x, float y);   /* copysignf */
#define copysignf func_0011DE60

/* fdlibm's static const float constants, written as macros: the original
   folds every use into an immediate and has no data object for them. */
#define two25    ((float)3.355443200e+07) /* 0x4c000000 */
#define twom25   ((float)2.9802322388e-08) /* 0x33000000 */
#define huge     ((float)1.0e+30)
#define tiny     ((float)1.0e-30)

float func_0011E148(float x, int n)
{
	int  k,ix;
	GET_FLOAT_WORD(ix,x);
        k = (ix&0x7f800000)>>23;		/* extract exponent */
        if (k==0) {				/* 0 or subnormal x */
            if ((ix&0x7fffffff)==0) return x; /* +-0 */
	    x *= two25;
	    GET_FLOAT_WORD(ix,x);
	    k = ((ix&0x7f800000)>>23) - 25;
            if (n< -50000) return tiny*x; 	/*underflow*/
	    }
        if (k==0xff) return x+x;		/* NaN or Inf */
        k = k+n;
        if (k >  0xfe) return huge*copysignf(huge,x); /* overflow  */
        if (k > 0) 				/* normal result */
	    {SET_FLOAT_WORD(x,(ix&0x807fffff)|(k<<23)); return x;}
        if (k <= -25)
	    if (n > 50000) 	/* in case integer overflow in n+k */
		return huge*copysignf(huge,x);	/*overflow*/
	    else return tiny*copysignf(tiny,x);	/*underflow*/
        k += 25;				/* subnormal result */
	SET_FLOAT_WORD(x,(ix&0x807fffff)|(k<<23));
        return x*twom25;
}
