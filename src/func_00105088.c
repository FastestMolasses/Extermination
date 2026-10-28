// NEARMISS func_00105088  (vram 0x00105088, 0x5C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 0.00% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff needs instructions that plain C cannot express:
// hand-written MMI assembly (an addi the compilers never emit, 128-bit loads, parallel
// halfword add/min/max and byte pack): no C compiler of this toolchain vectorises the
// scalar loop into MMI. Tried ee-gcc -O2: 0.00%. Only inline asm could match (not allowed).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

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
