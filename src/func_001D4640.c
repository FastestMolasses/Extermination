// Tail-call thunk: dispatches to func_001D4440 with selector 3.
extern void func_001D4440(int sel, int a0, int a1);

void func_001D4640(int a0, int a1) {
    func_001D4440(3, a0, a1);
}
