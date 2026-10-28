// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 90.51% NEARMISS).
// Fixes read from the original instructions:
// - The destination array is the relocated symbol D_70003190 (its address is
//   built from a relocated upper/lower pair), not a literal address.
// - 0x700031D0 is a relocated scratchpad extern (idiom-32, `// SPAD:`), as in
//   the byte-matched sibling func_0019B2C0; that keeps the following upper-half
//   load out of the call-result test's delay slot (the old "scheduling wall").
// - Local declaration order: the loop counter is declared before the
//   destination pointer, which gives the original register assignment.
//
// Copies 3 floats from arg0[] into the global float array at 0x70003190 and 3
// floats from arg1[] into the parallel array at +0x10, then writes the 1.0f
// constants at 0x700031AC / 0x7000319C, zeroes 0x700031D4, calls
// func_001A7280(arg2 & 0xFFFF), and returns 1 (also stored at 0x700031D8) on
// success, else zeroes 0x700031D0 and returns 0.
extern int func_001A7280(int a);
extern float D_70003190[];
extern int D_700031D0;

int func_0019AA80(float *arg0, float *arg1, int arg2) {
    int i;
    float *dst;
    int ret;

    dst = D_70003190;
    ret = 0;
    i = 0;
    do {
        i += 1;
        *dst = *arg0;
        arg0 += 1;
        dst[4] = *arg1;
        dst += 1;
        arg1 += 1;
    } while (i < 3);
    *(int *)0x700031AC = 0x3F800000;
    *(int *)0x7000319C = 0x3F800000;
    *(int *)0x700031D4 = 0;
    if (func_001A7280(arg2 & 0xFFFF) != 0) {
        ret = 1;
    } else {
        D_700031D0 = 0;
    }
    *(int *)0x700031D8 = ret;
    return ret;
}
