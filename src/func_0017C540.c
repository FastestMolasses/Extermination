// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_0017C540(unsigned char *a0) {
    unsigned char v1 = a0[0x25C];
    if (v1) {
        a0[5] = 1;
        a0[6] = 0;
        a0[0x1F0] = 1;
        a0[0x1F1] = 0;
    } else {
        a0[5] = 0;
        a0[6] = 0;
        a0[0x1F0] = 0;
        *(int *)((char *)a0 + 0x38) = 0;
    }
    a0[4] = 1;
}
