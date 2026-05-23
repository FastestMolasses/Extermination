// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int *func_001099F8(int *a0, int a1, int a2, int a3) {
    int *v0 = (int *)a0[0x40 / 4];
    v0[0x9C / 4] = a3;
    v0[0x94 / 4] = a1;
    v0[0x98 / 4] = a2;
    return v0;
}
