// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a CodeWarrior asm body;
// readable companion src/readable/func_001D7000.c was 98.39%).
// Fixes read from the original instructions:
// - The function returns the tag pointer it first read (the pre-advance write
//   pointer, still in the return register at exit); the companion was void.
// - The list-pointer slot is spelled as in the byte-matched twins func_001D7100 /
//   func_001D71A0: the channel byte offset (ch * 4) in its own local, the table
//   pointer in a local copy, and the slot formed as integer arithmetic, offset
//   first. That gives the original's register choice and add operand order.
//
// Appends one GS register packet to DMA list ch of D_00275670 (the same 0x40-byte
// layout as func_00207D90): an id 0x10 tag (addr 0, qwc 3), a zeroed quadword whose
// last word is 0x50000002, the GIF tag 0x1000000000008001 / 0xE (one A+D register),
// then the data word val sign-extended to 64 bits for GS register 0x3B. The cursor
// advances by 0x40. Returns the tag (the pre-advance write pointer).

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

DmaTag *func_001D7000(int ch, int val) {
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
    d[2] = (long)val;
    d[3] = 0x3B;
    return base;
}
