// Tail-call thunk: forwards to func_001F4D40 with a fixed 5.5 in the
// second float slot.
extern void func_001F4D40(int a0, int a1, float f12, float f13);

void func_001F4E20(int a0, int a1, float f12) {
    func_001F4D40(a0, a1, f12, 5.5f);
}
