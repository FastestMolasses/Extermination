// COMPILER: eegcc
// CFLAGS: -O2
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int t0, void *t1, int t2, int t3, int t4);
extern int D_00241C90;
extern int D_002792C0;
extern char D_00279240[];
extern int D_00279280[];

int func_0010F8F8(int a0) {
    if (D_00241C90 < 0) {
        return 0;
    }
    D_002792C0 = a0;
    if (func_0010E8A8(D_00279240, 1, 0, &D_002792C0, 4, D_00279280, 4, 0, 0) >= 0) {
        return D_00279280[0];
    }
    return 0;
}
