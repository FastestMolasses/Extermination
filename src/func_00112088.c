// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00279B80[];
extern char D_00279E00[];
extern volatile int D_00241D08;
extern int func_00111F18(int a);
extern int func_0010E8A8(void *a, int b, int c, int d, int e, void *f, int g, int h, int i);
extern int CreateSema(int a);

int func_00112088(void) {
    int *s0;
    if (func_00111F18(2) == 0) {
        return 0;
    }
    s0 = D_00279B80;
    if (func_0010E8A8(D_00279E00, 0xE, 0, 0, 0, s0, 4, 0, 0) < 0) {
        CreateSema(D_00241D08);
        return 0;
    }
    CreateSema(D_00241D08);
    return *(int *)((int)s0 | 0x20000000);
}
