// NEARMISS func_00125F48  (vram 0x00125F48, 0x60 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 72.29% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc register allocation and schedule of the 32-bit partial products (the original keeps the
// multiply-add halves in the second multiplier).
//
// The function links from the asm body in src/func_00125F48.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// libgcc: 64-bit multiply (__muldi3). The low words multiply to a full
// 64-bit product; the cross products low(u) * high(v) + high(u) * low(v)
// are added to its high word.
typedef union DWords {
    long long ll;
    struct {
        unsigned int low;
        int high;
    } s;
} DWords;

long long func_00125F48(long long u, long long v) {
    DWords w;
    DWords uu;
    DWords vv;

    uu.ll = u;
    vv.ll = v;
    w.ll = (unsigned long long)uu.s.low * (unsigned long long)vv.s.low;
    w.s.high += (unsigned int)uu.s.low * (unsigned int)vv.s.high
              + (unsigned int)uu.s.high * (unsigned int)vv.s.low;
    return w.ll;
}
