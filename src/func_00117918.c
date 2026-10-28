// NEARMISS func_00117918  (vram 0x00117918, 0xC8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.20% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// is ee-gcc register colouring inside each branch (divisor constant, quotient and table-address registers).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// NEARMISS 94.20 — matching wave 2026-10-27 (was 89.24%): the parameter copies at entry and
// the opcode order now match the original. Changes: the first branch adjusts a3 in place
// (a3 -= 0x40) before the scale, the second branch reuses a1 for the note distance, and the
// result goes through one local returned after the if/else.
// Remaining differences: register choice only — in the first branch the divisor 12 and the
// scaled offset sit in other argument registers, in the second the quotient and the table
// address swap registers.
//
// Pitch lookup: distance between two notes (a0, a1) indexes a 12-note x 16-step table
// (fine step a2 plus a scaled bend ((a3 - 0x40) * a4 >> 2) + 0xD0); the value is shifted
// up by whole octaves when a1 >= a0, otherwise taken from the mirrored note and shifted down.
extern unsigned short D_00241D70[];

int func_00117918(int a0, int a1, int a2, int a3, int a4) {
    int t, d, r;

    if (a0 <= a1) {
        a3 -= 0x40;
        t = ((a3 * a4) >> 2) + 0xD0;
        d = a1 - a0;
        r = D_00241D70[(d % 12) * 16 + a2 + t] << (d / 12);
    } else {
        t = (((a3 - 0x40) * a4) >> 2) + 0xD0;
        a1 = a0 - a1;
        r = D_00241D70[(12 - a1 % 12) * 16 + a2 + t] >> (a1 / 12 + 1);
    }
    return r;
}
