// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Recoil/kick timer driver on state byte arg0+6 (only states 0 and 1 are real;
// state 0 falls through into state 1's body after one-time init). arg0 = entity,
// arg1 = linked shake/recoil block. State 0 init: advance state, zero arg1+0x4,
// set arg1+0x24=3.0f (countdown), roll two random small perturbations into
// arg1+0x28 (-0.01f*rand-0.015f) and arg1+0x2C (0.3f*rand-0.15f) via
// func_00122BB8(), and set arg1+0x18=0x258 (600, a tick budget). State 1 body:
// if global flag D_0081080F==1, clear arg1+0x18. While arg1+0x24 (the countdown)
// is still negative: tick down arg1+0x14 if nonzero, else reroll it to a random
// 0-7 value and refresh the shake pose via func_001B1240(self+0xB0,D_00810360,
// D_00810368) -> arg1+0x34 and func_001B12B0(arg1+0x34, self+0xC4, pi/... ) ->
// self+0xC4; if arg1+0x28 is nonzero and arg1+0x24 has decayed to/below
// -0.96, clamp arg1+0x24=-0.96 and zero arg1+0x28 (perturbation burns out).
// Then runs a VU0-scratchpad recoil-blend sequence at 0x70003800/0x70003A20
// (func_00102948/func_001B15D0/func_0011DF78/func_0011E620/func_001B1470/
// func_001029C0/func_00102B08/func_001026A0, the same scratch idiom as
// func_001509A0) and writes the blended result back to arg1+0x2C.
//
// Matched 100.0 with mwcc 2.3.3 (991202 reaches 95.4%). Keys: (1) the
// arg1+0x28-nonzero guard needs `float z = 0.0f; ... != z` (not `!= 0.0f`) to
// get mwcc's f0/f1 c.eq.s operand order; (2) the func_0011E620 call's second
// arg (the func_0011DF78 result) must be captured in a plain float temp used
// for BOTH the 0x70003A24 store and the call, not re-read from the scratch
// address, to get the target's f13-then-mov-f12 emit order; (3) the
// func_00102948(&D_700038B0, arg0+0xB0) call needs its FIRST arg written as an
// assignment-in-place (`p = &D_700038B0`) to flip mwcc's operand-compute order
// so the register-direct second arg (self+0xB0, no lui needed) is computed
// first, matching the target.
extern void func_001026A0(float *a, float *b, float *c);
extern void func_00102948(float *a, float *b);
extern void func_001029C0(float *a);
extern void func_00102B08(float *a, float *b, float v);
extern float func_0011DF78(float a);
extern float func_0011E620(float a, float b);
extern int func_00122BB8(void);
extern float func_001B1240(char *p, float a, float b);
extern float func_001B12B0(float a, float b, float c);
extern float func_001B1470(float a);
extern float func_001B15D0(float *a, float *b);

extern float D_00810360;
extern float D_00810364;
extern float D_00810368;
extern unsigned char D_0081080F;
extern float D_700036A0;
extern float D_700038A0;
extern float D_700038B0;

void func_0014C9D0(char *arg0, char *arg1) {
    unsigned char st;
    int cnt;
    float *p;
    float t;
    float z;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 0x4) = 0;
        *(float *)(arg1 + 0x24) = 3.0f;
        *(float *)(arg1 + 0x28) = -0.01f * ((float)((func_00122BB8() >> 19) & 0xFF) / 255.0f) - 0.015f;
        *(float *)(arg1 + 0x2C) = 0.3f * ((float)((func_00122BB8() >> 12) & 0xFF) / 255.0f) - 0.15f;
        *(int *)(arg1 + 0x18) = 0x258;
        /* fallthrough */
    case 1:
        if (D_0081080F == 1) {
            *(int *)(arg1 + 0x18) = 0;
        }
        if (*(float *)(arg1 + 0x24) < 0.0f) {
            cnt = *(int *)(arg1 + 0x14);
            if (cnt != 0) {
                *(int *)(arg1 + 0x14) = cnt - 1;
            } else {
                *(int *)(arg1 + 0x14) = (func_00122BB8() >> 15) & 7;
                *(float *)(arg1 + 0x34) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
                *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x34), *(float *)(arg0 + 0xC4), 0.06108652800321579f);
            }
            z = 0.0f;
            if (*(float *)(arg1 + 0x28) != z && *(float *)(arg1 + 0x24) <= -0.96000004f) {
                *(float *)(arg1 + 0x24) = -0.96000004f;
                *(float *)(arg1 + 0x28) = 0.0f;
            }
            func_00102948(&D_700038A0, &D_00810360);
            func_00102948(p = &D_700038B0, (float *)(arg0 + 0xB0));
            *(volatile int *)0x700038B4 = 0;
            *(volatile int *)0x700038A4 = 0;
            *(volatile float *)0x70003A20 = func_001B15D0(&D_700038A0, &D_700038B0);
            t = func_0011DF78(D_00810364 - *(float *)(arg0 + 0xB4));
            *(volatile float *)0x70003A24 = t;
            *(volatile float *)0x70003A20 = func_001B1470(func_0011E620(t, *(volatile float *)0x70003A20));
            func_001029C0(&D_700036A0);
            func_00102B08(&D_700036A0, &D_700036A0, *(volatile float *)0x70003A20);
            *(volatile float *)0x700038A0 = 0.0f;
            *(volatile int *)0x700038A4 = 0;
            *(volatile float *)0x700038A8 = 1.1f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
            *(float *)(arg1 + 0x2C) = *(volatile float *)0x700038A8;
        }
        break;
    }
}
