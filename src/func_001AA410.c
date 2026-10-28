// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Horizontal reach range for an object, keyed on its type byte (+3); the
// nearest-object scan func_001AA4E0 passes it to func_001AA2A0:
//   types 1, 2, 4, 5, 7, 0xC -> 20.0
//   type  6                  -> 35.0
//   type  8                  -> 30.0
//   anything else            -> 0.0
// Types 7 and 2 are separate cases in the original (each its own 20.0 return).

typedef struct Obj {
    unsigned char pad0[3];
    unsigned char type;             /* +0x03 */
} Obj;

float func_001AA410(Obj *p) {
    switch (p->type) {
    case 1:
    case 12:
    case 4:
    case 5:
        return 20.0f;
    case 7:
        return 20.0f;
    case 2:
        return 20.0f;
    case 6:
        return 35.0f;
    case 8:
        return 30.0f;
    default:
        return 0.0f;
    }
}
