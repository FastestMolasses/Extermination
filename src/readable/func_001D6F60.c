// NEARMISS func_001D6F60  (vram 0x001D6F60, 0x94 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 98.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Register colouring only, as func_001D7080: the scaled channel index, the GIF constant and the
// first packet pointer get other temporaries (struct-array cursor spelling 98.65 -> 98.78; the
// permuter found no closer form in 45 minutes).
//
// The function links from the asm body in src/func_001D6F60.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Appends a texture switch to packet channel ch of the render context
// (D_00275670 + 0x10 + ch * 4 is the channel's cursor): a DMA cnt tag (id
// 0x10, 5 quadwords), VIF NOP + DIRECT 4, a GIF A+D tag (NLOOP 3, EOP) and
// the registers TEXFLUSH = 0, TEX0_1 = tex0 and TEXA = texa. The cursor
// advances 6 quadwords.
typedef unsigned int u128 __attribute__((mode(TI)));

/* D_00275670: packet context; +0x10 holds one packet cursor per DMA channel. */
typedef struct GifCtx { char pad[0x10]; char *pkt[4]; } GifCtx;
extern GifCtx *D_00275670;

void func_001D6F60(int ch, long long tex0, int texa) {
    GifCtx *c = D_00275670;
    char *q;

    c->pkt[ch][3] = 0x10;
    *(int *)(c->pkt[ch] + 4) = 0;
    *(short *)c->pkt[ch] = 5;
    q = c->pkt[ch];
    c->pkt[ch] = q + 0x60;
    *(u128 *)(q + 0x10) = 0;
    *(int *)(q + 0x1C) = 0x50000004;
    *(long long *)(q + 0x20) = 0x8003 | (long long)0x10000000 << 32;
    *(long long *)(q + 0x28) = 0xE;
    *(long long *)(q + 0x30) = 0;
    *(long long *)(q + 0x38) = 0x3F;
    *(long long *)(q + 0x40) = tex0;
    *(long long *)(q + 0x48) = 6;
    *(long long *)(q + 0x50) = texa;
    *(long long *)(q + 0x58) = 0x3B;
}
