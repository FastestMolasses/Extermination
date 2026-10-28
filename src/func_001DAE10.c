// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 85.68% NEARMISS).
// Fixes read from the original instructions:
//  - The x and y range tests are `a < 0 || !(a <= 65520)` pairs in an if/else-if
//    (the byte-matched twin func_001E0240's spelling); the old nested chain had
//    the wrong branch layout.
//  - The transformed point is a float[4], like the twin's buffer.
//  - The clip test is VU0 macro code inlined in the original (no C spelling), so
//    it is written as inline asm statements, one instruction each, instead of a
//    call to an extern.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
//
// Projects one world-space vertex for the GS: transforms it by the camera matrix
// K = P*V at scratchpad 0x70003AC0, then also clips it on VU0 against the matrix
// rows the caller loaded into VU0 registers 24..27. Then it scales x, y, z by
// 16 / w (the GS 12.4 fixed-point scale) and hands the result to func_001DAC90,
// which writes the vertex packet. The vertex is rejected (dst+0x2C = 0x8000) when
// any of the six clip flags is set, the scale is negative (behind the camera),
// or the scaled x or y falls outside 0..65520. The y test is skipped once x
// has failed.

typedef unsigned char u8;
typedef struct { float x, y, z, w; } Vec4;

extern Vec4 D_70003AC0[4];      /* camera K = P*V, EE scratchpad */

extern void func_001026A0(float *dst, const Vec4 *m, const Vec4 *v);
extern void func_001DAC90(u8 *dst, float *screen, void *uv, float *shade);

void func_001DAE10(u8 *dst, const Vec4 *world_pos, void *uv, float *shade)
{
    int   clip;
    float screen[4];                    /* x, y, z, w */
    float s;
    int   reject = 0;

    func_001026A0(screen, D_70003AC0, world_pos);

    /* VU0: transform world_pos by the matrix rows the caller left in VU0
       registers 24..27, clip x, y, z against w, wait out the flag pipeline,
       then read the six clip flags */
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
    asm("cfc2.ni %0, vi18" : "=r"(clip));
    if ((clip & 0x3F) != 0) {
        reject = 1;
    }

    s = 16.0f / screen[3];              /* 16 = the GS 12.4 scale */
    if (s < 0.0f) {
        reject = 1;                     /* behind the camera */
    }

    screen[0] *= s;
    screen[1] *= s;
    screen[2] *= s;

    if (screen[0] < 0.0f || !(screen[0] <= 65520.0f)) {
        reject = 1;                     /* the y test is skipped on this path */
    } else if (screen[1] < 0.0f || !(screen[1] <= 65520.0f)) {
        reject = 1;
    }

    func_001DAC90(dst, screen, uv, shade);

    if (reject) {
        *(int *)(dst + 0x2C) = 0x8000;
    }
}
