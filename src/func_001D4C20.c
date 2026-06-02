// Tail-call thunk: dispatches to func_001D4B80 with selector 0.
extern void func_001D4B80(int sel, int arg);

void func_001D4C20(int arg) {
    func_001D4B80(0, arg);
}
