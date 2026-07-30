// COMPILER: eegcc
// CFLAGS: -O2
typedef struct {
    int mode;
    int f4;
    int f8;
    char fC[12];
    int f18;
    int f1C;
} S279A80f;

extern S279A80f D_00279A80;
extern char D_00279700[];
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);
extern void func_00110BF8(int a, int b, int c);
extern void *memcpy(void *dst, const void *src, unsigned int n);

int func_00111318(int a0, int a1, void *a2) {
    S279A80f *d = &D_00279A80;
    d->mode = 0xB;
    d->f4 = a0;
    d->f8 = a1;
    memcpy(d->fC, a2, 12);
    if (func_0010E8A8(D_00279700, 1, 0, d, 0x80, d, 0x80, 0, 0) < 0) {
        return 0;
    }
    if (d->f1C == 1) {
        func_00110BF8(a0, a1, 2);
    }
    return d->f1C;
}
