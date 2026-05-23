// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_00109C18(int *a0, int a1) {
    int *v1 = (int *)a0[0x40 / 4];
    int v0 = v1[0xE8 / 4];
    v1[0xE8 / 4] = a1;
    return v0;
}
