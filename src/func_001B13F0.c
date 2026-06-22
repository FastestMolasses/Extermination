// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Vector-magnitude threshold test on the EE scratchpad. Calls
// func_001028D0(&D_70003600, arg0, arg1) to populate the scratchpad workspace,
// then reads three floats back from the RAW absolute scratchpad address
// 0x70003600 (PS2 SPRAM) and passes their sum-of-squares to func_0011E748
// (sqrt-like). Returns 0 if the saved input fparg0 is strictly less than the
// resulting magnitude, else 1 (i.e. !(fparg0 < mag)). As in the sibling
// func_001B15D0, the CALL arg is the symbol D_70003600 but the float LOADS are
// emitted against the literal address 0x70003600 (lui 0x7000 / lwc1 0x3600) --
// hence the raw float* p.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202. Two walls
// keep 991202 at 43.5%: (a) it lowers f0*f0+f1*f1+f2*f2 with mula.s/madda.s
// (FPU-MAC wall) while CW/2.3.3 use mul.s/mul.s/adda.s/madd.s, and (b) the
// mwcc-vs-CW branch-lowering wall on the 0/1 return. The 2.3.3 build clears (a);
// the goto form below pins (b) to the target's bc1t->return-0 / fall-through
// return-1 / dead trailing v0=0 layout. Verified objdiff 100% vs
// build/expected/func_001B13F0.o.
extern void func_001028D0(float *, int, int);
extern float func_0011E748(float);
extern float D_70003600[3];

int func_001B13F0(int arg0, int arg1, float fparg0) {
    float *p = (float *)0x70003600;
    func_001028D0(D_70003600, arg0, arg1);
    if (fparg0 < func_0011E748((p[0] * p[0]) + (p[1] * p[1]) + (p[2] * p[2]))) goto zero;
    return 1;
zero:
    return 0;
}
