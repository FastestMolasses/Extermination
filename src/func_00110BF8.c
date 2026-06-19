// COMPILER: eegcc
// CFLAGS: -O2
// SDK: p = func_00110AB8(a0, a1); p[0x71] = a2; return 1.
extern unsigned char *func_00110AB8(int a0, int a1);

int func_00110BF8(int a0, int a1, int a2) {
    unsigned char *p = func_00110AB8(a0, a1);
    p[0x71] = (unsigned char)a2;
    return 1;
}
