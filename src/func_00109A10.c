// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_00109A10(int *a0, int *a1, int *a2, int *a3) {
    int *t0 = (int *)a0[0x40 / 4];
    *a1 = t0[0x94 / 4];
    *a2 = t0[0x98 / 4];
    *a3 = t0[0x9C / 4];
}
