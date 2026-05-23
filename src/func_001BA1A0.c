// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_001BA1A0(int *a0, int a1) {
    a0[0] = 1;
    a0[1] = 0;
    a0[2] = a1;
    ((unsigned char *)a0)[0xC] = 0;
}
