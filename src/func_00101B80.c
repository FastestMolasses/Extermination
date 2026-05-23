// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
// memset a0[0..a1-1] = 0
void func_00101B80(unsigned char *a0, int a1) {
    int v0;
    if (a1 == 0) {
        return;
    }
    v0 = a1 - 1;
    do {
        *a0 = 0;
        v0--;
        a0++;
    } while (v0 != -1);
}
