// NEARMISS func_0010A400  (vram 0x0010A400, 0xD4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.17% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc volatile-store delay-slot-fill wall. The entire DMA-tag-build loop (slt/movz min, (chunk+15)/16 signed ceil-div, dsll32/dsll tag assembly, 16-byte tag stride, sd) is byte-identical, and after reordering id/qwc the hoisted const regs (t6=3,t5=-1) match, and the trailing register writes to 0x1000B430/B420/B400 m...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_00276080;

int func_0010A400(int src, int count) {
    long *tag = (long *)((((int)&D_00276080) & 0x0FFFFFFF) | 0x20000000);
    while (count > 0) {
        int chunk = (0xFFF40 < count) ? 0xFFF40 : count;
        int id;
        int qwc;
        count -= chunk;
        id = count ? 3 : 0;
        qwc = (chunk + 15) / 16;
        *tag = ((long)(src & 0x0FFFFFFF) << 32) |
               ((long)id << 28) |
               (long)(unsigned int)qwc;
        src += chunk;
        tag = (long *)((char *)tag + 0x10);
    }
    *(volatile int *)0x1000B430 = ((int)&D_00276080) & 0x0FFFFFFF;
    *(volatile int *)0x1000B420 = 0;
    *(volatile int *)0x1000B400 = 0x105;
    return 0x105;
}
