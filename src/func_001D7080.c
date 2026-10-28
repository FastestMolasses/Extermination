// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a CodeWarrior asm body;
// readable companion src/readable/func_001D7080.c was 98.33%).
// Fixes read from the original instructions:
// - The function returns the channel's packet pointer as it was on entry (the start of the
//   new DMA tag), like its byte-matched siblings func_001D7000 / func_001D6F60; the first
//   cursor read stays live in the return register to the end. The companion returned void,
//   which recoloured the scaled channel index, the first pointer and the tag-id constant.
// - Written in func_001D7000's spelling (DmaTag / VifLists, the CUR() cursor macro).
// Callers in src/ that declare it void and ignore the result are unaffected.
//
// Appends a vertex colour to packet channel ch of the render context
// (D_00275670 + 0x10 + ch * 4 is the channel's cursor): a DMA cnt tag (id
// 0x10, 3 quadwords), VIF NOP + DIRECT 2, a GIF A+D tag (NLOOP 1, EOP) and
// RGBAQ = rgba with Q = q. The cursor advances 4 quadwords. Returns the tag's address.

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

#define CUR(v, off) (((DmaTag **)((off) + (int)(v)))[4])

DmaTag *func_001D7080(int ch, int rgba, float q) {
    int off = ch * 4;
    VifLists *v = D_00275670;
    DmaTag *base = CUR(v, off);
    DmaTag *p;
    long *d;

    base->id = 0x10;
    CUR(v, off)->addr = 0;
    CUR(v, off)->qwc = 3;
    p = CUR(v, off);
    CUR(v, off) = p + 4;
    *(u128 *)(p + 1) = 0;
    p[1].w3 = 0x50000002;
    d = (long *)(p + 2);
    d[0] = 0x8001L | ((long)0x10000000 << 32);
    d[1] = 0xE;
    *(int *)(d + 2) = rgba;
    *((float *)(d + 2) + 1) = q;
    d[3] = 1;
    return base;
}
