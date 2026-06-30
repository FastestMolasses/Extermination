// NEARMISS func_00101CD0  (vram 0x00101CD0, 0x1D8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.44% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// MMIO config writer: validation branches, the 0xe000 bit-twiddle (D_002410B8/C8/D8 table lookups, masks 0xCF/0x3F/0xF3, b3 branch), all five MMIO reg writes (0x1000E000/E020/E030/E040/E050), and the unaligned struct copy to D_002410E8 all match structurally (improved 46->66->69% by reading all 5 volatile MMIO regs up...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_002410B8;
extern unsigned char D_002410C8;
extern unsigned char D_002410D8;
extern unsigned char D_002410E8;

int func_00101CD0(unsigned char *p)
{
    unsigned int reg00 = *(volatile unsigned int *)0x1000E000;
    unsigned int reg20 = *(volatile unsigned int *)0x1000E020;
    unsigned int reg30 = *(volatile unsigned int *)0x1000E030;
    unsigned int reg40 = *(volatile unsigned int *)0x1000E040;
    unsigned int reg50 = *(volatile unsigned int *)0x1000E050;
    unsigned int b3;

    if ((unsigned int)p[0] >= 10) return -1;
    if ((unsigned int)p[1] >= 10) return -2;
    if ((unsigned int)p[2] >= 10) return -3;
    b3 = p[3];
    if (b3 >= 7) return -4;

    reg00 = (reg00 & 0xFFFFFFCF) | ((unsigned int)(&D_002410B8)[p[0]] << 4);
    reg00 = (reg00 & 0xFFFFFF3F) | ((unsigned int)(&D_002410C8)[p[1]] << 6);
    reg00 = (reg00 & 0xFFFFFFF3) | ((unsigned int)(&D_002410D8)[p[2]] << 2);

    if (b3 != 0) {
        reg00 = ((reg00 | 2) & 0xFFFFFCFF) | ((p[3] - 1) << 8);
    } else {
        reg00 = reg00 & 0xFFFFFFFD;
    }

    *(volatile unsigned int *)0x1000E000 = reg00;
    *(volatile unsigned int *)0x1000E020 =
        ((unsigned int)*(unsigned short *)(p + 4) << 16) | *(unsigned short *)(p + 6);
    *(volatile unsigned int *)0x1000E030 =
        ((unsigned int)*(unsigned short *)(p + 0xA) << 16) | *(unsigned short *)(p + 8);
    *(volatile unsigned int *)0x1000E050 = *(unsigned int *)(p + 0xC);
    *(volatile unsigned int *)0x1000E040 = *(unsigned int *)(p + 0x10);

    {
        unsigned char *dst = &D_002410E8;
        __builtin_memcpy(dst, p, 16);
        *(unsigned int *)(dst + 16) = *(unsigned int *)(p + 16);
    }
    (void)reg20; (void)reg30; (void)reg40; (void)reg50;
    return 0;
}
