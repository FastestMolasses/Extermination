// NEARMISS func_00205F90  (vram 0x00205F90, 0x7C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 100.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// The original keeps q in a0 across the call to the leaf func_00205F80 (intra-TU register
// analysis); reproducing it needs a static copy of the callee in this TU, whose extra .text the
// per-function link cannot take.
//
// The function links from the asm body in src/func_00205F90.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Ring-buffer slot address. q points at a ring record: q[1] base address,
// q[2] / q[3] the two cursors, q[4] the slot count; func_00205F80(q) is true
// when q[3] == 0 (empty ring), and then this returns 0. Otherwise it returns
// q[1] + ((q[4] + (q[2] - q[3])) % q[4]) * 0x39640 (slot stride 0x39640).
// The original keeps q in a0 across the call (it knows the leaf callee leaves
// a0 alone), so the callee is copied here as a static of the same TU.
static int func_00205F80(int *q) {
    return q[3] == 0;
}

int func_00205F90(int *q) {
    if (func_00205F80(q)) {
        return 0;
    }
    return q[1] + ((q[4] + (q[2] - q[3])) % q[4]) * 0x39640;
}
