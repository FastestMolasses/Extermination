// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
unsigned int func_00102658(int *a0) {
    unsigned int v0 = (unsigned int)*a0;
    unsigned int v1 = v0 & ~0x100u;
    unsigned int r = v0 >> 8;
    *a0 = (int)v1;
    return r & 1u;
}
