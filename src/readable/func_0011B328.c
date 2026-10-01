// NEARMISS func_0011B328  (vram 0x0011B328, 0x14 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 68.00% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// The original puts the last store in the return's delay slot; with a volatile access ee-gcc keeps
// it before the return (non-volatile spellings measured lower).
//
// The function links from the asm body in src/func_0011B328.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK: resets the GIF (GIF_CTRL, 0x10003000, = 1).
void func_0011B328(void) {
    *(volatile int *)0x10003000 = 1;
}
