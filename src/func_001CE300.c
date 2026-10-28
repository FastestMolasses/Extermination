// NEARMISS func_001CE300  (vram 0x001CE300, 0x358 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.32% via mwcc 2.3.3 (mwcps2-2.3.3-000906); matching wave 2026-10-28 (inline VU0 asm,
// user-approved 2026-10-28 for the VU0 class, incl. the float-to-GPR move before a VU0
// transfer). Was 0.00%, 94.43% before the float-move round.
// Fix: 1.0f reaches VU0 from the FPU register that also divides 1 / w (a float-to-GPR
// move into t0; mwcc adds the hazard nop).
// Remaining differences: colouring in the triangle-staging loop (the original reuses
// s4 for the staging pointer and the s/t field pointers, ours keeps those in t7) and
// in the vertex loop (the original keeps the vertex pointer in v0, the source in a2).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// SPAD: 0x70003600 0x70003604 0x70003608 0x7000360C
typedef unsigned int        u32;
typedef unsigned long long  u64;
typedef unsigned int u128 __attribute__((mode(TI)));

typedef struct { float x, y, z, w; } Vec4;

typedef struct {
    /* 0x00 */ Vec4  pos;
    /* 0x10 */ char  pad10[0x30 - 0x10];
    /* 0x30 */ float s;
    /* 0x34 */ float t;
    /* 0x38 */ char  pad38[0x50 - 0x38];
} ClipVtx;

typedef struct {
    /* 0x00 */ float s, t;
    /* 0x08 */ float q;
    /* 0x0C */ float pad;
    /* 0x10 */ u32   rgbaq[4];
    /* 0x20 */ int   xyzf[4];
} GsVtx;

extern int D_70003600[4];           /* the colour unpacked to R, G, B, A ints */
extern Vec4 D_70003AC0[4];

extern char *D_00275670;
extern char  D_007635C0[];
extern ClipVtx D_008112C0[3];
extern ClipVtx D_008117C0[];

extern char *func_001CD370(int which);
extern int   func_001CF470(ClipVtx *tri, const void *objm);
extern char *func_001CB5F0(char *page, int sortkey, int qwc);
extern char *func_001CB950(char *page, int sortkey, u64 tex0);
extern void  func_001CB900(char *page, int sortkey, int tag);

/* Draws a textured quad (corners quad[0..3], UVs (0,0) (0,1) (1,0) (1,1))
   as two triangles {0,1,2} and {1,2,3}: each is staged in the clipper input
   with its corner UVs, clipped by func_001CF470 against the clip matrix
   func_001CD370(0), and the resulting fan is emitted as a PACKED GIF
   triangle fan (ST, RGBAQ, XYZF2). Per vertex, VU0 projects with the camera
   rows (28..31): x, y scaled by 1 / w, then w reduced by 1 and z scaled by
   1 / (w - 1), fog from the reduced w through the fog ramp (VU0 23), all to
   12.4; ST = (s, t) / w and Q = 1 / w on the FPU from the stacked clip w.
   The colour comes from the four bytes of rgba unpacked to the scratchpad.
   Finally the page gets tex0 (func_001CB950) and is closed with tag. */
void func_001CE300(int tag, const Vec4 *quad, u64 tex0, u32 rgba)
{
    Vec4 clip;
    int  pass, j, k;
    int  corner;

    D_70003600[0] = rgba & 0xFF;
    D_70003600[1] = (rgba >> 8) & 0xFF;
    D_70003600[2] = (rgba >> 16) & 0xFF;
    D_70003600[3] = (rgba >> 24) & 0xFF;

    for (pass = 0; pass < 2; pass++) {
        char  *p;
        GsVtx *v;
        const ClipVtx *src;
        const Vec4 *cam;
        float  one;
        Vec4  *c;
        int    count, qwc;

        for (j = 0; j < 3; j++) {
            corner = pass + j;
            *(u128 *)&D_008112C0[j].pos = *(u128 *)&quad[corner];
            switch (corner) {
            case 0:
                D_008112C0[j].s = 0.0f;
                D_008112C0[j].t = 0.0f;
                break;
            case 1:
                D_008112C0[j].s = 0.0f;
                D_008112C0[j].t = 1.0f;
                break;
            case 2:
                D_008112C0[j].s = 1.0f;
                D_008112C0[j].t = 0.0f;
                break;
            case 3:
                D_008112C0[j].s = 1.0f;
                D_008112C0[j].t = 1.0f;
                break;
            }
        }

        count = func_001CF470(D_008112C0, func_001CD370(0));
        if (count == 0) {
            continue;
        }

        qwc = count * 3 + 2;
        p = func_001CB5F0(D_007635C0, 0, qwc);
        *(u128 *)p = 0;
        *(u32 *)(p + 0xC) = (qwc - 1) | 0x50000000;
        *(u64 *)(p + 0x10) = (long long)count | ((long long)0x8000 | ((long long)0x303EC000 << 32));
        *(u64 *)(p + 0x18) = 0x412;
        v = (GsVtx *)(p + 0x20);

        asm("lqc2 vf23, 0x0(%0)" : : "r"(D_00275670 + 0xA0));
        cam = D_70003AC0;
        asm("lqc2 vf28, 0x0(%0)" : : "r"(cam));
        asm("lqc2 vf29, 0x10(%0)" : : "r"(cam));
        asm("lqc2 vf30, 0x20(%0)" : : "r"(cam));
        asm("lqc2 vf31, 0x30(%0)" : : "r"(cam));

        one = 1.0f;
        src = D_008117C0;
        c = &clip;
        for (k = 0; k < count; k++) {
            int  *xyzf = v->xyzf;
            float invw;

            asm("lqc2 vf1, 0x0(%0)" : : "r"(src));
            asm("vmulax.xyzw ACC, vf28, vf1x");
            asm("vmadday.xyzw ACC, vf29, vf1y");
            asm("vmaddaz.xyzw ACC, vf30, vf1z");
            asm("vmaddw.xyzw vf2, vf31, vf0w");
            asm("vdiv Q, vf0w, vf2w");
            asm("sqc2 vf2, 0x0(%0)" : : "r"(c) : "memory");
            asm("vwaitq");
            asm("vmulq.xy vf2, vf2, Q");
            asm("mfc1 $8, %0" : : "f"(one));
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
            asm("sqc2 vf2, 0x0(%0)" : : "r"(xyzf) : "memory");

            invw = one / clip.w;
            *(u128 *)v->rgbaq = *(u128 *)D_70003600;
            v->s = src->s * invw;
            v->t = src->t * invw;
            v->q = invw;
            v->pad = 1.0f;
            src++;
            v++;
        }
    }

    func_001CB950(D_007635C0, 0, tex0);
    func_001CB900(D_007635C0, 0, tag);
}
