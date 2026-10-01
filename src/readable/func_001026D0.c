// NEARMISS func_001026D0  (vram 0x001026D0, 0x44 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written SDK VU0 macro code (vector-unit loads, lane arithmetic, quadword stores); no C form
// reproduces it, the C states the arithmetic.
//
// The function links from the asm body in src/func_001026D0.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SDK VU0: 4x4 matrix product dst = a * b: output row j combines the rows
// of a by row j of b (a.row0 * b[j].x + ... + a.row3 * b[j].w); a is read
// in full first, so dst may alias a or b.
// Hand-written SDK VU0 macro code in the original; the C states the
// arithmetic in single-precision floats (lane order x, y, z, w).

void func_001026D0(float *dst, float *a, float *b) {
    float l[16];
    float r[4];
    int i;
    int j;

    for (i = 0; i < 16; i++) {
        l[i] = a[i];
    }
    for (j = 0; j < 4; j++) {
        for (i = 0; i < 4; i++) {
            r[i] = l[i] * b[j * 4] + l[4 + i] * b[j * 4 + 1] + l[8 + i] * b[j * 4 + 2] + l[12 + i] * b[j * 4 + 3];
        }
        for (i = 0; i < 4; i++) {
            dst[j * 4 + i] = r[i];
        }
    }
}
