// Tail-call thunk: dispatches to func_001D3E40 with selector 0.
extern void func_001D3E40(int sel, int arg);

void func_001D3F50(int arg) {
    func_001D3E40(0, arg);
}
