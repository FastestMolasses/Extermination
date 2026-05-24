// CFLAGS: -O4,p -sdatathreshold 0
extern short D_0024DB80[];
extern void func_001FBD50(int, int, int, float);

void func_001BBD20(int a0, int a1, int a2, float f12) {
    int v0;
    int v1;
    int a3;
    f12 = 300.0f;
    v1 = (int)D_0024DB80;
    v0 = a1 << 1;
    a3 = *(short *)((char *)a0 + 0x56);
    a1 = ((a3 & 0xff00) >> 8) << 2;
    v1 = v1 + a1;
    v0 = v0 + v1;
    a1 = *(unsigned short *)v0;
    func_001FBD50(a0, a1, a2, f12);
}
