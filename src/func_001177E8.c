// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 56.35% NEARMISS).
// Fixes read from the original instructions:
//  - No forward-branch-likely wall: rewritten in the spelling of the byte-matched
//    sibling func_001176E0 (typed Voice D_0027CCC0[48], Seq argument, Tempo
//    D_0027F740 with the 64-bit mask f28, a plain for (i < 48) loop with a 64-bit
//    bit shifted per voice, one && chain) and ee-gcc emits the original's
//    branch-likely skips itself.
//  - The selector is `if (f0C != 0) { if (f18 != 0) f18 = 0; else f08 = 1; }
//    else { f08 = 1; f18 = 0; }`; the inverted f0C test laid the arms out wrong.
//  - The voice fields compared are f02/f04/f06/f1A/f22 (the old C's offsets were
//    relative to voice + 0x18); f1A is compared with 1.
//  - Returns seq->f18 and advances seq->f08 by 3 as the last statement.
//
// Sequencer event: for every voice with f00 == 1, f02 == seq byte +2,
// f04 == (seq byte +0 & 0xF), f06 == seq->f18, f1A == 1 and f22 == seq->f24:
// if f0C is set, clear f18 when it is set, else set f08 = 1; if f0C is clear, set
// f08 = 1 and clear f18. Each such voice ORs its bit into D_0027F740.f28.
// Advances seq->f08 by 3 and returns seq->f18.

typedef struct {                /* 0x6A */
    unsigned short f00;         /* 0x00 */
    unsigned short f02;         /* 0x02 */
    unsigned short f04;         /* 0x04 */
    unsigned short f06;         /* 0x06 */
    unsigned short f08;         /* 0x08 */
    unsigned char pad0A[0x0C - 0x0A];
    unsigned short f0C;         /* 0x0C */
    unsigned char pad0E[0x18 - 0x0E];
    unsigned short f18;         /* 0x18 */
    unsigned short f1A;         /* 0x1A */
    unsigned char pad1C[0x22 - 0x1C];
    unsigned short f22;         /* 0x22 */
    unsigned char pad24[0x6A - 0x24];
} Voice;

typedef struct {
    unsigned char f00;          /* 0x00 */
    unsigned char pad01;
    unsigned char f02;          /* 0x02 */
    unsigned char pad03[0x08 - 0x03];
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

int func_001177E8(Seq *seq)
{
    int i;
    unsigned long bit;

    for (i = 0, bit = 1; i < 48; i++, bit <<= 1) {
        if (D_0027CCC0[i].f00 == 1 && D_0027CCC0[i].f02 == seq->f02 &&
            D_0027CCC0[i].f04 == (seq->f00 & 0xF) && D_0027CCC0[i].f06 == seq->f18 &&
            D_0027CCC0[i].f1A == 1 && D_0027CCC0[i].f22 == seq->f24) {
            if (D_0027CCC0[i].f0C != 0) {
                if (D_0027CCC0[i].f18 != 0) {
                    D_0027CCC0[i].f18 = 0;
                } else {
                    D_0027CCC0[i].f08 = 1;
                }
            } else {
                D_0027CCC0[i].f08 = 1;
                D_0027CCC0[i].f18 = 0;
            }
            D_0027F740.f28 |= bit;
        }
    }
    seq->f08 += 3;
    return seq->f18;
}
