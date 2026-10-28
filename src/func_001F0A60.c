// NEARMISS func_001F0A60  (vram 0x001F0A60, 0x6AC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.67% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4), was 17.97%
// (98.69% before the float-move round).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class, incl. the float-to-GPR
// move before a VU0 transfer)
// Matching wave 2026-10-28 rebuilt it from the original:
//  - the cull (clip flags of camera 0's cull matrix), the projection (raw
//    vector spilled to the stack, its w kept as the corner depth) and the two
//    corner-offset projections through the screen-scale matrix are VU0 macro
//    code inline, one instruction per asm statement (the old C emulated them
//    on the FPU and clipped against |w|).
//  - -sdatathreshold 4 (gp-relative render context); corner offsets and the
//    centre are per-quad scratchpad externs; the two projected offsets are a
//    stack array below the raw spill.
//  - the fog weighting is a static inline function applied to the inner then
//    the outer colour (a switch: 2, 3, 4 colour; 1 alpha); the packet's
//    channel extraction is on the int colour arguments; the first packet
//    quadword is cleared with one 128-bit store, then its last word set; the
//    outer vertex's fog word gets bit 15 after the vertex is written.
//  - the depth bias goes to VU0 as the original does: a float-to-GPR move into
//    t0 (mwcc adds the hazard nop) and the transfer; no stack union, frame 0xA0.
//  - the two corner offsets are one scratchpad array [2][4] (as separate
//    externs mwcc reordered the second corner's stores ahead of the first's w);
//    the second fog-programmer call takes its bias from a local.
// Remaining differences: the half-offset products load y before x (registers
// match), and one OR's operand order in the colour repack (as func_001CD520).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// The camera-facing pickup glint (port EFFECT_MANAGER.md section 8,
// em_effect_manager_001F0A60; CHAIN_PAGE.md). Nothing is drawn when the
// position is outside camera 0's view volume. Otherwise: fog programmer
// func_0021B9A0(2 / 3, 1.0, 150.0); the position projected to 12.4 with a fog
// value (z biased by depth) at 0x70003600; two corner offsets (size, 0) and
// (0, size) rotated by angle about z, halved and projected at the point's
// depth to 12.4 screen offsets; with a non-zero mode both colours weighted by
// the fog value; 14 quadwords on the chain (VIF DIRECT 13, a GIF tag for two
// 3-vertex Gouraud fogged blended strips: outer colour at centre + offset with
// fog bit 15, inner colour at the centre, outer colour at centre - offset);
// then the mode's trailer and func_0021B9A0(1, 0, 0) resets the fog.

typedef struct { float x, y, z, w; } Vec4;
typedef union { float f; int i; } Word;
typedef int u128 __attribute__((mode(TI)));

extern char *func_001CD370(int camera);
extern void  func_0021B9A0(int mode, float scale, float bias);
extern void  func_001029C0(Vec4 *m);
extern void  func_00102A60(void *dst, void *src, float angle);
extern void  func_001026A0(void *dst, void *m, void *v);
extern char *func_001CB5F0(int chain, int id, int qwc);
extern void  func_001CB900(int chain, int id, int mode);

extern char *D_00275670;            /* per-frame render context */
extern char  D_0028F700[];          /* render-bucket page tables, stride 0x8000 */
extern int   D_70003600[4];         /* projected centre: x, y, z (12.4), fog */
extern Word  D_70003400[2][4];      /* the two corner offsets */
extern Vec4  D_70003440[4];         /* rotation about z */
extern Vec4  D_70003A40[4];         /* screen-scale matrix */
extern Vec4  D_70003AC0[4];         /* camera P*V */

#define CHAIN(n) ((int)(D_0028F700 + ((n) << 15) + 0x4D3EC0))

/* Project the half corner offset (x, y, depth w) through the screen-scale
   matrix (its third row zeroed in x, y), divide by w and convert x, y to 12.4. */
static inline void vu0_project_extent(Word *v)
{
    Word *q = v;
    Vec4 *m = D_70003A40;

    asm("lqc2 vf1, 0x0(%0)" : : "r"(q));
    asm("lqc2 vf16, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf17, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf18, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf19, 0x30(%0)" : : "r"(m));
    asm("vsub.xy vf18, vf18, vf18");
    asm("vmulax.xyzw ACC, vf16, vf1x");
    asm("vmadday.xyzw ACC, vf17, vf1y");
    asm("vmaddaz.xyzw ACC, vf18, vf1z");
    asm("vmaddw.xyzw vf2, vf19, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("vwaitq");
    asm("vmulq.xy vf2, vf2, Q");
    asm("vftoi4.xy vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(q) : "memory");
}

/* Weight a colour by the centre's fog value (mode 1: alpha; 2..4: RGB, and
   the fog word is then pinned to full). */
static inline unsigned int fog_weight(int mode, unsigned int c)
{
    int f;
    unsigned int r, g, bl;

    if (mode != 0) {
        f = D_70003600[3] >> 4;
        if (f > 0xFF) f = 0xFF;
        if (f < 0)    f = 0;
        switch (mode) {
        case 2:
        case 3:
        case 4:
            r = c & 0xFF;
            g = (c >> 8) & 0xFF;
            bl = (c >> 16) & 0xFF;
            r = (r * f) >> 8;
            g = (g * f) >> 8;
            bl = (bl * f) >> 8;
            c &= 0xFF000000;
            D_70003600[3] = 0xFF0;
            c |= r | ((g << 8) | (bl << 16));
            break;
        case 1: {
            unsigned int a = (((c >> 24) & 0xFF) * f) >> 8;
            c &= 0xFFFFFF;
            c |= a << 24;
            break;
        }
        }
    }
    return c;
}

void func_001F0A60(int chain_no, int mode, Vec4 *pos, int inner, int outer,
                   float angle, float size, float depth)
{
    char *m;
    char *fog;
    Vec4 *cam;
    int *out;
    Vec4 *raw_p;
    Word off[2][4];
    Vec4 raw;
    float keep_w;
    int clip;
    int k;
    int *vtx;

    /* clip the point against camera 0's cull matrix */
    m = func_001CD370(0);
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));
    asm("lqc2 vf1, 0x0(%0)" : : "r"(pos));
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
    asm("cfc2.ni %0, vi18" : "=r"(clip));
    if ((clip & 0x3F) != 0) {
        return;
    }

    func_0021B9A0(2, 1.0f, 150.0f);
    {
        float bias = 150.0f;
        func_0021B9A0(3, 1.0f, bias);
    }

    /* project: camera matrix, divide x, y by w, bias w by depth, divide z
       by the biased w, fog ramp on w, convert to 12.4 */
    fog = D_00275670 + 0xA0;
    asm("lqc2 vf23, 0x0(%0)" : : "r"(fog));
    cam = D_70003AC0;
    asm("lqc2 vf28, 0x0(%0)" : : "r"(cam));
    asm("lqc2 vf29, 0x10(%0)" : : "r"(cam));
    asm("lqc2 vf30, 0x20(%0)" : : "r"(cam));
    asm("lqc2 vf31, 0x30(%0)" : : "r"(cam));
    out = D_70003600;
    raw_p = &raw;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(pos));
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(raw_p) : "memory");
    asm("vwaitq");
    asm("vmulq.xy vf2, vf2, Q");
    asm("mfc1 $8, %0" : : "f"(depth));
    asm("qmtc2.ni $8, vf3");
    asm("vsubx.w vf2, vf2, vf3x");
    asm("vdiv Q, vf0w, vf2w");
    asm("vwaitq");
    asm("vmulq.z vf2, vf2, Q");
    asm("vmulaz.w ACC, vf0, vf23z");
    asm("vmaddw.w vf2, vf23, vf2w");
    asm("vminix.w vf2, vf2, vf23x");
    asm("vmaxx.w vf2, vf2, vf0x");
    asm("vftoi4.xyzw vf2, vf2");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(out) : "memory");

    /* the two corner offsets (size, 0) and (0, size), rotated by angle about z */
    D_70003400[0][0].f = size;
    D_70003400[0][1].f = 0.0f;
    D_70003400[0][2].f = 0.0f;
    D_70003400[0][3].f = 1.0f;
    D_70003400[1][0].f = 0.0f;
    D_70003400[1][1].f = size;
    D_70003400[1][2].f = 0.0f;
    D_70003400[1][3].f = 1.0f;
    keep_w = raw.w;
    func_001029C0(D_70003440);
    func_00102A60(D_70003440, D_70003440, angle);
    func_001026A0(D_70003400[0], D_70003440, D_70003400[0]);
    func_001026A0(D_70003400[1], D_70003440, D_70003400[1]);

    /* half offsets projected at the point's depth: 12.4 screen offsets */
    off[0][0].f = 0.5f * D_70003400[0][0].f;
    off[0][1].f = 0.5f * D_70003400[0][1].f;
    off[0][2].f = keep_w;
    vu0_project_extent(off[0]);
    off[1][0].f = 0.5f * D_70003400[1][0].f;
    off[1][1].f = 0.5f * D_70003400[1][1].f;
    off[1][2].f = keep_w;
    vu0_project_extent(off[1]);

    inner = fog_weight(mode, inner);
    outer = fog_weight(mode, outer);

    vtx = (int *)func_001CB5F0(CHAIN(chain_no), D_70003600[2], 14);
    /* VIF: three NOPs then DIRECT 13; GIFtag: two strips (NLOOP 2, EOP, PRE),
       PRIM 0x5A (Gouraud fogged blended triangle strip), PACKED, NREG 6,
       REGS RGBAQ / XYZF2 x3 */
    *(u128 *)vtx = 0;
    vtx[3] = 0x5000000D;
    *(unsigned long long *)(vtx + 4) = 0x6035400000008002ULL;
    *(unsigned long long *)(vtx + 6) = 0x414141;
    vtx += 8;
    for (k = 0; k < 2; k++) {
        vtx[0] = outer & 0xFF;
        vtx[1] = (outer >> 8) & 0xFF;
        vtx[2] = (outer >> 16) & 0xFF;
        vtx[3] = (outer >> 24) & 0xFF;
        vtx[8] = inner & 0xFF;
        vtx[9] = (inner >> 8) & 0xFF;
        vtx[10] = (inner >> 16) & 0xFF;
        vtx[11] = (inner >> 24) & 0xFF;
        vtx[16] = outer & 0xFF;
        vtx[17] = (outer >> 8) & 0xFF;
        vtx[18] = (outer >> 16) & 0xFF;
        vtx[19] = (outer >> 24) & 0xFF;
        vtx[4] = D_70003600[0] + off[k][0].i;
        vtx[5] = D_70003600[1] + off[k][1].i;
        vtx[6] = D_70003600[2];
        vtx[7] = D_70003600[3];
        vtx[20] = D_70003600[0] - off[k][0].i;
        vtx[21] = D_70003600[1] - off[k][1].i;
        vtx[22] = D_70003600[2];
        vtx[23] = D_70003600[3];
        vtx[12] = D_70003600[0];
        vtx[13] = D_70003600[1];
        vtx[14] = D_70003600[2];
        vtx[15] = D_70003600[3];
        vtx[7] |= 0x8000;
        vtx += 24;
    }

    func_001CB900(CHAIN(chain_no), D_70003600[2], mode);
    func_0021B9A0(1, 0.0f, 0.0f);
}
