// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001749A0(int, int, int);

void func_001800C0(unsigned char *a0) {
    if (a0[0x2F1] == 0) {
        func_001749A0((int)a0, 0x9E, 1);
    } else {
        func_001749A0((int)a0, 0xA0, 1);
    }
}
