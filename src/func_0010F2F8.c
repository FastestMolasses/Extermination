// COMPILER: eegcc
// CFLAGS: -O2
extern int func_0010EFA8(void);
extern int func_0010E8A8(void *a0, int a1, int a2, void *a3, int t0, void *t1, int t2, int t3, int s9);
extern int D_00241C88;
extern int D_00278C00;
extern int D_00279040;
extern int D_00279210;

int func_0010F2F8(void) {
    int *p;
    int *q;

    q = &D_00278C00;
    p = (int *)func_0010EFA8();
    if (D_00241C88 == 0) {
        return -1;
    }
    if (p == 0) {
        return -9;
    }
    if (p[1] == 0) {
        return -9;
    }
    *q = p[0];
    if (func_0010E8A8(&D_00279210, 1, 0, q, 4, &D_00279040, 4, 0, 0) < 0) {
        return -1;
    }
    p[1] = 0;
    return D_00279040;
}
