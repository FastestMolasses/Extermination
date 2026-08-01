// NEARMISS func_001CDDC0  (vram 0x001CDDC0, 0x540 bytes) — readable decompilation, NOT byte-identical.
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

/* EE scratchpad (0x70000000) working set used by this renderer. */
#define SPR_MTX_VP ((Mtx4  *)0x70003AC0)  /* view-projection (vf28..vf31)         */
#define SPR_XYZF   ((int   *)0x70003600)  /* projected XYZF2, then reused as RGBAQ */

/* Clipper buffers: 16 slots each, stride 0x50. */
typedef struct {
    Vec4  pos;          /* +0x00 */
    float pad0[8];      /* +0x10 */
    float u;            /* +0x30 */
    float v;            /* +0x34 */
    float pad1[6];      /* +0x38 */
} ClipVtx;              /* 0x50 bytes */

extern ClipVtx D_008112C0[16];            /* clipper input                        */
extern ClipVtx D_008117C0[16];            /* clipper output                       */

extern char  *D_00275670;                 /* renderer context block               */
extern char   D_0028F700[];               /* base of the display-list arena       */

extern char  *func_001CD370(int which);   /* -> D_00275670 + (which<<6) + 0x2240  */
extern int    func_001CCF70(Vec4 *p);     /* project + clip test; GS Z or 0xFFFFFF */
extern int    func_001CF470(ClipVtx *in, char *mtx);  /* frustum clip -> D_008117C0 */
extern char  *func_001CB5F0(char *page, int sortkey, int qwc);
extern void   func_001CB950(char *page, int sortkey, u64 tex0);
extern void   func_001CB900(char *page, int sortkey, int tag);

/* One fan vertex as the GS sees it: 3 PACKED GIF qwords, REGS = ST, RGBAQ, XYZF2. */
typedef struct {
    float s, t;            /* +0x00  S = u/w, T = v/w                             */
    float q;               /* +0x08  Q = 1/w                                      */
    u32   pad;             /* +0x0C  written as the bit pattern of 1.0f           */
    u32   rgbaq[4];        /* +0x10  R, G, B, A                                   */
    int   xyzf[4];         /* +0x20  X, Y (12.4), GS Z, F                         */
} GsFanVtx;                /* 0x30 bytes */

void func_001CDDC0(int layer, int mode, Vec4 *corners, u64 tex0, u32 rgba)
{
    char    *page;
    char    *blk;
    GsFanVtx *out;
    Vec4    *fogvec;       /* vf23: (fogmax, -, fogbias, fogscale) */
    Vec4     clip;         /* pre-divide clip-space vertex */
    Vec4     xf;
    int      sortkey;
    int      fog;
    int      tri, i, n, ci;

    page   = (char *)D_0028F700 + 0x4D3EC0 + (layer << 15);
    fogvec = (Vec4 *)(D_00275670 + 0xA0);

    /* 1. Depth sort key = the largest GS Z over the four corners. */
    sortkey = func_001CCF70(&corners[0]);
    for (i = 1; i < 4; i++) {
        int z = func_001CCF70(&corners[i]);
        if (sortkey < z) {
            sortkey = z;
        }
    }

    /* 2. Re-project corner 0 for its fog value. */
    {
        Vec4 *v = &corners[0];
        clip.x = SPR_MTX_VP->r[0].x * v->x + SPR_MTX_VP->r[1].x * v->y
               + SPR_MTX_VP->r[2].x * v->z + SPR_MTX_VP->r[3].x;
        clip.y = SPR_MTX_VP->r[0].y * v->x + SPR_MTX_VP->r[1].y * v->y
               + SPR_MTX_VP->r[2].y * v->z + SPR_MTX_VP->r[3].y;
        clip.z = SPR_MTX_VP->r[0].z * v->x + SPR_MTX_VP->r[1].z * v->y
               + SPR_MTX_VP->r[2].z * v->z + SPR_MTX_VP->r[3].z;
        clip.w = SPR_MTX_VP->r[0].w * v->x + SPR_MTX_VP->r[1].w * v->y
               + SPR_MTX_VP->r[2].w * v->z + SPR_MTX_VP->r[3].w;

        xf.x = clip.x / clip.w;
        xf.y = clip.y / clip.w;
        xf.z = clip.z / clip.w;
        xf.w = fogvec->z + fogvec->w * clip.w;
        if (xf.w > fogvec->x) { xf.w = fogvec->x; }
        if (xf.w < 0.0f)      { xf.w = 0.0f;      }

        /* vftoi4: value * 16, truncated. */
        SPR_XYZF[0] = (int)(xf.x * 16.0f);
        SPR_XYZF[1] = (int)(xf.y * 16.0f);
        SPR_XYZF[2] = (int)(xf.z * 16.0f);
        SPR_XYZF[3] = (int)(xf.w * 16.0f);
    }

    /* 3. Fog-modulate the colour, then park it in the scratchpad quadword. */
    if (mode != 0) {
        fog = SPR_XYZF[3] >> 4;
        if (fog >= 0x100) { fog = 0xFF; }
        if (fog < 0)      { fog = 0;    }

        if (mode == 1) {
            u32 a = (rgba >> 24) & 0xFF;
            a = (a * fog) >> 8;
            rgba = (rgba & 0x00FFFFFF) | (a << 24);
        } else if (mode == 2 || mode == 3 || mode == 4) {
            u32 r = (rgba >>  0) & 0xFF;
            u32 g = (rgba >>  8) & 0xFF;
            u32 b = (rgba >> 16) & 0xFF;
            r = (r * fog) >> 8;
            g = (g * fog) >> 8;   /* the original runs this one on MMI `mult1` */
            b = (b * fog) >> 8;
            SPR_XYZF[3] = 0xFF0;  /* DEAD: overwritten by the alpha lane below */
            rgba = (rgba & 0xFF000000) | (b << 16) | (g << 8) | r;
        }
    }
    SPR_XYZF[0] = (rgba >>  0) & 0xFF;      /* R */
    SPR_XYZF[1] = (rgba >>  8) & 0xFF;      /* G */
    SPR_XYZF[2] = (rgba >> 16) & 0xFF;      /* B */
    SPR_XYZF[3] = (rgba >> 24) & 0xFF;      /* A */

    /* 4. Two triangles: (0,1,2) and (1,2,3). */
    for (tri = 0; tri < 2; tri++) {
        for (i = 0; i < 3; i++) {
            ci = tri + i;
            D_008112C0[i].pos = corners[ci];
            switch (ci) {
            case 0:  D_008112C0[i].u = 0.0f; D_008112C0[i].v = 0.0f; break;
            case 1:  D_008112C0[i].u = 0.0f; D_008112C0[i].v = 1.0f; break;
            case 2:  D_008112C0[i].u = 1.0f; D_008112C0[i].v = 0.0f; break;
            case 3:  D_008112C0[i].u = 1.0f; D_008112C0[i].v = 1.0f; break;
            }
        }

        n = func_001CF470(D_008112C0, func_001CD370(0));
        if (n == 0) {
            continue;
        }

        blk = func_001CB5F0(page, sortkey, 3 * n + 2);

        /* VIF quad: three NOPs then DIRECT(GIFtag + 3*n data qwords). */
        *(int *)(blk + 0x00) = 0;
        *(int *)(blk + 0x04) = 0;
        *(int *)(blk + 0x08) = 0;
        *(int *)(blk + 0x0C) = 0x50000000 | (3 * n + 1);

        /* GIFtag: NLOOP=n EOP=1 PRE=1 PRIM=0x5D FLG=PACKED NREG=3. */
        *(u64 *)(blk + 0x10) = 0x302EC00000008000ULL | (u64)(unsigned)n;
        *(u64 *)(blk + 0x18) = 0x412ULL;            /* REGS: ST, RGBAQ, XYZF2 */

        out = (GsFanVtx *)(blk + 0x20);

        for (i = 0; i < n; i++) {
            Vec4 *v = &D_008117C0[i].pos;
            float invw;

            clip.x = SPR_MTX_VP->r[0].x * v->x + SPR_MTX_VP->r[1].x * v->y
                   + SPR_MTX_VP->r[2].x * v->z + SPR_MTX_VP->r[3].x;
            clip.y = SPR_MTX_VP->r[0].y * v->x + SPR_MTX_VP->r[1].y * v->y
                   + SPR_MTX_VP->r[2].y * v->z + SPR_MTX_VP->r[3].y;
            clip.z = SPR_MTX_VP->r[0].z * v->x + SPR_MTX_VP->r[1].z * v->y
                   + SPR_MTX_VP->r[2].z * v->z + SPR_MTX_VP->r[3].z;
            clip.w = SPR_MTX_VP->r[0].w * v->x + SPR_MTX_VP->r[1].w * v->y
                   + SPR_MTX_VP->r[2].w * v->z + SPR_MTX_VP->r[3].w;

            xf.x = clip.x / clip.w;
            xf.y = clip.y / clip.w;
            xf.w = clip.w - 1.0f;              /* depth bias toward the camera  */
            xf.z = clip.z / xf.w;
            xf.w = fogvec->z + fogvec->w * xf.w;
            if (xf.w > fogvec->x) { xf.w = fogvec->x; }
            if (xf.w < 0.0f)      { xf.w = 0.0f;      }

            out[i].xyzf[0] = (int)(xf.x * 16.0f);
            out[i].xyzf[1] = (int)(xf.y * 16.0f);
            out[i].xyzf[2] = (int)(xf.z * 16.0f);
            out[i].xyzf[3] = (int)(xf.w * 16.0f);

            invw = 1.0f / clip.w;              /* pre-divide w, from the stack  */
            out[i].rgbaq[0] = (u32)SPR_XYZF[0];
            out[i].rgbaq[1] = (u32)SPR_XYZF[1];
            out[i].rgbaq[2] = (u32)SPR_XYZF[2];
            out[i].rgbaq[3] = (u32)SPR_XYZF[3];

            out[i].s   = D_008117C0[i].u * invw;
            out[i].t   = D_008117C0[i].v * invw;
            out[i].q   = invw;
            out[i].pad = 0x3F800000;           /* 1.0f, unused by the GS        */
        }
    }

    /* 5. Layer state, pushed after the geometry so it lands in front of it. */
    func_001CB950(page, sortkey, tex0);
    func_001CB900(page, sortkey, mode);
}
