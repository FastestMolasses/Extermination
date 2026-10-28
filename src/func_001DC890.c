// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Draws the aiming-sight (AUTO_SIGHT) zoom ring: fills a ring record on the stack and hands
// it to func_002082B0(ctx, ring). Centre (32768, 32768) (screen centre in GS
// coordinates), angle 0..360, inner radius r = (0.5 + 0.5 * zoom) * 142 and
// outer radius r + 3, colour (85, 195, 205, 128) copied to all four corners
// with func_00102948.
// Fix read from the original: the radius is computed then scaled in place
// (r = 0.5 + 0.5 * zoom; r *= 142) - the single-expression spelling colours
// it into another FPU register.
typedef struct { float x, y, z, w; } Vec4;

typedef struct {
    Vec4 pos;       /* centre x, y, start angle, sweep */
    Vec4 radius;    /* inner, inner, outer, outer */
    Vec4 color[4];  /* RGBA per corner */
} Ring;

extern void func_00102948(Vec4 *dst, Vec4 *src);
extern void func_002082B0(int ctx, Ring *ring);

void func_001DC890(int ctx, float zoom) {
    Ring ring;
    float r;

    r = 0.5f + 0.5f * zoom;
    r *= 142.0f;
    ring.pos.x = 32768.0f;
    ring.pos.y = 32768.0f;
    ring.pos.z = 0.0f;
    ring.pos.w = 360.0f;
    ring.radius.x = r;
    ring.radius.y = r;
    ring.radius.z = 3.0f + r;
    ring.radius.w = 3.0f + r;
    ring.color[0].x = 85.0f;
    ring.color[0].y = 195.0f;
    ring.color[0].z = 205.0f;
    ring.color[0].w = 128.0f;
    func_00102948(&ring.color[1], &ring.color[0]);
    func_00102948(&ring.color[2], &ring.color[0]);
    func_00102948(&ring.color[3], &ring.color[0]);
    func_002082B0(ctx, &ring);
}
