// NEARMISS func_001E17E0  (vram 0x001E17E0, 0x2EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 13.59% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 c0, c1, c2, c3; } Mtx4;

extern char *D_00275670;                    /* per-frame render context */
extern Mtx4 *func_001CD370(int which);      /* &ctx[0x2240 + which*0x40] */

/* VU0 helpers — see the header for what the real instruction streams are. */
static int  vu0_clip_flags(const Mtx4 *m, const Vec4 *p);
static void vu0_project_xyzf2(const Mtx4 *cam, const Vec4 *fog,
                              const Vec4 *p, u32 *out);

char *func_001E17E0(int chan)
{
    char **cursor = (char **)(D_00275670 + 0x10 + chan * 4);
    char  *first  = *cursor;
    const Mtx4 *clip_mtx = func_001CD370(2);
    const Mtx4 *cam      = (const Mtx4 *)0x70003AC0;
    const Vec4 *fog      = (const Vec4 *)(D_00275670 + 0xA0);  /* already in vf23 */
    int i, j;

    for (i = -8; i < 8; i++) {
        float z0 = 93.75f * (float)i       - 46.875f;
        float z1 = 93.75f * (float)(i + 1) - 46.875f;
        char *pkt;
        u32  *vp;
        int   adc = 0;

        pkt = *cursor;
        *(u16 *)(pkt + 0x00) = 0x0022;              /* DMAtag QWC = 34   */
        *(u8  *)(pkt + 0x03) = 0x10;                /* DMAtag ID  = CNT  */
        *(u32 *)(pkt + 0x04) = 0;                   /* DMAtag ADDR       */
        *cursor = pkt + 0x230;

        ((u64 *)(pkt + 0x10))[0] = 0;               /* VIFcode NOP x2    */
        ((u64 *)(pkt + 0x10))[1] = 0;               /* VIFcode NOP       */
        *(u32 *)(pkt + 0x1C) = 0x50000021;          /* VIFcode DIRECT 33 */
        *(u64 *)(pkt + 0x20) = 0x2002400000008010ULL;   /* GIFtag lo */
        *(u64 *)(pkt + 0x28) = 0x0000000000000044ULL;   /* REGS = XYZF2,XYZF2 */

        vp = (u32 *)(pkt + 0x30);

        for (j = -8; j < 8; j++) {
            float xj = 93.75f * (float)j - 46.875f;
            Vec4  p;

            /* vertex A: (x_j, 10, z_i) */
            adc >>= 1;
            p.x = xj; p.y = 10.0f; p.z = z0; p.w = 1.0f;
            if (vu0_clip_flags(clip_mtx, &p) != 0) {
                adc |= 4;
            }
            vu0_project_xyzf2(cam, fog, &p, vp);
            if (adc != 0) {
                vp[3] = 0xFFFF;                     /* ADC | F = 255 */
            }

            /* vertex B: (x_j, 10, z_(i+1)) */
            adc >>= 1;
            p.x = xj; p.y = 10.0f; p.z = z1; p.w = 1.0f;
            if (vu0_clip_flags(clip_mtx, &p) != 0) {
                adc |= 4;
            }
            vu0_project_xyzf2(cam, fog, &p, vp + 4);
            if (adc != 0) {
                vp[7] = 0xFFFF;
            }

            vp += 8;                                /* two qwords */
        }
    }

    return first;
}

/* --- VU0 kernels, written scalar ---------------------------------------- */

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

static void vu0_project_xyzf2(const Mtx4 *cam, const Vec4 *fog,
                              const Vec4 *p, u32 *out)
{
    float x = cam->c0.x * p->x + cam->c1.x * p->y + cam->c2.x * p->z + cam->c3.x;
    float y = cam->c0.y * p->x + cam->c1.y * p->y + cam->c2.y * p->z + cam->c3.y;
    float z = cam->c0.z * p->x + cam->c1.z * p->y + cam->c2.z * p->z + cam->c3.z;
    float w = cam->c0.w * p->x + cam->c1.w * p->y + cam->c2.w * p->z + cam->c3.w;
    float q = 1.0f / w;                 /* vdiv Q, vf0.w, vf2.w  +  vwaitq */
    float f;

    x *= q;                             /* vmulq.xyz */
    y *= q;
    z *= q;

    f = fog->z + fog->w * w;            /* vmulaz.w + vmaddw.w */
    if (f > fog->x) f = fog->x;         /* vminix.w  (fog.x == 255.0) */
    if (f < 0.0f)   f = 0.0f;           /* vmaxx.w   (vf0.x  == 0.0)  */

    /* vftoi4: fixed point with 4 fractional bits. Lanes 0/1 become the 12.4
     * screen X/Y, lane 2 the GS Z (XYZF2 stores it at bits 68..91, i.e. lane 2
     * shifted left 4 — which is exactly what the x16 gives), lane 3 the fog
     * byte at bits 100..107 plus the ADC flag at bit 111. */
    out[0] = (u32)(int)(x * 16.0f);
    out[1] = (u32)(int)(y * 16.0f);
    out[2] = (u32)(int)(z * 16.0f);
    out[3] = (u32)(int)(f * 16.0f);
}
