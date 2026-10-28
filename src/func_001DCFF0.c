// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Draws a fixed scope ring: centre (32768, 32768), angle 0..360, inner radius
// 200, outer 203, colour from the packed rgba word (bytes R, G, B, A converted
// as unsigned to float), copied to all four corners with func_00102948. Selects
// func_001D1F80(ctx, 0, 1) before submitting the ring with func_002082B0. The
// float third argument the callers pass (1.0f) is unused.
typedef struct { float x, y, z, w; } Vec4;

typedef struct {
    Vec4 pos;       /* centre x, y, start angle, sweep */
    Vec4 radius;    /* inner, inner, outer, outer */
    Vec4 color[4];  /* RGBA per corner */
} Ring;

extern void func_00102948(Vec4 *dst, Vec4 *src);
extern void func_001D1F80(int ch, int a, int b);
extern void func_002082B0(int ctx, Ring *ring);

void func_001DCFF0(int ctx, unsigned int rgba, float scale) {
    Ring ring;

    ring.pos.x = 32768.0f;
    ring.pos.y = 32768.0f;
    ring.pos.z = 0.0f;
    ring.pos.w = 360.0f;
    ring.radius.x = 200.0f;
    ring.radius.y = 200.0f;
    ring.radius.z = 203.0f;
    ring.radius.w = 203.0f;
    ring.color[0].x = (float)(rgba & 0xFF);
    ring.color[0].y = (float)((rgba >> 8) & 0xFF);
    ring.color[0].z = (float)((rgba >> 16) & 0xFF);
    ring.color[0].w = (float)((rgba >> 24) & 0xFF);
    func_00102948(&ring.color[1], &ring.color[0]);
    func_00102948(&ring.color[2], &ring.color[0]);
    func_00102948(&ring.color[3], &ring.color[0]);
    func_001D1F80(ctx, 0, 1);
    func_002082B0(ctx, &ring);
}
