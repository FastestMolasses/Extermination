// COMPILER: eegcc
// CFLAGS: -O2
extern unsigned char D_0027E0C0[];
extern unsigned char D_0027C6C0[];
extern unsigned char *D_00281AC0;

int func_00119DE0(unsigned int a0, int a1) {
    unsigned char *a2 = &D_0027E0C0[a0 * 0x78];
    int a3 = -1;
    if (a0 < 0x30 && a1 >= 0 && a1 < 0x80 &&
        *(unsigned short *)(a2 + 0x2E) == 1 &&
        *(unsigned short *)(a2 + 0x32) == 0) {
        unsigned char *p = D_0027C6C0 - (-(*(unsigned short *)(a2 + 0x24) * 0xC));
        unsigned char *base = *(unsigned char **)(p + 4);
        int off = *(int *)(base + 0x10);
        unsigned char *t0 = base + off;
        if (*(unsigned short *)(t0) + 1 != 0) {
            int i = 0;
            do {
                unsigned char *q = base + *(int *)(base + 0x10);
                q = q + *(unsigned short *)(t0 + i * 2 + 2);
                D_00281AC0 = q;
                *(signed char *)(q + 2) = (signed char)a1;
                i++;
            } while (i < (int)(*(unsigned short *)(t0) + 1));
        }
        a3 = 0;
    }
    return a3;
}
