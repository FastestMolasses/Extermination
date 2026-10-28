// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Picks the landing/turn animation for side `side` (0 or other; callers pass
// e[0x2F1]). If e[0xD] != 1 and func_0017F1C0(e) reports a result, clears the
// flag e[0x315], plays clip 0x84 (side 0) / 0x85 with the caller's blend, and
// fires event 0x105 / 0x106 through func_001FBD50 with 300.0; otherwise sets
// e[0x315] = 1 and plays clip 0xCC / 0xCD.
// func_0017F1C0 is used for its return value (its own src definition is void,
// but it ends in a call to func_0019AD00 whose result stays in the return
// register).
extern int func_0017F1C0(unsigned char *e);
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern int func_001FBD50(unsigned char *e, int id, int flags, float f);

void func_0017DFB0(unsigned char *e, int side, float blend) {
    if (side == 0) {
        if (e[0xD] != 1 && func_0017F1C0(e) != 0) {
            e[0x315] = 0;
            func_001749A0(e, 0x84, 0, blend);
            func_001FBD50(e, 0x105, 0, 300.0f);
        } else {
            e[0x315] = 1;
            func_001749A0(e, 0xCC, 0, blend);
        }
    } else {
        if (e[0xD] != 1 && func_0017F1C0(e) != 0) {
            e[0x315] = 0;
            func_001749A0(e, 0x85, 0, blend);
            func_001FBD50(e, 0x106, 0, 300.0f);
        } else {
            e[0x315] = 1;
            func_001749A0(e, 0xCD, 0, blend);
        }
    }
}
