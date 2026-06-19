// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: pass &local (local[0]=1) to func_00109B20(a0, &local).
extern int func_00109B20(int a0, int *p);

int func_00109B70(int a0) {
    int local[8];
    local[0] = 1;
    return func_00109B20(a0, local);
}
