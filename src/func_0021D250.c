// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001749A0(int, int, int, float);
extern void func_001B61C0(int, int, int, int);
extern void func_001FBD50(int, int, int, float);

void func_0021D250(unsigned char *p, int a1) {
    *(unsigned char *)(p + 0x0) = 2;
    *(int *)(p + 0x220) = 0;
    *(unsigned char *)(p + 0x4) = 2;
    *(unsigned char *)(p + 0x5) = 0x16;
    *(unsigned char *)(p + 0x6) = 0;
    *(unsigned char *)(p + 0x7) = 0;
    *(unsigned char *)(p + 0x1F0) = 0xE;
    if (a1 == 0) {
        func_001749A0((int)p, 0x72, 0, 8.0f);
    }
    func_001B61C0(1, 0xEE, 0x3C, 1);
    func_001FBD50((int)p, 0x159, 0, 300.0f);
}
