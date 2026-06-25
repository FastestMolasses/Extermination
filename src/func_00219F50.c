// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SEMANTICS: Re-inits an actor's path/anim sub-block at +0x1F0. Builds a
// transform from base vector D_700036A0 (copied from self+0xD0) into the
// scratch vector D_700038A0 = {0,0,100.0f,1.0f}, projects via
// func_001026A0/func_001028D0/func_00102760, stores 6.0f-scaled components
// at self+0x2D4 / +0x2D0, then picks one of two source poses
// (func_0019A570 hit-test) into sub+0x10, recomputes the transform, scales
// the global vec D_700036C0[4] by the timer length func_0011E748(...) and
// stores it at +0x2C8 / sub+0x30 (vector length sqrtf(x^2+y^2+z^2) via the
// adda.s/madd.s ACC fused MAC), finally seeding sub +0x38/+0x3C.
// mwcc 2.3.3 reproduces the adda.s/madd.s ACC-hazard codegen that the
// pinned 991202 build cannot match (991202 stalls at 86.74%).
extern void func_00102948(void *dst, void *src);
extern void copy_qw4(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern float func_00102738(void *a, void *b);
extern float func_0011E748(float x);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001A2370(void *a, void *b);
extern float D_700031B0[];
extern float D_700036A0[];
extern float D_700038A0[];
extern float D_700038B0[];

void func_00219F50(unsigned char *self) {
    unsigned char *sub = self + 0x1F0;
    func_00102948(sub, self + 0xB0);
    copy_qw4(D_700036A0, self + 0xD0);
    *(float *)0x700038A4 = 0.0f;
    *(float *)0x700038A0 = 0.0f;
    *(float *)0x700038A8 = 100.0f;
    *(float *)0x700038AC = 1.0f;
    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    func_001028D0(D_700038B0, D_700038A0, self + 0xB0);
    *(int *)0x700038BC = 0;
    func_00102760(D_700038B0, D_700038B0);
    *(float *)(self + 0x2D4) = 6.0f * *(float *)0x700038B0;
    *(float *)(self + 0x2D0) = 6.0f * *(float *)0x700038B8;
    if (func_0019A570(self + 0xB0, D_700038A0, 4, 0) != 0) {
        func_00102948(sub + 0x10, D_700031B0);
    } else {
        func_00102948(sub + 0x10, D_700038A0);
    }
    func_001028D0(D_700038A0, sub, sub + 0x10);
    *(int *)0x700038AC = 0;
    {
        float len = func_0011E748(func_00102738(D_700038A0, D_700038A0));
        *(float *)0x70003A20 = len;
        *(float *)0x700036C0 = *(float *)0x700036C0 * len;
        *(float *)0x700036C4 = *(float *)0x700036C4 * len;
        *(float *)0x700036C8 = *(float *)0x700036C8 * len;
        *(float *)0x700036CC = *(float *)0x700036CC * len;
    }
    func_001A2370(self, D_700036A0);
    func_001028D0(D_700038A0, sub + 0x10, sub);
    *(float *)(sub + 0x30) = func_0011E748(*(float *)0x700038A0 * *(float *)0x700038A0 + *(float *)0x700038A4 * *(float *)0x700038A4 + *(float *)0x700038A8 * *(float *)0x700038A8);
    *(float *)(self + 0x2C8) = *(float *)(sub + 0x30);
    *(int *)(sub + 0x3C) = 0;
    *(float *)(sub + 0x38) = 1.0f;
}
