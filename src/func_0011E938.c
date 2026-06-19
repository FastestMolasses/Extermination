// COMPILER: eegcc
// CFLAGS: -O2
// SDK: errno = 0; r = func_0010BE50(a1); if (r==-1 && errno) *a0 = errno; return r.
extern int D_008234E0;
extern int func_0010BE50(int a0);

int func_0011E938(int *a0, int a1) {
    int r;
    D_008234E0 = 0;
    r = func_0010BE50(a1);
    if (r == -1) {
        if (D_008234E0 != 0) {
            *a0 = D_008234E0;
        }
    }
    return r;
}
