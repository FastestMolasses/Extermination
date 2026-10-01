// NEARMISS func_0010E270  (vram 0x0010E270, 0xA4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 55.56% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written interrupt-masked section (di / COP0 Status poll / ei on every exit); the C states
// the pool search.
//
// The function links from the asm body in src/func_0010E270.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK (SIF RPC): takes a free 0x40-byte packet from the pool (pool+4 the
// packets, pool+8 their count). With interrupts masked the first packet
// whose busy bit (+0x10 bit 0) is clear is marked (index << 16 | 5), gets
// the pool's next request id (pool+0, skipping 0) at +0x18 and its own
// address at +0x14, and is returned; null when all are busy.
// Hand-written in the original: interrupts are disabled first (retrying
// until the COP0 Status interrupt-enable bit 16 reads clear) and enabled
// again afterwards (only when they were enabled on entry); the masking has
// no C form, the C states the work.
char *func_0010E270(int *pool) {
    int n = pool[2];
    char *p = (char *)pool[1];
    int i;
    int id;

    for (i = 0; i < n; i++, p += 0x40) {
        if (!(*(int *)(p + 0x10) & 1)) {
            *(int *)(p + 0x10) = (i << 16) | 5;
            id = ++pool[0];
            if (id == 1) {
                id = 1;
                pool[0] = 2;
            }
            *(int *)(p + 0x18) = id;
            *(char **)(p + 0x14) = p;
            return p;
        }
    }
    return 0;
}
