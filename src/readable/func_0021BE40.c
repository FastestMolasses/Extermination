// NEARMISS func_0021BE40  (vram 0x0021BE40, 0x90 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 90.83% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// branch layout of the float test: the original branches to a
// separate return-1 block when self+0x220 <= 0.0 and loads the float into f1; mwcc
// 2.3.3 inverts the test into a skip over an unconditional branch and uses f2 (nested,
// else-if, result-variable and five compare spellings measured).
//
// The function links from the asm body in src/func_0021BE40.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Returns 0 when every test passes, else 1: the scratchpad byte 0x70003B8D is 0,
// self+0x220 is not <= 0.0, self+0 == 1, self+4 == 1, func_0021BB00(self) returns 0 and
// the halfword self+0x20E is 0.
extern unsigned char D_70003B8D;
extern int func_0021BB00(char *p);

int func_0021BE40(char *p) {
    if (D_70003B8D != 0 || *(float *)(p + 0x220) <= 0.0f) {
        return 1;
    } else if (*(unsigned char *)(p + 0) != 1 || *(unsigned char *)(p + 4) != 1 ||
               func_0021BB00(p) != 0 || *(short *)(p + 0x20E) != 0) {
        return 1;
    } else {
        return 0;
    }
}
