// CFLAGS: -O4,p -sdatathreshold 0
extern int D_00277224;
extern int D_0027722C;

void func_0010DCA8(int a0, int a1, int a2) {
    int base;
    int *p;
    if (a0 < 0) {
        base = D_00277224;
    } else {
        base = D_0027722C;
    }
    p = (int *)(a0 * 8 + base);
    p[0] = a1;
    p[1] = a2;
}
