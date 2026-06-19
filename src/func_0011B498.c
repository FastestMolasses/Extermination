// COMPILER: eegcc
// CFLAGS: -O2
int func_0011B498(void) {
    int v = *(volatile int *)0x10003020;
    if (v & 0x8) {
        return 1;
    }
    if (v & 0xFC0) {
        return 2;
    }
    return 0;
}
