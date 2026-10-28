// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 45.25% NEARMISS).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Fixes read from the original instructions:
//  - the VU0 macro-mode work is written as one-instruction inline asm statements; the old C used
//    vu0_* stand-in helpers.
//  - the clip judgement result is read from the VU0 clip-flag register into an int,
//    masked to six bits and tested against plane_mask.
//  - the second projection reuses the point already loaded for the clip test.
//  - screen-quad and clip pointers are locals taken in that order.
//
// Clip-tests a world point against the planes selected in plane_mask using
// the clip matrix func_001CD370(0) (returns 0xFFFFFF when rejected), then
// projects it with the camera matrix (scratchpad 0x70003AC0) and the fog quad
// at render context + 0xA0 into the 12.4 screen quad at scratchpad 0x70003600.
// Stores the integer view depth (clip w) in D_00275C04 and returns the
// 12.4 GS depth (screen.z).

typedef struct { float x, y, z, w; } Vec4;
typedef struct { int   x, y, z, w; } IVec4;

extern char *D_00275670;
extern int   D_00275C04;
extern Vec4  D_70003AC0[4];
extern IVec4 D_70003600;

extern Vec4 *func_001CD370(int variant);
extern int   float_to_int(float v);

int func_001CD070(const Vec4 *world_pos, int plane_mask)
{
    Vec4 clip;
    Vec4 *mc;
    char *fog;
    const Vec4 *m;
    int flags;
    IVec4 *s;
    Vec4 *c;

    mc = func_001CD370(0);
    asm("lqc2 vf24, 0x0(%0)" : : "r"(mc));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(mc));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(mc));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(mc));
    asm("lqc2 vf1, 0x0(%0)" : : "r"(world_pos));
    asm("vmulax.xyzw ACC, vf24, vf1x");
    asm("vmadday.xyzw ACC, vf25, vf1y");
    asm("vmaddaz.xyzw ACC, vf26, vf1z");
    asm("vmaddw.xyzw vf2, vf27, vf0w");
    asm("vclipw.xyz vf2, vf2w");
    asm("vnop");
    asm("vnop");
    asm("vnop");
    asm("vnop");
    asm("vnop");
    asm("cfc2.ni %0, vi18" : "=r"(flags));
    if ((plane_mask & (flags & 0x3F)) != 0) {
        return 0xFFFFFF;
    }
    fog = D_00275670 + 0xA0;
    asm("lqc2 vf23, 0x0(%0)" : : "r"(fog));
    m = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(m));
    s = &D_70003600;
    c = &clip;
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(c));
    asm("vwaitq");
    asm("vmulq.xyz vf2, vf2, Q");
    asm("vmulaz.w ACC, vf0, vf23z");
    asm("vmaddw.w vf2, vf23, vf2w");
    asm("vminix.w vf2, vf2, vf23x");
    asm("vmaxx.w vf2, vf2, vf0x");
    asm("vftoi4.xyzw vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(s));
    D_00275C04 = float_to_int(clip.w);
    return D_70003600.z;
}
