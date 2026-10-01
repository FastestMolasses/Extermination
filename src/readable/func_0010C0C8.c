// NEARMISS func_0010C0C8  (vram 0x0010C0C8, 0x74 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 14.14% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written interrupt-masked wrapper (di / COP0 Status poll / ei); the C states the call only.
//
// The function links from the asm body in src/func_0010C0C8.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK kernel: invalidates the data cache over [start, end] (func_0010C020
// on the 64-byte aligned bounds) with interrupts masked.
// Hand-written in the original: interrupts are disabled first (retrying
// until the COP0 Status interrupt-enable bit 16 reads clear) and enabled
// again afterwards (only when they were enabled on entry); the masking has
// no C form, the C states the work.
extern void func_0010C020(unsigned int start, unsigned int end);

void func_0010C0C8(unsigned int start, unsigned int end) {
    func_0010C020(start & 0xFFFFFFC0, end & 0xFFFFFFC0);
}
