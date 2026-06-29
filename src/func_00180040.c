// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001749A0(int, int, int);

void func_00180040(unsigned char *a0) {
    if (a0[0x2F1] == 0) {
        func_001749A0((int)a0, 0x9C, 1);
    } else {
        func_001749A0((int)a0, 0x9A, 1);
    }
}
