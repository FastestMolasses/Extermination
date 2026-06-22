// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001749A0(unsigned char *, int, int);

void func_0017E150(unsigned char *a0, int a1) {
    if (a1 == 0) {
        if (a0[0x315] == 0) {
            func_001749A0(a0, 0x88, 0);
        } else {
            func_001749A0(a0, 0xD2, 0);
        }
    } else {
        if (a0[0x315] == 0) {
            func_001749A0(a0, 0x89, 0);
        } else {
            func_001749A0(a0, 0xD3, 0);
        }
    }
}
