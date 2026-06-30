// COMPILER: eegcc
// CFLAGS: -O2
extern int func_001157F0(int a, long b, int c, int d);

int func_0011A848(unsigned long a0) {
    if (a0 <= 0xFFFF) {
        func_001157F0(0x4B, (long)(int)a0, 0, 0);
    }
    return 0;
}
