// NEARMISS func_00179910  (vram 0x00179910, 0x280 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.31% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-28 (was 89.06%): the two record cases are an if / else-if pair with all three distance tests in one && chain, returning 1 on a hit and 0 at the end. The target has no VU0 instructions (0x70003A20.. are scratchpad stores), so no asm is used.
// Remaining differences: the first area test branches to the shared final return 0 with the arg0 copy in its delay slot; mwcc copies arg0 at entry and uses a branch-likely to its own return 0, which shifts a few register names.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Proximity test against the current scene table entry indexed by
// D_0024D650[D_00810700][D_00810701]. Computes |dx|,|dy|,|dz| between a
// target record's position (case 1: offsets 0xF0/0xF4/0xF8, case 0/2:
// offsets 0x120/0x124/0x128) and arg0's position (offsets 0xB0/0xB4/0xB8),
// via func_0011DF78 (fabsf). If all three deltas are < 8.0f, records a hit:
// D_008106B8 (found flag) = 1, D_008106B5 = area index, D_008106B7 = mode
// (6 for case 0/2, 5 for case 1), and D_008106B6 = sub-flag (1 for case
// 0/2; for case 1, derived from bit 0x80 of D_00810730[area]: 2 if set,
// else 0). Returns 1 on hit, 0 otherwise (including area != 2 / bad case).

extern float func_0011DF78(float a0);

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810730;
extern unsigned char D_008106B5;
extern unsigned char D_008106B6;
extern unsigned char D_008106B7;
extern unsigned char D_008106B8;
extern int **D_0024D650[];

#define VU0_A20 (*(float *)0x70003A20)
#define VU0_A24 (*(float *)0x70003A24)
#define VU0_A28 (*(float *)0x70003A28)

int func_00179910(char *arg0) {
    unsigned char *p;

    if (D_00810700 == 2) {
        if (D_00810701 == 0 || D_00810701 == 2) {
            p = (unsigned char *)D_0024D650[D_00810700][D_00810701];
            VU0_A20 = func_0011DF78(*(float *)(p + 0x120) - *(float *)(arg0 + 0xB0));
            VU0_A24 = func_0011DF78(*(float *)(p + 0x124) - *(float *)(arg0 + 0xB4));
            VU0_A28 = func_0011DF78(*(float *)(p + 0x128) - *(float *)(arg0 + 0xB8));
            if (VU0_A20 < 8.0f && VU0_A24 < 8.0f && VU0_A28 < 8.0f) {
                D_008106B8 = 1;
                D_008106B5 = D_00810700;
                D_008106B7 = 6;
                D_008106B6 = 1;
                return 1;
            }
        } else if (D_00810701 == 1) {
            p = (unsigned char *)D_0024D650[D_00810700][D_00810701];
            VU0_A20 = func_0011DF78(*(float *)(p + 0xF0) - *(float *)(arg0 + 0xB0));
            VU0_A24 = func_0011DF78(*(float *)(p + 0xF4) - *(float *)(arg0 + 0xB4));
            VU0_A28 = func_0011DF78(*(float *)(p + 0xF8) - *(float *)(arg0 + 0xB8));
            if (VU0_A20 < 8.0f && VU0_A24 < 8.0f && VU0_A28 < 8.0f) {
                D_008106B8 = 1;
                D_008106B5 = D_00810700;
                D_008106B7 = 5;
                if (*(&D_00810730 + D_00810700) & 0x80) {
                    D_008106B6 = 2;
                } else {
                    D_008106B6 = 0;
                }
                return 1;
            }
        }
    }
    return 0;
}
