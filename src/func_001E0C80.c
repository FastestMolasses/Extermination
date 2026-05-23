extern int *D_00275670;

int func_001E0C80(int a0, int a1) {
    int v1;
    int *a0ptr;
    int v0;
    v1 = a0 - 0x20;
    a0ptr = D_00275670;
    v0 = 1 << v1;
    v1 = *(int *)((char *)a0ptr + 0x174);
    if (a1) {
        a0 = v1 & v0;
        v1 = v1 | v0;
    } else {
        v0 = ~v0;
        v1 = v1 & v0;
        a0 = 0;
    }
    *(int *)((char *)a0ptr + 0x174) = v1;
    return a0 != 0;
}
