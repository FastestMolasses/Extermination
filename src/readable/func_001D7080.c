// NEARMISS func_001D7080  (vram 0x001D7080, 0x78 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 98.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Register colouring only: the original keeps the scaled channel index in v1, the packet's first
// quadword pointer in v0 and the byte constant in t0, mwcc picks a2 / t0 / t3 (the channel cursor
// written as a struct array, D_00275670->pkt[ch], fixed the add operand order: 96.83 -> 98.33; the
// permuter found no closer form in 45 minutes).
//
// The function links from the asm body in src/func_001D7080.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Appends a vertex colour to packet channel ch of the render context
// (D_00275670 + 0x10 + ch * 4 is the channel's cursor): a DMA cnt tag (id
// 0x10, 3 quadwords), VIF NOP + DIRECT 2, a GIF A+D tag (NLOOP 1, EOP) and
// RGBAQ = rgba with Q = q. The cursor advances 4 quadwords.
typedef unsigned int u128 __attribute__((mode(TI)));

/* D_00275670: packet context; +0x10 holds one packet cursor per DMA channel. */
typedef struct GifCtx { char pad[0x10]; char *pkt[4]; } GifCtx;
extern GifCtx *D_00275670;

void func_001D7080(int ch, int rgba, float q_) {
    GifCtx *c = D_00275670;
    char *q;

    c->pkt[ch][3] = 0x10;
    *(int *)(c->pkt[ch] + 4) = 0;
    *(short *)c->pkt[ch] = 3;
    q = c->pkt[ch];
    c->pkt[ch] = q + 0x40;
    *(u128 *)(q + 0x10) = 0;
    *(int *)(q + 0x1C) = 0x50000002;
    *(long long *)(q + 0x20) = 0x8001 | (long long)0x10000000 << 32;
    *(long long *)(q + 0x28) = 0xE;
    *(int *)(q + 0x30) = rgba;
    *(float *)(q + 0x34) = q_;
    *(long long *)(q + 0x38) = 1;
}
