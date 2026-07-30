// NEARMISS func_001D8C30  (vram 0x001D8C30, 0x394 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.45% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 89.45 (mwcc233 -O4,p -sdatathreshold 4; 233 instrs). The jr-table dispatch is FULLY byte-identical including the jtbl_0026E520 reloc (objdiff accepts the local `@14` vs `jtbl_0026E520` .rodata symbol), the prologue, all 28 zero-stores in both clearing cases, and every swc1/lwc1/copy_qw4 call site...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

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
extern void copy_qw4();

void func_001D8C30(int mode, int *m, float *out, float *in)
{
    float t;

    t = in[3] - 1.0f;
    if (t <= 0.0f) {
        t = 0.0f;
    }

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
        out[12] = 8388608.0f + (128.0f + in[0]);
        out[13] = 8388608.0f + (128.0f + in[1]);
        out[14] = 8388608.0f + (128.0f + in[2]);
        out[15] = 8388608.0f + (64.0f * t);
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
        out[12] = 8388608.0f + in[0];
        out[13] = 8388608.0f + in[1];
        out[14] = 8388608.0f + in[2];
        out[15] = 8388608.0f + (64.0f * t);
        break;
    case 3:
        out[0] = 8388608.0f + (128.0f * in[0]);
        out[1] = 8388608.0f + (128.0f * in[1]);
        out[2] = 8388608.0f + (128.0f * in[2]);
        out[3] = 8388608.0f + (64.0f * t);
        break;
    case 4:
        copy_qw4(m, D_00275670 + 0x2380);
        out[0] = 8388608.0f + (128.0f * in[0]);
        out[1] = 8388608.0f + (128.0f * in[1]);
        out[2] = 8388608.0f + (128.0f * in[2]);
        out[3] = 8388672.0f;
        break;
    case 5:
        copy_qw4(m, D_00275670 + 0x2380);
        out[0] = 8388608.0f + (128.0f * in[0]);
        out[1] = 8388608.0f + (128.0f * in[1]);
        out[2] = 8388608.0f + (128.0f * in[2]);
        out[3] = 0.2f * (128.0f * in[3]);
        break;
    case 6:
        copy_qw4(m, D_00275670 + 0x2380);
        out[0] = 8388608.0f + (128.0f * in[0]);
        out[1] = 8388608.0f + (128.0f * in[1]);
        out[2] = 8388608.0f + (128.0f * in[2]);
        out[3] = 8388608.0f + (128.0f * in[3]);
        break;
    }
}
