// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.41% NEARMISS). Fixes read from the original instructions:
// - The state dispatch is a `switch` on the local state byte with case 0 falling into case 1 and the whole common
//   body inside case 1 (no default). That gives the original's likely-branch on state 0 with the `st + 1` advance in
//   its slot, the direct jump to the epilogue for other states, and the leftover advance copy after it; the old
//   if/goto spelling produced a plain branch plus nop instead.
// - The bone-table address is spelled `(char *)(sel * 4) + (int)g + 0x110` (as in the matched twin func_0012CAA0) so
//   the scaled index comes first in the add, matching the original operand order.
//
// Per-state setup driver for entity arg0 (state byte at arg0+6), targeting the global
// actor struct D_008102B0. If the global-pause flag D_008106BC or *0x70003B8D is set, it
// resets arg0 (+5=0xA, +6=0, +7=0) and bails. State 0: advance the state, fire
// func_001287F0(arg0, arg1, 0x19, 0.0f), set arg1+0xD0=0xF0 / arg1+0xD4=1.0f, then fall into the
// state-1 case (the whole common body lives in case 1; any other state returns). Common (states 0 and 1): build a transform in the scratch matrix
// D_70003000 from arg0's C0/C4/C8 euler angles, compose with arg1+0x30, look up a bone via
// the D_00242DD0[arg1.0xF6 & 7] table into D_008102B0+0x110, copy the result to D_70003400,
// then run func_001C69A0(arg0) / func_0012DE90(arg1). Once arg1+0xD4 has reached 2.0, latch
// the actor: OR 2 into D_008102B0+0, set +0xF=1, pick a +0x22C range constant from the
// arg1+0xE1 and D_0081070A flags (40/50/55), seed the identity matrix D_700038A0, and build
// the D_700038B0 orientation from D_008102B0+0xC4 (offset by pi when the bone index >= 4),
// store sin/cos of that angle into D_008102B0+0x70/0x78, submit via
// func_001EFD90(0x80000009, ...), and set arg0+4 = 3 (done).
extern void copy_qw4(void *d, void *s);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001026D0(void *a, int b, void *c);
extern void func_001029C0(void *m);
extern void func_00102A60(void *a, void *b, float f);
extern void func_00102B08(void *a, void *b, float f);
extern void func_00102BB0(void *a, void *b, float f);
extern void func_001031E0(void *a, void *b);
extern float func_0011DE90(float);
extern float func_0011E2A8(float);
extern void func_001287F0(char *a, char *b, int c, float f);
extern void func_0012DE90(char *p);
extern float func_001B1470(float);
extern void func_001C69A0(char *p);
extern void func_001EFD90(int a, void *b, void *c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int D_00242DD0[];
extern char D_008102B0[];
extern unsigned char D_008106BC;
extern unsigned char D_0081070A;
extern char D_70003000[];
extern char D_70003030[];
extern char D_70003400[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_0012D240(char *arg0, char *arg1) {
    char *g;
    unsigned char st;
    int sel;
    float ang;

    g = D_008102B0;
    if (D_008106BC != 0 || *(unsigned char *)0x70003B8D != 0) {
        *(char *)(arg0 + 5) = 0xA;
        *(unsigned char *)(arg0 + 6) = 0;
        *(char *)(arg0 + 7) = 0;
        return;
    }
    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001287F0(arg0, arg1, 0x19, 0.0f);
        *(short *)(arg1 + 0xD0) = 0xF0;
        *(float *)(arg1 + 0xD4) = 1.0f;
        /* fallthrough */
    case 1:
        func_001029C0(D_70003000);
        func_00102B08(D_70003000, D_70003000, *(float *)(arg0 + 0xC0));
        func_00102BB0(D_70003000, D_70003000, *(float *)(arg0 + 0xC4));
        func_00102A60(D_70003000, D_70003000, *(float *)(arg0 + 0xC8));
        func_001031E0(D_70003030, arg1 + 0x30);
        sel = D_00242DD0[*(short *)(arg1 + 0xF6) & 7];
        func_001026D0(D_70003000, *(int *)((char *)(sel * 4) + (int)g + 0x110) + 0x90, D_70003000);
        func_001031E0(arg0 + 0xB0, D_70003030);
        copy_qw4(D_70003400, D_70003000);
        func_001C69A0(arg0);
        func_0012DE90(arg1);
        if (*(float *)(arg1 + 0xD4) < 2.0f) {
            return;
        }
        func_001FBD50(arg0, 0x1B0, 0, 300.0f);
        *(unsigned char *)(g + 0) = *(unsigned char *)(g + 0) | 2;
        *(char *)(g + 0xF) = 1;
        if (*(unsigned char *)(arg1 + 0xE1) == 0) {
            if (D_0081070A == 0) {
                *(float *)(g + 0x22C) = 40.0f;
            } else {
                *(float *)(g + 0x22C) = 50.0f;
            }
        } else {
            if (D_0081070A == 0) {
                *(float *)(g + 0x22C) = 40.0f;
            } else {
                *(float *)(g + 0x22C) = 55.0f;
            }
        }
        *(int *)0x700038A0 = 0;
        *(float *)0x700038A4 = 1.0f;
        *(float *)0x700038A8 = 1.0f;
        *(float *)0x700038AC = 1.0f;
        func_001026A0(D_700038A0, D_70003000, D_700038A0);
        if ((*(short *)(arg1 + 0xF6) & 7) >= 4) {
            ang = func_001B1470(3.1415927f + *(float *)(g + 0xC4));
            *(int *)0x700038B0 = 0;
            *(float *)0x700038B4 = ang;
            *(int *)0x700038B8 = 0;
            *(float *)0x700038BC = 1.0f;
        } else {
            ang = *(float *)(g + 0xC4);
            *(int *)0x700038B0 = 0;
            *(float *)0x700038B4 = ang;
            *(int *)0x700038B8 = 0;
            *(float *)0x700038BC = 1.0f;
        }
        *(float *)(g + 0x70) = func_0011E2A8(ang);
        *(int *)(g + 0x74) = 0;
        *(float *)(g + 0x78) = func_0011DE90(ang);
        *(float *)(g + 0x7C) = 1.0f;
        func_001EFD90(0x80000009, D_700038A0, D_700038B0);
        *(char *)(arg0 + 4) = 3;
        break;
    }
}
