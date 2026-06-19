// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241D68;
extern int D_0027B0C0[];
extern int D_0027B170[];
extern char D_0027C680[];
extern void func_00123418(void *p, int v, int n);
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);

int func_001151F8(int a0, int a1, int a2) {
    int r;
    if (D_00241D68 != 0) {
        return D_00241D68;
    }
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }
    D_0027B170[0] = a0;
    D_0027B170[1] = a1;
    func_00123418((char *)D_0027B170 + 0x14, a2, 0x3FF);
    *((char *)D_0027B170 + 0x413) = 0;
    r = func_0010E8A8(D_0027B0C0, 0x12, 1, D_0027B170, 0x414, D_0027C680, 4, 0, 0);
    if (r == 0) {
        D_00241D68 = 0x12;
    }
    return r;
}
