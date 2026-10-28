// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_001EAB50
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was an 80.61% NEARMISS).
// Fixes read from the original instructions:
//  - the order in which mwcc 2.3.3 sets up the float constants (1e-6, 1.0, 3.0)
//    depends on what the compiler did earlier in the same translation unit, not on
//    this function's source alone. Compiling the real preceding function
//    func_001EAB50 first (as a same-TU static, see TU_STATIC; tu_static.py removes
//    the copy after checking it equals the original) reproduces the original order.
//
// Single-call sibling of func_001EAF80: func_001CFB50(&D_0081F8F0, 0, arg0,
// D_00275C34->+0x54, D_00275C34->+0x5C, 1.0f, 1e-6f, 3.0f), then
// func_001CFBE0(arg1, 0, D_00255620, &D_0081F8F0, 0).
static void func_001EAB50(int arg0, int arg1);  /* same-TU predecessor, see TU_STATIC */
#include "src/func_001EAB50.c"
extern char D_00255620[64];

void func_001EACF0(int arg0, int arg1) {
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C),
                  1.0f, 1e-6f, 3.0f);
    func_001CFBE0(arg1, 0, D_00255620, D_0081F8F0, 0);
}
