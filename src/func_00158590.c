// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038B0 0x700038B4 0x700038B8 0x700038BC
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 86.95% NEARMISS).
// Fixes read from the original instructions:
//  - the facing test is a materialised flag (ok = dot >= 0) tested afterwards,
//    not a direct branch on the compare;
//  - the 0x700038A0 output and the 0x700038B0..BC words are relocated externs
//    (idiom-32): A0 as a float array, so the B0/B4/B8 reads may move above
//    the A0 stores while the p+0xB4/B8 reads cannot; B0..BC as volatile
//    float/int union words, which keeps the colour stores in source order and
//    the arg1 branch's delay slot empty;
//  - the colour tail stores 0x700038BC inside each arm;
//  - argument-setup order: the first vector-subtract call carries casts on its
//    second and third arguments, and the final call passes &D_700038B0 with no
//    cast (a redundant cast reorders argument setup).
//
// Orientation-gated vector/colour setup for an actor p. Unless mode is -2, the
// vector from D_008105D0 to p+0xB0 is dotted with p+0xF0 and the function
// returns early when it is negative. Then, unless mode is -2, the
// vector 0x700038A0 is built from mode: -1 = p+0xB0 plus the offset already
// in 0x700038B0; 1 = copy of p+0xB0; 0 = the fixed direction
// (0.4, -0.1, 0.25) passed through func_001026A0 with p+0xD0; any other mode = p+0xB0 plus a
// 1.5-unit offset along the actor's yaw (p+0xC4). w is always 1.0. Finally the
// colour 0x700038B0 is (0x80,0,0,0x80) when arg1 is 0, else (0,0x80,0,0x80),
// and both vectors go to func_001F4CC0.

extern void func_001028D0(void *dst, void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern float func_0011E2A8(float x);
extern float func_0011DE90(float x);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001F4CC0(void *a, void *b);
extern char D_008105D0[];
extern float D_700038A0[];
typedef union { float f; int i; } SpadWord;
extern volatile SpadWord D_700038B0;
extern volatile SpadWord D_700038B4;
extern volatile SpadWord D_700038B8;
extern volatile SpadWord D_700038BC;


void func_00158590(char *p, int arg1, int mode) {
    int sp40[4];

    if (mode != -2) {
        int ok;
        func_001028D0(sp40, (void *)D_008105D0, (void *)(p + 0xB0));
        ok = func_00102738(sp40, p + 0xF0) >= 0.0f;
        if (!ok) {
            return;
        }
    }

    if (mode != -2) {
        if (mode == -1) {
            D_700038A0[0] = *(float *)(p + 0xB0) + D_700038B0.f;
            D_700038A0[1] = *(float *)(p + 0xB4) + D_700038B4.f;
            D_700038A0[2] = *(float *)(p + 0xB8) + D_700038B8.f;
            *(int *)0x700038AC = 0x3F800000;
        } else if (mode == 1) {
            func_00102948(D_700038A0, p + 0xB0);
            *(int *)0x700038AC = 0x3F800000;
        } else if (mode != 0) {
            *(float *)0x700038B0 = 1.5f * func_0011E2A8(*(float *)(p + 0xC4));
            *(float *)0x700038B8 = 1.5f * func_0011DE90(*(float *)(p + 0xC4));
            *(int *)0x700038B4 = 0;
            D_700038A0[0] = *(float *)(p + 0xB0) + D_700038B0.f;
            D_700038A0[1] = *(float *)(p + 0xB4) + D_700038B4.f;
            D_700038A0[2] = *(float *)(p + 0xB8) + D_700038B8.f;
            *(int *)0x700038AC = 0x3F800000;
        } else {
            *(float *)0x700038A0 = 0.4f;
            *(float *)0x700038A4 = -0.1f;
            *(float *)0x700038A8 = 0.25f;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(D_700038A0, p + 0xD0, D_700038A0);
        }
    }

    if (arg1 == 0) {
        D_700038B0.i = 0x80;
        D_700038B4.i = 0;
        D_700038B8.i = 0;
        D_700038BC.i = 0x80;
    } else {
        D_700038B0.i = 0;
        D_700038B4.i = 0x80;
        D_700038B8.i = 0;
        D_700038BC.i = 0x80;
    }
    func_001F4CC0(D_700038A0, &D_700038B0);
}
