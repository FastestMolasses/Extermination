// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: if func_0011B498()==1, clear MMIO 0x10003000 and return 1; else 0.
extern int func_0011B498(void);

int func_0011B358(void) {
    if (func_0011B498() != 1) {
        return 0;
    }
    *(volatile int *)0x10003000 = 0;
    return 1;
}
