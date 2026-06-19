// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): bounds + range checked dispatch id 0x4D, else -1.
extern int func_001157F0(int a0, int a1, int a2, int a3);

int func_0011A8C8(int a0, unsigned int a1) {
    int ret = -1;
    if (a0 < 0x10) {
        if (a1 <= 0x1FFFFF) {
            func_001157F0(0x4D, a0, a1, 0);
            ret = 0;
        }
    }
    return ret;
}
