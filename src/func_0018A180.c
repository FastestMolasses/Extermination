// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001B61C0(int a, int b, int c, int d);
extern int func_00122BB8(void);
extern void func_001FBD50(void *p, int n, int z, float f);
extern unsigned char D_008102B0[];

void func_0018A180(unsigned char *p) {
    int v;
    int b;
    p[0xA] = 1;
    p[0] = 2;
    func_001B61C0(0, 0xD0, 0xA, 1);
    v = func_00122BB8();
    b = v & 1;
    if (v < 0) {
        if (b != 0) {
            b -= 2;
        }
    }
    func_001FBD50(D_008102B0, b + 0x180, 0, 300.0f);
}
