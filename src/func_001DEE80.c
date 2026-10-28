// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_001DEDB0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 78.54% NEARMISS).
// Fixes read from the original instructions:
// - func_001DEDB0 sits earlier in the same translation unit; compiled as a
//   static same-TU callee (TU_STATIC) the compiler knows it leaves a1 alone,
//   so the source pointer stays in a1 across the call with no saved register or larger frame.
// - CFLAGS use the callee's own -sdatathreshold 4 (tu_static's guard rejects the
//   static copy at 0).

// Copies the three words at src into the record func_001DEDB0(which) returns, at
// +0x10, +0x14 and +0x18.
static char *func_001DEDB0(int which);  /* same-TU callee, see TU_STATIC */
#include "src/func_001DEDB0.c"

void func_001DEE80(int which, int *src) {
    char *r = func_001DEDB0(which);
    *(int *)(r + 0x10) = src[0];
    *(int *)(r + 0x14) = src[1];
    *(int *)(r + 0x18) = src[2];
}
