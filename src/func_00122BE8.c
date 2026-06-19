// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): libkernel call; on -1 with a pending code, report it.
extern int D_008234E0;
extern int sub__9PsIIlibkernl2000_2(int a0, int a1, int a2);

int func_00122BE8(int *out, int a1, int a2, int a3) {
    int ret;
    D_008234E0 = 0;
    ret = sub__9PsIIlibkernl2000_2(a1, a2, a3);
    if (ret == -1) {
        if (D_008234E0 != 0) {
            *out = D_008234E0;
        }
    }
    return ret;
}
