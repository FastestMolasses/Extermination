// COMPILER: eegcc
// CFLAGS: -O2
// SDK leaf (ee-gcc 2.9): one-time init; stash args and start the helper.
extern int D_00241D68;
extern int D_0027B0C0[];
extern int D_0027B140[];
extern char D_0027C680[];
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, int a7, int a8);

int func_00114510(int a0, int a1, int a2) {
    int ret;
    if (D_00241D68 != 0) {
        return D_00241D68;
    }
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -100;
    }
    D_0027B140[0] = a0;
    D_0027B140[0x10 / 4] = a1;
    D_0027B140[0x14 / 4] = a2;
    ret = func_0010E8A8(D_0027B0C0, 4, 1, D_0027B140, 0x30, D_0027C680, 4, 0, 0);
    if (ret == 0) {
        D_00241D68 = 4;
    }
    return ret;
}
