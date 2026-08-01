// NEARMISS func_001E2800  (vram 0x001E2800, 0x3A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 4.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
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

/* Byte-matched display-list helpers (see their own source files). */
extern char *func_001CB5F0(int list, int z, int nqw);
extern void  func_001CB6B0(int list, int z, int nqw, unsigned long long ref);
extern void  func_001CB900(int list, int z, int kind);

/* The z-bucketed display list this effect draws into. */
extern int D_007635C0;
/* Pre-canned GIFtag + PRIM block for a 2-vertex gouraud line, referenced by
   address (DMA "ref" transfer) rather than copied. */
extern int D_00253720;

/* CodeWarrior runtime float -> int cast helper. */
extern int float_to_int(float f);

/* EE scratchpad staging block. */
#define SPR_PT_A  ((volatile int *)0x70003600)   /* x,y,z (12.4), w (fade)  */
#define SPR_PT_B  ((volatile int *)0x70003610)
#define SPR_COL_A ((volatile int *)0x70003620)   /* R,G,B,A one word each   */
#define SPR_COL_B ((volatile int *)0x70003630)

/* vf23: globally reserved depth-fade constants (.x ceiling, .z bias, .w slope). */
extern float VF23_x, VF23_z, VF23_w;
/* vf28..vf31: the world->screen matrix, live in VU0 across the call. */
extern Vec4 VF28_31[4];

/* vftoi4: convert to signed 12.4 fixed point (truncating, saturating). */
static int vu0_ftoi4(float f) { return float_to_int(f * 16.0f); }

/* The shared projection idiom: transform by the VU0-resident screen matrix,
   perspective-divide x,y by w, subtract a w bias (1.0f here), perspective-divide
   z by the biased w, then map the biased w through the depth-fade ramp and
   convert the whole vector to 12.4 fixed point.
   PS2 floats are not IEEE-754: the divides saturate rather than producing inf. */
static void vu0_project(volatile int *out, const Vec4 *p) {
    Vec4 v;
    float depth;

    v.x = VF28_31[0].x * p->x + VF28_31[1].x * p->y + VF28_31[2].x * p->z + VF28_31[3].x;
    v.y = VF28_31[0].y * p->x + VF28_31[1].y * p->y + VF28_31[2].y * p->z + VF28_31[3].y;
    v.z = VF28_31[0].z * p->x + VF28_31[1].z * p->y + VF28_31[2].z * p->z + VF28_31[3].z;
    v.w = VF28_31[0].w * p->x + VF28_31[1].w * p->y + VF28_31[2].w * p->z + VF28_31[3].w;

    v.x = v.x / v.w;
    v.y = v.y / v.w;
    v.w = v.w - 1.0f;          /* w bias, staged through qmtc2 in the target */
    v.z = v.z / v.w;

    depth = VF23_z + VF23_w * v.w;
    if (depth > VF23_x) depth = VF23_x;
    if (depth < 0.0f)   depth = 0.0f;

    out[0] = vu0_ftoi4(v.x);
    out[1] = vu0_ftoi4(v.y);
    out[2] = vu0_ftoi4(v.z);
    out[3] = vu0_ftoi4(depth);
}

/* Apply the depth fade held in colour-block `col`'s companion depth word.
   The 12.4 depth is shifted down to an integer 0..255 scale factor, clamped,
   applied to the selected channels as (c * f) >> 8, and the depth word is then
   reset to 0xFF0 (== 255.0 in 12.4) so downstream stages see "no fade". */
static void apply_depth_fade(int mode, volatile int *pt, volatile int *col) {
    int f;

    pt[3] = pt[3] >> 4;
    if (pt[3] >= 0x100) pt[3] = 0xFF;
    if (pt[3] < 0)      pt[3] = 0;
    f = pt[3];

    if (mode == 1) {
        col[3] = (col[3] * f) >> 8;              /* alpha only               */
    } else if (mode == 3 || mode == 2) {
        col[0] = (col[0] * f) >> 8;              /* R                        */
        col[1] = (col[1] * f) >> 8;              /* G (target uses `mult1`,  */
        col[2] = (col[2] * f) >> 8;              /*    the 2nd ALU pipeline) */
    }
    pt[3] = 0xFF0;
}

void func_001E2800(int mode, const Vec4 *pa, const Vec4 *ca,
                   const Vec4 *pb, const Vec4 *cb) {
    char *dst;
    int z;

    /* --- project both endpoints into the scratchpad staging block --------- */
    vu0_project(SPR_PT_A, pa);
    vu0_project(SPR_PT_B, pb);

    /* --- stage both colours (128-bit quadword copies) --------------------- */
    *(Vec4 *)SPR_COL_A = *ca;
    *(Vec4 *)SPR_COL_B = *cb;

    /* --- optional per-endpoint depth fade --------------------------------- */
    if (mode != 0) {
        apply_depth_fade(mode, SPR_PT_A, SPR_COL_A);
    }
    if (mode != 0) {
        apply_depth_fade(mode, SPR_PT_B, SPR_COL_B);
    }

    /* --- submit into the z-sorted display list ---------------------------- */
    z = SPR_PT_A[2];                       /* sort key: endpoint A's z        */

    dst = func_001CB5F0((int)&D_007635C0, z, 4);  /* reserve 4 quadwords      */
    *(Vec4 *)(dst + 0x00) = *(Vec4 *)SPR_COL_A;   /* RGBAQ vertex 0          */
    *(Vec4 *)(dst + 0x10) = *(Vec4 *)SPR_PT_A;    /* XYZ2  vertex 0          */
    *(Vec4 *)(dst + 0x20) = *(Vec4 *)SPR_COL_B;   /* RGBAQ vertex 1          */
    *(Vec4 *)(dst + 0x30) = *(Vec4 *)SPR_PT_B;    /* XYZ2  vertex 1          */

    /* 2-quadword DMA "ref" to the canned GIFtag/PRIM block, then the list's
       standard trailer (func_001CB900 resolves a mode-specific state block). */
    func_001CB6B0((int)&D_007635C0, z, 2, (unsigned long long)(unsigned int)&D_00253720);
    func_001CB900((int)&D_007635C0, z, mode);
}
