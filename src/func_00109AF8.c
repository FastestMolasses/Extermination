// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_00109AF8(int *a0, int a1, int a2, int a3) {
    int *v0 = (int *)a0[0x40 / 4];
    int *v1 = (int *)((char *)v0 + 0xC);
    int ret;
    a1 = a1 * 8;
    v0 = (int *)((char *)v0 + a1);
    v1 = (int *)((char *)v1 + a1);
    v0[4] = a3;
    ret = v1[0];
    v1[0] = a2;
    return ret;
}
