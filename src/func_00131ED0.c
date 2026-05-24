extern int **D_00275B40;

void func_00131ED0(unsigned char *a0) {
    int *v1;
    if (a0[5] == 0x63) return;
    v1 = *D_00275B40;
    *(int *)v1 = 0;
    v1 = *D_00275B40;
    *(int *)((char *)v1 + 8) = 0;
    v1 = *D_00275B40;
    *(int *)((char *)v1 + 0xC) = 0;
    v1 = *D_00275B40;
    *(int *)((char *)v1 + 0x14) = 0;
}
