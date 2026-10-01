// NEARMISS func_00121920  (vram 0x00121920, 0x104 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 51.82% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// EE-optimised newlib memmove (quadword / doubleword loops); ee-gcc schedules and counts this C's
// loops differently; semantics identical.
//
// The function links from the asm body in src/func_00121920.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libc: memmove (EE-optimised). An overlapping copy with src below dst
// copies backwards byte by byte; otherwise like memcpy (block_copy): two
// quadwords per step while 32 or more bytes remain on 16-byte aligned
// pointers, then doublewords, then bytes. Returns dst.
typedef unsigned int u128 __attribute__((mode(TI)));

void *func_00121920(void *dst0, const void *src0, unsigned int len) {
    char *dst = dst0;
    const char *src = src0;
    u128 *qd;
    const u128 *qs;
    long long *dd;
    const long long *ds;

    if (src < dst && dst < src + len) {
        src += len;
        dst += len;
        while (len--) {
            *--dst = *--src;
        }
        return dst0;
    }
    if (len >= 32 && !(((int)src | (int)dst) & 15)) {
        qd = (u128 *)dst;
        qs = (const u128 *)src;
        do {
            *qd++ = *qs++;
            *qd++ = *qs++;
            len -= 32;
        } while (len >= 32);
        dd = (long long *)qd;
        ds = (const long long *)qs;
        while (len >= 8) {
            *dd++ = *ds++;
            len -= 8;
        }
        dst = (char *)dd;
        src = (const char *)ds;
    }
    while (len--) {
        *dst++ = *src++;
    }
    return dst0;
}
