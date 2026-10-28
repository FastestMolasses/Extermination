// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 89.56% NEARMISS).
// Fixes read from the original instructions:
//  - The 0x04 field of D_00281AC0 is a pointer to 16-byte records, advanced and
//    rewound by a1 records with `+=` / `-=` (not a pre-shifted byte offset local).
//  - The first table lookup goes into its own byte local (v1) and the second
//    lookup indexes with it, so the two lookups colour the original registers.
//
// Temporarily advances the record cursor of D_00281AC0 by a1 records and reads
// the byte at offset 0x0C of the selected record. For mode 4 that byte is the
// result; otherwise it is translated twice through the 32-column table
// D_00242230: first with the column taken from the record byte and the row from
// byte 2 of the object at field 0x00, then with the column from that result and
// the row from byte 4 of the object at field 0x0C. The cursor is restored before
// returning.

extern unsigned char D_00242230[];

struct rec {
    int x[3];          /* 0x00 */
    unsigned char c;   /* 0x0C */
    unsigned char pad[3];
};

struct g {
    unsigned char *p0; /* 0x00 */
    struct rec *p4;    /* 0x04 */
    int pad8;          /* 0x08 */
    unsigned char *pC; /* 0x0C */
};
extern struct g D_00281AC0;

unsigned char func_00117BA0(int a0, int a1) {
    unsigned char v0;
    unsigned char v1;

    D_00281AC0.p4 += a1;
    if (a0 == 4) {
        v0 = D_00281AC0.p4->c;
    } else {
        v1 = D_00242230[(D_00281AC0.p4->c >> 2) + ((D_00281AC0.p0[2] >> 2) << 5)];
        v0 = D_00242230[(v1 >> 2) + ((D_00281AC0.pC[4] >> 2) << 5)];
    }
    D_00281AC0.p4 -= a1;
    return v0;
}
