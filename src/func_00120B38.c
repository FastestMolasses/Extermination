// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: clear D_008234E0, call func_0010BE60; on failure stash error code.
extern int D_008234E0;
extern int func_0010BE60(int a0, int a1, int a2);

int func_00120B38(int *out, int a0, int a1, int a2) {
    int r;
    D_008234E0 = 0;
    r = func_0010BE60(a0, a1, a2);
    if (r == -1) {
        if (D_008234E0 != 0) {
            *out = D_008234E0;
        }
    }
    return r;
}
