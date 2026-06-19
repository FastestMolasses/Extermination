// COMPILER: eegcc
// CFLAGS: -O2
// SDK: configure D_00279A80 (type 0x13, args a0/a1) and create via func_0010E8A8;
// on success return its handle (D_00279A80.fC), else 0.
struct cfg {
    int f0;     // 0x0
    int f4;     // 0x4
    int f8;     // 0x8
    int fC;     // 0xC
    int f10;    // 0x10
    int f14;    // 0x14
};
extern struct cfg D_00279A80;
extern int D_00279700;
extern int func_0010E8A8(int a0, int a1, int a2, struct cfg *a3,
                         int t0, struct cfg *t1, int t2, int t3, int s0);

int func_00111520(int a0, int a1) {
    int r;
    D_00279A80.f10 = a0;
    D_00279A80.f14 = a1;
    D_00279A80.f0 = 0x13;
    r = func_0010E8A8((int)&D_00279700, 1, 0, &D_00279A80,
                      0x80, &D_00279A80, 0x80, 0, 0);
    if (r < 0) {
        return 0;
    }
    return D_00279A80.fC;
}
