// COMPILER: eegcc
// CFLAGS: -O2
extern int func_00105370(void);
extern void func_00106070(int *p, int a, int b, int c, int d);
extern int func_00106540(int v);
extern int func_00106948(int h);

void func_00106278(int *p, int *out, int a2, int a3, int t0, int t1, int t2) {
    int s1;
    int n;

    s1 = func_00106540(2);
    if (a2 != 0 && s1 != 0)
        n = func_00106948(a2);
    else
        n = 0;
    func_00106070(p, a2, s1, n, t2);
    if (t0)
        out[0] = func_00105370();

    s1 = func_00106540(2);
    if (a3 != 0 && s1 != 0)
        n = func_00106948(a3);
    else
        n = 0;
    if (t1)
        p[1] = p[1] >> 1;
    func_00106070(p + 1, a3, s1, n, t2);
    if (t1)
        p[1] = p[1] << 1;
    if (t0)
        out[1] = func_00105370();
}
