// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00108640(int a0, int a1);
extern void func_001086F8(int a0, int a1);

int func_00108FF8(int a0) {
    func_001086F8(a0, 0x38);
    func_001086F8(a0, 0x28);
    while (func_00108640(a0, 1) == 1) {
        func_001086F8(a0, 0x18);
    }
    return 1;
}
