// COMPILER: eegcc
// CFLAGS: -O2
extern int func_001157F0(int, int, int, int);

int func_0011A6E8(unsigned long a0) {
    func_001157F0(0x43, (int)(a0 & 0xFFFFFF), (int)((a0 >> 24) & 0xFFFFFF), 0);
    return 0;
}
