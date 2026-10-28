// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a readable companion of an
// asm body at 99.84%). Fix read from the original instructions:
// - the write offset is built in two steps: the sector position scaled to bytes goes in its
//   own local (pos), then (pos + byte_ofs) % size; that keeps the byte offset as the second
//   operand of the add and the original register assignment.

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
    int pos;

    WaitSema(r->sema);
    cnt = r->count;
    bo = r->byte_ofs;
    pos = (r->read_sector + cnt) * 2048;
    ofs = (pos + bo) % r->size;
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
