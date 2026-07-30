// NEARMISS func_001179E0  (vram 0x001179E0, 0x1C0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.90% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall (+ minor list-scheduler). Body/structure fully recovered: all 8 func_00125F48 calls, both 64-bit dsra/dsrl chains, the D_0027F778==1 abs/max block (incl. the annulled `beql`), the >>1 truncation and the f4A pack are all present and in the right order. Residual is a systema...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

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
    long t, u;
    unsigned long k;
    unsigned short k2;
    short w, h;

    t = func_00125F48(D_0027CCC0[idx].f28, D_0027CCC0[idx].f34);
    u = func_00125F48(D_0027CCC0[idx].f2E, D_0027CCC0[idx].f2C);
    t = func_00125F48(t, u);
    u = func_00125F48(D_0027CCC0[idx].f2A, D_0027CCC0[idx].f30);
    t = func_00125F48(t, u);
    t = t >> 27;
    k = D_0027CCC0[idx].f32;

    w = (short)(func_00125F48(func_00125F48(t, (long)(k >> 8)),
                              *(int *)(p + 0x48)) >> 19);
    h = (short)(func_00125F48(func_00125F48(t, (long)(k & 0xFF)),
                              *(int *)(p + 0x4C)) >> 19);

    if (D_0027F778 == 1) {
        if (w < 0)
            w = -w;
        if (h < 0)
            h = -h;
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
