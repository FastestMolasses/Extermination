// CFLAGS: -O4,p -sdatathreshold 4
extern unsigned char D_00810700[8];
extern unsigned char D_00810730[0x100];
extern unsigned char D_00810701[8];
extern unsigned char D_00275BD8;
extern void func_001FF080(int, int);

void func_001FEFE0(int a0) {
    int v0 = a0 & 0xff;
    v0 = v0 & 0x80;
    if (v0 != 0) {
        int v1 = D_00810700[0];
        D_00810730[v1] = a0;
    }
    D_00810701[0] = a0 & 0x7f;
    D_00275BD8 = 1;
    func_001FF080(2, 0);
}
