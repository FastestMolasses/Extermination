// NEARMISS func_0011B910  (vram 0x0011B910, 0x24 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 82.22% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// The original puts the last store in the return's delay slot; with a volatile access ee-gcc keeps
// it before the return.
//
// The function links from the asm body in src/func_0011B910.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK: resets VIF1 (VIF1_FBRST, 0x10003C10, = 1) and sets VIF1_ERR
// (0x10003C20) = 6 (mask the DMAtag-mismatch and VIFcode errors).
void func_0011B910(void) {
    *(volatile int *)0x10003C10 = 1;
    *(volatile int *)0x10003C20 = 6;
}
