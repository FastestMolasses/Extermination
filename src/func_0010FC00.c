// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00121A28(void *a0, int a1, int a2);
extern int D_00241C98;
extern char D_00279668[];

int func_0010FC00(void) {
    D_00241C98 = -1;
    func_00121A28(D_00279668, 0, 4);
    return 0;
}
