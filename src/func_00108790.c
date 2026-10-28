// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 94.00% NEARMISS).
// Fixes read from the original instructions:
//  - Statement order: the original clears the bit window and the bit count and
//    stores the new position before it computes the byte cursor; the old C
//    computed the cursor between the two clears, which moved the position store
//    ahead of the clears (not a register-allocation wall).

// Bitstream reader (SDK, movie path): seek forward n bytes from the current
// bit position. pos += n * 8; the byte cursor becomes start + pos / 8 (wrapped
// back by size when it passes end), the window is emptied and refilled by
// func_00108660 with a skip of 0.
typedef struct BitReader {
    unsigned long long bits;    /* 0x00: left-aligned bit window */
    unsigned char *start;       /* 0x08: first byte of the stream */
    unsigned char *cur;         /* 0x0C: next byte to load */
    unsigned int nbits;         /* 0x10: valid bits in the window */
    int pad14;
    long long pos;              /* 0x18: bits consumed since start */
    unsigned char *ring;        /* 0x20: ring buffer start (wrap target) */
    unsigned char *end;         /* 0x24: ring buffer end */
    int size;                   /* 0x28: ring buffer size in bytes */
} BitReader;

extern void func_00108660(BitReader *br, int n);

void func_00108790(BitReader *br, int n) {
    long long pos = (n << 3) + br->pos;
    unsigned char *p;

    br->bits = 0;
    br->nbits = 0;
    br->pos = pos;
    p = br->start + (int)(pos >> 3);
    br->cur = p;
    if (p >= br->end) {
        br->cur = p - br->size;
    }
    func_00108660(br, 0);
}
