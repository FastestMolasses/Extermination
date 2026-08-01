// NEARMISS func_0010A998  (vram 0x0010A998, 0xE8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 41.76% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc CSE wall + GPR coloring. Body fully recovered (prologue, both global loads, mult, the slt/addiu/movn/sra signed-div-by-512 idiom, all 9 masked-OR stores in target order, frame 0x50, args read with lw at sp+0x50/0x58/0x60/0x68 exactly as target). Residual: the target emits the product q*0x18...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// SEMANTICS: build 9 DMA/GS "address tag" words from 3 base addresses.
//   Each output word is (addr & 0x0FFFFFFF) | 0x20000000 (uncached-accelerated
//   segment).  p0..p2 and p3..p5 get the three bases unchanged; p6..p8 get the
//   bases advanced by one framebuffer's worth of bytes,
//   (D_0024140C * D_00241410 / 512) * 0x180.
//   Note the EABI stack-argument layout: register args fill a0-a3/t0-t3 (p0..p7),
//   then each further arg occupies a full 8-byte stack slot, so p8/x/y/z land at
//   sp+0x50 / +0x58 / +0x60 / +0x68 and are read with lw.
//

extern int D_0024140C;
extern int D_00241410;

void func_0010A998(unsigned int *p0, unsigned int *p1, unsigned int *p2,
                   unsigned int *p3, unsigned int *p4, unsigned int *p5,
                   unsigned int *p6, unsigned int *p7, unsigned int *p8,
                   unsigned int x, unsigned int y, unsigned int z)
{
    int n = D_0024140C * D_00241410 / 512;

    *p0 = (x & 0x0FFFFFFF) | 0x20000000;
    *p1 = (y & 0x0FFFFFFF) | 0x20000000;
    *p2 = (z & 0x0FFFFFFF) | 0x20000000;
    *p3 = (x & 0x0FFFFFFF) | 0x20000000;
    *p4 = (y & 0x0FFFFFFF) | 0x20000000;
    *p5 = (z & 0x0FFFFFFF) | 0x20000000;
    *p6 = ((x + n * 0x180) & 0x0FFFFFFF) | 0x20000000;
    *p7 = ((y + n * 0x180) & 0x0FFFFFFF) | 0x20000000;
    *p8 = ((z + n * 0x180) & 0x0FFFFFFF) | 0x20000000;
}
