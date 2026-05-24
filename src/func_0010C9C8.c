extern char D_00276D00[];

int *func_0010C9C8(int a0) {
    int *v1 = (int *)D_00276D00;
    *(int *)v1 = a0;
    a0 = (int)((char *)v1 + 0x10);
    *(int **)((char *)v1 + 0x8) = (int *)a0;
    *(int *)((char *)v1 + 0x4) = 0;
    *(int **)((char *)v1 + 0xC) = (int *)a0;
    return v1;
}
