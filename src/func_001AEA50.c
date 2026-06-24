// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Builds two 0x60-byte GIF/DMA tag packets in D_0028A8E0, then clears
// the associated state words and calls func_001AE900 to kick the chain.

extern unsigned int D_0028A8E0;
extern short D_0028A9A0;
extern char D_0028A9A2;
extern char D_0028A9A3;
extern short D_0028A9A4;

extern void func_001AE900();

void func_001AEA50(void) {
    int *p;
    int i;

    D_0028A9A0 = 0;
    D_0028A9A3 = 0;
    D_0028A9A2 = 0;
    p = (int *)&D_0028A8E0;
    D_0028A9A4 = 0;
    i = 0;
    do {
        p[0] = 0x11000000;
        p[1] = 0;
        p[2] = 0;
        p[3] = 0x50000005;
        p[4] = 0x8001;
        p[5] = 0x41A34000;
        p[6] = 0xFFFF441E;
        p[7] = -1;
        *(long long *)(p + 8) = (long long)0xA1 | ((long long)0x80 << 32);
        *(long long *)(p + 10) = 0x43;
        p[12] = 0xFF;
        p[13] = 0xFF;
        p[14] = 0xFF;
        p[15] = 0x80;
        p[16] = 0x7000;
        p[17] = 0x7900;
        p[18] = 0x0FFFFFF0;
        p[19] = 0;
        p[20] = 0x9000;
        p[21] = 0x8700;
        i += 1;
        p[22] = 0x0FFFFFF0;
        p[23] = 0;
        p += 24;
    } while (i < 2);
    func_001AE900();
}
