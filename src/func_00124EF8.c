// COMPILER: eegcc
// CFLAGS: -O2
extern int D_008234E0;
extern int sub__9PsIIlibkernl2000(int a, int b, int c);

int func_00124EF8(int *out, int a, int b, int c) {
    int r;
    D_008234E0 = 0;
    r = sub__9PsIIlibkernl2000(a, b, c);
    if (r == -1) {
        if (D_008234E0 != 0) {
            *out = D_008234E0;
        }
    }
    return r;
}
