// Tail-call thunk: dispatches to func_001D3900 with selector 3.
extern void func_001D3900(int sel, int arg);

void func_001D3990(int arg) {
    func_001D3900(3, arg);
}
