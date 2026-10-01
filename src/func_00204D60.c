// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Movie decoder: queues a time stamp entry. Under the ring's semaphore, when
// the queue (r+0x50, 0x18-byte entries, capacity +0x54, count +0x58, tail
// +0x5C) has room, the entry is checked by sub_pts_is_not_used and, unless
// both its stamps are negative, copied in at the tail (tail advances modulo
// the capacity, count + 1). Returns 1 when there was room, else 0.
typedef struct PtsEntry {
    long long pts;  /* 0x00 */
    long long dts;  /* 0x08 */
    int pos;        /* 0x10 */
    int len;        /* 0x14 */
} PtsEntry;

typedef struct PtsRing {
    char pad[0x40];
    int sema;           /* 0x40 */
    char pad44[0xC];
    PtsEntry *ents;     /* 0x50 */
    int cap;            /* 0x54 */
    int count;          /* 0x58 */
    int tail;           /* 0x5C */
} PtsRing;

extern int WaitSema(int sema);
extern int SignalSema(int sema);
extern void sub_pts_is_not_used(PtsRing *r, PtsEntry *e);

int func_00204D60(PtsRing *r, PtsEntry *e) {
    int ok;

    ok = 0;
    WaitSema(r->sema);
    if (r->count < r->cap) {
        sub_pts_is_not_used(r, e);
        if (e->pts >= 0 || e->dts >= 0) {
            r->ents[r->tail].pts = e->pts;
            r->ents[r->tail].dts = e->dts;
            r->ents[r->tail].pos = e->pos;
            r->ents[r->tail].len = e->len;
            r->count++;
            r->tail = (r->tail + 1) % r->cap;
        }
        ok = 1;
    }
    SignalSema(r->sema);
    return ok;
}
