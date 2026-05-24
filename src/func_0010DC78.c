extern char D_00277218[];

int func_0010DC78(int a0, int a1) {
    int *v1 = (int *)D_00277218;
    int v0 = *(int *)((char *)v1 + 0x14);
    *(int *)((char *)v1 + 0x18) = a1;
    *(int *)((char *)v1 + 0x14) = a0;
    return v0;
}
