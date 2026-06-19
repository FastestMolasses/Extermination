// COMPILER: eegcc
// CFLAGS: -O2
// SDK: errno = 0; r = func_0010BE68(a1); if (r==0xFFFFFFFF && errno) *a0 = errno; return r.
extern int D_008234E0;
extern unsigned int func_0010BE68(int a0);

unsigned int func_00122C48(int *a0, int a1) {
    unsigned int r;
    D_008234E0 = 0;
    r = func_0010BE68(a1);
    if (r == 0xFFFFFFFF) {
        if (D_008234E0 != 0) {
            *a0 = D_008234E0;
        }
    }
    return r;
}
