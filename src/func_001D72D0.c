// NEARMISS func_001D72D0  (vram 0x001D72D0, 0xC8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 56.14% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
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

extern int   D_00275C08;            /* per-frame screen-point counter */
extern Vec4  D_70003AC0[4];         /* camera K = P*V, EE scratchpad  */
extern int   func_001D7200(int x, int y);

/* The VU0 kernel documented above; writes both the pre-divide clip quad and
   the post-ftoi4 12.4 screen quad. Not expressible in C. */
extern void vu0_project(const Vec4 *matK, const Vec4 *p,
                        Vec4 *clip_out, IVec4 *screen_12_4);

int func_001D72D0(const Vec4 *world_pos)
{
    Vec4  clip;
    IVec4 screen;

    if (D_00275C08 >= 0x10) {
        return -1;                          /* point list already full */
    }

    vu0_project(D_70003AC0, world_pos, &clip, &screen);

    if (clip.w < 0.0f) {
        return -1;                          /* behind the camera */
    }

    return func_001D7200((screen.x >> 4) - 0x700,   /* 1792 = 2048 - 256 */
                         (screen.y >> 4) - 0x790);  /* 1936 = 2048 - 112 */
}
