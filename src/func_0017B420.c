// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Countdown gate on the global counter D_00810CAC: while it is nonzero it is
// decremented, the companion counter D_00810CAE is reloaded with 100, and the
// function returns 0; once it reaches 0 it returns 1. Callers (func_00171E90)
// pass the entity pointer, which this function ignores.
// The old asm-body note called the trailing "v0 = 1" after the unconditional
// branch a dead 2.3.1 instruction; it is just mwcc 2.3.3's layout of an early
// `return 1` after the if-block.
extern short D_00810CAC;
extern short D_00810CAE;

int func_0017B420(void) {
    if (D_00810CAC != 0) {
        D_00810CAC--;
        D_00810CAE = 100;
        return 0;
    }
    return 1;
}
