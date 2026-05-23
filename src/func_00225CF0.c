// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
// memset-like: fill a0[a1..a2-1] with 0
void func_00225CF0(unsigned char *a0, unsigned int a1, unsigned int a2) {
    if (a2 < a1) {
        return;
    }
    do {
        a0[a1] = 0;
        a1++;
    } while (a1 <= a2);
}
