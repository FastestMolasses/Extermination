// NEARMISS func_001186B0  (vram 0x001186B0, 0xDC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 79.67% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely wall (confirmed s84). Same weapon-table scan family as func_001177E8. Expected emits 4x bnel on the field-comparison chain plus bnez/bnezl on the D_00281AC0 flag checks; our ee-gcc emits non-annulling bnez/bne and extra increment instructions. Body structure matches (the aligned bnel line...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_00281AC0[];
extern unsigned char D_0027CCC0[];
extern unsigned char D_0027F740[];

int func_001186B0(unsigned char *arg0) {
    unsigned char *base = D_00281AC0;
    unsigned char *p0 = *(unsigned char **)(base + 0x14) + *(int *)(arg0 + 8);
    *(unsigned char *)(*(unsigned char **)(base + 0xC) + 0xB) = *(unsigned char *)(p0 + 2);

    {
        int a8 = *(int *)(arg0 + 8);
        unsigned char *p = *(unsigned char **)(base + 0x14) + a8;
        if (*(unsigned char *)(p + 2) != 0) {
            *(int *)(arg0 + 8) = a8 + 3;
            return a8 + 3;
        } else {
            unsigned char *e = D_0027CCC0 + 0x18;
            unsigned char *t = D_0027F740;
            long bit = 1;
            int i;
            for (i = 0x2F; i >= 0; i--, bit <<= 1) {
                unsigned short c;
                if (*(unsigned short *)(e - 0x14) != (*(unsigned char *)(arg0 + 0) & 0xF)) { e += 0x6A; continue; }
                if (*(unsigned short *)(e + 0xA) != *(unsigned short *)(arg0 + 0x24)) { e += 0x6A; continue; }
                if (*(unsigned short *)(e - 0x12) != *(int *)(arg0 + 0x18)) { e += 0x6A; continue; }
                c = *(unsigned short *)(e - 0xC);
                if (c != 1) { e += 0x6A; continue; }
                if (*(unsigned char *)(*(unsigned char **)(base + 0xC) + 0xB) != 0) {
                    *(unsigned short *)(e + 0) = c;
                } else {
                    *(long *)(t + 0x28) |= bit;
                    *(unsigned short *)(e - 0x10) = c;
                }
                e += 0x6A;
            }
            *(int *)(arg0 + 8) = a8 + 3;
            return a8 + 3;
        }
    }
}
