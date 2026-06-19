// COMPILER: eegcc
// CFLAGS: -O2
extern void func_001274B0(long a0, long a1);

int func_0011DB90(long *a0) {
    func_001274B0(a0[1], a0[1]);
    return 0;
}
