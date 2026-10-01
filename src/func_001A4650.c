// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003190 0x70003194 0x70003198 0x700031A4
extern float D_70003190;
extern float D_70003194;
extern float D_70003198;
extern float D_700031A4;
// Vertical segment against one axis-face collision record (ACTOR_COLLISION.md:
// record kind 0x2000, faces 3 / 4, the segment companion of func_001A5760).
// The segment runs from y0 (scratchpad 0x70003194) to y1 (0x700031A4) at
// x = 0x70003190, z = 0x70003198. Face 3 (an upward-facing plane) is only hit
// moving down (y0 >= y1), face 4 only moving up. When x and z lie inside the
// face's spans (inclusive) and the face's y lies strictly between the segment
// ends, the hit is published to the scratchpad: point (x, face y, z) at
// 0x700031B0..B8, 0x700030D4 / 0x700030DC cleared, the face angle halfword
// 0x700030CA (0x4000 for face 3, -0x8000 for face 4) and the normal sign
// 0x700030D8 (1.0 / -1.0); returns 1. Otherwise 0.
typedef struct AxisFace {
    float x;    /* rec + 0x04 */
    float y;    /* rec + 0x08 */
    float z;    /* rec + 0x0C */
    float dx;   /* rec + 0x10 */
    float pad;
    float dz;   /* rec + 0x18 */
} AxisFace;

int func_001A4650(unsigned char *rec) {
    AxisFace *f = (AxisFace *)(rec + 4);
    unsigned char face = rec[2];
    float y0;
    float lo;
    float hi;
    float x0;
    float x1;
    float z0;
    float z1;
    float px;

    if ((unsigned int)(face - 3) > 1) {
        return 0;
    }
    y0 = D_70003194;
    hi = D_700031A4;
    if (y0 < hi) {
        if (face == 3) {
            return 0;
        } else {
            lo = y0;
        }
    } else {
        if (face == 4) {
            return 0;
        } else {
            lo = hi;
            hi = y0;
        }
    }
    if (f->dx < 0.0f) {
        x1 = f->x;
        x0 = f->x + f->dx;
    } else {
        x0 = f->x;
        x1 = f->x + f->dx;
    }
    if (f->dz < 0.0f) {
        z1 = f->z;
        z0 = f->z + f->dz;
    } else {
        z0 = f->z;
        z1 = f->z + f->dz;
    }
    px = D_70003190;
    if (px < x0 || !(px <= x1) || D_70003198 < z0 || !(D_70003198 <= z1)) {
        return 0;
    } else if (lo < f->y && !(hi <= f->y)) {
            *(float *)0x700031B0 = px;
            *(float *)0x700031B4 = f->y;
            *(int *)0x700030DC = 0;
            *(int *)0x700030D4 = 0;
            *(float *)0x700031B8 = D_70003198;
            if (face == 3) {
                *(short *)0x700030CA = 0x4000;
                *(float *)0x700030D8 = 1.0f;
            } else {
                *(short *)0x700030CA = -0x8000;
                *(float *)0x700030D8 = -1.0f;
            }
        return 1;
    } else {
        return 0;
    }
}
