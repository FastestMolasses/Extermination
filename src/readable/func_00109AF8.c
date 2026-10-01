// NEARMISS func_00109AF8  (vram 0x00109AF8, 0x24 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 73.89% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own (mwcc 2.3 / 2.3.3 / 2.4 give
// 75.33%; this is SDK code, so ee-gcc is the compiler of record). Object
// similarity does not prove semantic equivalence. Remaining differences:
// Scheduling and register choice of the table address arithmetic (mwcc and ee-gcc measured).
//
// The function links from the asm body in src/func_00109AF8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// Movie library (SDK libmpeg): installs a callback. The context's +0x40
// table holds 8-byte (function, argument) pairs from +0xC; entry type gets
// func and arg and the previous function is returned.
void *func_00109AF8(char *m, int type, void *func, void *arg) {
    char *tab = *(char **)(m + 0x40);
    void **slot = (void **)(tab + 0xC + type * 8);
    void *old;

    *(void **)(tab + type * 8 + 0x10) = arg;
    old = *slot;
    *slot = func;
    return old;
}
