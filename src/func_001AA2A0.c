// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Reach test for the nearest-object scan func_001AA4E0: can self (position at
// +0xA0/+0xA4/+0xA8) reach object p (position at +0xB0/+0xB4/+0xB8) within the
// horizontal range r (from func_001AA410)? The horizontal distance
// sqrtf(dx^2 + dz^2) is always stored to scratchpad 0x70003A20 (the scan's
// "best distance" metric) before the test; if it exceeds r, 0. Otherwise the
// height difference dy = p.y - self.y is checked against a per-type window:
// p types 8 and 4 accept -8 <= dy <= 30, every other type -13 <= dy <= 10.
// Returns 1 inside the window, else 0.

typedef struct Self {
    unsigned char pad0[0xA0];
    float x;                        /* +0xA0 */
    float y;                        /* +0xA4 */
    float z;                        /* +0xA8 */
} Self;

typedef struct Obj {
    unsigned char pad0[3];
    unsigned char type;             /* +0x03 */
    unsigned char pad4[0xB0 - 4];
    float x;                        /* +0xB0 */
    float y;                        /* +0xB4 */
    float z;                        /* +0xB8 */
} Obj;

extern float func_0011E748(float x);    /* sqrtf */

int func_001AA2A0(Self *self, Obj *p, float range) {
    float dx = p->x - self->x;
    float dz = p->z - self->z;
    float dist;
    float dy;

    dist = func_0011E748(dx * dx + dz * dz);
    *(float *)0x70003A20 = dist;
    if (!(dist <= range)) {
        return 0;
    }
    dy = p->y - self->y;
    if (dy < 0.0f) {
        if (p->type == 8 || p->type == 4) {
            if (dy < -8.0f) {
                return 0;
            }
        } else {
            if (dy < -13.0f) {
                return 0;
            }
        }
    } else {
        if (p->type == 8 || p->type == 4) {
            if (dy > 30.0f) {
                return 0;
            }
        } else {
            if (dy > 10.0f) {
                return 0;
            }
        }
    }
    return 1;
}
