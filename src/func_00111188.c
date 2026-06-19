// COMPILER: eegcc
// CFLAGS: -O2
typedef struct {
    int mode;
    int f4;
    int f8;
    int fC;
} S279A80;

extern S279A80 D_00279A80;
extern char D_00279700[];
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);

int func_00111188(int a0, int a1) {
    D_00279A80.f4 = a0;
    D_00279A80.f8 = a1;
    D_00279A80.mode = 9;
    if (func_0010E8A8(D_00279700, 1, 0, &D_00279A80, 0x80, &D_00279A80, 0x80, 0, 0) < 0) {
        return 0;
    }
    return D_00279A80.fC;
}
