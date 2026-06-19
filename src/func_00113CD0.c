// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: clear D_00241D40, then return func_00113F68(0,0,0,3,&D_00241D50).
extern int D_00241D40;
extern int D_00241D50;
extern int func_00113F68(int a0, int a1, int a2, int a3, void *a4);

int func_00113CD0(void) {
    D_00241D40 = 0;
    return func_00113F68(0, 0, 0, 3, &D_00241D50);
}
