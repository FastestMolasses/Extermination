// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x7000319C 0x700031AC
extern float D_7000319C;
extern float D_700031AC;
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Column query against one round collision record (record kind 0x4000 in the
// func_0019BC40 dispatch; sibling of the axis-face query func_001A5760). The
// record (after its 4-byte header) holds a centre (x, y, z), a radius and a
// half-height; when header bit 0x8000 is set the shape is a sphere and the
// radius is also used as the vertical half-extent. If the query point p
// (x at +0, z at +4) lies within the radius horizontally (squared-distance test,
// boundary inclusive), the vertical span is written to out as
// (y + h, y - h, 1.0, -1.0), the scratchpad epsilons 0x7000319C / 0x700031AC
// get +1e-8 / -1e-8, and 1 is returned; otherwise 0.

typedef struct RoundShape {
    float x;            /* rec + 0x04 */
    float y;            /* rec + 0x08 */
    float z;            /* rec + 0x0C */
    float radius;       /* rec + 0x10 */
    float halfHeight;   /* rec + 0x14 */
} RoundShape;

int func_001A56A0(float *p, float *out, unsigned char *rec) {
    RoundShape *c = (RoundShape *)(rec + 4);
    float h;
    float dx;
    float dz;

    if (*(short *)rec & 0x8000) {
        h = c->radius;
    } else {
        h = c->halfHeight;
    }
    dx = p[0] - c->x;
    dz = p[1] - c->z;
    if (c->radius * c->radius < dx * dx + dz * dz) {
        return 0;
    }
    out[0] = c->y + h;
    out[1] = c->y - h;
    out[2] = 1.0f;
    out[3] = -1.0f;
    D_7000319C = 1e-8f;
    D_700031AC = -1e-8f;
    return 1;
}
