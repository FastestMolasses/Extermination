// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_0081070A;

int func_00128390(int a0, int a1) {
    int v0;
    int v1;
    if (D_0081070A == 0) {
        v0 = 0xf;
        v1 = 0x1e;
        v0 = a1 != 0 ? v1 : v0;
        return v0;
    }
    v0 = 0x1e;
    v1 = 0x32;
    v0 = a1 != 0 ? v1 : v0;
    return v0;
}
