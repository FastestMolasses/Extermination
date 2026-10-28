// NEARMISS func_00205240  (vram 0x00205240, 0x4BC bytes) — readable companion C, NOT byte-identical.
//
// objdiff 97.07% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own
// (was 82.26%). Object similarity does not prove semantic equivalence. Matching wave 2026-10-28
// fixes read from the original instructions:
//  - the packet cursor is a 64-bit value end to end: pkt = (pkt_addr & 0x0FFFFFFF) | 0x20000000
//    is passed on without a 32-bit sign extension, so in this file the helpers take and return
//    unsigned long long (their own files declare a pointer);
//  - src is a 32-bit address (the += 0x400 is a 32-bit add) zero-extended into a 64-bit third
//    argument of func_00205990 (its own file declares int);
//  - the 0x70003B94 / 0x70003B96 offsets are relocated scratchpad externs (idiom-32, // SPAD:);
//  - the macroblock x / y are bx * 16 / by * 16 (mwcc's induction variables), not hand-kept
//    counters, which puts the loop-counter clears in the original's delay slots;
//  - the DBP / TBP0 sizes are shifted unsigned, the BITBLTBUF width is ((w+63)>>6<<6)>>6 (the
//    26-bit sign extension) and TBW the same value shifted unsigned;
//  - prologue statement order (ox first, oy before r0[3], ny before nx).
// Remaining differences: prologue register choice only: the original computes y1 * 16 before the
// first scratchpad load and reuses y1's register for it (and m+0x34 / h land in a0 / t5 instead of
// t3 / a0); about 40 statement orders, local types and a 560 s permuter run did not move it.
//
// The function links from the asm body in src/func_00205240.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003B94 0x70003B96

// Movie decoder: builds the GS packet that shows a decoded frame (title
// movie path). pkt_addr is the packet buffer (used through the uncached
// accelerated mirror), src the decoded image in 16x16 macroblocks of 0x400
// bytes, m+0x30 / m+0x34 the frame width / height in pixels, and w / h the
// region in pixels. The scratchpad halfwords 0x70003B94 / 0x70003B96 give
// the display offset. Unless keep is set, the frame is first uploaded:
//   DMA cnt (3 qw): GIF tag A+D x2, BITBLTBUF (DBP = (width rounded to 64)
//     * (height rounded to 32) * 3 / 64, DBW = (w + 63) / 64, PSMCT32),
//     TRXREG 16 x 16;
//   per macroblock, x = 0, 16, .. < w outside and y = 0, 16, .. < h inside
//   (column-major; src advances 0x400 per block): DMA cnt (4 qw) with a
//     GIF tag A+D x2, TRXPOS (0, 0, x, y), TRXDIR 0 (host to local) and an
//     IMAGE GIF tag of 0x40 qw, then a DMA ref of the block's 0x40 qw.
// Then a DMA end tag (0x11 qw) and a GIF tag A+D x16 (EOP) with two
// textured sprites: TEXFLUSH, TEX1 0 (nearest), TEX0 (TBP0 = the upload's
// base, TBW = (w rounded to 64) / 64, PSMCT32, TW = TH = 10, TFX = 1 decal),
// sprite via func_00205810 with the rectangle pairs r0 / t0; then TEXFLUSH,
// TEX1 with MMAG = MMIN = 1 (linear), TEX0 of the second plane (TBP0 =
// width * height * 2 / 64, TBW = width / 64, TW = TH = 10, TFX = 1) and the
// sprite r1 / t1. The rectangles are on the stack in 1/16 pixel units with
// 8 / 0x18 subpixel offsets.
// Every helper is called with all the register and stack arguments the
// original sets (the void-returning ones tail-call func_00205A50 and so
// return the advanced packet pointer).
// func_00205A80 (TEXFLUSH) reads only the packet pointer.
extern short D_70003B94;
extern short D_70003B96;
extern unsigned long long func_00205990(unsigned long long dst, unsigned int a1, unsigned long long addr, unsigned int a3,
                           unsigned int id, unsigned int t1, unsigned int qwc);
extern unsigned long long func_00205A00(unsigned long long dst, unsigned long long regs, int nreg, int flg, int d, int e,
                           int eop, int nloop);
extern unsigned long long func_00205A80(unsigned long long pkt);
extern unsigned long long func_00205A90(unsigned long long pkt, unsigned int lcm, unsigned int mxl, unsigned int mmag,
                           unsigned int mmin, unsigned int mtba, unsigned int l, unsigned int k);
extern unsigned long long func_00205B00(unsigned long long pkt, unsigned int tbp0, unsigned int tbw, unsigned int psm,
                           unsigned int tw, unsigned int th, unsigned int tcc, unsigned int tfx,
                           unsigned int cbp, unsigned int cpsm, unsigned int csm,
                           unsigned int csa, unsigned int cld);
extern unsigned long long func_00205EA0(unsigned long long pkt, unsigned int dbp, unsigned int dbw, unsigned int dpsm);
extern unsigned long long func_00205EE0(unsigned long long pkt, unsigned int dir, unsigned int dsax, unsigned int dsay);
extern unsigned long long func_00205F20(unsigned long long pkt, unsigned int w, unsigned int h);
extern unsigned long long func_00205F40(unsigned long long pkt, unsigned int xdir);
extern unsigned long long func_00205810(unsigned long long pkt, int *xy, int *uv);

void func_00205240(int *m, unsigned long long pkt_addr, unsigned int src, int keep,
                   int x0, int y0, int x1, int y1, int w, int h) {
    int r0[4];
    int t0[4];
    int r1[4];
    int t1[4];
    int nx;
    int ny;
    int bx;
    int by;
    int ox;
    int oy;
    unsigned long long pkt;
    unsigned int blocks;

    ox = 0x40 - D_70003B94;
    r0[0] = 0;
    r0[1] = ((m[0xD] + 0x1F) >> 5) << 10;
    r0[2] = x1 * 16;
    oy = 0x30 - D_70003B96 * 2;
    r0[3] = y1 * 16;
    ny = h >> 4;
    nx = w >> 4;
    pkt = (pkt_addr & 0x0FFFFFFF) | 0x20000000;
    if (keep == 0) {
        t0[0] = ox * 16 + 8;
        t0[1] = oy * 16 + 8;
        t0[2] = x1 * 16;
        t0[3] = 0x1C00;
        r1[0] = x0 * 16;
        r1[1] = y0 * 16;
        r1[2] = x1 * 16;
        r1[3] = y1 * 16;
        t1[0] = 8;
        t1[1] = 8;
        t1[2] = 0x2000;
        t1[3] = 0xC00;
        blocks = (unsigned int)((((m[0xC] + 0x3F) >> 6) << 6) * (((m[0xD] + 0x1F) >> 5) << 5) * 3) >> 6;
        pkt = func_00205990(pkt, 0, 0, 0, 1, 0, 3);
        pkt = func_00205A00(pkt, 0xE, 1, 0, 0, 0, 0, 2);
        pkt = func_00205EA0(pkt, blocks, (((w + 0x3F) >> 6) << 6) >> 6, 0);
        pkt = func_00205F20(pkt, 0x10, 0x10);
        for (bx = 0; bx < nx; bx++) {
            for (by = 0; by < ny; by++) {
                pkt = func_00205990(pkt, 0, 0, 0, 1, 0, 4);
                pkt = func_00205A00(pkt, 0xE, 1, 0, 0, 0, 0, 2);
                pkt = func_00205EE0(pkt, 0, bx * 16, by * 16);
                pkt = func_00205F40(pkt, 0);
                pkt = func_00205A00(pkt, 0, 0, 2, 0, 0, 0, 0x40);
                pkt = func_00205990(pkt, 0, src & 0x0FFFFFFF, 0, 3, 0, 0x40);
                src += 0x400;
            }
        }
    } else {
        t0[0] = ox * 16 + 8;
        t0[1] = oy * 16 + 0x18;
        r1[0] = x0 * 16;
        t0[2] = x1 * 16;
        t0[3] = 0x1C00;
        r1[1] = (y0 + (((m[0xD] + 0x1F) >> 5) << 5)) * 16;
        t1[0] = 8;
        t1[1] = 8;
        r1[2] = x1 * 16;
        t1[2] = 0x2000;
        r1[3] = y1 * 16;
        t1[3] = 0xC00;
    }
    pkt = func_00205990(pkt, 0, 0, 0, 7, 0, 0x11);
    pkt = func_00205A00(pkt, 0xE, 1, 0, 0, 0, 1, 0x10);
    pkt = func_00205A80(pkt);
    pkt = func_00205A90(pkt, 0, 0, 0, 0, 0, 0, 0);
    pkt = func_00205B00(pkt,
                        (unsigned int)((((m[0xC] + 0x3F) >> 6) << 6) * (((m[0xD] + 0x1F) >> 5) << 5) * 3) >> 6,
                        ((unsigned int)(((w + 0x3F) >> 6) << 6)) >> 6, 0, 10, 10, 0, 1,
                        0, 0, 0, 0, 0);
    pkt = func_00205810(pkt, r0, t0);
    pkt = func_00205A80(pkt);
    pkt = func_00205A90(pkt, 0, 0, 1, 1, 0, 0, 0);
    blocks = ((m[0xC] + 0x3F) >> 6) << 6;
    pkt = func_00205B00(pkt, (blocks * (((m[0xD] + 0x1F) >> 5) << 5) * 2) >> 6, blocks >> 6,
                        0, 10, 10, 0, 1, 0, 0, 0, 0, 0);
    func_00205810(pkt, r1, t1);
}
