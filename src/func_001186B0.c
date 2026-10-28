// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 79.67% NEARMISS).
// Fixes read from the original instructions:
//  - Typed records indexed directly (Voice D_0027CCC0[48], Seq argument, Song
//    D_00281AC0, the 64-bit mask word at D_0027F740 + 0x28) in a for (i < 48)
//    loop, as in the byte-matched sibling func_00118078, instead of
//    hand-strength-reduced byte pointers; no branch-likely wall was involved.
//  - The voice mask is its own 64-bit variable, set to 1 in the loop init and
//    shifted left once per voice (not 1L << i).
//  - The flag test is `== 0` with the mask/f08 arm first and the f18 store in
//    the else arm; the stores write the constant 1 (the compiler reuses the
//    loaded f0C register because f0C == 1 was just tested).
//
// Sequencer event with a 3-byte payload: copy payload[2] into byte 0xB of the
// song's p0C block. If it is zero, for every voice with f04 == (seq->f00 & 0xF),
// f22 == seq->f24, f06 == seq->f18 and f0C == 1: when the p0C flag byte is
// still zero, OR the voice's bit into the mask at D_0027F740 + 0x28 and set
// f08 = 1, otherwise set f18 = 1. Then advance seq->f08 by 3 and return it.

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
    unsigned char pad1A[0x22 - 0x1A];
    unsigned short f22;         /* 0x22 */
    unsigned char pad24[0x6A - 0x24];
} Voice;

typedef struct {
    unsigned char f00;          /* 0x00 */
    unsigned char pad01[0x08 - 0x01];
    int f08;                    /* 0x08 */
    unsigned char pad0C[0x18 - 0x0C];
    int f18;                    /* 0x18 */
    unsigned char pad1C[0x24 - 0x1C];
    unsigned short f24;         /* 0x24 */
} Seq;

typedef struct {
    int f00;                    /* 0x00 */
    int f04;                    /* 0x04 */
    int f08;                    /* 0x08 */
    unsigned char *p0C;         /* 0x0C */
    int f10;                    /* 0x10 */
    unsigned char *p14;         /* 0x14 */
} Song;

typedef struct {
    unsigned char pad00[0x28];
    unsigned long f28;          /* 0x28 */
} Tempo;

extern Voice D_0027CCC0[48];
extern Song D_00281AC0;
extern Tempo D_0027F740;

int func_001186B0(Seq *seq)
{
    int i;
    unsigned long bit;

    D_00281AC0.p0C[0xB] = D_00281AC0.p14[seq->f08 + 2];
    if (D_00281AC0.p14[seq->f08 + 2] == 0) {
        for (i = 0, bit = 1; i < 48; i++, bit <<= 1) {
            if (D_0027CCC0[i].f04 == (seq->f00 & 0xF) && D_0027CCC0[i].f22 == seq->f24 &&
                D_0027CCC0[i].f06 == seq->f18 && D_0027CCC0[i].f0C == 1) {
                if (D_00281AC0.p0C[0xB] == 0) {
                    D_0027F740.f28 |= bit;
                    D_0027CCC0[i].f08 = 1;
                } else {
                    D_0027CCC0[i].f18 = 1;
                }
            }
        }
    }
    return seq->f08 += 3;
}
