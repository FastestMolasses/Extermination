// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241D40;
extern int func_00113F68(int a0, int a1, int a2, int a3, void *a4);

int func_00113C68(int a0, void *a1) {
    D_00241D40 = 1;
    return func_00113F68(a0, 0, 0, 1, a1);
}
