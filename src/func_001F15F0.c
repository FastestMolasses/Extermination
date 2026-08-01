// NEARMISS func_001F15F0  (vram 0x001F15F0, 0x2D0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 46.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
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
typedef struct { Vec4 c0, c1, c2, c3; } Mtx4;

extern char *D_00275670;            /* per-frame render context   */
extern char *D_00275B40;            /* published node-pointer table */
extern char  D_007635C0[];          /* display-list page table      */

extern int   func_001CCF70(char *desc);                 /* -> GS page, 0xFFFFFF bad */
extern char *func_001CB5F0(char *pages, int page, int qwords);
extern void  func_001CB900(char *pages, int page, int kind);

/* VU0 leaves (byte-matched; see their src files). */
extern void func_00102900(Vec4 *dst, const Vec4 *src, float s);  /* dst = src*s   */
extern void func_00102990(void *dst, const Vec4 *src);           /* dst = vftoi0  */

static void vu0_project_xyzf2(const Mtx4 *cam, const Vec4 *fog,
                              const Vec4 *p, u32 *out);

void func_001F15F0(char *actor, int node, int samp)
{
    char *desc = *(char **)(actor + 0x1F0);
    int   page = func_001CCF70(desc + 0x30);
    const Mtx4 *cam;
    const Vec4 *fog;
    Vec4 *scratch0 = (Vec4 *)0x70003600;
    Vec4 *scratch1 = (Vec4 *)0x70003610;
    char *blk;
    int   node0 = node, samp0 = samp;
    int   iter  = 0;

    if (page == 0xFFFFFF) {
        return;                                  /* texture page not resident */
    }

    fog = (const Vec4 *)(D_00275670 + 0xA0);     /* -> vf23 */
    cam = (const Mtx4 *)0x70003AC0;              /* -> vf28..vf31 */

    blk = func_001CB5F0(D_007635C0, page, 0x3E);

    ((u64 *)blk)[0] = 0;                         /* VIFcode NOP, NOP */
    ((u64 *)blk)[1] = 0;                         /* VIFcode NOP      */
    *(u32 *)(blk + 0x0C) = 0x50000000 | (0x3E - 1);   /* DIRECT, 61 qwords */
    blk += 0x10;

    *(u64 *)(blk + 0x00) = 0x402640000000800FULL;     /* GIFtag lo */
    *(u64 *)(blk + 0x08) = 0x0000000000004141ULL;     /* REGS      */
    blk += 0x10;

    do {
        char  *hist  = *(char **)(D_00275B40 + node * 4);
        float *fadep = (float *)(hist + samp * 4 + 0xA0);
        float  fade  = *fadep;
        float  scale = fade / 10.0f;

        func_00102900(scratch0, (const Vec4 *)(actor + 0x220), scale);
        func_00102900(scratch1, (const Vec4 *)(actor + 0x230), scale);

        func_00102990(blk + 0x00, scratch0);     /* RGBAQ, vertex 0 */
        func_00102990(blk + 0x20, scratch1);     /* RGBAQ, vertex 1 */

        vu0_project_xyzf2(cam, fog, (const Vec4 *)(hist + samp * 0x10),
                          (u32 *)(blk + 0x10));
        vu0_project_xyzf2(cam, fog, (const Vec4 *)(hist + (samp + 1) * 0x10),
                          (u32 *)(blk + 0x30));

        fade = *fadep;
        blk += 0x40;                             /* bumped on BOTH paths */

        if (fade > 0.0f) {
            *fadep = fade - 1.0f;                /* age the sample */
        } else {
            /* See the CAVEAT above: post-advance cursor, so these land in the
             * next block. Reproduced verbatim. */
            *(u32 *)(blk + 0x1C) |= 0x8000;      /* GS ADC bit */
            *(u32 *)(blk + 0x2C) |= 0x8000;
        }

        samp -= 2;
        if (samp < 0) {
            samp = 8;
            node -= 1;
            if (node < 0) {
                node = 2;
            }
        }
        iter++;
        /* The .s skips this test on the first pass (`beqz s0`), i.e. do/while. */
    } while (node != node0 || samp != samp0);

    func_001CB900(D_007635C0, page, 2);
}

/* --- VU0 screen kernel, written scalar ---------------------------------- */

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
    if (f > fog->x) f = fog->x;         /* vminix.w (fog.x == 255.0) */
    if (f < 0.0f)   f = 0.0f;           /* vmaxx.w  (vf0.x  == 0.0)  */

    out[0] = (u32)(int)(x * 16.0f);     /* vftoi4: 12.4 screen X */
    out[1] = (u32)(int)(y * 16.0f);     /*         12.4 screen Y */
    out[2] = (u32)(int)(z * 16.0f);     /*         GS Z (bits 68..91) */
    out[3] = (u32)(int)(f * 16.0f);     /*         F (bits 100..107), ADC at 15 */
}
