// NEARMISS func_0010C2F8  (vram 0x0010C2F8, 0x64 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 27.68% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written interrupt-masked wrapper (di / COP0 Status poll / sync / ei); the C states the call
// only.
//
// The function links from the asm body in src/func_0010C2F8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK kernel: calls _EnableIntc (the kernel's interrupt / DMA-channel mask service
// for cause cause) with interrupts masked, followed by a sync.
// Hand-written in the original: interrupts are disabled first (retrying
// until the COP0 Status interrupt-enable bit 16 reads clear) and enabled
// again afterwards (only when they were enabled on entry); the masking has
// no C form, the C states the work.
extern int _EnableIntc(int cause);

int func_0010C2F8(int cause) {
    return _EnableIntc(cause);
}
