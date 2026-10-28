// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28, replaces the asm body
// and the readable companion (src/readable/func_00102918.c).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0 / MMI: translate a matrix: dst rows 0..2 = src rows 0..2 (raw
// quadword copies), dst row 3 xyz = src row 3 xyz + v xyz, w unchanged.
// Rows 0..2 are copied as raw 128-bit quadwords in C (volatile source keeps the
// three loads in order; the register variables reproduce the original's a3,
// t0, t1 choice).

typedef unsigned int u128 __attribute__((mode(TI)));
void func_00102918(u128 *dst, volatile u128 *m, float *v)
{
    register u128 r0 asm("$7");
    register u128 r1 asm("$8");
    register u128 r2 asm("$9");

    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(v) : "memory");
    asm __volatile__("lqc2 vf5, 0x30(%0)" : : "r"(m) : "memory");
    r0 = m[0];
    r1 = m[1];
    r2 = m[2];
    asm __volatile__("vadd.xyz vf5, vf5, vf4" : : : "memory");
    dst[0] = r0;
    dst[1] = r1;
    dst[2] = r2;
    asm __volatile__("sqc2 vf5, 0x30(%0)" : : "r"(dst) : "memory");
}
