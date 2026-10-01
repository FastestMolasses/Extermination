// NEARMISS func_00205240  (vram 0x00205240, 0x4BC bytes) — readable companion C, NOT byte-identical.
//
// objdiff 82.26% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Prologue scheduling of the scratchpad offset loads and the stack-argument loads; the original
// shifts the upload size unsigned and sign-extends the BITBLTBUF width from 26 bits (the helpers'
// packed-field types); the call sequence, every argument value and the loop nesting follow the
// instructions (rewritten from them 2026-10-01 after review: earlier text dropped register
// arguments and nested the upload loops the wrong way round).
//
// The function links from the asm body in src/func_00205240.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
// original sets (prototypes from their own files; the void-returning ones
// tail-call func_00205A50 and so return the advanced packet pointer).
// func_00205A80 (TEXFLUSH) reads only the packet pointer.
extern char *func_00205990(char *dst, unsigned int a1, int addr, unsigned int a3,
                           unsigned int id, unsigned int t1, unsigned int qwc);
extern char *func_00205A00(char *dst, unsigned long long regs, int nreg, int flg, int d, int e,
                           int eop, int nloop);
extern char *func_00205A80(char *pkt);
extern char *func_00205A90(char *pkt, unsigned int lcm, unsigned int mxl, unsigned int mmag,
                           unsigned int mmin, unsigned int mtba, unsigned int l, unsigned int k);
extern char *func_00205B00(char *pkt, unsigned int tbp0, unsigned int tbw, unsigned int psm,
                           unsigned int tw, unsigned int th, unsigned int tcc, unsigned int tfx,
                           unsigned int cbp, unsigned int cpsm, unsigned int csm,
                           unsigned int csa, unsigned int cld);
extern char *func_00205EA0(char *pkt, unsigned int dbp, unsigned int dbw, unsigned int dpsm);
extern char *func_00205EE0(char *pkt, unsigned int dir, unsigned int dsax, unsigned int dsay);
extern char *func_00205F20(char *pkt, unsigned int w, unsigned int h);
extern char *func_00205F40(char *pkt, unsigned int xdir);
extern char *func_00205810(char *pkt, int *xy, int *uv);

void func_00205240(int *m, unsigned long long pkt_addr, unsigned long long src, int keep,
                   int x0, int y0, int x1, int y1, int w, int h) {
    int r0[4];
    int t0[4];
    int r1[4];
    int t1[4];
    int ox;
    int oy;
    int nx;
    int ny;
    int bx;
    int by;
    int x;
    int y;
    char *pkt;
    unsigned int blocks;

    r0[0] = 0;
    r0[1] = ((m[0xD] + 0x1F) >> 5) << 10;
    r0[2] = x1 * 16;
    ny = h >> 4;
    nx = w >> 4;
    ox = 0x40 - *(short *)0x70003B94;
    oy = 0x30 - *(short *)0x70003B96 * 2;
    r0[3] = y1 * 16;
    pkt = (char *)(int)((pkt_addr & 0x0FFFFFFF) | 0x20000000);
    if (keep == 0) {
        t0[0] = ox * 16 + 8;
        t0[1] = oy * 16 + 8;
        t0[2] = x1 * 16;
        t0[3] = 0x1C00;
        r1[0] = x0 * 16;
        r1[1] = y0 * 16;
        r1[2] = x1 * 16;
        t1[0] = 8;
        t1[1] = 8;
        t1[2] = 0x2000;
        r1[3] = y1 * 16;
        t1[3] = 0xC00;
        blocks = ((((m[0xC] + 0x3F) >> 6) << 6) * (((m[0xD] + 0x1F) >> 5) << 5) * 3) >> 6;
        pkt = func_00205990(pkt, 0, 0, 0, 1, 0, 3);
        pkt = func_00205A00(pkt, 0xE, 1, 0, 0, 0, 0, 2);
        pkt = func_00205EA0(pkt, blocks, (w + 0x3F) >> 6, 0);
        pkt = func_00205F20(pkt, 0x10, 0x10);
        for (bx = 0, x = 0; bx < nx; bx++, x += 0x10) {
            for (by = 0, y = 0; by < ny; by++, y += 0x10) {
                pkt = func_00205990(pkt, 0, 0, 0, 1, 0, 4);
                pkt = func_00205A00(pkt, 0xE, 1, 0, 0, 0, 0, 2);
                pkt = func_00205EE0(pkt, 0, x, y);
                pkt = func_00205F40(pkt, 0);
                pkt = func_00205A00(pkt, 0, 0, 2, 0, 0, 0, 0x40);
                pkt = func_00205990(pkt, 0, (int)(src & 0x0FFFFFFF), 0, 3, 0, 0x40);
                src += 0x400;
            }
        }
    } else {
        t0[0] = ox * 16 + 8;
        t0[1] = oy * 16 + 0x18;
        r1[0] = x0 * 16;
        t0[2] = x1 * 16;
        t0[3] = 0x1C00;
        t1[0] = 8;
        r1[1] = (y0 + (((m[0xD] + 0x1F) >> 5) << 5)) * 16;
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
                        ((((m[0xC] + 0x3F) >> 6) << 6) * (((m[0xD] + 0x1F) >> 5) << 5) * 3) >> 6,
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
