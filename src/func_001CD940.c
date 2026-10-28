// NEARMISS func_001CD940  (vram 0x001CD940, 0x480 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.03% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4), was 22.80%.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Matching wave 2026-10-28 rebuilt it from the original: it is func_001E2800's
// line drawer (same projection, fade and packet code, its own display list
// constants) preceded by a VU0 clip test of both endpoints against camera 0's
// cull matrix and the loading of the fog ramp and camera matrix, all as VU0
// macro code inline (one instruction per asm statement). -sdatathreshold 4
// (gp-relative render context); per-quad scratchpad externs; 128-bit colour
// copies; the fade's mode test is a switch.
//  - FIDELITY: func_001CB6B0 gets a plain 32-bit address (the old C zero-extended it).
// Remaining differences: the w bias (1.0f) reaches VU0 through the fixed
// register t0; the original moves it from the FPU (hazard nop after); mwcc
// 2.3.3 drops a float asm operand, so the bits go through a stack union
// (frame 0x90, not 0x80) with the register colouring that follows.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Draws a gouraud line between two points when both pass camera 0's view
// volume test: projects both endpoints (12.4 with a fog value), stages the two
// colours, optionally fades them (mode 1 alpha, modes 2 and 3 RGB), and queues
// the 4-quadword packet with the static line GIF state and the mode's trailer.

typedef struct { float x, y, z, w; } Vec4;
typedef int u128 __attribute__((mode(TI)));
typedef struct { int w[4]; } Qw __attribute__((aligned(16)));

extern char *func_001CB5F0(int list, int z, int nqw);
extern void  func_001CB6B0(int list, int z, int nqw, int ref);
extern void  func_001CB900(int list, int z, int kind);

extern int D_007635C0;
extern int D_00251240;
extern char *D_00275670;            /* per-frame render context */
extern Vec4 D_70003AC0[4];          /* camera P*V */
extern char *func_001CD370(int camera);

/* EE scratchpad staging block for one line. */
extern int D_70003600[4];   /* endpoint A: x, y, z (12.4), fade */
extern int D_70003610[4];   /* endpoint B */
extern int D_70003620[4];   /* colour A: R, G, B, A */
extern int D_70003630[4];   /* colour B */

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

/* Clip p against the matrix rows in VU0 registers 24..27; returns the six
   clip flags (0 = inside). */
static inline int vu0_clip(const Vec4 *p)
{
    int c;

    asm("lqc2 vf1, 0x0(%0)" : : "r"(p));
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
    asm("cfc2.ni %0, vi18" : "=r"(c));
    return c & 0x3F;
}

void func_001CD940(int mode, const Vec4 *pa, const void *ca,
                   const Vec4 *pb, const void *cb)
{
    char *dst;
    char *m;
    char *fog;
    Vec4 *cam;

    m = func_001CD370(0);
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));
    if (vu0_clip(pa) != 0) return;
    if (vu0_clip(pb) != 0) return;

    fog = D_00275670 + 0xA0;
    asm("lqc2 vf23, 0x0(%0)" : : "r"(fog));
    cam = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(cam));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(cam));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(cam));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(cam));

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
                  (int)&D_00251240);
    func_001CB900((int)&D_007635C0, D_70003600[2], mode);
}
