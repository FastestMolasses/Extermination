// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_001DEDB0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 66.38% NEARMISS).
// Fixes read from the original instructions:
// - func_001DEDB0 sits earlier in the same translation unit; compiled as a
//   static same-TU callee (TU_STATIC) the compiler knows it leaves a1 alone,
//   so v stays in a1 across the call with no saved register or larger frame.
// - CFLAGS use the callee's own -sdatathreshold 4 (tu_static's guard rejects the
//   static copy at 0).

// Stores v at +4 of the record func_001DEDB0(which) returns.
static char *func_001DEDB0(int which);  /* same-TU callee, see TU_STATIC */
#include "src/func_001DEDB0.c"

void func_001DEEC0(int which, int v) {
    *(int *)(func_001DEDB0(which) + 4) = v;
}
