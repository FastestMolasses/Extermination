// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 85.79% NEARMISS).
// Fixes read from the original instructions:
// - The function returns the tag pointer it first read (the pre-advance write
//   pointer, still in the return register at exit); it was declared void.
// - The list-pointer slot is spelled as in the byte-matched twin func_001D7100:
//   the channel byte offset (ch * 4) in its own local, the table pointer in a
//   local copy, and the slot formed as integer arithmetic, offset first. That
//   gives the original's register choice and add operand order.
//
// Appends a 2-qword tag to DMA list ch: an id 0x10 tag (addr 0, qwc 1)
// followed by a zeroed qword whose last word is val | 0x14000000; the cursor
// advances by 0x20. Returns the tag (the pre-advance write pointer).

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

DmaTag *func_001D71A0(int ch, int val) {
    int off = ch * 4;
    VifLists *v = D_00275670;
    DmaTag *base = CUR(v, off);
    DmaTag *p;

    base->id = 0x10;
    CUR(v, off)->addr = 0;
    CUR(v, off)->qwc = 1;
    p = CUR(v, off);
    CUR(v, off) = p + 2;
    *(u128 *)(p + 1) = 0;
    p[1].w3 = val | 0x14000000;
    return base;
}
