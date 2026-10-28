// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.48% NEARMISS).
// Fixes read from the original instructions:
//  - The "FP even/odd coloring wall" was a wrong body: in each of the four
//    staged-candidate tails the original reads the scratch value
//    at 0x70003A20 ONCE into a temp, compares the field against it, and stores
//    that temp into the field. The old C read the volatile scratch twice
//    (compare, then store), which forced a second load.
//
// Camera/aim limit-clamp using the VU0 scratchpad block at 0x70003800/0x70003A20.
// arg0 is the camera object (pitch arg0+0xB0, distance arg0+0xB8), arg1 the input
// state (mode byte arg1+0x65, flag byte arg1+0x64).
// Part 1 (unless arg1+0x64 & 2): builds a scratch vector at D_700038A0 from
//   arg0+0xB0, picks a range limit (420 if mode==2 else 345), and if the current
//   reach func_001B15D0() exceeds it, rebuilds the clamped position through
//   func_001B1240/func_001029C0/func_00102BB0/func_001026A0/func_001028B8 and
//   writes back arg0+0xB0/0xB8.
// Part 2 (unless arg1+0x64 & 4): per-mode soft clamps of arg0+0xB0/0xB8 against
//   constant rails (350/250/-50/195/5/-70/10) with func_0011DF78-based easing,
//   staging each candidate through the scratch float at 0x70003A20.
//
// Scratch writes use the project's `*(volatile float/int *)0x7000XXXX` idiom;
// each easing tail reads the staged scratch candidate once into a temp, compares
// it with the field and stores that temp.
// vector bases passed as &D_700038A0 / &D_700038B0 / &D_700036A0.
//
extern void func_001026A0(float *a, float *b, float *c);
extern void func_001028B8(float *a, float *b, float *c);
extern void func_00102948(float *a, char *b);
extern void func_001029C0(float *a);
extern void func_00102BB0(float *a, float *b, float v);
extern float func_0011DF78(int a, float b);
extern float func_001B1240(float *a, float b, float c);
extern float func_001B15D0(float *a, float *b);

extern float D_700036A0;
extern float D_700038A0;
extern float D_700038B0;

void func_001509A0(char *arg0, char *arg1) {
    float lim;
    float t;
    int sel;

    if (!(*(char *)(arg1 + 0x64) & 2)) {
        if (*(char *)(arg1 + 0x65) == 2) {
            lim = 420.0f;
        } else {
            lim = 345.0f;
        }
        func_00102948(&D_700038A0, arg0 + 0xB0);
        *(volatile int *)0x700038A4 = 0;
        *(volatile float *)0x700038B0 = 100.0f;
        *(volatile int *)0x700038B4 = 0;
        *(volatile float *)0x700038B8 = 150.0f;
        *(volatile float *)0x700038BC = 1.0f;
        if (!(func_001B15D0(&D_700038A0, &D_700038B0) <= lim)) {
            *(volatile float *)0x70003A20 = func_001B1240(&D_700038B0, *(volatile float *)0x700038A0, *(volatile float *)0x700038A8);
            func_001029C0(&D_700036A0);
            func_00102BB0(&D_700036A0, &D_700036A0, *(volatile float *)0x70003A20);
            *(volatile float *)0x700038A0 = 0.0f;
            *(volatile int *)0x700038A4 = 0;
            *(volatile float *)0x700038A8 = lim;
            *(volatile float *)0x700038AC = 1.0f;
            func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
            func_001028B8(&D_700038A0, &D_700038B0, &D_700038A0);
            *(float *)(arg0 + 0xB0) = *(volatile float *)0x700038A0;
            *(float *)(arg0 + 0xB8) = *(volatile float *)0x700038A8;
        }
    }
    if (!(*(char *)(arg1 + 0x64) & 4)) {
        sel = *(char *)(arg1 + 0x65);
        if (sel != 2) {
            if (!(*(float *)(arg0 + 0xB8) <= 350.0f)) {
                *(float *)(arg0 + 0xB8) = 350.0f;
                return;
            }
            if (sel != 0) {
                if (*(float *)(arg0 + 0xB0) < 250.0f) {
                    *(float *)(arg0 + 0xB0) = 250.0f;
                }
                if (!(*(char *)(arg1 + 0x64) & 1) && *(float *)(arg0 + 0xB8) < 10.0f) {
                    *(volatile float *)0x70003A20 = 10.0f - (*(float *)(arg0 + 0xB0) - 195.0f);
                    if (*(float *)(arg0 + 0xB8) < (t = *(volatile float *)0x70003A20)) {
                        *(float *)(arg0 + 0xB8) = t;
                    }
                }
            } else {
                if (!(*(float *)(arg0 + 0xB0) <= -50.0f)) {
                    *(float *)(arg0 + 0xB0) = -50.0f;
                }
                if (!(*(char *)(arg1 + 0x64) & 1) && *(float *)(arg0 + 0xB8) < 10.0f) {
                    *(volatile float *)0x70003A20 = 10.0f - func_0011DF78(sel, *(float *)(arg0 + 0xB0) - 5.0f);
                    if (*(float *)(arg0 + 0xB8) < (t = *(volatile float *)0x70003A20)) {
                        *(float *)(arg0 + 0xB8) = t;
                    }
                }
            }
        } else {
            if (!(*(float *)(arg0 + 0xB8) <= -70.0f)) {
                *(float *)(arg0 + 0xB8) = -70.0f;
            }
            if (*(float *)(arg0 + 0xB0) < 5.0f) {
                *(volatile float *)0x70003A20 = 5.0f + (*(float *)(arg0 + 0xB8) - 10.0f);
                if (*(float *)(arg0 + 0xB0) < (t = *(volatile float *)0x70003A20)) {
                    *(float *)(arg0 + 0xB0) = t;
                }
            } else if (!(*(float *)(arg0 + 0xB0) <= 195.0f)) {
                *(volatile float *)0x70003A20 = 195.0f + func_0011DF78(sel, *(float *)(arg0 + 0xB8) - 10.0f);
                if (!(*(float *)(arg0 + 0xB0) <= (t = *(volatile float *)0x70003A20))) {
                    *(float *)(arg0 + 0xB0) = t;
                }
            }
        }
    }
}
