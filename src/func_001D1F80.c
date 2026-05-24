// CFLAGS: -O4,p -sdatathreshold 4
extern int D_00275670;
extern int D_00275674;

void func_001D1F80(int a0, int a1, int a2) {
    int v1;
    int a1b;
    int a2b;
    int a3;
    int t0;
    int *v1p;

    /* compute a1*45*32 */
    v1 = a1 << 4;
    a1b = v1 - a1;
    v1 = a1b << 2;
    v1 = v1 - a1b;
    t0 = v1 << 5;

    /* load D_00275674, compute a2*9 */
    a3 = D_00275674;
    v1 = a2 << 3;
    a1b = v1 + a2;

    /* load D_00275670 into v1 */
    v1 = D_00275670;

    /* finalize a3 = D_00275674 + a1*1440 + a2*144 + 0xBA0 */
    a2b = a1b << 4;
    a3 = a3 + t0;
    a3 = a3 + a2b;

    /* compute channel ptr */
    a0 = a0 << 2;
    a2b = v1 + a0;

    /* push command */
    v1p = *(int **)(a2b + 0x10);
    a3 = a3 + 0xBA0;
    *(char *)((char *)v1p + 3) = 0x30;
    v1p = *(int **)(a2b + 0x10);
    *(int *)((char *)v1p + 4) = a3;
    v1p = *(int **)(a2b + 0x10);
    *(short *)v1p = 9;
    v1p = *(int **)(a2b + 0x10);
    v1p = (int *)((char *)v1p + 0x10);
    *(int *)(a2b + 0x10) = (int)v1p;
}
