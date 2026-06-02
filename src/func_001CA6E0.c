// Tail-call thunk: forwards to func_001CA5E0 with a zeroed third argument.
extern void func_001CA5E0(int a0, int a1, int a2);

void func_001CA6E0(int a0, int a1) {
    func_001CA5E0(a0, a1, 0);
}
