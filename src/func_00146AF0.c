// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Camera/aim acceptance test. Bails (returns 0) unless the global mode byte
// D_008104A0 is '1' (0x31) or '2' (0x32). Stages a transform in the scratch
// block at 0x700038A0 two different ways depending on D_008104A1: when it is 1,
// it copies a block from (*D_008102D0)+0xC0 and runs func_00102760; otherwise it
// seeds a unit transform and runs func_001029C0/func_00102BB0/func_001026A0 over
// D_700036A0. It then computes an angle via func_001B1240(D_00810360, self->B0,
// self->B8), stages a second block at 0x700038B0, combines them with
// func_00102738, post-processes through func_0011E420 then func_0011E2A8, scales
// by arg1[0x5C], and accepts (returns 1) iff the float arg v is >= that result,
// rejecting (0) otherwise.
//
// Matched with mwcc 2.3.3; pinned 991202 only reaches 87.70% (the final fcmp
// branch / FP coloring diverges). objdiff 100.0 vs expected.
extern void func_001029C0(void *p);
extern void func_00102BB0(void *a, void *b, float s);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_00102948(void *dst, void *src);
extern void func_00102760(void *a, void *b);
extern float func_001B1240(void *p, float a, float b);
extern float func_0011E420(float x);
extern float func_0011E2A8(float x);
extern float func_00102738(void *a, void *b);
extern unsigned char D_008104A0;
extern unsigned char D_008104A1;
extern int D_008102D0;
extern float D_00810374;
extern char D_00810360[];
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

int func_00146AF0(char *self, char *arg1, float v) {
    float r;

    if (D_008104A0 != 0x31 && D_008104A0 != 0x32) {
        return 0;
    }
    if (D_008104A1 != 1) {
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x3F800000;
        *(int *)0x700038AC = 0x3F800000;
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, D_00810374);
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);
    } else {
        func_00102948(D_700038A0, (char *)D_008102D0 + 0xC0);
        *(int *)0x700038A4 = 0;
        func_00102760(D_700038A0, D_700038A0);
    }
    *(float *)0x70003A24 = func_001B1240(D_00810360, *(float *)(self + 0xB0), *(float *)(self + 0xB8));
    *(int *)0x700038B0 = 0;
    *(int *)0x700038B4 = 0;
    *(int *)0x700038B8 = 0x3F800000;
    *(int *)0x700038BC = 0x3F800000;
    func_001029C0(D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)0x70003A24);
    func_001026A0(D_700038B0, D_700036A0, D_700038B0);
    r = func_0011E420(func_00102738(D_700038A0, D_700038B0));
    *(float *)0x70003A24 = r;
    r = func_0011E2A8(r);
    r = *(float *)(arg1 + 0x5C) * r;
    if (v >= r) {
        return 1;
    }
    return 0;
}
