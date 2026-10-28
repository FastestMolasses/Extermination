// NEARMISS func_0010BC90  (vram 0x0010BC90, 0x40 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.00% via ee-gcc 2.9-991111-01 (-O2) (was an asm-word body, matching wave 2026-10-28).
// Remaining differences: the original reads INTC_STAT in the poll loop and does the final store
// with a direct constant address (the assembler's high-half-plus-negative-offset form, the loop
// padded by three assembler nops); ee-gcc keeps the address in a register, hoisted out of the
// loop. A goto loop with -fno-gcse reaches 73.00% (final store direct, loop still register
// form); -fno-schedule-insns 71.19%; -O1 and other CSE flags no better.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// Vertical-blank wait (SDK libgraph, used by the vsync helper func_001009C8):
// acknowledges the VBLANK-start interrupt flag (bit 2 of INTC_STAT, 0x1000F000;
// writing 1 clears it), spins until the flag is raised again, then acknowledges it.

#define INTC_STAT (*(volatile unsigned int *)0x1000F000)
#define INTC_VBLANK_S 4

void func_0010BC90(void) {
    INTC_STAT = INTC_VBLANK_S;
    while ((INTC_STAT & INTC_VBLANK_S) == 0) {
    }
    INTC_STAT = INTC_VBLANK_S;
}
