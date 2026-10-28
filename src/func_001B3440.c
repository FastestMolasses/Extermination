// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 92.07% NEARMISS).
// Fixes read from the original instructions:
//  - the first call's result is assigned inside the test
//    (`if ((r = func_0019AB20(...)) == 0)`): the original tests the raw return
//    value and copies it to the saved register in the branch delay slot; a
//    separate assignment statement made mwcc test the saved copy instead.
//
// Sets the w component of two vectors to 1.0, runs func_0019AB20 on them; on
// success queries a float through func_0019A310 and, if it is <= f, calls
// func_001028B8 on the object's field at 0xB0 and returns the first result.
// Returns 0 on any failure.

extern int func_0019AB20(int a0, int a1, int a2, int a3);
extern int func_0019A310(int a0);
extern int func_001028B8(int a0, int a1, int a2);
extern int D_700031C0;

int func_001B3440(float f, int a0, int *a1, int *a2) {
    int r;
    float t;

    *(int *)((char *)a2 + 0xC) = 0x3F800000;
    *(int *)((char *)a1 + 0xC) = 0x3F800000;
    if ((r = func_0019AB20(a0, (int)a1, (int)a2, 7)) == 0) {
        return 0;
    }
    if (func_0019A310((int)&t) == 0) {
        return 0;
    }
    if (!(t <= f)) {
        return 0;
    }
    func_001028B8(a0 + 0xB0, a0 + 0xB0, (int)&D_700031C0);
    return r;
}
