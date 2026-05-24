// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001FE6B0(int, int, int, int, int);
extern int *D_0028A4A0;
extern int D_002739B0;

void func_00229640(int a0) {
    int v0 = a0;
    int *t0 = D_0028A4A0;
    int a1 = *(int *)(v0 + 0x48);
    int a2 = *(int *)(v0 + 0x4C);
    int t1 = t0[0x18 / 4];
    (void)t1;
    int v1 = (int)t0 + t0[0];
    int arg5 = v1 + t0[0x10 / 4];
    func_001FE6B0(v0 + 0x64, a1, a2, (int)&D_002739B0, arg5);
}
