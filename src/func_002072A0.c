// Tail-call thunk: requests a 0x1000-aligned allocation of class 0x8CD.
extern void func_001FB9F0(int sel, int size, int a2, int a3);

void func_002072A0(void) {
    func_001FB9F0(0x8CD, 0x1000, 0x1000, 0x1000);
}
