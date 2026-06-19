// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241CF8;
extern char D_0026BE90[];
extern char D_00241D50[];
extern void func_00122B58(void *p);
extern int func_00113F68(int a, int b, int c, int d, void *e);

int func_00113F20(void) {
    if (D_00241CF8 > 0) {
        func_00122B58(D_0026BE90);
    }
    return func_00113F68(0, 0, 0, 6, D_00241D50);
}
