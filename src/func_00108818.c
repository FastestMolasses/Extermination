// NEARMISS func_00108818  (vram 0x00108818, 0x88 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.79% via ee-gcc 2.9-991111-01 (-O2). Instruction stream, control flow and
// size match the original exactly (the movn of the old if/else spelling is gone: the
// mask test is a switch, as in the sibling func_001088A0).
// Remaining differences: register colouring only — the argument copy, the scaled index
// and the 0xFFFF000000 constant land in t0/a3/a2 instead of a3/a2/a0 (and the table
// base in t1 instead of t0); survives declaration order, local copies, types and
// result/return spellings.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// Returns entry idx (idx < 10) of the 16-byte {value, mask} table D_00241238 with
// arg shifted into the field the mask selects (24 for mask 0xFFFF000000, 32 for
// 0xFF00000000, 0 otherwise) OR-ed into the value; 0 for an out-of-range index.

typedef struct {
    unsigned long val;
    unsigned long mask;
} Ent;

extern Ent D_00241238[];

unsigned long func_00108818(unsigned int idx, unsigned long arg)
{
    unsigned long result = 0;
    int sh = 0;

    if (idx < 10) {
        switch (D_00241238[idx].mask) {
        case 0xFFFFFFFFFFL: sh = 0; break;
        case 0xFFFF000000L: sh = 24; break;
        case 0xFF00000000L: sh = 32; break;
        }
        result = D_00241238[idx].val | (arg << sh);
    }
    return result;
}
