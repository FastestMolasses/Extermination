// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: align both args down to 64 bytes, tail-call func_0010C020.
extern void func_0010C020(int a0, int a1);

void func_0010C140(int a0, int a1) {
    func_0010C020(a0 & 0xFFFFFFC0, a1 & 0xFFFFFFC0);
}
