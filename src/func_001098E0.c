// CFLAGS: -O4,p -sdatathreshold 0
extern int D_002412F8;
extern int D_00241400;
extern void func_001026A0(int, int, int, int, int);

void func_001098E0(int a0, int a1, int a2, int a3, int t0) {
    int v0;
    int v1;

    v1 = a2 + 19;
    v0 = (-1 < v1) ? 1 : 0;
    a2 = a2 + 34;
    if (v0) a2 = v1;
    a2 = (a2 >> 4) << 4;
    D_002412F8 = a1;
    D_00241400 = a2;
    func_001026A0(a1, a2, a3, t0, 0);
}
