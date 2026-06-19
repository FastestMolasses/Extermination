// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): read a sub-slot status byte; 0xFF means "none".
extern char D_0027E0C0[];

int func_00119D80(unsigned int a0, int a1, int a2) {
    char *e = &D_0027E0C0[a0 * 0x78];
    int ret = -1;
    if (a0 < 0x30 && a1 >= 0 && a1 < 0x10) {
        if (*(unsigned short *)(e + 0x2E) == 1 && a2 == 0) {
            unsigned char b = *(unsigned char *)(*(int *)(e + 0xC) + a1 * 0x10 + 0x12);
            if (b != 0xFF) {
                ret = b;
            }
        }
    }
    return ret;
}
