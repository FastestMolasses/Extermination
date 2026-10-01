// NEARMISS func_001232E0  (vram 0x001232E0, 0x138 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 11.33% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// EE-optimised libc strlen (doubleword and MMI quadword zero-byte scan); the C is the plain byte
// loop with the same result.
//
// The function links from the asm body in src/func_001232E0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libc: strlen. The original is the EE-optimised version: from an 8-byte
// aligned start it scans a doubleword (16-byte aligned: a quadword with MMI
// byte subtracts) at a time for a zero byte with the (x - 0x01..) & ~x &
// 0x80.. test, then finishes byte by byte. Semantically a plain count.
unsigned int func_001232E0(const char *s) {
    const char *p = s;

    while (*p != 0) {
        p++;
    }
    return p - s;
}
