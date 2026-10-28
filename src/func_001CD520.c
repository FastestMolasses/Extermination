// NEARMISS func_001CD520  (vram 0x001CD520, 0x420 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.96% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4), was 21.91%
// (98.72% before the float-move round).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class, incl. the float-to-GPR
// move before a VU0 transfer)
// Matching wave 2026-10-28 rebuilt it from the original:
//  - the cull, the projection and the half-extent projection are VU0 macro code
//    inline (one instruction per asm statement): clip against camera 0's cull
//    matrix and read the six clip flags; the projection reuses the loaded point,
//    spills the raw transformed vector to the stack and takes the extent's depth
//    from that spill (raw w), as func_001CD180 does for the extent.
//  - -sdatathreshold 4 (the render context pointer is gp-relative); per-quad
//    scratchpad externs; stores before the extent pointers; depth read first.
//  - the fade is a switch (2, 3, 4 colour; 1 alpha); channels extracted first,
//    locals declared bl, g, r before f; alpha masks rgba in place.
//  - FIDELITY: func_001CB6B0 gets a plain 32-bit address (the old C zero-extended it).
//  - the z bias goes to VU0 as the original does: a float-to-GPR move into t0
//    (mwcc adds the hazard nop itself) and the transfer, no stack union.
//  - rgba is declared before the three floats (several callers in src/ pass it
//    there; the ABI registers are the same), which orders its parameter copy
//    before the float copies as in the original.
// Remaining differences: one OR's operand order in the colour repack (the
// original ORs blue-shifted with green-shifted; every spelling that names
// blue first also reschedules the three multiplies).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Queues a camera-facing sprite: returns 0xFFFFFF when the point is outside
// camera 0's view volume; otherwise projects it to 12.4 screen space with a
// fog value (z biased by zbias), projects the half extent (0.5 w, 0.5 h) at
// its depth, optionally fades the colour by the fog value (mode 1 alpha,
// modes 2..4 RGB with the fog word reset to full), and writes a 6-quadword
// sprite packet (giftag, colour, the two corners) into the bucket's z-sorted
// display list with the static GIF state and the mode's trailer. Returns z.

typedef struct { float x, y, z, w; } Vec4;
typedef union { float f; int i; } Word;

extern char *D_00275670;            /* per-frame render context */
extern char  D_00251220[];          /* static GIF state packet */
extern char  D_0028F700[];          /* render-bucket page tables, stride 0x8000 */

extern char *func_001CD370(int camera);
extern char *func_001CB5F0(int page_table, int z, int nquads);
extern void  func_001CB6B0(int page_table, int z, int kind, int addr);
extern void  func_001CB900(int page_table, int z, int mode);

extern int  D_70003600[4];          /* projected centre: x, y, z (12.4), fog */
extern Word D_70003610[4];          /* half extent: floats in, 12.4 out */
extern Vec4 D_70003A40[4];          /* screen-scale matrix */
extern Vec4 D_70003AC0[4];          /* camera P*V */

#define PAGE_TABLE(b) ((int)(D_0028F700 + ((b) << 15) + 0x4D3EC0))

int func_001CD520(int bucket, int mode, Vec4 *world, unsigned long long giftag,
                  unsigned int rgba, float w, float h, float zbias)
{
    char *m;
    char *fog;
    Vec4 *cam;
    int *out;
    Vec4 *raw_p;
    Word *q;
    Vec4 *sm;
    Vec4 raw;
    int clip;
    unsigned int bl, g, r;
    int f;
    char *prim;
    float depth;

    /* clip the point against the culling matrix of camera 0 */
    m = func_001CD370(0);
    asm("lqc2 vf24, 0x0(%0)" : : "r"(m));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(m));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(m));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(m));
    asm("lqc2 vf1, 0x0(%0)" : : "r"(world));
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
        return 0xFFFFFF;
    }

    /* project: camera matrix, divide x, y by w, bias w by zbias, divide z
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
    asm("vmulax.xyzw ACC, vf28, vf1x");
    asm("vmadday.xyzw ACC, vf29, vf1y");
    asm("vmaddaz.xyzw ACC, vf30, vf1z");
    asm("vmaddw.xyzw vf2, vf31, vf0w");
    asm("vdiv Q, vf0w, vf2w");
    asm("sqc2 vf2, 0x0(%0)" : : "r"(raw_p) : "memory");
    asm("vwaitq");
    asm("vmulq.xy vf2, vf2, Q");
    asm("mfc1 $8, %0" : : "f"(zbias));
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

    /* half extent at the point's depth, through the screen-scale matrix */
    depth = raw.w;
    D_70003610[0].f = 0.5f * w;
    D_70003610[1].f = 0.5f * h;
    D_70003610[2].f = depth;
    q = D_70003610;
    sm = D_70003A40;
    asm("lqc2 vf1, 0x0(%0)" : : "r"(q));
    asm("lqc2 vf16, 0x0(%0)" : : "r"(sm));
    asm("lqc2 vf17, 0x10(%0)" : : "r"(sm));
    asm("lqc2 vf18, 0x20(%0)" : : "r"(sm));
    asm("lqc2 vf19, 0x30(%0)" : : "r"(sm));
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

    /* depth-driven colour modulation */
    if (mode != 0) {
        f = D_70003600[3] >> 4;
        if (f > 0xFF) f = 0xFF;
        if (f < 0)    f = 0;
        switch (mode) {
        case 2:
        case 3:
        case 4: {
            r = rgba & 0xFF;
            g = (rgba >> 8) & 0xFF;
            bl = (rgba >> 16) & 0xFF;
            r = (r * f) >> 8;
            g = (g * f) >> 8;
            bl = (bl * f) >> 8;
            rgba &= 0xFF000000;
            D_70003600[3] = 0xFF0;
            rgba |= r | ((g << 8) | (bl << 16));
            break;
        }
        case 1:
        {
            unsigned int a = (((rgba >> 24) & 0xFF) * f) >> 8;
            rgba &= 0xFFFFFF;
            rgba |= a << 24;
        }
            break;
        }
    }

    prim = func_001CB5F0(PAGE_TABLE(bucket), D_70003600[2], 6);
    *(unsigned long long *)(prim + 0x00) = giftag;
    *(int *)(prim + 0x10) = rgba & 0xFF;
    *(int *)(prim + 0x14) = (rgba >> 8) & 0xFF;
    *(int *)(prim + 0x18) = (rgba >> 16) & 0xFF;
    *(int *)(prim + 0x1C) = (rgba >> 24) & 0xFF;
    *(int *)(prim + 0x20) = 0;
    *(int *)(prim + 0x24) = 0;
    *(float *)(prim + 0x28) = 1.0f;
    *(int *)(prim + 0x30) = D_70003600[0] + D_70003610[0].i;
    *(int *)(prim + 0x34) = D_70003600[1] + D_70003610[1].i;
    *(int *)(prim + 0x38) = D_70003600[2];
    *(int *)(prim + 0x3C) = D_70003600[3];
    *(float *)(prim + 0x40) = 1.0f;
    *(float *)(prim + 0x44) = 1.0f;
    *(float *)(prim + 0x48) = 1.0f;
    *(int *)(prim + 0x50) = D_70003600[0] - D_70003610[0].i;
    *(int *)(prim + 0x54) = D_70003600[1] - D_70003610[1].i;
    *(int *)(prim + 0x58) = D_70003600[2];
    *(int *)(prim + 0x5C) = D_70003600[3];
    func_001CB6B0(PAGE_TABLE(bucket), D_70003600[2], 2, (int)D_00251220);
    func_001CB900(PAGE_TABLE(bucket), D_70003600[2], mode);
    return D_70003600[2];
}
