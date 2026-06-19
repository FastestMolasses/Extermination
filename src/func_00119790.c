// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): if slot is armed and idle, (re)issue it; return 0/-1.
extern char D_0027E0C0[];
extern void func_00121A28(void *entry, int a1, int a2);

int func_00119790(unsigned int a0) {
    char *e = &D_0027E0C0[a0 * 0x78];
    int ret = -1;
    if (a0 < 0x30) {
        if (*(unsigned short *)(e + 0x2E) == 1) {
            if (*(unsigned short *)(e + 0x30) == 0) {
                func_00121A28(e, 0, 0x78);
                ret = 0;
            }
        }
    }
    return ret;
}
