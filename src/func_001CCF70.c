// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 38.39% NEARMISS).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class): each VU0 macro
// instruction is its own asm statement; everything else is C.
// Fixes read from the original instructions:
//  - the three "COP2 primitive" externs did not exist: the clip test and the
//    projection are inline VU0 code in this function, and the projection keeps
//    the fog quadword it loads itself (from the render context + 0xA0).
//  - the integer globals are gp-relative (-sdatathreshold 4, not 0).
//  - the stack address of the clip-space result is taken before the camera
//    transform starts (it is computed ahead of the multiply chain).
//
// Depth key of a world point (sort key for the caller's packet chain).
//   1. The clip matrix func_001CD370(0) is loaded into VU0 rows 24..27 and the
//      point (w forced to 1) is transformed; the w-clip judgement of x, y, z
//      sets six flags in the clip-flag register. Any of them set -> the point
//      is outside the view volume and 0xFFFFFF is returned.
//   2. Otherwise the fog quadword (render context + 0xA0) goes to VU0 row 23,
//      the camera matrix 0x70003AC0 to rows 28..31 and the point is projected:
//      the raw clip-space result is stored on the stack, x, y, z are divided by
//      w through Q, w becomes clamp(fog.z + fog.w * w, 0, fog.x) and the whole
//      quadword is converted to 12.4 fixed point at the scratchpad quad
//      0x70003600.
//   3. D_00275C04 = float_to_int(raw w) (the integer view depth); the return
//      value is the 12.4 screen z.

typedef struct { float x, y, z, w; } Vec4;
typedef struct { int   x, y, z, w; } IVec4;

extern char *D_00275670;
extern int   D_00275C04;
extern Vec4  D_70003AC0[4];
extern IVec4 D_70003600;

extern Vec4 *func_001CD370(int variant);
extern int   float_to_int(float v);

int func_001CCF70(const Vec4 *world_pos)
{
    Vec4 *m;
    char *fog;
    Vec4 *k;
    IVec4 *scr;
    Vec4 clip;
    Vec4 *cp;
    int flags;

    m = func_001CD370(0);
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));
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
    if ((flags & 0x3F) != 0) {
        return 0xFFFFFF;
    }

    fog = D_00275670 + 0xA0;
    asm("lqc2 vf23, 0x0(%0)" : : "r"(fog));
    k = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(k));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(k));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(k));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(k));
    scr = &D_70003600;
    cp = &clip;
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(cp));
    asm("vwaitq");
    asm("vmulq.xyz vf2, vf2, Q");
    asm("vmulaz.w ACC, vf0, vf23z");
    asm("vmaddw.w vf2, vf23, vf2w");
    asm("vminix.w vf2, vf2, vf23x");
    asm("vmaxx.w vf2, vf2, vf0x");
    asm("vftoi4.xyzw vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(scr));

    D_00275C04 = float_to_int(clip.w);
    return D_70003600.z;
}
