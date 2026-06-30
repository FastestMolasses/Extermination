// NEARMISS func_001CA0A0  (vram 0x001CA0A0, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.06% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Body byte-identical (insns 7-68). Sole residual: clamp branch delay-slot fill. Target leaves the bc1t delay slot a nop and reloads 1.0 via a fresh `lui 0x3f80` after the branch; mwcc (both builds) speculates/CSEs that `lui` into the delay slot and adjusts the branch target (bc1t 0x20 vs 0x24). Tr...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// 4-vector blend with dot-product sign select (quaternion-style lerp):
// clamp t to <=1, s=1-t, dot = sum(a[i]*b[i]); if dot<0 dst=b*t-a*s else dst=a*s+b*t.
// Body is byte-identical; sole residual is a delay-slot-fill/constant-rematerialize
// scheduling artifact (mwcc speculates the `lui 0x3f80` 1.0 reload into the clamp
// branch's delay slot; the target leaves a nop and reloads 1.0 separately).
void func_001CA0A0(float *dst, float *a, float *b, float t) {
    float s;
    float dot;

    if (!(t <= 1.0f)) {
        t = 1.0f;
    }
    s = 1.0f - t;
    dot = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
    if (dot < 0.0f) {
        dst[0] = b[0] * t - a[0] * s;
        dst[1] = b[1] * t - a[1] * s;
        dst[2] = b[2] * t - a[2] * s;
        dst[3] = b[3] * t - a[3] * s;
    } else {
        dst[0] = a[0] * s + b[0] * t;
        dst[1] = a[1] * s + b[1] * t;
        dst[2] = a[2] * s + b[2] * t;
        dst[3] = a[3] * s + b[3] * t;
    }
}
