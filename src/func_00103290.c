// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0 (sceVu0ClipScreen3 shape): the same screen-bounds test as
// func_00103248 applied to three vertices; the status flags accumulate, so
// the result (status & 0xC0) is nonzero if ANY of the three is off screen
// (x, y or w <= 0, or x or y >= 4096).

int func_00103290(float *v0, float *v1, float *v2)
{
    unsigned long c;
    int flags;

    asm __volatile__("vsub.xyzw vf4, vf0, vf0" : : : "memory");
    c = 0x4580000045800000UL;
    asm __volatile__("lqc2 vf6, 0x0(%0)" : : "r"(v0) : "memory");
    asm __volatile__("lqc2 vf8, 0x0(%0)" : : "r"(v1) : "memory");
    asm __volatile__("lqc2 vf9, 0x0(%0)" : : "r"(v2) : "memory");
    asm __volatile__("qmtc2.ni %0, vf7" : : "r"(c) : "memory");
    asm __volatile__("ctc2.ni $0, $vi16" : : : "memory");
    asm __volatile__("vsub.xyw vf5, vf6, vf4" : : : "memory");
    asm __volatile__("vsub.xy vf5, vf7, vf6" : : : "memory");
    asm __volatile__("vsub.xyw vf5, vf8, vf4" : : : "memory");
    asm __volatile__("vsub.xy vf5, vf7, vf8" : : : "memory");
    asm __volatile__("vsub.xyw vf5, vf9, vf4" : : : "memory");
    asm __volatile__("vsub.xy vf5, vf7, vf9" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("cfc2.ni %0, $vi16" : "=r"(flags) : : "memory");
    return flags & 0xC0;
}
