// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.88% NEARMISS).
// Fixes read from the original instructions:
// - The "register coloring" residual was the spelling of the list-pointer slot
//   address. The original keeps the channel's byte offset (ch * 4) in its own
//   local and forms the slot as integer arithmetic, offset first, on a local
//   copy of the list table pointer; that gives both the target's register
//   choice (offset before the table pointer) and its add operand order.
// - block_copy is the game's three-argument memcpy (dst, src, len), as in the
//   matched callers; the old fourth argument was only the header pointer that
//   already sits in the fourth argument register.
//
// Sub-allocator on DMA list ch: takes the list's current write pointer
// (D_00275670->cur[ch], table +0x10), stamps a header tag (qwc = n + 1, id
// byte 0x10 at +3, +4 = 0) where n = len >> 4, advances the pointer past the
// header and n + 1 more quadwords, fills the next quadword with {0, 0,
// 0x01000404, arg1 | (n << 16) | 0x6C000000} and copies len bytes of payload
// from src after it. Returns the header (the pre-advance pointer).
// D_00275670 is gp-relative (sdatathreshold 8).

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
extern void *block_copy(void *dst, const void *src, unsigned int len);

/* v->cur[ch], spelled the way the original computes it */
#define CUR(v, off) (((DmaTag **)((off) + (int)(v)))[4])

DmaTag *func_001D7100(int ch, int arg1, int src, int len) {
    int off = ch * 4;
    VifLists *v = D_00275670;
    int n = len >> 4;
    DmaTag *base = CUR(v, off);
    DmaTag *p;
    int *q;

    base->id = 0x10;
    CUR(v, off)->addr = 0;
    CUR(v, off)->qwc = n + 1;
    p = CUR(v, off);
    CUR(v, off) = p + (n + 2);
    q = (int *)(p + 1);
    q[0] = 0;
    q[1] = 0;
    q[2] = 0x01000404;
    q[3] = arg1 | (n << 0x10) | 0x6C000000;
    block_copy(q + 4, (const void *)src, len);
    return base;
}
