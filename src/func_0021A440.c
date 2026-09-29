// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Computes two 3-float vector lengths into an object's sub-struct. s0 = arg0+0x1F0.
// Takes two arguments: it first copies the quadword at arg1 to s0+0x20
// (func_00102948(dst, src); arg1 stays in its argument register and is passed
// straight through, corrected 2026-09-28 against the original instructions -- the
// old C declared one argument and dropped the source), then twice calls
// func_001028D0(&D_700038A0, s0+0x20, dst) (dst = s0, then s0+0x10) which fills the
// PS2 scratchpad at 0x700038A0; the three resulting floats are read back from the
// raw absolute address 0x700038A0 (not through the symbol) and their sum of
// squares passed to func_0011E748 (sqrt-like). Result -> s0+0x30 then s0+0x34;
// finally s0+0x3C = 1 (a 'computed/valid' flag).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: 991202
// lowers the f0*f0+f1*f1+f2*f2 chain entirely through the FPU accumulator
// (FPU-MAC wall, 53.7%), while 2.3.3 emits the target's shape: two plain
// multiplies, an accumulator add, then a multiply-add. The offset pointer s0
// must be char* (not int) so the argument-setup adds schedule in the target's
// order. Verified objdiff 100% vs build/expected/func_0021A440.o.
extern void func_00102948(void *a0, void *a1);
extern void func_001028D0(float *p, char *arg1, char *arg2);
extern float func_0011E748(float x);
extern float D_700038A0[3];

void func_0021A440(char *arg0, void *arg1) {
    char *s0 = arg0 + 0x1F0;
    float *p = (float *)0x700038A0;
    func_00102948(s0 + 0x20, arg1);
    func_001028D0(D_700038A0, s0 + 0x20, s0);
    *(float *)(s0 + 0x30) = func_0011E748((p[0] * p[0]) + (p[1] * p[1]) + (p[2] * p[2]));
    func_001028D0(D_700038A0, s0 + 0x20, s0 + 0x10);
    *(float *)(s0 + 0x34) = func_0011E748((p[0] * p[0]) + (p[1] * p[1]) + (p[2] * p[2]));
    *(int *)(s0 + 0x3C) = 1;
}
