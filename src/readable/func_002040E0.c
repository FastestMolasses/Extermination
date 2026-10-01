// NEARMISS func_002040E0  (vram 0x002040E0, 0x5C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 33.48% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written interrupt-masked section (di / COP0 Status poll / ei): no C form; the C states the
// register writes.
//
// The function links from the asm body in src/func_002040E0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Writes DMA channel 3 (fromIPU) CHCR = chcr with the DMA controller suspended.
// Hand-written in the original: interrupts are disabled first (retrying
// until the COP0 Status interrupt-enable bit 16 reads clear), D_ENABLEW
// (0x1000F590) = D_ENABLER (0x1000F520) | 0x10000 suspends all channels,
// the CHCR (0x1000B000) is written, D_ENABLEW = D_ENABLER & ~0x10000 resumes them,
// and interrupts are enabled again on return. The C below states the
// register writes only; the interrupt masking has no C form.
void func_002040E0(int chcr) {
    *(volatile unsigned int *)0x1000F590 = *(volatile unsigned int *)0x1000F520 | 0x10000;
    *(volatile int *)0x1000B000 = chcr;
    *(volatile unsigned int *)0x1000F590 = *(volatile unsigned int *)0x1000F520 & ~0x10000;
}
