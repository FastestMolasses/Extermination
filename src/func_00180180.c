// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001749A0(int, int, int);

void func_00180180(unsigned char *p, int sel) {
    if (sel == 0) {
        if (p[0x2F1] == 0) {
            func_001749A0((int)p, 0xA8, 0);
            return;
        }
        func_001749A0((int)p, 0xAE, 0);
        return;
    }
    if (p[0x2F1] == 0) {
        func_001749A0((int)p, 0xAB, 0);
        return;
    }
    func_001749A0((int)p, 0xB1, 0);
}
