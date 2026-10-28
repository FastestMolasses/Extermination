// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 73.89% NEARMISS
// companion in src/readable/). Fix read from the original instructions:
//  - the callback table is the same struct as in func_00109B20 (CbTab: a
//    0xC-byte header and an array of 8-byte { func, data } entries), indexed per
//    use (t->ent[type].data / .func). The old byte-offset form with a cached slot
//    pointer computed the entry address in a different order and registers.

// Movie library (SDK libmpeg): installs a callback. Entry type of the context's
// callback table (+0x40) gets func and arg; the previous function is returned.
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

void *func_00109AF8(Ctx *m, int type, int (*func)(), void *arg) {
    CbTab *t = m->cbtab;
    int (*old)();
    t->ent[type].data = arg;
    old = t->ent[type].func;
    t->ent[type].func = func;
    return (void *)old;
}
