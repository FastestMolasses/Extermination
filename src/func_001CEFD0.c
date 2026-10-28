// NEARMISS func_001CEFD0  (vram 0x001CEFD0, 0x348 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906); matching wave 2026-10-28 (inline VU0 asm,
// user-approved 2026-10-28 for the VU0 class). Remaining differences:
// (1) the two 7-float .rodata tables are copied with lq/ld and a final integer word load;
// the struct copy here moves the last word through an FPU register (same bytes).
// (2) the original re-reads the centred X and Y from the scratchpad for r; mwcc here
// forwards the just-stored Y and hoists the X load. Was 0.00% (scalar emulation).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003600 0x70003604 0x7000360C 0x70003620 0x70003624 0x70003A20 0x70003A24
typedef unsigned long long u64;
typedef struct { float f[7]; } Float7;   /* a 7-entry .rodata table, copied whole */

typedef union { int i; float f; } Word;
extern Word  D_70003600;           /* projected X (12.4), then centred float X */
extern Word  D_70003604;           /* projected Y (12.4), then centred float Y */
extern int   D_7000360C;           /* projected fog F */
extern float D_70003610[4];        /* faded colour */
extern int   D_70003620;           /* sprite X (12.4) */
extern int   D_70003624;           /* sprite Y (12.4) */
extern float D_70003A20;           /* float scratch */
extern float D_70003A24;           /* float scratch */
typedef struct { float x, y, z, w; } Vec4;
extern Vec4  D_70003AC0[4];        /* camera P*V */
extern char  D_007635C0;           /* flare display-list page */

extern const Float7 D_00251370;     /* position factor along the centre axis */
extern const Float7 D_00251390;     /* sprite size */

extern char *func_001CD370(int which);
extern float func_0011E748(float x);
extern int   float_to_int(float x);
extern void  func_00102900(float *dst, const float *v, float s);
extern void  func_00102990(float *dst, float *v);
extern void  func_001CF320(int *xy, float *rgba, u64 tex0, float size);
extern void  func_001CB900(char *page, int sortkey, int tag);

/* Lens flare for a light at pos: VU0 clip test against the matrix from
   func_001CD370(2), loaded into VU0 24..27; if any clip flag is set,
   nothing is drawn. Otherwise project with the camera rows
   (28..31) and the live-in fog ramp (VU0 23) to 12.4 X, Y, Z and F in the
   scratchpad; draw only when F is 0..255. The centred screen offset gives a
   fade 1 - min(r / 393.85, 1) that scales rgba (func_00102900, then made
   integer by func_00102990); seven sprites are placed along the centre axis
   at posFactor[i] with size[i], alternating two TEX0 values, and the page is
   closed with func_001CB900. */
void func_001CEFD0(const float *pos, const float *rgba)
{
    Float7 posFactor;
    Float7 size;
    float  clip[4];
    char  *m;
    const Vec4 *cam;
    int    flags;
    int    i;
    float  cx, cy;

    posFactor = D_00251370;
    size = D_00251390;

    m = func_001CD370(2);
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

    cam = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(cam));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(cam));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(cam));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(cam));
    {
        int *out = &D_70003600.i;
        float *c = clip;

        asm("vmulax.xyzw ACC, vf28, vf1x");
        asm("vmadday.xyzw ACC, vf29, vf1y");
        asm("vmaddaz.xyzw ACC, vf30, vf1z");
        asm("vmaddw.xyzw vf2, vf31, vf0w");
        asm("vdiv Q, vf0w, vf2w");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(c) : "memory");
        asm("vwaitq");
        asm("vmulq.xyz vf2, vf2, Q");
        asm("vmulaz.w ACC, vf0, vf23z");
        asm("vmaddw.w vf2, vf23, vf2w");
        asm("vminix.w vf2, vf2, vf23x");
        asm("vmaxx.w vf2, vf2, vf0x");
        asm("vftoi4.xyzw vf2, vf2");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(out) : "memory");
    }

    if (D_7000360C < 0) {
        return;
    }
    if (D_7000360C >= 0x100) {
        return;
    }

    D_70003600.f = (float)((D_70003600.i >> 4) - 0x800);
    D_70003604.f = (float)((D_70003604.i >> 4) - 0x800);

    cx = D_70003600.f;
    cy = D_70003604.f;
    D_70003A20 = func_0011E748(cx * cx + cy * cy);
    D_70003A20 = D_70003A20 / 393.84616f;
    D_70003A20 = (D_70003A20 > 1.0f) ? 1.0f : D_70003A20;
    D_70003A20 = 1.0f - D_70003A20;
    func_00102900(D_70003610, rgba, D_70003A20);
    func_00102990(D_70003610, D_70003610);

    for (i = 0; i < 7; i++) {
        u64 tex0;

        D_70003A20 = posFactor.f[i];
        D_70003A24 = size.f[i];
        D_70003620 = (float_to_int(D_70003600.f * D_70003A20) + 0x800) << 4;
        D_70003624 = (float_to_int(D_70003604.f * D_70003A20) + 0x800) << 4;
        if (i & 1) {
            tex0 = 0x20045F0599421EB8ULL;
        } else {
            tex0 = 0x20045DA599421EA8ULL;
        }
        func_001CF320(&D_70003620, D_70003610, tex0, D_70003A24);
    }

    func_001CB900(&D_007635C0, 0xFFF000, 2);
}
