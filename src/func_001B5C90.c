// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
int func_001B5C90(int a0) {
    int v0 = a0 & 0xFF;
    int v1;
    v0 += 2;
    v1 = v0 & 0xFFFF;
    if (v1 < 0x100) {
        return v1 & 0xFC;
    }
    return 0xFC;
}
