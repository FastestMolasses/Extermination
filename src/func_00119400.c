// CFLAGS: -O4,p -sdatathreshold 0
extern int D_0027F740[0x50];
extern void func_001157F0(int, int, int, int);

void func_00119400(int a0, int a1, int a2, int a3) {
    int *t0 = D_0027F740;
    int v0 = t0[0x12];
    int v1 = 0x00ffffff;
    int t1 = ((unsigned int)a2 >> 8) & 0xffff;
    int t2 = ((unsigned int)a1 >> 16) & 0xff;
    v0 = v0 + 1;
    t0[0x12] = v0;
    a3 = a3 & v1;
    a2 = a2 << 24;
    a1 = a1 << 16;
    a3 = a2 | a3;
    v0 = t0[0x12];
    a2 = a1 | t1;
    v0 = v0 << 8;
    func_001157F0(a0, v0 | t2, a2, a3);
}
