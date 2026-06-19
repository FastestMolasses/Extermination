// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241D68;
extern int D_0027B0C0[];
extern int D_0027B140[];
extern volatile int D_0027C680[];
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);

int func_00115168(int a0, int a1) {
    int r;
    if (D_00241D68 != 0) {
        return D_00241D68;
    }
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }
    D_0027B140[1] = a0;
    D_0027B140[2] = a1;
    r = func_0010E8A8(D_0027B0C0, 0x11, 1, D_0027B140, 0x30, (void *)D_0027C680, 4, 0, 0);
    if (r == 0) {
        D_00241D68 = 0x11;
    }
    return r;
}
