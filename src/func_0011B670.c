// COMPILER: eegcc
// CFLAGS: -O2
// if (func_0011B6E0() == 1) { *(int*)0x10003810 = 8; return 1; } return 0;
extern int func_0011B6E0(void);

int func_0011B670(void) {
    if (func_0011B6E0() != 1) {
        return 0;
    }
    *(volatile int *)0x10003810 = 8;
    return 1;
}
