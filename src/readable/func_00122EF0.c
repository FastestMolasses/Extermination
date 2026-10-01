// NEARMISS func_00122EF0  (vram 0x00122EF0, 0x12C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 19.99% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// EE-optimised libc strcat (aligned doubleword / MMI quadword end search, then strcpy); the C is
// the plain loop with the same result.
//
// The function links from the asm body in src/func_00122EF0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libc: strcat. The original is the EE-optimised version: it finds the end
// of dst a doubleword (or MMI quadword) at a time from an aligned start,
// then appends src with func_00123168 (strcpy). Returns dst.
extern char *func_00123168(char *dst, const char *src);

char *func_00122EF0(char *dst, const char *src) {
    char *p = dst;

    while (*p != 0) {
        p++;
    }
    func_00123168(p, src);
    return dst;
}
