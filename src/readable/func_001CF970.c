// NEARMISS func_001CF970  (vram 0x001CF970, 0xE4 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written VU0 macro code (vector subtract, lane multiply, absolute value and a divide through
// the Q register, with an asm loop of 3-lane rotations); no C form, the C states the arithmetic.
//
// The function links from the asm body in src/func_001CF970.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Shadow-decal clipper: the point where edge a -> b crosses the clip plane
// (port SHADOW_DECAL.md: em_shadow_decal_001CF970). Each vertex record is
// five quadwords: four attribute quadwords (+0x00..+0x30, the last one the
// texture coordinates) and the clip position (+0x40, x y z w). The plane is
// coordinate axis (0..2) = sign * w. The original is hand-written VU0 macro
// code: da = |a.clip - sign * a.w| and db likewise for b (all four lanes),
// the low three lanes rotated axis times (prot3w) so lane x holds the axis,
// t = |da / (db + da)| through the Q register, then every quadword of out
// = a + (b - a) * t, the clip position stored last.
extern float func_0011DF78(float x);

void func_001CF970(float *out, float *a, float *b, int axis, float sign) {
    float da;
    float db;
    float t;
    int i;

    da = func_0011DF78(a[16 + axis] - sign * a[19]);
    db = func_0011DF78(b[16 + axis] - sign * b[19]);
    t = func_0011DF78(da / (db + da));
    for (i = 0; i < 16; i++) {
        out[i] = a[i] + (b[i] - a[i]) * t;
    }
    for (i = 16; i < 20; i++) {
        out[i] = a[i] + (b[i] - a[i]) * t;
    }
}
