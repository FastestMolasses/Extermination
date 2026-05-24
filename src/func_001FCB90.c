// CFLAGS: -O4,p -sdatathreshold 0
extern int *D_0028A498;
extern void func_001FE070(int, int, int, int);

void func_001FCB90(int a0, int a1, int a2, int a3) {
    int *t0;
    int v0;
    int v1;
    int a4;

    t0 = D_0028A498;
    v0 = a2 << 4;
    v1 = a1;
    a2 = a0;
    a1 = a3;
    a3 = v1;
    a4 = (int)t0 + 16;
    v1 = t0[0];
    v0 = *(int *)(a4 + v0);
    v1 = (int)t0 + v1;
    func_001FE070(v1 + v0, a1, a2, a3);
}
