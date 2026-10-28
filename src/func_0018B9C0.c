// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031F0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.56% NEARMISS).
// Fixes read from the original instructions:
//  - func_0022EEF0 takes only the record pointer (its definition in src/); the
//    extra argument made mwcc keep the constant 1 in the wrong argument register.
//  - The state-1 sub-switch lists case 0 before case 3 (mwcc emits the case
//    compares in reverse source order).
//  - The field6 bail-out tests are three separate ifs, not one || chain.
//  - The camera/aim block D_008105F0..FC is one float[4] stored element by
//    element (0, -1, 0, 1), which keeps the original store order.
//  - The input-bits byte at 0x700031F0 is a volatile relocated scratchpad extern
//    (idiom-32), ORed in with |=.
//
// Per-frame state driver dispatched on the state byte at arg0+0. First decays the
// global cooldown D_008106EF and ORs the input-bits byte at 0x700031F0 into arg0+0x8B.
// State 0 (one-shot init): advances state to 1, sets the camera/aim block
// D_008105F0[0..3] to (0, -1.0f, 0, 1.0f), copies the float at arg0+0x64 into arg0+0xC,
// clears several flags/fields, seeds defaults (0x54=1000.0f, 0x50=-200.0f,
// 0x5C=2.0f, 0x8C=6.0f as raw bit patterns), recomputes the aim scalar via
// func_001B1240(&D_008105D0, ...) into arg0+0x44, then func_0018CE60(self, base+0xB0,
// 5). If field5==1 or field6 in {0xA,0xF,0xD} it bails; otherwise sets field6=8
// (or 0 when D_00810700==0x12 && D_00810702==0), runs func_0018C0C0/D7B0/C0D0 and
// recomputes the aim scalar again. State 1: sub-dispatch on field4 -- 3 ->
// func_0022EEF0+C0D0(0); 0 -> func_00191390+0018BC20(both with base=&D_008102B0)+C0D0(1);
// 1/2/default -> C0D0(1). base = &D_008102B0 is hoisted to a callee-saved reg.
extern void func_0018BC20(char *p, void *q);
extern void func_0018C0C0(char *p);
extern void func_0018C0D0(char *p, int a);
extern void func_0018CE60(char *p, void *q, int n);
extern void func_0018D7B0(char *p, int a);
extern void func_00191390(char *p, void *q);
extern float func_001B1240(void *p, float a, float b);
extern void func_0022EEF0(unsigned char *d);
extern volatile unsigned char D_700031F0;
extern char D_008102B0;
extern char D_008105D0;
extern float D_008105E0;
extern float D_008105E8;
extern float D_008105F0[4];
extern unsigned char D_008106EF;
extern unsigned char D_00810700;
extern unsigned char D_00810702;

void func_0018B9C0(char *arg0) {
    char *base = &D_008102B0;
    unsigned char st;
    unsigned char sub;

    if (D_008106EF != 0) {
        D_008106EF -= 1;
    }
    *(unsigned char *)(arg0 + 0x8B) |= D_700031F0;
    st = *(unsigned char *)(arg0 + 0);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 0) = st + 1;
        D_008105F0[0] = 0.0f;
        D_008105F0[1] = -1.0f;
        D_008105F0[2] = 0.0f;
        D_008105F0[3] = 1.0f;
        *(float *)(arg0 + 0xC) = *(float *)(arg0 + 0x64);
        *(unsigned char *)(arg0 + 4) = 0;
        *(char *)(arg0 + 7) = 0;
        *(int *)(arg0 + 0x90) = 0;
        *(int *)(arg0 + 0x54) = 0x447A0000;
        *(int *)(arg0 + 0x50) = 0xC3480000;
        *(int *)(arg0 + 0x5C) = 0x40000000;
        *(int *)(arg0 + 0x8C) = 0x40C00000;
        *(short *)(arg0 + 0x5A) = 0;
        *(char *)(arg0 + 0x6D) = 0;
        *(char *)(arg0 + 0x6C) = 0;
        *(short *)(arg0 + 0xA0) = 0;
        *(float *)(arg0 + 0x44) = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
        func_0018CE60(arg0, base + 0xB0, 5);
        sub = *(unsigned char *)(arg0 + 5);
        if (sub == 1) {
            return;
        }
        sub = *(unsigned char *)(arg0 + 6);
        if (sub == 0xA) {
            return;
        }
        if (sub == 0xF) {
            return;
        }
        if (sub == 0xD) {
            return;
        }
        *(unsigned char *)(arg0 + 6) = 8;
        if (D_00810700 == 0x12) {
            if (D_00810702 == 0) {
                *(unsigned char *)(arg0 + 6) = 0;
            }
        }
        func_0018C0C0(arg0);
        func_0018D7B0(arg0, 1);
        func_0018C0D0(arg0, 1);
        *(float *)(arg0 + 0x44) = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
        break;
    case 1:
        sub = *(unsigned char *)(arg0 + 4);
        switch (sub) {
        case 0:
            func_00191390(arg0, base);
            func_0018BC20(arg0, base);
            func_0018C0D0(arg0, 1);
            break;
        case 3:
            func_0022EEF0((unsigned char *)arg0);
            func_0018C0D0(arg0, 0);
            break;
        case 1:
        case 2:
        default:
            func_0018C0D0(arg0, 1);
            break;
        }
        break;
    }
}
