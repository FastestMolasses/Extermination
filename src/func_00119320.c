// COMPILER: eegcc
// CFLAGS: -O2
extern void func_001157F0(int a0, int a1, int a2, int a3);

void func_00119320(void) {
    int i;
    for (i = 0; i < 2; i++) {
        func_001157F0(0xB, i, 0xFFFFFF, 0);
        func_001157F0(0x28, i, 0, 0);
    }
    func_001157F0(0x1F, 0, 0, 0);
}
