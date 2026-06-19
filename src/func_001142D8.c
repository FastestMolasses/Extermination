// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241D68;
extern int D_0027B0C0[];
extern int D_0027B140[];
extern volatile int D_0027C680[];
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);

int func_001142D8(int a0) {
    int r;
    if (D_00241D68 != 0) {
        return -0x64 - D_00241D68;
    }
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }
    D_0027B140[1] = a0;
    r = func_0010E8A8(D_0027B0C0, 0x15, 0, D_0027B140, 0x30, (void*)D_0027C680, 4, 0, 0);
    if (r == 0) {
        r = D_0027C680[0];
    }
    return r;
}
