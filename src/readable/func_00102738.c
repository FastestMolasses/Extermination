// NEARMISS func_00102738  (vram 0x00102738, 0x24 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 24.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written SDK VU0 macro code (lane multiply and two lane adds); the C states the dot product.
//
// The function links from the asm body in src/func_00102738.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SDK VU0: the xyz dot product a.x * b.x + a.y * b.y + a.z * b.z.
// Hand-written SDK VU0 macro code in the original; the C states the
// arithmetic in single-precision floats (lane order x, y, z, w).

float func_00102738(float *a, float *b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
