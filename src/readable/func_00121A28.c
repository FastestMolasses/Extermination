// NEARMISS func_00121A28  (vram 0x00121A28, 0xC0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 60.40% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// EE-optimised newlib memset (the byte replicated across a quadword with MMI copies, stored two
// quadwords per step); the C uses doublewords; semantics identical.
//
// The function links from the asm body in src/func_00121A28.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libc: memset (EE-optimised). On a 16-byte aligned destination with 8 or
// more bytes, the byte is replicated across a quadword (MMI halfword /
// doubleword copies) and stored two quadwords at a time while 32 or more
// bytes remain, then doublewords; the rest byte by byte. Returns dst.
void *func_00121A28(void *dst0, int c, unsigned int len) {
    unsigned char *dst = dst0;
    unsigned long long v;
    long long *d;

    if (len >= 8 && !((int)dst & 15)) {
        v = (unsigned char)c;
        v |= v << 8;
        v |= v << 16;
        v |= v << 32;
        d = (long long *)dst;
        while (len >= 32) {
            d[0] = v;
            d[1] = v;
            d[2] = v;
            d[3] = v;
            d += 4;
            len -= 32;
        }
        while (len >= 8) {
            *d++ = v;
            len -= 8;
        }
        dst = (unsigned char *)d;
    }
    while (len--) {
        *dst++ = (unsigned char)c;
    }
    return dst0;
}
