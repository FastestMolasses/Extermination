// NEARMISS func_001E0750  (vram 0x001E0750, 0x36C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906); matching wave 2026-10-28 (inline VU0 asm,
// user-approved 2026-10-28 for the VU0 class). Remaining differences:
// The original hands 0.0f to VU0 from an FPU register (a float-to-integer move plus a
// hazard nop, not VU0 instructions); this C passes the integer 0, and two argument
// registers shift with it. Was 13.75% (scalar emulation).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003600 0x70003604 0x70003608
typedef unsigned int u128 __attribute__((mode(TI)));
typedef struct { float x, y, z, w; } Vec4;
typedef struct { float f[12]; } Corners;

typedef struct { char pad[0x1C]; char *cur; } RenderCtx;
extern RenderCtx *D_00275670;

extern int   D_70003600[4];             /* projected point, 12.4 fixed point */
extern Vec4  D_70003AC0[4];             /* camera P*V */
extern const Corners D_00253530;        /* six (x, y) corner offsets */

extern Vec4 *func_001CD370(int n);
extern int   func_00122BB8(void);
extern int   float_to_int(float f);

/* Heat-haze sprite at pos: VU0 clip test against func_001CD370(0)'s rows
   (VU0 24..27), nothing drawn if any clip flag is set. Otherwise project with
   the camera rows (28..31; the w reduction is by 0.0) and the fog ramp
   (VU0 23) into the scratchpad as 12.4 X, Y, Z, F; scale = 8 * 256 / w.
   A six-vertex PACKED GIF strip (RGBAQ, ST, XYZ2) follows: vertex 0 opaque
   (alpha style[3]), the rim transparent, XY = projected centre + scale *
   corner offset, and ST taken from the vertex's own 12.4 screen position
   plus a random jitter, which samples the frame buffer around it. */
void func_001E0750(const Vec4 *pos, const int *style)
{
    unsigned int rgba_centre[4];
    unsigned int rgba_edge[4];
    Corners corner;
    Vec4  clip;
    const Vec4 *m;
    const Vec4 *cam;
    Vec4 *c;
    int  *out;
    int   i;
    char *vtx;
    float jitter_s, jitter_t;
    float scale;
    RenderCtx *ctx;
    char *pkt;
    int   flags;
    int   zero;

    m = func_001CD370(0);
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));
    asm("lqc2 vf1, 0x0(%0)" : : "r"(pos));
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

    rgba_centre[0] = 0x80;
    rgba_centre[1] = 0x80;
    rgba_centre[2] = 0x80;
    rgba_centre[3] = style[3];
    rgba_edge[0] = 0x80;
    rgba_edge[1] = 0x80;
    rgba_edge[2] = 0x80;
    rgba_edge[3] = 0;

    cam = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(cam));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(cam));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(cam));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(cam));
    zero = 0;                       /* the w reduction (0.0f; see the header) */
    out = D_70003600;
    c = &clip;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(pos));
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(c) : "memory");
    asm("vwaitq");
    asm("vmulq.xy vf2, vf2, Q");
    asm("qmtc2.ni %0, vf3" : : "r"(zero));
    asm("vsubx.w vf2, vf2, vf3x");
    asm("vdiv Q, vf0w, vf2w");
    asm("vwaitq");
    asm("vmulq.z vf2, vf2, Q");
    asm("vmulaz.w ACC, vf0, vf23z");
    asm("vmaddw.w vf2, vf23, vf2w");
    asm("vminix.w vf2, vf2, vf23x");
    asm("vmaxx.w vf2, vf2, vf0x");
    asm("vftoi4.xyzw vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(out) : "memory");

    scale = clip.w;
    scale = 8.0f * (256.0f / scale);
    corner = D_00253530;

    ctx = D_00275670;
    *(char *)(ctx->cur + 3) = 0x10;
    *(int *)(ctx->cur + 4) = 0;
    *(short *)(ctx->cur + 0) = 0x14;
    pkt = ctx->cur;
    vtx = pkt + 0x10;
    ctx->cur = pkt + 0x150;
    *(u128 *)vtx = 0;
    *(int *)(vtx + 0xC) = 0x50000013;
    *(long long *)(vtx + 0x10) = (long long)0x8006 | ((long long)0x302EC000 << 32);
    *(long long *)(vtx + 0x18) = 0x421;
    vtx += 0x20;

    for (i = 0; i < 6; i++) {
        jitter_s = scale * (-0.000195312503f + 0.000390625006f * (4.65661287e-10f * (float)func_00122BB8()));
        jitter_t = scale * (-0.000446428574f + 0.000892857148f * (4.65661287e-10f * (float)func_00122BB8()));

        if (i == 0) {
            *(u128 *)vtx = *(u128 *)rgba_centre;
        } else {
            *(u128 *)vtx = *(u128 *)rgba_edge;
        }

        *(int *)(vtx + 0x20) = float_to_int((float)D_70003600[0] + scale * corner.f[i * 2 + 0]);
        *(int *)(vtx + 0x24) = float_to_int((float)D_70003600[1] + scale * corner.f[i * 2 + 1]);
        *(int *)(vtx + 0x28) = D_70003600[2];
        *(int *)(vtx + 0x2C) = 0;
        *(float *)(vtx + 0x10) = jitter_s + (-3.5f + 0.0001220703125f * (float)*(int *)(vtx + 0x20));
        *(float *)(vtx + 0x14) = jitter_t + (-7.5625f + 0.000244140625f * (float)*(int *)(vtx + 0x24));
        *(int *)(vtx + 0x18) = 0x3F800000;
        vtx += 0x30;
    }
}
