// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: if func_0011BA10()==1, write 8 to MMIO 0x10003C10 and return 1; else 0.
extern int func_0011BA10(void);

int func_0011B9A0(void) {
    if (func_0011BA10() != 1) {
        return 0;
    }
    *(volatile int *)0x10003C10 = 8;
    return 1;
}
