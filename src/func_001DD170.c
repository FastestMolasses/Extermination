// NEARMISS func_001DD170  (vram 0x001DD170, 0x180 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 29.59% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned char      u8;
typedef unsigned int       u32;
typedef struct { float x, y, z, w; } Vec4;
typedef struct { int   x, y, z, w; } IVec4;

extern char *D_00275670;        /* per-frame render context        */
extern Vec4  D_70003AC0[4];     /* camera K = P*V, EE scratchpad   */
extern IVec4 D_70003600;        /* 12.4 screen quad, EE scratchpad */

extern int  func_001C5FB0(int value, int a1, int a2);
extern void func_001CBA50(int a0, int x, int y, int a3, int t0, int handle,
                          void *colour_rec);
extern void func_001DD2F0(int style, IVec4 *screen, u32 colour);
extern void func_001DD600(int style, IVec4 *screen, u32 colour);

/* COP2 primitives; see src/func_001D72D0.c. Note vu0_project_keep_fog does
   NOT reload vf23 — it reuses whatever the previous projector left there. */
extern int  vu0_clip_flags(const Vec4 *matClip, const Vec4 *p);
extern void vu0_project_keep_fog(const Vec4 *matK, const Vec4 *p,
                                 Vec4 *clip_out, IVec4 *screen_12_4);

void func_001DD170(int style, const Vec4 *world_pos, int kind, u32 colour,
                   int value)
{
    Vec4 clip;
    u8   rec[8];

    if ((vu0_clip_flags((Vec4 *)(D_00275670 + 0x2240), world_pos) & 0x3F) != 0) {
        return;
    }

    vu0_project_keep_fog(D_70003AC0, world_pos, &clip, &D_70003600);

    if (kind == 1) {
        *(u32 *)&rec[0] = colour & 0x00FFFFFF;
        rec[4] = (u8)(colour >> 24);
        rec[5] = 0;
        func_001CBA50(1,
                      (D_70003600.x + 0xC0) >> 4,
                      D_70003600.y >> 4,
                      8, 8,
                      func_001C5FB0(value, 4, 0),
                      rec);
        func_001DD600(style, &D_70003600, colour);
    } else if (kind == 0) {
        func_001DD2F0(style, &D_70003600, colour);
    }
}
