// COMPILER: eegcc
// CFLAGS: -O2
extern int func_001157F0(int a, long b, long c, int d);

int func_0011A6A0(unsigned long a0) {
    func_001157F0(0x42, (int)(a0 & 0xFFFFFF), (int)((a0 >> 24) & 0xFFFFFF), 0);
    return 0;
}
