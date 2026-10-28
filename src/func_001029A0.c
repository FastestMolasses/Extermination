// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class): the VU0 macro
// instructions are GCC-style asm statements, everything else is C.
//
// SDK VU0: dst = src (12.4 fixed-point integers) converted lane by lane to
// float (vitof4: value / 16).

void func_001029A0(float *dst, int *v)
{
    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(v) : "memory");
    asm __volatile__("vitof4.xyzw vf5, vf4" : : : "memory");
    asm __volatile__("sqc2 vf5, 0x0(%0)" : : "r"(dst) : "memory");
}
