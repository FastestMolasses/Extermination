// Tail-call thunk: dispatches to func_001D4A90 with selector 0.
extern void func_001D4A90(int sel, int arg);

void func_001D4B10(int arg) {
    func_001D4A90(0, arg);
}
