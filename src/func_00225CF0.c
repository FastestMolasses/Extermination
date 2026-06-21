// Auto-recovered trivial leaf function. The boot ELF is stripped, so the
// name is the VRAM address until the real purpose is identified.
// Zero-fills bytes a0[a1..a2] inclusive.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
void func_00225CF0(unsigned char *a0, unsigned int a1, unsigned int a2) {
    while (a1 <= a2) {
        a0[a1] = 0;
        a1++;
    }
}
