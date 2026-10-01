// NEARMISS func_00101810  (vram 0x00101810, 0x88 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 49.26% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc 64-bit shift and sign-extension scheduling of the packed register (semantics follow the
// original; not iterated).
//
// The function links from the asm body in src/func_00101810.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libgraph: sets a draw environment's XYOFFSET so that the screen
// centre (cx, cy) maps to the middle of its scissor area. env+0x30 is the
// SCISSOR register (SCAX1 at bits 16..26, SCAY1 at bits 48..58); the offset
// register env+0x20 gets OFX = (cx - (SCAX1 + 1) / 2) * 16 and OFY =
// (cy - (SCAY1 + 1) / 2) * 16, plus a half pixel (8) on y when half is set
// (the odd field). The two copies serve the two draw contexts.
void func_00101810(char *env, short cx, short cy, int half) {
    unsigned long long sc = *(unsigned long long *)(env + 0x30);
    long long oy = cy - ((unsigned int)((int)((sc >> 48) & 0x7FF) + 1) >> 1);
    long long ox = cx - ((unsigned int)((int)((sc >> 16) & 0x7FF) + 1) >> 1);
    long long y;

    if ((short)half != 0) {
        y = (oy * 16 + 8) << 32;
    } else {
        y = oy << 36;
    }
    *(long long *)(env + 0x20) = (ox * 16) | y;
}
