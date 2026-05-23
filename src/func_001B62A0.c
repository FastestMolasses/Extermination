// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
void func_001B62A0(void *a0) {
    ((unsigned char *)a0)[0x16] = 0;
    ((unsigned short *)a0)[0x28 / 2] = 0;
    ((unsigned char *)a0)[0x18] = 0;
    ((unsigned char *)a0)[0x19] = 0;
}
