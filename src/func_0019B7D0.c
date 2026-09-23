// NEARMISS func_0019B7D0  (vram 0x0019B7D0, 0xE8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Only residual: the expected object renders scratchpad 0x700031D0,0x700031D4,0x700031D8 as literal operands (not in build.py _SPAD_SYMS) while this C uses relocated externs; linked bytes identical. objdiff 100.0% with _SPAD_SYMS += 0x700031D0,0x700031D4,0x700031D8.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// MATCH NOTE (m1-firstlevel-matching lane): the scratchpad globals at 0x700031D0, 0x700031D4, 0x700031D8
// are referenced as relocated externs (D_7000xxxx), as the original build did
// (see tools/match/spad_symbolize.py). A literal address let mwcc CSE and
// speculate `lui at,0x7000` into delay slots that the target leaves as nop.
// objdiff is 100.0% once build.py _SPAD_SYMS lists these addresses (so the
// expected object carries the same relocations); the linked bytes are identical.

//
// Semantics: copies two input float vectors a0[0..2], a1[0..2] into the
// global block at D_70003190 (a0 -> +0x00, a1 -> +0x10), stashing a1 in a
// local; seeds three globals to 1.0f / 0 (D_70003190+0x0C, +0x1C, 0x700031D4);
// calls func_0019E280 (an acceptance/validation check). If it returns nonzero,
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
