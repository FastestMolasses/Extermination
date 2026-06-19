// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): unpack a packed word, dispatch id 0x41 (long args).
extern int func_001157F0(int a0, long a1, long a2, int a3);

int func_0011A658(unsigned long a0, int a1) {
    int lo = a0 & 0xFFFFFF;
    int hi = (a0 >> 24) & 0xFFFFFF;
    func_001157F0(0x41, lo, hi, a1);
    return 0;
}
