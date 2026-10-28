// NEARMISS func_001CD940  (vram 0x001CD940, 0x480 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.08% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4), was 94.03%.
// Inline VU0 asm (user-approved 2026-10-28 for the VU0 class, incl. the float-to-GPR
// move before a VU0 transfer), one instruction per statement; mwcc inserts the hazard
// nop after the move itself (no explicit nop is written).
// Matching wave 2026-10-28 rebuilt it from the original: it is func_001E2800's
// line drawer (same projection, fade and packet code, its own display list
// constants) preceded by a VU0 clip test of both endpoints against camera 0's
// cull matrix and the loading of the fog ramp and camera matrix, all as VU0
// macro code inline (one instruction per asm statement). -sdatathreshold 4
// (gp-relative render context); the display list and the line GIF state block
// are unsized arrays (not gp-relative); per-quad scratchpad externs, the fade
// words their own externs (// SPAD:); 128-bit colour copies; the fade's mode
// test is a switch. The w bias 1.0f is a float local of the projection moved
// to t0 before each VU0 transfer (no stack union; frame back to 0x80).
//  - FIDELITY: func_001CB6B0 gets a plain 32-bit address (the old C zero-extended it).
// Remaining differences: mwcc pads the label after each fade clamp's 0xFF store
// with a nop the original lacks (shifting the later branch targets), and the
// two colour copies and the final packet copies colour their address registers
// differently (as func_001E2800).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// SPAD: 0x7000360C 0x7000361C
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

extern char D_007635C0[];
extern char D_00251240[];
extern char *D_00275670;            /* per-frame render context */
extern Vec4 D_70003AC0[4];          /* camera P*V */
extern char *func_001CD370(int camera);

/* EE scratchpad staging block for one line. */
extern int D_70003600[4];   /* endpoint A: x, y, z (12.4), fade */
extern int D_70003610[4];
extern int D_7000360C[];   /* endpoint A's fade word */
extern int D_7000361C[];   /* endpoint B's fade word */   /* endpoint B */
extern int D_70003620[4];   /* colour A: R, G, B, A */
extern int D_70003630[4];   /* colour B */

static inline void vu0_project(int *out, const Vec4 *p)
{
    Vec4 raw;
    int *o = out;
    Vec4 *r = &raw;
    float one = 1.0f;

    asm("lqc2 vf1, 0x0(%0)" : : "r"(p));
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(r) : "memory");
    asm("vwaitq");
    asm("vmulq.xy vf2, vf2, Q");
    asm("mfc1 $8, %0" : : "f"(one) : "$8");
    asm("qmtc2.ni $8, vf3");
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

#define DEPTH_FADE(mode, fw, col)                            \
    do {                                                   \
        fw = fw >> 4;                                \
        if (fw > 0xFF) fw = 0xFF;                   \
        if (fw < 0)      fw = 0;                     \
        switch (mode) {                                    \
        case 2:                                            \
        case 3:                                            \
            col[0] = (col[0] * fw) >> 8;                \
            col[1] = (col[1] * fw) >> 8;                \
            col[2] = (col[2] * fw) >> 8;                \
            break;                                         \
        case 1:                                            \
            col[3] = (col[3] * fw) >> 8;                \
            break;                                         \
        }                                                  \
        fw = 0xFF0;                                     \
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

    vu0_project(D_70003600, pa);
    vu0_project(D_70003610, pb);

    *(u128 *)D_70003620 = *(const u128 *)ca;
    *(u128 *)D_70003630 = *(const u128 *)cb;

    if (mode != 0) {
        DEPTH_FADE(mode, D_7000360C[0], D_70003620);
    }
    if (mode != 0) {
        DEPTH_FADE(mode, D_7000361C[0], D_70003630);
    }

    dst = func_001CB5F0((int)D_007635C0, D_70003600[2], 4);
    ((u128 *)dst)[0] = *(u128 *)D_70003620;
    ((u128 *)dst)[1] = *(u128 *)D_70003600;
    ((u128 *)dst)[2] = *(u128 *)D_70003630;
    ((u128 *)dst)[3] = *(u128 *)D_70003610;
    func_001CB6B0((int)D_007635C0, D_70003600[2], 2,
                  (int)D_00251240);
    func_001CB900((int)D_007635C0, D_70003600[2], mode);
}
