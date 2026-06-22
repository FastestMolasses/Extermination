// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Magnitude/length of a 3-float vector held in the EE scratchpad. Calls
// func_001028D0(&D_70003600, arg0, arg1) to populate the scratchpad workspace,
// then reads the three floats back from the RAW absolute scratchpad address
// 0x70003600 (PS2 SPRAM) and passes their sum-of-squares to func_0011E748
// (sqrt-like). Note the asymmetry the original codegen requires: the CALL
// argument is the symbol D_70003600, but the float LOADS are emitted against
// the literal address 0x70003600 (lui 0x7000 / lwc1 0x3600), not a %hi/%lo
// reloc against the symbol -- hence the raw float* p.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// lowers the f0*f0 + f1*f1 + f2*f2 chain with mula.s/madda.s (FPU-MAC wall,
// 35.7%), while 2.3.3 emits the target's mul.s/mul.s/adda.s/madd.s sequence.
// Verified objdiff 100% vs build/expected/func_001B15D0.o.
extern void func_001028D0(float *, int, int);
extern float func_0011E748(float);
extern float D_70003600[3];

float func_001B15D0(int arg0, int arg1) {
    float *p = (float *)0x70003600;
    func_001028D0(D_70003600, arg0, arg1);
    return func_0011E748((p[0] * p[0]) + (p[1] * p[1]) + (p[2] * p[2]));
}
