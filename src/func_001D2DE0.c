extern char *D_00275670;

void func_001D2DE0(int a0, int a1) {
    char *v1 = (char *)(a0 << 2);
    v1 += (int)D_00275670;
    *(int *)(v1 + 0x2520) = a1;
}
