// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned char D_0027E0C0[];

int func_00119990(int a0, int a1, int a2) {
    int *p = (int *)(D_0027E0C0 + a0 * 0x78);
    int a3 = -1;
    if ((unsigned int)a0 < 0x30 && a1 >= 0 && a1 < 0x80) {
        if (a2 == 0xFFFF) {
            a3 = p[0x6C / 4];
            p[0x68 / 4] = a2;
            p[0x6C / 4] = a1;
        } else {
            p[0x70 / 4] = a2;
            a3 = 0;
            p[0x74 / 4] = a1;
        }
        p[0x64 / 4] = 1;
    }
    return a3;
}
