// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700031D0 0x700031D8

//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x700031D0, 0x700031D4, 0x700031D8
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// The `// SPAD:` directive above opts THIS file's expected object into the
// same %hi/%lo relocations (build.py _symbolize_scratchpad); the address stays
// out of the global _SPAD_SYMS because other matched files name the symbol
// yet store through literals. objdiff 100.0% (m2-matching lane).

//
// Semantics: copies two input float vectors a0[0..2], a1[0..2] into the
// global block at D_70003190 (a0 -> +0x00, a1 -> +0x10), stashing a1 in a
// local; seeds three globals to 1.0f / 0 (D_70003190+0x0C, +0x1C, 0x700031D4);
// calls func_0019E280 (the AABB sweep over the D_70003190/D_700031A0 segment;
// see its matched source). If it returns nonzero,
// ret=4 and the a1 copy is re-applied (D_70003190+0x10); otherwise 0x700031D0
// is cleared. Records the result code at 0x700031D8 and returns it.

extern int D_700031D0;
extern int D_700031D4;
extern int D_700031D8;
extern float D_70003190[];
extern int func_0019E280(void);

int func_0019B7D0(float *a0, float *a1) {
    float local[4];
    int i;
    int ret = 0;

    for (i = 0; i < 3; i++) {
        float t;
        D_70003190[i] = a0[i];
        t = a1[i];
        local[i] = t;
        D_70003190[i + 4] = t;
    }

    *(float *)0x700031AC = 1.0f;
    *(float *)0x7000319C = 1.0f;
    D_700031D4 = 0;
    local[3] = 1.0f;

    if (func_0019E280()) {
        ret = 4;
    }

    if (ret != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[i + 4] = local[i];
        }
    } else {
        D_700031D0 = 0;
    }

    D_700031D8 = ret;
    return ret;
}
