// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class): the VU0 macro
// instructions are GCC-style asm statements, everything else is C.
//
// SDK VU0: clamps every lane of v to [lo, hi] (max with lo first, then min
// with hi) and stores the result in dst. Both scalars move to t0/t1 before the
// vector load, as in the original.

void func_00102CA8(float *dst, float *v, float lo, float hi)
{
    register float rlo asm("$8");
    register float rhi asm("$9");

    rlo = lo;
    rhi = hi;
    asm __volatile__("lqc2 vf6, 0x0(%0)" : : "r"(v) : "memory");
    asm __volatile__("qmtc2.ni %0, vf4" : : "r"(rlo) : "memory");
    asm __volatile__("qmtc2.ni %0, vf5" : : "r"(rhi) : "memory");
    asm __volatile__("vmaxx.xyzw vf6, vf6, vf4x" : : : "memory");
    asm __volatile__("vminix.xyzw vf6, vf6, vf5x" : : : "memory");
    asm __volatile__("sqc2 vf6, 0x0(%0)" : : "r"(dst) : "memory");
}
