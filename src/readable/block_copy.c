// NEARMISS block_copy  (vram 0x00121870, 0xB0 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 54.86% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// EE-optimised newlib memcpy: the original's quadword / doubleword loops are scheduled and counted
// differently by ee-gcc from this C; semantics identical.
//
// The function links from the asm body in src/block_copy.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libc: memcpy (EE-optimised). When both pointers are 16-byte aligned and
// at least 32 bytes remain, it copies two quadwords per step, then
// doublewords while 8 or more bytes remain, then the rest byte by byte.
// Returns dst.
typedef unsigned int u128 __attribute__((mode(TI)));

void *block_copy(void *dst0, const void *src0, unsigned int len) {
    char *dst = dst0;
    const char *src = src0;
    u128 *qd;
    const u128 *qs;
    long long *dd;
    const long long *ds;

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
