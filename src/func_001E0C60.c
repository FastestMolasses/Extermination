extern int *D_00275670;

int func_001E0C60(int a0) {
    int v1 = 1 << (a0 - 0x20);
    int *a0ptr = D_00275670;
    int v0 = *(int *)((char *)a0ptr + 0x174);
    return v0 & v1;
}
