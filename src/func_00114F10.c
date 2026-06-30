// COMPILER: eegcc
// CFLAGS: -O2
struct blk { char b[0x40]; };

extern int D_00241D68;
extern int D_0027B0C0[];
extern struct blk D_0027B100;
extern int D_0027B170[];
extern char D_0027C680[];
extern void func_00123418(void *dst, void *src, int len);
extern int DisableDmacHandler(int id);
extern int func_0010E8A8(void *a, int b, int c, void *d, int e, void *f, int g, int h, int i);

int func_00114F10(int a0, int a1, int a2, struct blk *a3, int a4) {
    int r;
    if (D_00241D68 != 0) {
        return D_00241D68;
    }
    if (D_0027B0C0[0x24 / 4] == 0) {
        return -0x64;
    }
    D_0027B170[0] = a0;
    D_0027B170[1] = a1;
    a4 &= 7;
    D_0027B170[2] = a4;
    D_0027B100 = *a3;
    D_0027B170[4] = (int)&D_0027B100;
    func_00123418((char *)D_0027B170 + 0x14, (void *)a2, 0x3FF);
    *((char *)D_0027B170 + 0x413) = 0;
    DisableDmacHandler(0);
    r = func_0010E8A8(D_0027B0C0, 0xE, 1, D_0027B170, 0x414, D_0027C680, 4, 0, 0);
    if (r == 0) {
        D_00241D68 = 0xE;
    }
    return r;
}
