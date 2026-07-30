// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// SEMANTICS: per-frame scripted-move state machine for actor `arg0` acting on
// entity `arg1`, dispatched on the state byte at arg0+6 through jtbl_0026D060
// (6 states).  func_001C2770(arg0, arg1, 0) reports whether a move is still
// playing (`busy`); when it is not, func_001C3D60(arg0, arg1) runs at the end.
//   0: clear arg1+0xD8 and the sub-state byte arg0+7.  When not busy, stage the
//      vec4 (0, -1.4, -5.0, 1.0) in EE scratchpad 0x700038A0 and probe it with
//      func_001C25E0; on failure set arg0+5 = 2 and reset state/sub-state to 0.
//      On success pull arg0's position (0xB0/0xB4/0xB8) halfway toward arg1's
//      (0x80/0x84/0x88), stage (0, -4.0, 0, 1.0) at 0x700038B0 and aim with
//      func_001C2540(arg0, arg1+0x60, .., arg0+0xD0).  Then if arg1+0xE4 ==
//      0x100 advance to state 2 and re-aim with (0, 1.0, -5.0, 1.0) via
//      func_001C24D0; otherwise state = func_00128600(1) ? 1 : 2.
//   1: delegate to func_0012D580(arg0, arg1, busy).
//   2: func_00128830(arg0, 0, 0, -3.5) + func_001287F0(arg0, arg1, 0xA, 0),
//      then state++ and sub-state = 0.
//   3: if the entity flag arg1+0xF4 & 0x1000 is set, func_00128830(arg0, 0,
//      3.5, 0) + func_001287F0(arg0, arg1, 0xB, 0), state++, sub-state = 0;
//      otherwise func_0012E2C0(arg0, arg1).
//   4: if arg0+0x3C == 70.0 build a quaternion at 0x700038A0 from
//      (*D_00275B40)+0x14 +0x90, a second one at 0x700038B0 combined with
//      arg0+0xD0, and fire func_001EFFD0(0x80000008, .., 0xA or 0xB depending
//      on the latch arg1+0xE1, 0.6).  Then, when arg1+0xF4 & 0x1000, run
//      func_00128830(arg0, 0, -3.5, 3.5) + func_001287F0(arg0, arg1, 0xC, 0)
//      and state++.
//   5: when arg1+0xF4 & 0x1000 and func_00128640(arg0) == 0, stage
//      (0, 1.0, -5.0, 1.0), re-aim with func_001C24D0, set arg0+5 = 1 and
//      reset state/sub-state to 0.

extern int func_001C2770(char *a, char *b, int c);
extern int func_001C25E0(char *p, int *m);
extern void func_001C2540(char *a, char *b, int *m, char *o);
extern void func_001C24D0(char *a, int *m, char *o);
extern void func_001C3D60(char *a, char *b);
extern int func_00128600(int a);
extern int func_00128640(char *a);
extern void func_00128830(char *a, float x, float y, float z);
extern void func_001287F0(char *a, char *b, int c, float f);
extern void func_0012D580(char *a, char *b, int c);
extern void func_0012E2C0(char *a, char *b);
extern void func_001026A0(int *a, char *b, int *c);
extern void func_00102760(int *a, int *b);
extern void func_001EFFD0(int a, int *b, int *c, int d, float f);
extern char *D_00275B40;
extern int D_700038A0[8];
extern int D_700038B0[8];

void func_0012B970(char *arg0, char *arg1) {
    int busy;

    busy = func_001C2770(arg0, arg1, 0);
    switch (*(unsigned char *)(arg0 + 6)) {
    case 0:
        *(int *)(arg1 + 0xD8) = 0;
        arg0[7] = 0;
        if (busy == 0) {
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0xBFB33333;
            *(int *)0x700038A8 = 0xC0A00000;
            *(int *)0x700038AC = 0x3F800000;
            if (func_001C25E0(arg0, D_700038A0) == 0) {
                arg0[5] = 2;
                arg0[6] = 0;
                arg0[7] = 0;
            } else {
                float two = 2.0f;
                *(float *)(arg0 + 0xB0) -= *(float *)(arg1 + 0x80) / two;
                *(float *)(arg0 + 0xB4) -= *(float *)(arg1 + 0x84) / two;
                *(float *)(arg0 + 0xB8) -= *(float *)(arg1 + 0x88) / two;
                *(int *)0x700038B0 = 0;
                *(int *)0x700038B4 = 0xC0800000;
                *(int *)0x700038B8 = 0;
                *(int *)0x700038BC = 0x3F800000;
                func_001C2540(arg0, arg1 + 0x60, D_700038B0, arg0 + 0xD0);
                if (*(int *)(arg1 + 0xE4) == 0x100) {
                    arg0[6] = 2;
                    *(int *)0x700038B0 = 0;
                    *(int *)0x700038B4 = 0x3F800000;
                    *(int *)0x700038B8 = 0xC0A00000;
                    *(int *)0x700038BC = 0x3F800000;
                    func_001C24D0(arg0, D_700038B0, arg0 + 0xD0);
                } else if (func_00128600(1) != 0) {
                    arg0[6] = 1;
                } else {
                    arg0[6] = 2;
                }
            }
        }
        break;
    case 1:
        func_0012D580(arg0, arg1, busy);
        break;
    case 2:
        func_00128830(arg0, 0.0f, 0.0f, -3.5f);
        func_001287F0(arg0, arg1, 0xA, 0.0f);
        *(unsigned char *)(arg0 + 6) += 1;
        arg0[7] = 0;
        break;
    case 3:
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            func_00128830(arg0, 0.0f, 3.5f, 0.0f);
            func_001287F0(arg0, arg1, 0xB, 0.0f);
            *(unsigned char *)(arg0 + 6) += 1;
            arg0[7] = 0;
        } else {
            func_0012E2C0(arg0, arg1);
        }
        break;
    case 4:
        if (*(float *)(arg0 + 0x3C) == 70.0f) {
            *(int *)0x700038A0 = 0x3F56872B;
            *(int *)0x700038A4 = 0x3ED70A3D;
            *(int *)0x700038A8 = 0;
            *(int *)0x700038AC = 0x3F800000;
            func_001026A0(D_700038A0, *(char **)(D_00275B40 + 0x14) + 0x90, D_700038A0);
            *(int *)0x700038B0 = 0;
            *(int *)0x700038B4 = 0x3F34FDF4;
            *(int *)0x700038B8 = 0x3F34FDF4;
            *(int *)0x700038BC = 0x3F800000;
            func_00102760(D_700038B0, D_700038B0);
            func_001026A0(D_700038B0, arg0 + 0xD0, D_700038B0);
            if (*(unsigned char *)(arg1 + 0xE1) == 0) {
                func_001EFFD0(0x80000008, D_700038A0, D_700038B0, 0xA, 0.6f);
            } else {
                func_001EFFD0(0x80000008, D_700038A0, D_700038B0, 0xB, 0.6f);
            }
        }
        if (*(short *)(arg1 + 0xF4) & 0x1000) {
            func_00128830(arg0, 0.0f, -3.5f, 3.5f);
            func_001287F0(arg0, arg1, 0xC, 0.0f);
            *(unsigned char *)(arg0 + 6) += 1;
        }
        break;
    case 5:
        if ((*(short *)(arg1 + 0xF4) & 0x1000) && (func_00128640(arg0) == 0)) {
            *(int *)0x700038B0 = 0;
            *(int *)0x700038B4 = 0x3F800000;
            *(int *)0x700038B8 = 0xC0A00000;
            *(int *)0x700038BC = 0x3F800000;
            func_001C24D0(arg0, D_700038B0, arg0 + 0xD0);
            arg0[5] = 1;
            arg0[6] = 0;
            arg0[7] = 0;
        }
        break;
    }
    if (busy == 0) {
        func_001C3D60(arg0, arg1);
    }
}
