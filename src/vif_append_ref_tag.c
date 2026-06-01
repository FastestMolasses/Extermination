// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275670;
extern int D_00275674;

void vif_append_ref_tag(int a0, int a1) {
    int v1;
    int t0;
    int t1;
    int t2;
    int *v1p;

    v1 = D_00275670;
    t2 = a0 << 2;
    t1 = D_00275674;
    t0 = v1 + t2;

    v1p = *(int **)(t0 + 0x10);
    *(char *)((char *)v1p + 3) = 0x30;
    v1p = *(int **)(t0 + 0x10);
    *(int *)((char *)v1p + 4) = t1;
    v1p = *(int **)(t0 + 0x10);
    *(short *)v1p = 1;
    v1p = *(int **)(t0 + 0x10);
    v1p = (int *)((char *)v1p + 0x10);
    *(int *)(t0 + 0x10) = (int)v1p;

    *(int *)(t0 + 0x50) = a1;

    v1 = D_00275670;
    t0 = v1 + t2;
    v1p = *(int **)(t0 + 0x10);
    *(char *)((char *)v1p + 3) = 0x50;
    v1p = *(int **)(t0 + 0x10);
    *(int *)((char *)v1p + 4) = a1;
    v1p = *(int **)(t0 + 0x10);
    *(short *)v1p = 0;
    v1p = *(int **)(t0 + 0x10);
    v1p = (int *)((char *)v1p + 0x10);
    *(int *)(t0 + 0x10) = (int)v1p;
}
