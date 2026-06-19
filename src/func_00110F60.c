// COMPILER: eegcc
// CFLAGS: -O2
typedef struct {
    int mode;
    int f4;
    int f8;
    int fC;
    int f10;
    int f14;
} S279A80e;

extern S279A80e D_00279A80;
extern char D_00279700[];
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);
extern void func_00110BF8(int a, int b, int c);

int func_00110F60(int a0, int a1, int a2, int a3) {
    D_00279A80.fC = a2;
    D_00279A80.f10 = a3;
    D_00279A80.mode = 6;
    D_00279A80.f4 = a0;
    D_00279A80.f8 = a1;
    if (func_0010E8A8(D_00279700, 1, 0, &D_00279A80, 0x80, &D_00279A80, 0x80, 0, 0) < 0) {
        return 0;
    }
    if (D_00279A80.f14 == 1) {
        func_00110BF8(a0, a1, 2);
    }
    return D_00279A80.f14;
}
