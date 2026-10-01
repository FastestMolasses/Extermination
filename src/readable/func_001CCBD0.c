// NEARMISS func_001CCBD0  (vram 0x001CCBD0, 0xE8 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 90.69% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Loop induction: the original keeps the palette base in a saved register and recomputes
// base + offset each iteration; mwcc strength-reduces the address into a pointer register, which
// shifts the saved-register colouring by one. Same artifact as func_001CCB10.
//
// The function links from the asm body in src/func_001CCBD0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
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

    ch = dmac_channel_base(1);
    for (blk = first; blk <= last; blk++) {
        func_001CCE80(buf.pkt, blk, 1, 0, 0, 0, 8, 8, 0x10);
        for (i = 0; i < 0x100; i += 4) {
            *(unsigned int *)((char *)buf.clut + i) = colour;
        }
        FlushCache(0);
        dma_kick(ch, &buf, 0x17);
        func_00102468(ch, 0, 0);
    }
}
