// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 70.00% NEARMISS).
// Fixes read from the original instructions:
//  - Rewritten in the spelling of the byte-matched sibling func_00119650: the
//    0x78-byte sequence record is a typed Seq (f08 cursor, p0C data, f1C, f54,
//    f56) and the tempo halfword is D_0027F740.f3A (the Tempo block) instead of
//    byte offsets and a bare D_0027F77A. The second divide then lands on the
//    EE's second divide pipe as in the original (no compiler wall).
//  - The cursor advance (+4) is the last statement, after the +0x1C store.
//
// Sequencer event: reads a little-endian halfword from payload bytes +2/+3 at the
// record's cursor into f54, recomputes f1C = (f56 * f54 << 12) / tempo / 60, and
// advances the cursor by 4.

typedef struct {
    unsigned char pad00[0x08];
    int f08;
    unsigned char *p0C;
    unsigned char pad10[0x1C - 0x10];
    int f1C;
    unsigned char pad20[0x54 - 0x20];
    unsigned short f54;
    unsigned short f56;
    unsigned char pad58[0x78 - 0x58];
} Seq;

typedef struct {
    unsigned char pad00[0x3A];
    unsigned short f3A;
} Tempo;

extern Tempo D_0027F740;

void func_00117CB0(Seq *seq)
{
    unsigned char *p = seq->p0C + seq->f08;

    seq->f54 = (p[3] << 8) | p[2];
    seq->f1C = ((seq->f56 * seq->f54) << 12) / D_0027F740.f3A / 60;
    seq->f08 += 4;
}
