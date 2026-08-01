// NEARMISS func_001D0540  (vram 0x001D0540, 0x11C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 52.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct { float x, y, z, w; } Vec4;

extern Vec4  D_70003660;        /* scratchpad working quad A */
extern Vec4  D_70003670;        /* scratchpad working quad B */
extern float func_0011DF78(float v);            /* fabsf */

/* lqc2 vf20..vf23 = m; vmulax/vmadday/vmaddaz/vmaddw; sqc2 -> *dst.
   COP2 macro mode, no C spelling. */
extern void vu0_mat4_mul_vec4(Vec4 *dst, const Vec4 *m, const Vec4 *v);

float func_001D0540(const float *pos, const Vec4 *m, float d)
{
    D_70003660.x = pos[0];
    D_70003660.y = pos[1];
    D_70003660.z = pos[2];
    D_70003660.w = 1.0f;

    D_70003670.x = pos[0];
    D_70003670.y = pos[1];
    D_70003670.z = pos[2] - d;
    D_70003670.w = 1.0f;

    vu0_mat4_mul_vec4(&D_70003660, m, &D_70003660);
    vu0_mat4_mul_vec4(&D_70003670, m, &D_70003670);

    D_70003660.z = D_70003660.z * (1.0f / D_70003660.w);
    D_70003670.z = D_70003670.z * (1.0f / D_70003670.w);

    return func_0011DF78(D_70003660.z - D_70003670.z);
}
