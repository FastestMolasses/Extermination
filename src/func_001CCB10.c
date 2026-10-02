// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Uploads the 8x8 grey-ramp CLUT. On DMA channel 1 (dmac_channel_base(1)):
// func_001CCBD0(0x1B00, 0x1B80, 0) first, then a texture-transfer packet for
// 64 entries at GS block 0x1B67 (width 1, PSMCT32, 8x8, 0x10 quadwords) is
// built by func_001CCE80; the palette is v0 = 0, v[n+1] = (v[n] + 0x101010)
// | 0x80000000; the D-cache is flushed (FlushCache(0)), the packet is kicked
// with dma_kick(ch, packet, 0x17) and func_00102468(ch, 0, 0) waits for it.
extern int dmac_channel_base(int ch);
extern void func_001CCBD0(int a, int b, int c);
extern void func_001CCE80(void *pkt, int dbp, int dbw, int psm, int x, int y, int w, int h, int qwc);
extern void FlushCache(int mode);
extern void dma_kick(int ch, void *pkt, int n);
extern void func_00102468(int ch, int a, int b);

// One contiguous 0x170-byte DMA source: the 7-quadword transfer packet
// (func_001CCE80) followed directly by the 0x100-byte palette it carries
// (sp+0x30 and sp+0xA0 in the original frame).
typedef struct ClutUpload {
    char pkt[0x70];
    unsigned int clut[0x44];   /* 0x40 entries sent; the original frame reserves 0x10 bytes more */
} ClutUpload;

void func_001CCB10(void) {
    ClutUpload buf;
    int ch;
    int i;
    unsigned int v;
    char *p;

    ch = dmac_channel_base(1);
    func_001CCBD0(0x1B00, 0x1B80, 0);
    func_001CCE80(buf.pkt, 0x1B67, 1, 0, 0, 0, 8, 8, 0x10);
    p = (char *)buf.clut;
    for (i = 0, v = 0; i < 0x100; i += 4) {
        *(unsigned int *)(p + i) = v;
        v += 0x101010;
        v |= 0x80000000;
    }
    FlushCache(0);
    dma_kick(ch, &buf, 0x17);
    func_00102468(ch, 0, 0);
}
