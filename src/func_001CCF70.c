// NEARMISS func_001CCF70  (vram 0x001CCF70, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 38.39% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
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
typedef struct { int   x, y, z, w; } IVec4;

extern char *D_00275670;            /* per-frame render context      */
extern int   D_00275C04;            /* cached integer view depth     */
extern Vec4  D_70003AC0[4];         /* camera K = P*V, EE scratchpad */
extern IVec4 D_70003600;            /* 12.4 screen quad, EE scratchpad */

extern Vec4 *func_001CD370(int variant);      /* ctx + 0x2240 + variant*0x40 */
extern int   float_to_int(float v);

/* COP2 primitives; see src/func_001D72D0.c. */
extern void vu0_load_fog_quad(const void *quad);              /* lqc2 vf23 */
extern int  vu0_clip_flags(const Vec4 *matClip, const Vec4 *p); /* vclipw + cfc2 */
extern void vu0_project(const Vec4 *matK, const Vec4 *p,
                        Vec4 *clip_out, IVec4 *screen_12_4);

int func_001CCF70(const Vec4 *world_pos)
{
    Vec4 clip;

    if ((vu0_clip_flags(func_001CD370(0), world_pos) & 0x3F) != 0) {
        return 0xFFFFFF;                    /* outside the clip volume */
    }

    vu0_load_fog_quad(D_00275670 + 0xA0);
    vu0_project(D_70003AC0, world_pos, &clip, &D_70003600);

    D_00275C04 = float_to_int(clip.w);      /* integer view depth */
    return D_70003600.z;                    /* 12.4 GS depth key  */
}
