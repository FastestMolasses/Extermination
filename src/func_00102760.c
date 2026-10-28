// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28, replaces the asm body
// and the readable companion (src/readable/func_00102760.c).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0: normalise: dst.xyz = v.xyz / |v.xyz| (square root and divide
// through the Q register), dst.w = 0.

void func_00102760(float *dst, float *v)
{
    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(v) : "memory");
    asm __volatile__("vmul.xyz vf5, vf4, vf4" : : : "memory");
    asm __volatile__("vaddy.x vf5, vf5, vf5y" : : : "memory");
    asm __volatile__("vaddz.x vf5, vf5, vf5z" : : : "memory");
    asm __volatile__("vsqrt Q, vf5x" : : : "memory");
    asm __volatile__("vwaitq" : : : "memory");
    asm __volatile__("vaddq.x vf5, vf0, Q" : : : "memory");
    asm __volatile__("vdiv Q, vf0w, vf5x" : : : "memory");
    asm __volatile__("vsub.xyzw vf6, vf0, vf0" : : : "memory");
    asm __volatile__("vwaitq" : : : "memory");
    asm __volatile__("vmulq.xyz vf6, vf4, Q" : : : "memory");
    asm __volatile__("sqc2 vf6, 0x0(%0)" : : "r"(dst) : "memory");
}
