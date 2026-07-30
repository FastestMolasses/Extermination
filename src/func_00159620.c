// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SEMANTICS: Two-level actor/effect state machine on the entity at `e`.
// Outer state byte e[4]: 0 = init, 1 = run the sub-state machine, 2 = advance
// e[4] by one, 3 = call func_001AFC10, anything else = fall straight through to
// the epilogue. Init (case 0) bails out if func_001B0FD0 returns non-zero;
// otherwise it resets e[0xA], parks the gp-rel descriptor &D_00275458 at e+0x30,
// sets e[8]=5 and e[0]=1, then fires three func_001C5570 requests (ids 0x6E/0x6F/
// 0x70), each time staging a 4-word float parameter block in scratchpad at
// 0x700038A0 (word0 = 0, word1 = 1.0f / 0.4f / 1.0f, word2 = 0, word3 = 0.2f).
// The run state dispatches on the sub-state byte e[5] through jtbl_0026D370
// (7 entries; index 3 is empty, index >= 7 skips straight to the tail):
//   0 - poll func_00157B30(e, e+0x1F0): 2 -> sub-state 4, 3 -> sub-state+1,
//       anything else -> sub-state 5 plus a func_001FB9F0(0x3EA, ...) event.
//   1 - if e[0xA]==0 go to sub-state 4 and play clip D_00247DA0, else advance to
//       sub-state 2, play D_00247FA0 and kick func_001BA1F0.
//   2/4 - wait on func_001BA1F0, then clear e[0xB], set e[0]=1, reset sub-state.
//   5 - wait on func_001BA1F0, then sub-state 6.
//   6 - if e[0xA]==0 go to sub-state 4 with clip D_00247DA0, else sub-state 2,
//       event func_001FB9F0(0x3EC, ...), clip D_00247FE0 and func_001BA1F0.
// Every run-state path then calls func_001B1B70(e) and the virtual at e+0x4C.
// D_700038A0 is the scratchpad staging block: it is written through absolute
// addresses (0x700038A0..0x700038AC) and passed by symbol, which is exactly how
// the target encodes it (raw lui/sw for the stores, %hi/%lo for the argument).
extern int func_00157B30();
extern int func_001AFC10();
extern int func_001B0FD0();
extern int func_001B1B70();
extern int func_001BA1A0();
extern int func_001BA1F0();
extern int func_001C5570();
extern int func_001C6380();
extern int func_001FB9F0();
extern int D_00247DA0[64];
extern int D_00247FA0[64];
extern int D_00247FE0[64];
extern int D_00275458;
extern int D_700038A0[4];

void func_00159620(char *e) {
    unsigned char st = *(unsigned char *)(e + 4);
    char *sub = e + 0x1F0;
    unsigned char sub_st;
    int r;

    switch (st) {
    case 0:
        if (func_001B0FD0(e) != 0) {
            return;
        }
        func_001C6380(e);
        *(char *)(e + 0xA) = 0;
        *(int *)(e + 0x30) = (int)&D_00275458;
        *(char *)(e + 8) = 5;
        *(char *)(e + 0) = 1;
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0x3F800000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3E4CCCCD;
        func_001C5570(e, D_700038A0, 0x6E, 1);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0x3ECCCCCD;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3E4CCCCD;
        func_001C5570(e, D_700038A0, 0x6F, 1);
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0x3F800000;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3E4CCCCD;
        func_001C5570(e, D_700038A0, 0x70, 1);
        return;
    case 1:
        sub_st = *(unsigned char *)(e + 5);
        switch (sub_st) {
        case 0:
            r = func_00157B30(e, sub);
            if (r == 0) {
            } else if (r == 2) {
                *(char *)(e + 5) = 4;
            } else if (r == 3) {
                *(unsigned char *)(e + 5) = *(unsigned char *)(e + 5) + 1;
            } else {
                *(char *)(e + 5) = 5;
                func_001FB9F0(0x3EA, 0x1000, 0x1000, 0x1000);
            }
            break;
        case 1:
            if (*(unsigned char *)(e + 0xA) == 0) {
                *(char *)(e + 5) = 4;
                func_001BA1A0(sub, D_00247DA0);
            } else {
                *(char *)(e + 5) = sub_st + 1;
                func_001BA1A0(sub, D_00247FA0);
                func_001BA1F0(e);
            }
            break;
        case 2:
            if (func_001BA1F0(e) != 0) {
                *(char *)(e + 0xB) = 0;
                *(char *)(e + 0) = 1;
                *(char *)(e + 5) = 0;
            }
            break;
        case 3:
            break;
        case 4:
            if (func_001BA1F0(e) != 0) {
                *(char *)(e + 0) = 1;
                *(char *)(e + 0xB) = 0;
                *(char *)(e + 5) = 0;
            }
            break;
        case 5:
            if (func_001BA1F0(e) != 0) {
                *(char *)(e + 5) = 6;
            }
            break;
        case 6:
            if (*(unsigned char *)(e + 0xA) == 0) {
                *(char *)(e + 5) = 4;
                func_001BA1A0(sub, D_00247DA0);
            } else {
                *(char *)(e + 5) = 2;
                func_001FB9F0(0x3EC, 0x1000, 0x1000, 0x1000);
                func_001BA1A0(sub, D_00247FE0);
                func_001BA1F0(e);
            }
            break;
        }
        func_001B1B70(e);
        (*(void (**)(char *))(e + 0x4C))(e);
        return;
    case 2:
        *(char *)(e + 4) = st + 1;
        return;
    case 3:
        func_001AFC10(e);
        break;
    }
}
