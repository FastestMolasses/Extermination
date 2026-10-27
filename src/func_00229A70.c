// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 78.56% NEARMISS).
// Fixes read from the original instructions:
//  - The object pointer is a plain `char *` copied into a local (as in the
//    byte-matched sibling func_002283B0); with it the gate call sets up its
//    first argument first and the 0x54 address in the call's delay slot.
//  - The switch cases end in `break` with one shared `return 0` after it.
//  - Branch senses: the busy-flag test is `if (flag) clear; else return 0;`
//    and the result test is `if (r == 0) clear; else set;`. These give the
//    original's separate early-return blocks; the old C merged them.
//  - func_001FEC20 is declared with six int parameters, matching the sibling
//    files. Its own definition in src/ is `(void)`, but the original sets up
//    six argument registers at every call site.
//
// Two-stage async-load state machine, sibling of func_002283B0/func_00228530.
// Guard: func_001FE9A0(1, &+0x50, &+0x54); on 0, return. state(+4)==0: issue
// step 0 via func_001FEC20(0,0,&D_002738E0,0,1,&D_00821500); on -1 honor the
// busy flag D_00275C58 (==0 -> return, else clear +0x1A), else +0x1A = (result
// != 0); func_001FE8D0(); advance state to 1. state==1: same for step 1, flag
// into +0x1B, advance to 2. state==2: +3 = ((+0x1A||+0x1B) ? 1 : 2); reset
// state to 0. Returns 0.
extern int func_001FE9A0(int, char *, char *);
extern int func_001FEC20(int, int, int, int, int, int);
extern void func_001FE8D0(void);
extern int D_002738E0[];
extern int D_00821500[];
extern int D_00275C58;

int func_00229A70(char *arg0) {
    char *s0;
    int r;

    s0 = arg0;
    if (func_001FE9A0(1, s0 + 0x50, s0 + 0x54) == 0) {
        return 0;
    }
    switch (*(unsigned char *)(s0 + 4)) {
    case 0:
        r = func_001FEC20(0, 0, (int)D_002738E0, 0, 1, (int)D_00821500);
        if (r == -1) {
            if (D_00275C58 != 0) {
                s0[0x1A] = 0;
            } else {
                return 0;
            }
        } else if (r == 0) {
            s0[0x1A] = 0;
        } else {
            s0[0x1A] = 1;
        }
        func_001FE8D0();
        s0[4] = 1;
        break;
    case 1:
        r = func_001FEC20(1, 0, (int)D_002738E0, 0, 1, (int)D_00821500);
        if (r == -1) {
            if (D_00275C58 != 0) {
                s0[0x1B] = 0;
            } else {
                return 0;
            }
        } else if (r == 0) {
            s0[0x1B] = 0;
        } else {
            s0[0x1B] = 1;
        }
        func_001FE8D0();
        s0[4] = 2;
        break;
    case 2:
        if (*(unsigned char *)(s0 + 0x1A) != 0 || *(unsigned char *)(s0 + 0x1B) != 0) {
            s0[3] = 1;
        } else {
            s0[3] = 2;
        }
        s0[4] = 0;
        break;
    }
    return 0;
}
