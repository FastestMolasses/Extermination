// NEARMISS func_001CD940  (vram 0x001CD940, 0x480 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 22.80% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned __int128 uint128;

extern char *D_00275670;
extern char  D_00251240[];      /* static GIF state packet chased in step 6 */
extern char  D_007635C0[];      /* the line bucket's page table */

extern char *func_001CD370(int camera);
extern char *func_001CB5F0(int page_table, int z, int nquads);
extern void  func_001CB6B0(int page_table, int z, int kind, unsigned long long addr);
extern void  func_001CB900(int page_table, int z, int mode);

extern volatile float D_70003AC0[16];   /* world -> screen matrix (scratchpad) */

/* ---- EE scratchpad staging (0x70000000 is the SPR) ------------------------ */
/* Two projected endpoints, 28.4 fixed point: x, y, z, fog. */
#define P0  ((volatile int *)0x70003600)
#define P1  ((volatile int *)0x70003610)
/* Two colours, four 32-bit words each: r, g, b, a. */
#define C0  ((volatile int *)0x70003620)
#define C1  ((volatile int *)0x70003630)

typedef struct Vec4 {
    float x, y, z, w;
} Vec4;

/* vmulax/vmadday/vmaddaz/vmaddw ACC chain: row-vector point transform,
   with w taken from vf0.w == 1.0 rather than from the source vector. */
static Vec4 vu0_transform_point(const volatile float *m, const Vec4 *v)
{
    Vec4 r;
    r.x = m[0] * v->x + m[4] * v->y + m[ 8] * v->z + m[12];
    r.y = m[1] * v->x + m[5] * v->y + m[ 9] * v->z + m[13];
    r.z = m[2] * v->x + m[6] * v->y + m[10] * v->z + m[14];
    r.w = m[3] * v->x + m[7] * v->y + m[11] * v->z + m[15];
    return r;
}

/* vclipw.xyz + cfc2 $vi18: 1 if any of x, y, z falls outside +/-w. */
static int vu0_clip_outside(const Vec4 *p)
{
    return p->x >  p->w || p->x < -p->w
        || p->y >  p->w || p->y < -p->w
        || p->z >  p->w || p->z < -p->w;
}

/* vftoi4: float -> 28.4 fixed point, truncating toward zero. */
static int vu0_ftoi4(float f)
{
    return (int)(f * 16.0f);
}

/* Project one endpoint into `out` (x, y, z, fog), all 28.4 fixed point. */
static void project_endpoint(const volatile float *fogp, const Vec4 *world,
                             volatile int *out)
{
    Vec4 cs = vu0_transform_point(D_70003AC0, world);

    cs.x /= cs.w;
    cs.y /= cs.w;
    cs.w -= 1.0f;                        /* hard-coded depth bias */
    cs.z /= cs.w;
    cs.w = fogp[2] + fogp[3] * cs.w;     /* vmulaz.w / vmaddw.w */
    if (cs.w > fogp[0]) { cs.w = fogp[0]; }   /* vmini.w */
    if (cs.w < 0.0f)    { cs.w = 0.0f; }      /* vmax.w against vf0.x == 0 */

    out[0] = vu0_ftoi4(cs.x);
    out[1] = vu0_ftoi4(cs.y);
    out[2] = vu0_ftoi4(cs.z);
    out[3] = vu0_ftoi4(cs.w);
}

/* Fog-modulate one staged colour in place; `pos[3]` is that endpoint's fog word,
   which is clamped in place and then pinned to full. */
static void modulate_colour(int mode, volatile int *pos, volatile int *col)
{
    pos[3] = pos[3] >> 4;
    if (pos[3] >= 0x100) { pos[3] = 0xFF; }
    if (pos[3] < 0)      { pos[3] = 0; }

    if (mode == 1) {
        col[3] = (col[3] * pos[3]) >> 8;          /* alpha only */
    } else if (mode == 3 || mode == 2) {
        col[0] = (col[0] * pos[3]) >> 8;          /* r */
        col[1] = (col[1] * pos[3]) >> 8;          /* g — issued on `mult1` */
        col[2] = (col[2] * pos[3]) >> 8;          /* b */
    }
    /* every non-zero mode ends with full fog in the emitted vertex */
    pos[3] = 0xFF0;
}

void func_001CD940(int mode, Vec4 *p0, uint128 *c0, Vec4 *p1, uint128 *c1)
{
    const volatile float *cull;
    const volatile float *fogp;
    Vec4 clip;
    char *prim;

    /* 1. reject the whole line if either endpoint is outside the frustum. */
    cull = (const volatile float *)func_001CD370(0);
    clip = vu0_transform_point(cull, p0);
    if (vu0_clip_outside(&clip)) {
        return;
    }
    clip = vu0_transform_point(cull, p1);
    if (vu0_clip_outside(&clip)) {
        return;
    }

    /* 2. project both endpoints. */
    fogp = (const volatile float *)(D_00275670 + 0xA0);
    project_endpoint(fogp, p0, P0);
    project_endpoint(fogp, p1, P1);

    /* 3. stage both colours. */
    *(volatile uint128 *)C0 = *c0;
    *(volatile uint128 *)C1 = *c1;

    /* 4. per-endpoint fog modulation. */
    if (mode != 0) {
        modulate_colour(mode, P0, C0);
    }
    if (mode != 0) {
        modulate_colour(mode, P1, C1);
    }

    /* 5. append the primitive, keyed on endpoint 0's screen Z. */
    prim = func_001CB5F0((int)D_007635C0, P0[2], 4);
    *(uint128 *)(prim + 0x00) = *(volatile uint128 *)C0;
    *(uint128 *)(prim + 0x10) = *(volatile uint128 *)P0;
    *(uint128 *)(prim + 0x20) = *(volatile uint128 *)C1;
    *(uint128 *)(prim + 0x30) = *(volatile uint128 *)P1;

    /* 6. trailing state + blend-mode command words. */
    func_001CB6B0((int)D_007635C0, P0[2], 2,
                  (unsigned long long)(unsigned int)D_00251240);
    func_001CB900((int)D_007635C0, P0[2], mode);
}
