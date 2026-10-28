// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 78.95% NEARMISS
// companion in src/readable/). Fix read from the original instructions:
//  - the byte fetch post-increments the cursor inside the OR expression
//    (*br->cur++); the old separate br->cur++ statement made ee-gcc store the
//    cursor before the window and keep the window in another register.

// Bitstream reader (SDK, movie path): skip n bits. The window shifts left by
// n, then is refilled a byte at a time (each byte ORed in at bit 56 - nbits)
// while 56 or fewer bits are valid; the byte cursor wraps from end back to the
// ring start. pos accumulates the skipped bits.
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

void func_00108660(BitReader *br, int n) {
    br->bits <<= n;
    br->nbits -= n;
    while (br->nbits < 57) {
        br->bits |= (unsigned long long)*br->cur++ << (56 - br->nbits);
        if (br->cur >= br->end) {
            br->cur = br->ring;
        }
        br->nbits += 8;
    }
    br->pos += n;
}
