// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 75.73% NEARMISS).
// Fixes read from the original instructions:
//  - The routine is newlib/fdlibm __kernel_rem_pio2f (float Payne-Hanek
//    argument reduction); rewritten in that library's own source shape
//    (x, y, e0, nx, prec, ipio2), with its zero/one/two8/twon8 constants and
//    the prec switch for compressing fq[] into y[].
//  - Callee func_0011E148 is scalbnf(float x, int n) and func_0011DF98 is
//    floorf; the old C declared scalbnf with its arguments swapped.
//  - For q0 == 0 the high bit test shifts iq[jz-1] right by 8 (as in newlib's
//    float version), confirmed by the original.
//  - The init_jk (D_0026C538) and PIo2 (D_0026C548) tables are const; a
//    non-const extern changed the prologue's spill scheduling.
//
// func_0011CE20 = __kernel_rem_pio2f: reduces a large float argument modulo
// pi/2 using the 2/pi digit table ipio2 (8-bit limbs), returns the octant
// n & 7 and writes the reduced value as 1-3 floats in y[] by precision.
// Algorithm: FIR convolution of x[] with the table into q[], base-256 limb
// distillation into iq[], octant extraction with scalbnf/floorf, 1-q
// complement with carry when the fraction exceeds one half, recomputation
// with more table terms when the result cancels to zero, then conversion
// back to floats and multiplication by PIo2.

extern const int D_0026C538[];
extern const float D_0026C548[];

extern float func_0011E148(float x, int n);
extern float func_0011DF98(float x);

#define zero  0.0f
#define one   1.0f
#define two8  2.5600000000e+02f
#define twon8 3.9062500000e-03f

int func_0011CE20(float *x, float *y, int e0, int nx, int prec, const int *ipio2)
{
	int jz,jx,jv,jp,jk,carry,n,iq[20],i,j,k,m,q0,ih;
	float z,fw,f[20],fq[20],q[20];

	jk = D_0026C538[prec];
	jp = jk;

	jx =  nx-1;
	jv = (e0-3)/8; if(jv<0) jv=0;
	q0 =  e0-8*(jv+1);

	j = jv-jx; m = jx+jk;
	for(i=0;i<=m;i++,j++) f[i] = (j<0)? zero : (float) ipio2[j];

	for (i=0;i<=jk;i++) {
	    for(j=0,fw=0.0f;j<=jx;j++) fw += x[j]*f[jx+i-j]; q[i] = fw;
	}

	jz = jk;
recompute:
	for(i=0,j=jz,z=q[jz];j>0;i++,j--) {
	    fw    =  (float)((int)(twon8* z));
	    iq[i] =  (int)(z-two8*fw);
	    z     =  q[j-1]+fw;
	}

	z  = func_0011E148(z,(int)q0);
	z -= (float)8.0*func_0011DF98(z*(float)0.125);
	n  = (int) z;
	z -= (float)n;
	ih = 0;
	if(q0>0) {
	    i  = (iq[jz-1]>>(8-q0)); n += i;
	    iq[jz-1] -= i<<(8-q0);
	    ih = iq[jz-1]>>(7-q0);
	}
	else if(q0==0) ih = iq[jz-1]>>8;
	else if(z>=(float)0.5) ih=2;

	if(ih>0) {
	    n += 1; carry = 0;
	    for(i=0;i<jz ;i++) {
		j = iq[i];
		if(carry==0) {
		    if(j!=0) {
			carry = 1; iq[i] = 0x100- j;
		    }
		} else  iq[i] = 0xff - j;
	    }
	    if(q0>0) {
	        switch(q0) {
	        case 1:
	    	   iq[jz-1] &= 0x7f; break;
	    	case 2:
	    	   iq[jz-1] &= 0x3f; break;
	        }
	    }
	    if(ih==2) {
		z = one - z;
		if(carry!=0) z -= func_0011E148(one,(int)q0);
	    }
	}

	if(z==zero) {
	    j = 0;
	    for (i=jz-1;i>=jk;i--) j |= iq[i];
	    if(j==0) {
		for(k=1;iq[jk-k]==0;k++);

		for(i=jz+1;i<=jz+k;i++) {
		    f[jx+i] = (float) ipio2[jv+i];
		    for(j=0,fw=0.0f;j<=jx;j++) fw += x[j]*f[jx+i-j];
		    q[i] = fw;
		}
		jz += k;
		goto recompute;
	    }
	}

	if(z==(float)0.0) {
	    jz -= 1; q0 -= 8;
	    while(iq[jz]==0) { jz--; q0-=8;}
	} else {
	    z = func_0011E148(z,-(int)q0);
	    if(z>=two8) {
		fw = (float)((int)(twon8*z));
		iq[jz] = (int)(z-two8*fw);
		jz += 1; q0 += 8;
		iq[jz] = (int) fw;
	    } else iq[jz] = (int) z ;
	}

	fw = func_0011E148(one,(int)q0);
	for(i=jz;i>=0;i--) {
	    q[i] = fw*(float)iq[i]; fw*=twon8;
	}

	for(i=jz;i>=0;i--) {
	    for(fw=0.0f,k=0;k<=jp&&k<=jz-i;k++) fw += D_0026C548[k]*q[i+k];
	    fq[jz-i] = fw;
	}

	switch(prec) {
	    case 0:
		fw = 0.0f;
		for (i=jz;i>=0;i--) fw += fq[i];
		y[0] = (ih==0)? fw: -fw;
		break;
	    case 1:
	    case 2:
		fw = 0.0f;
		for (i=jz;i>=0;i--) fw += fq[i];
		y[0] = (ih==0)? fw: -fw;
		fw = fq[0]-fw;
		for (i=1;i<=jz;i++) fw += fq[i];
		y[1] = (ih==0)? fw: -fw;
		break;
	    case 3:
		for (i=jz;i>0;i--) {
		    fw      = fq[i-1]+fq[i];
		    fq[i]  += fq[i-1]-fw;
		    fq[i-1] = fw;
		}
		for (i=jz;i>1;i--) {
		    fw      = fq[i-1]+fq[i];
		    fq[i]  += fq[i-1]-fw;
		    fq[i-1] = fw;
		}
		for (fw=0.0f,i=jz;i>=2;i--) fw += fq[i];
		if(ih==0) {
		    y[0] =  fq[0]; y[1] =  fq[1]; y[2] =  fw;
		} else {
		    y[0] = -fq[0]; y[1] = -fq[1]; y[2] = -fw;
		}
	}
	return n&7;
}
