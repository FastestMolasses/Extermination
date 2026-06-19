// COMPILER: eegcc
// CFLAGS: -O2
// SDK init: stash func_00106948(5); if func_00106948(1), set up D_00241BC0 and
// start helpers; else clear D_00241BC0. Returns 0.
extern int func_00106948(int a0);
extern int func_00106830(int a0);
extern int func_00107060(void);
extern int D_00241540;
extern int D_00241BC0;

int func_00106B18(void) {
    int *p = &D_00241540;
    *p = func_00106948(5);
    if (func_00106948(1)) {
        int *q = &D_00241BC0;
        *q = func_00106948(1);
        func_00106830(7);
        func_00107060();
    } else {
        D_00241BC0 = 0;
    }
    return 0;
}
