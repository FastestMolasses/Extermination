// COMPILER: eegcc
// CFLAGS: -O2
// Gate on func_0011B6E0(); then set MMIO 0x10003810 per mode a0; return status.
extern int func_0011B6E0(void);

int func_0011B608(int a0) {
    int st = func_0011B6E0();
    if (st == 0) {
        return 0;
    }
    if (st == 1) {
        return 0;
    }
    if (a0 == 0) {
        *(volatile int *)0x10003810 = 2;
    } else if (a0 == 1) {
        *(volatile int *)0x10003810 = 4;
    }
    return 1;
}
