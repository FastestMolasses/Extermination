// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 72.22% NEARMISS).
// Fixes read from the original instructions:
//  - The VU0 work is inline in the original: the clip matrix rows (VU0 24..27),
//    the camera P*V rows (28..31, from the scratchpad camera) and the fog ramp
//    (23) are loaded once at entry, and each point is clipped by a transform,
//    clip-against-w and clip-flag read. It is written as inline asm statements,
//    one instruction each; the per-point clip is a static inline helper (inlined,
//    not emitted) replacing the old scalar emulation.
//  - -sdatathreshold 4: the render-context pointer is gp-relative.
//  - Local order puts the two colour arrays below the vectors in the frame.
//  - The random phase is converted, divided and scaled in place.
//  - The clip history is shifted and masked before the new point's clip, as
//    three compound assignments.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
//
// Draws a shimmering line from start to end as 32 segments: each segment's
// colour is colour * |sin(phase)| (phase starts random and advances by
// 0.1 * length / 4 per segment), and a segment is drawn by func_001E2800 only
// when both of its endpoints pass the VU0 clip test (the previous point's six
// clip flags are kept in bits 6..11).

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 c0, c1, c2, c3; } Mtx4;

extern char *D_00275670;                    /* per-frame render context */
extern Vec4  D_70003AC0[4];                  /* camera P*V, EE scratchpad */

extern Mtx4 *func_001CD370(int which);      /* &ctx[0x2240 + which*0x40] */
extern float func_0011E748(float x);        /* sqrtf */
extern float func_0011E2A8(float a);        /* sinf  */
extern float func_0011DF78(float x);        /* fabsf */
extern int   func_00122BB8(void);           /* engine rand(), 31-bit */
extern int   float_to_int(float x);

/* VU0 leaves (already byte-matched, see their src files). */
extern void func_001028D0(Vec4 *d, const Vec4 *a, const Vec4 *b);  /* d = a - b */
extern void func_001028B8(Vec4 *d, const Vec4 *a, const Vec4 *b);  /* d = a + b */
extern void func_00102870(Vec4 *d, const Vec4 *a, float s);        /* d.xyz=a.xyz/s */
extern void func_00102948(void *d, const void *s);                 /* qword copy */

/* Shaded-line emitter; relies on vf23 / vf28..vf31 still holding the fog ramp
 * and the camera matrix loaded below. */
extern void func_001E2800(int kind, const Vec4 *p0, const int *rgba0,
                          const Vec4 *p1, const int *rgba1);

/* Clip p on VU0 against the matrix rows held in VU0 registers 24..27:
   transform (implicit w = 1), clip x, y, z against w, wait for the flag
   pipeline, and return the six clip flags (0 == fully inside). */
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

void func_001E2BA0(const Vec4 *start, const Vec4 *end, const float *colour,
                   float len_hint)
{
    const Mtx4 *clip_mtx = func_001CD370(2);
    int   prev_rgba[4];
    int   rgba[4];
    Vec4  cur, step, next;
    float len, phase, dphase;
    int   clip;
    int   i;

    const Vec4 *cam;

    (void)len_hint;                              /* genuinely unused */

    /* load the clip matrix into VU0 rows 24..27, and for func_001E2800's
     * benefit the camera P*V into 28..31 and the fog ramp into 23 */
    asm("lqc2 vf24, 0x0(%0)" : : "r"(clip_mtx));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(clip_mtx));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(clip_mtx));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(clip_mtx));
    cam = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(cam));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(cam));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(cam));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(cam));
    asm("lqc2 vf23, 0x0(%0)" : : "r"(D_00275670 + 0xA0));

    func_001028D0(&step, end, start);            /* step = end - start */
    len = func_0011E748(step.x * step.x + step.y * step.y + step.z * step.z);
    func_00102870(&step, &step, 32.0f);          /* step /= 32 (xyz only) */
    func_00102948(&cur, start);

    prev_rgba[0] = prev_rgba[1] = prev_rgba[2] = prev_rgba[3] = 0;

    phase  = (float)func_00122BB8();
    phase /= 2147483648.0f;
    phase *= 6.28318548f;
    dphase = (0.1f * len) / 4.0f;

    clip = vu0_clip(&cur);

    for (i = 0; i < 32; i++) {
        float shade = func_0011DF78(func_0011E2A8(phase));

        rgba[0] = float_to_int(255.0f * (colour[0] * shade));
        rgba[1] = float_to_int(255.0f * (colour[1] * shade));
        rgba[2] = float_to_int(255.0f * (colour[2] * shade));
        rgba[3] = float_to_int(255.0f * (colour[3] * shade));

        func_001028B8(&next, &cur, &step);       /* next = cur + step */

        clip <<= 6;
        clip &= 0xFC0;
        clip |= vu0_clip(&next);
        if (clip == 0) {
            func_001E2800(2, &cur, prev_rgba, &next, rgba);
        }

        func_00102948(&cur, &next);
        func_00102948(prev_rgba, rgba);

        phase += dphase;
        if (phase >= 6.28318548f) {
            phase -= 6.28318548f;
        }
    }
}
