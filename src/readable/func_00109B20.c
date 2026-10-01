// NEARMISS func_00109B20  (vram 0x00109B20, 0x50 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 72.00% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc branch layout of the two null checks and the result register.
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
    int (*fn)(char *, int *, void *);
    int r = 0;

    if (m != 0) {
        tab = *(char **)(m + 0x40);
        if (tab == 0) {
            return 0;
        }
        fn = *(int (**)(char *, int *, void *))(tab + cb[0] * 8 + 0xC);
        if (fn == 0) {
            return 0;
        }
        r = fn(m, cb, *(void **)(tab + cb[0] * 8 + 0x10));
    }
    return r;
}
