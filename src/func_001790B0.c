// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001026A0(int, int, int);
extern int func_0019AD00(int, int, int);
extern int D_00248970;
extern int D_700038A0;

int func_001790B0(int a0) {
    int s0;
    unsigned char *s2 = (unsigned char *)a0;
    int *s1 = &D_00248970;
    s2[0x314] = 0;
    s0 = 0;
    while (s0 < 7) {
        func_001026A0((int)&D_700038A0, a0 + 0xD0, (int)s1);
        if (func_0019AD00(a0, (int)&D_700038A0, 0x80000007)) {
            s2[0x314] = s2[0x314] | (1 << s0);
            s0++;
        }
        s0++;
        s1 = (int *)((char *)s1 + 0x10);
    }
    return s2[0x314];
}
