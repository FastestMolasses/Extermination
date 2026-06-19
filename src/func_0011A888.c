// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: if (unsigned)a0 <= 0xFFFF call func_001157F0(0x4C, (int)a0,0,0).
// a0 arrives as a 64-bit arg; low 32 bits are sign-extended (dsll32/dsra32).
extern int func_001157F0(int a0, long a1, int a2, int a3);

int func_0011A888(unsigned long a0) {
    if (a0 <= 0xFFFF) {
        func_001157F0(0x4C, (int)a0, 0, 0);
    }
    return 0;
}
