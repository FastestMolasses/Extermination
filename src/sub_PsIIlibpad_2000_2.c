// COMPILER: eegcc
// CFLAGS: -O2
//
// SDK/lowmem (ee-gcc unit). Publishes the request-size word (15) into the
// request block at D_00279A80, then issues a transfer/command via
// func_0010E8A8 with the D_00279700 descriptor and the D_00279A80 block used
// as both source and destination (0x80 bytes each). On failure (negative
// return) it reports 0; otherwise it returns the status word at D_00279A80+0xC,
// clearing the D_00241CB0 latch when that status is 1.
//
// Matching notes (ee-gcc 2.9-991111-01 -O2, objdiff 100.0):
//  * plain global accesses are correct here: ee-gcc CSEs the symbol address of
//    D_00279A80 into the callee-saved $s0 for the two pointer args and reuses it
//    for the post-call load, while the `= 15` store keeps its own %lo on the
//    lui register.
//  * the forward `bgezl` (eegcc idiom-1) fills itself: the taken path's first
//    op is the `D_00279A80[3]` load through the already-live $s0 base, so the
//    delay-slot pass annuls it. No rewrite needed.

extern int D_00279A80[];
extern unsigned char D_00279700[];
extern int D_00241CB0;

extern int func_0010E8A8();

int sub_PsIIlibpad_2000_2(void)
{
    int state;

    D_00279A80[0] = 15;

    if (func_0010E8A8(D_00279700, 1, 0, D_00279A80, 0x80, D_00279A80, 0x80, 0, 0) < 0) {
        return 0;
    }

    state = D_00279A80[3];
    if (state == 1) {
        D_00241CB0 = 0;
    }
    return state;
}
