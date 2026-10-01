// NEARMISS func_00102870  (vram 0x00102870, 0x20 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written SDK VU0 macro code (vector-unit loads, lane arithmetic, quadword stores); no C form
// reproduces it, the C states the arithmetic.
//
// The function links from the asm body in src/func_00102870.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SDK VU0: dst.xyz = src.xyz / d (through the Q register), dst.w = src.w.
// Hand-written VU0 macro code in the original; the C states the arithmetic.

void func_00102870(float *dst, float *src, float d) {
    float q = 1.0f / d;

    dst[0] = src[0] * q;
    dst[1] = src[1] * q;
    dst[2] = src[2] * q;
    dst[3] = src[3];
}
