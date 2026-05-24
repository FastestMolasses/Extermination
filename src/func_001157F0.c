// CFLAGS: -O4,p -sdatathreshold 0
extern int D_0027F740[256];
extern int D_0027F7C0;

int func_001157F0(int a0, int a1, int a2, int a3) {
    int *t2;
    int t0;
    int v0;
    int v1;
    int t1;
    t2 = D_0027F740;
    t1 = (int)&D_0027F7C0;
    t0 = t2[15];
    v0 = t2[16];
    v1 = t2[16];
    t0 = t0 << 12;
    v0 = (v0 << 4) + t1;
    v1 = (v1 < 255) ? 1 : 0;
    t0 = t0 + v0;
    if (!v1) {
        return -1;
    }
    v0 = t2[16];
    *(int *)(t0 + 12) = a3;
    v0++;
    *(int *)t0 = a0;
    t2[16] = v0;
    *(int *)(t0 + 4) = a1;
    v0 = t2[16];
    *(int *)(t0 + 8) = a2;
    return v0;
}
