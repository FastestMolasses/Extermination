// NEARMISS func_00108790  (vram 0x00108790, 0x54 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 94.00% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register allocation of the argument copies differs.
//
// The function links from the asm body in src/func_00108790.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

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
    p = br->start + (int)(pos >> 3);
    br->nbits = 0;
    br->pos = pos;
    br->cur = p;
    if (p >= br->end) {
        br->cur = p - br->size;
    }
    func_00108660(br, 0);
}
