// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern unsigned char D_0081083C;

// Releases the grab slot a record holds. When bit 7 of the halfword at +0xF6
// is set, clears grab bit (+0xF6 & 7) in the progress byte D_0081083C. The
// halfword itself is cleared on both paths (the original's store sits in the
// return's delay slot).
void func_0012E070(char *a0) {
    short a1 = *(short *)(a0 + 0xf6);
    if (a1 & 0x80) {
        D_0081083C &= ~(1 << (a1 & 0x7));
    }
    *(short *)(a0 + 0xf6) = 0;
}
