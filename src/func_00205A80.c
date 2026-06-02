// Tail-call thunk: forwards to func_00205A50 with fixed second/third args.
extern void func_00205A50(int a0, int a1, int a2, int a3);

void func_00205A80(int a0, int a1, int a2, int a3) {
    func_00205A50(a0, 0x3F, 0, a3);
}
