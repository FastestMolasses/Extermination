// NEARMISS func_001CE300  (vram 0x001CE300, 0x358 bytes) — readable decompilation, NOT byte-identical.
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

typedef unsigned int        u32;
typedef unsigned long long  u64;
typedef unsigned __int128   uint128;

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 r[4]; }        Mtx4;   /* rows; r[3] is the translation row */

/* Staging / clipper vertex record; only these three fields are touched here. */
typedef struct {
    /* 0x00 */ Vec4  pos;
    /* 0x10 */ char  pad10[0x30 - 0x10];
    /* 0x30 */ float s;
    /* 0x34 */ float t;
    /* 0x38 */ char  pad38[0x50 - 0x38];
} ClipVtx;                                   /* 0x50 bytes */

/* One PACKED GIF vertex group: { ST, RGBAQ, XYZF2 }. */
typedef struct {
    /* 0x00 */ float s, t;                   /* s/w, t/w                          */
    /* 0x08 */ float q;                      /* 1/w                               */
    /* 0x0C */ float pad;                    /* written as 1.0f                   */
    /* 0x10 */ u32   rgbaq[4];               /* R, G, B, A as ints                */
    /* 0x20 */ int   xyzf[4];                /* 12.4 X, 12.4 Y, GS Z, F           */
} GsVtx;                                     /* 0x30 bytes */

#define SPR_RGBA    ((int  *)0x70003600)     /* unpacked colour, blitted per vertex */
#define SPR_MTX_VP  ((Mtx4 *)0x70003AC0)     /* view-projection (vf28..vf31)        */

extern char *D_00275670;                     /* renderer context block              */
extern char  D_007635C0;                     /* display-list page for this layer    */
extern ClipVtx D_008112C0[3];                /* clipper INPUT triangle              */
extern ClipVtx D_008117C0[];                 /* clipper OUTPUT fan                  */

extern char *func_001CD370(int which);       /* -> D_00275670 + (which<<6) + 0x2240 */
extern int   func_001CF470(ClipVtx *tri, const Mtx4 *objm);  /* -> out vertex count */
extern char *func_001CB5F0(char *page, int sortkey, int qwc);
extern char *func_001CB950(char *page, int sortkey, u64 tex0);
extern void  func_001CB900(char *page, int sortkey, int tag);

void func_001CE300(int tag, const Vec4 *quad, u64 tex0, u32 rgba)
{
    Mtx4 vpm;                 /* vf28..vf31 */
    Vec4 fog;                 /* vf23: (fogmax, -, fogbias, fogscale) */
    Vec4 clip;                /* sp+0x70 staging of the pre-divide vector */
    int  pass, j, k;

    SPR_RGBA[0] = (int)(rgba & 0xFF);
    SPR_RGBA[1] = (int)((rgba >> 8) & 0xFF);
    SPR_RGBA[2] = (int)((rgba >> 16) & 0xFF);
    SPR_RGBA[3] = (int)((rgba >> 24) & 0xFF);

    for (pass = 0; pass < 2; pass++) {
        char *p;
        GsVtx *v;
        int count, qwc;

        /* --- stage triangle {pass+0, pass+1, pass+2} with corner-keyed UVs --- */
        for (j = 0; j < 3; j++) {
            int corner = pass + j;

            D_008112C0[j].pos = quad[corner];

            /* The original is a beq chain on `corner`; anything outside 0..3
               would leave S/T untouched, but corner is always 0..3 here. */
            if (corner == 3) {
                D_008112C0[j].s = 1.0f;  D_008112C0[j].t = 1.0f;
            } else if (corner == 2) {
                D_008112C0[j].s = 1.0f;  D_008112C0[j].t = 0.0f;
            } else if (corner == 1) {
                D_008112C0[j].s = 0.0f;  D_008112C0[j].t = 1.0f;
            } else if (corner == 0) {
                D_008112C0[j].s = 0.0f;  D_008112C0[j].t = 0.0f;
            }
        }

        /* --- frustum clip; D_008117C0 receives the resulting fan ------------ */
        count = func_001CF470(D_008112C0, (const Mtx4 *)func_001CD370(0));
        if (count == 0) {
            continue;
        }

        /* --- allocate and head the GIF packet ------------------------------- */
        qwc = count * 3 + 2;
        p = func_001CB5F0(&D_007635C0, 0, qwc);

        *(uint128 *)(p + 0x00) = 0;                       /* 3x VIFNOP           */
        *(u32 *)(p + 0x0C) = 0x50000000u | (u32)(qwc - 1); /* DIRECT(qwc-1)      */

        /* NLOOP=count, EOP=1, PRE=1, PRIM=TRIANGLE_FAN|IIP|TME|FGE|ABE,
           FLG=PACKED, NREG=3. */
        *(u64 *)(p + 0x10) = ((u64)0x303EC000u << 32) | 0x8000u | (u64)(long)count;
        *(u64 *)(p + 0x18) = 0x412ULL;                    /* ST, RGBAQ, XYZF2    */

        v = (GsVtx *)(p + 0x20);

        fog = *(Vec4 *)(D_00275670 + 0xA0);               /* lqc2 vf23           */
        vpm = *SPR_MTX_VP;                                /* lqc2 vf28..vf31     */

        /* --- transform every clipped vertex --------------------------------- */
        for (k = 0; k < count; k++) {
            const ClipVtx *src = &D_008117C0[k];
            float invw, wbias, q, f;

            clip.x = vpm.r[0].x * src->pos.x + vpm.r[1].x * src->pos.y
                   + vpm.r[2].x * src->pos.z + vpm.r[3].x;
            clip.y = vpm.r[0].y * src->pos.x + vpm.r[1].y * src->pos.y
                   + vpm.r[2].y * src->pos.z + vpm.r[3].y;
            clip.z = vpm.r[0].z * src->pos.x + vpm.r[1].z * src->pos.y
                   + vpm.r[2].z * src->pos.z + vpm.r[3].z;
            clip.w = vpm.r[0].w * src->pos.x + vpm.r[1].w * src->pos.y
                   + vpm.r[2].w * src->pos.z + vpm.r[3].w;

            q     = 1.0f / clip.w;            /* vdiv Q, vf0w, vf2w             */
            wbias = clip.w - 1.0f;            /* vsubx.w with vf3.x = 1.0f      */

            f = fog.z + fog.w * wbias;        /* vmulaz.w + vmaddw.w            */
            if (f > fog.x) { f = fog.x; }     /* vminix.w                       */
            if (f < 0.0f)  { f = 0.0f; }      /* vmaxx.w (vf0.x == 0)           */

            v->xyzf[0] = (int)((clip.x * q)     * 16.0f);   /* vftoi4.xyzw      */
            v->xyzf[1] = (int)((clip.y * q)     * 16.0f);
            v->xyzf[2] = (int)((clip.z / wbias) * 16.0f);
            v->xyzf[3] = (int)(f                * 16.0f);

            invw = 1.0f / clip.w;             /* recomputed on the FPU (div.s)  */

            *(uint128 *)v->rgbaq = *(uint128 *)SPR_RGBA;
            v->s   = src->s * invw;
            v->t   = src->t * invw;
            v->q   = invw;
            v->pad = 1.0f;

            v++;
        }
    }

    func_001CB950(&D_007635C0, 0, tex0);      /* A+D: tex0 -> GS TEX0_1 */
    func_001CB900(&D_007635C0, 0, tag);       /* close the page         */
}
