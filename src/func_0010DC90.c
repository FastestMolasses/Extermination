extern char D_00277218[];

int func_0010DC90(int a0, int a1) {
    int *v1 = (int *)D_00277218;
    int v0 = *(int *)((char *)v1 + 0xC);
    *(int *)((char *)v1 + 0x10) = a1;
    *(int *)((char *)v1 + 0xC) = a0;
    return v0;
}
