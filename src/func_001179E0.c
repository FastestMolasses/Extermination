// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 91.90% NEARMISS).
// Fixes read from the original instructions:
// - The absolute values are ternaries (`w = (w < 0) ? -w : w;`): the original
//   negates a copy of each value and re-truncates both before the compare.
// - The scale factor is ONE nested expression of the five func_00125F48 calls
//   shifted right by 27, not a chain of assignments through temporaries; the
//   temporaries changed ee-gcc's register priorities (the parameter and the
//   scale factor swapped saved registers).

//
// Fixed-point 2D extent helper. func_00125F48 is a 64-bit (long) two-operand
// math helper — every intermediate here is 64-bit, hence the dsra/dsrl shifts.
// Returns two packed shorts: (w << 16) | h.
//
// D_0027CCC0 must be typed as an array of the 0x6A-byte entry struct (not a
// byte array): that keeps the reloc base at the symbol itself with the field
// offsets in the load displacements, which is what the target does.

typedef struct Ent {            /* 0x6A bytes */
    unsigned char pad00[0x28];
    unsigned short f28;         /* 0x28 */
    unsigned short f2A;         /* 0x2A */
    unsigned short f2C;         /* 0x2C */
    unsigned short f2E;         /* 0x2E */
    unsigned short f30;         /* 0x30 */
    unsigned short f32;         /* 0x32 */
    unsigned short f34;         /* 0x34 */
    unsigned char pad36[0x14];  /* 0x36 */
    unsigned short f4A;         /* 0x4A */
    unsigned char pad4C[0x1E];  /* 0x4C */
} Ent;

extern long func_00125F48(long a, long b);

extern Ent D_0027CCC0[];
extern unsigned short D_0027F778;

int func_001179E0(int idx, unsigned char *p)
{
    long t;
    unsigned long k;
    unsigned short k2;
    short w, h;

    t = func_00125F48(func_00125F48(func_00125F48(D_0027CCC0[idx].f28, D_0027CCC0[idx].f34),
                                    func_00125F48(D_0027CCC0[idx].f2E, D_0027CCC0[idx].f2C)),
                      func_00125F48(D_0027CCC0[idx].f2A, D_0027CCC0[idx].f30)) >> 27;
    k = D_0027CCC0[idx].f32;

    w = (short)(func_00125F48(func_00125F48(t, (long)(k >> 8)),
                              *(int *)(p + 0x48)) >> 19);
    h = (short)(func_00125F48(func_00125F48(t, (long)(k & 0xFF)),
                              *(int *)(p + 0x4C)) >> 19);

    if (D_0027F778 == 1) {
        w = (w < 0) ? -w : w;
        h = (h < 0) ? -h : h;
        if (h < w)
            h = w;
        else
            w = h;
    }

    w = (short)((w & 0xFFFF) >> 1);
    h = (short)((h & 0xFFFF) >> 1);

    k2 = D_0027CCC0[idx].f4A;
    if (k2 != 0) {
        w = (short)((k2 << 8) | (w >> 7));
        h = (short)((k2 << 8) | (h >> 7));
    }

    return (w << 16) | h;
}
