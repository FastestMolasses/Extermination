// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): bounds-checked dispatch id 0x49, else -1.
extern int func_001157F0(int a0, int a1, int a2, int a3);

int func_0011A7F0(int a0) {
    int ret = -1;
    if (a0 < 0x10) {
        func_001157F0(0x49, a0, 0, 0);
        ret = 0;
    }
    return ret;
}
