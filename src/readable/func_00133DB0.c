// NEARMISS func_00133DB0  (vram 0x00133DB0, 0x78 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 88.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// the original tests self+0x54 with a branch over an
// unconditional branch to the exit and fills the last compare's delay slot with the
// constant 8; mwcc 2.3.3 folds the empty exit into one branch (empty-then, goto,
// switch, int-return spellings and mwcc 991202 / 2.4 measured: best 89.33 on
// 991202).
//
// The function links from the asm body in src/func_00133DB0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Unless bit 7 of self+0xD is set, ent+0x5C or ent+0x61 is nonzero or ent+0x56 is 0:
// with self+4 == 1, the halfword self+0x54 nonzero and self+5 below 3, self+5 = 8 and
// self+6 = 0.
void func_00133DB0(unsigned char *self, unsigned char *ent) {
    if (!(self[0xD] & 0x80) && *(unsigned short *)(ent + 0x5C) == 0 && ent[0x61] == 0 &&
        *(unsigned short *)(ent + 0x56) != 0 && self[4] == 1) {
        if (*(short *)(self + 0x54) == 0) {
            return;
        }
        if (self[5] < 3) {
            self[5] = 8;
            self[6] = 0;
        }
    }
}
