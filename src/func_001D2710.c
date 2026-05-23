extern int *D_00275670;

int func_001D2710(int a0) {
    int bit = 1 << a0;
    return *(int *)((char *)D_00275670 + 0xC) & bit;
}
