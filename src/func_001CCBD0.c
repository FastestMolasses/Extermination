// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Fills the CLUT blocks first..last in GS memory with one colour: for each
// block a 64-entry (8x8 PSMCT32) transfer packet is built by func_001CCE80,
// all entries are set to colour, the D-cache is flushed (FlushCache(0)) and
// the packet is sent on DMA channel 1 (dma_kick(ch, packet, 0x17), then
// func_00102468(ch, 0, 0) waits for it).
extern int dmac_channel_base(int ch);
extern void func_001CCE80(void *pkt, int dbp, int dbw, int psm, int x, int y, int w, int h, int qwc);
extern void FlushCache(int mode);
extern void dma_kick(int ch, void *pkt, int n);
extern void func_00102468(int ch, int a, int b);

// One contiguous 0x170-byte DMA source: the 7-quadword transfer packet
// (func_001CCE80) followed directly by the 0x100-byte palette it carries
// (sp+0x70 and sp+0xE0 in the original frame).
typedef struct ClutUpload {
    char pkt[0x70];
    unsigned int clut[0x44];   /* 0x40 entries sent; the original frame reserves 0x10 bytes more */
} ClutUpload;

void func_001CCBD0(int first, int last, unsigned int colour) {
    ClutUpload buf;
    int ch;
    int blk;
    int i;
    char *p;

    ch = dmac_channel_base(1);
    for (blk = first; blk <= last; blk++) {
        func_001CCE80(buf.pkt, blk, 1, 0, 0, 0, 8, 8, 0x10);
        p = (char *)buf.clut;
        for (i = 0; i < 0x100; i += 4) {
            *(unsigned int *)(p + i) = colour;
        }
        FlushCache(0);
        dma_kick(ch, &buf, 0x17);
        func_00102468(ch, 0, 0);
    }
}
