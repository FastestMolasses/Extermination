// NEARMISS func_001DEEC0  (vram 0x001DEEC0, 0x20 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 66.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// the original keeps v in a1 across the call to the same-unit
// leaf func_001DEDB0; mwcc saves it in s0 (intra-TU register analysis wall, as
// func_001DEE80).
//
// The function links from the asm body in src/func_001DEEC0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. Later-level decomp lane (LDEC) 2026-10-01 (docs/LEVELS_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Stores v at +4 of the record func_001DEDB0(which) returns.
extern char *func_001DEDB0(int which);

void func_001DEEC0(int which, int v) {
    *(int *)(func_001DEDB0(which) + 4) = v;
}
