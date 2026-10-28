// NEARMISS func_0010A400  (vram 0x0010A400, 0xD4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.23% via ee-gcc 2.9-991111-01 (-O2), 2.96 gas. The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-27: the return value is a local (0x105) set once and both stored to the
// DMA channel control register and returned, so it lives in the return register (was 90.38%).
// Remaining differences: loop-preheader constant setup order only (the low half of the
// 0x0FFFFFFF mask is scheduled after the 3 / -1 constants instead of before them); every
// instruction of the loop and the register-write tail is otherwise identical.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// EEGCC_AS: 2.96
// (2.96 gas: matching wave 2026-10-27 sweep; it fills call/return delay slots as the original does)
// CFLAGS: -O2

extern unsigned char D_00276080;

int func_0010A400(int src, int count) {
    long *tag = (long *)((((int)&D_00276080) & 0x0FFFFFFF) | 0x20000000);
    int r = 0x105;
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
    *(volatile int *)0x1000B400 = r;
    return r;
}
