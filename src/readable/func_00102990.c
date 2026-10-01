// NEARMISS func_00102990  (vram 0x00102990, 0x10 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written SDK VU0 macro code (vector-unit loads, lane arithmetic, quadword stores); no C form
// reproduces it, the C states the arithmetic.
//
// The function links from the asm body in src/func_00102990.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SDK VU0: dst = (int)src lane by lane (vftoi0: truncation toward zero, the
// VU saturates out-of-range values instead of producing the FPU's result).
// Hand-written VU0 macro code in the original; the C states the arithmetic.

void func_00102990(int *dst, float *src) {
    dst[0] = (int)src[0];
    dst[1] = (int)src[1];
    dst[2] = (int)src[2];
    dst[3] = (int)src[3];
}
