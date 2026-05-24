// CFLAGS: -O4,p -sdatathreshold 0
extern int func_00122BB8(int);
extern unsigned char D_00242ED0[];

int func_00128600(int a0) {
    int r;
    unsigned char *base;
    r = func_00122BB8(a0) & 0xF;
    base = D_00242ED0 + a0 * 16;
    return *(base + r);
}
