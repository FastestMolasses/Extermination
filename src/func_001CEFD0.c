// NEARMISS func_001CEFD0  (vram 0x001CEFD0, 0x348 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned int       u32;
typedef unsigned long long u64;

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 r[4]; }        Mtx4;   /* rows; r[3] is the translation row */

/* EE scratchpad working set used by this routine. */
#define SPR_PROJ_I  ((int   *)0x70003600)  /* vftoi4 output: X, Y, Z, F (12.4)     */
#define SPR_PROJ_F  ((float *)0x70003600)  /* words 0/1 rewritten as centred floats */
#define SPR_COLOUR  ((float *)0x70003610)  /* faded colour, then vftoi0'd in place  */
#define SPR_XY      ((int   *)0x70003620)  /* 12.4 GS position handed to the sprite */
#define SPR_TMP0    (*(float *)0x70003A20) /* float scratch                         */
#define SPR_TMP1    (*(float *)0x70003A24) /* float scratch                         */
#define SPR_MTX_VP  ((Mtx4  *)0x70003AC0)  /* view-projection (vf28..vf31)          */

extern char *D_00275670;
extern char  D_007635C0;                   /* flare display-list page               */

/* 7-entry .rodata tables, copied to the stack as 0x1C bytes each. */
extern const float D_00251370[7];          /* position factor along the centre axis */
extern const float D_00251390[7];          /* sprite size                           */

extern char *func_001CD370(int which);     /* -> D_00275670 + (which<<6) + 0x2240   */
extern float func_0011E748(float x);       /* sqrtf (libm domain-error wrapper)     */
extern int   float_to_int(float x);
extern void  func_00102900(void *dst, const void *src, float s); /* dst = src * s  */
extern void  func_00102990(void *dst, const void *src);          /* dst = vftoi0(src) */
extern void  func_001CF320(int *xy12_4, float *rgba_i, u64 tex0, float size);
extern void  func_001CB900(char *page, int sortkey, int tag);

void func_001CEFD0(const Vec4 *pos, const float *rgba)
{
    float posFactor[7];      /* stack copy of D_00251370 (sp+0x30) */
    float size[7];           /* stack copy of D_00251390 (sp+0x50) */
    Vec4  clip;              /* sp+0x70 — pre-divide vector; the original stores it
                                but never reads it back */
    Mtx4  objm;              /* vf24..vf27 */
    Mtx4  vpm;               /* vf28..vf31 */
    Vec4  fog;               /* vf23 — LIVE-IN, see the header note */
    Vec4  xf;
    float cx, cy, r, t, intensity;
    int   i;

    __builtin_memcpy(posFactor, D_00251370, 0x1C);
    __builtin_memcpy(size,      D_00251390, 0x1C);

    objm = *(Mtx4 *)func_001CD370(2);

    /* ---- 1. frustum cull (vclipw.xyz + cfc2 $vi18, low 6 bits) -------------- */
    xf.x = objm.r[0].x * pos->x + objm.r[1].x * pos->y + objm.r[2].x * pos->z + objm.r[3].x;
    xf.y = objm.r[0].y * pos->x + objm.r[1].y * pos->y + objm.r[2].y * pos->z + objm.r[3].y;
    xf.z = objm.r[0].z * pos->x + objm.r[1].z * pos->y + objm.r[2].z * pos->z + objm.r[3].z;
    xf.w = objm.r[0].w * pos->x + objm.r[1].w * pos->y + objm.r[2].w * pos->z + objm.r[3].w;
    if (!(-xf.w <= xf.x && xf.x <= xf.w) ||
        !(-xf.w <= xf.y && xf.y <= xf.w) ||
        !(-xf.w <= xf.z && xf.z <= xf.w)) {
        return;                                   /* light is off-screen */
    }

    /* ---- 2. project to screen, apply the fog ramp, convert to 12.4 --------- */
    vpm = *SPR_MTX_VP;
    clip.x = vpm.r[0].x * pos->x + vpm.r[1].x * pos->y + vpm.r[2].x * pos->z + vpm.r[3].x;
    clip.y = vpm.r[0].y * pos->x + vpm.r[1].y * pos->y + vpm.r[2].y * pos->z + vpm.r[3].y;
    clip.z = vpm.r[0].z * pos->x + vpm.r[1].z * pos->y + vpm.r[2].z * pos->z + vpm.r[3].z;
    clip.w = vpm.r[0].w * pos->x + vpm.r[1].w * pos->y + vpm.r[2].w * pos->z + vpm.r[3].w;

    fog = *(Vec4 *)(D_00275670 + 0xA0);   /* NOT loaded here — vf23 is live-in */

    {
        float q = 1.0f / clip.w;                  /* vdiv Q, vf0w, vf2w */
        xf.x = clip.x * q;
        xf.y = clip.y * q;
        xf.z = clip.z * q;
        xf.w = fog.z + fog.w * clip.w;            /* vmulaz.w + vmaddw.w  */
        if (xf.w > fog.x) { xf.w = fog.x; }       /* vminix.w             */
        if (xf.w < 0.0f)  { xf.w = 0.0f; }        /* vmaxx.w  (vf0.x = 0) */
        SPR_PROJ_I[0] = (int)(xf.x * 16.0f);      /* vftoi4.xyzw          */
        SPR_PROJ_I[1] = (int)(xf.y * 16.0f);
        SPR_PROJ_I[2] = (int)(xf.z * 16.0f);
        SPR_PROJ_I[3] = (int)(xf.w * 16.0f);
    }

    /* ---- 3. fog gate: F must land inside the GS 8-bit range ---------------- */
    if (SPR_PROJ_I[3] < 0)      { return; }
    if (SPR_PROJ_I[3] >= 0x100) { return; }

    /* ---- 4. centred screen pixels, distance and fade ----------------------- */
    cx = (float)((SPR_PROJ_I[0] >> 4) - 0x800);
    cy = (float)((SPR_PROJ_I[1] >> 4) - 0x800);
    SPR_PROJ_F[0] = cx;
    SPR_PROJ_F[1] = cy;

    r = func_0011E748(cx * cx + cy * cy);         /* mula.s / madd.s then sqrtf */
    SPR_TMP0 = r;
    t = r / 393.84616f;                           /* 0x43C4EC4F */
    SPR_TMP0 = t;
    if (t > 1.0f) { t = 1.0f; }                   /* clamp via c.le.s / bc1t   */
    SPR_TMP0 = t;
    intensity = 1.0f - t;
    SPR_TMP0 = intensity;

    func_00102900(SPR_COLOUR, rgba, intensity);   /* colour = rgba * intensity */
    func_00102990(SPR_COLOUR, SPR_COLOUR);        /* float -> GS integer RGBA  */

    /* ---- 5. the seven flare sprites ---------------------------------------- */
    for (i = 0; i < 7; i++) {
        u64 tex0;

        SPR_TMP0 = posFactor[i];
        SPR_TMP1 = size[i];

        SPR_XY[0] = (float_to_int(SPR_PROJ_F[0] * SPR_TMP0) + 0x800) << 4;
        SPR_XY[1] = (float_to_int(SPR_PROJ_F[1] * SPR_TMP0) + 0x800) << 4;

        /* Two alternating GS TEX0_1 values (they differ only in TBP0). */
        if (i & 1) {
            tex0 = ((u64)0x20045F05u << 32) | 0x99421EB8u;
        } else {
            tex0 = ((u64)0x20045DA5u << 32) | 0x99421EA8u;
        }

        func_001CF320(SPR_XY, SPR_COLOUR, tex0, SPR_TMP1);
    }

    func_001CB900(&D_007635C0, 0xFFF000, 2);
}
