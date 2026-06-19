// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: call func_00119400(0x21, a1, a0, a2); return 0.
extern void func_00119400(int a0, int a1, int a2, int a3);

int func_001193D8(int a0, int a1, int a2) {
    func_00119400(0x21, a1, a0, a2);
    return 0;
}
