// NEARMISS func_00109B20  (vram 0x00109B20, 0x50 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 92.00% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register choice (the original keeps the table in a2 and the result in a3) and the
// original recomputes the entry address in the call's delay slot where this compile uses a branch-
// likely (nested-if form brought it from 72.00).
//
// The function links from the asm body in src/func_00109B20.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// Movie library (SDK libmpeg): dispatches a callback event. With a context
// and its callback table (+0x40), the handler for the event's type (cb[0])
// is called as handler(m, cb, arg) with the stored argument; returns its
// result, or 0 when there is no handler.
int func_00109B20(char *m, int *cb) {
    char *tab;
    char *ent;
    int (*fn)(char *, int *, void *);
    int r = 0;

    if (m != 0) {
        tab = *(char **)(m + 0x40);
        if (tab != 0) {
            ent = tab + cb[0] * 8;
            fn = *(int (**)(char *, int *, void *))(ent + 0xC);
            if (fn != 0) {
                r = fn(m, cb, *(void **)(ent + 0x10));
            }
        }
    }
    return r;
}
