// NEARMISS func_00102718  (vram 0x00102718, 0x1C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written SDK VU0 macro code (vector-unit loads, lane arithmetic, quadword stores); no C form
// reproduces it, the C states the arithmetic.
//
// The function links from the asm body in src/func_00102718.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SDK VU0: cross product dst.xyz = a.xyz x b.xyz (an outer-product
// multiply-subtract pair), dst.w = 0.
// Hand-written SDK VU0 macro code in the original; the C states the
// arithmetic in single-precision floats (lane order x, y, z, w).

void func_00102718(float *dst, float *a, float *b) {
    float x = a[1] * b[2] - b[1] * a[2];
    float y = a[2] * b[0] - b[2] * a[0];
    float z = a[0] * b[1] - b[0] * a[1];

    dst[0] = x;
    dst[1] = y;
    dst[2] = z;
    dst[3] = 0.0f;
}
