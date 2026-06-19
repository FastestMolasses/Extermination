// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241D68;
struct s_b0c0 {
    char pad[0x24];
    int f24;
};
extern struct s_b0c0 D_0027B0C0;
extern int D_0027B170;
extern int D_0027C680;
extern void func_00123418(int *, int, int);
extern void func_0010DFD8(int, int);
extern int func_0010E8A8(struct s_b0c0 *, int, int, int *, int, int *, int, int, int);

int func_00114AC0(int x, int a1, int a2, int a3, int t0, int t1) {
    int *p;
    int r;

    if (D_00241D68 != 0)
        return D_00241D68;
    if (D_0027B0C0.f24 == 0)
        return -0x64;

    p = &D_0027B170;
    p[0] = x;
    p[1] = a1;
    p[2] = a3;
    p[3] = t0;
    p[4] = t1;
    func_00123418(p + 5, a2, 0x3FF);
    *((char *)p + 0x413) = 0;
    func_0010DFD8(t1, t0 << 6);
    r = func_0010E8A8(&D_0027B0C0, 0xD, 1, &D_0027B170, 0x414, &D_0027C680, 4, 0, 0);
    if (r == 0)
        D_00241D68 = 0xD;
    return r;
}
