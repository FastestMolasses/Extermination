// NEARMISS func_001DAE10  (vram 0x001DAE10, 0x18C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 76.96% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned char u8;
typedef struct { float x, y, z, w; } Vec4;

extern Vec4 D_70003AC0[4];      /* camera K = P*V, EE scratchpad */

extern void func_001026A0(Vec4 *dst, const Vec4 *m, const Vec4 *v);
extern void func_001DAC90(u8 *dst, const Vec4 *screen, void *uv, float *shade);

/* vclipw.xyz against the caller-supplied vf24..vf27 + cfc2 $vi18.
   COP2 macro mode, no C spelling. */
extern int vu0_clip_flags_inherited(const Vec4 *p);

void func_001DAE10(u8 *dst, const Vec4 *world_pos, void *uv, float *shade)
{
    Vec4  screen;
    float s;
    int   reject = 0;

    func_001026A0(&screen, D_70003AC0, world_pos);

    if ((vu0_clip_flags_inherited(world_pos) & 0x3F) != 0) {
        reject = 1;
    }

    s = 16.0f / screen.w;               /* 16 = the GS 12.4 scale */
    if (s < 0.0f) {
        reject = 1;                     /* behind the camera */
    }

    screen.x *= s;
    screen.y *= s;
    screen.z *= s;

    if (screen.x < 0.0f) {
        reject = 1;                     /* the y test is skipped on this path */
    } else if (!(screen.x <= 65520.0f)) {
        reject = 1;
    } else if (screen.y < 0.0f) {
        reject = 1;
    } else if (!(screen.y <= 65520.0f)) {
        reject = 1;
    }

    func_001DAC90(dst, &screen, uv, shade);

    if (reject) {
        *(int *)(dst + 0x2C) = 0x8000;
    }
}
