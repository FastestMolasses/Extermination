// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Hit-reaction code for a struck actor, keyed on its type byte (+3). Used by
// the weapon-fire hit dispatcher (func_001861C0) and func_00186A60:
//   types 0x06, 0x1C, 0x1E, 0x1F, 0x50              -> 0x000 (no reaction)
//   types 0x0A, 0x0C, 0x0E, 0x18, 0x2A              -> 0x010
//   types 0x1B, 0x2B                                -> 0x200
//   type  0x51                                      -> 0x300
//   types 0x28, 0x33, 0x34, 0x4F                    -> 0x100
//   anything else                                   -> 0x101
// The case labels are in the original's source order (mwcc 2.3.3 emits the
// compare chain in reverse).

typedef struct Actor {
    unsigned char pad0[3];
    unsigned char type;             /* +0x03 */
} Actor;

short func_001839A0(Actor *other) {
    switch (other->type) {
    case 0x06:
    case 0x1C:
    case 0x1F:
    case 0x1E:
    case 0x50:
        return 0;
    case 0x0A:
    case 0x0C:
    case 0x0E:
    case 0x18:
    case 0x2A:
        return 0x10;
    case 0x2B:
    case 0x1B:
        return 0x200;
    case 0x51:
        return 0x300;
    case 0x33:
    case 0x34:
    case 0x4F:
    case 0x28:
        return 0x100;
    default:
        return 0x101;
    }
}
