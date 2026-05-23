extern int *D_00275670;

void func_001D2160(char *a0) {
    int *a1 = D_00275670;
    a0[3] = 0x20;
    *(int *)(a0 + 4) = *(int *)((char *)a1 + 8);
    *(short *)a0 = 0;
}
