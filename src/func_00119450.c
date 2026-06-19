// COMPILER: eegcc
// CFLAGS: -O2
struct S1 { char _pad[0x1C0]; volatile int f1C0; };
struct S2 { char _pad[0x48]; volatile int f48; };
extern struct S1 D_002817C0;
extern struct S2 D_0027F740;
int func_00119450(int a0) {
    if (a0 != 0) {
        if (a0 != 1) goto ret_neg;
        while (D_002817C0.f1C0 != D_0027F740.f48) {
            ;
        }
        return 1;
    }
    return D_002817C0.f1C0 == D_0027F740.f48;
ret_neg:
    return -1;
}
