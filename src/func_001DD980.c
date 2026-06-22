// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Distance between two 3-float vectors arg0/arg1 (components at +0x0/+0x4/+0x8),
// then a dependent update. Computes d = sqrt(dx*dx + dy*dy + dz*dz) via
// func_0011E748 over the componentwise difference (arg1 - arg0), then calls
// func_001DD950(&D_008105E0, 2.0f + 1.02f*d, d) (1.02f = 0x3F828F5C). Returns void.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// lowers the sum-of-squares with mula.s/madda.s (FPU-MAC wall, 86.0%) while
// CW/2.3.3 emit the target's mul.s/adda.s/madd.s sequence. Verified objdiff 100%
// vs build/expected/func_001DD980.o.
extern float func_0011E748(float);
extern void func_001DD950(void *, float, float);
extern char D_008105E0;

void func_001DD980(float *arg0, float *arg1) {
    float dx = arg1[0] - arg0[0];
    float dy = arg1[1] - arg0[1];
    float dz = arg1[2] - arg0[2];
    float d = func_0011E748((dx * dx) + (dy * dy) + (dz * dz));
    func_001DD950(&D_008105E0, 2.0f + (1.02f * d), d);
}
