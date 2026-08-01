// NEARMISS func_001CAAC0  (vram 0x001CAAC0, 0xDC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 49.07% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
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
extern Vec4  D_70003AC0[4];         /* camera K = P*V, EE scratchpad */
extern char  D_007635C0[];          /* depth-bucket page table       */

extern void func_00102948(void *dst, const void *src);       /* lq/sq copy */
extern void func_001CB760(void *page_table, int key, unsigned long long payload);

/* See src/func_001D72D0.c for the full kernel description. */
extern void vu0_project(const Vec4 *matK, const Vec4 *p,
                        Vec4 *clip_out, IVec4 *screen_12_4);
extern void vu0_load_fog_quad(const void *quad);             /* lqc2 vf23 */

int func_001CAAC0(const Vec4 *world_pos, void *payload)
{
    Vec4  pos;
    Vec4  clip;
    IVec4 screen;
    int   key;

    func_00102948(&pos, world_pos);

    vu0_load_fog_quad(D_00275670 + 0xA0);
    vu0_project(D_70003AC0, &pos, &clip, &screen);

    key = screen.z;                     /* 16 * (clip.z / clip.w) */
    if (key < 0) {
        key = 0xFFB000;                 /* last bucket */
    }
    if (key < 0x1000) {
        key = 0x1000;                   /* first bucket */
    }

    func_001CB760(D_007635C0, key, (unsigned long long)(unsigned int)payload);
    return key;
}
