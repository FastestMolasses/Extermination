// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_001EBD20 func_001EBE10
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 79.80% NEARMISS).
// Fixes read from the original instructions:
//  - the order in which mwcc 2.3.3 sets up the float constants (1e-6, 1.0) depends
//    on what the compiler did earlier in the same translation unit, not on this
//    function's source alone. Compiling the real preceding functions func_001EBD20
//    and func_001EBE10 first (same-TU statics, see TU_STATIC; func_001EBE10's file
//    brings func_001EBD20; tu_static.py removes both copies after checking they
//    equal the originals) reproduces the original order.
//  - arg0 is a pointer passed straight through (a cast from int changes the
//    schedule); 1e-6 is written in the call, not held in a local.
//
// Single-call sibling of func_001EAF80: func_001CFB50(&D_0081F8F0, 0, arg0,
// D_00275C34->+0x54, D_00275C34->+0x5C, 1.0f, 1e-6f, 0.0f), then
// func_001CFBE0(arg1, 1, D_00256550, &D_0081F8F0, 0).
static void func_001EBE10(void *arg0, int arg1);  /* same-TU predecessor, see TU_STATIC */
#include "src/func_001EBE10.c"
extern char D_00256550[0x100];

void func_001EBE90(void *arg0, int arg1) {
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)(D_00275C34 + 0x54),
                  *(float *)(D_00275C34 + 0x5C),
                  1.0f, 1e-6f, 0.0f);
    func_001CFBE0(arg1, 1, D_00256550, D_0081F8F0, 0);
}
