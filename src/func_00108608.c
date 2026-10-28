// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 78.62% NEARMISS
// companion in src/readable/). Fix read from the original instructions:
//  - statement order only (same BitReader struct as func_00108790): the original
//    stores start, clears the window, the bit count and the position, then stores
//    the cursor, the ring start, the ring end and the size. The old order (cursor
//    first, ring last) made ee-gcc schedule the stores differently.

// Bitstream reader (SDK, movie path): initialise br over the ring buffer
// [ring, ring + size) with its first byte at start, then fill the window
// (func_00108660 with a skip of 0).
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

void func_00108608(BitReader *br, unsigned char *start, unsigned char *ring, int size) {
    br->start = start;
    br->bits = 0;
    br->nbits = 0;
    br->pos = 0;
    br->cur = start;
    br->ring = ring;
    br->end = ring + size;
    br->size = size;
    func_00108660(br, 0);
}
