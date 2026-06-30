// NEARMISS func_00106948  (vram 0x00106948, 0x168 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 50.74% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the
// residual diff is a genuine compiler artifact that no source change fixes here:
// eegcc reg-alloc wall / saved-register-count divergence (confirmed s84). Instruction-for-instruction the LOGIC matches (DMA/0x10002010 busy-wait calling func_00109B70 up to 0x1389, then func_00106490 over D_00241324/B9C/170/B98), but expected uses 5 callee-saved regs (s0-s4, frame 0x60) keeping only the four global %...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_002412F4;
extern int D_00241324;
extern int D_00241B9C;
extern int D_00241B98;
extern int D_00241170[];

extern void func_00109B70(int);
extern int func_00106490(int);

int func_00106948(int a0) {
    int i;
    unsigned int v;
    int ret;

    if ((*(volatile unsigned int *)0x10002010 & 0x80004000) == 0x80000000) {
        i = 0;
        do {
            if (i >= 0x1389) {
                func_00109B70(D_002412F4);
                i = 0;
            }
            i++;
        } while ((*(volatile unsigned int *)0x10002010 & 0x80004000) == 0x80000000);
    }

    if (D_00241324 != 0 || D_00241B9C < a0) {
        *(volatile unsigned int *)0x10002000 = 0x40000000;
        D_00241324 = D_00241170[4];
        D_00241B98 = func_00106490(D_00241170[4]);
    }

    D_00241B9C = 0x20;
    v = (unsigned int)a0 | 0x40000000;
    *(volatile unsigned int *)0x10002000 = v;
    ret = (unsigned int)D_00241B98 >> (0x20 - a0);
    D_00241324 = D_00241170[v >> 28];
    D_00241B98 = func_00106490(D_00241170[v >> 28]);
    return ret;
}
