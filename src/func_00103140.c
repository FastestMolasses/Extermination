// NEARMISS func_00103140  (vram 0x00103140, 0x58 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.09% via ee-gcc 2.9-991111-01 (-O2) (was an asm-word body; matching wave 2026-10-28).
// The original is hand-written assembly (splat marks it handwritten): it steps the counter and
// both pointers with the trapping add-immediate form and tests the counter against zero with
// the operands swapped, which no C compiler emits. Remaining differences: those four
// instructions only (same operations, same registers, same order); everything else is identical.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// inline VU0 asm (user-approved 2026-10-28 for the VU0 class): the VU0 macro
// instructions are GCC-style asm statements, everything else is C.
//
// SDK VU0: the batch form of func_00103198. Loads the 4x4 matrix m (four
// column vectors) once, then for each of count vectors from src: transforms
// it, divides x, y, z by w, converts to 12.4 fixed point (z and w as plain
// integers when zw_int is non-zero) and stores the 16-byte result to dst.
// The loop is a do/while: count is decremented before the test, so a count
// of 0 runs (practically) forever, as in the original.

void func_00103140(int *dst, float *m, float *src, int count, int zw_int)
{
    asm __volatile__("lqc2 vf4, 0x0(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf5, 0x10(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf6, 0x20(%0)" : : "r"(m) : "memory");
    asm __volatile__("lqc2 vf7, 0x30(%0)" : : "r"(m) : "memory");
    do {
        asm __volatile__("lqc2 vf8, 0x0(%0)" : : "r"(src) : "memory");
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
        count--;
        src += 4;
        dst += 4;
    } while (count != 0);
}
