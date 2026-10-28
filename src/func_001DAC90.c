// NEARMISS func_001DAC90  (vram 0x001DAC90, 0x180 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0) (was an asm-word body, matching
// wave 2026-10-28). Each fog path stores its own result (fog == 0 first).
// Remaining differences: register colouring only. The original keeps the two
// long-lived float-constant temps (0.5 and 3584) in a3/a2 where ours uses a1/a0, and it
// copies screen into its saved register after dst (ours before). Tried: no fog local,
// a Vec4 screen, a scoped screen copy, swapped multiply operands, compound s/t chains,
// mwcc 2.3-991202 / 2.4, -sdatathreshold 4.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Writes one GS vertex record (called by func_001DAE10). The first quadword is the
// RGBA/fog block copied from shade. If shade's fog word (+0xC) is 0, fog is
// computed from uv: float_to_int(uv[2] + (0 + k * uv[0])) + 16 with k = 5.2 when
// uv[0] > 0, else 0.5; otherwise the word is kept. Texture coordinates:
// s = 0.5 + ((x + 16 * uv[0]) / 8192 - 4), t = 0.5 + ((y + 16 * uv[1]) / 3584 -
// 9.142858), q = 1.0; then the 12.4 screen x, y, z through float_to_int and
// +0x2C = 0xFF (func_001DAE10 overwrites it with 0x8000 on reject).
typedef int u128 __attribute__((mode(TI)));

typedef struct {
    u128 color;     /* 0x00 copied from shade; word 3 = fog/depth */
    float s;        /* 0x10 */
    float t;        /* 0x14 */
    float q;        /* 0x18 */
    int pad1C;
    int x;          /* 0x20 */
    int y;          /* 0x24 */
    int z;          /* 0x28 */
    int a;          /* 0x2C */
} Vtx;

extern int float_to_int(float t);

void func_001DAC90(Vtx *dst, float *screen, float *uv, int *shade) {
    int fog;

    dst->color = *(u128 *)shade;
    fog = shade[3];
    if (fog == 0) {
        if (uv[0] > 0.0f) {
            ((int *)dst)[3] = float_to_int(uv[2] + ((float)fog + 5.2f * uv[0])) + 0x10;
        } else {
            ((int *)dst)[3] = float_to_int(uv[2] + ((float)fog + 0.5f * uv[0])) + 0x10;
        }
    } else {
        ((int *)dst)[3] = fog;
    }
    dst->s = 0.5f + ((screen[0] + 16.0f * uv[0]) / 8192.0f - 4.0f);
    dst->t = 0.5f + ((screen[1] + 16.0f * uv[1]) / 3584.0f - 9.142858f);
    dst->q = 1.0f;
    dst->x = float_to_int(screen[0]);
    dst->y = float_to_int(screen[1]);
    dst->z = float_to_int(screen[2]);
    dst->a = 0xFF;
}
