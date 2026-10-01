// NEARMISS func_0011E080  (vram 0x0011E080, 0x24 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 28.89% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// ee-gcc materialises the mask before the move and orders the subtraction operands differently
// (union spelling only; the comparison semantics are exact).
//
// The function links from the asm body in src/func_0011E080.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK libm (newlib): isnanf, 1 when the magnitude bits exceed 0x7F800000.
typedef union {
    float f;
    int i;
} FloatBits;

int func_0011E080(float x) {
    FloatBits b;

    b.f = x;
    return (unsigned int)(0x7F800000 - (b.i & 0x7FFFFFFF)) >> 31;
}
