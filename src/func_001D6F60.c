// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a readable companion of an
// asm body at 98.78%). Fixes read from the original instructions:
// - The function returns the channel's packet pointer as it was on entry (the start of the
//   new DMA tag), exactly like its byte-matched sibling func_001D7000; the first cursor
//   read stays live in the return register to the end. The companion returned void, which
//   recoloured the scaled channel index, the GIF constant and that first pointer.
// - Written in func_001D7000's spelling (DmaTag / VifLists, the CUR() cursor macro).
// Callers in src/ declare it void and ignore the result; that is harmless.
//
// Appends a texture switch to packet channel ch of the render context
// (D_00275670 + 0x10 + ch * 4 is the channel's cursor): a DMA cnt tag (id
// 0x10, 5 quadwords), VIF NOP + DIRECT 4, a GIF A+D tag (NLOOP 3, EOP) and
// the registers TEXFLUSH = 0, TEX0_1 = tex0 and TEXA = texa. The cursor
// advances 6 quadwords. Returns the tag's address.

typedef unsigned __int128 u128;
typedef struct {
    unsigned short qwc;
    unsigned char pad;
    unsigned char id;
    int addr;
    int w2;
    int w3;
} DmaTag;
typedef struct {
    char pad[0x10];
    DmaTag *cur[16];
} VifLists;
extern VifLists *D_00275670;

/* v->cur[ch], spelled the way the original computes it */
#define CUR(v, off) (((DmaTag **)((off) + (int)(v)))[4])

DmaTag *func_001D6F60(int ch, long tex0, int texa) {
    int off = ch * 4;
    VifLists *v = D_00275670;
    DmaTag *base = CUR(v, off);
    DmaTag *p;
    long *d;

    base->id = 0x10;
    CUR(v, off)->addr = 0;
    CUR(v, off)->qwc = 5;
    p = CUR(v, off);
    CUR(v, off) = p + 6;
    *(u128 *)(p + 1) = 0;
    p[1].w3 = 0x50000004;
    d = (long *)(p + 2);
    d[0] = 0x8003L | ((long)0x10000000 << 32);   /* GIF tag: NLOOP 3, EOP, A+D */
    d[1] = 0xE;
    d[2] = 0;                                     /* TEXFLUSH */
    d[3] = 0x3F;
    d[4] = tex0;                                  /* TEX0_1 */
    d[5] = 6;
    d[6] = (long)texa;                            /* TEXA */
    d[7] = 0x3B;
    return base;
}
