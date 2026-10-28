// COMPILER: mwcc233
// CFLAGS: -O4,p

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 78.53% NEARMISS).
// Fixes read from the original instructions:
// - The goto/label-inside-if shape was wrong: the original is a plain switch
//   whose every case ends in `break`, with the D_00275C58 = 1 flag store
//   written in each arm that sets it (state 0 after a nonzero kick, state 2
//   with a negative countdown, and the default case), exactly like the
//   byte-matched sibling func_001FECB0. The "constant-rematerialization +
//   branch-sense" residual came from the old shape, not the compiler.
//
// State-machine step on the global phase var D_00275C5C (sibling of
// func_001FECB0; here the dispatch values are 0 = init, 2 = running/poll).
// Returns -1 by default.
// State 0: arms the phase (D_00275C5C = 2) and kicks func_00114360(); if that
// returns nonzero, flags D_00275C58 = 1; returns -1 either way.
// State 2: reads the countdown D_00275C64 -- if negative, just flags
// D_00275C58 = 1 and returns -1; if >= 0, resets the phase (D_00275C5C = 0) and
// returns D_00275C64 (without flagging). Any other phase flags
// D_00275C58 = 1 and returns -1.

extern int func_00114360(void);
extern int D_00275C58;
extern int D_00275C5C;
extern int D_00275C64;

int func_001FE9E0(void) {
    int ret = -1;

    switch (D_00275C5C) {
    case 0:
        D_00275C5C = 2;
        if (func_00114360() != 0) D_00275C58 = 1;
        break;
    case 2:
        if (D_00275C64 < 0) {
            D_00275C58 = 1;
            break;
        }
        D_00275C5C = 0;
        ret = D_00275C64;
        break;
    default:
        D_00275C58 = 1;
        break;
    }
    return ret;
}
