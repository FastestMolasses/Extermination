// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: return func_00113F68(a0, 0, 0, 4, &D_00241D50) using EABI 5th reg arg.
extern int D_00241D50;
extern int func_00113F68(int a0, int a1, int a2, int a3, void *a4);

int func_00113CA0(int a0) {
    return func_00113F68(a0, 0, 0, 4, &D_00241D50);
}
