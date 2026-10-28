// NEARMISS func_001DAE10  (vram 0x001DAE10, 0x18C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 85.68% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-27 (was 76.96%): the x and y range tests are `a < 0 || !(a <= 65520)`
// pairs (the byte-matched twin func_001E0240's spelling), and the transformed point is a
// float[4] like the twin's buffer.
// Remaining differences: the original inlines the VU0 clip test (a matrix-transform chain
// on vf24..vf27, a clip against w, five VU0 no-ops, then a read of the clip flags) where
// this C calls an extern; with no call there, the original also addresses screen[1]
// directly instead of through a temporary pointer. An inline-asm version of that block
// (not committable without a user decision, as for func_001DB480) scores 100% at the
// original 0x18C size.
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

extern void func_001026A0(float *dst, const Vec4 *m, const Vec4 *v);
extern void func_001DAC90(u8 *dst, float *screen, void *uv, float *shade);

/* Clip test of p against the caller-loaded VU0 matrix rows 24..27; returns the
   VU0 clip flags. VU0 macro code in the original, inlined there. */
extern int vu0_clip_flags_inherited(const Vec4 *p);

void func_001DAE10(u8 *dst, const Vec4 *world_pos, void *uv, float *shade)
{
    float screen[4];                    /* x, y, z, w */
    float s;
    int   reject = 0;

    func_001026A0(screen, D_70003AC0, world_pos);

    if ((vu0_clip_flags_inherited(world_pos) & 0x3F) != 0) {
        reject = 1;
    }

    s = 16.0f / screen[3];               /* 16 = the GS 12.4 scale */
    if (s < 0.0f) {
        reject = 1;                     /* behind the camera */
    }

    screen[0] *= s;
    screen[1] *= s;
    screen[2] *= s;

    if (screen[0] < 0.0f || !(screen[0] <= 65520.0f)) {
        reject = 1;                     /* the y test is skipped on this path */
    } else if (screen[1] < 0.0f || !(screen[1] <= 65520.0f)) {
        reject = 1;
    }

    func_001DAC90(dst, screen, uv, shade);

    if (reject) {
        *(int *)(dst + 0x2C) = 0x8000;
    }
}
