// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_001FE480(int *a0, int a1) {
    int v0 = a1 << 4;
    char *a1p;
    char *a1q;
    int v1;
    int *a0p;
    int w;
    a1p = (char *)a0 + a0[0];
    a1q = a1p + a0[2];
    v1 = *(int *)a1q;
    a0p = (int *)(a1q + 0x10 + v0);
    w = a0p[0];
    v1 = (int)(a1q + v1);
    return w + v1;
}
