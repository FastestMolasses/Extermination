// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class): the VU0 macro
// instructions are GCC-style asm statements, everything else is C.
//
// SDK VU0: transforms one vector by the 4x4 matrix m (four column vectors),
// divides x, y, z by the resulting w, and stores the vector converted to
// 12.4 fixed point (screen x/y). When zw_int is non-zero, z and w are
// instead converted as plain integers (vftoi0), as the GS XYZ/depth expects.

void func_00103198(int *dst, float *m, float *v, int zw_int)
{
    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf5, 0x10(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf6, 0x20(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf7, 0x30(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf8, 0x0(%0)" : : "r"(v) : "memory");
    asm __volatile__("vmulax.xyzw ACC, vf4, vf8x" : : : "memory");
    asm __volatile__("vmadday.xyzw ACC, vf5, vf8y" : : : "memory");
    asm __volatile__("vmaddaz.xyzw ACC, vf6, vf8z" : : : "memory");
    asm __volatile__("vmaddw.xyzw vf9, vf7, vf8w" : : : "memory");
    asm __volatile__("vdiv Q, vf0w, vf9w" : : : "memory");
    asm __volatile__("vwaitq" : : : "memory");
    asm __volatile__("vmulq.xyz vf9, vf9, Q" : : : "memory");
    asm __volatile__("vftoi4.xyzw vf10, vf9" : : : "memory");
    if (zw_int)
        asm __volatile__("vftoi0.zw vf10, vf9" : : : "memory");
    asm __volatile__("sqc2 vf10, 0x0(%0)" : : "r"(dst) : "memory");
}
