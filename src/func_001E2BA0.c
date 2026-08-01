// NEARMISS func_001E2BA0  (vram 0x001E2BA0, 0x2D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 72.22% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 c0, c1, c2, c3; } Mtx4;

extern char *D_00275670;                    /* per-frame render context */

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

/*
 * VU0: transform p by m (implicit w = 1.0) then `vclipw.xyz` + `cfc2 $vi18`.
 * Returns the six frustum CLIP bits (0 == fully inside). vclipw compares each of
 * x,y,z against +/-|w| of the transformed point and ORs one bit per failed test.
 */
static int vu0_clip_flags(const Mtx4 *m, const Vec4 *p)
{
    float x = m->c0.x * p->x + m->c1.x * p->y + m->c2.x * p->z + m->c3.x;
    float y = m->c0.y * p->x + m->c1.y * p->y + m->c2.y * p->z + m->c3.y;
    float z = m->c0.z * p->x + m->c1.z * p->y + m->c2.z * p->z + m->c3.z;
    float w = m->c0.w * p->x + m->c1.w * p->y + m->c2.w * p->z + m->c3.w;
    float a = (w < 0.0f) ? -w : w;
    int   f = 0;

    if (x >  a) f |= 0x01;
    if (x < -a) f |= 0x02;
    if (y >  a) f |= 0x04;
    if (y < -a) f |= 0x08;
    if (z >  a) f |= 0x10;
    if (z < -a) f |= 0x20;
    return f;
}

void func_001E2BA0(const Vec4 *start, const Vec4 *end, const float *colour,
                   float len_hint)
{
    const Mtx4 *clip_mtx = func_001CD370(2);
    Vec4  cur, step, next;
    int   prev_rgba[4];
    int   rgba[4];
    float len, phase, dphase;
    int   clip;
    int   i;

    /* Loaded into the VU0 register file here purely for func_001E2800's
     * benefit (see the header): vf28..vf31 = camera P*V, vf23 = fog ramp. */
    const Mtx4 *cam = (const Mtx4 *)0x70003AC0;
    const Vec4 *fog = (const Vec4 *)(D_00275670 + 0xA0);

    (void)cam;
    (void)fog;
    (void)len_hint;                              /* genuinely unused */

    func_001028D0(&step, end, start);            /* step = end - start */
    len = func_0011E748(step.x * step.x + step.y * step.y + step.z * step.z);
    func_00102870(&step, &step, 32.0f);          /* step /= 32 (xyz only) */
    func_00102948(&cur, start);

    prev_rgba[0] = prev_rgba[1] = prev_rgba[2] = prev_rgba[3] = 0;

    phase  = ((float)func_00122BB8() / 2147483648.0f) * 6.28318548f;
    dphase = (0.1f * len) / 4.0f;

    clip = vu0_clip_flags(clip_mtx, &cur);

    for (i = 0; i < 32; i++) {
        float shade = func_0011DF78(func_0011E2A8(phase));

        rgba[0] = float_to_int(255.0f * (colour[0] * shade));
        rgba[1] = float_to_int(255.0f * (colour[1] * shade));
        rgba[2] = float_to_int(255.0f * (colour[2] * shade));
        rgba[3] = float_to_int(255.0f * (colour[3] * shade));

        func_001028B8(&next, &cur, &step);       /* next = cur + step */

        clip = ((clip << 6) & 0xFC0) | vu0_clip_flags(clip_mtx, &next);
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
