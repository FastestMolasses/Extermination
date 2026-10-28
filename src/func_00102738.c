// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28, replaces the asm body
// and the readable companion (src/readable/func_00102738.c).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0: the xyz dot product a.x * b.x + a.y * b.y + a.z * b.z.
// The dot product leaves VU0 through an integer register and is returned as
// the float result.

float func_00102738(float *a, float *b)
{
    float ret;

    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(a) : "memory");
    asm __volatile__("lqc2 vf5, 0x0(%0)" : : "r"(b) : "memory");
    asm __volatile__("vmul.xyz vf5, vf4, vf5" : : : "memory");
    asm __volatile__("vaddy.x vf5, vf5, vf5y" : : : "memory");
    asm __volatile__("vaddz.x vf5, vf5, vf5z" : : : "memory");
    asm __volatile__("qmfc2.ni %0, vf5" : "=r"(ret) : : "memory");
    return ret;
}
