// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 79.62% NEARMISS).
// Fixes read from the original instructions:
//  - Typed records indexed directly (a 0x78-byte Seq record array for D_0027E0C0,
//    Song D_00281AC0 with the header pointer at +0x14, the D_0027F740 tempo word
//    at 0x3A, D_0027C6C0 as 12-byte rows) instead of byte offsets; the Song and
//    tempo addresses are then materialised after the gate, as in the original.
//  - The format pair at header+2 is read through a pointer assigned in the loop
//    body (loop-hoisted, so the second halfword goes through it).
//  - Byte copies in the order 0, 2, 3, then 1 from the held first byte; the two
//    0x1000 words are one chained assignment (+0x4C stored first).
//  - ret = i follows the slot-index store (that store uses the loop counter).
//
// Claims a free channel for sound bank `id` (< 0x80) with header `hdr`: requires
// the 'SSsq' magic word (0x71735353) at hdr+0xC and state 1 in the 12-byte
// D_0027C6C0 row for id. The first of the 48 records with +0x2E, +0x30 and +0x34
// all zero is initialised: Song pointer = hdr, active = 1, bank id, offset 0x110,
// base hdr, slot index, bytes from hdr+0x110..0x112 (the first duplicated into
// byte 1), two 0x1000 volumes, the two format halfwords from hdr+2, and +0x1C =
// (f56 * f54 << 12) / tempo / 60. Returns the slot index, or -1.

typedef struct {
    unsigned char f00;
    unsigned char f01;
    unsigned char f02;
    unsigned char f03;
    unsigned char pad04[0x08 - 0x04];
    int f08;
    unsigned char *p0C;
    unsigned char pad10[0x18 - 0x10];
    int f18;
    int f1C;
    unsigned char pad20[0x24 - 0x20];
    unsigned short f24;
    unsigned char pad26[0x2E - 0x26];
    unsigned short f2E;
    int f30;
    unsigned short f34;
    unsigned char pad36[0x48 - 0x36];
    int f48;
    int f4C;
    unsigned char pad50[0x54 - 0x50];
    unsigned short f54;
    unsigned short f56;
    unsigned char pad58[0x78 - 0x58];
} Seq;

typedef struct {
    int f00;
    int f04;
    int f08;
    unsigned char *p0C;
    int f10;
    unsigned char *p14;
} Song;

typedef struct {
    unsigned char pad00[0x3A];
    unsigned short f3A;
} Tempo;

extern Seq D_0027E0C0[48];
extern int D_0027C6C0[][3];
extern Song D_00281AC0;
extern Tempo D_0027F740;

int func_00119650(int id, unsigned char *hdr)
{
    Seq *e = D_0027E0C0;
    int ret = -1;
    int i;
    unsigned char b;
    unsigned short *fmt;

    if ((unsigned int)id < 0x80 && *(int *)(hdr + 0xC) == 0x71735353 &&
        D_0027C6C0[id][0] == 1) {
        for (i = 0; i < 48; i++, e++) {
            if (e->f2E != 0) continue;
            if (e->f30 != 0) continue;
            if (e->f34 != 0) continue;
            fmt = (unsigned short *)(hdr + 2);
            D_00281AC0.p14 = hdr;
            e->f2E = 1;
            e->f24 = id;
            e->f08 = 0x110;
            e->p0C = hdr;
            e->f18 = i;
            ret = i;
            b = hdr[0x110];
            e->f00 = b;
            e->f02 = hdr[0x111];
            e->f03 = hdr[0x112];
            e->f01 = b;
            e->f4C = e->f48 = 0x1000;
            e->f56 = fmt[0];
            e->f54 = fmt[1];
            e->f1C = ((e->f56 * e->f54) << 12) / D_0027F740.f3A / 60;
            break;
        }
    }
    return ret;
}
