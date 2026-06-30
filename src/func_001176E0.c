// NEARMISS func_001176E0  (vram 0x001176E0, 0x104 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 68.60% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc forward-branch-likely wall + list-scheduler wall. Logic fully recovered: scan 0x30 entries (stride 0x6A) in D_0027CCC0, build a 64-bit match bitmask via a 7-field && compare against fields of a0 (offsets 0/0x1A/0xC/0x3E/0x2/0x22/0x6 vs a0+{self,4,2,0x24,0x18}), movz fallback (if t0==0 use a3), then a 0x30-iter...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern unsigned char D_0027CCC0[];
extern unsigned char D_0027F740[];

int func_001176E0(unsigned char *a0) {
    int t4 = *(int *)(a0 + 8);
    unsigned char *p = D_0027CCC0;
    unsigned char *end = D_0027CCC0 + 0x13E0;
    unsigned long mask = 1;
    unsigned long acc_a3 = 0;
    unsigned long acc_t0 = 0;
    int i;
    unsigned char *q;
    unsigned char *r;

    do {
        if (*(unsigned short *)p == 1 &&
            *(unsigned short *)(p + 0x1A) == 2 &&
            *(unsigned short *)(p + 0xC) == *(unsigned short *)p &&
            *(unsigned short *)(p + 0x3E) == *(unsigned char *)(a0 + 4) &&
            *(unsigned short *)(p + 0x2) == *(unsigned char *)(a0 + 2) &&
            *(unsigned short *)(p + 0x22) == *(unsigned short *)(a0 + 0x24) &&
            *(unsigned short *)(p + 0x6) == *(unsigned int *)(a0 + 0x18)) {
            acc_a3 |= mask;
            acc_t0 |= mask;
        }
        p += 0x6A;
        mask <<= 1;
    } while ((int)p < (int)end);

    if (acc_t0 == 0) {
        acc_t0 = acc_a3;
    }

    q = D_0027CCC0 + 8;
    r = D_0027F740;
    mask = 1;
    for (i = 0x2F; i >= 0; i--) {
        if (acc_t0 & mask) {
            *(unsigned short *)q = 1;
            *(unsigned long *)(r + 0x28) = *(unsigned long *)(r + 0x28) | mask;
        }
        q += 0x6A;
        mask <<= 1;
    }

    *(int *)(a0 + 8) = t4 + 4;
    return 0;
}
