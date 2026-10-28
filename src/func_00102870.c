// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28, replaces the asm body
// and the readable companion (src/readable/func_00102870.c).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0: dst.xyz = src.xyz / d (through the Q register), dst.w = src.w.
// The scalar reaches VU0 through an integer register: the register variable
// pins it to t0 as the SDK's own clobber list did, and is assigned after the
// first vector load so the float-to-integer move follows that load as in the
// original.

void func_00102870(float *dst, float *v, float s)
{
    register float t asm("$8");

    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(v) : "memory");
    t = s;
    asm __volatile__("qmtc2.ni %0, vf5" : : "r"(t) : "memory");
    asm __volatile__("vdiv Q, vf0w, vf5x" : : : "memory");
    asm __volatile__("vwaitq" : : : "memory");
    asm __volatile__("vmulq.xyz vf4, vf4, Q" : : : "memory");
    asm __volatile__("sqc2 vf4, 0x0(%0)" : : "r"(dst) : "memory");
}
