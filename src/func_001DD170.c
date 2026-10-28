// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 29.59% NEARMISS).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class): each VU0 macro
// instruction is its own asm statement; everything else is C.
// Fixes read from the original instructions:
//  - the clip test and the projection are inline VU0 code (no helper calls);
//    the projection does NOT reload the fog quadword in VU0 row 23, it uses
//    whatever the previous projector left there (as the old comment said).
//  - the kind dispatch is a switch with case 0 written before case 1 and both
//    ending in break (the compares come out in reverse source order).
//  - the colour record handed to func_001CBA50 is a struct (24-bit colour
//    word, alpha byte, a zero flag byte), not a byte array; the render-context
//    pointer is gp-relative (-sdatathreshold 4).
//
// Marker / label at a world point.
//   1. w-clip test of the point against the clip matrix at render context
//      + 0x2240; any clip flag -> nothing is drawn.
//   2. Projection through the camera matrix 0x70003AC0 to the 12.4 screen
//      quad 0x70003600 (raw result on the stack).
//   3. kind 0: func_001DD2F0(style, screen, colour).
//      kind 1: a colour record { colour & 0xFFFFFF, colour >> 24, 0 } is
//      handed with the screen position ((x + 0xC0) >> 4, y >> 4), size 8x8 and
//      the handle func_001C5FB0(value, 4, 0) to func_001CBA50, then
//      func_001DD600(style, screen, colour).

typedef unsigned char      u8;
typedef unsigned int       u32;
typedef struct { float x, y, z, w; } Vec4;
typedef struct { int   x, y, z, w; } IVec4;

extern char *D_00275670;
extern Vec4  D_70003AC0[4];
extern IVec4 D_70003600;

extern int  func_001C5FB0(int value, int a1, int a2);
extern void func_001CBA50(int a0, int x, int y, int a3, int t0, int handle,
                          void *colour_rec);
extern void func_001DD2F0(int style, IVec4 *screen, u32 colour);
extern void func_001DD600(int style, IVec4 *screen, u32 colour);

void func_001DD170(int style, const Vec4 *world_pos, int kind, u32 colour,
                   int value)
{
    Vec4 *m;
    Vec4 *k;
    IVec4 *scr;
    Vec4 *cp;
    Vec4 clip;
    struct { u32 rgb; u8 a; u8 flag; } rec;
    int flags;

    m = (Vec4 *)(D_00275670 + 0x2240);
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
        return;
    }

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

    switch (kind) {
    case 0:
        func_001DD2F0(style, &D_70003600, colour);
        break;
    case 1:
        rec.rgb = colour & 0x00FFFFFF;
        rec.a = (u8)(colour >> 24);
        rec.flag = 0;
        func_001CBA50(1,
                      (D_70003600.x + 0xC0) >> 4,
                      D_70003600.y >> 4,
                      8, 8,
                      func_001C5FB0(value, 4, 0),
                      &rec);
        func_001DD600(style, &D_70003600, colour);
        break;
    }
}
