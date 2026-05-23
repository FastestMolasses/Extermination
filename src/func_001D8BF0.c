// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_001D8BF0(void *a0, int a1) {
    unsigned char v1;
    if (a1) {
        v1 = ((unsigned char *)a0)[2];
        v1 |= 0x20;
        ((unsigned char *)a0)[2] = v1;
        return;
    }
    v1 = ((unsigned char *)a0)[2];
    v1 &= 0xDF;
    ((unsigned char *)a0)[2] = v1;
}
