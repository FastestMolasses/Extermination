// NEARMISS func_0010BCD0  (vram 0x0010BCD0, 0x60 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 49.50% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc layout of the wait loop (the original tests the register bit first and the flag word
// second with branch-likely-free slots).
//
// The function links from the asm body in src/func_0010BCD0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK: waits for the next vertical blank. SetVSyncFlag(&flag, &count)
// registers two words the kernel updates on vblank; the INTC_STAT VBLANK
// start bit (0x1000F000, bit 2) is cleared, then the loop runs until the
// bit is set again or the flag word turns non-zero; the bit is cleared once
// more and the vblank count is returned.
extern void SetVSyncFlag(int *flag, long long *count);

long long func_0010BCD0(void) {
    int flag;
    long long count;

    SetVSyncFlag(&flag, &count);
    flag = 0;
    *(volatile int *)0x1000F000 = 4;
    while (!(*(volatile int *)0x1000F000 & 4) && flag == 0) {
    }
    *(volatile int *)0x1000F000 = 4;
    return count;
}
