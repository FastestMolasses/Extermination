// NEARMISS func_001DEE80  (vram 0x001DEE80, 0x34 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 78.54% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// the original keeps the source pointer in a1 across the call to
// func_001DEDB0 (a leaf of the same translation unit that does not touch a1), so it needs
// no frame slot for it; compiled on its own, mwcc must assume the call clobbers a1 and
// saves it in s0 (larger frame). Same intra-TU register analysis wall as func_00205F90 /
// func_001CC170 (docs/FIRST_LEVEL_DECOMP.md section 4).
//
// The function links from the asm body in src/func_001DEE80.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Copies the three words at src into the record func_001DEDB0(which) returns, at
// +0x10, +0x14 and +0x18.
extern char *func_001DEDB0(int which);

void func_001DEE80(int which, int *src) {
    char *r = func_001DEDB0(which);
    *(int *)(r + 0x10) = src[0];
    *(int *)(r + 0x14) = src[1];
    *(int *)(r + 0x18) = src[2];
}
