// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 83.03% NEARMISS).
// Fixes read from the original instructions:
//  - The body is one if/else: the work runs when the 0x36 halfword or
//    D_0081080F is nonzero and returns 1; the else arm returns 0. The old
//    early-return guard lowered differently (the "dead return-0 block" is the
//    else arm's return value set-up, not a compiler wall).
//
// Writes the start state into the record (byte 0 = 3, byte 4 = 2, bytes 5-7
// cleared), runs func_0012E070 on the second argument, and when D_0081080F is
// set copies the halfword at 0x34 into 0x36. Returns 0 without touching
// anything when both the 0x36 halfword and D_0081080F are zero.

extern void func_0012E070(char *p);
extern unsigned char D_0081080F;

int func_00128B80(char *arg0, char *arg1) {
    if (*(short *)(arg0 + 0x36) != 0 || D_0081080F != 0) {
        *(unsigned char *)(arg0 + 0) = 3;
        *(unsigned char *)(arg0 + 4) = 2;
        *(unsigned char *)(arg0 + 5) = 0;
        *(unsigned char *)(arg0 + 6) = 0;
        *(unsigned char *)(arg0 + 7) = 0;
        func_0012E070(arg1);
        if (D_0081080F != 0) {
            *(short *)(arg0 + 0x36) = *(short *)(arg0 + 0x34);
        }
        return 1;
    } else {
        return 0;
    }
}
