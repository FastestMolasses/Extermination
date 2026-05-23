// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_00206500(int *a0) {
    int v1 = a0[0x54 / 4];
    int v0 = a0[0x48 / 4];
    return !(v1 < v0);
}
