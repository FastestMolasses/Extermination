// NEARMISS func_001020C8  (vram 0x001020C8, 0xE4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 62.98% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc dead-single-iteration-loop shape + regalloc wall: body decoded correctly (flag-poll loop calling func_00122B58, conditional store via forward bnel reproduced, final *p=(*p&~0xD)|0x109), but expected emits a one-iteration dead loop (bnez $v0 back-edge with $v0 const-0) that clean C collapses to straight-line; n...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_FFFFFF;            /* absolute symbol = 0x00FFFFFF */
extern unsigned char D_0026B220[];

extern void func_00122B58(unsigned char *);

void func_001020C8(int *p, int a1, int a2) {
    int i;

    if (*p & 0x100) {
        i = (int)&D_FFFFFF;
        do {
            if (i < 0) {
                func_00122B58(D_0026B220);
                if ((*p >> 8) & 1) {
                    *p &= ~0x100;
                }
            }
            i--;
        } while (*p & 0x100);
    }

    if (*(int *)((char *)p + 0x10) != -1) {
        *(int *)((char *)p + 0x10) = a1;
    }
    *(int *)((char *)p + 0x20) = a2;
    *p = (*p & ~0xD) | 0x109;
}
