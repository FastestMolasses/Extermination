// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: return func_00110AB8(a0, a1)->[0x58].
extern int *func_00110AB8(int a0, int a1);

int func_00110B18(int a0, int a1) {
    int *p = func_00110AB8(a0, a1);
    return p[0x16];
}
