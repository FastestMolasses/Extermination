// NEARMISS func_001E00F0  (vram 0x001E00F0, 0x14C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was an asm-word body, matching
// wave 2026-10-28). s and t are built as in-place compound chains on locals.
// Remaining differences: register colouring of the float-constant setup. The original
// materialises 16.0 first and keeps 16 / 2048 / 512 / 0.5 in t1/t0/a3/a2 (never a0/a1),
// computes s in f6 and t in the jitter register; ours colours from a0 and swaps the
// f1/f3/f6 roles. Tried: single expressions, an inline helper, pre-scaled jitter locals,
// split first steps, mwcc 2.3-991202 / 2.4. Same allocator pattern as func_001DAC90.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Writes one jittered GS vertex record (twin of func_001DAC90, used by a
// haze/sparkle emitter). Two func_00122BB8 random numbers scaled by 2^-31 give
// ju, jv in [0, 1); colour (0x80, 0x80, 0x80), fog word = arg; texture
// s = ((x + 16 * (20 ju - 10)) / 16 - 2048) / 512 + 0.5 and t likewise with y and
// 224 (a +-10 pixel random offset); q = 1.0; the 12.4 screen x, y, z through
// float_to_int; +0x2C = 0xFF.
typedef struct {
    int r, g, b;    /* 0x00 */
    int fog;        /* 0x0C */
    float s;        /* 0x10 */
    float t;        /* 0x14 */
    float q;        /* 0x18 */
    int pad1C;
    int x;          /* 0x20 */
    int y;          /* 0x24 */
    int z;          /* 0x28 */
    int a;          /* 0x2C */
} Vtx;

extern int func_00122BB8(void);
extern int float_to_int(float t);

void func_001E00F0(Vtx *dst, float *screen, int fog) {
    float ju, jv;
    float s, t;

    ju = 4.656613e-10f * (float)func_00122BB8();
    jv = 4.656613e-10f * (float)func_00122BB8();
    dst->r = 0x80;
    dst->g = 0x80;
    dst->b = 0x80;
    dst->fog = fog;
    s = screen[0];
    s += 16.0f * (-10.0f + 20.0f * ju);
    s /= 16.0f;
    s -= 2048.0f;
    s /= 512.0f;
    s += 0.5f;
    dst->s = s;
    t = screen[1];
    t += 16.0f * (-10.0f + 20.0f * jv);
    t /= 16.0f;
    t -= 2048.0f;
    t /= 224.0f;
    t += 0.5f;
    dst->t = t;
    dst->q = 1.0f;
    dst->x = float_to_int(screen[0]);
    dst->y = float_to_int(screen[1]);
    dst->z = float_to_int(screen[2]);
    dst->a = 0xFF;
}
