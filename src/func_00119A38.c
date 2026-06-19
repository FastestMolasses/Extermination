// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): if slot armed and its type is ready, latch and reset.
extern char D_0027E0C0[];
extern char D_0027C6C0[];

void func_00119A38(unsigned int a0) {
    char *e = &D_0027E0C0[a0 * 0x78];
    if (a0 < 0x30) {
        unsigned short st = *(unsigned short *)(e + 0x2E);
        if (st == 1) {
            unsigned short ty = *(unsigned short *)(e + 0x24);
            int v = *(int *)&D_0027C6C0[ty * 0xC];
            if (v == st) {
                *(short *)(e + 0x30) = v;
                *(short *)(e + 0x3E) = 0;
                *(short *)(e + 0x40) = 0;
                *(int *)(e + 0x10) = 0;
            }
        }
    }
}
