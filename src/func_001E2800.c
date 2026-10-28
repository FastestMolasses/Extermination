// NEARMISS func_001E2800  (vram 0x001E2800, 0x3A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0), was 4.87%.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Matching wave 2026-10-28 rebuilt it block by block from the original:
//  - each endpoint is projected by VU0 macro code inline (camera matrix in
//    VU0 registers 28..31, fog ramp in 23, both loaded by the caller); the raw
//    transformed vector is also spilled to a stack temporary, as the original does.
//  - the scratchpad words are per-quad relocated externs; colours are 128-bit copies.
//  - the fade's mode test is a switch (2/3 colour, 1 alpha), clamp written "> 0xFF".
//  - FIDELITY: func_001CB6B0's fourth argument is a plain 32-bit address (the old
//    C zero-extended it to 64 bits); the old C emulated the VU0 projection on the FPU.
// Remaining differences: the w bias (1.0f) reaches VU0 through an integer
// register. The original builds 1.0f in an FPU register once and moves it to
// the fixed register t0 before each transfer (moving the fifth argument out of
// t0 at entry); mwcc 2.3.3 cannot express that move without non-VU0 asm (a float
// "r" operand is silently not materialised), so the bits go through a stack
// union here. Two label paddings and register colouring follow from that size
// difference (with the sizes aligned the rest of the body lines up).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Draws one gouraud line between two points: projects both endpoints to 12.4
// screen space with a depth-fade value in w, stages the two colours, optionally
// applies the depth fade (mode 1: alpha; modes 2 and 3: RGB), then reserves
// four quadwords in the z-sorted display list keyed by endpoint A's depth and
// appends the canned line GIFtag block and the mode's trailer.

typedef struct { float x, y, z, w; } Vec4;
typedef int u128 __attribute__((mode(TI)));
typedef struct { int w[4]; } Qw __attribute__((aligned(16)));

extern char *func_001CB5F0(int list, int z, int nqw);
extern void  func_001CB6B0(int list, int z, int nqw, int ref);
extern void  func_001CB900(int list, int z, int kind);

extern int D_007635C0;
extern int D_00253720;

/* EE scratchpad staging block for one line. */
extern int D_70003600[4];   /* endpoint A: x, y, z (12.4), fade */
extern int D_70003610[4];   /* endpoint B */
extern int D_70003620[4];   /* colour A: R, G, B, A */
extern int D_70003630[4];   /* colour B */

/* Transform p by the camera matrix in VU0 (implicit w = 1), divide x, y by w,
   subtract the bias from w and divide z by the biased w, map the biased w
   through the fog ramp (clamped to [0, ramp.x]) and convert all four to 12.4. */
static inline void vu0_project(int *out, const Vec4 *p, float bias)
{
    Vec4 raw;
    int *o = out;
    Vec4 *r = &raw;
    union { float f; int i; } u;

    asm("lqc2 vf1, 0x0(%0)" : : "r"(p));
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(r) : "memory");
    asm("vwaitq");
    asm("vmulq.xy vf2, vf2, Q");
    u.f = bias;
    asm("qmtc2.ni %0, vf3" : : "r"(u.i));
    asm("vsubx.w vf2, vf2, vf3x");
    asm("vdiv Q, vf0w, vf2w");
    asm("vwaitq");
    asm("vmulq.z vf2, vf2, Q");
    asm("vmulaz.w ACC, vf0, vf23z");
    asm("vmaddw.w vf2, vf23, vf2w");
    asm("vminix.w vf2, vf2, vf23x");
    asm("vmaxx.w vf2, vf2, vf0x");
    asm("vftoi4.xyzw vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(o) : "memory");
}

#define DEPTH_FADE(mode, pt, col)                          \
    do {                                                   \
        pt[3] = pt[3] >> 4;                                \
        if (pt[3] > 0xFF) pt[3] = 0xFF;                   \
        if (pt[3] < 0)      pt[3] = 0;                     \
        switch (mode) {                                    \
        case 2:                                            \
        case 3:                                            \
            col[0] = (col[0] * pt[3]) >> 8;                \
            col[1] = (col[1] * pt[3]) >> 8;                \
            col[2] = (col[2] * pt[3]) >> 8;                \
            break;                                         \
        case 1:                                            \
            col[3] = (col[3] * pt[3]) >> 8;                \
            break;                                         \
        }                                                  \
        pt[3] = 0xFF0;                                     \
    } while (0)

void func_001E2800(int mode, const Vec4 *pa, const void *ca,
                   const Vec4 *pb, const void *cb)
{
    char *dst;

    vu0_project(D_70003600, pa, 1.0f);
    vu0_project(D_70003610, pb, 1.0f);

    *(u128 *)D_70003620 = *(const u128 *)ca;
    *(u128 *)D_70003630 = *(const u128 *)cb;

    if (mode != 0) {
        DEPTH_FADE(mode, D_70003600, D_70003620);
    }
    if (mode != 0) {
        DEPTH_FADE(mode, D_70003610, D_70003630);
    }

    dst = func_001CB5F0((int)&D_007635C0, D_70003600[2], 4);
    ((u128 *)dst)[0] = *(u128 *)D_70003620;
    ((u128 *)dst)[1] = *(u128 *)D_70003600;
    ((u128 *)dst)[2] = *(u128 *)D_70003630;
    ((u128 *)dst)[3] = *(u128 *)D_70003610;
    func_001CB6B0((int)&D_007635C0, D_70003600[2], 2,
                  (int)&D_00253720);
    func_001CB900((int)&D_007635C0, D_70003600[2], mode);
}
