// NEARMISS func_00204390  (vram 0x00204390, 0xF4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 99.84% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// One add's operand order (the original adds the byte offset as the second operand); register
// allocation now matches with the two reads cached in reverse declaration order (expression orders
// measured).
//
// The function links from the asm body in src/func_00204390.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Sector ring buffer (movie / stream path): returns the free region as up to
// two spans. Under the ring's semaphore (+0x40), the write offset is
// ((read_sector + count) * 2048 + byte_ofs) % size and the free byte count is
// (capacity - 2 - count) * 2048 - byte_ofs. When it fits before the end of
// the buffer the first span gets it all and the second is empty; otherwise
// the first span runs to the end and the second starts at the buffer base.
typedef struct SectorRing {
    char *base;         /* 0x00 */
    int pad04;
    int capacity;       /* 0x08: in 2048-byte sectors */
    int read_sector;    /* 0x0C */
    int count;          /* 0x10: sectors filled */
    int byte_ofs;       /* 0x14 */
    int size;           /* 0x18: in bytes */
    int pad1C[9];
    int sema;           /* 0x40 */
} SectorRing;

extern int WaitSema(int sema);
extern int SignalSema(int sema);

void func_00204390(SectorRing *r, char **ptr1, int *len1, char **ptr2, int *len2) {
    int ofs;
    int avail;
    int bo;
    int cnt;

    WaitSema(r->sema);
    cnt = r->count;
    bo = r->byte_ofs;
    ofs = ((r->read_sector + cnt) * 2048 + bo) % r->size;
    avail = (r->capacity - 2 - cnt) * 2048 - bo;
    if (r->size - ofs >= avail) {
        *ptr1 = r->base + ofs;
        *len1 = avail;
        *ptr2 = 0;
        *len2 = 0;
    } else {
        *ptr1 = r->base + ofs;
        *len1 = r->size - ofs;
        *ptr2 = r->base;
        *len2 = avail - (r->size - ofs);
    }
    SignalSema(r->sema);
}
