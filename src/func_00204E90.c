// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 75.23% NEARMISS).
// Fixes read from the original instructions:
// - The IPU_BP fields (FP, IFC) are int locals extracted before the semaphore
//   wait; with unsigned locals mwcc folded them into their single use after the
//   call (two fewer saved registers).
// - Statement order before the wait: found = 0, the toIPU MADR read, the IPU_BP
//   read, then the saved bit pointer, the two fields, and the ring size last.
// - Declaration order (madr, bit, fp, ifc, bp, k, n, i, first, at, size, found)
//   gives the original saved-register assignment.
// - The position is built in steps (madr minus the FIFO bytes, then size plus
//   that plus bit / 8; then the ring-relative modulo and += size), and the count
//   update tests count <= 0 first.

// Movie decoder: finds the time stamps of the picture the IPU is decoding.
// The decode position is the toIPU MADR (0x1000B410) minus the quadwords
// still buffered in the IPU (IPU_BP FP + IFC) plus the saved bit pointer
// (+0x38, bits 0..6) in bytes, taken relative to the ring base modulo the
// ring size (sectors * 2048). out starts as (-1, -1). The queued entries
// (oldest first) are searched for the first one whose span [pos, pos + len)
// contains the position (modulo the ring size); its stamps are copied to
// out, the entry is marked empty and the queue count drops by one (never
// below zero). Runs under the ring's semaphore; returns 1.
typedef struct PtsEntry {
    long long pts;  /* 0x00 */
    long long dts;  /* 0x08 */
    int pos;        /* 0x10 */
    int len;        /* 0x14 */
} PtsEntry;

typedef struct PtsRing {
    unsigned int base;  /* 0x00 */
    int pad04;
    int sectors;        /* 0x08 */
    char pad0C[0x2C];
    unsigned int ipu_bp;/* 0x38 */
    int pad3C;
    int sema;           /* 0x40 */
    char pad44[0xC];
    PtsEntry *ents;     /* 0x50 */
    int cap;            /* 0x54 */
    int count;          /* 0x58 */
    int tail;           /* 0x5C */
} PtsRing;

extern int WaitSema(int sema);
extern int SignalSema(int sema);

int func_00204E90(PtsRing *r, PtsEntry *out) {
    unsigned int madr;
    int bit;
    int fp;
    int ifc;
    unsigned int bp;
    int k;
    int n;
    int i;
    int first;
    unsigned int at;
    unsigned int size;
    int found;

    found = 0;
    madr = *(volatile unsigned int *)0x1000B410;
    bp = *(volatile unsigned int *)0x10002020;
    bit = r->ipu_bp & 0x7F;
    fp = (bp >> 16) & 3;
    ifc = (bp >> 8) & 0xF;
    size = r->sectors << 11;
    WaitSema(r->sema);
    at = madr - (fp + ifc) * 16;
    at = size + (at + (bit >> 3));
    out->pts = -1;
    out->dts = -1;
    n = r->count;
    first = r->tail - n;
    at = (at - r->base) % size;
    at += size;
    for (i = 0; i < n && found == 0; i++) {
        k = (i + (r->cap + first)) % r->cap;
        if ((int)(at - r->ents[k].pos) % (int)size < r->ents[k].len) {
            out->pts = r->ents[k].pts;
            out->dts = r->ents[k].dts;
            r->ents[k].pts = -1;
            r->ents[k].dts = -1;
            found = 1;
            r->count -= (r->count <= 0) ? r->count : found;
        }
    }
    SignalSema(r->sema);
    return 1;
}
