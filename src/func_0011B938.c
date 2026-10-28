// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// SDK VIF1: unless func_0011BA10() reports state 0 or 1 (then returns 0),
// writes VIF1_FBRST (0x10003C10) = 2 (force break) for mode 0 or 4 (stop) for
// mode 1; any other mode writes nothing. Returns 1 in both non-early cases.

extern int func_0011BA10(void);
int func_0011B938(int mode) {
    int st = func_0011BA10();
    if (st == 0) {
        return 0;
    }
    if (st == 1) {
        return 0;
    }
    if (mode == 0) {
        *(volatile int *)0x10003C10 = 2;
    } else if (mode == 1) {
        *(volatile int *)0x10003C10 = 4;
    }
    return 1;
}
