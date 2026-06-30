// NEARMISS func_001021B0  (vram 0x001021B0, 0xC4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.98% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// Frame-size stride wall: correct readable C (DMA/IRQ spin-wait; loop body, srl, li constants all match) but ee-gcc hoists the ~0x100 mask into a 4th callee-saved reg -> frame 0x50 vs expected 0x40 (3 saved regs). Original kept a dead inner spin sub-loop that this ee-gcc optimizer always eliminates. 480s seeded permut...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern void func_00122B58(void *);
extern unsigned char D_0026B220;
extern unsigned char D_FFFFFF;

void func_001021B0(volatile int *p)
{
    int i = (int)&D_FFFFFF;

    if (*p & 0x100) {
        do {
            if (i < 0) {
                func_00122B58(&D_0026B220);
                if (((unsigned int)*p >> 8) & 1) {
                    *p = *p & -0x101;
                }
            }
            i -= 1;
        } while (*p & 0x100);
    }

    p[8] = 0;
    *p = (((*p & -0xD) | 0x4) & -0x2) | 0x100;
}
