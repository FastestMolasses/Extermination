// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
unsigned int func_001FE4B0(int *a0, int a1) {
    unsigned int v0;
    int v1 = (int)a0 + 0x10;
    v0 = (unsigned int)(a1 << 4);
    v0 = v0 + (unsigned int)v1;
    return *(unsigned int *)(v0 + 0xC) >> 4;
}
