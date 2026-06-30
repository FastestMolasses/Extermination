// NEARMISS func_0012B410  (vram 0x0012B410, 0x434 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring near-miss (98.31% on mwcc 2.3.3; logic fully recovered). Body, the 5-way state switch (cases 0-4 with the case-1 beql branch-likely), the nested sub-state switch on arg0+7, and all field/global I/O match. Residuals are pure compiler artifacts: (1) the switch selector byte arg0+6...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (objdiff 98.31% vs expected on mwcc 2.3.3; 81.53% on pinned 991202).
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
// Residual is register-coloring only (switch selector colored a0 vs target a1,
// one paddub-vs-addiu copy of the constant 1, and the D_70003B68 absolute-vs-
// symbol reloc form). Permuter territory; not a clean-store delay-slot nop.
extern int func_001C2770(int);
extern int func_00122BB8(void);
extern int func_001C25E0(char *p, int *v);
extern int func_0012ADC0(char *a, char *b, char *c, float f);
extern void func_001287F0(char *a, char *b, int c, float f);
extern int func_00128600(int a);
extern int func_00128640(char *a, int b);
extern float func_001B12B0(float goal, float cur, float rate);
extern float func_001B1470(float a);
extern short func_00128390(char *a, int b);
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
    busy = func_001C2770(0);
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
        if (func_00128640(arg0, 4) == 0) {
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
            *(short *)(arg0 + 0x34) = func_00128390(arg0, 1);
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
