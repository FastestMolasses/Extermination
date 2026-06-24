// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Camera/aim setup helper. Clears owner flag byte 0x5A, builds a scratch
// transform in the global block D_700038A0 from self+0xB0 via func_00102948,
// seeds constants (340.0, 270.0, 1.0) into the block, then func_001B15D0
// measures it. When the result exceeds 62.0, it interpolates (func_001B1240),
// runs func_001029C0 / func_00102BB0 on D_700036A0, rebuilds D_700038A0 with
// (62.0, 1.0), combines the two scratch blocks (func_001026A0 / func_001028B8)
// and copies the result floats back into self+0xB0 / self+0xB8, setting bit 1
// of the flag. When owner+0x28 (< 0) triggers, it stages D_700038A0 with
// (-15.0, 1.0) and on a nonzero func_001B3440 (angle 0.785398) clears
// owner+0x28 and sets bit 2 of the flag.
//
// Matched with mwcc 2.3.3; the pinned 991202 build only reaches 95.40%
// (the func_001B15D0 fcmp / leading-store scheduling diverges). objdiff 100.0
// vs expected (all 124 instructions DIFF_NONE).
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *p);
extern void func_00102BB0(void *a, void *b, float s);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028B8(void *a, void *b, void *c);
extern float func_001B1240(void *p, float a, float b);
extern float func_001B15D0(void *a, void *b);
extern int func_001B3440(void *self, void *p, void *m, float a);
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_001366E0(char *self, char *owner) {
    owner[0x5A] = 0;
    func_00102948(D_700038A0, self + 0xB0);
    *(int *)0x700038A4 = 0;
    *(int *)0x700038B0 = 0x43AA0000;
    *(int *)0x700038B4 = 0;
    *(int *)0x700038B8 = 0x43870000;
    *(int *)0x700038BC = 0x3F800000;
    if (!(func_001B15D0(D_700038A0, D_700038B0) <= 62.0f)) {
        owner[0x5A] = owner[0x5A] | 1;
        *(float *)0x70003A20 = func_001B1240(D_700038B0, *(float *)0x700038A0, *(float *)0x700038A8);
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)0x70003A20);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x42780000;
        *(int *)0x700038AC = 0x3F800000;
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);
        func_001028B8(D_700038A0, D_700038B0, D_700038A0);
        *(float *)(self + 0xB0) = *(float *)0x700038A0;
        *(float *)(self + 0xB8) = *(float *)0x700038A8;
    }
    if (*(float *)(owner + 0x28) < 0.0f) {
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0xC1700000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3F800000;
        if (func_001B3440(self, self + 0xB0, D_700038A0, 0.7853981852531433f) != 0) {
            *(float *)(owner + 0x28) = 0.0f;
            owner[0x5A] = owner[0x5A] | 2;
        }
    }
}
