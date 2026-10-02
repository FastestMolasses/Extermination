// NEARMISS func_00123020  (vram 0x00123020, 0x144 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 0.00% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// the original is hand-written EE code: the 16-byte path compares
// quadwords with the MMI parallel subtract / pack instructions and tests for a zero
// byte with a parallel byte subtract; ee-gcc compiles this C to ordinary 64-bit loads
// and its own schedule, so the objects share almost no instruction rows.
//
// The function links from the asm body in src/func_00123020.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// strcmp. When both pointers are 8-byte aligned it compares a doubleword at a time
// (16-byte aligned: a quadword, as two doublewords), returning 0 as soon as an equal
// chunk of a contains a zero byte; at the first differing chunk (or for unaligned
// strings) it compares bytes until a's byte is 0 or the bytes differ, and returns
// the difference of the two bytes as unsigned values.
typedef unsigned long long u64;

/* "Any byte of w is zero": (w - 0x01..01) & ~w & 0x80..80 is nonzero
 * exactly when some byte of w is zero. */
#define HAS_ZERO_BYTE(w) ((((w) - 0x0101010101010101ULL) & ~(w) & 0x8080808080808080ULL) != 0)

int func_00123020(const char *a, const char *b) {
    unsigned int both = (unsigned int)a | (unsigned int)b;
    if ((both & 7) == 0) {
        if ((both & 0xF) == 0) {
            /* 16-byte aligned: compare a quadword (two doublewords) at a time. */
            while (((const u64 *)a)[0] == ((const u64 *)b)[0] &&
                   ((const u64 *)a)[1] == ((const u64 *)b)[1]) {
                if (HAS_ZERO_BYTE(((const u64 *)a)[0]) || HAS_ZERO_BYTE(((const u64 *)a)[1])) {
                    return 0;
                }
                a += 16;
                b += 16;
            }
        } else {
            /* 8-byte aligned: compare a doubleword at a time. */
            while (*(const u64 *)a == *(const u64 *)b) {
                if (HAS_ZERO_BYTE(*(const u64 *)a)) {
                    return 0;
                }
                a += 8;
                b += 8;
            }
        }
    }
    while (*a != 0 && *a == *b) {
        a++;
        b++;
    }
    return *(const unsigned char *)a - *(const unsigned char *)b;
}
