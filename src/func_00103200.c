// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0 (sceVu0InterVectorXYZ shape): dst.xyz = v0.xyz * t + v1.xyz * (1 - t),
// dst.w = v0.w. The scalar reaches VU0 through an integer register: the
// register variable pins it to t0 and is assigned after both vector loads so
// the float-to-integer move follows them as in the original.

void func_00103200(float *dst, float *v0, float *v1, float t)
{
    register float r asm("$8");

    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(v0) : "memory");
    asm __volatile__("lqc2 vf5, 0x0(%0)" : : "r"(v1) : "memory");
    r = t;
    asm __volatile__("qmtc2.ni %0, vf6" : : "r"(r) : "memory");
    asm __volatile__("vmove.w vf9, vf4" : : : "memory");
    asm __volatile__("vaddw.x vf7, vf0, vf0w" : : : "memory");
    asm __volatile__("vsub.x vf8, vf7, vf6" : : : "memory");
    asm __volatile__("vmulax.xyz ACC, vf4, vf6x" : : : "memory");
    asm __volatile__("vmaddx.xyz vf9, vf5, vf8x" : : : "memory");
    asm __volatile__("sqc2 vf9, 0x0(%0)" : : "r"(dst) : "memory");
}
