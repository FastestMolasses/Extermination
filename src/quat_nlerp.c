// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 99.06% NEARMISS).
// Fixes read from the original instructions:
//  - The upper clamp on the blend factor is a ternary
//    (`t = (t > 1.0f) ? 1.0f : t;`), not an if statement. With the if
//    statement the compiler moved the second 1.0f constant setup into the
//    clamp branch's delay slot; the ternary keeps the slot empty like the
//    original.
//
// Despite the historical name, this does NOT normalize the result or use
// spherical interpolation. It clamps only the upper blend bound, computes
// the dot product, and negates the earlier quaternion for opposite signs.
// anim_eval_skeleton passes this result directly to quat_to_mat3. Preserving
// the non-unit result matters for the original cinematic bone transforms.
void quat_nlerp(float *out, const float *a, const float *b, float t)
{
    float inverse;

    t = (t > 1.0f) ? 1.0f : t;
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
