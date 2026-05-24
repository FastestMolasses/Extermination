// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275670;
extern int D_00275674;

void func_001D1FF0(int a0, int a1) {
    int v1;
    int a2;
    int a3;
    int *v1p;
    int t0;

    v1 = D_00275670;
    a2 = a0 << 2;
    t0 = a1 << 6;
    a3 = D_00275674;
    a2 = v1 + a2;
    a3 = a3 + t0;
    a3 = a3 + 0x4A0;
    v1p = *(int **)(a2 + 0x10);
    *(char *)((char *)v1p + 3) = 0x30;
    v1p = *(int **)(a2 + 0x10);
    *(int *)((char *)v1p + 4) = a3;
    v1p = *(int **)(a2 + 0x10);
    *(short *)v1p = 4;
    v1p = *(int **)(a2 + 0x10);
    v1p = (int *)((char *)v1p + 0x10);
    *(int *)(a2 + 0x10) = (int)v1p;
}
