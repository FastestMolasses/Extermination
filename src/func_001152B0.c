// CFLAGS: -O4,p -sdatathreshold 0
extern int D_0027F740;
extern void func_001152D8(void);

void func_001152B0(void) {
    int v1 = (&D_0027F740)[0x11];
    if (v1 != 0) {
        func_001152D8();
    }
}
