// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// "Is this record a targetable actor?" predicate (used by the targeting scans
// func_00199220 / func_00199770). Only records whose kind byte (+2) is 2 once
// the 0xE0 flag bits are masked off can qualify. Then by type byte (+3):
//   type 6                  -> targetable unless the byte at +0x9F is set
//   types 0x0D, 0x0E, 0x0F, 0x13 -> never
//   type 0x12               -> only when the byte at +0xD is 1
//   any other type          -> always
// Returns 1 / 0.

typedef struct Actor {
    unsigned char pad0[2];
    unsigned char kind;             /* +0x02 (top 3 bits are flags) */
    unsigned char type;             /* +0x03 */
    unsigned char pad4[0xD - 4];
    unsigned char subtype;          /* +0x0D */
    unsigned char padE[0x9F - 0xE];
    unsigned char hidden;           /* +0x9F */
} Actor;

int func_00183B80(Actor *e) {
    if ((e->kind & ~0xE0) == 2) {
        switch (e->type) {
        case 6:
            return (e->hidden == 0) ? 1 : 0;
        case 0xF:
        case 0xD:
        case 0xE:
        case 0x13:
            return 0;
        case 0x12:
            return (e->subtype != 1) ? 0 : 1;
        default:
            return 1;
        }
    }
    return 0;
}
