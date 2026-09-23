// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// MATCH NOTE (m3-matching lane): 70.25% under mwcc 991202 -> 100%;
// mwcc 2.3.3 reproduces the target scheduling. func_00122BB8 takes no argument (it is the
// LCG step at 0x00122BB8), so the stray (a0) argument was dropped.

extern int func_00122BB8(void);
extern unsigned char D_00242ED0[];

int func_00128600(int a0) {
    int r;
    unsigned char *base;
    r = func_00122BB8() & 0xF;
    base = D_00242ED0 + a0 * 16;
    return *(base + r);
}
