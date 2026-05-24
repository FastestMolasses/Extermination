// CFLAGS: -O4,p -sdatathreshold 0
extern int D_00281B70[];
extern void func_0011A070(void);

void func_001FC520(int *a0) {
    int *s0 = a0;
    int v1;
    if (a0[0] == -1) return;
    func_0011A070();
    v1 = s0[0];
    D_00281B70[v1] = -1;
    s0[0] = -1;
}
