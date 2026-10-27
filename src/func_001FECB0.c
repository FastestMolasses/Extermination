// COMPILER: mwcc233
// CFLAGS: -O4,p

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 84.70% NEARMISS).
// Fixes read from the original instructions:
// - The goto/label-inside-if shape was wrong: the original is a plain switch
//   whose every case ends in `break`, with the D_00275C58 = 1 flag store
//   written in each arm that sets it (state 0 after a nonzero kick, state 1
//   with a nonzero countdown, and the default case). mwcc then keeps the
//   compare constant 1 live for the flag stores and copies them into the
//   branch delay slots, which was the "constant-rematerialization" residual.
//
// State-machine step on the global phase var D_00275C5C (0 = init,
// 1 = running/poll). Returns -1 by default.
// State 0: arms the phase (D_00275C5C = 1) and kicks func_00114988(); if that
// returns nonzero, flags D_00275C58 = 1; returns -1 either way.
// State 1: if the countdown D_00275C64 is nonzero, returns D_00275C64 - 1 and
// flags D_00275C58 = 1; if zero, resets the phase (D_00275C5C = 0) and
// returns 0 without flagging. Any other phase just flags D_00275C58 = 1 and
// returns -1.

extern int func_00114988(void);
extern int D_00275C58;
extern int D_00275C5C;
extern int D_00275C64;

int func_001FECB0(void) {
    int ret = -1;

    switch (D_00275C5C) {
    case 0:
        D_00275C5C = 1;
        if (func_00114988() != 0) D_00275C58 = 1;
        break;
    case 1:
        if (D_00275C64 != 0) {
            ret = D_00275C64 - 1;
            D_00275C58 = 1;
            break;
        }
        D_00275C5C = 0;
        ret = 0;
        break;
    default:
        D_00275C58 = 1;
        break;
    }
    return ret;
}
