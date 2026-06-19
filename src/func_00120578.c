// COMPILER: eegcc
// CFLAGS: -O2
extern int D_008234E0;
extern int func_0010BF18(int a, int b);

int func_00120578(int *out, int a, int b) {
    int r;
    D_008234E0 = 0;
    r = func_0010BF18(a, b);
    if (r == -1) {
        if (D_008234E0 != 0) {
            *out = D_008234E0;
        }
    }
    return r;
}
