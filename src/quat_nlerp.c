// NEARMISS quat_nlerp (vram 0x001CA0A0, 0x114 bytes) — readable C, not byte-identical.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Measured 99.05797% with mwcc 2.3.3; compiled symbol size is exactly 276
// bytes. Raw comparison differs at only 0x14/0x18: the compiler duplicates
// the second 1.0f LUI into the first branch's delay slot and advances that
// branch target by four bytes. The other 67 instructions match exactly.
// The original assembly remains the linked implementation until this last
// scheduling difference is resolved. mwcc991202 measured 92.46377%/264 bytes.
// A later bounded search also reproduced the same residual with mwcc 2.4
// and alternate readable clamp/control-flow forms. See docs/QUATERNION_MATCH.md.
//
// Despite the historical name, this does NOT normalize the result or use
// spherical interpolation. It clamps only the upper blend bound, computes
// the dot product, and negates the earlier quaternion for opposite signs.
// anim_eval_skeleton passes this result directly to quat_to_mat3. Preserving
// the non-unit result matters for the original cinematic bone transforms.
void quat_nlerp(float *out, const float *a, const float *b, float t)
{
    float inverse;

    if (!(t <= 1.0f)) t = 1.0f;
    inverse = 1.0f - t;
    if (((a[0]*b[0] + a[1]*b[1]) + a[2]*b[2]) + a[3]*b[3] < 0.0f) {
        out[0] = b[0]*t - a[0]*inverse;
        out[1] = b[1]*t - a[1]*inverse;
        out[2] = b[2]*t - a[2]*inverse;
        out[3] = b[3]*t - a[3]*inverse;
    } else {
        out[0] = a[0]*inverse + b[0]*t;
        out[1] = a[1]*inverse + b[1]*t;
        out[2] = a[2]*inverse + b[2]*t;
        out[3] = a[3]*inverse + b[3]*t;
    }
}
