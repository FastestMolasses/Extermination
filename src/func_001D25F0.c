extern int *D_00275670;

void func_001D25F0(float fa0) {
    int *v1 = D_00275670;
    *(volatile float *)0x70003B60 = fa0;
    *(float *)((char *)v1 + 0x2468) = fa0;
}
