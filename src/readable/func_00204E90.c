// NEARMISS func_00204E90  (vram 0x00204E90, 0x1C0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 75.23% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Scheduling and saved-register allocation: the original extracts the IPU_BP fields before the
// semaphore wait and keeps them in two more saved registers (the body arithmetic and search order
// follow the original).
//
// The function links from the asm body in src/func_00204E90.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
    unsigned int madr = *(volatile unsigned int *)0x1000B410;
    unsigned int bit = r->ipu_bp & 0x7F;
    unsigned int size = r->sectors << 11;
    unsigned int bp = *(volatile unsigned int *)0x10002020;
    unsigned int fp = (bp >> 16) & 3;
    unsigned int ifc = (bp >> 8) & 0xF;
    int found = 0;
    unsigned int at;
    int first;
    int n;
    int i;
    int k;

    WaitSema(r->sema);
    at = size + (madr - (fp + ifc) * 16 + ((int)bit >> 3));
    out->pts = -1;
    out->dts = -1;
    n = r->count;
    first = r->tail - n;
    at = (at - r->base) % size + size;
    for (i = 0; i < n && found == 0; i++) {
        k = (i + (r->cap + first)) % r->cap;
        if ((int)(at - r->ents[k].pos) % (int)size < r->ents[k].len) {
            out->pts = r->ents[k].pts;
            out->dts = r->ents[k].dts;
            r->ents[k].pts = -1;
            r->ents[k].dts = -1;
            found = 1;
            r->count -= (r->count > 0) ? found : r->count;
        }
    }
    SignalSema(r->sema);
    return 1;
}
