// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// One-call non-leaf wrapper: calls func_001FB9F0(0x3EF, 0x1000, 0x1000, 0x1000)
// and returns 1. mwcc 2.3.3 reproduces the `paddub aN,a1,zero` register-copy
// idiom (a2,a3 = a1) that 991202 does not (81.8%).
extern void func_001FB9F0(int, int, int, int);

int func_001575B0(void) {
    func_001FB9F0(0x3EF, 0x1000, 0x1000, 0x1000);
    return 1;
}
