// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_001F6AC0(int *a0) {
    unsigned int v0 = ~(unsigned int)a0[0x24 / 4];
    return v0 != 0;
}
