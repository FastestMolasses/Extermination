// Tail-call thunk: requests a 0x1000-aligned allocation of class 2.
extern void func_001FB9F0(int sel, int size, int a2, int a3);

void func_0020CD80(void) {
    func_001FB9F0(2, 0x1000, 0x1000, 0x1000);
}
