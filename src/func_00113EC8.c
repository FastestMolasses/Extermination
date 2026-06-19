// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241CF8;
extern int D_00241D40;
extern char D_0026BE78[];
extern char D_00241D50[];
extern void func_00122B58(void *p);
extern int func_00113F68(int a, int b, int c, int d, void *e);

int func_00113EC8(void) {
    D_00241D40 = 1;
    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BE78);
    }
    return func_00113F68(0, 0, 0, 8, D_00241D50);
}
