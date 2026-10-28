// NEARMISS func_001172B8  (vram 0x001172B8, 0x170 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.32% via ee-gcc 2.9-991111-01 (-O2) (was 98.21%; 61.66% before 2026-10-27). The old
// "forward branch-likely wall" verdict was wrong: the old C had the first loop's test inverted
// and used byte offsets; written with typed Voice D_0027CCC0[48], Tempo D_0027F740 (unsigned
// rotating cursor f30) and plain loops, ee-gcc emits the original's branch-likely code.
// Matching wave 2026-10-28: the selection locals declared min2, min1, best2, best1 (best colouring
// of eight orders swept).
// Remaining differences: the t4..t8 colouring of the four selection locals and the divide-check
// zero, and at the final select the original loads -1 again for the "none" result where ours
// reuses best1 (known -1). Same tail pattern as func_00117428; ~15 tail spellings tried
// (ternary, nested if, `~x`, `x + 1 != 0`, unsigned compare, result local): none better.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

typedef struct {                /* 0x6A */
    unsigned short f00;         /* 0x00 */
    unsigned char pad02[0x08 - 0x02];
    unsigned short f08;         /* 0x08 */
    unsigned short f0A;         /* 0x0A */
    unsigned char pad0C[0x1A - 0x0C];
    unsigned short f1A;         /* 0x1A */
    unsigned char pad1C[0x6A - 0x1C];
} Voice;

typedef struct {
    unsigned char pad00[0x30];
    unsigned int f30;           /* 0x30 */
} Tempo;

extern Voice D_0027CCC0[48];
extern Tempo D_0027F740;

// Voice allocator: scans the 48 voices starting at the rotating cursor
// D_0027F740.f30 (cursor mod 48, advanced once per probe). First pass: return the
// first voice with f00 == 0 and f1A != 3. Second pass (another 48 probes): among
// voices with f1A == 1, pick the one with the lowest f0A, separately for f08 == 1
// and f08 != 1. Returns the f08 == 1 pick, else the other pick, else -1. The
// argument is unused.

int func_001172B8(int a0)
{
    int i;
    int idx;
    int min2 = -1;
    int min1 = -1;
    int best2 = -1;
    int best1 = -1;

    for (i = 0; i < 48; i++) {
        idx = D_0027F740.f30 % 48;
        if (D_0027CCC0[idx].f00 == 0 && D_0027CCC0[idx].f1A != 3) {
            return idx;
        }
        D_0027F740.f30++;
    }
    for (i = 0; i < 48; i++) {
        idx = D_0027F740.f30 % 48;
        if (D_0027CCC0[idx].f1A == 1) {
            if (D_0027CCC0[idx].f08 == 1) {
                if (D_0027CCC0[idx].f0A < min1) {
                    best1 = idx;
                    min1 = D_0027CCC0[idx].f0A;
                }
            } else {
                if (D_0027CCC0[idx].f0A < min2) {
                    best2 = idx;
                    min2 = D_0027CCC0[idx].f0A;
                }
            }
        }
        D_0027F740.f30++;
    }
    if (best1 != -1) {
        return best1;
    } else if (best2 != -1) {
        return best2;
    } else {
        return -1;
    }
}
