// NEARMISS func_00105088  (vram 0x00105088, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 0.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef short s16;
typedef unsigned char u8;

/* qword constant at D_00105130: eight halfword lanes, each 0x00FF. */
extern const s16 D_00105130[8];

void func_00105088(u8 *dst, const s16 *a, const s16 *b)
{
    int blk;
    int i;

    /* 0x18 iterations x 16 lanes = 384 samples. */
    for (blk = 0x18; blk != 0; blk--) {
        for (i = 0; i < 16; i++) {
            int v = a[i] + b[i];            /* paddh (wraps per 16-bit lane) */
            if (v > D_00105130[i & 7])      /* pminh against the 0x00FF quad */
                v = D_00105130[i & 7];
            if (v < 0)                      /* pmaxh against $zero           */
                v = 0;
            dst[i] = (u8)v;                 /* ppacb: low byte of each lane  */
        }
        dst += 0x10;                        /* 16 packed bytes out           */
        a   += 0x10;                        /* 16 halfwords (0x20 bytes) in  */
        b   += 0x10;
    }
}
