// NEARMISS func_001027E0  (vram 0x001027E0, 0x6C bytes) — readable companion C, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written SDK VU0 macro code (vector-unit loads, lane arithmetic, quadword stores); no C form
// reproduces it, the C states the arithmetic.
//
// The function links from the asm body in src/func_001027E0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SDK VU0 / MMI: inverse of a rigid transform. The 3x3 rotation block is
// transposed (its fourth column becomes 0, 0, 0 from the zeroed row-3
// lanes) and row 3 becomes (0, 0, 0, w) - t * R^T for the translation t =
// src row 3 xyz, keeping its w.
// Hand-written SDK VU0 macro code in the original; the C states the
// arithmetic in single-precision floats (lane order x, y, z, w).

void func_001027E0(float *dst, float *src) {
    float r[12];
    float t[4];
    int i;
    int j;

    for (i = 0; i < 4; i++) {
        t[i] = src[12 + i];
    }
    for (j = 0; j < 3; j++) {
        for (i = 0; i < 3; i++) {
            r[j * 4 + i] = src[i * 4 + j];
        }
        r[j * 4 + 3] = 0.0f;
    }
    for (i = 0; i < 12; i++) {
        dst[i] = r[i];
    }
    for (i = 0; i < 3; i++) {
        dst[12 + i] = 0.0f - (r[i] * t[0] + r[4 + i] * t[1] + r[8 + i] * t[2]);
    }
    dst[15] = t[3];
}
