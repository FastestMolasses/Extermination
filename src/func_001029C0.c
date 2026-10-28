// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28, replaces the asm body
// and the readable companion (src/readable/func_001029C0.c).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0: the identity matrix (rows built from the constant register
// (0, 0, 0, 1) and its lane rotations, row 3 first).

void func_001029C0(float (*m)[4])
{
    asm __volatile__("vsub.xyzw vf4, vf0, vf0" : : : "memory");
    asm __volatile__("vadd.w vf4, vf4, vf0" : : : "memory");
    asm __volatile__("vmr32.xyzw vf5, vf4" : : : "memory");
    asm __volatile__("vmr32.xyzw vf6, vf5" : : : "memory");
    asm __volatile__("vmr32.xyzw vf7, vf6" : : : "memory");
    asm __volatile__("sqc2 vf4, 0x30(%0)" : : "r"(m) : "memory");
    asm __volatile__("sqc2 vf5, 0x20(%0)" : : "r"(m) : "memory");
    asm __volatile__("sqc2 vf6, 0x10(%0)" : : "r"(m) : "memory");
    asm __volatile__("sqc2 vf7, 0x0(%0)" : : "r"(m) : "memory");
}
