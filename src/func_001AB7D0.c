// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
// Clears the byte at offset 0 of the struct accessed via global pointer 0x70003B6C
void func_001AB7D0(void) {
    int *v1 = *(int **)0x70003B6C;
    ((unsigned char *)v1)[0] = 0;
}
