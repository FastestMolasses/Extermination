// NEARMISS func_001CDDC0  (vram 0x001CDDC0, 0x540 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.70% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4), was 0.00%.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Matching wave 2026-10-28 rebuilt it from the original:
//  - corner 0's projection (x, y, z all divided by w, no bias) and each clipped
//    vertex's projection (z divided by w - 1, as func_001E2800) are VU0 macro
//    code inline, one instruction per asm statement; each vertex's Q is 1 / w
//    of the raw transformed vector spilled to the stack, and its colour is a
//    128-bit copy of the scratchpad quadword.
//  - -sdatathreshold 4 (gp-relative render context); the sort-key loop and the
//    triangle loop share one counter (saved-register order); the fade is a
//    switch (2, 3, 4 colour; 1 alpha) with channel locals declared before f.
//  - the VIF header quadword is cleared with one 128-bit store, then its last
//    word set; the GIFtag's NLOOP is the sign-extended vertex count.
// Remaining differences: the w bias (1.0f) reaches VU0 through the fixed
// register t0; the original moves it from the FPU (hazard nop after); mwcc
// 2.3.3 drops a float asm operand, so the bits go through a stack union (frame
// 0xC0, not 0xB0). The triangle-setup loop allocates its address bases and
// case constants to different registers (the original keeps i + j and the
// case address in one register, so its case dispatch uses branch-likely
// compares), and one OR in the colour repack has its operands swapped.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Draws a textured, fogged quad (corners 0..3, UVs (0,0) (0,1) (1,0) (1,1)):
// the sort key is the largest GS Z of the four corners; corner 0's fog value
// fades the colour (mode 1 alpha, modes 2..4 RGB); each of the two triangles
// (0, 1, 2) and (1, 2, 3) goes through the frustum clipper and the surviving
// fan is written as a VIF DIRECT packet of ST / RGBAQ / XYZF2 vertices; the
// layer's TEX0 state and the mode's trailer follow.

typedef unsigned int       u32;
typedef unsigned long long u64;
typedef int u128 __attribute__((mode(TI)));

typedef struct { float x, y, z, w; } Vec4;

/* Clipper vertex: 16 slots each, stride 0x50. */
typedef struct {
    Vec4  pos;          /* +0x00 */
    float pad0[8];      /* +0x10 */
    float u;            /* +0x30 */
    float v;            /* +0x34 */
    float pad1[6];      /* +0x38 */
} ClipVtx;              /* 0x50 bytes */

/* One fan vertex as the GS sees it: 3 PACKED GIF qwords, REGS = ST, RGBAQ, XYZF2. */
typedef struct {
    float s, t;            /* +0x00  S = u/w, T = v/w                  */
    float q;               /* +0x08  Q = 1/w                           */
    int   pad;             /* +0x0C  the bit pattern of 1.0f           */
    u128  rgbaq;           /* +0x10  R, G, B, A                        */
    int   xyzf[4];         /* +0x20  X, Y (12.4), GS Z, F              */
} GsFanVtx;                /* 0x30 bytes */

extern ClipVtx D_008112C0[16];            /* clipper input  */
extern ClipVtx D_008117C0[16];            /* clipper output */

extern char  *D_00275670;                 /* renderer context block */
extern char   D_0028F700[];               /* base of the display-list arena */
extern Vec4   D_70003AC0[4];              /* camera P*V */
extern int    D_70003600[4];              /* projected corner 0, then the colour */

extern char  *func_001CD370(int which);
extern int    func_001CCF70(const Vec4 *p);   /* project + clip test; GS Z or 0xFFFFFF */
extern int    func_001CF470(ClipVtx *in, char *mtx);  /* frustum clip -> D_008117C0 */
extern char  *func_001CB5F0(int page, int sortkey, int qwc);
extern void   func_001CB950(int page, int sortkey, u64 tex0);
extern void   func_001CB900(int page, int sortkey, int tag);

#define PAGE(layer) ((int)(D_0028F700 + ((layer) << 15) + 0x4D3EC0))

/* Load the fog ramp (VU0 register 23) and the camera matrix (28..31). */
static inline void vu0_load_camera(void)
{
    char *fog = D_00275670 + 0xA0;
    Vec4 *cam;

    asm("lqc2 vf23, 0x0(%0)" : : "r"(fog));
    cam = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(cam));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(cam));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(cam));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(cam));
}

void func_001CDDC0(int layer, int mode, Vec4 *corners, u64 tex0, u32 rgba)
{
    int i;
    int sortkey;
    int n;
    int j;
    unsigned int bl, g, r;
    int f;
    Vec4 raw0;
    char *blk;
    GsFanVtx *out;

    /* 1. depth sort key = the largest GS Z over the four corners */
    sortkey = func_001CCF70(&corners[0]);
    for (i = 1; i < 4; i++) {
        int z = func_001CCF70(&corners[i]);
        sortkey = (sortkey < z) ? z : sortkey;
    }

    /* 2. project corner 0 for its fog value: camera matrix, divide x, y, z by
       w, fog ramp on w, convert to 12.4 */
    vu0_load_camera();
    {
        int *o = D_70003600;
        Vec4 *rp = &raw0;

        asm("lqc2 vf1, 0x0(%0)" : : "r"(corners));
        asm("vmulax.xyzw ACC, vf28, vf1x");
        asm("vmadday.xyzw ACC, vf29, vf1y");
        asm("vmaddaz.xyzw ACC, vf30, vf1z");
        asm("vmaddw.xyzw vf2, vf31, vf0w");
        asm("vdiv Q, vf0w, vf2w");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(rp) : "memory");
        asm("vwaitq");
        asm("vmulq.xyz vf2, vf2, Q");
        asm("vmulaz.w ACC, vf0, vf23z");
        asm("vmaddw.w vf2, vf23, vf2w");
        asm("vminix.w vf2, vf2, vf23x");
        asm("vmaxx.w vf2, vf2, vf0x");
        asm("vftoi4.xyzw vf2, vf2");
        asm("sqc2 vf2, 0x0(%0)" : : "r"(o) : "memory");
    }

    /* 3. fog-modulate the colour, then park it in the scratchpad quadword */
    if (mode != 0) {
        f = D_70003600[3] >> 4;
        if (f > 0xFF) f = 0xFF;
        if (f < 0)    f = 0;
        switch (mode) {
        case 2:
        case 3:
        case 4:
            r = rgba & 0xFF;
            g = (rgba >> 8) & 0xFF;
            bl = (rgba >> 16) & 0xFF;
            r = (r * f) >> 8;
            g = (g * f) >> 8;
            bl = (bl * f) >> 8;
            rgba &= 0xFF000000;
            D_70003600[3] = 0xFF0;          /* dead: the alpha lane below overwrites it */
            rgba |= r | ((g << 8) | (bl << 16));
            break;
        case 1: {
            unsigned int a = (((rgba >> 24) & 0xFF) * f) >> 8;
            rgba &= 0xFFFFFF;
            rgba |= a << 24;
            break;
        }
        }
    }
    D_70003600[0] = rgba & 0xFF;
    D_70003600[1] = (rgba >> 8) & 0xFF;
    D_70003600[2] = (rgba >> 16) & 0xFF;
    D_70003600[3] = (rgba >> 24) & 0xFF;

    /* 4. two triangles, corners (0, 1, 2) and (1, 2, 3) */
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            int ci = i + j;
            *(u128 *)&D_008112C0[j].pos = *(u128 *)&corners[ci];
            switch (ci) {
            case 0: D_008112C0[j].u = 0.0f; D_008112C0[j].v = 0.0f; break;
            case 1: D_008112C0[j].u = 0.0f; D_008112C0[j].v = 1.0f; break;
            case 2: D_008112C0[j].u = 1.0f; D_008112C0[j].v = 0.0f; break;
            case 3: D_008112C0[j].u = 1.0f; D_008112C0[j].v = 1.0f; break;
            }
        }

        n = func_001CF470(D_008112C0, func_001CD370(0));
        if (n == 0) {
            continue;
        }

        {
            int nq = 3 * n + 2;

            blk = func_001CB5F0(PAGE(layer), sortkey, nq);
            /* VIF: three NOPs then DIRECT(GIFtag + 3 n data quadwords) */
            *(u128 *)blk = 0;
            *(int *)(blk + 0x0C) = 0x50000000 | (nq - 1);
        }
        /* GIFtag: NLOOP = n, EOP, PRE, PRIM 0x5D, PACKED, NREG 3; REGS ST, RGBAQ, XYZF2 */
        *(u64 *)(blk + 0x10) = (long long)n | (0x8000 | (0x302EC000ULL << 32));
        *(u64 *)(blk + 0x18) = 0x412;

        out = (GsFanVtx *)(blk + 0x20);
        vu0_load_camera();
        {
            Vec4 raw;
            Vec4 *rp = &raw;
            union { float f; int i; } one;
            ClipVtx *cv = D_008117C0;
            u128 *col = (u128 *)D_70003600;

            for (j = 0; j < n; j++) {
                int *o = out->xyzf;
                float q;

                /* project: divide x, y by w, bias w by 1.0 and divide z by
                   the biased w, fog ramp on w, convert to 12.4 */
                asm("lqc2 vf1, 0x0(%0)" : : "r"(cv));
                asm("vmulax.xyzw ACC, vf28, vf1x");
                asm("vmadday.xyzw ACC, vf29, vf1y");
                asm("vmaddaz.xyzw ACC, vf30, vf1z");
                asm("vmaddw.xyzw vf2, vf31, vf0w");
                asm("vdiv Q, vf0w, vf2w");
                asm("sqc2 vf2, 0x0(%0)" : : "r"(rp) : "memory");
                asm("vwaitq");
                asm("vmulq.xy vf2, vf2, Q");
                one.f = 1.0f;
                asm("qmtc2.ni %0, vf3" : : "r"(one.i));
                asm("vsubx.w vf2, vf2, vf3x");
                asm("vdiv Q, vf0w, vf2w");
                asm("vwaitq");
                asm("vmulq.z vf2, vf2, Q");
                asm("vmulaz.w ACC, vf0, vf23z");
                asm("vmaddw.w vf2, vf23, vf2w");
                asm("vminix.w vf2, vf2, vf23x");
                asm("vmaxx.w vf2, vf2, vf0x");
                asm("vftoi4.xyzw vf2, vf2");
                asm("sqc2 vf2, 0x0(%0)" : : "r"(o) : "memory");

                q = 1.0f / raw.w;
                out->rgbaq = *col;
                out->s = cv->u * q;
                out->t = cv->v * q;
                out->q = q;
                out->pad = 0x3F800000;
                cv++;
                out++;
            }
        }
    }

    /* 5. layer state, pushed after the geometry */
    func_001CB950(PAGE(layer), sortkey, tex0);
    func_001CB900(PAGE(layer), sortkey, mode);
}
