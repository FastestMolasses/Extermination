// Tail-call thunk: dispatches to func_001D42E0 with selector 3.
extern void func_001D42E0(int sel, int a0, int a1);

void func_001D4430(int a0, int a1) {
    func_001D42E0(3, a0, a1);
}
