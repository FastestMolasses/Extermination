// NEARMISS func_0011A4E8  (vram 0x0011A4E8, 0xE0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.55% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring + tail list-scheduler wall (fast-park class). .text size 224 == expected 224 and the ENTIRE loop (dsllv/nor preheader, beqzl + annulled counter decrement, forward 'beq mask,1' / 'beq mask,2' switch dispatch with the speculative 'and' in the delay slot, cross-jumped 'sd', 'bgez;...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// NEARMISS 82.55% - body/structure correct (loop + switch dispatch are byte-identical);
// residual is ee-gcc GPR coloring + tail list-scheduler ordering in the argument-packing
// block (identical instruction set, different register assignment / emission order).
extern long D_0027F740;
extern void func_001157F0(int a0, unsigned int a1, unsigned int a2, unsigned int a3);

int func_0011A4E8(unsigned int *p) {
    unsigned int r1;
    long bit = (long)1 << p[0];
    unsigned int mask = 1;
    int i;

    for (i = 15; i >= 0; i--, mask <<= 1) {
        if ((p[1] & mask) != 0) {
            switch (mask) {
            case 1:
                D_0027F740 = D_0027F740 | bit;
                break;
            case 2:
                D_0027F740 = D_0027F740 & ~bit;
                break;
            }
        }
    }

    r1 = (p[0] << 24) | (p[1] & 0xFF0000);
    r1 = r1 | (p[5] & 0xFF00);
    r1 = r1 | ((p[4] & 0xFF0000) >> 16);
    func_001157F0(0x3E, r1,
                  (p[4] << 16) | ((p[3] >> 8) & 0xFFFF),
                  (p[3] << 24) | (p[2] & 0xFFFFFF));
    return 0;
}
