// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Box-overlap test for the class-3 interaction of func_001A97B0 (a, b). Same
// shape as func_001A8CE0 but with the roles swapped and a 1.0 vertical margin:
// a's extents record (pointer at +0x30) holds half-sizes (x, y, z); when
// |b.x - a.x| <= ext.x, |b.z - a.z| <= ext.z and |b.y - a.y| <= 1.0 + ext.y
// (positions at +0xB0/+0xB4/+0xB8, func_0011DF78 is fabsf), b's short at +0x54
// is set to 1 and the countdown at scratchpad 0x70003B86 is zeroed (ending the
// caller's walk).

typedef struct Extents {
    float x;                        /* +0x0 */
    float y;                        /* +0x4 */
    float z;                        /* +0x8 */
} Extents;

typedef struct Obj {
    unsigned char pad0[0x30];
    Extents *ext;                   /* +0x30 */
    unsigned char pad34[0x54 - 0x34];
    short hit;                      /* +0x54 */
    unsigned char pad56[0xB0 - 0x56];
    float x;                        /* +0xB0 */
    float y;                        /* +0xB4 */
    float z;                        /* +0xB8 */
} Obj;

extern float func_0011DF78(float x);    /* fabsf */

void func_001A96F0(Obj *a, Obj *b) {
    if (func_0011DF78(b->x - a->x) <= a->ext->x &&
        func_0011DF78(b->z - a->z) <= a->ext->z &&
        func_0011DF78(b->y - a->y) <= 1.0f + a->ext->y) {
        b->hit = 1;
        *(short *)0x70003B86 = 0;
    }
}
