// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 82.25% NEARMISS).
// Fixes read from the original instructions:
//  - the three sub-state handlers (func_00229770/00229C00/00229C90) take only
//    the state block pointer; the old C passed a second argument (1).
//  - every callee now has an explicit prototype.
//  - one switch whose cases end in `break` with a single `return 0` after it;
//    case 0 does not fall into a shared return, case 2 is an empty `break`.
//
// State-machine step on the block at D_00810040: state 0 initialises it (state
// becomes 1, clears fields, kicks func_001FE8D0); state 1 runs the sub-state
// handler selected by byte 1 and stores its result at byte 0x16, returning 2
// when arg0 is set and that result is 2; state 3 returns 1; otherwise 0.

extern unsigned char D_00810040[];
void func_00225CF0(unsigned char *a0, unsigned int a1, unsigned int a2);
void func_001FE8D0(void);
int func_00229770(unsigned char *arg0);
int func_00229C00(unsigned char *arg0);
int func_00229C90(unsigned char *arg0);

int func_0022A460(int arg0) {
    unsigned char *o = D_00810040;

    switch (o[0]) {
    case 0:
        o[0] = 1;
        func_00225CF0(o, 1, 7);
        o[9] = 0;
        o[0xA] = 0;
        *(int *)(o + 0x24) = 0;
        *(int *)(o + 0x10) = 0;
        func_001FE8D0();
        break;
    case 1:
        switch (o[1]) {
        case 0:
            o[0x16] = func_00229770(o);
            break;
        case 1:
            o[0x16] = func_00229C00(o);
            break;
        case 2:
            o[0x16] = func_00229C90(o);
            break;
        }
        if (arg0 != 0 && o[0x16] == 2) {
            return 2;
        }
        break;
    case 2:
        break;
    case 3:
        return 1;
    }
    return 0;
}