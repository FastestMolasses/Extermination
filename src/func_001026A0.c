// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28, replaces the asm body
// and the readable companion (src/readable/func_001026A0.c).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0: dst = m * v, the 4x4 matrix (rows of four) applied to a vec4:
// dst = m.row0 * v.x + m.row1 * v.y + m.row2 * v.z + m.row3 * v.w.

void func_001026A0(float *dst, float (*m)[4], float *v)
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
    asm __volatile__("sqc2 vf9, 0x0(%0)" : : "r"(dst) : "memory");
}
