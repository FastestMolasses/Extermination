// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.13% NEARMISS).
// Fixes read from the original instructions:
//  - The scratchpad vector 0x700038A0 is a relocated extern (idiom-32, opted in
//    with // SPAD:), declared float: the first word of each of the three vector
//    stores and the func_001026A0 vector argument go through D_700038A0. As a
//    relocated symbol its upper half is no longer speculated into the branch
//    slot of the +0x0D == 1 test, which the original leaves empty.
//    0x700038A4..AC stay literals.
//  - func_001026A0 is declared with void * parameters, as its definition in
//    src/ has, so &D_700038A0 is passed without a cast (a cast reordered the
//    argument setup).

//
// Player state handler dispatched on the sub-state byte +0x06 (0, 10, 20, 21,
// 22, 30). Corrected in decomp lane round 8 from the original instructions
// (93.89% before):
//  - Sub-state 21 calls func_00179880(self, self + 0x2EC): the target sets $a1
//    to self + 0x2EC and leaves self in $a0; the old C passed only the pointer.
//  - Sub-state 0 calls func_001B0B50() with no argument (the target sets none
//    and func_001B0B50 takes none); the old C passed the state byte.
//  - The scratchpad float 0x70003A20 is the relocated extern D_70003A20
//    (MATCHING_GUIDE idiom-32), as the original addresses it.
//  - Every case ends with break, not return: nothing follows the switch, so the
//    two are equivalent, and break gives the target's empty branch delay slots
//    (MATCHING_GUIDE idiom-27).

extern void func_001B0B50(void);
extern void func_001749A0(char *, int, int, float);
extern void anim_eval_skeleton(char *);
extern void func_001026A0(void *, void *, void *);
extern void build_trs_matrix(char *, char *, char *, char *);
extern int func_0019AD00(char *, char *, int);
extern float func_0011E620(float, float);
extern float func_001B1470(float);
extern int func_00188550(char *);
extern void func_00179880(unsigned char *a0, float *a1);
extern int func_00175900(char *, int);
extern void func_00182870(char *, int);
extern short D_0028A9A0;
extern float D_700038A0;
extern char D_700038B0[];
extern float D_700031B0;
extern float D_700031B8;
extern char *D_700031D0;
extern float D_70003A20;

void func_0016EBA0(char *arg0) {
    unsigned char st;
    char *v;
    short t;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        func_001B0B50();
        *(int *)(arg0 + 0x38) = 0;
        if (*(unsigned char *)(arg0 + 0xD) == 0) {
            *(unsigned char *)(arg0 + 6) = 0xA;
            func_001749A0(arg0, 0x153, 0, 1.0f);
            anim_eval_skeleton(arg0);
            D_700038A0 = 0.0f;
            *(volatile float *)0x700038A4 = 0.0f;
            *(volatile float *)0x700038A8 = 5.0f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001026A0(arg0 + 0xB0, arg0 + 0xD0, &D_700038A0);
            break;
        }
        if (*(unsigned char *)(arg0 + 0xD) == 1) {
            D_700038A0 = 0.0f;
            *(volatile float *)0x700038A4 = 0.0f;
            *(volatile float *)0x700038A8 = 5.0f;
            *(volatile float *)0x700038AC = 1.0f;
            func_001026A0(arg0 + 0xB0, arg0 + 0xD0, &D_700038A0);
            build_trs_matrix(arg0 + 0xD0, arg0 + 0xB0, arg0 + 0xC0, arg0 + 0x60);
            func_001749A0(arg0, 0x72, 0, 1.0f);
            *(unsigned char *)(arg0 + 6) = 0x14;
            *(int *)(arg0 + 0x2EC) = 0;
            *(short *)(arg0 + 0x28) = 8;
            break;
        }
        D_700038A0 = 0.0f;
        *(volatile float *)0x700038A4 = -3.0f;
        *(volatile float *)0x700038A8 = -5.0f;
        *(volatile float *)0x700038AC = 1.0f;
        func_001026A0(D_700038B0, arg0 + 0xD0, &D_700038A0);
        if (func_0019AD00(arg0, D_700038B0, 7) != 0) {
            v = (char *)*(volatile int *)0x700031D0;
            *(float *)(arg0 + 0xB0) = *(volatile float *)0x700031B0 + (1.5f * *(float *)(v + 0x24));
            v = (char *)*(volatile int *)0x700031D0;
            *(float *)(arg0 + 0xB8) = *(volatile float *)0x700031B8 + (1.5f * *(float *)(v + 0x2C));
            *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) - 20.5f;
            v = (char *)*(volatile int *)0x700031D0;
            D_70003A20 = func_0011E620(-*(float *)(v + 0x2C), *(float *)(v + 0x24));
            *(float *)(arg0 + 0xC4) = func_001B1470(4.712389f + D_70003A20);
            *(unsigned char *)(arg0 + 6) = 0x1E;
            func_001749A0(arg0, func_00188550(arg0), 0, 0.0f);
            break;
        }
        break;
    case 10:
        if (D_0028A9A0 == 0 && (*(int *)(arg0 + 0x200) & 0x1000)) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0;
            break;
        }
        break;
    case 20:
        t = *(short *)(arg0 + 0x28);
        *(short *)(arg0 + 0x28) = t - 1;
        if (t == 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            break;
        }
        break;
    case 21:
        func_00179880((unsigned char *)arg0, (float *)(arg0 + 0x2EC));
        if (func_00175900(arg0, 1) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            func_00182870(arg0, 1);
            func_001749A0(arg0, 0x6D, 0, 8.0f);
            break;
        }
        break;
    case 22:
        if (D_0028A9A0 == 0 && (*(int *)(arg0 + 0x200) & 0x1000)) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0;
            break;
        }
        break;
    case 30:
        if (D_0028A9A0 == 0) {
            *(char *)(arg0 + 5) = 0x18;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x2C;
            *(char *)(arg0 + 0x1F1) = 0;
            *(unsigned char *)(arg0 + 0xD) = 2;
        }
        break;
    }
}
