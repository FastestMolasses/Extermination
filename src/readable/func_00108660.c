// NEARMISS func_00108660  (vram 0x00108660, 0x98 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 78.95% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register allocation and refill-loop scheduling differ (window kept in a different
// register; the cursor store and branch are ordered differently).
//
// The function links from the asm body in src/func_00108660.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

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
        br->bits |= (unsigned long long)*br->cur << (56 - br->nbits);
        br->cur++;
        if (br->cur >= br->end) {
            br->cur = br->ring;
        }
        br->nbits += 8;
    }
    br->pos += n;
}
