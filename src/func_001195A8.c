// COMPILER: eegcc
// CFLAGS: -O2
extern char D_0027C6C0[];
extern char D_0027CCC0[];
extern void func_00121A28(void *entry, int a1, int a2);

int func_001195A8(unsigned int a0) {
    int ret = -1;
    if (a0 < 0x80) {
        if (*(int *)(&D_0027C6C0[a0 * 12]) == 1) {
            char *p = D_0027CCC0;
            char *end = &D_0027CCC0[0x13E0];
            do {
                if (*(unsigned short *)(p + 0) == 1 &&
                    *(unsigned short *)(p + 0x22) == a0) {
                    return -1;
                }
                p += 0x6A;
            } while ((int)p < (int)end);
            func_00121A28(&D_0027C6C0[a0 * 12], 0, 12);
            ret = 0;
        }
    }
    return ret;
}
