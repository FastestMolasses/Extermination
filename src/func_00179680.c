// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_00179680(unsigned char *a0) {
    a0[5] = 5;
    a0[6] = 0;
    a0[0x1F0] = 0xB;
    a0[0x25F] = 2;
    if (!a0[0x236]) {
        return;
    }
    a0[0x235] = a0[0x235] & 1;
    a0[0x236] = 0;
}
