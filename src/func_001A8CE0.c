// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Box-overlap test called from the double-loop pass func_001A8DA0 for an outer
// entry e and an inner entry f. f's extents record (pointer at +0x30) holds
// half-sizes (x, y, z). When |e.x - f.x| <= ext.x, |e.z - f.z| <= ext.z and
// |e.y - f.y| <= 0.5 + ext.y (positions at +0xB0/+0xB4/+0xB8, func_0011DF78 is
// fabsf), it sets e's byte at +0xA to 1 and zeroes the inner-loop countdown at
// scratchpad 0x70003B86, which ends the caller's inner walk.

typedef struct Extents {
    float x;                        /* +0x0 */
    float y;                        /* +0x4 */
    float z;                        /* +0x8 */
} Extents;

typedef struct Obj {
    unsigned char pad0[0xA];
    unsigned char hit;              /* +0x0A */
    unsigned char padB[0x30 - 0xB];
    Extents *ext;                   /* +0x30 */
    unsigned char pad34[0xB0 - 0x34];
    float x;                        /* +0xB0 */
    float y;                        /* +0xB4 */
    float z;                        /* +0xB8 */
} Obj;

extern float func_0011DF78(float x);    /* fabsf */

void func_001A8CE0(Obj *e, Obj *f) {
    if (func_0011DF78(e->x - f->x) <= f->ext->x &&
        func_0011DF78(e->z - f->z) <= f->ext->z &&
        func_0011DF78(e->y - f->y) <= 0.5f + f->ext->y) {
        e->hit = 1;
        *(short *)0x70003B86 = 0;
    }
}
