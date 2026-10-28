// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.45% NEARMISS).
// Fixes read from the original instructions:
//  - The clamp is a ternary that keeps t when positive, `(t > 0.0f) ? t : 0.0f`,
//    not `if (t <= 0.0f) t = 0.0f;` (gives the original's branch-around shape
//    with the jump-table bound test in both slots).
//  - The 8388608.0f float->fixed bias is a file-scope `static const float`, not
//    a literal. The named constant is still folded (no data is emitted) but its
//    setup is scheduled ahead of the input loads, as in the original, which
//    fixed every case's constant order and FP register choice.

// SEMANTICS: builds the GS "UV / XYZ" register payload for one sprite-ish
//   primitive.  Parameters: `mode` selects the packing style, `m` is a 4x4
//   float matrix slot (16 words), `out` is the 64-byte vertex/register block
//   being filled, `in` is the source vec4 {x, y, z, w}.
//   `t` = clamp(in.w - 1.0f, 0, +inf) -- a "extra scale/alpha" term that is
//   only ever used for out[15] / out[3].
//   Common constants: 8388608.0f (0x4B000000) is the classic PS2
//   float->fixed bias, 128.0f and 64.0f are the sub-pixel scales.
//     mode 0/1 and anything >= 7 : zero the whole 4x4 matrix column-major
//         (m[0],m[4],m[8],m[12], m[1],...), zero out[0..11] (each group of 4
//         written high-index-first), then bias-encode in.xyz with a +128.0f
//         offset into out[12..14] and 64*t into out[15].
//     mode 2 : same clearing, but the +128.0f offset is dropped.
//     mode 3 : no clearing; out[0..2] = bias + 128*in.xyz, out[3] = bias + 64*t.
//     mode 4 : copy the shared 4-quadword template at D_00275670+0x2380 into
//         m, encode in.xyz into out[0..2], and store the constant
//         8388672.0f (bias + 64) into out[3].
//     mode 5 : as mode 4, but out[3] = 0.2f * (128 * in.w)  (unbiased).
//     mode 6 : as mode 4, but out[3] = bias + 128 * in.w.

extern int D_00275670;
static const float bias = 8388608.0f;
extern void copy_qw4();

void func_001D8C30(int mode, int *m, float *out, float *in)
{
    float t;

    t = in[3] - 1.0f;
    t = (t > 0.0f) ? t : 0.0f;

    switch (mode) {
    case 0:
    case 1:
    default:
        m[0] = 0;
        m[4] = 0;
        m[8] = 0;
        m[12] = 0;
        m[1] = 0;
        m[5] = 0;
        m[9] = 0;
        m[13] = 0;
        m[2] = 0;
        m[6] = 0;
        m[10] = 0;
        m[14] = 0;
        m[3] = 0;
        m[7] = 0;
        m[11] = 0;
        m[15] = 0;
        out[3] = 0.0f;
        out[2] = 0.0f;
        out[1] = 0.0f;
        out[0] = 0.0f;
        out[7] = 0.0f;
        out[6] = 0.0f;
        out[5] = 0.0f;
        out[4] = 0.0f;
        out[11] = 0.0f;
        out[10] = 0.0f;
        out[9] = 0.0f;
        out[8] = 0.0f;
        out[12] = bias + (128.0f + in[0]);
        out[13] = bias + (128.0f + in[1]);
        out[14] = bias + (128.0f + in[2]);
        out[15] = bias + (64.0f * t);
        break;
    case 2:
        m[0] = 0;
        m[4] = 0;
        m[8] = 0;
        m[12] = 0;
        m[1] = 0;
        m[5] = 0;
        m[9] = 0;
        m[13] = 0;
        m[2] = 0;
        m[6] = 0;
        m[10] = 0;
        m[14] = 0;
        m[3] = 0;
        m[7] = 0;
        m[11] = 0;
        m[15] = 0;
        out[3] = 0.0f;
        out[2] = 0.0f;
        out[1] = 0.0f;
        out[0] = 0.0f;
        out[7] = 0.0f;
        out[6] = 0.0f;
        out[5] = 0.0f;
        out[4] = 0.0f;
        out[11] = 0.0f;
        out[10] = 0.0f;
        out[9] = 0.0f;
        out[8] = 0.0f;
        out[12] = bias + in[0];
        out[13] = bias + in[1];
        out[14] = bias + in[2];
        out[15] = bias + (64.0f * t);
        break;
    case 3:
        out[0] = bias + (128.0f * in[0]);
        out[1] = bias + (128.0f * in[1]);
        out[2] = bias + (128.0f * in[2]);
        out[3] = bias + (64.0f * t);
        break;
    case 4:
        copy_qw4(m, D_00275670 + 0x2380);
        out[0] = bias + (128.0f * in[0]);
        out[1] = bias + (128.0f * in[1]);
        out[2] = bias + (128.0f * in[2]);
        out[3] = 8388672.0f;
        break;
    case 5:
        copy_qw4(m, D_00275670 + 0x2380);
        out[0] = bias + (128.0f * in[0]);
        out[1] = bias + (128.0f * in[1]);
        out[2] = bias + (128.0f * in[2]);
        out[3] = 0.2f * (128.0f * in[3]);
        break;
    case 6:
        copy_qw4(m, D_00275670 + 0x2380);
        out[0] = bias + (128.0f * in[0]);
        out[1] = bias + (128.0f * in[1]);
        out[2] = bias + (128.0f * in[2]);
        out[3] = bias + (128.0f * in[3]);
        break;
    }
}
