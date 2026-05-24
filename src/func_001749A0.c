// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001C67E0(int, int);

int func_001749A0(int a0, short a1, int a2) {
    if (a2 == 0) {
        short v0 = *(short *)(a0 + 0x20C);
        if ((short)a1 == v0) {
            return 1;
        }
    }
    *(short *)(a0 + 0x20C) = a1;
    func_001C67E0(a0, *(short *)(a0 + 0x20C));
    return 0;
}
