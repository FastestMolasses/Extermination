// NEARMISS func_00123168  (vram 0x00123168, 0x114 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 9.13% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// EE-optimised libc strcpy (doubleword and MMI quadword copy with a zero-byte test); the C is the
// plain byte loop with the same result.
//
// The function links from the asm body in src/func_00123168.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libc: strcpy. The original is the EE-optimised version: when both
// pointers are 8-byte aligned it copies whole doublewords (16-byte aligned:
// quadwords, with MMI byte subtracts for the zero test) until a word holds
// the terminator, then finishes byte by byte. Returns dst.
char *func_00123168(char *dst, const char *src) {
    char *d = dst;

    while ((*d++ = *src++) != 0) {
    }
    return dst;
}
