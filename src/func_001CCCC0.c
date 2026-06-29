// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Builds a 0x180-byte DMA tag/transfer chain on the stack, then issues it.
// dmac_channel_base(1) -> channel; DisableDmacHandler(0); dma_kick(ch, buf, 0x18);
// func_00102468(ch, 0, 0). Leading qword at 0x20 is zeroed as one sq; the two
// 32-bit words at 0x28/0x2C are the DMA tag header (0x11000000 / 0x50000017).
typedef unsigned u128 __attribute__((mode(TI)));
extern int dmac_channel_base(int);
extern void DisableDmacHandler(int);
extern void dma_kick(int, void *, int);
extern void func_00102468(int, int, int);

typedef union {
    u128 q0;
    struct { unsigned long long z; unsigned int w2; unsigned int w3; } h;
    unsigned long long d[64];
} DmaBuf;

void func_001CCCC0(void) {
    DmaBuf buf;
    int ch;
    unsigned long long t0;
    unsigned long long t1;

    buf.q0 = 0;
    buf.h.w2 = 0x11000000;
    buf.h.w3 = 0x50000017;
    buf.d[2] = (unsigned long long)0x8016 | ((unsigned long long)0x10000000 << 32);
    buf.d[3] = 0xE;
    buf.d[5] = 0x18;
    buf.d[4] = 0;
    buf.d[7] = 0x19;
    buf.d[6] = 0;
    buf.d[9] = 6;
    t0 = (unsigned long long)0x20000000 | ((unsigned long long)0x20000006 << 32);
    buf.d[8] = t0;
    buf.d[11] = 7;
    buf.d[10] = t0;
    buf.d[13] = 0x14;
    buf.d[12] = 0;
    buf.d[15] = 0x15;
    buf.d[14] = 0;
    buf.d[17] = 0x16;
    t1 = (unsigned long long)0x20000000 << 32;
    buf.d[16] = t1;
    buf.d[19] = 0x17;
    buf.d[18] = t1;
    buf.d[21] = 8;
    buf.d[20] = 0;
    buf.d[23] = 9;
    buf.d[22] = 0;
    buf.d[25] = 0x34;
    buf.d[24] = 0;
    buf.d[27] = 0x35;
    buf.d[26] = 0;
    buf.d[29] = 0x36;
    buf.d[28] = 0;
    buf.d[31] = 0x37;
    buf.d[30] = 0;
    buf.d[33] = 0x40;
    t1 = (unsigned long long)0x02000000 | ((unsigned long long)0xE00000 << 32);
    buf.d[32] = t1;
    buf.d[35] = 0x41;
    buf.d[34] = t1;
    buf.d[37] = 0x42;
    buf.d[36] = 0x48;
    buf.d[39] = 0x43;
    buf.d[38] = 0x48;
    buf.d[41] = 0x47;
    buf.d[40] = 0x5000C;
    buf.d[43] = 0x48;
    buf.d[42] = 0x5000C;
    buf.d[45] = 0x4A;
    buf.d[44] = 0;
    buf.d[47] = 0x4B;
    buf.d[46] = 0;
    ch = dmac_channel_base(1);
    DisableDmacHandler(0);
    dma_kick(ch, &buf, 0x18);
    func_00102468(ch, 0, 0);
}
