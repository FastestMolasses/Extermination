// COMPILER: eegcc
// CFLAGS: -O2
extern int func_0011B3F8(void);

int func_0011B410(int a0) {
    if (a0 == 0) {
        *(volatile int *)0x10003010 = func_0011B3F8() & 0xFFFFFFFE;
    } else if (a0 == 1) {
        *(volatile int *)0x10003010 = func_0011B3F8() | 1;
    }
    return 1;
}
