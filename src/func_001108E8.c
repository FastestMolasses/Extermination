// COMPILER: eegcc
// CFLAGS: -O2

typedef int i128 __attribute__((mode(TI)));

extern unsigned char D_0026BA78[];
extern unsigned char D_00279A80[];
extern unsigned char D_00279700[];
extern unsigned char D_00279750[];

extern void func_00122B58(void *fmt, int a1);
extern void func_00121A28(void *p, int a1, int a2);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, int a7, int a8);

int func_001108E8(int a, int b, unsigned char *buf) {
    unsigned char *p;
    unsigned char *ctl;
    unsigned char *q;
    int i;
    int off;

    if (((unsigned int)buf & 0x3F) != 0) {
        func_00122B58(D_0026BA78, (int)buf);
        return 0;
    }

    p = buf;
    for (i = 0; i < 2; i++) {
        *(int *)(p + 0x58) = 0;
        p[0x70] = 5;
        p[0x71] = 2;
        p[0x67] = 0;
        func_00121A28(p, 0xFF, 0x20);
        *(int *)(p + 0x60) = 0;
        p += 0x80;
    }

    ctl = D_00279A80;
    *(int *)ctl = 1;
    *(int *)(ctl + 4) = a;
    *(int *)(ctl + 8) = b;
    *(unsigned char **)(ctl + 0x10) = buf;

    if (func_0010E8A8(D_00279700, 1, 0, ctl, 0x80, ctl, 0x80, 0, 0) < 0) {
        return 0;
    }

    off = b * 0x60 + a * 0x180;
    q = D_00279750 + off;
    *(int *)q = 1;
    *(unsigned char **)(q + 0xC) = buf;
    *(int *)(q + 0x10) = *(int *)(ctl + 0x14);
    *(i128 *)(D_00279750 + off + 0x20) = 0;
    return *(int *)(ctl + 0xC);
}
