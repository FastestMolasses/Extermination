// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): forward call passing a global handle as first arg.
extern int D_0024295C;
extern int func_00120A48(int a0, int a1, int a2);

int func_00120AE0(int a0, int a1) {
    return func_00120A48(D_0024295C, a0, a1);
}
