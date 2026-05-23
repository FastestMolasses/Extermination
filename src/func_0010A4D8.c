// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_0010A4D8(int *a0, int a1, int a2) {
    int v0 = a1 >> 4;
    int v1 = a2 >> 4;
    a0[3] = v0;
    a0[4] = v1;
    a0[1] = a1;
    a0[2] = a2;
    return 1;
}
