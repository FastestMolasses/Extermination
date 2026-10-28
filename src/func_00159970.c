// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 77.35% NEARMISS).
// Fixes read from the original instructions:
//  - The "argument hoisting" wall was wrong externs: func_001B0FD0, func_001BA1F0
//    and func_001AFC10 take only the entity, func_00157CE0 takes (entity, anim),
//    func_001BA1A0 takes (anim, clip). The 2/3 constants in a2/a3 are the state
//    compare values, not call arguments. Unprototyped externs as in the
//    byte-matched twin func_00159620.
//  - -sdatathreshold 8 (D_00275458 is gp-relative); the scratchpad block and the
//    clip are arrays so they stay %hi/%lo.
//  - The sub-state-0 result dispatch is an if/else-if chain (0, 2, 3, else), not
//    a switch; the 0x71/0x7B id is an if/else; case 0 bails with an early return.
//  - The clip argument carries an (int *) cast, which routes it through a
//    temporary as in the original.
//
// SEMANTICS: Per-entity state machine dispatched on the state byte e[4] (0..3);
// sub = e + 0x1F0 is the animation sub-object. State 0: if func_001B0FD0(e)
// reports done (== 0), fires the spawn path -- func_001C6380(e), clears e[0xA],
// installs handler &D_00275458 at e+0x30, sets e[0] = 1, seeds the scratchpad
// float block at 0x700038A0 (0, 0.3f, 0, 0.2f) and kicks
// func_001C5570(e, block, id, 1) with id 0x71 when (e[2] & 0x1F) == 6 else 0x7B.
// State 1: sub-state machine on e[5] (0..3) polling func_00157CE0, playing clip
// D_00247020 via func_001BA1A0 and waiting on func_001BA1F0; then always runs
// func_001B17A0(e) and the virtual at e+0x4C. State 2: bumps the state byte.
// State 3: func_001AFC10(e).
extern int func_00157CE0();
extern void func_001AFC10(unsigned char *self);
extern int func_001B0FD0();
extern int func_001B17A0();
extern int func_001BA1A0();
extern int func_001BA1F0();
extern int func_001C5570();
extern int func_001C6380();
extern int func_001FB9F0();
extern int D_00247020[64];
extern int D_00275458;
extern int D_700038A0[4];

void func_00159970(char *e) {
    unsigned char st = *(unsigned char *)(e + 4);
    char *sub = e + 0x1F0;
    unsigned char sub_st;
    int r;
    int id;

    switch (st) {
    case 0:
        if (func_001B0FD0(e) != 0) {
            return;
        }
        func_001C6380(e);
        *(char *)(e + 0xA) = 0;
        *(int *)(e + 0x30) = (int)&D_00275458;
        *(char *)(e + 0) = 1;
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0x3E99999A;
        *(int *)0x700038A8 = 0;
        *(int *)0x700038AC = 0x3E4CCCCD;
        if ((*(unsigned char *)(e + 2) & ~0xE0) == 6) {
            id = 0x71;
        } else {
            id = 0x7B;
        }
        func_001C5570(e, D_700038A0, id, 1);
        return;
    case 1:
        sub_st = *(unsigned char *)(e + 5);
        switch (sub_st) {
        case 0:
            r = func_00157CE0(e, sub);
            if (r == 0) {
            } else if (r == 2) {
                *(char *)(e + 5) = 2;
            } else if (r == 3) {
                *(unsigned char *)(e + 5) = *(unsigned char *)(e + 5) + 1;
            } else {
                *(char *)(e + 5) = 3;
                func_001FB9F0(0x3E9, 0x1000, 0x1000, 0x1000);
            }
            break;
        case 1:
            *(char *)(e + 5) = sub_st + 1;
            func_001BA1A0(sub, (int *)D_00247020);
            break;
        case 2:
            if (func_001BA1F0(e) != 0) {
                *(char *)(e + 0xB) = 0;
                *(char *)(e + 0) = 1;
                *(char *)(e + 5) = 0;
            }
            break;
        case 3:
            if (func_001BA1F0(e) != 0) {
                *(char *)(e + 5) = 1;
            }
            break;
        }
        func_001B17A0(e);
        (*(void (**)(char *))(e + 0x4C))(e);
        return;
    case 2:
        *(char *)(e + 4) = st + 1;
        return;
    case 3:
        func_001AFC10((unsigned char *)e);
        break;
    }
}
