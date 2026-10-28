// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class): the VU0 macro
// instructions are GCC-style asm statements, everything else is C.
//
// SDK VU0: linear interpolation, dst = v0 * t + v1 * (1 - t) in all four
// lanes (t = 1 returns v0, t = 0 returns v1). The scalar reaches VU0 through
// t0 (register variable), assigned after both vector loads as in the original.

void func_00102890(float *dst, float *v0, float *v1, float t)
{
    register float r asm("$8");

    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(v0) : "memory");
    asm __volatile__("lqc2 vf5, 0x0(%0)" : : "r"(v1) : "memory");
    r = t;
    asm __volatile__("qmtc2.ni %0, vf6" : : "r"(r) : "memory");
    asm __volatile__("vaddw.x vf7, vf0, vf0w" : : : "memory");
    asm __volatile__("vsub.x vf8, vf7, vf6" : : : "memory");
    asm __volatile__("vmulax.xyzw ACC, vf4, vf6x" : : : "memory");
    asm __volatile__("vmaddx.xyzw vf9, vf5, vf8x" : : : "memory");
    asm __volatile__("sqc2 vf9, 0x0(%0)" : : "r"(dst) : "memory");
}
