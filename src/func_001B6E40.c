// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane, 67.3% -> 100%): mwcc 2.3.3 with the second
// test written as if/else. The target leaves the else-branch's `v0 = 0` as
// dead code after the then-branch's `b` (idiom-33) and copies it into the
// bnez delay slot.
//
// Semantics (target 0x001B6E40): if the scratchpad mode byte 0x70003B8D is
// nonzero, return 0. Otherwise call func_00182BF0(D_008102B0); when that
// returns 0, set the mode byte to 3 and return 1, else return 0.

extern unsigned char D_008102B0[8];
extern int func_00182BF0(unsigned char *);

int func_001B6E40(void) {
    if (*(unsigned char *)0x70003B8D != 0) {
        return 0;
    }
    if (func_00182BF0(D_008102B0) == 0) {
        *(unsigned char *)0x70003B8D = 3;
        return 1;
    } else {
        return 0;
    }
}
