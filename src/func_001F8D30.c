// NEARMISS func_001F8D30  (vram 0x001F8D30, 0x3D0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (was 42.78%; matching wave 2026-10-28).
// Remaining differences: one float register: the original computes the clamped fade into
// f0 and moves it to f12 at the join before func_00102900; this C coalesces fade with f12
// (one instruction shorter). Clamp spellings, if/else orders, a separate fade variable and an
// inline fade helper all tried (about 18 trials).
// Inline VU0 asm (user-approved 2026-10-28 for the VU0 class), one instruction per statement.
// Fixes found: `facing` is a POINTER (a3), not a Vec4 by value (FIDELITY: atan2 takes
// facing->z, facing->x); the fade test is `fade_rng != 0` with the 1.0 arm as the else; the
// corner transforms and the three projections are inline VU0 code with three separate stack
// clip results and three scratchpad quads (one extern each).
// FIDELITY: the original's projection uses VU0 row 23 as the fog quadword, but here row 23
// still holds the decal matrix's translation row, so the projected depth (w) is computed
// from it; only x and y are read afterwards, so the result does not change.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Ground decal (shadow / scorch) under `owner` at `contact`: builds the decal matrix
// (identity, yaw atan2(facing->z, facing->x), the surface basis of `normal`, translated
// to contact), fades the colour by 1 - |owner.y - contact.y| / fade_rng clamped to
// 0.1..1 (1 when fade_rng is 0), transforms the four corners (+-half_w, +-half_h, 0, 1),
// projects the first three to the scratchpad and draws the quad with func_001CE300 unless
// the screen-space winding is negative (back-facing).

typedef struct { float x, y, z, w; } Vec4;
typedef struct { int x, y, z, w; } IVec4;

extern void  func_001029C0(Vec4 *m);
extern void  func_00102A60(Vec4 *dst, Vec4 *src, float a);
extern void  func_001CD390(Vec4 *dst, const Vec4 *normal);
extern void  func_001026D0(Vec4 *dst, Vec4 *m, Vec4 *src);
extern void  func_00102918(Vec4 *dst, Vec4 *src, const Vec4 *t);
extern void  func_00102900(Vec4 *dst, const Vec4 *v, float s);
extern float func_0011E620(float y, float x);
extern float func_0011DF78(float x);
extern int   func_00128250(float f);
extern void  func_001CE300(int n, const Vec4 *corners,
                           unsigned long long gs_state, unsigned int rgba);

extern Vec4  D_70003AC0[4];
extern IVec4 D_70003600;
extern IVec4 D_70003610;
extern IVec4 D_70003620;

void func_001F8D30(const float *owner, const Vec4 *contact, const Vec4 *normal,
                   const Vec4 *facing, const Vec4 *colour,
                   float half_w, float half_h, float fade_rng)
{
    Vec4 basis[4];
    Vec4 surf[4];
    Vec4 corner[4];
    Vec4 tinted;
    Vec4 clip0, clip1, clip2;
    Vec4 *m;
    Vec4 *c0, *c1, *c2, *c3;
    Vec4 *k;
    Vec4 *cp;
    IVec4 *scr;
    float fade;
    unsigned int rgba;
    int cross;

    func_001029C0(basis);
    func_00102A60(basis, basis, func_0011E620(facing->z, facing->x));
    func_001CD390(surf, normal);
    func_001026D0(basis, surf, basis);
    func_00102918(basis, basis, contact);

    if (fade_rng != 0.0f) {
        fade = 1.0f - func_0011DF78(owner[1] - contact->y) / fade_rng;
        if (fade > 1.0f) fade = 1.0f;
        if (fade < 0.1f) fade = 0.1f;
    } else {
        fade = 1.0f;
    }

    func_00102900(&tinted, colour, fade);
    rgba  = func_00128250(tinted.x);
    rgba |= func_00128250(tinted.y) << 8;
    rgba |= func_00128250(tinted.z) << 16;
    rgba |= func_00128250(tinted.w) << 24;

    m = basis;
    asm("lqc2 vf20, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf21, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf22, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf23, 0x30(%0)" : : "r"(m));

    c0 = &corner[0];
    corner[0].x = -half_w; corner[0].y = -half_h; corner[0].z = 0.0f; corner[0].w = 1.0f;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(c0));
    asm("vmulax.xyzw ACC, vf20, vf1x");
    asm("vmadday.xyzw ACC, vf21, vf1y");
    asm("vmaddaz.xyzw ACC, vf22, vf1z");
    asm("vmaddw.xyzw vf2, vf23, vf0w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(c0));
    c1 = &corner[1];
    corner[1].x =  half_w; corner[1].y = -half_h; corner[1].z = 0.0f; corner[1].w = 1.0f;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(c1));
    asm("vmulax.xyzw ACC, vf20, vf1x");
    asm("vmadday.xyzw ACC, vf21, vf1y");
    asm("vmaddaz.xyzw ACC, vf22, vf1z");
    asm("vmaddw.xyzw vf2, vf23, vf0w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(c1));
    c2 = &corner[2];
    corner[2].x = -half_w; corner[2].y =  half_h; corner[2].z = 0.0f; corner[2].w = 1.0f;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(c2));
    asm("vmulax.xyzw ACC, vf20, vf1x");
    asm("vmadday.xyzw ACC, vf21, vf1y");
    asm("vmaddaz.xyzw ACC, vf22, vf1z");
    asm("vmaddw.xyzw vf2, vf23, vf0w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(c2));
    c3 = &corner[3];
    corner[3].x =  half_w; corner[3].y =  half_h; corner[3].z = 0.0f; corner[3].w = 1.0f;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(c3));
    asm("vmulax.xyzw ACC, vf20, vf1x");
    asm("vmadday.xyzw ACC, vf21, vf1y");
    asm("vmaddaz.xyzw ACC, vf22, vf1z");
    asm("vmaddw.xyzw vf2, vf23, vf0w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(c3));

    k = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(k));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(k));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(k));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(k));

    scr = &D_70003600;
    cp = &clip0;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(c0));
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

    scr = &D_70003610;
    cp = &clip1;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(c1));
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

    scr = &D_70003620;
    cp = &clip2;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(c2));
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

    cross = (D_70003610.x - D_70003600.x) * (D_70003620.y - D_70003610.y)
          - (D_70003610.y - D_70003600.y) * (D_70003620.x - D_70003610.x);
    if (cross < 0) {
        return;
    }
    func_001CE300(1, corner, 0x2004290511322469ULL, rgba);
}
