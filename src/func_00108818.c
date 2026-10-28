// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 97.79% NEARMISS).
// Fixes read from the original instructions:
//  - the mask test is a switch (as in the sibling func_001088A0), not if/else
//  - the second parameter is an int, widened to 64 bits only for the shift
//    ((unsigned long)arg << sh); the 64-bit parameter recoloured the argument
//    copy, the scaled index, the mask constant and the table base
//    (the caller func_00108DB0 passes an int)

// Returns entry idx (idx < 10) of the 16-byte {value, mask} table D_00241238 with
// arg shifted into the field the mask selects (24 for mask 0xFFFF000000, 32 for
// 0xFF00000000, 0 otherwise) OR-ed into the value; 0 for an out-of-range index.

typedef struct {
    unsigned long val;
    unsigned long mask;
} Ent;

extern Ent D_00241238[];

unsigned long func_00108818(unsigned int idx, int arg)
{
    unsigned long result = 0;
    int sh = 0;

    if (idx < 10) {
        switch (D_00241238[idx].mask) {
        case 0xFFFFFFFFFFL: sh = 0; break;
        case 0xFFFF000000L: sh = 24; break;
        case 0xFF00000000L: sh = 32; break;
        }
        result = D_00241238[idx].val | ((unsigned long)arg << sh);
    }
    return result;
}
