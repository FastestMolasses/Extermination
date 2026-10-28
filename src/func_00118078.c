// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 81.88% NEARMISS).
// Fixes read from the original instructions:
//  - Typed records indexed directly (Voice D_0027CCC0[48], Seq argument, Song
//    D_00281AC0, the D_0027F740 tempo word at 0x3A) in a for (i < 48) loop, as in
//    the byte-matched sibling func_00117EE8, instead of hand-strength-reduced
//    byte pointers; seq->f08 and the song pointer are read in each expression.
//  - The voice's f44 store is the constant 1 (the compiler reuses the loaded
//    f00 register because f00 == 1 was just tested).
//  - The scaled value is (byte << 2) * tempo / 60; writing `* 4` let the
//    compiler move the shift onto the tempo operand.
//
// Sequencer event with a 6-byte payload: for every voice with f00 == 1,
// f1A == 2 and f22 == seq->f24 whose key/channel bytes (payload +4 / +5) and
// f06 == seq->f18 match, copy seq->f58 to f4E when f62 is non-zero, set f44 = 1,
// f60 = payload[3], and set f62 and f64 to (payload[2] * 4) * tempo / 60, where
// tempo is the halfword at D_0027F740 + 0x3A. Then advance seq->f08 by 6.
typedef struct {                /* 0x6A */
    unsigned short f00;         /* 0x00 */
    unsigned short f02;         /* 0x02 */
    unsigned short f04;         /* 0x04 */
    unsigned short f06;         /* 0x06 */
    unsigned char pad08[0x1A - 0x08];
    unsigned short f1A;         /* 0x1A */
    unsigned char pad1C[0x22 - 0x1C];
    unsigned short f22;         /* 0x22 */
    unsigned char pad24[0x3E - 0x24];
    unsigned short f3E;         /* 0x3E */
    unsigned char pad40[0x44 - 0x40];
    unsigned short f44;         /* 0x44 */
    unsigned char pad46[0x4E - 0x46];
    unsigned short f4E;         /* 0x4E */
    unsigned char pad50[0x60 - 0x50];
    unsigned short f60;         /* 0x60 */
    unsigned short f62;         /* 0x62 */
    unsigned short f64;         /* 0x64 */
    unsigned char pad66[0x6A - 0x66];
} Voice;

typedef struct {
    unsigned char f00;          /* 0x00 */
    unsigned char pad01[0x08 - 0x01];
    int f08;                    /* 0x08 */
    unsigned char pad0C[0x18 - 0x0C];
    int f18;                    /* 0x18 */
    unsigned char pad1C[0x24 - 0x1C];
    unsigned short f24;         /* 0x24 */
    unsigned char pad26[0x58 - 0x26];
    unsigned short f58;         /* 0x58 */
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
    unsigned char pad00[0x3A];
    unsigned short f3A;         /* 0x3A */
} Tempo;

extern Voice D_0027CCC0[48];
extern Song D_00281AC0;
extern Tempo D_0027F740;

void func_00118078(Seq *seq)
{
    int i;

    for (i = 0; i < 48; i++) {
        if (D_0027CCC0[i].f00 == 1 && D_0027CCC0[i].f1A == 2 && D_0027CCC0[i].f22 == seq->f24 &&
            D_0027CCC0[i].f3E == D_00281AC0.p14[seq->f08 + 4] &&
            D_0027CCC0[i].f02 == D_00281AC0.p14[seq->f08 + 5] && D_0027CCC0[i].f06 == seq->f18) {
            if (D_0027CCC0[i].f62 != 0)
                D_0027CCC0[i].f4E = seq->f58;
            D_0027CCC0[i].f44 = 1;
            D_0027CCC0[i].f60 = D_00281AC0.p14[seq->f08 + 3];
            D_0027CCC0[i].f62 = (D_00281AC0.p14[seq->f08 + 2] << 2) * D_0027F740.f3A / 60;
            D_0027CCC0[i].f64 = (D_00281AC0.p14[seq->f08 + 2] << 2) * D_0027F740.f3A / 60;
        }
    }
    seq->f08 += 6;
}
