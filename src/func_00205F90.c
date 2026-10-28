// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// TU_STATIC: func_00205F80
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a CodeWarrior asm body with a
// readable companion; same-TU callee via // TU_STATIC:). tools/decomp/tu_static.py removes the
// static copy of func_00205F80 after checking it equals the original; the call links to that
// function's own unit.
//
// Ring-buffer slot address. q points at a ring record: q[1] base address,
// q[2] / q[3] the two cursors, q[4] the slot count; func_00205F80(q) is true
// when q[3] == 0 (empty ring), and then this returns 0. Otherwise it returns
// q[1] + ((q[4] + (q[2] - q[3])) % q[4]) * 0x39640 (slot stride 0x39640).
// The original keeps q in a0 across the call (it knows the leaf callee leaves
// a0 alone), so the callee is compiled here as a static of the same TU.
static int func_00205F80(int *q);  /* same-TU callee, see TU_STATIC */
#include "src/func_00205F80.c"

int func_00205F90(int *q) {
    if (func_00205F80(q)) {
        return 0;
    }
    return q[1] + ((q[4] + (q[2] - q[3])) % q[4]) * 0x39640;
}
