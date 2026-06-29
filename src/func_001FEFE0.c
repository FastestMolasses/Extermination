// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
extern unsigned char D_00810700[8];
extern unsigned char D_00810701[8];
extern unsigned char D_00810730[256];
extern unsigned char D_00275BD8;
extern void func_001FF080(int, int);

void func_001FEFE0(char a0) {
    if ((unsigned char)a0 & 0x80) {
        D_00810730[D_00810700[0]] = (unsigned char)a0;
    }
    D_00810701[0] = (unsigned char)(a0 & 0x7F);
    D_00275BD8 = 1;
    func_001FF080(2, 0);
}
