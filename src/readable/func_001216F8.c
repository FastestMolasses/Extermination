// NEARMISS func_001216F8  (vram 0x001216F8, 0xE0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 10.41% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// EE-optimised newlib memchr (MMI quadword byte compare); the C is the plain byte loop with the
// same result.
//
// The function links from the asm body in src/func_001216F8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libc: memchr (EE-optimised). On a 16-byte aligned start it scans whole
// quadwords (MMI byte compare against the replicated byte) while 16 or more
// bytes remain, then byte by byte. Returns the first match or null.
void *func_001216F8(const void *s, int c, unsigned int n) {
    const unsigned char *p = s;

    while (n--) {
        if (*p == (unsigned char)c) {
            return (void *)p;
        }
        p++;
    }
    return 0;
}
