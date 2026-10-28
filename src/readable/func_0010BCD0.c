// NEARMISS func_0010BCD0  (vram 0x0010BCD0, 0x60 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 95.375% via ee-gcc 2.9-991111-01 (-O2 -fno-schedule-insns) when compiled on its own
// (was 49.50%; matching wave 2026-10-28). Object similarity does not prove semantic equivalence.
// What fixed most of it: the flag and count words the kernel writes are volatile (re-read each
// pass, count read after the final clear), and -fno-schedule-insns gives the original's order
// around the first INTC_STAT clear. Remaining differences: the original reads INTC_STAT inside
// the loop through a fresh constant address every pass, while ee-gcc keeps the address of the
// first clear in a register across the loop (goto, do/while, for and while spellings and the
// CSE/GCSE flags were tried; none drops the hoist).
//
// The function links from the asm body in src/func_0010BCD0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2 -fno-schedule-insns

// SDK: waits for the next vertical blank. SetVSyncFlag(&flag, &count)
// registers two words the kernel updates on vblank; the INTC_STAT VBLANK
// start bit (0x1000F000, bit 2) is cleared, then the loop runs until the
// bit is set again or the flag word turns non-zero; the bit is cleared once
// more and the vblank count is returned.
#define INTC_STAT ((volatile unsigned int *)(0x1000f000))
extern void SetVSyncFlag(int *flag, long *count);

long func_0010BCD0(void) {
    volatile int flag;
    volatile long count;

    SetVSyncFlag((int *)&flag, (long *)&count);
    flag = 0;
    *INTC_STAT = 4;
    while (!(*INTC_STAT & 4) && flag == 0)
        ;
    *INTC_STAT = 4;
    return count;
}
