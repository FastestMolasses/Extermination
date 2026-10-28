// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_001EBF10
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 76.87% NEARMISS).
// Fixes read from the original instructions:
//  - D_00275C34 is gp-relative: -sdatathreshold 4 (the file said 0).
//  - the order in which mwcc 2.3.3 sets up the three float constants (1.0, 5.0,
//    1e-6) depends on what the compiler did earlier in the same translation unit,
//    not on this function's source alone. Compiling the real preceding function
//    func_001EBF10 first (as a same-TU static, see TU_STATIC; tu_static.py removes
//    the copy after checking it equals the original) reproduces the original order.
//
// Animation/effect setup wrapper, sibling of func_001EAF80 (same template, this one
// uses constant 5.0f, table D_00256700, one func_001CFBE0 call). Pulls two floats
// from the gp-relative global object *D_00275C34 (fields +0x54 and +0x5C) plus the
// constants 1.0f, 1e-6f and 5.0f into func_001CFB50(&D_0081F8F0, 0, arg0, ...), then
// func_001CFBE0(arg1, 1, D_00256700, &D_0081F8F0, 0).
static void func_001EBF10(char *arg0, int arg1);  /* same-TU predecessor, see TU_STATIC */
#include "src/func_001EBF10.c"
extern int D_00256700[2];

void func_001EC1F0(int arg0, int arg1) {
    float one = 1.0f;
    float five = 5.0f;
    float f54 = *(float *)((char *)D_00275C34 + 0x54);
    float tiny = 1e-6f;
    float f5c = *(float *)((char *)D_00275C34 + 0x5C);
    func_001CFB50(D_0081F8F0, 0, (void *)arg0, f54, f5c, one, tiny, five);
    func_001CFBE0(arg1, 1, D_00256700, D_0081F8F0, 0);
}
