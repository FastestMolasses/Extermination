// NEARMISS func_0011DF78  (vram 0x0011DF78, 0x1C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 85.00% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc moves the float word through an extra register (union and pointer spellings measured).
//
// The function links from the asm body in src/func_0011DF78.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libm (newlib): fabsf, the float with its sign bit cleared.
typedef union {
    float f;
    int i;
} FloatBits;

float func_0011DF78(float x) {
    FloatBits b;

    b.f = x;
    b.i &= 0x7FFFFFFF;
    return b.f;
}
