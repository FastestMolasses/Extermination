// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003600 0x70003604 0x70003610 0x70003614 0x70003618

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 46.71% NEARMISS).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Fixes read from the original instructions:
//  - the VU0 macro-mode work is written as one-instruction inline asm statements; the old C called a
//    stand-in helper.
//  - scratchpad words are per-word relocated externs (idiom-32, // SPAD:) declared
//    as unsized arrays so they are not gp-relative; the working quad is a
//    float/int union.
//  - the depth store comes first, then the two half extents.
//  - cy and hh are read only after the first window test passes; compares are
//    written as "> 0x9000" / "> 0x8700"; locals declared hh, cy, hw, cx.
//  - the result reloads the stored half width from the scratchpad.
//
// Screen-space extent test for a sprite of size width x height centred on
// the last projected point. The half extents and the integer view depth go
// into the working quad (0x70003610), which is transformed by the screen-scale
// matrix at 0x70003A40 (its third row zeroed in x, y first), divided by w and
// converted to 12.4 in x, y. Returns 0 when the box misses the GS window
// (centre at 0x70003600), else (larger half extent >> 3) + 1, keeping the
// larger extent in the working quad.

typedef struct { float x, y, z, w; } Vec4;
typedef union { float f; int i; } Word;

extern int   D_00275C04;
extern int   D_70003600[];
extern int   D_70003604[];
extern Word  D_70003610[];
extern Word  D_70003614[];
extern Word  D_70003618[];
extern Vec4  D_70003A40[4];

int func_001CD180(float width, float height)
{
    Word *q;
    Vec4 *m;
    int hh;
    int cy;
    int hw;
    int cx;

    D_70003618[0].f = (float)D_00275C04;
    D_70003610[0].f = 0.5f * width;
    D_70003614[0].f = 0.5f * height;
    q = D_70003610;
    m = D_70003A40;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(q));
    asm("lqc2 vf16, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf17, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf18, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf19, 0x30(%0)" : : "r"(m));
    asm("vsub.xy vf18, vf18, vf18");
    asm("vmulax.xyzw ACC, vf16, vf1x");
    asm("vmadday.xyzw ACC, vf17, vf1y");
    asm("vmaddaz.xyzw ACC, vf18, vf1z");
    asm("vmaddw.xyzw vf2, vf19, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("vwaitq");
    asm("vmulq.xy vf2, vf2, Q");
    asm("vftoi4.xy vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(q));

    cx = D_70003600[0];
    hw = D_70003610[0].i;
    if (cx - hw > 0x9000) return 0;
    cy = D_70003604[0];
    hh = D_70003614[0].i;
    if (cy - hh > 0x8700) return 0;
    if (cx + hw <  0x7000) return 0;
    if (cy + hh <  0x7900) return 0;

    if (hw < hh) {
        D_70003610[0].i = hh;
    }
    return (D_70003610[0].i >> 3) + 1;
}
