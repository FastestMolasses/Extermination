// NEARMISS func_00117918  (vram 0x00117918, 0xC8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.24% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall (confirmed s84) — opcode/operand-structure identical, only physical register assignment differs (target: a3-copy in v1, address accumulator v0, loaded halfword in v1; ours: a3-copy in t1, accumulator v1, load in v0), which drags two adjacent independent ops (lui %hi vs add...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// NEARMISS 89.24 — eegcc GPR-coloring wall (confirmed s84). Instruction sequence,
// opcodes and operand structure are identical to the target; only the physical
// register assignment differs (target keeps the a3 copy in v1 and accumulates the
// element address in v0; ee-gcc keeps them in t1/v1 and loads into v0), which also
// pulls two adjacent independent ops (lui %hi / addiu -0x40, and the dead `li 12`
// reload of the divisor) past each other. Logic fully recovered.
extern unsigned short D_00241D70[];

int func_00117918(int a0, int a1, int a2, int a3, int a4) {
    int t, d;

    if (a0 <= a1) {
        t = (((a3 - 0x40) * a4) >> 2) + 0xD0;
        d = a1 - a0;
        return D_00241D70[(d % 12) * 16 + a2 + t] << (d / 12);
    } else {
        t = (((a3 - 0x40) * a4) >> 2) + 0xD0;
        d = a0 - a1;
        return D_00241D70[(12 - d % 12) * 16 + a2 + t] >> (d / 12 + 1);
    }
}
