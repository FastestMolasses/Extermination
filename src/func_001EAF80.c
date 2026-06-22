// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Setup of an animation / interpolation object (D_0081F8F0). First calls
// func_001CFB50(&D_0081F8F0, 0, arg0, ...) forwarding two floats loaded from the
// gp-rel global object D_00275C34 (fields +0x54 and +0x5C) plus three FPU
// constants 1.0f, 1e-6f (0x358637BD), and 9.0f. Then twice calls
// func_001CFBE0(arg1, 1, table, &D_0081F8F0, 0) with two parameter tables
// (D_00255860, D_002558F0). sq/lq frame saving $ra and $s0 (s0 = arg1 held
// across the calls).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the pinned 991202 build schedules
// the two const call-args and the saved-reg param copy differently around the jal
// (89.7%). 2.3.3 reproduces the target's paddub-in-delay-slot timing
// (idiom-17: the last const arg t0=0 fills each jal delay slot while paddub a0,s0
// emits before). func_001CFBE0 genuinely takes 5 args (the trailing 0). The
// callee tables D_00255860/D_002558F0/D_0081F8F0 are forced absolute by [8]-array
// over-declaration while D_00275C34 stays gp-rel (-sdatathreshold 4). Verified
// objdiff 100.0%.
extern char *D_00275C34;
extern int D_00255860[8];
extern int D_002558F0[8];
extern int D_0081F8F0[8];
extern void func_001CFB50(int *, int, int, float, float, float, float, float);
extern void func_001CFBE0(int, int, int *, int *, int);

void func_001EAF80(int arg0, int arg1) {
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C),
                  1.0f, 1e-6f, 9.0f);
    func_001CFBE0(arg1, 1, D_00255860, D_0081F8F0, 0);
    func_001CFBE0(arg1, 1, D_002558F0, D_0081F8F0, 0);
}
