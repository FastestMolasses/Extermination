// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_001DEDB0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Resets the two render records 2 and 9 (func_001DEDB0 returns their address in
// the render context): clears bytes 0..3 and stores the record id at +8, then
// for each id copies the three words of D_0026E850 into it (func_001DEE80) and
// sets its value to 0x60 (func_001DEEC0).
// Fixes read from the original: func_001DEDB0 is a same-TU static callee
// (TU_STATIC) so the compiler knows a0 still holds the id after the call and
// stores it from there; -sdatathreshold 4 (the callee's own); the four byte
// clears are written in the original order 0, 3, 2, 1.
static char *func_001DEDB0(int which);  /* same-TU callee, see TU_STATIC */
#include "src/func_001DEDB0.c"

extern void func_001DEE80(int which, int *src);
extern void func_001DEEC0(int which, int v);
extern int D_0026E850[];

void func_001DEDF0(void) {
    char *r;

    r = func_001DEDB0(2);
    r[0] = 0;
    r[3] = 0;
    r[2] = 0;
    r[1] = 0;
    *(int *)(r + 8) = 2;
    r = func_001DEDB0(9);
    r[0] = 0;
    r[3] = 0;
    r[2] = 0;
    r[1] = 0;
    *(int *)(r + 8) = 9;
    func_001DEE80(2, D_0026E850);
    func_001DEEC0(2, 0x60);
    func_001DEE80(9, D_0026E850);
    func_001DEEC0(9, 0x60);
}
