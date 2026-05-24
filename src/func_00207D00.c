// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275670;
extern int D_00275674;

void func_00207D00(int a0, int a1) {
    int v1;
    int a2;
    int a3;
    int *v1p;

    if (a1 == 3) goto L_3;
    if (a1 == 2) goto L_2;
    if (a1 == 1) goto L_1;
    v1 = D_00275674;
    a3 = v1 + 0x720;
    goto L_done;
L_1:
    v1 = D_00275674;
    a3 = v1 + 0x7A0;
    goto L_done;
L_2:
    v1 = D_00275674;
    a3 = v1 + 0x820;
    goto L_done;
L_3:
    v1 = D_00275674;
    a3 = v1 + 0x6A0;
L_done:
    v1 = D_00275670;
    a2 = a0 << 2;
    a2 = v1 + a2;
    v1p = *(int **)(a2 + 0x10);
    *(char *)((char *)v1p + 3) = 0x30;
    v1p = *(int **)(a2 + 0x10);
    *(int *)((char *)v1p + 4) = a3;
    v1p = *(int **)(a2 + 0x10);
    *(short *)v1p = 8;
    v1p = *(int **)(a2 + 0x10);
    v1p = (int *)((char *)v1p + 0x10);
    *(int *)(a2 + 0x10) = (int)v1p;
}
