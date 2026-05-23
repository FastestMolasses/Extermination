// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_0010D928(int *a0, int *a1) {
    int v0 = a0[0x10 / 4];
    int a2 = a1[0x1C / 4];
    int v1 = a0[0x14 / 4];
    int *p = (int *)(a2 + v0 * 4);
    *p = v1;
}
