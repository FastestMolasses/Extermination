// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 65.19% NEARMISS).
// Fixes read from the original instructions:
//  - Typed records as in the byte-matched siblings: a 0x78-byte Seq record array
//    for D_0027E0C0 (indexed directly) and the D_0027F740 Tempo struct with the
//    tempo halfword at 0x3A, instead of byte offsets and a split symbol.
//  - The +0x1C product reads the stored +0x54 field back (e->f54) rather than a
//    hand-masked copy of the argument; ee-gcc forwards the stored value itself.
//    The old "trailing nop" verdict was a measurement of the wrong body.
//
// Sets the tempo of sequence channel `ch` (< 48) when 0 <= tempo <= 0x3C0: stores
// it at +0x54 and recomputes +0x1C = (f56 * f54 << 12) / tempo word / 60.

typedef struct {
    unsigned char pad00[0x1C];
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

extern Seq D_0027E0C0[48];
extern Tempo D_0027F740;

void func_00119C98(unsigned int ch, int tempo)
{
    Seq *e = &D_0027E0C0[ch];

    if (ch < 48 && tempo >= 0 && tempo < 0x3C1) {
        e->f54 = tempo;
        e->f1C = ((e->f56 * e->f54) << 12) / D_0027F740.f3A / 60;
    }
}
