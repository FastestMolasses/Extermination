// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
unsigned int func_001FE4B0(int *a0, int a1) {
    int *v1 = (int *)((char *)a0 + 0x10);
    unsigned int v0 = (unsigned int)((int *)((char *)v1 + a1 * 0x10))[3] >> 4;
    return v0;
}
