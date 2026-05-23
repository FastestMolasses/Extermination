// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_0020BEF0(unsigned char *a0) {
    int v1 = a0[0x17];
    int v0 = a0[0x19];
    v0 = v1 + v0;
    v1 = a0[0x18];
    if (v0 < v1) {
        return v0;
    }
    return v0 - v1;
}
