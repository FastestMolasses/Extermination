// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 68.60% NEARMISS).
// Fixes read from the original instructions:
//  - Rewritten in the spelling of the byte-matched sibling func_001186B0: typed
//    Voice D_0027CCC0[48], Seq argument and the 64-bit mask word Tempo.f28 at
//    D_0027F740 + 0x28, plain for (i < 48) loops with a 64-bit bit variable
//    shifted once per voice, instead of hand-strength-reduced byte pointers.
//    No branch-likely or scheduler wall was involved.
//  - The body was wrong: the f06 == seq->f18 test does not gate the match. Voices
//    passing the other tests set their bit in a "partial" mask, and only those
//    whose f06 also matches set it in the "exact" mask (the old C set both).
//  - The cursor advance is `seq->f08 += 4` as the last statement (a value cached
//    in a local made the compiler form the sum early).
//
// Sequencer event: for every voice with f00 == 1, f1A == 2, f0C == 1,
// f3E == seq byte +4, f02 == seq byte +2 and f22 == seq->f24, OR the voice's bit
// into the exact mask when f06 == seq->f18, else into the partial mask. If no
// voice matched exactly, use the partial mask. Every voice whose bit is set gets
// f08 = 1 and its bit ORed into D_0027F740.f28. Advances seq->f08 by 4 and
// returns 0.

typedef struct {                /* 0x6A */
    unsigned short f00;         /* 0x00 */
    unsigned short f02;         /* 0x02 */
    unsigned short f04;         /* 0x04 */
    unsigned short f06;         /* 0x06 */
    unsigned short f08;         /* 0x08 */
    unsigned char pad0A[0x0C - 0x0A];
    unsigned short f0C;         /* 0x0C */
    unsigned char pad0E[0x1A - 0x0E];
    unsigned short f1A;         /* 0x1A */
    unsigned char pad1C[0x22 - 0x1C];
    unsigned short f22;         /* 0x22 */
    unsigned char pad24[0x3E - 0x24];
    unsigned short f3E;         /* 0x3E */
    unsigned char pad40[0x6A - 0x40];
} Voice;

typedef struct {
    unsigned char f00;          /* 0x00 */
    unsigned char pad01;
    unsigned char f02;          /* 0x02 */
    unsigned char pad03;
    unsigned char f04;          /* 0x04 */
    unsigned char pad05[0x08 - 0x05];
    int f08;                    /* 0x08 */
    unsigned char pad0C[0x18 - 0x0C];
    int f18;                    /* 0x18 */
    unsigned char pad1C[0x24 - 0x1C];
    unsigned short f24;         /* 0x24 */
} Seq;

typedef struct {
    unsigned char pad00[0x28];
    unsigned long f28;          /* 0x28 */
} Tempo;

extern Voice D_0027CCC0[48];
extern Tempo D_0027F740;

int func_001176E0(Seq *seq)
{
    int i;
    unsigned long bit;
    unsigned long exact;
    unsigned long partial;

    exact = 0;
    partial = 0;
    for (i = 0, bit = 1; i < 48; i++, bit <<= 1) {
        if (D_0027CCC0[i].f00 == 1 && D_0027CCC0[i].f1A == 2 &&
            D_0027CCC0[i].f0C == 1 && D_0027CCC0[i].f3E == seq->f04 &&
            D_0027CCC0[i].f02 == seq->f02 && D_0027CCC0[i].f22 == seq->f24) {
            if (D_0027CCC0[i].f06 == seq->f18) {
                exact |= bit;
            } else {
                partial |= bit;
            }
        }
    }
    if (exact == 0) {
        exact = partial;
    }
    for (i = 0, bit = 1; i < 48; i++, bit <<= 1) {
        if (exact & bit) {
            D_0027CCC0[i].f08 = 1;
            D_0027F740.f28 |= bit;
        }
    }
    seq->f08 += 4;
    return 0;
}
