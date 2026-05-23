// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
// Writes to a struct accessed via a global pointer at 0x70003B6C
void func_001AB790(int a0) {
    int *v1 = *(int **)0x70003B6C;
    ((unsigned char *)v1)[0] = 1;
    v1 = *(int **)0x70003B6C;
    v1[1] = a0;
    v1 = *(int **)0x70003B6C;
    v1[2] = 0;
    v1[3] = 0;
    v1[4] = 0;
    v1[5] = 0;
}
