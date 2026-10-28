// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.88% NEARMISS).
// Fixes read from the original instructions:
//  - The first call's result is assigned inside its own test
//    (`if ((r = func_0019AD00(...)) == 0)`), so the compiler tests the raw
//    return value and copies it to the saved register in the branch delay slot.
//    A separate `r = ...;` statement tested the saved copy instead.
//
// Sets a1's +0xC word to 1.0f, then runs three guards (func_0019AD00 result,
// func_0019A310 output flag, sampled value t must exceed f), each returning 0
// on failure; on success calls func_001028B8 on a0+0xB0 with D_700031C0 and
// returns the first call's result.

extern int func_0019AD00(int a0, int a1, int a2);
extern int func_0019A310(int a0);
extern int func_001028B8(int a0, int a1, int a2);
extern int D_700031C0;

int func_001B32F0(int a0, int *a1, float f) {
    int r;
    float t;
    *(int *)((char *)a1 + 0xC) = 0x3F800000;
    if ((r = func_0019AD00(a0, (int)a1, 7)) == 0) {
        return 0;
    }
    if (func_0019A310((int)((char *)&t)) == 0) {
        return 0;
    }
    if (t <= f) {
        return 0;
    }
    func_001028B8(a0 + 0xB0, a0 + 0xB0, (int)&D_700031C0);
    return r;
}
