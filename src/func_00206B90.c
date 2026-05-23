// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_00206B90(int *a0, int a1) {
    int v0 = a0[0xA8 / 4];
    a0[0xA8 / 4] = a1;
    return v0;
}
