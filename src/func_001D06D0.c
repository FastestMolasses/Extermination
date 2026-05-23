// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_001D06D0(int *a0, unsigned char a1) {
    int *v1 = (int *)a0[0x90 / 4];
    ((unsigned char *)v1)[0x81] = a1;
}
