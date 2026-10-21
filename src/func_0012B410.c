// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// TU_STATIC: func_00128390
//
// Byte-matched (objdiff 100%), decomp lane round 8 (was a 99.09% NEARMISS).
// Two source fixes, both read from the original instructions:
//  - State 3's gate is func_00128640(arg0): the target sets only $a0 before the
//    call, and func_00128640 takes one argument (the old C also passed 4).
//  - func_00128390 is compiled as a static function of this file
//    (// TU_STATIC:, MATCHING_GUIDE "Decomp lane round 7"). The original compiler
//    saw it earlier in the same translation unit and knew it leaves $a1 alone,
//    so state 4 sub 2 stores arg0+0 = 1 from $a1 (still the func_00128390
//    argument 1) instead of a fresh constant: the former residual.
//    tools/decomp/tu_static.py removes the static copy after checking it equals
//    the original func_00128390; the call links to that function's own unit.
//
// CORRECTED (lane NMFIX4, 2026-09-28): func_001C2770 takes (arg0, arg1, 0) -- the original calls it with
// its own a0/a1 unchanged and a2 = 0 (the old C passed one argument). Checked by an original-instruction
// harness over the captured AREA00 RAM; objdiff 98.33% -> 99.09%.
//
// Logic fully recovered: a 5-way per-frame state machine dispatched on the
// state byte at arg0+6. State 0: zero arg1+0xD8, then (only when func_001C2770
// reports not-busy) pick the 0xD0 timer (0x3C/0x78 by func_00122BB8&1), stage a
// vec4 at 0x700038A0, and depending on func_001C25E0 / func_00128600 advance the
// state, set arg1+0xD8=0x3E99999A, prime arg1+0xE8 from arg0+0xC4 via
// func_0012ADC0, and kick func_001287F0. State 1: ease arg0+0xC4 toward arg1+0xE8
// via func_001B12B0 (pi/45 step); if the level id at base+0x230==8 and the frame
// mask 0x70003B68&0x1F is clear, retarget arg1+0xE8 from a random-angle table,
// else periodically re-aim via func_0012ADC0; then decrement the 0xD0 timer and
// at zero advance to state 3. State 2: if the latch arg1+0xE1 is clear and the
// arg0+0x54 short is set, jump to state 4; otherwise run the same 0xD0-timer
// countdown to state 3. State 3: func_00128640 gate, on success reset to state 0
// and set arg0+5=1. State 4: an inner sub-state machine on arg0+7 (fire/wait/
// finish): sub0 sets arg0=2, fires func_001287F0(0x21) + func_001EFE00; sub1
// waits on arg1+0xF4&0x1000 then latches arg1+0xE1=1; sub2 snapshots
// func_00128390 into arg0+0x34, clears arg0+0x36, sets arg0=1 and returns to
// state 3. Finally, when not-busy, run func_001C3D60.
//
extern int func_001C2770(char *a, char *b, int flags);
extern int func_00122BB8(void);
extern int func_001C25E0(char *p, int *v);
extern int func_0012ADC0(char *a, char *b, char *c, float f);
extern void func_001287F0(char *a, char *b, int c, float f);
extern int func_00128600(int a);
extern int func_00128640(char *arg0);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float a);
static int func_00128390(int a0, int a1);  /* same-TU callee, see TU_STATIC */
#include "src/func_00128390.c"
extern void func_001EFE00(int a, char *p);
extern void func_001C3D60(char *a, char *b);
extern char D_008102B0;
extern int D_700038A0;
extern int D_70003B68;

void func_0012B410(char *arg0, char *arg1) {
    char *base;
    int busy;
    short cnt;

    base = &D_008102B0;
    busy = func_001C2770(arg0, arg1, 0);
    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        *(int *)(arg1 + 0xD8) = 0;
        if (busy == 0) {
            if (func_00122BB8() & 1) {
                *(short *)(arg1 + 0xD0) = 0x3C;
            } else {
                *(short *)(arg1 + 0xD0) = 0x78;
            }
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0xBFB33333;
            *(int *)0x700038A8 = 0xC0A00000;
            *(int *)0x700038AC = 0x3F800000;
            if (func_001C25E0(arg0, &D_700038A0) == 0) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(int *)(arg1 + 0xD8) = 0x3E99999A;
                if (func_0012ADC0(arg0, arg0 + 0xB0, base + 0xA0, 220.0f) == 0) {
                    *(float *)(arg1 + 0xE8) = *(float *)(arg0 + 0xC4);
                }
                func_001287F0(arg0, arg1, 6, 8.0f);
            } else if (func_00128600(3) != 0) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                *(int *)(arg1 + 0xD8) = 0x3E99999A;
                if (func_0012ADC0(arg0, arg0 + 0xB0, base + 0xA0, 220.0f) == 0) {
                    *(float *)(arg1 + 0xE8) = *(float *)(arg0 + 0xC4);
                }
                func_001287F0(arg0, arg1, 6, 8.0f);
            } else {
                *(unsigned char *)(arg0 + 6) = 2;
                *(int *)(arg1 + 0xD8) = 0;
                func_001287F0(arg0, arg1, 3, 8.0f);
            }
        }
        break;
    case 1:
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0xE8), *(float *)(arg0 + 0xC4), 0.0698131695f);
        if (*(int *)(base + 0x230) == 8) {
            if ((D_70003B68 & 0x1F) == 0) {
                *(float *)(arg1 + 0xE8) = func_001B1470((6.2831855f * (float)(func_00122BB8() & 0xF0)) / 256.0f);
            }
        } else if ((D_70003B68 & 0xF) == 0) {
            func_0012ADC0(arg0, arg0 + 0xB0, base + 0xA0, 220.0f);
        }
        cnt = *(short *)(arg1 + 0xD0) - 1;
        *(short *)(arg1 + 0xD0) = cnt;
        if (cnt == 0) {
            *(unsigned char *)(arg0 + 6) = 3;
            *(int *)(arg1 + 0xD8) = 0;
        }
        break;
    case 2:
        if (*(unsigned char *)(arg1 + 0xE1) == 0 && *(short *)(arg0 + 0x54) != 0) {
            *(unsigned char *)(arg0 + 6) = 4;
            *(unsigned char *)(arg0 + 7) = 0;
        } else {
            cnt = *(short *)(arg1 + 0xD0) - 1;
            *(short *)(arg1 + 0xD0) = cnt;
            if (cnt == 0) {
                *(unsigned char *)(arg0 + 6) = 3;
                *(int *)(arg1 + 0xD8) = 0;
            }
        }
        break;
    case 3:
        if (func_00128640(arg0) == 0) {
            *(char *)(arg0 + 5) = 1;
            *(unsigned char *)(arg0 + 6) = 0;
            *(unsigned char *)(arg0 + 7) = 0;
        }
        break;
    case 4:
        switch (*(unsigned char *)(arg0 + 7)) {
        case 0:
            *(int *)(arg1 + 0xD8) = 0;
            *(char *)(arg0 + 0) = 2;
            func_001287F0(arg0, arg1, 0x21, 8.0f);
            func_001EFE00(0x8000002B, arg0);
            *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) + 1;
            break;
        case 1:
            if (*(short *)(arg1 + 0xF4) & 0x1000) {
                *(unsigned char *)(arg1 + 0xE1) = 1;
                *(unsigned char *)(arg0 + 7) = *(unsigned char *)(arg0 + 7) + 1;
            }
            break;
        case 2:
            *(short *)(arg0 + 0x34) = func_00128390((int)arg0, 1);
            *(short *)(arg0 + 0x36) = 0;
            *(char *)(arg0 + 0) = 1;
            *(unsigned char *)(arg0 + 6) = 3;
            *(unsigned char *)(arg0 + 7) = 0;
            break;
        }
        break;
    }
    if (busy == 0) {
        func_001C3D60(arg0, arg1);
    }
}
