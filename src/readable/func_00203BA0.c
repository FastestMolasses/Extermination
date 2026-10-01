// NEARMISS func_00203BA0  (vram 0x00203BA0, 0x8C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 72.57% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written interrupt-masked section (di / COP0 Status poll / ei): no C form; the C states the
// data updates.
//
// The function links from the asm body in src/func_00203BA0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Movie frame ring: marks the slot at the write cursor filled. Hand-written
// in the original inside an interrupt-disabled section (interrupts disabled,
// retrying until the COP0 Status interrupt-enable bit 16 reads clear;
// enabled again on return): the first word of slot r[2] (0x39640-byte slots
// at r[1]) becomes 2, the filled count r[3] grows by one and the cursor
// advances modulo the slot count r[4]. The C states the data updates only.
void func_00203BA0(int *r) {
    *(int *)(r[1] + r[2] * 0x39640) = 2;
    r[3]++;
    r[2] = (r[2] + 1) % r[4];
}
