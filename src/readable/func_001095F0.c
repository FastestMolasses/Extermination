// NEARMISS func_001095F0  (vram 0x001095F0, 0xA8 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 50.10% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written interrupt-masked section; the C states the DMA register updates and the tail call.
//
// The function links from the asm body in src/func_001095F0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK: stops the IPU DMA channels. With interrupts masked and the DMA
// controller suspended (D_ENABLEW = D_ENABLER | 0x10000), the STR bit
// (bit 8) of D3_CHCR (fromIPU, 0x1000B000) and D4_CHCR (toIPU, 0x1000B400)
// is cleared and the controller resumed; after the interrupts are enabled
// both QWC registers are zeroed and func_0010B160 is tail-called.
// Hand-written in the original: interrupts are disabled first (retrying
// until the COP0 Status interrupt-enable bit 16 reads clear) and enabled
// again afterwards (only when they were enabled on entry); the masking has
// no C form, the C states the work.
extern void func_0010B160(void);

void func_001095F0(void) {
    *(volatile unsigned int *)0x1000F590 = *(volatile unsigned int *)0x1000F520 | 0x10000;
    *(volatile unsigned int *)0x1000B000 &= ~0x100;
    *(volatile unsigned int *)0x1000B400 &= ~0x100;
    *(volatile unsigned int *)0x1000F590 = *(volatile unsigned int *)0x1000F520 & ~0x10000;
    *(volatile int *)0x1000B020 = 0;
    *(volatile int *)0x1000B420 = 0;
    func_0010B160();
}
