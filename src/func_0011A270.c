// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned char D_0027E0C0[];

void func_0011A270(int a0, int a1) {
    short *p = (short *)(D_0027E0C0 + a0 * 0x78);
    if ((unsigned int)a0 < 0x30 && a1 >= 0 && a1 < 0x4000) {
        p[0x44 / 2] = a1;
        p[0x50 / 2] = 1;
    }
}
