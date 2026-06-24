// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Subsystem init. Resets a byte flag (D_008105CB=-1) and an int (D_00282160=-1),
// memset-style inits two 0xC0-byte tables (D_00281C30, D_00281B70) via
// func_00121A28, then walks the int table D_00281D50 for (signed char)D_0028215A
// entries: for each handle, if func_00119D38(h)&3 it calls func_00119AA0(h,1).
// Finally func_00119828(0,...) / (1,...) and clears a 10-element array of 0x10-
// byte records at D_00281F30 (each: int 0 then int -1).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): the lone 991202 residual was the
// idiom-13 clean-store delay-slot nop. NOTE the declaration order (counter i
// before pointer p) is load-bearing: it makes mwcc assign counter->$s0,
// pointer->$s1 to match the target. objdiff 100% vs build/expected.
extern void func_00119828(int, int, int);
extern void func_00119AA0(int, int);
extern int func_00119D38(int);
extern void func_0011A198(int);
extern void func_00121A28(void *, int, int);
extern int D_00281B70;
extern int D_00281C30;
extern int D_00281D50[];
extern int D_00281F30[];
extern signed char D_0028215A;
extern int D_00282160;
extern signed char D_008105CB;

void func_001FBC50(void) {
    int i;
    int *p;

    func_0011A198(1);
    D_008105CB = -1;
    D_00282160 = -1;
    func_00121A28(&D_00281C30, -1, 0xC0);
    func_00121A28(&D_00281B70, -1, 0xC0);
    p = &D_00281D50[0];
    for (i = 0; i < D_0028215A; i++) {
        if (func_00119D38(*p) & 3) {
            func_00119AA0(*p, 1);
        }
        p += 1;
    }
    func_00119828(0, 0x1999, 0x1999);
    func_00119828(1, 0x1999, 0x1999);
    p = &D_00281F30[0];
    i = 0;
    do {
        i += 1;
        p[0] = 0;
        p[1] = -1;
        p += 4;
    } while (i < 0xA);
}
