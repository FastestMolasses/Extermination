// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: func_001157F0(0x4A, (int)a0, a1, a2); a0 is a 64-bit arg
// truncated to int (dsll32/dsra32 sign-extend).
extern void func_001157F0(int a0, int a1, int a2, int a3);

void func_0011A918(long a0, int a1, int a2) {
    func_001157F0(0x4A, (int)a0, a1, a2);
}
