// COMPILER: eegcc
// CFLAGS: -O2
// SDK wrapper: clear D_00241C88, then func_00121A28(&D_00279238, 0, 4); return 0.
extern int D_00241C88;
extern int D_00279238;
extern void func_00121A28(void *a0, int a1, int a2);

int func_0010F140(void) {
    D_00241C88 = 0;
    func_00121A28(&D_00279238, 0, 4);
    return 0;
}
