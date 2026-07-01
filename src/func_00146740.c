// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// AI transform planner. Builds a facing/position transform for arg0 in the
// scratch block at 0x700038A0/0x700038B0 and measures a distance/angle via
// func_001B15D0. If the result is <= 23.5, it stages toward the target read
// from *0x700031D0 (offsets 0x24/0x2C -- position fields) and rejects
// (returns 3) unless the resulting angle from func_00102738/func_0011E420 is
// <= ~135 degrees (2.3561945f rad); on acceptance it commits a facing value
// to arg1+0x79 via func_001B1380 and returns 1. Otherwise (> 23.5) it halves
// the distance, advances the scratch block by 3.0f, resolves a facing index
// via func_001B39F0 into arg1+0x79, and rejects (3) if the resulting
// transform component (indexed by 1-facing) is < 10.0f, else accepts (1).
extern void func_00102948(void *dst, void *src);
extern float func_001B15D0(void *a, void *b);
extern float func_0011E420(float x);
extern void func_001B2B10(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B1380(void *a, void *b, float c);
extern int func_001B39F0(void *a, void *b, void *c);

extern int D_700031B0;
extern int D_700038A0;
extern int D_700038B0;

int func_00146740(char *arg0, char *arg1) {
    float d;
    int r;

    func_00102948(&D_700038A0, arg0 + 0xB0);
    func_00102948(&D_700038B0, &D_700031B0);
    *(int *)0x700038B4 = 0;
    *(int *)0x700038A4 = 0;
    d = func_001B15D0(&D_700038A0, &D_700038B0);
    *(float *)0x70003A20 = d;
    if (d <= 23.5f) {
        void *p;

        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038A8 = 0x3F800000;
        *(int *)0x700038AC = 0x3F800000;
        func_001B2B10(arg0, &D_700038A0, &D_700038A0);
        p = *(void **)0x700031D0;
        *(float *)0x700038B0 = *(float *)((char *)p + 0x24);
        *(int *)0x700038B4 = 0;
        *(float *)0x700038B8 = *(float *)((char *)p + 0x2C);
        *(int *)0x700038BC = 0x3F800000;
        func_00102760(&D_700038B0, &D_700038B0);
        if (!(func_0011E420(func_00102738(&D_700038A0, &D_700038B0)) <= 2.3561945f)) {
            return 3;
        }
        func_001028B8(&D_700038B0, &D_700038B0, arg0 + 0xB0);
        *(char *)(arg1 + 0x79) = func_001B1380(&D_700038B0, arg0 + 0xB0, *(float *)(arg0 + 0xC4));
        return 1;
    }
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0;
    *(float *)0x700038A8 = d / 2.0f;
    *(int *)0x700038AC = 0x3F800000;
    func_001B2B10(arg0, &D_700038A0, &D_700038A0);
    func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
    *(float *)0x700038A4 += 3.0f;
    *(char *)(arg1 + 0x79) = func_001B39F0(arg0, &D_700038A0, &D_700038B0);
    r = *(char *)(arg1 + 0x79);
    if (*(float *)((char *)&D_700038B0 + ((1 - r) * 4)) < 10.0f) {
        return 3;
    }
    return 1;
}
