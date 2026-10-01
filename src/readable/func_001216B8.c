// NEARMISS func_001216B8  (vram 0x001216B8, 0x3C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 42.67% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Branch layout: the original selects the scratch word with a conditional move and loads the byte
// in a likely-branch slot; semantics identical.
//
// The function links from the asm body in src/func_001216B8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libc (newlib, single-byte locale): mbtowc. A null s returns 0 (no shift
// states); n == 0 returns -1; otherwise the byte *s is stored as the wide
// character (to pwc, or to a scratch word when pwc is null) and the result
// is 1 for a non-zero byte, 0 for the terminator. The first argument (the
// reentrancy pointer) is not used.
int func_001216B8(void *reent, int *pwc, unsigned char *s, unsigned int n) {
    int dummy;

    if (pwc == 0) {
        pwc = &dummy;
    }
    if (s == 0) {
        return 0;
    }
    if (n == 0) {
        return -1;
    }
    *pwc = *s;
    return *s != 0;
}
