// NEARMISS func_00119C98  (vram 0x00119C98, 0x6C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 65.19% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc reg-alloc/delay-slot wall. C reaches objdiff 96.3% (permuter best, score 100); body is instruction-for-instruction identical EXCEPT one trailing nop placement at the function tail: expected emits `sw v0,0x1c(a3); jr ra; nop` (27 instrs) while ee-gcc 2.9 -O2 emits `sw; nop; jr ra; nop` (28 instrs, +4 bytes ove...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern char D_0027E0C0[];
extern unsigned short D_0027F77A;

void func_00119C98(unsigned int a0, int a1) {
    char *e = D_0027E0C0 + a0 * 0x78;

    if (a0 < 0x30 && a1 >= 0 && a1 < 0x3C1) {
        *(short *)(e + 0x54) = (short)a1;
        *(int *)(e + 0x1C) =
            (int)((*(unsigned short *)(e + 0x56) * (a1 & 0xFFFF)) << 12)
            / D_0027F77A / 0x3C;
    }
}
