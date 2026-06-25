// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Initializes a 2x2 grid of 0x90-byte display-list blocks at D_0028A7B0: per outer block a 0x30-byte GIF/header (constants), then two 0x30-byte entries whose XY fields come from D_0024A8C0 (s16 * 0x10).
extern short D_0024A8C0;
extern int D_0028A7B0;
extern short D_0028A8D0;
extern short D_0028A8D2;
extern int D_0028A8D4;

void func_001AE900(void) {
    int *hdr;
    int i;
    int j;

    D_0028A8D0 = 0;
    D_0028A8D4 = 0;
    D_0028A8D2 = 0;
    hdr = &D_0028A7B0;
    j = 0;
    do {
        int *ent;
        short *src;

        hdr[0] = 0x13000000;
        hdr[1] = 0;
        hdr[2] = 0;
        hdr[3] = 0x50000008;
        hdr[4] = 0x8001;
        hdr[5] = 0x71A34000;
        hdr[6] = 0xF441441E;
        hdr[7] = -1;
        *((long long *)(((char *)hdr) + 0x20)) = (long long)(0xA1 | (0x80LL << 0x20));
        ent = (int *)(((char *)hdr) + 0x30);
        *((long long *)(((char *)hdr) + 0x28)) = (long long)0x43;
        src = &D_0024A8C0;
        i = 0;
        do {
            ent[0] = 0xFF;
            ent[1] = 0xFF;
            ent[2] = 0xFF;
            ent[3] = 0x80;
            i += 1;
            ent[4] = (int)(src[0] * 0x10);
            ent[5] = (int)(src[1] * 0x10);
            ent[6] = 0x0FFFFFE0;
            ent[7] = 0;
            ent[8] = (int)(src[2] * 0x10);
            ent[9] = (int)(src[3] * 0x10);
            ent[10] = 0x0FFFFFE0;
            ent[11] = 0;
            src += 4;
            ent = (int *)(((char *)ent) + 0x30);
        } while (i < 2);
        j += 1;
        hdr = (int *)(((char *)hdr) + 0x90);
    } while (j < 2);
}
