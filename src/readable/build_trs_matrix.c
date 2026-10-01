// NEARMISS build_trs_matrix  (vram 0x001C94B0, 0xB8 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 3.15% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) when compiled on its own. Object
// similarity does not prove semantic equivalence. Remaining differences:
// Handwritten VU0 section: the original scales the rotation rows with VU0 macro instructions
// (quadword loads, lane-broadcast multiplies, quadword stores) between the C calls; C can only
// express it as FPU multiplies, which changes the whole middle of the function.
//
// The function links from the asm body in src/build_trs_matrix.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Builds a bone's local matrix m (4x4 floats, rows of four) from a
// translation, Euler rotation and scale: identity (func_001029C0), rotate
// about X, Y, then Z by rot[0..2] (func_00102B08, func_00102BB0,
// func_00102A60, each m = R * m), scale the first three rows' xyz by
// scale[0..2] (done in VU0 macro mode in the original: one lane-broadcast
// multiply per row on rows 0..2, row 3 stored back unchanged), then translate by pos
// (func_00102918).
extern void func_001029C0(float *m);
extern void func_00102B08(float *dst, float *src, float angle);
extern void func_00102BB0(float *dst, float *src, float angle);
extern void func_00102A60(float *dst, float *src, float angle);
extern void func_00102918(float *dst, float *src, float *v);

void build_trs_matrix(float *m, float *pos, float *rot, float *scale) {
    func_001029C0(m);
    func_00102B08(m, m, rot[0]);
    func_00102BB0(m, m, rot[1]);
    func_00102A60(m, m, rot[2]);
    m[0] *= scale[0];
    m[1] *= scale[0];
    m[2] *= scale[0];
    m[4] *= scale[1];
    m[5] *= scale[1];
    m[6] *= scale[1];
    m[8] *= scale[2];
    m[9] *= scale[2];
    m[10] *= scale[2];
    func_00102918(m, m, pos);
}
