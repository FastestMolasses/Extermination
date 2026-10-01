// NEARMISS func_00102948  (vram 0x00102948, 0xC bytes) — readable companion C, NOT byte-identical.
//
// objdiff 96.67% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written: the original moves the quadword through a2; any compiler picks v0.
//
// The function links from the asm body in src/func_00102948.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK: copies one 128-bit quadword, *dst = *src.
typedef unsigned int u128 __attribute__((mode(TI)));

void func_00102948(u128 *dst, u128 *src) {
    *dst = *src;
}
