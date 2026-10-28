// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28, replaces the asm body
// and the readable companion (src/readable/func_00102990.c).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0: dst = (int)src lane by lane (vftoi0: truncation toward zero, the
// VU saturates out-of-range values instead of producing the FPU's result).

void func_00102990(int *dst, float *v)
{
    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(v) : "memory");
    asm __volatile__("vftoi0.xyzw vf5, vf4" : : : "memory");
    asm __volatile__("sqc2 vf5, 0x0(%0)" : : "r"(dst) : "memory");
}
