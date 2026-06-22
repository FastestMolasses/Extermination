// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001FB9F0(int a0, int a1, int a2, int a3);
extern int D_00281F30;

void func_001FC6E0(void) {
    int i;
    int *p;
    int v;
    int a0;

    i = 0;
    p = &D_00281F30;
    do {
        v = p[0];
        if (v != 0) {
            p[0] = v - 1;
        } else {
            a0 = p[1];
            if (a0 != -1) {
                if (a0 != 0) {
                    func_001FB9F0(a0, 0x1000, p[2], p[3]);
                }
                p[1] = -1;
            }
        }
        i += 1;
        p += 4;
    } while (i < 0xA);
}
