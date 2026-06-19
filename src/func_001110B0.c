// COMPILER: eegcc
// CFLAGS: -O2
typedef struct {
    int mode;
    int f4;
    int f8;
    unsigned char fC[6];
    short _padE;
    int f14;
} S279A80b;
extern S279A80b D_00279A80;
extern char D_00279700[];
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);
extern void func_00110BF8(int a, int b, int c);

int func_001110B0(int a0, int a1, unsigned char *a2) {
    S279A80b *d = &D_00279A80;
    int i;
    d->mode = 8;
    d->f4 = a0;
    d->f8 = a1;
    for (i = 0; i < 6; i++) {
        d->fC[i] = a2[i];
    }
    if (func_0010E8A8(D_00279700, 1, 0, d, 0x80, d, 0x80, 0, 0) < 0) {
        return 0;
    }
    if (d->f14 == 1) {
        func_00110BF8(a0, a1, 2);
    }
    return d->f14;
}
