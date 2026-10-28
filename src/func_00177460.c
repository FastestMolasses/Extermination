// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003680 0x70003684 0x70003688

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 96.19% NEARMISS).
// Fixes read from the original instructions:
//  - The three scratchpad words are relocated externs (idiom-32, opted in with
//    // SPAD:), one extern per address. Both offsets are stored first and the
//    squared distance is formed from the scratchpad words themselves; with the
//    named externs mwcc reloads x, keeps z from its register (the no-op float
//    move) and places the z store ahead of the multiply-add exactly like the
//    original. The leash test reads the stored distance back from 0x70003688.

// Is the actor farther than its leash from its anchor? The x / z offsets of
// the anchor (+0x2E0 / +0x2E8) from the actor position (+0xB0 / +0xB8) go to
// the scratchpad (0x70003680 / 0x70003684) and the planar distance (sqrtf,
// func_0011E748) to 0x70003688. The leash is 5.0, or 8.0 when wide is set.
// The result (1 beyond the leash, else 0) is stored in +0x2F2 and returned.
extern float D_70003680;
extern float D_70003684;
extern float D_70003688;
extern float func_0011E748(float x);

unsigned char func_00177460(unsigned char *actor, int wide) {
    float leash;

    if (wide == 0) {
        leash = 5.0f;
    } else {
        leash = 8.0f;
    }
    D_70003680 = *(float *)(actor + 0x2E0) - *(float *)(actor + 0xB0);
    D_70003684 = *(float *)(actor + 0x2E8) - *(float *)(actor + 0xB8);
    D_70003688 = func_0011E748(D_70003680 * D_70003680 + D_70003684 * D_70003684);
    if (D_70003688 > leash) {
        actor[0x2F2] = 1;
    } else {
        actor[0x2F2] = 0;
    }
    return actor[0x2F2];
}
