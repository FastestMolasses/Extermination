// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Carry an actor on a spinning object (called from func_001A9B10 for an outer
// entry e and an inner type-7 entry f whose float at +0x38 is nonzero; the
// original passes f in the second argument register). When e is within
// 67 + its own radius (first float of e's record at +0x30) of f horizontally
// (sqrtf of dx^2 + dz^2) and within 8 vertically (fabsf), e's x/z position is
// rotated about f's position by f's per-frame spin angle (+0x38):
//   x' = f.x + dx*cos(a) + dz*sin(a),  z' = f.z - dx*sin(a) + dz*cos(a)
// (cosf/sinf re-evaluated for each term, as in the original), e's heading at
// +0xC4 is advanced by the same angle and wrapped by func_001B1470, and the
// inner-loop countdown at scratchpad 0x70003B88 is zeroed.

typedef struct Range {
    float radius;                   /* +0x0 */
} Range;

typedef struct Obj {
    unsigned char pad0[0x30];
    Range *range;                   /* +0x30 */
    unsigned char pad34[0x38 - 0x34];
    float spin;                     /* +0x38 */
    unsigned char pad3C[0xB0 - 0x3C];
    float x;                        /* +0xB0 */
    float y;                        /* +0xB4 */
    float z;                        /* +0xB8 */
    unsigned char padBC[0xC4 - 0xBC];
    float heading;                  /* +0xC4 */
} Obj;

extern float func_0011E748(float x);    /* sqrtf */
extern float func_0011DF78(float x);    /* fabsf */
extern float func_0011DE90(float x);    /* cosf */
extern float func_0011E2A8(float x);    /* sinf */
extern float func_001B1470(float angle);

void func_001A99E0(Obj *e, Obj *f) {
    float dx = e->x - f->x;
    float dz = e->z - f->z;

    if (func_0011E748(dx * dx + dz * dz) <= 67.0f + e->range->radius &&
        func_0011DF78(e->y - f->y) <= 8.0f) {
        e->x = f->x + (dx * func_0011DE90(f->spin) + dz * func_0011E2A8(f->spin));
        e->z = f->z + (dx * -func_0011E2A8(f->spin) + dz * func_0011DE90(f->spin));
        e->heading = func_001B1470(e->heading + f->spin);
        *(short *)0x70003B88 = 0;
    }
}
