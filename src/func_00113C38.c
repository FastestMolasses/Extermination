// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00113F68(int a0, int a1, int a2, int a3, void *a4);
extern int D_00241D40;
extern char D_00241D50[];

int func_00113C38(int a0, int a1, int a2) {
    D_00241D40 = 0;
    return func_00113F68(a0, a1, a2, 5, D_00241D50);
}
