// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Compiler: ee-gcc 2.9 (-O2), as the SDK's libvu0; the VU0 macro instructions
// are GCC-style asm statements, everything else is C.
//
// SDK VU0 (sceVu0ClipScreen shape): screen-bounds test of one vertex.
// Clears the VU0 status flag, then computes vertex - 0 (x, y, w) and
// (4096, 4096) - vertex (x, y) so the flags record any sign or zero; returns the
// zero-sticky and sign-sticky status bits (mask 0xC0). Nonzero means the
// vertex is off screen: x, y or w <= 0, or x or y >= 4096 (exact edges count
// as outside). The limit constant is built after the first VU0 op, so it is
// assigned there in the C.

int func_00103248(float *v)
{
    unsigned long c;
    int flags;

    asm __volatile__("vsub.xyzw vf4, vf0, vf0" : : : "memory");
    c = 0x4580000045800000UL;
    asm __volatile__("lqc2 vf7, 0x0(%0)" : : "r"(v) : "memory");
    asm __volatile__("qmtc2.ni %0, vf6" : : "r"(c) : "memory");
    asm __volatile__("ctc2.ni $0, $vi16" : : : "memory");
    asm __volatile__("vsub.xyw vf5, vf7, vf4" : : : "memory");
    asm __volatile__("vsub.xy vf5, vf6, vf7" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("vnop" : : : "memory");
    asm __volatile__("cfc2.ni %0, $vi16" : "=r"(flags) : : "memory");
    return flags & 0xC0;
}
