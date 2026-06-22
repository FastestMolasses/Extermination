// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001749A0(int, int, int);

void func_00180200(unsigned char *p, int sel) {
    if (sel == 0) {
        if (p[0x2F1] == 0) {
            func_001749A0((int)p, 0xA9, 0);
            return;
        }
        func_001749A0((int)p, 0xAF, 0);
        return;
    }
    if (p[0x2F1] == 0) {
        func_001749A0((int)p, 0xAC, 0);
        return;
    }
    func_001749A0((int)p, 0xB2, 0);
}
