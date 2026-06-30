// NEARMISS func_00109F90  (vram 0x00109F90, 0x3C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.33% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// ee-gcc register-coloring wall. Entire instruction stream, branch order (beq v1,v0 -> E68; fall-through FD0), jal-not-tail-call shape (achieved via trailing asm volatile("") + early return), and both delay slots are byte-identical. The ONLY diff (1 instruction, 14/15 match): the comparison constant 0x3 is allocated t...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_002414AC;

extern void func_00109FD0(void);
extern void func_00109E68(void);

void func_00109F90(void)
{
    if (D_002414AC != 3) {
        func_00109FD0();
        asm volatile("");
        return;
    }
    func_00109E68();
    asm volatile("");
}
