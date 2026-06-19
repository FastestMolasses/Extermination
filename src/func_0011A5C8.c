// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf: if a0 < 0x30, func_001157F0(0x3F, a0, 0, 0) and return 0; else -1.
extern void func_001157F0(int a0, int a1, int a2, int a3);

int func_0011A5C8(unsigned int a0) {
    int r = -1;
    if (a0 < 0x30) {
        func_001157F0(0x3F, a0, 0, 0);
        r = 0;
    }
    return r;
}
