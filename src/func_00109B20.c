// COMPILER: eegcc
// CFLAGS: -O2
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 92.00% NEARMISS).
// Fixes read from the original instructions:
//  - the callback table is a struct: a 0xC-byte header followed by an array of
//    8-byte { handler, data } entries indexed by the event type, and the context
//    and event are struct pointers. Writing the dispatch as array indexing
//    (t->ent[cb->type].func / .data, the entry address computed per use) gives
//    the original's recomputation of the entry address in the call's delay slot
//    and its register choice (table in a2, result in a3); the old byte-offset
//    form with one cached entry pointer did not.
//
// Movie library (SDK libmpeg): dispatches a callback event. With a context
// and its callback table (+0x40), the handler for the event's type (cb[0])
// is called as handler(m, cb, data) with the entry's stored data; returns its
// result, or 0 when there is no handler.

typedef struct {
    int (*func)();
    void *data;
} CbEnt;

typedef struct {
    int pad[3];
    CbEnt ent[1];
} CbTab;

typedef struct {
    int pad[16];
    CbTab *cbtab;
} Ctx;

typedef struct {
    int type;
} CbData;

int func_00109B20(Ctx *m, CbData *cb) {
    int r = 0;
    CbTab *t;
    if (m != 0) {
        t = m->cbtab;
        if (t != 0) {
            if (t->ent[cb->type].func != 0) {
                r = t->ent[cb->type].func(m, cb, t->ent[cb->type].data);
            }
        }
    }
    return r;
}
