// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241D68;
extern int D_0027B0C0[];
extern int D_0027B140[];
extern int D_0027B5C0[];
extern int D_0027C680[];
extern int D_0027B0E8;
extern int D_0027B0EC;
extern int D_0027B0F0;
extern void func_00114930(int a0);
extern void func_0010DFD8(void *a0, int a1);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int a4, void *a5,
                         int a6, void *a7, void *a8);

int func_00114988(int a0, int a1, int a2, int a3, int t0) {
    int r;

    if (D_00241D68 != 0) {
        return D_00241D68;
    }
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }
    D_0027B140[1] = a0;
    D_0027B140[2] = a1;
    D_0027B140[7] = (int)D_0027B5C0;
    if (a2) {
        D_0027B140[5] = 1;
    } else {
        D_0027B140[5] = 0;
    }
    if (a3) {
        D_0027B140[4] = 1;
    } else {
        D_0027B140[4] = 0;
    }
    if (t0) {
        D_0027B140[3] = 1;
    } else {
        D_0027B140[3] = 0;
    }
    D_0027B0E8 = a2;
    D_0027B0EC = a3;
    D_0027B0F0 = t0;
    func_0010DFD8(D_0027B5C0, 0xC0);
    r = func_0010E8A8(D_0027B0C0, 1, 1, D_0027B140, 0x30, D_0027C680, 4,
                      func_00114930, D_0027B5C0);
    if (r == 0) {
        D_00241D68 = 1;
    }
    return r;
}
