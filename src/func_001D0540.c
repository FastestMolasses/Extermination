// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 52.75% NEARMISS).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Fixes read from the original instructions:
//  - the VU0 macro-mode work is written as one-instruction inline asm statements; the old C called a
//    stand-in helper.
//  - each transform takes its own destination and source pointers (both the same
//    scratchpad quad) inside a block, as the original materialises the address
//    twice.
//
// Depth span of a vertical segment after projection: quad A = (pos, 1) and
// quad B = (pos.x, pos.y, pos.z - d, 1) in scratchpad 0x70003660 / 0x70003670 are
// both transformed by the matrix m (loaded into VU0 rows 20..23), their z is
// divided by w in place, and the result is fabsf(zA - zB) via func_0011DF78
// (a tail call).

typedef struct { float x, y, z, w; } Vec4;

extern Vec4  D_70003660;
extern Vec4  D_70003670;
extern float func_0011DF78(float v);

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

    asm("lqc2 vf20, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf21, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf22, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf23, 0x30(%0)" : : "r"(m));
    {
        Vec4 *dst = &D_70003660;
        Vec4 *src = &D_70003660;
        asm("lqc2 vf1, 0x0(%0)" : : "r"(src));
        asm("vmulax.xyzw ACC, vf20, vf1x");
        asm("vmadday.xyzw ACC, vf21, vf1y");
        asm("vmaddaz.xyzw ACC, vf22, vf1z");
        asm("vmaddw.xyzw vf2, vf23, vf0w");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(dst));
    }
    {
        Vec4 *dst = &D_70003670;
        Vec4 *src = &D_70003670;
        asm("lqc2 vf1, 0x0(%0)" : : "r"(src));
        asm("vmulax.xyzw ACC, vf20, vf1x");
        asm("vmadday.xyzw ACC, vf21, vf1y");
        asm("vmaddaz.xyzw ACC, vf22, vf1z");
        asm("vmaddw.xyzw vf2, vf23, vf0w");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(dst));
    }

    D_70003660.z = D_70003660.z * (1.0f / D_70003660.w);
    D_70003670.z = D_70003670.z * (1.0f / D_70003670.w);

    return func_0011DF78(D_70003660.z - D_70003670.z);
}
