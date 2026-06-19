// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): call dispatch id 0x20, always return 0.
extern int func_00119400(int a0, int a1, int a2, int a3);

int func_001193A8(int a0, int a1, int a2) {
    func_00119400(0x20, a0, a1, a2);
    return 0;
}
