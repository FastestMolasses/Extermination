// NEARMISS func_00102918  (vram 0x00102918, 0x2C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written SDK VU0 macro code (vector-unit loads, lane arithmetic, quadword stores); no C form
// reproduces it, the C states the arithmetic.
//
// The function links from the asm body in src/func_00102918.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SDK VU0 / MMI: translate a matrix: dst rows 0..2 = src rows 0..2 (raw
// quadword copies), dst row 3 xyz = src row 3 xyz + v xyz, w unchanged.
// Hand-written SDK VU0 macro code in the original; the C states the
// arithmetic in single-precision floats (lane order x, y, z, w).

void func_00102918(float *dst, float *src, float *v) {
    int i;

    for (i = 0; i < 12; i++) {
        dst[i] = src[i];
    }
    dst[12] = src[12] + v[0];
    dst[13] = src[13] + v[1];
    dst[14] = src[14] + v[2];
    dst[15] = src[15];
}
