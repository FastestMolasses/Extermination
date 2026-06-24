// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Tests 3 candidate facing-angle offsets (D_00248950[0..2] added to the actor's
// current heading at +0xC4) by building a TRS query matrix into the global GS
// work buffer D_700038A0..AC (z/scale row preset before the loop) and the per-
// iteration matrix scratch D_700036A0, then querying collision/visibility via
// func_0019AD00(arg0, D_700038B0, 0x80000007). Each successful probe sets bit
// `i` of a mask; returns mask & 1 (whether the first/base angle is clear).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the loop's saved-reg coloring/scheduling left it at 94.1%; 2.3.3 is a
// true objdiff 100% byte match vs build/expected/func_00181950.o. The loop
// counter must take the LOW saved reg (s0) and the float cursor the HIGH one
// (s2), achieved by declaring i before mask before p.
extern float func_001B1470(float);
extern void func_001029C0(char *);
extern void func_00102BB0(char *, char *, float);
extern void func_00102918(char *, char *, char *);
extern void func_001026A0(char *, char *, char *);
extern int func_0019AD00(char *, char *, int);
extern float D_00248950[];
extern int D_700036A0[];
extern int D_700038A0[];
extern int D_700038B0[];

int func_00181950(char *arg0) {
    int i;
    int mask;
    float *p;

    *(volatile int *)0x700038A0 = 0;
    *(volatile int *)0x700038A4 = 0;
    *(volatile int *)0x700038A8 = 0x40900000;
    *(volatile int *)0x700038AC = 0x3F800000;
    p = D_00248950;
    mask = 0;
    i = 0;
    do {
        func_001029C0((char *)D_700036A0);
        func_00102BB0((char *)D_700036A0, (char *)D_700036A0, func_001B1470(*(float *)(arg0 + 0xC4) + *p));
        func_00102918((char *)D_700036A0, (char *)D_700036A0, arg0 + 0xB0);
        func_001026A0((char *)D_700038B0, (char *)D_700036A0, (char *)D_700038A0);
        if (func_0019AD00(arg0, (char *)D_700038B0, 0x80000007) != 0) {
            mask |= 1 << i;
        }
        i += 1;
        p += 1;
    } while (i < 3);
    return mask & 1;
}
