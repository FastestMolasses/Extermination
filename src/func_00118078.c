// NEARMISS func_00118078  (vram 0x00118078, 0x134 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.88% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc GPR-coloring wall. Fully decompiled: a loop over the table at D_0027CCC0 (stride 0x6A, end +0x13E0) that, for entries matching state==1/2, a0+0x24, and the joint-data record at a0[8]+*(D_00281AC0+0x14) (a1+3/4/5), writes timer fields scaled by (b<<2)*(*(D_0027F740+0x3A))/60. Instruction stream, scheduling, the...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027CCC0[];
extern unsigned char D_00281AC0[];
extern unsigned char D_0027F740[];

void func_00118078(unsigned char *a0)
{
    unsigned char *t1 = a0;
    unsigned char *base = D_0027CCC0;
    int t6 = *(int *)(t1 + 8);
    int t5 = t6;
    unsigned char *s0 = D_00281AC0;
    unsigned char *t4 = D_0027F740;
    unsigned char *p = base;
    unsigned char *q = base + 0x62;
    unsigned char *end = base + 0x13E0;
    int one = 1, two = 2, sixty = 0x3C;

    do {
        unsigned short a3 = *(unsigned short *)(p + 0);
        if (a3 == one &&
            *(unsigned short *)(p + 0x1A) == two &&
            *(unsigned short *)(p + 0x22) == *(unsigned short *)(t1 + 0x24)) {
            unsigned char *a1 = (unsigned char *)(t5 + *(int *)(s0 + 0x14));
            if (*(unsigned short *)(p + 0x3E) == *(unsigned char *)(a1 + 4) &&
                *(unsigned short *)(p + 0x2) == *(unsigned char *)(a1 + 5) &&
                *(unsigned short *)(p + 0x6) == *(int *)(t1 + 0x18)) {
                if (*(unsigned short *)(q + 0) != 0) {
                    *(unsigned short *)(q - 0x14) = *(unsigned short *)(t1 + 0x58);
                }
                *(unsigned short *)(q - 0x1E) = a3;
                *(unsigned short *)(q - 0x2) = *(unsigned char *)(a1 + 3);
                *(unsigned short *)(q + 0) =
                    (unsigned short)(((*(unsigned char *)(a1 + 2) << 2) *
                        *(unsigned short *)(t4 + 0x3A)) / sixty);
                *(unsigned short *)(p + 0x64) =
                    (unsigned short)(((*(unsigned char *)(a1 + 2) << 2) *
                        *(unsigned short *)(t4 + 0x3A)) / sixty);
            }
        }
        q += 0x6A;
        p += 0x6A;
    } while (p < end);

    *(int *)(t1 + 8) = t6 + 6;
}
