// COMPILER: eegcc
// CFLAGS: -O2

//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 41.76% NEARMISS).
// Fixes read from the original instructions:
//   - The original multiplies by 0x180 three separate times (once per buffer), so
//     the per-buffer offset was not one shared expression: it is an inline helper
//     applied to each base, which also takes the w*h product and divides it by 512.
//   - The uncached-accelerated address conversion is an inline helper as well
//     (writing it out at each store let CSE share the product and changed the
//     register allocation).
//
// SEMANTICS: build 9 DMA/GS "address tag" words from 3 base addresses.
//   Each output word is (addr & 0x0FFFFFFF) | 0x20000000 (uncached-accelerated
//   segment).  p0..p2 and p3..p5 get the three bases unchanged; p6..p8 get the
//   bases advanced by one framebuffer's worth of bytes,
//   (D_0024140C * D_00241410 / 512) * 0x180.
//   Note the EABI stack-argument layout: register args fill a0-a3/t0-t3 (p0..p7),
//   then each further arg occupies a full 8-byte stack slot, so p8/x/y/z land at
//   sp+0x50 / +0x58 / +0x60 / +0x68.
//

extern int D_0024140C;
extern int D_00241410;

static inline unsigned int uncached(unsigned int a)
{
    return (a & 0x0FFFFFFF) | 0x20000000;
}

static inline unsigned int bufaddr(unsigned int a, int size)
{
    return uncached(a + size / 512 * 0x180);
}

void func_0010A998(unsigned int *p0, unsigned int *p1, unsigned int *p2,
                   unsigned int *p3, unsigned int *p4, unsigned int *p5,
                   unsigned int *p6, unsigned int *p7, unsigned int *p8,
                   unsigned int x, unsigned int y, unsigned int z)
{
    int n = D_0024140C * D_00241410;

    *p0 = uncached(x);
    *p1 = uncached(y);
    *p2 = uncached(z);
    *p3 = uncached(x);
    *p4 = uncached(y);
    *p5 = uncached(z);
    *p6 = bufaddr(x, n);
    *p7 = bufaddr(y, n);
    *p8 = bufaddr(z, n);
}
