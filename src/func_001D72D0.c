// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 56.14% NEARMISS).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Fixes read from the original instructions:
//  - the VU0 macro-mode work is written as one-instruction inline asm statements; the old C
//    called a fictitious vu0_project helper in its place.
//  - -sdatathreshold 4: the point counter D_00275C08 is gp-relative.
//  - screen quad below the clip quad on the stack; the matrix, screen and clip
//    pointers are locals taken in that order.
//
// Projects a world point for the per-frame screen-point list.
// Returns -1 when the list already holds 16 points or the point is behind
// the camera (clip w < 0); otherwise hands the 12.4 screen x, y re-centred
// on the 512x224 window (x - 0x700, y - 0x790 in pixels) to func_001D7200.
// The camera matrix K = P*V lives in scratchpad 0x70003AC0 and is loaded
// into VU0 rows 28..31; the fog quad in VU0 row 23 is whatever the last
// caller left there. The projection divides by w, clamps the fog lane
// into [0, fog.x] and converts to 12.4 fixed point.

typedef struct { float x, y, z, w; } Vec4;
typedef struct { int   x, y, z, w; } IVec4;

extern int   D_00275C08;
extern Vec4  D_70003AC0[4];
extern int   func_001D7200(int x, int y);

int func_001D72D0(const Vec4 *world_pos)
{
    IVec4 screen;
    Vec4  clip;
    const Vec4 *m;
    IVec4 *s;
    Vec4 *c;

    if (D_00275C08 >= 0x10) {
        return -1;
    }
    m = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(m));
    s = &screen;
    c = &clip;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(world_pos));
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
    if (clip.w < 0.0f) {
        return -1;
    }
    return func_001D7200((screen.x >> 4) - 0x700, (screen.y >> 4) - 0x790);
}
