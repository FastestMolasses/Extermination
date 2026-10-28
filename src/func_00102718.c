// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28, replaces the asm body
// and the readable companion (src/readable/func_00102718.c).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0: cross product dst.xyz = a.xyz x b.xyz (an outer-product
// multiply-subtract pair), dst.w = 0.

void func_00102718(float *dst, float *a, float *b)
{
    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(a) : "memory");
    asm __volatile__("lqc2 vf5, 0x0(%0)" : : "r"(b) : "memory");
    asm __volatile__("vopmula.xyz ACC, vf4, vf5" : : : "memory");
    asm __volatile__("vopmsub.xyz vf6, vf5, vf4" : : : "memory");
    asm __volatile__("vsub.w vf6, vf6, vf6" : : : "memory");
    asm __volatile__("sqc2 vf6, 0x0(%0)" : : "r"(dst) : "memory");
}
